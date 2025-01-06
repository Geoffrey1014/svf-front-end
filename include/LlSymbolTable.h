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
    std::unordered_map<LlLocation*, std::string> paramTable;
    std::unordered_map<LlLocation*, LlComponent*> arrayTable;


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

    std::unordered_map<LlLocation*, std::string> getParamTable(){
        return this->paramTable;
    }

    // void setUseDef(std::unordered_map<CFG.SymbolDef, std::vector<CFG.Tuple>> useDef){
    //     this->useDef = useDef;
    // }

    // std::unordered_map<CFG.SymbolDef, std::vector<CFG.Tuple>> getUseDef(){
    //     return this->useDef;
    // }

    void putOnArrayTable(LlLocation* key, LlComponent* val){
        this->arrayTable[key] = val;
    }

    LlComponent* getFromArrayTable(LlLocation* key){
        auto it = this->arrayTable.find(key);
        if(it != this->arrayTable.end()){
            return it->second;
        }
        return nullptr;
    }

    std::unordered_map<LlLocation*, LlComponent*> getArrayTable(){
        return this->arrayTable;
    }

};

#endif