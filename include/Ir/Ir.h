#ifndef IR_H
#define IR_H
#include <string>
#include <tree_sitter/api.h> 
#include <vector>
#include "Ll.h"
#include "LlBuilder.h"
#include "LlSymbolTable.h"

using namespace std;

class Ir {
    private:
        const TSNode & node;
    public:
    Ir(const TSNode & node): node(node) {}
    virtual ~Ir() = default;

    int getLineNumber() {
        return ts_node_start_point(node).row;
    }

    int getColNumber() {
        return ts_node_start_point(node).column;
    }

    const TSNode& getNode() {
        return node;
    }

    virtual bool operator==(const Ir& other) const {
        return this == &other;
    }

    // virtual std::string semanticCheck(ScopeStack& scopeStack) = 0;
    virtual LlComponent* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable){
        std::cerr << "Ir Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("Error")); 
    };
    
    virtual std::string prettyPrint(std::string indentSpace) const =0;
    virtual std::string toString() const = 0;
    
    std::string addIndent(const std::string& baseIndent, int level = 1) const {
        return baseIndent + std::string(level * 2, ' '); // 2 spaces per level
    }

};

#endif