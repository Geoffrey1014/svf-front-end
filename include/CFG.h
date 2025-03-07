#ifndef CFG_BUILDER_H
#define CFG_BUILDER_H

#include "LlBuilder.h"
#include "BasicBlock.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <sstream>
#include <fstream>
#include <algorithm>


// Control Flow Graph class
class CFG {
private:
    BasicBlock* entry;
    BasicBlock* exit;
    std::unordered_map<std::string, BasicBlock*> blocks;
    std::vector<BasicBlock*> blocksList;

public:
    CFG() : entry(nullptr), exit(nullptr) {}
    
    ~CFG() {
        for (auto& pair : blocks) {
            delete pair.second;
        }
    }

    void setEntry(BasicBlock* block) {
        entry = block;
    }

    void setExit(BasicBlock* block) {
        exit = block;
    }

    void addBlock(BasicBlock* block) {
        blocks[block->getLabel()] = block;
        blocksList.push_back(block);
    }

    const std::vector<BasicBlock*>& getBlocksList() const {
        return blocksList;
    }   

    BasicBlock* getBlock(const std::string& label) {
        auto it = blocks.find(label);
        if (it != blocks.end()) {
            return it->second;
        }
        return nullptr;
    }

    BasicBlock* getEntry() {
        return entry;
    }

    BasicBlock* getExit() {
        return exit;
    }

    const std::unordered_map<std::string, BasicBlock*>& getBlocks() const {
        return blocks;
    }

    std::string toString() {
        std::stringstream ss;
        ss << "Control Flow Graph:\n";
        
        for (const auto& pair : blocks) {
            BasicBlock* block = pair.second;
            ss << "-----------------------------------\n";
            ss << "Block " << block->getLabel() << ":\n";
            
            ss << "  Instructions:\n ";
            for (const auto& inst : block->getLlStatements()) {
                ss << inst->toString() << std::endl;
            }
            ss << "\n";
            
            ss << "  Predecessors: ";
            for (const auto* pred : block->getPredecessors()) {
                ss << pred->getLabel() << " ";
            }
            ss << "\n";
            
            ss << "  Successors: ";
            for (const auto* succ : block->getSuccessors()) {
                ss << succ->getLabel() << " ";
            }
            ss << "\n-----------------------------------\n\n";
        }
        
        return ss.str();
    }

    // generate a dot file for the CFG
    std::string generateDotFile() const {
        std::stringstream dot;
        dot << "digraph CFG {\n";
        dot << "    node [shape=box];\n\n";
        
        // Add nodes (basic blocks)
        for (const auto& pair : blocks) {
            BasicBlock* block = pair.second;
            dot << "    \"" << block->getLabel() << "\" [label=\"" << block->getLabel() << "\\n";
            
            // Add instructions to node label
            for (const auto& stmt : block->getLlStatements()) {
                std::string stmtStr = stmt->toString();
                // Escape special characters for DOT format
                std::replace(stmtStr.begin(), stmtStr.end(), '"', '\'');
                std::replace(stmtStr.begin(), stmtStr.end(), '\n', ' ');
                dot << stmtStr << "\\n";
            }
            dot << "\"];\n";
        }
        
        dot << "\n";
        
        // Add edges (control flow)
        for (const auto& pair : blocks) {
            BasicBlock* block = pair.second;
            for (const auto* succ : block->getSuccessors()) {
                dot << "    \"" << block->getLabel() << "\" -> \"" << succ->getLabel() << "\";\n";
            }
        }
        
        dot << "}\n";
        return dot.str();
    }

    // Write the DOT representation to a file
    void writeDotFile(const std::string& filename) const {
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << generateDotFile();
            outFile.close();
        }
    }
};

