#include "SymbolTable.h"
#include <string>
#include "Ir.h"

std::string SymbolTable::toString(){
    std::stringstream str;
    const int labelWidth = 15;
    
    str << std::right << std::setw(labelWidth) << "SymbolTable"  << " : " << this->methodName << "\n";
    str << std::right << std::setw(labelWidth) <<  "TypeDefTable"  << " : " << "\n";
    for(auto pair : this->typeDefTable){
        str << std::right << std::setw(labelWidth) <<  pair.first << " : " << pair.second->toString() << "\n";
    }
    str << std::right << std::setw(labelWidth) <<  "VarTable"  << " : " << "\n";
    for(auto pair : this->varTable){
        str << std::right << std::setw(labelWidth) <<  pair.first << " : " << pair.second->toString() << "\n";
    }

    return str.str();
}