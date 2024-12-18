#ifndef IR_DECLARATOR_H
#define IR_DECLARATOR_H

#include "Ir.h"
#include "IrDecl.h"

class IrDeclDeclarator : public virtual Ir {
public:
    IrDeclDeclarator(const TSNode& node) : Ir(node) {}};

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

    // Pretty print method to adhere to the CST hierarchy
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--array_declarator\n";

        if (baseDeclarator) {
            prettyString += baseDeclarator->prettyPrint("    " + indentSpace);
        } else{
            std::cout << "Error: Invalid base declarator" << std::endl;
        }

        if (sizeExpr){
            prettyString += sizeExpr->prettyPrint("    " + indentSpace);
        }
        
        return prettyString;
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

    std::string prettyPrint(std::string indentSpace) const override {
        std::string str = indentSpace + "|--abstract_pointer_declarator: *\n";
        if(baseDeclarator){
            str += baseDeclarator->prettyPrint(indentSpace + "  ");
        }
        return str;
    }
};

class IrPointerDeclarator : public IrDeclDeclarator {
private:
    IrDeclDeclarator* baseDeclarator;
public:
    IrPointerDeclarator(IrDeclDeclarator* base, const TSNode& node) : Ir(node), IrDeclDeclarator(node), baseDeclarator(base) {}
    ~IrPointerDeclarator() { delete baseDeclarator; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string str = indentSpace + "|--pointer_declarator(*)\n";
        str += baseDeclarator->prettyPrint(indentSpace + "  ");
        return str;
    }
};

class IrIdent : public IrDeclDeclarator, public IrExpr {
private:
    const std::string name;

public:
    IrIdent(const std::string& name, const TSNode & node) : Ir(node), IrDeclDeclarator(node), IrExpr(node), name(name) {}
    ~IrIdent() = default;

    // Getter for name
    const std::string &getValue() const {
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

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--id: " + name + "\n";
    }
    std::string toString() {
        return "IrIdent: " + name;
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