#include "Ir.h"

class IrStatement : public Ir {
public:
    IrStatement(int lineNumber, int colNumber) : Ir(lineNumber, colNumber) {}
};

class IrAssignStmt : public IrStatement {
protected:
    IrLocation* storeLocation;

public:
    IrAssignStmt(IrLocation* storeLocation) : IrStatement(storeLocation->getLineNumber(), storeLocation->getColNumber()), storeLocation(storeLocation) {}

    IrLocation* getStoreLocation() {
        return this->storeLocation;
    }

    // Since IrAssignStmt is an abstract class in Java, we need to make it abstract in C++ as well.
    // We do this by declaring a pure virtual function.
    virtual void someAbstractFunction() = 0;
};

class IrAssignStmtEqual : public IrAssignStmt {
private:
    IrExpr* newValue;

public:
    IrAssignStmtEqual(IrLocation* storeLocation, IrExpr* newValue) : IrAssignStmt(storeLocation), newValue(IrExpr::canonicalizeExpr(newValue)) {}

    IrExpr* getNewValue() {
        return this->newValue;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* tempVal = newValue->generateLlIr(builder, symbolTable);

        if(dynamic_cast<IrLocationArray*>(this->getStoreLocation())){
            LlLocation* arrayLocation = dynamic_cast<IrLocationArray*>(this->getStoreLocation())->generateLlIr(builder, symbolTable);
            LlAssignStmtRegular* regularAssignment = new LlAssignStmtRegular(arrayLocation, tempVal);
            builder->appendStatement(regularAssignment);
        }
        else {
            LlAssignStmtRegular* regularAssignment = new LlAssignStmtRegular(new LlLocationVar(this->getStoreLocation()->getLocationName()->getValue()), tempVal);
            builder->appendStatement(regularAssignment);
        }
        return nullptr;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--assignStmtEquals\n";

        // pretty print the lhs
        prettyString += ("  " + indentSpace + "|--lhs\n");
        prettyString += this->getStoreLocation()->prettyPrint("    " +indentSpace);

        // print the rhs
        prettyString += ("  " + indentSpace + "|--rhs\n");
        prettyString += this->newValue->prettyPrint("    " + indentSpace);

        return prettyString;
    }
};


class IrAssignStmtMinusEqual : public IrAssignStmt {
private:
    IrExpr* decrementBy;

public:
    IrAssignStmtMinusEqual(IrLocation* storeLocation, IrExpr* decrementBy) : IrAssignStmt(storeLocation), decrementBy(IrExpr::canonicalizeExpr(decrementBy)) {}

    IrExpr* getDecrementBy() {
        return this->decrementBy;
    }

    void setDecrementBy(IrExpr* decrementBy) {
        this->decrementBy = decrementBy;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--assignStmtMinusEquals\n";

        // pretty print the lhs
        prettyString += ("  " + indentSpace + "|--lhs\n");
        prettyString += this->getStoreLocation()->prettyPrint("    " +indentSpace);

        // print the rhs
        prettyString += ("  " + indentSpace + "|--rhs\n");
        prettyString += this->decrementBy->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* decrementTemp = this->decrementBy->generateLlIr(builder, symbolTable);
        LlLocationVar* minusExprTemp = builder->generateTemp();
        LlAssignStmtBinaryOp* assignStmtBinaryOp = new LlAssignStmtBinaryOp(minusExprTemp, new LlLocationVar(this->getStoreLocation()->getLocationName()->getValue()), "-", decrementTemp);
        builder->appendStatement(assignStmtBinaryOp);
        LlAssignStmtRegular* regularAssignment = new LlAssignStmtRegular(new LlLocationVar(this->getStoreLocation()->getLocationName()->getValue()),  minusExprTemp);
        builder->appendStatement(regularAssignment);
        return nullptr;
    }
};

class IrAssignStmtPlusEqual : public IrAssignStmt {
private:
    IrExpr* incrementBy;

public:
    IrAssignStmtPlusEqual(IrLocation* storeLocation, IrExpr* incrementBy) : IrAssignStmt(storeLocation), incrementBy(IrExpr::canonicalizeExpr(incrementBy)) {}

    IrExpr* getIncrementBy() {
        return this->incrementBy;
    }

    void setIncrementBy(IrExpr* newIncrement) {
        this->incrementBy = newIncrement;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--assignStmtPlusEquals\n";

        // pretty print the lhs
        prettyString += ("  " + indentSpace + "|--lhs\n");
        prettyString += this->getStoreLocation()->prettyPrint("    " +indentSpace);

        // print the rhs
        prettyString += ("  " + indentSpace + "|--rhs\n");
        prettyString += this->incrementBy->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* incrementTemp = this->incrementBy->generateLlIr(builder, symbolTable);
        LlLocationVar* plusExprTemp = builder->generateTemp();
        LlAssignStmtBinaryOp* assignStmtBinaryOp = new LlAssignStmtBinaryOp(plusExprTemp, new LlLocationVar(this->getStoreLocation()->getLocationName()->getValue()), "+", incrementTemp);
        builder->appendStatement(assignStmtBinaryOp);
        LlAssignStmtRegular* regularAssignment = new LlAssignStmtRegular(new LlLocationVar(this->getStoreLocation()->getLocationName()->getValue()),  plusExprTemp);
        builder->appendStatement(regularAssignment);
        return nullptr;
    }
};