// CFGBuilder class to construct a CFG from an LlBuilder
class CFGBuilder {
private:
    // Helper function to identify leaders (first instruction of a basic block)
    std::unordered_set<std::string> identifyLeaders(LlBuilder& builder) {
        std::unordered_set<std::string> leaders;
        const auto& statementTable = builder.getStatementTable();
        const auto& insertionOrder = builder.getInsertionOrder(); 

        // First instruction is always a leader
        if (!insertionOrder.empty()) {
            leaders.insert(insertionOrder[0]);
        }
        
        // Targets of jumps and instructions following jumps are leaders
        for (size_t i = 0; i < insertionOrder.size(); i++) {
            const std::string& label = insertionOrder[i];
            LlStatement* stmt = statementTable.at(label);
            
            // Check if this is a jump instruction
            if (stmt->isJump()) {
                // If there's a target label, it's a leader
                LlJump* jumpStmt = dynamic_cast<LlJump*>(stmt);
                std::string* targetLabel = jumpStmt->getJumpToLabel();
                if (targetLabel) {
                    leaders.insert(*targetLabel);
                }
                
                // Instruction after a jump is a leader (if it exists)
                if (i + 1 < insertionOrder.size()) {
                    leaders.insert(insertionOrder[i + 1]);
                }
            }
        }
        
        return leaders;
    }

public:
    CFG* buildCFG(LlBuilder& builder) {
        CFG* cfg = new CFG();
        
        // Identify leaders (first instruction of each basic block)
        std::unordered_set<std::string> leaders = identifyLeaders(builder);
        
        // Create basic blocks for each leader and add instructions to them
        BasicBlock* currentBlock = nullptr;
        for (const std::string& label : builder.getInsertionOrder()) {
            LlStatement* stmt = builder.getStatementTable().at(label);
            if (leaders.find(label) != leaders.end()) {
                currentBlock = new BasicBlock("BB_" + label);
                cfg->addBlock(currentBlock);
            }
            currentBlock->addLlStatement(stmt);
        }

        // connect the blocks 
        std::vector<BasicBlock*> blocksList = cfg->getBlocksList();
        for (int i = 0; i < blocksList.size(); i++) {
            BasicBlock* block = blocksList[i];
            LlStatement* lastStmt = block->getLlStatements().back();
            if (lastStmt->isJump()) {
                LlJump* jumpStmt = dynamic_cast<LlJump*>(lastStmt);
                std::string* targetLabel = jumpStmt->getJumpToLabel();
                if (targetLabel) {
                    block->addSuccessor(cfg->getBlock("BB_" + *targetLabel));
                    cfg->getBlock("BB_" + *targetLabel)->addPredecessor(block);
                }
                if (jumpStmt->isConditionalJump() && i + 1 < blocksList.size()){
                    // add the next block as a successor
                    block->addSuccessor(blocksList[i+1]);
                    blocksList[i+1]->addPredecessor(block);
                }
            }
            else if (i + 1 < blocksList.size()) {
                block->addSuccessor(blocksList[i+1]);
                blocksList[i+1]->addPredecessor(block);
            }
        }

        // Set entry and exit blocks
        if (!blocksList.empty()) {
            cfg->setEntry(blocksList[0]);

            // Find blocks with no successors and connect them to an exit block
            BasicBlock* exitBlock = new BasicBlock("EXIT");
            cfg->addBlock(exitBlock);
            cfg->setExit(exitBlock);

            for (BasicBlock* const block: cfg->getBlocksList()) {
                if (block != exitBlock && block->getSuccessors().empty()) {
                    block->addSuccessor(exitBlock);
                    exitBlock->addPredecessor(block);
                }
            }
        }

        return cfg;
    }
};

// SSA Generator class to convert CFG to SSA form
class SSAGenerator {
private:
    std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>> dominanceFrontier;
    std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>> dom;
    std::unordered_map<std::string, int> variableVersions;
    std::unordered_map<std::string, std::stack<int>> variableStack;

public:
    const auto& getDom() const { return dom; }

    // Compute dominance tree using Cooper, Harvey, Kennedy algorithm
    void computeDominators(CFG* cfg) {
        BasicBlock* entry = cfg->getEntry();
        const auto& blocks = cfg->getBlocksList();

        // Initialize dominators
        for (BasicBlock* block : blocks) {
            std::unordered_set<BasicBlock*> domSet(blocks.begin(), blocks.end());
            dom[block] = domSet;
        }

        dom[entry].clear();
        dom[entry].insert(entry);

        bool changed = true;
        while (changed) {
            changed = false;
            for (BasicBlock* block : blocks) {
                if (block == entry) continue;

                auto oldDom = dom[block];
                std::unordered_set<BasicBlock*> new_dom (blocks.begin(), blocks.end());

                for (BasicBlock* pred : block->getPredecessors()) {
                    std::unordered_set<BasicBlock*> temp;
                    const auto& pred_dom = dom[pred];
                    // Compute intersection of new_dom and pred_dom
                    std::copy_if(new_dom.begin(), new_dom.end(),
                                std::inserter(temp, temp.begin()),
                                [&pred_dom](BasicBlock* d) { return pred_dom.count(d) > 0; });
                    for (BasicBlock* d : new_dom) {
                        if (pred_dom.find(d) != pred_dom.end()) {
                            temp.insert(d);
                        }
                    }
                    new_dom = temp;
                }
                new_dom.insert(block);

                if (oldDom.size() != new_dom.size() ||
                    !std::all_of(new_dom.begin(), new_dom.end(),
                               [&oldDom](BasicBlock* b) { return oldDom.find(b) != oldDom.end(); })) {
                    dom[block] = new_dom;
                    changed = true;
                }
            }
        }
    }


