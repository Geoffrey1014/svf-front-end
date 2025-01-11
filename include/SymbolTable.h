#ifndef LLSYMBOLTABLE_H
#define LLSYMBOLTABLE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Ll.h"
class IrType;
class SymbolTable {
private:
    std::string methodName;
    std::unordered_map<LlLocation*, std::string> paramTable;
    std::unordered_map<std::string, LlComponent*> varTable;
    std::unordered_map<std::string, IrType*> table;

public:
    SymbolTable(std::string methodName) : methodName(methodName) {}

    ~SymbolTable() {
    }

    void putOnTable(std::string key, IrType* value){
        this->table[key] = value;
    }

    IrType* getFromTable(std::string key){
        auto it = this->table.find(key);
        if(it != this->table.end()){
            return it->second;
        }
        return nullptr;
    }

    void putOnVarTable(std::string key, LlComponent* value){
        this->varTable[key] = value;
    }

    LlComponent* getFromVarTable(std::string key){
        auto it = this->varTable.find(key);
        if(it != this->varTable.end()){
            return it->second;
        }
        return nullptr;
    }

    std::unordered_map<std::string, LlComponent*> getVarTable(){
        return this->varTable;
    }


    std::string toString();

    std::string getMethodName() {
        return this->methodName;
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

};

#endif