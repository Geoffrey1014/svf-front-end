#include "Ir.h"
class TypeAST : public Ir {
    public:
        TypeAST(TSNode* node) : Ir(node) {}
};

class TypeBoolAST : public TypeAST {
public:
    TypeBoolAST(TSNode* node) : TypeAST(node) {}

    int hashCode() const {
        return 11; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: bool\n";
    }
};

class ASTTypeVoid : public TypeAST {
public:
    ASTTypeVoid(TSNode* node) : TypeAST(node) {}

    int hashCode() const {
        return 13; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: void\n";
    }
};

class ASTTypeInt : public TypeAST {
public:
    ASTTypeInt(TSNode* node) : TypeAST(node) {}

    int hashCode() const {
        return 17; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: int\n";
    }
};

