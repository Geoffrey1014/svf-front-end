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
    std::string toString() override{
        return "IrStmtReturnVoid";
    }
};

class IrCompoundStmt : public IrStatement {
private:
    std::deque<IrStatement*> stmtsList;
    IrExpr* expr;
public:
    IrCompoundStmt(const TSNode& node)
        : IrStatement(node), stmtsList(), expr(nullptr) {}
    virtual ~IrCompoundStmt() {
        for (IrStatement* stmt : this->stmtsList) {
            delete stmt;
        }
        delete expr;
    }

    void addStmtToFront(IrStatement* stmt) {
        this->stmtsList.push_front(stmt);
    }

    void setExpression(IrExpr* expr) {
        this->expr = expr;
    }

    std::string toString() const {
        std::string s = "";
        for (IrStatement* statement: this->stmtsList) {
            s += statement->toString() + "\n";
        }
        return s;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--block\n";

        for (IrStatement* statement : this->stmtsList) {
            prettyString += statement->prettyPrint("  " + indentSpace);
        }
        if (this->expr) {
            prettyString += this->expr->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        for (IrStatement* stmt: this->stmtsList) {
            stmt->generateLlIr(builder, symbolTable);
        }
        if (this->expr) {
            expr->generateLlIr(builder, symbolTable);
        }
        return nullptr;
    }

};

class IrUnsafeBlock : public IrStatement {
private:
    IrCompoundStmt* block;
public:
    IrUnsafeBlock(IrCompoundStmt* block, const TSNode& node)
        : IrStatement(node), block(block) {}
    ~IrUnsafeBlock() {
        delete block;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--unsafeBlock\n";
        prettyString += block->prettyPrint("  " + indentSpace);
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        return block->generateLlIr(builder, symbolTable);
    }
};

class IrExprStmt : public IrStatement {
private:
    IrExpr* expr;
    IrUnsafeBlock* unsafeBlock;
public:
    IrExprStmt(IrExpr* expr, const TSNode& node) : IrStatement(node), expr(expr) {}
    IrExprStmt(IrUnsafeBlock* unsafeBlock, const TSNode& node) : IrStatement(node), unsafeBlock(unsafeBlock) {}
    ~IrExprStmt() {
        delete expr;
        delete unsafeBlock;
    }

    IrExpr* getExpr() {
        return this->expr;
    }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--exprStmt\n";

        // pretty print the expression
        if (this->expr) {
            prettyString += this->expr->prettyPrint("  " + indentSpace);
        } else if (this->unsafeBlock) {
            prettyString += this->unsafeBlock->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }
    std::string toString() override{
        return "IrExprStmt";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        if (this->expr) {
            return this->expr->generateLlIr(builder, symbolTable);
        } else if (this->unsafeBlock) {
            return this->unsafeBlock->generateLlIr(builder, symbolTable);
        }
    }
};

class IrElseClause : public Ir {
private:
    IrExpr* elseExpr;
    IrStatement* elseStmt;
public:
    IrElseClause(IrExpr* elseExpr, const TSNode& node)
        : elseExpr(elseExpr), elseStmt(nullptr), Ir(node) {}
    IrElseClause(IrStatement* elseStmt, const TSNode& node)
        : elseExpr(nullptr), elseStmt(elseStmt), Ir(node) {}
    ~IrElseClause() {
        delete elseExpr;
        delete elseStmt;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--elseClause\n";
        if (elseExpr) {
            prettyString += elseExpr->prettyPrint("  " + indentSpace);
        } else if (elseStmt) {
            prettyString += elseStmt->prettyPrint("  " + indentSpace);
        }
        return prettyString;
    }
};

#endif