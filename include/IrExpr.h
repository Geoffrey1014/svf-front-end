#include "Ir.h"
#ifndef IR_EXPR_H
#define IR_EXPR_H

class IrExpr : public Ir {
public:
    IrExpr(const TSNode & node) : Ir(node) {}
    
    std::string toString() {
        return "IrExpr";
    }

    bool operator==(const IrExpr& other) const {
        // Implementation of operator==
        return true;
    }

    int hashCode() const {
        // Implementation of hashCode
        return 0;
    }
};


class IrIdent : public IrExpr {
private:
    const std::string* name;

public:
    IrIdent(const std::string* name, const TSNode & node) : IrExpr(node), name(name) {}
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


class IrBinaryExpr : public IrExpr {
private:
    std::string* operation;
    IrExpr* leftOperand;
    IrExpr* rightOperand;

public:
    IrBinaryExpr(std::string* operation, IrExpr* leftOperand, IrExpr* rightOperand, const TSNode & node) 
        : IrExpr(node), operation(operation), 
          leftOperand(leftOperand), rightOperand(rightOperand) {}

    IrExpr* getLeftOperand() {
        return this->leftOperand;
    }

    IrExpr* getRightOperand() {
        return this->rightOperand;
    }

    std::string* getOperation() {
        return this->operation;
    }

    std::string toString() {
        return leftOperand->toString() + " " + *operation + " " + rightOperand->toString();
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--binaryExpr\n";

        // pretty print the lhs
        prettyString += "  " + indentSpace + "|--lhs\n";
        prettyString += this->leftOperand->prettyPrint("    " + indentSpace);

        // print the operator
        prettyString += "  " + indentSpace + "|--op: " + *(this->operation) + "\n";

        // pretty print the rhs
        prettyString += "  " + indentSpace + "|--rhs\n";
        prettyString += this->rightOperand->prettyPrint("    " + indentSpace);

        return prettyString;
    }
};
#endif