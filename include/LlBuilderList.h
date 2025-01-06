#ifndef LLBUILDERLIST_H
#define LLBUILDERLIST_H

#include <vector>
#include <unordered_map>
#include "LlSymbolTable.h"
#include "LlBuilder.h"

class LlBuildersList {
private:
    std::vector<LlBuilder*> builders;
    std::vector<LlSymbolTable*> symbolTables;

public:
    LlBuildersList() {
    }

    void addBuilder(LlBuilder* builder) {
        builders.push_back(builder);
    }

    void addSymbolTable(LlSymbolTable* symbolTable) {
        symbolTables.push_back(symbolTable);
    }

    std::vector<LlBuilder*> getBuilders() {
        return builders;
    }

    std::vector<LlSymbolTable*> getSymbolTables() {
        return symbolTables;
    }
};

#endif