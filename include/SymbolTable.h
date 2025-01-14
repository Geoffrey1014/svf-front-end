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
    std::unordered_map<std::string, LlComponent*> varTable;
    std::unordered_map<std::string, IrType*> typeTable;
    SymbolTable* parentTable;

public:
    SymbolTable(std::string methodName, SymbolTable* parent = nullptr)
        : methodName(methodName), parentTable(parent) {}

    ~SymbolTable() {
    }

    void putOnTypeTable(std::string key, IrType* value){
        this->typeTable[key] = value;
    }

    IrType* getFromTypeTable(std::string key){
        auto it = this->typeTable.find(key);
        if(it != this->typeTable.end()){
            return it->second;
        }
        // Search in parent/global table if not found locally
        if (parentTable) {
            return parentTable->getFromTypeTable(key);
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
        if (parentTable) {
            return parentTable->getFromVarTable(key);
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

};

#endif