    // Compute dominance frontier
    void computeDominanceFrontier(CFG* cfg) {
        for (BasicBlock* block : cfg->getBlocksList()) {
            dominanceFrontier[block] = std::unordered_set<BasicBlock*>();
        }

        for (BasicBlock* block : cfg->getBlocksList()) {
            if (block->getPredecessors().size() >= 2) {
                for (BasicBlock* pred : block->getPredecessors()) {
                    BasicBlock* runner = pred;
                    const auto& idom = dom[block];
                    while (idom.find(runner) == idom.end() && runner != nullptr) {
                        dominanceFrontier[runner].insert(block);
                        if (!dom[runner].empty()) {
                            runner = *dom[runner].begin(); // Take any dominator
                        } else {
                            runner = nullptr;
                        }
                    }
                }
            }
        }
    }

    // Insert Phi functions
    void insertPhiFunctions(CFG* cfg) {
        // 0   worklist = ∅
        // 1   for each node n do
        // 2   {
        // 3       inserted_n = x0         /* x0 should not occur in the program */
        // 4       inWorklist_n = x0
        // 5   }

        // 6   for each variable x do
        // 7       for each n ∈ assign(x) do
        // 8       {
        // 9           inWorklist_n = x
        // 10          worklist = worklist ∪ {n}
        // 11      }

        // 12  while worklist ≠ ∅ do
        // 13  {
        // 14      remove a node n from worklist
        // 15      for each m ∈ dfn do
        // 16          if inserted_m ≠ x then
        // 17          {
        // 18              place a Φ-instruction for x at m
        // 19              inserted_m = x
        // 20              if inWorklist_m ≠ x then
        // 21              {
        // 22                  inWorklist_m = x
        // 23                  worklist = worklist ∪ {m}
        // 24              }
        // 25          }
        // 26      }
        // 27  }

        std::unordered_set<BasicBlock*> worklist;
        std::unordered_map<BasicBlock*, std::string> inserted;
        std::unordered_map<BasicBlock*, std::string> inWorklist;

        for (BasicBlock* block : cfg->getBlocksList()) {
            inserted[block] = "";
            inWorklist[block] = "";
        }

        std::unordered_set<std::string> variables;
        std::unordered_map<std::string, std::unordered_set<BasicBlock*>> variableBlocks;

        // get all variables in the program
        for (BasicBlock* block : cfg->getBlocksList()) {
            for (LlStatement* stmt : block->getLlStatements()) {
                std::string* def = stmt->getDefinedVariable();
                // def is not start with #, which means it is a variable
                if (def && def->at(0) != '#') {
                    variables.insert(*def);
                    variableBlocks[*def].insert(block);
                }
            }
        }

        for (const auto& var : variables) {
            for (BasicBlock* block : variableBlocks[var]) {
                inWorklist[block] = var;
                worklist.insert(block);
            }
            // while worklist is not empty
            while (!worklist.empty()) {
                BasicBlock* block = *worklist.begin();
                worklist.erase(worklist.begin());

                // for each block in the dominance frontier of block
                for (BasicBlock* dfBlock : dominanceFrontier[block]) {
                    if (inserted[dfBlock] != var) {
                        inserted[dfBlock] = var;
                        // place a phi function for var at dfBlock
                        LlPhiStatement* phi = new LlPhiStatement(var);
                        
                        // Add the phi function to the beginning of the block
                        dfBlock->getLlStatements().insert(dfBlock->getLlStatements().begin(), phi);

                        if (inWorklist[dfBlock] != var) {
                            inWorklist[dfBlock] = var;
                            worklist.insert(dfBlock);
                        }
                    }
                }
            }
        }


    }

