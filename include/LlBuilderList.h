#ifndef LLBUILDERLIST_H
#define LLBUILDERLIST_H

#include <vector>
#include <unordered_map>
#include "SymbolTable.h"
#include "LlBuilder.h"

class LlBuildersList {
private:
    std::vector<LlBuilder*> builders;
    std::vector<SymbolTable*> symbolTables;

public:
    LlBuildersList() {
    }

    void addBuilder(LlBuilder* builder) {
        builders.push_back(builder);
    }

    void addSymbolTable(SymbolTable* symbolTable) {
        symbolTables.push_back(symbolTable);
    }

    std::vector<LlBuilder*> getBuilders() {
        return builders;
    }

    std::vector<SymbolTable*> getSymbolTables() {
        return symbolTables;
    }
};

#endif