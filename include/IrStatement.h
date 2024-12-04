#ifndef IR_STATEMENT_H
#define IR_STATEMENT_H
#include "Ir.h"
#include "IrExpr.h"

class IrStatement : public Ir {
public:
    IrStatement(const TSNode& node) : Ir(node) {}
    std::string toString() override{
        return "IrStatement";
    }
};

class IrStmtReturn : public IrStatement {
public:
    IrStmtReturn(const TSNode& node) : IrStatement(node) {}
    // virtual IrType* getExpressionType() = 0; // pure virtual function
};

class IrStmtReturnExpr : public IrStmtReturn {
private:
    IrExpr* result;

public:
    IrStmtReturnExpr(IrExpr* result, const TSNode& node) : IrStmtReturn(node), result(result) {}

    // IrType* getExpressionType() override {
    //     return this->result->getExpressionType();
    // }

    std::string prettyPrint(std::string indentSpace)const override {
        std::string prettyString = indentSpace + "|--returnExpr\n";

        // pretty print the expression
        prettyString += this->result->prettyPrint("  " + indentSpace);

        return prettyString;
    }
};

class IrStmtReturnVoid : public IrStmtReturn {
public:
    IrStmtReturnVoid(const TSNode& node) : IrStmtReturn(node) {}

    // IrType* getExpressionType() override {
    //     return new IrTypeVoid(this->getLineNumber(), this->getColNumber());
    // }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--returnVoid\n";
        return prettyString;
    }
    std::string toString() {
        return "IrStmtReturnVoid";
    }
};

class IrCompoundStmt : public IrStatement {
private:
    std::vector<IrStatement*> stmtsList;
public:
    IrCompoundStmt(const TSNode& node)
        : IrStatement(node),
          stmtsList() {}

    // std::vector<IrStatement*> getStmtsList() {
    //     return this->stmtsList;
    // }
    void addStmt(IrStatement* stmt) {
        this->stmtsList.push_back(stmt);
    }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--compoundStmt:\n";

        // pretty print statement
        for (IrStatement* statement: this->stmtsList) {
            prettyString += statement->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }
};

class IrExprStmt : public IrStatement {
private:
    IrExpr* expr;
public:
    IrExprStmt(IrExpr* expr, const TSNode& node) : IrStatement(node), expr(expr) {}

    IrExpr* getExpr() {
        return this->expr;
    }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--exprStmt\n";

        // pretty print the expression
        prettyString += this->expr->prettyPrint("  " + indentSpace);

        return prettyString;
    }
    std::string toString() {
        return "IrExprStmt";
    }
};

#endif