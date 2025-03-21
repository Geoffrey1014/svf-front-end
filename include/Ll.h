#ifndef LL_H
#define LL_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "BasicBlock.h"

class Ll{
public:
    Ll (){};
    virtual ~Ll()=default;
    virtual std::string toString() const = 0;
    virtual bool operator==(const Ll& other) const = 0;
    virtual std::size_t hashCode() const = 0;
};

class LlStatement : public Ll{
protected:
    std::string* definedVar;
    std::vector<std::string*> usedVars;
    bool isJumpInst;
    bool isCondJump;
    std::string* jumpLabel;

public:
    LlStatement() : definedVar(nullptr), isJumpInst(false), isCondJump(false), jumpLabel(nullptr) {}
    virtual ~LlStatement() {
        delete definedVar;
        delete jumpLabel;
        for(auto var : usedVars) {
            delete var;
        }
    }

    std::string* getDefinedVariable() const { return definedVar; }
    const std::vector<std::string*>& getUsedVariables() const { return usedVars; }
    bool isJump() const { return isJumpInst; }
    bool isConditionalJump() const { return isCondJump; }
    std::string* getJumpToLabel() const { return jumpLabel; }

    void renameUse(const std::string& oldName, const std::string& newName) {
        for(auto var : usedVars) {
            if(*var == oldName) *var = newName;
            else std::cerr << "Error: oldName variable " << oldName << " not found in used variables\n";
        }
    }

    void renameDef(const std::string& oldName, const std::string& newName) {
        if(definedVar && *definedVar == oldName) *definedVar = newName;
        else std::cerr << "Error: oldName variable " << oldName << " not found in used definedVar\n";
    }
};

class BasicBlock;
// Phi function statement
class LlPhiStatement : public LlStatement {
private:
    std::vector<std::string*> incomingVars;
    std::vector<BasicBlock*> incomingBlocks;

public:
    LlPhiStatement(const std::string& var) {
        definedVar = new std::string(var);

    }

    void setIncoming(std::string* var, BasicBlock* block) {
        incomingVars.push_back(var);
        incomingBlocks.push_back(block);
    }
    std::string toString() const override{
        std::ostringstream ss;
    ss << *definedVar << " = phi [";
    for(size_t i = 0; i < incomingVars.size(); ++i) {
        if(i > 0) ss << ", ";
        ss << *incomingVars[i] << " from " << incomingBlocks[i]->getLabel();
    }
    ss << "]";
    return ss.str();
    }

