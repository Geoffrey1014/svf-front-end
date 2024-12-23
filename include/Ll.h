#ifndef LL_H
#define LL_H

#include <string>
#include <iostream>
#include <vector>

class Ll{
private:
    /* data */
public:
    Ll (){};
    ~Ll()=default;
    virtual std::string toString()=0;
};

class LlStatement: public Ll{
private:
    /* data */
public:
    LlStatement (){};
    ~LlStatement ()=default;
    virtual std::string toString();
    virtual bool operator==(const LlStatement& other) const;
    virtual std::size_t hashCode() const;
};

class LlEmptyStmt : public LlStatement {
public:
    std::string toString() {
        return "EMPTY_STATEMENT";
    }

   bool operator==(const LlEmptyStmt& other) const {
        if (dynamic_cast<const LlEmptyStmt*>(&other) == nullptr) 
            return false;
        else
            return true;
    }

    std::size_t hashCode() const {
        return 0;
    }

};

class LlComponent: public Ll{
private:
    /* data */
public:
    LlComponent (){};
    ~LlComponent ()=default;
    virtual std::string toString();
    virtual bool operator==(const LlComponent& other) const;
    virtual std::size_t hashCode() const;
};

class LlLiteral: public LlComponent{
public:
    LlLiteral (/* args */);
    ~LlLiteral ();
    std::string toString() override;
    bool operator==(const LlLiteral& other) const;
    std::size_t hashCode() const override;
};

class LlLocation: public LlComponent{
private:
    const std::string* varName;
public:
    LlLocation (const std::string* varName): varName(varName){};
    ~LlLocation (){};
    const std::string* getVarName() const;
    std::string toString() override{
        return *(this->varName);
    };
    virtual bool operator==(const LlLocation& other) const;
    virtual std::size_t hashCode() const override;
};

class LlAssignStmt : public LlStatement {
protected:
    LlLocation storeLocation;

public:
    LlAssignStmt(LlLocation storeLocation) : storeLocation(storeLocation) {}

    LlLocation getStoreLocation() {
        return this->storeLocation;
    }
    std::string toString() override{
        return this->storeLocation.toString() + " = ";
    }
    bool operator==(const LlAssignStmt& other) const;
    std::size_t hashCode() const override;
};


class LlAssignStmtBinaryOp : public LlAssignStmt {
private:
    LlComponent leftOperand;
    std::string* operation;
    LlComponent rightOperand;

public:
    LlAssignStmtBinaryOp(LlLocation storeLocation, LlComponent leftOperand, std::string* operation, LlComponent rightOperand)
        : LlAssignStmt(storeLocation), leftOperand(leftOperand), operation(operation), rightOperand(rightOperand) {}

    LlComponent getLeftOperand() {
        return this->leftOperand;
    }

    std::string* getOperation() {
        return this->operation;
    }

    LlComponent getRightOperand() {
        return this->rightOperand;
    }

    std::string toString() override{
        return this->storeLocation.toString() + " = " + this->leftOperand.toString() + " " + *(this->operation)  + " " + this->rightOperand.toString();
    }

    bool operator==(const LlAssignStmtBinaryOp& other) const;
    std::size_t hashCode() const override;

};

class LlAssignStmtUnaryOp : public LlAssignStmt {
private:
    LlComponent operand;
    std::string* operator_;

public:
    LlAssignStmtUnaryOp(LlLocation storeLocation, LlComponent operand, std::string* operator_)
        : LlAssignStmt(storeLocation), operand(operand), operator_(operator_) {}

    LlComponent getOperand() {
        return this->operand;
    }

    std::string* getOperator() {
        return this->operator_;
    }

    std::string toString() override{
        return this->storeLocation.toString() + " = " + *(operator_) + " " + operand.toString();
    }

    bool operator==(const LlAssignStmtUnaryOp& other) const;
    std::size_t hashCode() const override;
};

class LlJump : public LlStatement {
protected:
    std::string* jumpToLabel;

public:
    LlJump(std::string* jumpToLabel) : jumpToLabel(jumpToLabel) {}

    std::string* getJumpToLabel() {
        return jumpToLabel;
    }
    
