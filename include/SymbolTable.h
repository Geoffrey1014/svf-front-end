#ifndef LLSYMBOLTABLE_H
#define LLSYMBOLTABLE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Ll.h"

// TypeTable:
//     m : int               // Variable (not a type)
//     A : struct {...}      // Type alias

class IrType;
class SymbolTable {
private:
    std::string methodName;
    // std::unordered_map<std::string, LlComponent*> varTable;
    std::unordered_map<std::string, IrType*> typeDefTable;
    std::unordered_map<std::string, IrType*> varTable;
    SymbolTable* parentTable;

public:
    SymbolTable(std::string methodName, SymbolTable* parent = nullptr)
        : methodName(methodName), parentTable(parent) {}

    ~SymbolTable() {
    }

    void putOnVarTable(std::string key, IrType* value){
        this->varTable[key] = value;
    }

    IrType* getFromVarTable(std::string key){
        auto it = this->varTable.find(key);
        if(it != this->varTable.end()){
            return it->second;
        }
        // Search in parent/global table if not found locally
        if (parentTable) {
            return parentTable->getFromVarTable(key);
        }
        return nullptr;
    }

    // TypeDef handling
    void putOnTypeDefTable(const std::string& alias, IrType* actualType) {
        this->typeDefTable[alias] = actualType;
    }

    IrType* getFromTypeDefTable(const std::string& alias) {
        auto it = this->typeDefTable.find(alias);
        if (it != this->typeDefTable.end()) {
            return it->second;
        }
        if (parentTable) {
            return parentTable->getFromTypeDefTable(alias);
        }
        return nullptr;
    }
    
    std::string toString();

    std::string getMethodName() {
        return this->methodName;
    }

};

#endif