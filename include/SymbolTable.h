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
    std::unordered_map<std::string, IrType*> table;
    SymbolTable* parentTable;

public:
    SymbolTable(std::string methodName, SymbolTable* parent = nullptr)
        : methodName(methodName), parentTable(parent) {}

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
        // Search in parent/global table if not found locally
        if (parentTable) {
            return parentTable->getFromTable(key);
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

// extern: declare the variable globalSymbolTable exists, 
// but is defined in somewhere else (src/SymbolTable.cpp)
// extern SymbolTable globalSymbolTable;
#endif