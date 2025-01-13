#ifndef IR_STATEMENT_H
#define IR_STATEMENT_H
#include "Ir.h"
#include "IrExpr.h"

class IrStatement : public Ir {
public:
    IrStatement(const TSNode& node) : Ir(node) {}
    virtual ~IrStatement() = default;
    std::string toString() override{
        return "IrStatement";
    }
};

class IrStmtReturn : public IrStatement {
public:
    IrStmtReturn(const TSNode& node) : IrStatement(node) {}
    virtual ~IrStmtReturn() = default;
    // virtual IrType* getExpressionType() = 0; // pure virtual function
};

class IrStmtReturnExpr : public IrStmtReturn {
private:
    IrExpr* result;

public:
    IrStmtReturnExpr(IrExpr* result, const TSNode& node) : IrStmtReturn(node), result(result) {}
    ~IrStmtReturnExpr() {
        delete result;
    }
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
    ~IrStmtReturnVoid() = default;
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
    IrExpr* expr;
    std::deque<IrStatement*> stmtsList;
public:
    IrCompoundStmt(IrExpr* expr, const TSNode& node)
        : IrStatement(node), expr(expr),
          stmtsList() {}
    ~IrCompoundStmt() {
        delete expr;
        for (IrStatement* stmt: this->stmtsList) {
            delete stmt;
        }
    }

    void addStmtToFront(IrStatement* stmt) {
        this->stmtsList.push_front(stmt);
    }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--compoundStmt:\n";

        for (IrStatement* statement: this->stmtsList) {
            prettyString += statement->prettyPrint("  " + indentSpace);
        }
        if (this->expr) {
            prettyString += this->expr->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }
};

class IrExprStmt : public IrStatement {
private:
    IrExpr* expr;
public:
    IrExprStmt(IrExpr* expr, const TSNode& node) : IrStatement(node), expr(expr) {}
    ~IrExprStmt() {
        delete expr;
    }

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

class IrElseClause : public IrExpr {
private:
    IrExpr* elseExpr;
    IrStatement* elseStmt;
public:
    IrElseClause(IrExpr* elseExpr, const TSNode& node)
        : elseExpr(elseExpr), elseStmt(nullptr), IrExpr(node) {}
    IrElseClause(IrStatement* elseStmt, const TSNode& node)
        : elseExpr(nullptr), elseStmt(elseStmt), IrExpr(node) {}
    ~IrElseClause() {
        delete elseExpr;
        delete elseStmt;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--else_clause\n";
        if (elseExpr) {
            prettyString += elseExpr->prettyPrint("  " + indentSpace);
        } else if (elseStmt) {
            prettyString += elseStmt->prettyPrint("  " + indentSpace);
        }
        return prettyString;
    }
};

#endif