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
    std::string toString() const override{
        string s = "return " + result->toString();
        return s;
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
    std::string toString() const override{
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
    std::string toString() const override{
        std::string s = "";
        for (IrStatement* statement: this->stmtsList) {
            s += statement->toString() + "\n";
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

    std::string toString() const override{
        string s = expr->toString();
        return s;
    }

    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        return expr->generateLlIr(builder, symbolTable);
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

    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        alternative->generateLlIr(builder, symbolTable);
        return nullptr;
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

    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        LlLocation* conditionVar = this->condition->generateLlIr(builder, symbolTable);

        std::string label = builder.generateLabel();
        std::string* ifThenLabel = new std::string();
        ifThenLabel->append("if.then.");
        ifThenLabel->append(label);
        
        std::string* endLabel = new std::string();
        endLabel->append("if.end.");
        endLabel->append(label);

        
        LlJumpConditional* conditionalJump = new LlJumpConditional(ifThenLabel,conditionVar);
        builder.appendStatement(conditionalJump);

        if (elseBody) {
            std::string* elseLabel = new std::string();
            elseLabel->append("if.else.");
            elseLabel->append(label);
            LlEmptyStmt* emptyStmtElse = new LlEmptyStmt();
            LlJumpUnconditional *jumpUnconditionalToElse = new LlJumpUnconditional(elseLabel);
            builder.appendStatement(jumpUnconditionalToElse);
            builder.appendStatement(*elseLabel, emptyStmtElse);
            elseBody->generateLlIr(builder, symbolTable);
        }
        
        LlJumpUnconditional *jumpUnconditionalToEnd = new LlJumpUnconditional(endLabel);
        builder.appendStatement(jumpUnconditionalToEnd);

        // add the label to the if body block
        LlEmptyStmt* emptyStmt = new LlEmptyStmt();
        builder.appendStatement(*ifThenLabel, emptyStmt);
        thenBody->generateLlIr(builder, symbolTable);


        // append end if label
        LlEmptyStmt* endIfEmptyStmt = new LlEmptyStmt();
        builder.appendStatement(*endLabel, endIfEmptyStmt);
        return nullptr;
    }
};

#endif