    // Rename variables
    void renameVariables(CFG* cfg) {
        // Initialize stacks for each variable
        for (BasicBlock* block : cfg->getBlocksList()) {
            for (LlStatement* stmt : block->getLlStatements()) {
                std::string* def = stmt->getDefinedVariable();
                if (def && def->at(0) != '#') {
                    variableStack[*def] = std::stack<int>();
                    variableStack[*def].push(0);
                }
            }
        }

        // Start renaming from entry block
        renameVariablesInBlock(cfg->getEntry());
    }

    void renameVariablesInBlock(BasicBlock* block) {

        // For each phi function in block
        for (LlStatement* stmt : block->getLlStatements()) {
            if (auto phi = dynamic_cast<LlPhiStatement*>(stmt)) {
                std::string* def = phi->getDefinedVariable();
                if (def && def->at(0) != '#') {
                    int newVersion = variableVersions[*def]++;
                    variableStack[*def].push(newVersion);
                    phi->renameDef(*def, *def + "_" + std::to_string(newVersion));
                }
            }
        }

        // For each regular statement in block
        for (LlStatement* stmt : block->getLlStatements()) {
            if (dynamic_cast<LlPhiStatement*>(stmt) == nullptr) {
                // Rename uses
                for (std::string* use : stmt->getUsedVariables()) {
                    if (!variableStack[*use].empty()) {
                        stmt->renameUse(*use, *use + "_" + std::to_string(variableStack[*use].top()));
                    }
                }
                
                // Rename definition
                std::string* def = stmt->getDefinedVariable();
                if (def && def->at(0) != '#') {
                    int newVersion = variableVersions[*def]++;
                    variableStack[*def].push(newVersion);
                    stmt->renameDef(*def, *def + "_" + std::to_string(newVersion));
                }
            }
        }

        // For each successor block
        for (BasicBlock* succ : block->getSuccessors()) {
            // Fill in phi function parameters in successor
            for (LlStatement* stmt : succ->getLlStatements()) {
                if (auto phi = dynamic_cast<LlPhiStatement*>(stmt)) {
                    std::string* phiVar = phi->getDefinedVariable();
                    if (phiVar && !variableStack[*phiVar].empty()) {
                        // Set the incoming value from this predecessor
                        phi->setIncoming(
                            new std::string(*phiVar + "_" + std::to_string(variableStack[*phiVar].top())),
                            block
                        );
                    }
                }
            }
        }

        // Recursively rename in dominator tree children
        for (BasicBlock* succ : block->getSuccessors()) {
            if (dom[succ].count(block) > 0) {  // if block dominates succ
                renameVariablesInBlock(succ);
            }
        }

        // Restore stacks by popping variables defined in this block
        for (LlStatement* stmt : block->getLlStatements()) {
            std::string* def = stmt->getDefinedVariable();
            if (def && def->at(0) != '#' && !variableStack[*def].empty()) {
                variableStack[*def].pop();
            }
        }
    }


    void convertToSSA(CFG* cfg) {
        // Step 1: Compute dominators
        computeDominators(cfg);

        // output the dominance tree
        std::cout << "\nDominance Tree:" << std::endl;
        for (const auto& pair : dom) {
            std::cout << pair.first->getLabel() << " is dominated by: ";
            for (const auto& dom : pair.second) {
                std::cout << dom->getLabel() << " ";
            }
            std::cout << std::endl;
        }

        // Step 2: Compute dominance frontier
        computeDominanceFrontier(cfg);

        // output the dominance frontier
        std::cout << "\nDominance Frontier:" << std::endl;
        for (const auto& pair : dominanceFrontier) {
            std::cout << pair.first->getLabel() << " -> ";
            for (const auto &dfBlock: pair.second) {
                std::cout << dfBlock->getLabel() << " ";
            }
            std::cout << std::endl;
        }

        // Step 3: Insert phi functions
        insertPhiFunctions(cfg);


        // Step 4: Rename variables
        renameVariables(cfg);

        // output the CFG after inserting phi functions
        std::cout << "\nCFG after inserting phi functions:" << std::endl;
        cfg->writeDotFile("cfg_phi.dot");
    }
};

#endif