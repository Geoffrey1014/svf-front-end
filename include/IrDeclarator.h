#ifndef IR_DECLARATOR_H
#define IR_DECLARATOR_H

#include "Ir.h"
#include "IrDecl.h"

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
class IrDeclarator : public Ir {
public:
    IrDeclarator(const TSNode& node) : Ir(node) {}
    virtual ~IrDeclarator() = default;

    virtual std::string prettyPrint(std::string indentSpace) const = 0;
};

class IrArrayDeclarator : public IrDeclarator {
private:
    IrDeclarator* baseDeclarator; // Base declarator for the array
    IrExpr* sizeExpr;       // Size expression for the current dimension

public:
    IrArrayDeclarator(IrDeclarator* baseDeclarator, IrExpr* sizeExpr, const TSNode& node)
        : IrDeclarator(node), baseDeclarator(baseDeclarator), sizeExpr(sizeExpr) {}

    ~IrArrayDeclarator() {
        delete baseDeclarator;
        delete sizeExpr;
    }

    // Getter for the base declarator
    IrDeclarator* getBaseDeclarator() const {
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

class IrIdent : public IrDeclarator {
private:
    const std::string name;

public:
    IrIdent(const std::string& name, const TSNode & node) : IrDeclarator(node), name(name) {}
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