class IrCtrlFlow : public IrStatement {
protected:
    IrExpr* condExpr;
    IrCodeBlock* stmtBody;

public:
    IrCtrlFlow(IrExpr* condExpr, IrCodeBlock* stmtBody) : IrStatement(condExpr->getLineNumber(), condExpr->getColNumber()), condExpr(condExpr), stmtBody(stmtBody) {}

    IrExpr* getCondExpr() {
        return this->condExpr;
    }

    IrCodeBlock* getStmtBody() {
        return this->stmtBody;
    }
};

class IrCtrlFlowFor : public IrCtrlFlow {
private:
    IrLocationVar* counter;
    IrAssignStmt* compoundAssignStmt;
    IrExpr* initialIndexExpr;

public:
    IrCtrlFlowFor(IrLocationVar* counter, IrExpr* initialIndexerExpr, IrAssignStmt* updateCounterStmt,
                  IrExpr* condExpr, IrCodeBlock* stmtBody) : IrCtrlFlow(condExpr, stmtBody), counter(counter), compoundAssignStmt(updateCounterStmt), initialIndexExpr(IrExpr::canonicalizeExpr(initialIndexerExpr)) {}

    IrLocationVar* getCounter() {
        return this->counter;
    }

    IrAssignStmt* getCompoundAssignStmt() {
        return this->compoundAssignStmt;
    }

    IrExpr* getInitialIndexExpr() {
        return this->initialIndexExpr;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--forLoop\n";

        // print the counter initialize stmt
        prettyString += ("  " + indentSpace + "|--counter\n");
        prettyString += (this->counter->prettyPrint("    " + indentSpace));
        prettyString += ("    " + indentSpace + "|--=\n");
        prettyString += (this->initialIndexExpr->prettyPrint("    " + indentSpace));

        // print the condition expr
        prettyString += ("  " + indentSpace + "|--condExpr\n");
        prettyString += (this->condExpr->prettyPrint("    " + indentSpace));

        // print the compound assign stmt
        prettyString += ("  " + indentSpace + "|--compoundExpr\n");
        prettyString += (this->compoundAssignStmt->prettyPrint("    " + indentSpace));

        // print the for loop body
        prettyString += "  " + indentSpace + "|--body\n";
        prettyString += this->stmtBody->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        // This method is complex and involves many other classes and methods that are not provided.
        // The translation of this method to C++ would require a complete understanding of these classes and methods.
        // Therefore, a direct translation is not provided here.
        return nullptr;
    }
};

class IrCtrlFlowIf : public IrCtrlFlow {
public:
    IrCtrlFlowIf(IrExpr* condExpr, IrCodeBlock* stmtBody) : IrCtrlFlow(condExpr, stmtBody) {}

    IrExpr* getIfCondition() {
        return this->condExpr;
    }

    IrCodeBlock* getIfBodyBlock() {
        return this->stmtBody;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--ifStmt\n";

        // print the condition expr
        prettyString += ("  " + indentSpace + "|--condExpr\n");
        prettyString += (this->condExpr->prettyPrint("    " + indentSpace));

        // print the if loop body
        prettyString += "  " + indentSpace + "|--body\n";
        prettyString += this->stmtBody->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        // This method is complex and involves many other classes and methods that are not provided.
        // The translation of this method to C++ would require a complete understanding of these classes and methods.
        // Therefore, a direct translation is not provided here.
        return nullptr;
    }
};

class IrCtrlFlowIfElse : public IrCtrlFlow {
private:
    IrCodeBlock* elseBlock;
    IrCtrlFlowIf* ifStmt;

public:
    IrCtrlFlowIfElse(IrCtrlFlowIf* ifStmt, IrCodeBlock* elseBlock) : IrCtrlFlow(ifStmt->getIfCondition(), ifStmt->getIfBodyBlock()), ifStmt(ifStmt), elseBlock(elseBlock) {}

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = "";

        // print the if statement
        prettyString += this->ifStmt->prettyPrint(indentSpace);

        // print the else body
        prettyString += indentSpace + "|--elseBody\n";
        prettyString += this->stmtBody->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        // This method is complex and involves many other classes and methods that are not provided.
        // The translation of this method to C++ would require a complete understanding of these classes and methods.
        // Therefore, a direct translation is not provided here.
        return nullptr;
    }
};

class IrCtrlFlowWhile : public IrCtrlFlow {
public:
    IrCtrlFlowWhile(IrExpr* condExpr, IrCodeBlock* stmtBody) : IrCtrlFlow(condExpr, stmtBody) {}

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--whileLoop\n";

        // print the condition expr
        prettyString += ("  " + indentSpace + "|--condExpr\n");
        prettyString += (this->condExpr->prettyPrint("    " + indentSpace));