    bool operator==(const Ll& other) const override {
        if (&other == this) {
            return true;
        }
        if (auto otherPhi = dynamic_cast<const LlPhiStatement*>(&other)) {
            if (*definedVar != *otherPhi->definedVar) {
                return false;
            }
            if (incomingVars.size() != otherPhi->incomingVars.size()) {
                return false;
            }
            for (size_t i = 0; i < incomingVars.size(); i++) {
                if (*incomingVars[i] != *otherPhi->incomingVars[i]) {
                    return false;
                }
                if (incomingBlocks[i] != otherPhi->incomingBlocks[i]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    std::size_t hashCode() const override {
        std::size_t hash = std::hash<std::string>()(*definedVar);
        for (size_t i = 0; i < incomingVars.size(); i++) {
            hash ^= std::hash<std::string>()(*incomingVars[i]);
            hash ^= std::hash<BasicBlock*>()(incomingBlocks[i]);
        }
        return hash;
    }
};


class LlComponent: public Ll{
    public:
        LlComponent ()= default;
        ~LlComponent () override =default;
        std::string toString() const override{
            return "LlComponent";
        }
        bool operator==(const Ll& other) const override{
            return this == &other;
        }
        std::size_t hashCode() const override{
            return 3;
        }
};

class LlLiteral: public LlComponent{
    public:
        LlLiteral ()= default;
        ~LlLiteral () override =default;
        std::string toString() const override{
            return "LlLiteral";
        }

};

class LlEmptyStmt : public LlStatement {
public:
    LlEmptyStmt() = default;
    ~LlEmptyStmt() override = default;
    std::string toString() const override{
        return "EMPTY_STATEMENT";
    }

   bool operator==(const Ll& other) const override{
        if (dynamic_cast<const LlEmptyStmt*>(&other) == nullptr)
            return false;
        else
            return true;
    }

    std::size_t hashCode() const override{
        return 5;
    }

};

class LlLocation: public LlComponent{
private:
    std::string* varName;
public:
    LlLocation (std::string* varName): varName(varName){};
    ~LlLocation () override{delete varName;};

    std::string toString() const override{
        return *(this->varName);
    };

    std::string* getVarName() const {
        return varName;
    }

    bool operator==(const Ll& other) const override{
        if (dynamic_cast<const LlLocation*>(&other) == nullptr)
            return false;
        else
            return *varName == *dynamic_cast<const LlLocation*>(&other)->varName;
    }

    std::size_t hashCode() const override{
        return std::hash<std::string>()(*varName);
    }
};

class LlLocationDeref : public LlLocation {
    private:
        LlLocation* base;
    public:
    LlLocationDeref(LlLocation* base) : LlLocation(base->getVarName()), base(base) {}
    ~LlLocationDeref() override {
        delete base;
    }

    std::string toString() const override {
        return "*" + base->toString();
    }

    LlLocation* getBase() {
        return base;
    }
};

class LlAssignStmt : public LlStatement {
protected:
    LlLocation* storeLocation;

public:
    LlAssignStmt(LlLocation* storeLocation) : storeLocation(storeLocation) {
        definedVar = storeLocation->getVarName();
    }
    virtual ~LlAssignStmt() {
        delete storeLocation;
    }

    LlLocation* getStoreLocation() {
        return this->storeLocation;
    }
    std::string toString() const override{
        return this->storeLocation->toString() + " = ";
    }
};


class LlAssignStmtRegular : public LlAssignStmt {
private:
    LlComponent* rightHandSide;

public:
    LlAssignStmtRegular(LlLocation* storeLocation, LlComponent* rightHandSide) : LlAssignStmt(storeLocation), rightHandSide(rightHandSide) {}
    ~LlAssignStmtRegular() override {
        delete rightHandSide;
    }

    LlComponent* getRightHandSide() {
        return this->rightHandSide;
    }

    std::string toString() const override{
        return this->storeLocation->toString() + " = " + this->rightHandSide->toString();
    }

    bool operator==(const Ll& other) const override {
        if (&other == this) {
            return true;
        }
        if (auto otherOp = dynamic_cast<const LlAssignStmtRegular*>(&other)) {
            return *storeLocation == *otherOp->storeLocation &&
                   *rightHandSide == *otherOp->rightHandSide;
        }
        return false;
    }
    std::size_t hashCode() const override {
        return storeLocation->hashCode() * rightHandSide->hashCode();
    }

};

class LlAssignStmtBinaryOp : public LlAssignStmt {
private:
    LlComponent* leftOperand;
    std::string operation;
    LlComponent* rightOperand;

public:
    LlAssignStmtBinaryOp(LlLocation* storeLocation, LlComponent* leftOperand, std::string operation, LlComponent* rightOperand)
        : LlAssignStmt(storeLocation), leftOperand(leftOperand), operation(operation), rightOperand(rightOperand) {}

    ~LlAssignStmtBinaryOp() override {
        delete leftOperand;
        delete rightOperand;
    }

    LlComponent* getLeftOperand() {
        return this->leftOperand;
    }

    std::string getOperation() {
        return this->operation;
    }

    LlComponent* getRightOperand() {
        return this->rightOperand;
    }

    std::string toString() const override{
        return this->storeLocation->toString() + " = " + this->leftOperand->toString() + " " + (this->operation)  + " " + this->rightOperand->toString();
    }

    bool operator==(const Ll& other) const override {
        if (&other == this) {
            return true;
        }
        if (auto otherOp = dynamic_cast<const LlAssignStmtBinaryOp*>(&other)) {
            return *storeLocation == *otherOp->storeLocation &&
                   *leftOperand == *otherOp->leftOperand &&
                   *rightOperand == *otherOp->rightOperand &&
                   operation == otherOp->operation;
        }
        return false;
    }
    std::size_t hashCode() const override {
        std::size_t hash = storeLocation->hashCode()
        * leftOperand->hashCode()
        * rightOperand->hashCode()
        * std::hash<std::string>()(operation);
        return hash;
    }

};

class LlAssignStmtAddr : public LlAssignStmt {
private:
    LlLocation* loadLocation;
public:
    LlAssignStmtAddr(LlLocation* storeLocation, LlLocation* loadLocation)
        : LlAssignStmt(storeLocation), loadLocation(loadLocation) {}

    ~LlAssignStmtAddr() override {
        delete loadLocation;
    }

    LlLocation* getLoadLocation() {
        return this->loadLocation;
    }

    std::string toString() const override{
        return this->storeLocation->toString() + " = &" + this->loadLocation->toString();
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherOp = dynamic_cast<const LlAssignStmtAddr*>(&other)) {
            return *storeLocation == *otherOp->storeLocation &&
                   *loadLocation == *otherOp->loadLocation;
        }
        return false;
    }

    std::size_t hashCode() const override{
        return storeLocation->hashCode() * loadLocation->hashCode();
    }
};

class LlAssignStmtDeref : public LlAssignStmt {
private:
    LlComponent* storeValue;

public:
    LlAssignStmtDeref(LlLocation* storeLocation, LlComponent* storeValue)
        : LlAssignStmt(storeLocation), storeValue(storeValue) {}

    ~LlAssignStmtDeref() override {
        delete storeValue;
    }

    LlComponent* getStoreValue() {
        return this->storeValue;
    }

    std::string toString() const override{
        return storeLocation->toString() + " = " + storeValue->toString();
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherOp = dynamic_cast<const LlAssignStmtDeref*>(&other)) {
            return *storeLocation == *otherOp->storeLocation &&
                   *storeValue == *otherOp->storeValue;
        }
        return false;
    }

    std::size_t hashCode() const override{
        return storeLocation->hashCode() * storeValue->hashCode();
    }
};

class LlAssignStmtUnaryOp : public LlAssignStmt {
private:
    LlComponent* operand;
    std::string* operator_;

public:
    LlAssignStmtUnaryOp(LlLocation* storeLocation, LlComponent* operand, std::string* operator_)
        : LlAssignStmt(storeLocation), operand(operand), operator_(operator_) {}
    ~LlAssignStmtUnaryOp() override {
        delete operand;
        delete operator_;
    }

    LlComponent* getOperand() {
        return this->operand;
    }

    std::string* getOperator() {
        return this->operator_;
    }

    std::string toString() const override{
        return this->storeLocation->toString() + " = " + *(operator_) + " " + operand->toString();
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherOp = dynamic_cast<const LlAssignStmtUnaryOp*>(&other)) {
            return *storeLocation == *otherOp->storeLocation &&
                   *operand == *otherOp->operand &&
                   *operator_ == *otherOp->operator_;
        }
        return false;
    }
    std::size_t hashCode() const override {
        return storeLocation->hashCode() * operand->hashCode() * std::hash<std::string>()(*operator_);
    }
};

class LlJump : public LlStatement {
protected:
    std::string* jumpToLabel;
    bool conditionalJump;

public:
    LlJump(std::string* jumpToLabel) : jumpToLabel(jumpToLabel) {this->isJumpInst = true; this->conditionalJump = false;}
    ~LlJump() override {
        delete jumpToLabel;
    }

