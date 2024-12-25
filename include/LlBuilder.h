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
    std::unordered_map<std::string, LlStatement*> statementTable;
    std::vector<std::string> insertionOrder;
    int labelCounter = 0;
    int tempCounter = 0;
    std::deque<LlLocationVar*> params;
    bool arrLeftSide = false;
    std::stack<std::string> currentBlockLabel;
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
            statementTable[label] = statement;
        }
    }

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

    std::string toString() {
        std::stringstream st;
        for (auto& label : insertionOrder) {
            st << label << " : " << statementTable[label]->toString() << "\n";
        }
        return st.str();
    }
};

#endif