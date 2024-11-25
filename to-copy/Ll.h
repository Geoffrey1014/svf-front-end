#include <string>
#include <iostream>
#include <vector>

class Ll{
private:
    /* data */
public:
    Ll (/* args */);
    ~Ll();
    virtual std::string toString()=0;
};

class LlStatement: public Ll{
private:
    /* data */
public:
    LlStatement (/* args */);
    ~LlStatement ();
    std::string toString();
};

class LlEmptyStmt : public LlStatement {
public:
    std::string toString() {
        return "EMPTY_STATEMENT";
    }

    // ... equals and hashCode methods

};

class LlComponent: public Ll{
private:
    /* data */
public:
    LlComponent (/* args */);
    ~LlComponent ();
    std::string toString();
};

class LlLiteral: public LlComponent{
public:
    LlLiteral (/* args */);
    ~LlLiteral ();
    std::string toString();
};

class LlLocation: public LlComponent{
private:
    std::string varName;
public:
    LlLocation (std::string varName): varName(varName){};
    ~LlLocation ();
    std::string getVarName();
    std::string toString();
};

class LlAssignStmt : public LlStatement {
protected:
    LlLocation storeLocation;

public:
    LlAssignStmt(LlLocation storeLocation) : storeLocation(storeLocation) {}

    LlLocation getStoreLocation() {
        return this->storeLocation;
    }
};


class LlAssignStmtBinaryOp : public LlAssignStmt {
private:
    LlComponent leftOperand;
    std::string operation;
    LlComponent rightOperand;

public:
    LlAssignStmtBinaryOp(LlLocation storeLocation, LlComponent leftOperand, std::string operation, LlComponent rightOperand)
        : LlAssignStmt(storeLocation), leftOperand(leftOperand), operation(operation), rightOperand(rightOperand) {}

    LlComponent getLeftOperand() {
        return this->leftOperand;
    }

    std::string getOperation() {
        return this->operation;
    }

    LlComponent getRightOperand() {
        return this->rightOperand;
    }

    std::string toString() {
        return this->storeLocation.toString() + " = " + this->leftOperand.toString() + " " + this->operation  + " " + this->rightOperand.toString();
    }

};

class LlAssignStmtUnaryOp : public LlAssignStmt {
private:
    LlComponent operand;
    std::string operator_;

public:
    LlAssignStmtUnaryOp(LlLocation storeLocation, LlComponent operand, std::string operator_)
        : LlAssignStmt(storeLocation), operand(operand), operator_(operator_) {}

    LlComponent getOperand() {
        return this->operand;
    }

    std::string getOperator() {
        return this->operator_;
    }

    std::string toString() {
        return this->storeLocation.toString() + " = " + operator_ + " " + operand.toString();
    }
};

class LlJump : public LlStatement {
protected:
    std::string jumpToLabel;

public:
    LlJump(std::string jumpToLabel) : jumpToLabel(jumpToLabel) {}

    std::string getJumpToLabel() {
        return jumpToLabel;
    }
};


class LlJumpConditional : public LlJump {
private:
    LlComponent condition;

public:
    LlJumpConditional(std::string jumpToLabel, LlComponent condition) 
        : LlJump(jumpToLabel), condition(condition) {}

    LlComponent getCondition() {
        return this->condition;
    }

    std::string toString() {
        return "if " + condition.toString() + " goto " + this->jumpToLabel;
    }

    // ... equals and hashCode methods 
};

#include <string>

class LlJumpUnconditional : public LlJump {
public:
    LlJumpUnconditional(std::string jumpToLabel) : LlJump(jumpToLabel) {}

    std::string toString() {
        return "goto " + this->jumpToLabel;
    }

    // ... equals and hashCode methods 
};


class LlLiteralBool : public LlLiteral {
private:
    bool boolValue;

public:
    LlLiteralBool(bool boolValue) : boolValue(boolValue) {}

    bool getBoolValue() {
        return this->boolValue;
    }

    std::string toString() {
        return this->boolValue ? "true" : "false";
    }

    // ... equals and hashCode methods 
};


class LlLiteralInt : public LlLiteral {
private:
    long intValue;

public:
    LlLiteralInt(long intValue) : intValue(intValue) {}

    long getIntValue() {
        return this->intValue;
    }

    std::string toString() {
        return std::to_string(this->intValue);
    }

    // ... equals and hashCode methods 
};


class LlLocationArray : public LlLocation {
private:
    LlComponent elementIndex;

public:
    LlLocationArray(std::string varName, LlComponent elementIndex) : LlLocation(varName), elementIndex(elementIndex) {}

    LlComponent getElementIndex() {
        return this->elementIndex;
    }

    std::string toString() {
        return this->getVarName() + "[" + elementIndex.toString() + "] ";
    }

    // ... equals and hashCode methods.

    std::string getArrayHead(std::string arrayLocation) {
        int arrayHead = std::stoi(arrayLocation.substr(0, arrayLocation.length() - 6)) / 8;
        return std::to_string(arrayHead);
    }
};

class LlLocationVar : public LlLocation {
public:
    LlLocationVar(std::string varName) : LlLocation(varName) {}

    std::string toString() {
        return this->getVarName();
    }

    // ... equals and hashCode methods are not typically overridden in C++ ...

    bool isStringLoc() {
        return this->getVarName().find("str") != std::string::npos;
    }
};

class LlMethodCallStmt : public LlStatement {
private:
    LlLocation returnLocation;
    std::string methodName;
    std::vector<LlComponent> argsList;

public:
    LlMethodCallStmt(std::string methodName, std::vector<LlComponent> argsList, LlLocation returnLocation) 
        : methodName(methodName), argsList(argsList), returnLocation(returnLocation) {}

    std::string getMethodName() {
        return this->methodName;
    }

    LlLocation getReturnLocation() {
        return this->returnLocation;
    }

    std::vector<LlComponent> getArgsList() {
        return this->argsList;  // Return a copy of the vector
    }

    std::string toString() {
        std::string argsString = "";
        for(auto &arg : argsList){
            argsString +=  arg.toString() +",";
        }
        return this->returnLocation.toString() + " = " + this->methodName + "(" + argsString +")" ;
    }

    // ... equals and hashCode methods are not typically overridden in C++ ...
};

#include <string>

class LlParallelMethodStmt : public LlStatement {
private:
    std::string parallelMethodName;

public:
    LlParallelMethodStmt(std::string methodName) : parallelMethodName(methodName) {}

    std::string toString() {
        return "create_and_run_threads(" + this->parallelMethodName + ")";
    }
};


#include <string>

class LlReturn : public LlStatement {
private:
    LlComponent* returnValue;

public:
    LlReturn(LlComponent* returnValue) : returnValue(returnValue) {}

    LlComponent* getReturnValue() {
        return this->returnValue;
    }

    std::string toString() {
        if (this->returnValue == nullptr) {
            return "return ";
        }
        return "return " + this->returnValue->toString();
    }

    // ... equals and hashCode methods 
};

