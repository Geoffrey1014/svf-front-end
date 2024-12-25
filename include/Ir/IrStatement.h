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
    
    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        std::cerr << "Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("")); // Return empty location
    }
};

class IrStmtReturn : public IrStatement {
public:
    IrStmtReturn(const TSNode& node) : IrStatement(node) {}
    virtual ~IrStmtReturn() = default;
    
    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        std::cerr << "Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("")); // Return empty location
    }
};

class IrStmtReturnExpr : public IrStmtReturn {
private:
    IrExpr* result;

public:
    IrStmtReturnExpr(IrExpr* result, const TSNode& node) : IrStmtReturn(node), result(result) {}
    virtual ~IrStmtReturnExpr() {
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
        return "IrStmtReturnExpr";
    }

    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        LlLocation* resultVar = this->result->generateLlIr(builder, symbolTable);
        LlReturn* returnStmt = new LlReturn(resultVar);
        builder.appendStatement(returnStmt);
        return resultVar;
    }
};

class IrStmtReturnVoid : public IrStmtReturn {
public:
    IrStmtReturnVoid(const TSNode& node) : IrStmtReturn(node) {}
    virtual ~IrStmtReturnVoid() = default;
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
    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        LlReturn* returnStmt = new LlReturn(nullptr);
        builder.appendStatement(returnStmt);
        return nullptr;
    }
};

class IrCompoundStmt : public IrStatement {
private:
    std::deque<IrStatement*> stmtsList;
public:
    IrCompoundStmt(const TSNode& node)
        : IrStatement(node),
          stmtsList() {}
    virtual ~IrCompoundStmt() {
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
            s += statement->toString();
        }
        return s;
    }

    LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override {
        for (IrStatement* stmt: this->stmtsList) {
            stmt->generateLlIr(builder, symbolTable);
        }
        return nullptr;
    }
};

class IrExprStmt : public IrStatement {
private:
    IrExpr* expr;
public:
    IrExprStmt(IrExpr* expr, const TSNode& node) : IrStatement(node), expr(expr) {}
    virtual ~IrExprStmt() {
        delete expr;
    }

    IrExpr* getExpr() {
        return this->expr;
    }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--exprStmt\n";
        prettyString += this->expr->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const{
        return "IrExprStmt";
    }

    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        return this->expr->generateLlIr(builder, symbolTable);
    }
};

#endif