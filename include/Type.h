#include "NodeAST.h"
class Type : public NodeAST {
    public:
        Type(TSNode* node) : NodeAST(node) {}
};

class TypeBoolAST : public Type {
public:
    TypeBoolAST(TSNode* node) : Type(node) {}

    int hashCode() const {
        return 11; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: bool\n";
    }
};

class TypeVoid : public Type {
public:
    TypeVoid(TSNode* node) : Type(node) {}

    int hashCode() const {
        return 13; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: void\n";
    }
};

class ASTTypeInt : public Type {
public:
    ASTTypeInt(TSNode* node) : Type(node) {}

    int hashCode() const {
        return 17; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: int\n";
    }
};

