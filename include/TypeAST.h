#include "ASTNode.h"
class TypeAST : public ASTNode {
    public:
        TypeAST(TSNode node) : ASTNode(node) {}
};

class ASTTypeVoid : public TypeAST {
public:
    ASTTypeVoid(TSNode node) : TypeAST(node) {}

    int hashCode() const {
        return 13; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: void\n";
    }
};