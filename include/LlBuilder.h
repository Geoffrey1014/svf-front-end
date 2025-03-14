#ifndef LL_BUILDER_H
#define LL_BUILDER_H

#include <unordered_map>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <iomanip>
#include "Ll.h"

// Generated Ll Ir for a single scope
class LlBuilder {
private:
    std::string name;
    std::unordered_map<std::string, LlStatement*> statementTable;
    std::vector<std::string> insertionOrder;
    int labelCounter = 0;
    int tempCounter = 0;
    std::deque<LlLocationVar*> params;
    bool arrLeftSide = false;
    std::stack<std::string> currentBlockLabel;
    std::stack<std::string> loopExitStack; // Stack to store the exit label of the loop
    std::string currentLoopCondition;
    Ll* pocket = nullptr;

public:
    LlBuilder(std::string name) : name(name) {}
    ~LlBuilder() {
        for(auto& pair : statementTable){
            delete pair.second;
        }
        for(auto& param : params){
            delete param;
        }
    }

    std::vector<std::string> getInsertionOrder(){
        return this->insertionOrder;
    }

    std::string getName() {
        return this->name;
    }

    void addParam(LlLocationVar* param) {
        params.push_front(param);
    }

    void setStatementTable(std::unordered_map<std::string, LlStatement*> statementTable) {
        this->statementTable = statementTable;
    }

    void appendStatement(LlStatement* statement){
        std::string label = this->generateLabel();
        insertionOrder.push_back(label);
        statementTable[label] = statement;
    }

    void appendStatement(std::string label, LlStatement* statement){
        if(statementTable.find(label) != statementTable.end()){
            std::cerr << "Duplicate label key . Please use the label generator! " << std::endl;
            std::cerr << "Key :" << label << std::endl;
            std::cerr << "Statement : " << statement->toString() << std::endl;
            std::cerr << "StackSize " << labelCounter << std::endl;
        }
        else{
            insertionOrder.push_back(label);
            statementTable[label] = statement;
        }
    }

    // Tied to the the builder, representing the specific block of instructions
    std::string generateLabel(){
       return "L" + std::to_string(labelCounter++);
    }

    LlLocationVar* generateTemp(){
        std::string* t =  new std::string();
        t->append("#_t");
        t->append(std::to_string(tempCounter++));
        return new LlLocationVar(t);
    }

    LlLocationVar* generateStrTemp(){
        std::string* t =  new std::string();
        t->append("#str_t");
        t->append(std::to_string(tempCounter++));
        return new LlLocationVar(t);
    }

    void putInPocket(Ll* o){
        this->pocket = o;
    }

    void emptyPocket(){
        this->pocket = nullptr;
    }

    Ll* pickPocket(){
        return this->pocket;
    }

    std::unordered_map<std::string, LlStatement*> getStatementTable() {
        return statementTable;
    }

    void getOutOfBlock(){
        currentBlockLabel.pop();
    }

    std::string getCurrentBlock(){
        if(currentBlockLabel.size() > 0){
            return currentBlockLabel.top();
        }
        return "";
    }

    void getInBlock(std::string loopLabel){
        currentBlockLabel.push(loopLabel);
    }

    void getInBlock(std::string loopLabel, std::string loopCondition){
        currentBlockLabel.push(loopLabel);
        this->currentLoopCondition = loopCondition;
    }

    std::string getCurrentLoopCondition() {
        return this->currentLoopCondition;
    }

    void pushLoopExit(std::string exitLabel){
        loopExitStack.push(exitLabel);
    }

    std::string getCurrentLoopExit(){
        if(!loopExitStack.empty()){
            return loopExitStack.top();
        }
        return "";
    }

    void popLoopExit(){
        loopExitStack.pop();
    }

    std::string toString() {
        std::stringstream st;
        const int labelWidth = 15;

        st << "IR for Builder: " << this->name << "\n";

        for (const auto& label : insertionOrder) {
            std::string statement = statementTable[label]->toString();
            st << std::right << std::setw(labelWidth) << label << " : " << statement << "\n";
        }
        return st.str();
    }
};

#endif