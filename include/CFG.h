#ifndef CFG_BUILDER_H
#define CFG_BUILDER_H

#include "LlBuilder.h" // Include your LlBuilder header
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

// Forward declaration
class LlStatement;

// Basic Block class representing a node in the CFG
class BasicBlock {
private:
    std::string label;
    std::vector<LlStatement*> llStatements; 
    std::unordered_set<BasicBlock*> predecessors;
    std::unordered_set<BasicBlock*> successors;

public:
    BasicBlock(const std::string& label) : label(label) {}

    void addSuccessor(BasicBlock* block) {
        successors.insert(block);
    }

    void addPredecessor(BasicBlock* block) {
        predecessors.insert(block);
    }

    const std::string& getLabel() const {
        return label;
    }

    void addLlStatement(LlStatement* stmt){
        llStatements.push_back(stmt);
    }

    const std::vector<LlStatement*>& getLlStatements() const {
        return llStatements;
    }


    const std::unordered_set<BasicBlock*>& getSuccessors() const {
        return successors;
    }

    const std::unordered_set<BasicBlock*>& getPredecessors() const {
        return predecessors;
    }
};

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

        // connect the blocks according to the last instruction of each block
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
            else if (i + 1 == blocksList.size()){
                // add exit as a successor
                block->addSuccessor(cfg->getExit());
                cfg->getExit()->addPredecessor(block);
            }
            else{
                block->addSuccessor(blocksList[i+1]);
                blocksList[i+1]->addPredecessor(block);
            }

            
        }
        return cfg;
    }
};

#endif