    std::string toString() override{
        return "goto " + *(this->jumpToLabel);
    }
    bool operator==(const LlJump& other) const;
    std::size_t hashCode() const override;
};


class LlJumpConditional : public LlJump {
private:
    LlComponent condition;

public:
    LlJumpConditional(std::string* jumpToLabel, LlComponent condition) 
        : LlJump(jumpToLabel), condition(condition) {}

    LlComponent getCondition() {
        return this->condition;
    }

    std::string toString() override{
        return "if " + condition.toString() + " goto " + *(this->jumpToLabel);
    }

    bool operator==(const LlJumpConditional& other) const;
    std::size_t hashCode() const override;
};

class LlJumpUnconditional : public LlJump {
public:
    LlJumpUnconditional(std::string* jumpToLabel) : LlJump(jumpToLabel) {}

    std::string toString() override{
        return "goto " + *(this->jumpToLabel);
    }

    bool operator==(const LlJumpUnconditional& other) const;
    std::size_t hashCode() const override;
};


class LlLiteralBool : public LlLiteral {
private:
    bool boolValue;

public:
    LlLiteralBool(bool boolValue) : boolValue(boolValue) {}

    bool getBoolValue() {
        return this->boolValue;
    }

    std::string toString() override{
        return this->boolValue ? "true" : "false";
    }

    bool operator==(const LlLiteralBool& other) const;
    std::size_t hashCode() const override;
};


class LlLiteralInt : public LlLiteral {
private:
    long intValue;

public:
    LlLiteralInt(long intValue) : intValue(intValue) {}

    long getIntValue() {
        return this->intValue;
    }

    std::string toString() override{
        return std::to_string(this->intValue);
    }

    bool operator==(const LlLiteralInt& other) const;
    std::size_t hashCode() const override;
};


class LlLocationArray : public LlLocation {
private:
    LlComponent elementIndex;

public:
    LlLocationArray(std::string* varName, LlComponent elementIndex) : LlLocation(varName), elementIndex(elementIndex) {}

    LlComponent getElementIndex() {
        return this->elementIndex;
    }

    std::string toString() override{
        return *(this->getVarName()) + "[" + elementIndex.toString() + "] ";
    }

    bool operator==(const LlLocationArray& other) const;
    std::size_t hashCode() const override;

    std::string getArrayHead(std::string arrayLocation) {
        int arrayHead = std::stoi(arrayLocation.substr(0, arrayLocation.length() - 6)) / 8;
        return std::to_string(arrayHead);
    }
};

class LlLocationVar : public LlLocation {
public:
    LlLocationVar(const std::string* varName) : LlLocation(varName) {}

    std::string toString() override{
        return *(this->getVarName());
    }

    bool operator==(const LlLocationVar& other) const {
        if (&other == this) {
            return true;
        }
        else if (dynamic_cast<const LlLocationVar*>(&other) == nullptr) {
            return false;
        }
        return *(other.getVarName()) == *(this->getVarName());
    }

    std::size_t hashCode() const override{
        std::hash<std::string> hasher;
        return hasher(*(this->getVarName()));
    }

    bool isStringLoc() {
        return this->getVarName()->find("str") != std::string::npos;
    }
};

// Specialize std::hash for LlComponent
namespace std {
    template <>
    struct hash<LlComponent> {
        std::size_t operator()(const LlComponent& k) const {
            return k.hashCode();
        }
    };
    
    template <>
    struct hash<LlLocationVar> {
        std::size_t operator()(const LlLocationVar& k) const {
            return  k.hashCode();
        }
    };
}

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

    bool operator==(const LlMethodCallStmt& other) const;
    std::size_t hashCode() const;
};


class LlParallelMethodStmt : public LlStatement {
private:
    std::string parallelMethodName;

public:
    LlParallelMethodStmt(std::string methodName) : parallelMethodName(methodName) {}

    std::string toString() {
        return "create_and_run_threads(" + this->parallelMethodName + ")";
    }

    bool operator==(const LlParallelMethodStmt& other) const;
    std::size_t hashCode() const;
};


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

    bool operator==(const LlReturn& other) const;
    std::size_t hashCode() const;
};

#endif