#ifndef IR_BINARY_EXPR_H
#define IR_BINARY_EXPR_H
#include "IrExpr.h"

class IrBinaryExpr : public IrExpr {
private:
    std::string operation;
    IrExpr* leftOperand;
    IrExpr* rightOperand;

public:
    IrBinaryExpr(std::string& operation, IrExpr* leftOperand, IrExpr* rightOperand, const TSNode & node) 
        : Ir(node), IrExpr(node), operation(operation), 
          leftOperand(leftOperand), rightOperand(rightOperand) {}
    ~IrBinaryExpr() {
        delete leftOperand;
        delete rightOperand;
    }
    IrExpr* getLeftOperand() {
        return this->leftOperand;
    }

    IrExpr* getRightOperand() {
        return this->rightOperand;
    }

    std::string& getOperation() {
        return this->operation;
    }

    std::string toString() const{
        return leftOperand->toString() + " " + operation + " " + rightOperand->toString();
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--binaryExpr\n";

        // pretty print the lhs
        prettyString += addIndent(indentSpace) + + "|--lhs\n";
        prettyString += this->leftOperand->prettyPrint(addIndent(indentSpace, 2));

        // print the operator
        prettyString += addIndent(indentSpace) + "|--op: " + operation + "\n";

        // pretty print the rhs
        prettyString += addIndent(indentSpace) + "|--rhs\n";
        prettyString += this->rightOperand->prettyPrint(addIndent(indentSpace, 2));

        return prettyString;
    }
};
#endif