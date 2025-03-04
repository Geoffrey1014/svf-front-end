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
        cfg->setEntry(new BasicBlock("BB_entry"));
        cfg->setExit(new BasicBlock("BB_exit"));
        
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
                if (jumpStmt->isConditionalJump()){
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
    std::unordered_map<BasicBlock*, BasicBlock*> immediateDominator;
    std::unordered_map<std::string, int> variableVersions;
    std::unordered_map<std::string, std::stack<int>> variableStack;

    // Compute dominance tree using Cooper, Harvey, Kennedy algorithm
    void computeDominators(CFG* cfg) {
        BasicBlock* entry = cfg->getEntry();
        const auto& blocks = cfg->getBlocksList();
        
        // Initialize dominators
        for (BasicBlock* block : blocks) {
            immediateDominator[block] = nullptr;
        }
        immediateDominator[entry] = entry;

        bool changed = true;
        while (changed) {
            changed = false;
            for (BasicBlock* block : blocks) {
                if (block == entry) continue;

                // Find first processed predecessor
                BasicBlock* newIdom = nullptr;
                for (BasicBlock* pred : block->getPredecessors()) {
                    if (immediateDominator[pred] != nullptr) {
                        newIdom = pred;
                        break;
                    }
                }

                // Intersect all other processed predecessors
                for (BasicBlock* pred : block->getPredecessors()) {
                    if (pred == newIdom) continue;
                    if (immediateDominator[pred] != nullptr) {
                        newIdom = intersectDominators(pred, newIdom);
                    }
                }

                if (immediateDominator[block] != newIdom) {
                    immediateDominator[block] = newIdom;
                    changed = true;
                }
            }
        }
    }

    // Helper function to find common dominator
    BasicBlock* intersectDominators(BasicBlock* b1, BasicBlock* b2) {
        // Find common dominator of b1 and b2
        BasicBlock* runner = b1;
        while (runner != immediateDominator[b2]) {
            runner = immediateDominator[runner];
        }
        return runner;
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
                    while (runner != immediateDominator[block] && runner != nullptr) {
                        dominanceFrontier[runner].insert(block);
                        runner = immediateDominator[runner];
                    }
                }
            }
        }
    }

    // Insert Phi functions
    void insertPhiFunctions(CFG* cfg) {
        // Collect all variables that are assigned to
        std::unordered_set<std::string> variables;
        for (BasicBlock* block : cfg->getBlocksList()) {
            for (LlStatement* stmt : block->getLlStatements()) {
                std::string* def = stmt->getDefinedVariable();
                if (def) variables.insert(*def);
            }
        }

        // For each variable, insert phi functions where needed
        for (const std::string& var : variables) {
            std::unordered_set<BasicBlock*> phiBlocks;
            std::queue<BasicBlock*> workList;
            
            // Find blocks where variable is defined
            for (BasicBlock* block : cfg->getBlocksList()) {
                for (LlStatement* stmt : block->getLlStatements()) {
                    std::string* def = stmt->getDefinedVariable();
                    if (def && *def == var) {
                        workList.push(block);
                        break;
                    }
                }
            }

            // Insert phi functions in dominance frontier
            while (!workList.empty()) {
                BasicBlock* block = workList.front();
                workList.pop();

                for (BasicBlock* dfBlock : dominanceFrontier[block]) {
                    if (phiBlocks.find(dfBlock) == phiBlocks.end()) {
                        // Create phi function with the right number of incoming edges
                        LlPhiStatement* phi = new LlPhiStatement(var, dfBlock->getPredecessors().size());
                        
                        // Set incoming values for each predecessor
                        size_t index = 0;
                        for (BasicBlock* pred : dfBlock->getPredecessors()) {
                            // For each predecessor, we'll use the original variable name
                            // The actual renaming will happen in the renameVariables phase
                            std::string* incomingVar = new std::string(var);
                            phi->setIncoming(index++, incomingVar, pred);
                        }
                        
                        // Insert phi at the beginning of the block
                        dfBlock->getLlStatements().insert(dfBlock->getLlStatements().begin(), phi);
                        phiBlocks.insert(dfBlock);
                        workList.push(dfBlock);
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
                if (def) {
                    variableStack[*def] = std::stack<int>();
                    variableStack[*def].push(0);
                }
            }
        }

        // Start renaming from entry block
        renameVariablesInBlock(cfg->getEntry());
    }

    void renameVariablesInBlock(BasicBlock* block) {
        // Save copies of stacks
        std::unordered_map<std::string, std::stack<int>> savedStacks;
        for (const auto& pair : variableStack) {
            savedStacks[pair.first] = pair.second;
        }

        // Rename definitions and uses in the block
        for (LlStatement* stmt : block->getLlStatements()) {
            // Rename uses
            for (std::string* use : stmt->getUsedVariables()) {
                if (variableStack.find(*use) != variableStack.end() && !variableStack[*use].empty()) {
                    stmt->renameUse(*use, *use + "_" + std::to_string(variableStack[*use].top()));
                }
            }

            // Rename definition
            std::string* def = stmt->getDefinedVariable();
            if (def) {
                int newVersion = variableVersions[*def]++;
                variableStack[*def].push(newVersion);
                stmt->renameDef(*def, *def + "_" + std::to_string(newVersion));
            }
        }

        // Recursively rename in successor blocks
        for (BasicBlock* succ : block->getSuccessors()) {
            renameVariablesInBlock(succ);
        }

        // Restore stacks
        variableStack = savedStacks;
    }

public:
    void convertToSSA(CFG* cfg) {
        // Step 1: Compute dominators
        computeDominators(cfg);

        // output the dominance tree
        std::cout << "\nDominance Tree:" << std::endl;
        for (const auto& pair : immediateDominator) {
            std::cout << pair.first->getLabel() << " <- " << pair.second->getLabel() << std::endl;
        }

        // Step 2: Compute dominance frontier
        computeDominanceFrontier(cfg);

        // output the dominance frontier
        std::cout << "Dominance Frontier:" << std::endl;
        for (const auto& pair : dominanceFrontier) {
            std::cout << pair.first->getLabel() << " -> ";
            for (const auto &dfBlock: pair.second) {
                std::cout << dfBlock->getLabel() << " ";
            }
            std::cout << std::endl;
        }

        // Step 3: Insert phi functions
        insertPhiFunctions(cfg);

        // output the CFG after inserting phi functions
        std::cout << "\nCFG after inserting phi functions:" << std::endl;
        cfg->writeDotFile("cfg_phi.dot");

        // Step 4: Rename variables
        renameVariables(cfg);
    }
};

#endif