    std::string* getJumpToLabel() {
        return jumpToLabel;
    }

    bool isConditionalJump() {
        return this->conditionalJump;
    }

    std::string toString() const override{
        return "goto " + *(this->jumpToLabel);
    }
    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherJump = dynamic_cast<const LlJump*>(&other)) {
            return *jumpToLabel == *otherJump->jumpToLabel;
        }
        return false;
    }
    virtual std::size_t hashCode() const override{
        return std::hash<std::string>()(*jumpToLabel);
    }
};

// ifZ Condition goto Label (ifZ: if condition is zero/false, then jump)
class LlJumpConditional : public LlJump {
private:
    LlComponent* condition;

public:
    LlJumpConditional(std::string* jumpToLabel, LlComponent* condition)
        : LlJump(jumpToLabel), condition(condition) {this->conditionalJump = true;}

    ~LlJumpConditional() override {
        delete condition;
    }

    LlComponent* getCondition() {
        return this->condition;
    }

    std::string toString() const override{
        return "ifZ " + condition->toString() + " goto " + *(this->jumpToLabel);
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherJump = dynamic_cast<const LlJumpConditional*>(&other)) {
            return *jumpToLabel == *otherJump->jumpToLabel &&
                   *condition == *otherJump->condition;
        }
        return false;
    }
    std::size_t hashCode() const override {
        return std::hash<std::string>()(*jumpToLabel) * condition->hashCode();
    }
};

// goto Label always executed when reached used for loops and break
class LlJumpUnconditional : public LlJump {
public:
    LlJumpUnconditional(std::string* jumpToLabel) : LlJump(jumpToLabel) {this->conditionalJump = false;}

    ~LlJumpUnconditional() override {}

