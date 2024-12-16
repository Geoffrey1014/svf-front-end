#ifndef LL_BUILDER_H
#define LL_BUILDER_H

#include <unordered_map>
#include <string>
#include <vector>
#include <stack>
#include <sstream> // Include this header for std::stringstream

class LlBuilder {
private:
    std::string name;
    std::unordered_map<std::string, LlStatement> statementTable;
    int labelCounter = 0;
    int tempCounter = 0;
    std::vector<LlLocationVar> params;
    bool arrLeftSide = false;
    std::stack<std::string> currentBlockLabel;
    std::string currentLoopCondition;
    

public:
    LlBuilder(std::string name) : name(name) {}

    std::string getName() {
        return this->name;
    }

    void setStatementTable(std::unordered_map<std::string, LlStatement> statementTable) {
        this->statementTable = statementTable;
    }

    void appendStatement(LlStatement statement){
        std::string label = this->generateLabel();
        statementTable[label] = statement;
    }

    void appendStatement(std::string label, LlStatement statement){
        if(statementTable.find(label) != statementTable.end()){
            std::cerr << "Duplicate label key . Please use the label generator! " << std::endl;
            std::cerr << "Key :" << label << std::endl;
            std::cerr << "Statement : " << statement.toString() << std::endl;
            std::cerr << "StackSize " << labelCounter << std::endl;
        }
        else{
            statementTable[label] = statement;
        }
    }

    std::string generateLabel(){
       return "L" + std::to_string(labelCounter++);
    }

    LlLocationVar generateTemp(){
        return LlLocationVar("#_t" + std::to_string(tempCounter++));
    }

    LlLocationVar generateStrTemp(){
        return LlLocationVar("#str_t" + std::to_string(tempCounter++));
    }

    // void putInPocket(std::any o){
    //     this->pocket = o;
    // }

    // void emptyPocket(){
    //     this->pocket.reset();
    // }

    // std::any pickPocket(){
    //     return this->pocket;
    // }

    std::unordered_map<std::string, LlStatement> getStatementTable() {
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

    std::string toString() {
        std::stringstream st;
        for(auto& pair : statementTable){
            st << pair.first << " : " << pair.second.toString() << "\n";
        }
        return st.str();
    }
};

#endif