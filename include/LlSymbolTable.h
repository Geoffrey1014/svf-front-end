#ifndef LLSYMBOLTABLE_H
#define LLSYMBOLTABLE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Ll.h"

class LlSymbolTable {
private:
    std::string methodName;
    std::unordered_map<LlComponent*, std::string> llStringTable;
    std::unordered_map<LlLocationVar*, std::string> paramTable;
    std::unordered_map<LlLocationVar*, int> arrayTable;
    std::unordered_map<LlLocationVar*, int> globalArrays;
    std::vector<LlLocationVar*> globalVars;

public:
    LlSymbolTable(std::string methodName) : methodName(methodName) {}

    std::string getMethodName() {
        return this->methodName;
    }

    void putOnStringTable(LlComponent* key, std::string value){
        this->llStringTable[key] = value;
    }

    std::string getFromStringTable(LlComponent* key){
        return this->llStringTable[key];
    }

    std::unordered_map<LlComponent*, std::string> getLlStringTable(){
        return this->llStringTable;
    }

    void putOnParamTable(LlLocationVar* key, std::string value){
        this->paramTable[key] = value;
    }

    std::string getFromParamTable(LlLocationVar* key){
       auto it = this->paramTable.find(key);
       if(it != this->paramTable.end()){
           return it->second;
       }
       return "";
    }

    std::unordered_map<LlLocationVar*, std::string> getParamTable(){
        return this->paramTable;
    }

    // void setUseDef(std::unordered_map<CFG.SymbolDef, std::vector<CFG.Tuple>> useDef){
    //     this->useDef = useDef;
    // }

    // std::unordered_map<CFG.SymbolDef, std::vector<CFG.Tuple>> getUseDef(){
    //     return this->useDef;
    // }

    void putOnArrayTable(LlLocationVar* key, int val){
        this->arrayTable[key] = val;
    }

    int getFromArrayTable(LlLocationVar* key){
        auto it = this->arrayTable.find(key);
        if(it != this->arrayTable.end()){
            return it->second;
        }
        return -1;
    }

    std::unordered_map<LlLocationVar*, int> getArrayTable(){
        return this->arrayTable;
    }

    void addToGlobalArrays(LlLocationVar* var, int size){
        this->globalArrays[var] = size;
    }

    std::unordered_map<LlLocationVar*, int> getGlobalArrays(){
        return this->globalArrays;
    }

    void addToGlobalVars(LlLocationVar* var){
        this->globalVars.push_back(var);
    }

    std::vector<LlLocationVar*> getGlobalVars(){
        return this->globalVars;
    }

    bool isInGlobalArraysTable(LlLocationVar* var){
        return this->globalArrays.find(var) != this->globalArrays.end();
    }

    bool isInGlobalVarsTable(LlLocationVar* var){
        for (auto &v : this->globalVars){
            if(v->getVarName() == var->getVarName()){
                return true;
            }
        }
        return false;
    }
};

#endif