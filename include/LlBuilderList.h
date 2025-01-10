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
    std::unordered_map<LlLocationArray*, int> globalArrays;
    std::vector<LlLocationVar*> globalVars;

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

    void addGlobalArray(LlLocationArray* array, int size) {
        globalArrays[array] = size;
    }

    std::unordered_map<LlLocationArray*, int> getGlobalArrays() {
        return globalArrays;
    }

    void addGlobalVar(LlLocationVar* var) {
        globalVars.push_back(var);
    }

    std::vector<LlLocationVar*> getGlobalVars() {
        return globalVars;
    }
};

#endif