    std::string toString() const override{
        return "goto " + *(this->jumpToLabel);
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherJump = dynamic_cast<const LlJumpUnconditional*>(&other)) {
            return *jumpToLabel == *otherJump->jumpToLabel;
        }
        return false;
    }
    std::size_t hashCode() const override {
        return std::hash<std::string>()(*jumpToLabel);
    }
};


class LlLiteralBool : public LlLiteral {
private:
    bool boolValue;

public:
    LlLiteralBool(bool boolValue) : boolValue(boolValue) {}
    ~LlLiteralBool() override {}

    bool getBoolValue() {
        return this->boolValue;
    }

    std::string toString() const override{
        return this->boolValue ? "true" : "false";
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherBool = dynamic_cast<const LlLiteralBool*>(&other)) {
            return this->boolValue == otherBool->boolValue;
        }
        return false;
    }
    std::size_t hashCode() const override {
        return std::hash<bool>()(this->boolValue);
    }
};


class LlLiteralInt : public LlLiteral {
private:
    int intValue;

public:
    LlLiteralInt(long intValue) : intValue(intValue) {}
    ~LlLiteralInt() override {}

    int getValue() {
        return this->intValue;
    }

    std::string toString()  const override{
        return std::to_string(this->intValue);
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherInt = dynamic_cast<const LlLiteralInt*>(&other)) {
            return this->intValue == otherInt->intValue;
        }
        return false;
    }
    std::size_t hashCode() const override {
        return std::hash<long>()(this->intValue);
    }
};

class LlLiteralChar : public LlLiteral {
private:
    char charValue;

public:
    LlLiteralChar(char charValue) : charValue(charValue) {}
    ~LlLiteralChar() override {}

    char getValue() {
        return this->charValue;
    }

    std::string toString() const override{
        return std::string(1, this->charValue);
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherChar = dynamic_cast<const LlLiteralChar*>(&other)) {
            return this->charValue == otherChar->charValue;
        }
        return false;
    }
    std::size_t hashCode() const override{
        return std::hash<char>()(this->charValue);
    }
};

class LlLiteralString : public LlLiteral {
private:
    std::string* stringValue;
public:
    LlLiteralString(std::string* stringValue) : stringValue(stringValue) {}
    ~LlLiteralString() override {
        delete stringValue;
    }

    std::string* getValue() {
        return this->stringValue;
    }

    std::string toString() const override{
        return *(this->stringValue);
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherLlString = dynamic_cast<const LlLiteralString*>(&other)) {
            return *otherLlString->stringValue == *this->stringValue;
        }
        return false;
    }
    std::size_t hashCode() const override{
        return std::hash<std::string>()(*stringValue);
    }
};

class LlLocationArray : public LlLocation {
private:
    LlComponent* elementIndex;

public:
    LlLocationArray(std::string* varName, LlComponent* elementIndex) : LlLocation(varName), elementIndex(elementIndex) {}
    ~LlLocationArray() override {
        delete elementIndex;
    }

    LlComponent* getElementIndex() {
        return this->elementIndex;
    }

    std::string toString() const override{
        return *(this->getVarName()) + "[" + elementIndex->toString() + "] ";
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherArray = dynamic_cast<const LlLocationArray*>(&other)) {
            return *otherArray->getVarName() == *this->getVarName() &&
                   *otherArray->elementIndex == *this->elementIndex;
        }
        return false;
    }
    std::size_t hashCode() const override{
        return std::hash<std::string>()(*getVarName()) * elementIndex->hashCode();
    }

    std::string getArrayHead(std::string arrayLocation) {
        int arrayHead = std::stoi(arrayLocation.substr(0, arrayLocation.length() - 6)) / 8;
        return std::to_string(arrayHead);
    }
};

class LlLocationVar : public LlLocation {
public:
    LlLocationVar(std::string* varName) : LlLocation(varName) {}
    ~LlLocationVar() override {}


    std::string toString() const override{
        return *(this->getVarName());
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherVar = dynamic_cast<const LlLocationVar*>(&other)) {
            return *otherVar->getVarName() == *this->getVarName();
        }
        return false;
    }

    std::size_t hashCode() const override{
        std::hash<std::string> hasher;
        return hasher(*(this->getVarName()));
    }

    bool isStringLoc() {
        return this->getVarName()->find("str") != std::string::npos;
    }
};

