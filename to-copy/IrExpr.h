#include "Ir.h"

class IrExpr : public Ir {
public:
    IrExpr(int lineNumber, int colNumber) : Ir(lineNumber, colNumber) {}
    virtual IrType* getExpressionType() = 0; // This is an abstract method in C++
    std::string toString() {
        return "IrExpr";
    }

    // returns an expression that is in the standard
    // canonical form according to the 20 rules found
    // the whale book
    static IrExpr* canonicalizeExpr(IrExpr* expr) {
        // This method is quite complex and relies on a number of other classes
        // that haven't been provided. I'll leave it as a placeholder for now.
        // You'll need to translate this method based on the actual definitions
        // of the other classes it uses.
        return expr;
    }

    bool operator==(const IrExpr& other) const {
        // Implementation of operator==
        return true;
    }

        int hashCode() const {
        // Implementation of hashCode
        return 0;
    }
};

class IrOper : public IrExpr {
public:
    IrOper(int lineNumber, int colNumber) : IrExpr(lineNumber, colNumber) {}

    std::string toString() {
        return "IrOper";
    }

};


class IrOperBinary : public IrOper {
private:
    std::string operation;
    IrExpr* leftOperand;
    IrExpr* rightOperand;

public:
    IrOperBinary(std::string operation, IrExpr* leftOperand, IrExpr* rightOperand) 
        : IrOper(leftOperand->getLineNumber(), leftOperand->getColNumber()), operation(operation), 
          leftOperand(IrExpr::canonicalizeExpr(leftOperand)), rightOperand(IrExpr::canonicalizeExpr(rightOperand)) {}

    IrExpr* getLeftOperand() {
        return this->leftOperand;
    }

    IrExpr* getRightOperand() {
        return this->rightOperand;
    }

    std::string getOperation() {
        return this->operation;
    }

    std::string toString() {
        return leftOperand->toString() + " " + operation + " " + rightOperand->toString();
    }

};

class IrOperBinaryArith : public IrOperBinary {
public:
    IrOperBinaryArith(std::string operation, IrExpr* leftOperand, IrExpr* rightOperand) 
        : IrOperBinary(operation, leftOperand, rightOperand) {}

    IrType* getExpressionType() override {
        return new IrTypeInt(this->getLeftOperand()->getLineNumber(), this->getLeftOperand()->getColNumber());
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check that rhs and lhs are valid
        errorMessage += this->getRightOperand()->semanticCheck(scopeStack);
        errorMessage += this->getLeftOperand()->semanticCheck(scopeStack);

        // 2) verify that both lhs and rhs are IrType int
        if (!(dynamic_cast<IrTypeInt*>(this->getRightOperand()->getExpressionType()) 
            && dynamic_cast<IrTypeInt*>(this->getLeftOperand()->getExpressionType()))) {
            errorMessage += "The lhs and rhs of an arithmetic expression must be of type int" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--binaryArithOper\n";

        // pretty print the lhs
        prettyString += "  " + indentSpace + "|--lhs\n";
        prettyString += this->getLeftOperand()->prettyPrint("    " + indentSpace);

        // print the operator
        prettyString += "  " + indentSpace + "|--op: " + this->getOperation() + "\n";

        // pretty print the rhs
        prettyString += "  " + indentSpace + "|--rhs\n";
        prettyString += this->getRightOperand()->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* leftTemp = this->getLeftOperand()->generateLlIr(builder, symbolTable);
        LlLocation* rightTemp = this->getRightOperand()->generateLlIr(builder, symbolTable);
        LlLocationVar* returnTemp = builder->generateTemp();
        LlAssignStmtBinaryOp* assignStmtBinaryOp = new LlAssignStmtBinaryOp(returnTemp, leftTemp, this->getOperation(), rightTemp);
        builder->appendStatement(assignStmtBinaryOp);
        return returnTemp;
    }
};

class IrOperBinaryCond : public IrOperBinary {
public:
    IrOperBinaryCond(std::string operation, IrExpr* leftOperand, IrExpr* rightOperand) 
        : IrOperBinary(operation, leftOperand, rightOperand) {}

