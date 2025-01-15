#include "SymbolTable.h"
#include <string>
#include "Ir/IrType.h"

std::string SymbolTable::toString(){
    std::stringstream str;
    const int labelWidth = 15;
    
    str << std::right << std::setw(labelWidth) << "SymbolTable"  << " : " << this->methodName << "\n";
    str << std::right << std::setw(labelWidth) <<  "TypeTable"  << " : " << "\n";
    for(auto pair : this->varTable){
        str << std::right << std::setw(labelWidth) <<  pair.first << " : " << pair.second->toString() << "\n";
    }

    return str.str();
}