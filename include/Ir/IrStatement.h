#ifndef IR_STATEMENT_H
#define IR_STATEMENT_H
#include "Ir.h"
#include "IrExpr.h"
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

class IrIfStmt : public IrStatement {
private:
    IrExpr* condition;
    IrStatement* consequence;
    IrStatement* alternative;  // Can be null if no else clause exists

public:
    IrIfStmt(IrExpr* cond, IrStatement* cons, IrStatement* alt, const TSNode& node)
        : IrStatement(node), condition(cond), consequence(cons), alternative(alt) {}

    ~IrIfStmt() {
        delete condition;
        delete consequence;
        if (alternative) {
            delete alternative;
        }
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--ifStmt\n";
        prettyString += addIndent(indentSpace) + "|--condition\n";
        prettyString += condition->prettyPrint(addIndent(indentSpace, 2));
        prettyString += addIndent(indentSpace) + "|--consequence\n";
        prettyString += consequence->prettyPrint(addIndent(indentSpace, 2));
        if (alternative) {
            prettyString += addIndent(indentSpace) + "|--else\n";
            prettyString += alternative->prettyPrint(addIndent(indentSpace, 2));
        }
        return prettyString;
    }

    std::string toString() const override {
        std::string result = "if" + condition->toString() + "{" + consequence->toString(); + "}\n";
        if (alternative) {
            result += " else {" + alternative->toString(); + "}\n";
        }
        return result;
    }
};


#endif