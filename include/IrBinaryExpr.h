#include "IrExpr.h"
#ifndef IR_BINARY_EXPR_H
#define IR_BINARY_EXPR_H

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