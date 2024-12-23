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

class IrArrayDeclarator : public IrDeclDeclarator {
private:
    IrDeclDeclarator* baseDeclarator; // Base declarator for the array
    IrExpr* sizeExpr;       // Size expression for the current dimension

public:
    IrArrayDeclarator(IrDeclDeclarator* baseDeclarator, IrExpr* sizeExpr, const TSNode& node)
        : Ir(node), IrDeclDeclarator(node), baseDeclarator(baseDeclarator), sizeExpr(sizeExpr) {}

    ~IrArrayDeclarator() {
        delete baseDeclarator;
        delete sizeExpr;
    }

    // Getter for the base declarator
    IrDeclDeclarator* getBaseDeclarator() const {
        return baseDeclarator;
    }

    // Getter for the size expression
    IrExpr* getSizeExpr() const {
        return sizeExpr;
    }

    const std::string getName() const override {
        if (baseDeclarator) {
            return baseDeclarator->getName();
        }
        return "";
    }

    // Pretty print method to adhere to the CST hierarchy
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--array_declarator\n";

        if (baseDeclarator) {
            prettyString += baseDeclarator->prettyPrint(addIndent(indentSpace));
        } else{
            std::cout << "Error: Invalid base declarator" << std::endl;
        }

        if (sizeExpr){
            prettyString += sizeExpr->prettyPrint(addIndent(indentSpace));
        }
        
        return prettyString;
    }

    std::string toString() const{
        return  baseDeclarator->toString() + "[" + sizeExpr->toString() + "]";
    }
};

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

    std::string toString() const{
        if (baseDeclarator) {
            return "IrAbstractPointerDeclarator: " + baseDeclarator->toString() + "*";
        }
        return "IrAbstractPointerDeclarator ";
    }
};

class IrPointerDeclarator : public IrDeclDeclarator {
private:
    IrDeclDeclarator* baseDeclarator;
public:
    IrPointerDeclarator(IrDeclDeclarator* base, const TSNode& node) : Ir(node), IrDeclDeclarator(node), baseDeclarator(base) {}
    ~IrPointerDeclarator() { delete baseDeclarator; }

    const std::string getName() const override {
        if (baseDeclarator) {
            return baseDeclarator->getName();
        }
        return "";
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string str = indentSpace + "|--pointer_declarator(*)\n";
        str += baseDeclarator->prettyPrint(addIndent(indentSpace));
        return str;
    }

    std::string toString() const{
        return baseDeclarator->toString() + "*";
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

    bool operator==(const Ir & that) const {
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

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--id: " + name + "\n";
    }

    const std::string getName() const override {
        return name;
    }

    std::string toString() const{
        return name;
    }

};
#endif

// Though this is not the named node, but it's kind of necessary to have this
// Pop the declarator (_declaration_declarator: function, array, or variable)
// _declaration_declarator: $ => choice(
//   $.attributed_declarator,
//   $.pointer_declarator,  --- done
//   alias($._function_declaration_declarator, $.function_declarator),
//   $.array_declarator,  --- done
//   $.parenthesized_declarator,
//   $.identifier,   --- done
// ),