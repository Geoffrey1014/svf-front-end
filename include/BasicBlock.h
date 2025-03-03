#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <string>
#include <vector>
#include <unordered_set>

// Forward declarations
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

    std::vector<LlStatement*>& getLlStatements() {
        return llStatements;
    }

    const std::unordered_set<BasicBlock*>& getSuccessors() const {
        return successors;
    }

    const std::unordered_set<BasicBlock*>& getPredecessors() const {
        return predecessors;
    }
};

#endif 