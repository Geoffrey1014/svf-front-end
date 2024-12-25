#ifndef IR_STATEMENT_H
#define IR_STATEMENT_H
#include "Ir.h"
#include "IrExpr.h"
#include "IrNonBinaryExpr.h"
#include <deque>

class IrStatement : public Ir {
public:
    IrStatement(const TSNode& node) : Ir(node) {}
    virtual ~IrStatement() = default;
    std::string toString() const override{
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

        prettyString += this->result->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }
    std::string toString() const{
        string s = "return " + result->toString();
        return s;
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
    std::string toString() const{
        return "IrStmtReturnVoid";
    }
};

class IrCompoundStmt : public IrStatement {
private:
    std::deque<IrStatement*> stmtsList;
public:
    IrCompoundStmt(const TSNode& node)
        : IrStatement(node),
          stmtsList() {}
    ~IrCompoundStmt() {
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
            prettyString += statement->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }
    std::string toString() const{
        std::string s = "";
        for (IrStatement* statement: this->stmtsList) {
            s += statement->toString() + "\n";
        }
        return s;
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
        prettyString += this->expr->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }
    std::string toString() const{
        string s = expr->toString();
        return s;
    }
};

class IrElseClause : public IrStatement {
private:
    IrStatement* alternative;

public:
    IrElseClause(IrStatement* alternative, const TSNode& node)
        : IrStatement(node), alternative(alternative) {}

    ~IrElseClause() {
        delete alternative;
    }

    IrStatement* getAlternative() const {
        return alternative;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--elseClause\n";
        prettyString += alternative->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override {
        std::string result = "else " + alternative->toString();
        return result;
    }
};

class IrIfStmt : public IrStatement {
private:
    IrParenthesizedExpr* condition;  
    IrStatement* thenBody;
    IrElseClause* elseBody;  
public:
    IrIfStmt(IrParenthesizedExpr* condition, IrStatement* thenBody, IrElseClause* elseBody, const TSNode& node)
        : IrStatement(node), condition(condition), thenBody(thenBody), elseBody(elseBody) {}

    ~IrIfStmt() {
        delete condition;
        delete thenBody;
        delete elseBody;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--ifStmt\n";

        // Print condition
        prettyString += addIndent(indentSpace) + "|--condition\n";
        prettyString += condition->prettyPrint(addIndent(indentSpace, 2));

        // Print thenBody (if block)
        prettyString += addIndent(indentSpace) + "|--consequence\n";
        prettyString += thenBody->prettyPrint(addIndent(indentSpace, 2));

        // Print elseBody (if exists)
        if (elseBody) {
            prettyString += addIndent(indentSpace) + "|--else\n";
            prettyString += elseBody->prettyPrint(addIndent(indentSpace, 2));
        }

        return prettyString;
    }

    std::string toString() const override {
        std::string result = "if " + condition->toString() + " " + thenBody->toString();
        if (elseBody) {
            result += " " + elseBody->toString();
        }
        return result;
    }
};

#endif