class LlMethodCallStmt : public LlStatement {
private:
    LlLocation* returnLocation;
    std::string methodName;
    std::vector<LlComponent*> argsList;

public:
    LlMethodCallStmt(const std::string methodName, std::vector<LlComponent*> argsList, LlLocation* returnLocation)
        : methodName(methodName), argsList(argsList), returnLocation(returnLocation) {}
    ~LlMethodCallStmt() override {
        delete returnLocation;
        for (LlComponent* arg : argsList) {
            delete arg;
        }
    }

    std::string getMethodName() {
        return this->methodName;
    }

    LlLocation* getReturnLocation() {
        return this->returnLocation;
    }

    std::vector<LlComponent*> getArgsList() {
        return this->argsList;  // Return a copy of the vector
    }

    std::string toString() const override{
        std::string argsString = "";
        for(auto &arg : argsList){
            argsString +=  arg->toString() +",";
        }
        return this->returnLocation->toString() + " = " + this->methodName + "(" + argsString +")" ;
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherMethod = dynamic_cast<const LlMethodCallStmt*>(&other)) {
            if (methodName != otherMethod->methodName || !(*returnLocation == *otherMethod->returnLocation) || argsList.size() != otherMethod->argsList.size()) {
                return false;
            }
            for (int i = 0; i < argsList.size(); i++) {
                if (!(*argsList[i] == *otherMethod->argsList[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    std::size_t hashCode() const override{
        std::size_t hash = std::hash<std::string>()(methodName) * returnLocation->hashCode();
        for (auto arg : argsList) {
            hash *= arg->hashCode();
        }
        return hash;
    }
};


class LlParallelMethodStmt : public LlStatement {
private:
    std::string parallelMethodName;

public:
    LlParallelMethodStmt(std::string methodName) : parallelMethodName(methodName) {}
    ~LlParallelMethodStmt() override {}

    std::string toString() const override {
        return "create_and_run_threads(" + this->parallelMethodName + ")";
    }
};


class LlReturn : public LlStatement {
private:
    LlComponent* returnValue;

public:
    LlReturn(LlComponent* returnValue) : returnValue(returnValue) {}
    ~LlReturn() override {
        delete returnValue;
    }

    LlComponent* getReturnValue() {
        return this->returnValue;
    }

    std::string toString() const override{
        if (this->returnValue == nullptr) {
            return "return ";
        }
        return "return " + this->returnValue->toString();
    }

    bool operator==(const Ll& other) const override{
        if (&other == this) {
            return true;
        }
        if (auto otherReturn = dynamic_cast<const LlReturn*>(&other)) {
            if (returnValue == nullptr) {
                return otherReturn->returnValue == nullptr;
            }
            return *returnValue == *otherReturn->returnValue;
        }
        return false;
    }
    std::size_t hashCode() const override{
        if (returnValue == nullptr) {
            return 13;
        }
        return returnValue->hashCode();
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


class LlLocationTypeAlias : public LlLocation {
private:
    const std::string* aliasName;

public:
    LlLocationTypeAlias(std::string* aliasName)
        : LlLocation(aliasName), aliasName(aliasName) {}

    ~LlLocationTypeAlias() override {
        delete aliasName;
    }

    // Override getVarName to return aliasName directly
    const std::string* getAliasTypeName() const {
        return aliasName;
    }

    // Override toString for clarity
    std::string toString() const override {
        return "TypeAlias: " + *aliasName;
    }
};

class LlLocationStruct : public LlLocation {
private:
    LlLocation* baseLocation;
    std::string fieldName;      // Field name within the struct
    int offset;

public:
    LlLocationStruct(LlLocation* baseLocation, const std::string& fieldName, int offset)
        : LlLocation(baseLocation->getVarName()), baseLocation(baseLocation), fieldName(fieldName), offset(offset) {}

    ~LlLocationStruct() override {
        delete baseLocation;
    }

    LlLocation* getBaseLocation() const {
        return baseLocation;
    }

    std::string getFieldName() const {
        return fieldName;
    }

    int getOffset() const {
        return offset;
    }

    std::string toString() const override {
        return baseLocation->toString() + "->" + fieldName;
    }

    bool operator==(const Ll& other) const override {
        if (auto* otherField = dynamic_cast<const LlLocationStruct*>(&other)) {
            return *baseLocation == *(otherField->baseLocation) && fieldName == otherField->fieldName;
        }
        return false;
    }

    std::size_t hashCode() const override {
        return baseLocation->hashCode() ^ std::hash<std::string>()(fieldName);
    }
};

#endif