    IrType* getExpressionType() override {
        return new IrTypeBool(this->getLeftOperand()->getLineNumber(), this->getLeftOperand()->getColNumber());
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check that rhs and lhs are valid
        errorMessage += this->getRightOperand()->semanticCheck(scopeStack);
        errorMessage += this->getLeftOperand()->semanticCheck(scopeStack);

        // 2) verify that both lhs and rhs are IrTypeBool
        if (!(dynamic_cast<IrTypeBool*>(this->getRightOperand()->getExpressionType()) 
            && dynamic_cast<IrTypeBool*>(this->getLeftOperand()->getExpressionType()))) {
            errorMessage += "The lhs and rhs of an conditional expression must be of type bool" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string getCommand(std::string operation) {
        std::string retCommand = "";
        if (operation == "&&") {
            retCommand = "and ";
        } else if (operation == "||") {
            retCommand = "or ";
        } else {
            std::cerr << "Runtime Error: Unrecognized Operation";
        }
        return retCommand;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--binaryCondOper\n";

        // pretty print the lhs
        prettyString += "  " + indentSpace + "|--lhs\n";
        prettyString += this->getLeftOperand()->prettyPrint("    " + indentSpace);

        // print the operator
        prettyString += "  " + indentSpace + "|--op: " + this->getOperation() + "\n";

        // pretty print the rhs
        prettyString += "  " + indentSpace + "|--rhs\n";
        prettyString += this->getRightOperand()->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* leftTemp = this->getLeftOperand()->generateLlIr(builder, symbolTable);
        LlLocation* rightTemp = this->getRightOperand()->generateLlIr(builder, symbolTable);
        LlLocationVar* returnTemp = builder->generateTemp();
        LlAssignStmtBinaryOp* assignStmtBinaryOp = new LlAssignStmtBinaryOp(returnTemp, leftTemp, this->getOperation(), rightTemp);
        builder->appendStatement(assignStmtBinaryOp);
        return returnTemp;
    }

    // Other methods specific to IrOperBinaryCond...
};


class IrOperBinaryEq : public IrOperBinary {
public:
    IrOperBinaryEq(std::string operation, IrExpr* leftOperand, IrExpr* rightOperand) 
        : IrOperBinary(operation, leftOperand, rightOperand) {}

    IrType* getExpressionType() override {
        return new IrTypeBool(this->getLeftOperand()->getLineNumber(), this->getLeftOperand()->getColNumber());
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check that rhs and lhs are valid
        errorMessage += this->getRightOperand()->semanticCheck(scopeStack);
        errorMessage += this->getLeftOperand()->semanticCheck(scopeStack);

        // 2) verify that both lhs and rhs are either IrTypeInt or IrTypeBool
        bool bothAreInts = dynamic_cast<IrTypeInt*>(this->getRightOperand()->getExpressionType()) 
            && dynamic_cast<IrTypeInt*>(this->getLeftOperand()->getExpressionType());
        bool bothAreBools = dynamic_cast<IrTypeBool*>(this->getRightOperand()->getExpressionType()) 
            && dynamic_cast<IrTypeBool*>(this->getLeftOperand()->getExpressionType());
        if (!bothAreBools && !bothAreInts) {
            errorMessage += "The lhs and rhs of an equator operation must both be ints or both be bools" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string getMoveCommand(std::string operation) {
        std::string retCommand = "";
        if (operation == "!=") {
            retCommand = "cmovne ";
        } else if (operation == "==") {
            retCommand = "cmove ";
        }
        return retCommand;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--binaryEquateOper\n";

        // pretty print the lhs
        prettyString += "  " + indentSpace + "|--lhs\n";
        prettyString += this->getLeftOperand()->prettyPrint("    " + indentSpace);

        // print the operator
        prettyString += "  " + indentSpace + "|--op: " + this->getOperation() + "\n";

        // pretty print the rhs
        prettyString += "  " + indentSpace + "|--rhs\n";
        prettyString += this->getRightOperand()->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* rightTemp = this->getRightOperand()->generateLlIr(builder, symbolTable);
        LlLocation* leftTemp = this->getLeftOperand()->generateLlIr(builder, symbolTable);
        LlLocationVar* returnTemp = builder->generateTemp();
        LlAssignStmtBinaryOp* assignStmtBinaryOp = new LlAssignStmtBinaryOp(returnTemp, leftTemp, this->getOperation(), rightTemp);
        builder->appendStatement(assignStmtBinaryOp);
        return returnTemp;
    }

    // Other methods specific to IrOperBinaryEq...
};


class IrOperBinaryRel : public IrOperBinary {
public:
    IrOperBinaryRel(std::string operation, IrExpr* leftOperand, IrExpr* rightOperand) 
        : IrOperBinary(operation, leftOperand, rightOperand) {}

    IrType* getExpressionType() override {
        return new IrTypeBool(this->getLeftOperand()->getLineNumber(), this->getLeftOperand()->getColNumber());
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check that rhs and lhs are valid
        errorMessage += this->getRightOperand()->semanticCheck(scopeStack);
        errorMessage += this->getLeftOperand()->semanticCheck(scopeStack);

        // 2) verify that both lhs and rhs are IrTypeInt
        if (!(dynamic_cast<IrTypeInt*>(this->getRightOperand()->getExpressionType()) 
            && dynamic_cast<IrTypeInt*>(this->getLeftOperand()->getExpressionType()))) {
            errorMessage += "The lhs and rhs of a relational expression must be of type int" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|__binaryRelOper\n";

        // pretty print the lhs
        prettyString += "  " + indentSpace + "|__lhs\n";
        prettyString += this->getLeftOperand()->prettyPrint("    " + indentSpace);

        // print the operator
        prettyString += "  " + indentSpace + "|__op: " + this->getOperation() + "\n";

        // pretty print the rhs
        prettyString += "  " + indentSpace + "|__rhs\n";
        prettyString += this->getRightOperand()->prettyPrint("    " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* rightTemp = this->getRightOperand()->generateLlIr(builder, symbolTable);
        LlLocation* leftTemp = this->getLeftOperand()->generateLlIr(builder, symbolTable);
        LlLocationVar* returnTemp = builder->generateTemp();
        LlAssignStmtBinaryOp* assignStmtBinaryOp = new LlAssignStmtBinaryOp(returnTemp, leftTemp, this->getOperation(), rightTemp);
        builder->appendStatement(assignStmtBinaryOp);
        return returnTemp;
    }

    // Other methods specific to IrOperBinaryRel...
};

class IrOperUnary : public IrOper {
protected:
    IrExpr* operand;

public:
    IrOperUnary(IrExpr* operand)
        : IrOper(operand->getLineNumber(), operand->getColNumber()), operand(IrExpr::canonicalizeExpr(operand)) {}

    // Other methods specific to IrOperUnary...

    // Destructor
    virtual ~IrOperUnary() {
        delete operand;
    }
};

class IrOperUnaryNeg : public IrOperUnary {
public:
    IrOperUnaryNeg(IrExpr* operand) : IrOperUnary(operand) {}

    IrType* getExpressionType() override {
        return new IrTypeInt(this->operand->getLineNumber(), this->operand->getColNumber());
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check that the operand is valid
        errorMessage += this->operand->semanticCheck(scopeStack);

        // 2) verify that the operand is IrTypeInt
        if (!(dynamic_cast<IrTypeInt*>(this->operand->getExpressionType()))) {
            errorMessage += "The negation '-' operand must be used on an int" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--unaryNegateOp\n";

        // pretty print the operand
        prettyString += this->operand->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* operandTemp = this->operand->generateLlIr(builder, symbolTable);

        LlLocationVar* returnTemp = builder->generateTemp();
        LlAssignStmtUnaryOp* unaryOp = new LlAssignStmtUnaryOp(returnTemp, operandTemp, "-");
        builder->appendStatement(unaryOp);
        return returnTemp;
    }

    // Other methods specific to IrOperUnaryNeg...
};


class IrOperUnaryNot : public IrOperUnary {
public:
    IrOperUnaryNot(IrExpr* operand) : IrOperUnary(operand) {}

    IrType* getExpressionType() override {
        return new IrTypeBool(this->operand->getLineNumber(), this->operand->getColNumber());
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // 1) check that the operand is valid
        errorMessage += this->operand->semanticCheck(scopeStack);

        // 2) verify that the operand is IrTypeBool
        if (!(dynamic_cast<IrTypeBool*>(this->operand->getExpressionType()))) {
            errorMessage += "The not '!' operand must be used on a bool" +
                    " line: "+std::to_string(this->getLineNumber()) + " col: " +std::to_string(this->getColNumber()) + "\n";
        }

        return errorMessage;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--unaryNotOp\n";

        // pretty print the operand
        prettyString += this->operand->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocation* operandTemp = this->operand->generateLlIr(builder, symbolTable);

        LlLocationVar* returnTemp = builder->generateTemp();
        LlAssignStmtUnaryOp* unaryOp = new LlAssignStmtUnaryOp(returnTemp, operandTemp, "!");
        builder->appendStatement(unaryOp);
        return returnTemp;
    }

    // Other methods specific to IrOperUnaryNot...
};

class IrSizeOf : public IrExpr {
public:
    IrSizeOf(int lineNumber, int colNumber) : IrExpr(lineNumber, colNumber) {}

    // Other methods specific to IrSizeOf...
};

class IrSizeOfLocation : public IrSizeOf {
private:
    IrIdent* fieldName;
    Ir* irDecl; // IrFieldDeclArray, IrFieldDeclVar, IrParamDecl

public:
    IrSizeOfLocation(IrIdent* fieldName, int lineNumber, int colNumber) : IrSizeOf(lineNumber, colNumber), fieldName(fieldName) {}

    IrType* getExpressionType() override {
        return new IrTypeInt(this->fieldName->getLineNumber(), this->fieldName->getColNumber());
    }

    // Other methods specific to IrSizeOfLocation...
};

class IrSizeOfType : public IrSizeOf {
private:
    IrType* type;

public:
    IrSizeOfType(IrType* type) : IrSizeOf(type->getLineNumber(), type->getColNumber()), type(type) {}

    IrType* getExpressionType() override {
        return this->type;
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        return "";
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--sizeOfType\n";

        // pretty print the location
        prettyString += this->type->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        // Generate a type and store the size in it.
        LlLocationVar* tempLocation = builder->generateTemp();

        if(dynamic_cast<IrTypeInt*>(type)){
            LlAssignStmtRegular* regularAssignment;
            regularAssignment = new LlAssignStmtRegular(tempLocation, new LlLiteralInt(8));
            builder->appendStatement(regularAssignment);
        }

        if(dynamic_cast<IrTypeBool*>(type)){
            LlAssignStmtRegular* regularAssignment;
            regularAssignment = new LlAssignStmtRegular(tempLocation, new LlLiteralInt(1));
            builder->appendStatement(regularAssignment);
        }
        return tempLocation;
    }
};


class IrMethodCallExpr : public IrExpr {
private:
    IrIdent* methodName;
    std::vector<IrArg*> argsList;
    IrType* methodType;

public:
    IrMethodCallExpr(IrIdent* methodName, std::vector<IrArg*> argsList) : IrExpr(methodName->getLineNumber(), methodName->getColNumber()), methodName(methodName), argsList(argsList) {}

    IrType* getExpressionType() override {
        return this->methodType;
    }

    std::string toString() {
        std::string argsListString;
        for (auto& arg : argsList) {
            argsListString += arg->toString() + ", ";
        }
        argsListString = argsListString.substr(0, argsListString.length() - 2);  // Remove trailing ", "
        return this->methodName->getValue() + "(" + argsListString + ")";
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--methodCallExpr\n";

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
        std::vector<LlComponent*> argsList;
        for(IrArg* arg : this->argsList){
            argsList.push_back(arg->generateLlIr(builder, symbolTable));
        }
        LlLocationVar* returnLocation = builder->generateTemp();
        LlMethodCallStmt* methodCallStmt = new LlMethodCallStmt(this->methodName->getValue(), argsList, returnLocation);
        builder->appendStatement(methodCallStmt);
        return returnLocation;
    }


};

