#ifndef IR_BINARY_EXPR_H
#define IR_BINARY_EXPR_H
#include "IrExpr.h"

class IrBinaryExpr : public IrExpr {
private:
    std::string* operation;
    IrExpr* leftOperand;
    IrExpr* rightOperand;

public:
    IrBinaryExpr(std::string* operation, IrExpr* leftOperand, IrExpr* rightOperand, const TSNode & node) 
        : IrExpr(node), operation(operation), 
          leftOperand(leftOperand), rightOperand(rightOperand) {}
    ~IrBinaryExpr() {
        delete operation;
        delete leftOperand;
        delete rightOperand;
    }
    IrExpr* getLeftOperand() {
        return this->leftOperand;
    }

    IrExpr* getRightOperand() {
        return this->rightOperand;
    }

    std::string* getOperation() {
        return this->operation;
    }

    std::string toString() override{
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

     LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* left = leftOperand->generateLlIr(builder, symbolTable);
        LlLocation* right = rightOperand->generateLlIr(builder, symbolTable);
        LlLocationVar* result = builder.generateTemp();
        builder.appendStatement(new LlAssignStmtBinaryOp(result, left, *operation, right));
        return result;
     }
};
#endif