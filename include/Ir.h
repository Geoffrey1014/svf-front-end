#include <string>
#include <tree_sitter/api.h> // Add this line to include the definition of TSNode
#include <vector>
#ifndef IR_H
#define IR_H

using namespace std;

class Ir {
    private:
        const TSNode & node;
    public:
    Ir(const TSNode & node): node(node) {}
    ~Ir() = default;

    int getLineNumber() {
        return ts_node_start_point(node).row;
    }

    int getColNumber() {
        return ts_node_start_point(node).column;
    }

    // virtual std::string semanticCheck(ScopeStack& scopeStack) = 0;
    // virtual LlLocation generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) = 0;

    virtual std::string prettyPrint(std::string indentSpace) const =0;

    virtual std::string toString() {
        return "ASTNode: " + std::to_string(getLineNumber()) + ", " + std::to_string(getColNumber()) + " - " + std::to_string(ts_node_end_point(node).row) + ", " + std::to_string(ts_node_end_point(node).column);
    }
};



class IrIdent : public Ir {
private:
    const std::string* name;

public:
    IrIdent(const std::string* name, const TSNode & node) : Ir(node), name(name) {}
    ~IrIdent() {
        delete name;
    }

    const std::string* getValue() const{
        return name;
    }

    bool operator==(const Ir & that) const {
        if (&that == this) {
            return true;
        }
        if (auto thatIdent = dynamic_cast<const IrIdent*>(&that)) {
            return *(this->getValue()) == *(thatIdent->getValue());
        }
        return false;
    }

    int hashCode() const {
        std::hash<std::string> hasher;
        return hasher(*this->name);
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--id: " + *name + "\n";
    }
    std::string toString() {
        return "IrIdent: " + *name;
    }

};

#endif