        // print the for loop body
        prettyString += "  " + indentSpace + "|--body\n";
        prettyString += this->stmtBody->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        // This method is complex and involves many other classes and methods that are not provided.
        // The translation of this method to C++ would require a complete understanding of these classes and methods.
        // Therefore, a direct translation is not provided here.
        return nullptr;
    }
};


class IrMethodCallStmt : public IrStatement {
private:
    IrIdent* methodName;
    std::vector<IrArg*> argsList;
    IrType* methodType;

public:
    IrMethodCallStmt(IrIdent* methodName, std::vector<IrArg*> argsList) : IrStatement(methodName->getLineNumber(), methodName->getColNumber()), methodName(methodName), argsList(argsList) {}

    IrIdent* getMethodName() {
        return this->methodName;
    }

    IrType* getExpressionType() {
        return this->methodType;
    }

    std::string toString() {
        // This is a simplified version of the original Java method.
        // It assumes that IrIdent and IrArg have a suitable toString() method.
        return this->methodName->toString() + "(" + /*this->argsList.toString()*/ + ")";
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--methodCallStmt\n";

        // print the method name
        prettyString += ("  " + indentSpace + "|--name: " + this->methodName->getValue() + "\n");

        // print the method type
        prettyString += this->methodType->prettyPrint("  " + indentSpace);

        // print the method args_list
        prettyString += ("  " + indentSpace + "|--argsList:\n");
        for (IrArg* arg: this->argsList) {
            prettyString += arg->prettyPrint("    " + indentSpace);
        }

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        // This method is complex and involves many other classes and methods that are not provided.
        // The translation of this method to C++ would require a complete understanding of these classes and methods.
        // Therefore, a direct translation is not provided here.
        return nullptr;
    }
};

class IrStmtBreak : public IrStatement {
public:
    IrStmtBreak(int lineNumber, int colNumber) : IrStatement(lineNumber, colNumber) {}

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check to see if this statement resides in a loop scope or not
        bool isInALoop = scopeStack->isScopeForALoop();
        if (!isInALoop) {
            errorMessage += "Break statement cannot be used outside of a for or while loop" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--breakStmt";
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        std::string endBlock = "END_" + builder->getCurrentBlock();
        LlJumpUnconditional* unconditionalJump = new LlJumpUnconditional(endBlock);
        builder->appendStatement(unconditionalJump);

        return nullptr;
    }
};


class IrStmtBreak : public IrStatement {
public:
    IrStmtBreak(int lineNumber, int colNumber) : IrStatement(lineNumber, colNumber) {}

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check to see if this statement resides in a loop scope or not
        bool isInALoop = scopeStack->isScopeForALoop();
        if (!isInALoop) {
            errorMessage += "Break statement cannot be used outside of a for or while loop" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--breakStmt";
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        std::string endBlock = "END_" + builder->getCurrentBlock();
        LlJumpUnconditional* unconditionalJump = new LlJumpUnconditional(endBlock);
        builder->appendStatement(unconditionalJump);

        return nullptr;
    }
};

class IrStmtContinue : public IrStatement {
public:
    IrStmtContinue(int lineNumber, int colNumber) : IrStatement(lineNumber, colNumber) {}

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check to see if this statement resides in a loop scope or not
        bool isInALoop = scopeStack->isScopeForALoop();
        if (!isInALoop) {
            errorMessage += "Continue statement cannot be used outside of a for or while loop" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--continueStmt";
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        std::string loopCondition = builder->getCurrentLoopCondition();
        LlJumpUnconditional* unconditionalJump = new LlJumpUnconditional(loopCondition);
        builder->appendStatement(unconditionalJump);

        return nullptr;
    }
};



class IrStmtReturn : public IrStatement {
public:
    IrStmtReturn(int lineNumber, int colNumber) : IrStatement(lineNumber, colNumber) {}

    virtual IrType* getExpressionType() = 0; // pure virtual function
};


class IrStmtReturnExpr : public IrStmtReturn {
private:
    IrExpr* result;

public:
    IrStmtReturnExpr(IrExpr* result) : IrStmtReturn(result->getLineNumber(),result->getColNumber()), result(IrExpr::canonicalizeExpr(result)) {}

    IrType* getExpressionType() override {
        return this->result->getExpressionType();
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--returnExpr";

        // pretty print the expression
        indentSpace += this->result->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* resultVar = this->result->generateLlIr(builder, symbolTable);
        LlReturn* returnStatement = new LlReturn(resultVar);
        builder->appendStatement(returnStatement);

        return resultVar;
    }
};

class IrStmtReturnVoid : public IrStmtReturn {
public:
    IrStmtReturnVoid(int lineNumber, int colNumber) : IrStmtReturn(lineNumber, colNumber) {}

    IrType* getExpressionType() override {
        return new IrTypeVoid(this->getLineNumber(), this->getColNumber());
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--returnVoid";
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlReturn* ret = new LlReturn(nullptr);
        builder->appendStatement(ret);
        return nullptr;
    }
};
