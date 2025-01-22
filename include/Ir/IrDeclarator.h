#ifndef IR_DECLARATOR_H
#define IR_DECLARATOR_H

#include "Ir.h"
#include "IrDecl.h"
#include <iostream>

class IrDeclDeclarator : public virtual Ir {
public:
    IrDeclDeclarator(const TSNode& node) : Ir(node) {}
    virtual const std::string getName() const {
        static const std::string emptyString = "";
        return emptyString;
    }
};

//     LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
//         LlLocation* index = sizeExpr->generateLlIr(builder, symbolTable);
//         LlLocationArray* arrayLoc = new LlLocationArray(new std::string(getName()), index);
//         return arrayLoc;
//     }
// };

class IrAbstractPointerDeclarator : public IrDeclDeclarator {
private:
    IrDeclDeclarator* baseDeclarator;
public:
    IrAbstractPointerDeclarator(IrDeclDeclarator* base, const TSNode& node)
        : Ir(node), IrDeclDeclarator(node), baseDeclarator(base) {}

    ~IrAbstractPointerDeclarator() {
        delete baseDeclarator;
    }

    const std::string getName() const override {
        if (baseDeclarator) {
            return baseDeclarator->getName();
        }
        return "";
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string str = indentSpace + "|--abstract_pointer_declarator: *\n";
        if(baseDeclarator){
            str += baseDeclarator->prettyPrint(addIndent(indentSpace));
        }
        return str;
    }

    std::string toString() const override{
        if (baseDeclarator) {
            return baseDeclarator->toString() + "*";
        }
        return "*";
    }
};

class IrIdent : public IrDeclDeclarator, public IrExpr {
private:
    const std::string name;
    bool isTypeAlias;

public:
    IrIdent(const std::string& name, const TSNode & node, bool isTypeAlias = false) : Ir(node), IrDeclDeclarator(node), IrExpr(node), name(name) {}
    ~IrIdent() = default;

    const std::string & getValue() const {
        return name;
    }

    bool operator==(const Ir & that) const override{
        if (&that == this) {
            return true;
        }
        if (auto thatIdent = dynamic_cast<const IrIdent *>(&that)) {
            return this->name == thatIdent->name;
        }
        return false;
    }

    int hashCode() const {
        std::hash<std::string> hasher;
        return hasher(this->name);
    }
    bool isType() const { return isTypeAlias; }
    void markAsTypeAlias() { isTypeAlias = true; }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--id: " + name + "\n";
    }

    const std::string getName() const override {
        return name;
    }

    std::string toString() const override{
        return name;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        return new LlLocationVar(new std::string(name));
    }

};
#endif