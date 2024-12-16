#ifndef LLBUILDERLIST_H
#define LLBUILDERLIST_H

#include <vector>
#include <unordered_map>
#include "LlSymbolTable.h"
#include "LlBuilder.h"

class LlBuildersList {
private:
    std::vector<LlBuilder> builders;
    std::vector<LlSymbolTable> symbolTables;
    std::unordered_map<LlLocationVar, int> globalArrays;
    std::vector<LlLocationVar> globalVars;

public:
    LlBuildersList() {
    }

    void addBuilder(const LlBuilder& builder) {
        builders.push_back(builder);
    }

    void addSymbolTable(const LlSymbolTable& symbolTable) {
        symbolTables.push_back(symbolTable);
    }

    void addToGlobalArrays(const LlLocationVar& var, int size) {
        globalArrays[var] = size;
    }

    std::unordered_map<LlLocationVar, int> getGlobalArrays() {
        return globalArrays;
    }

    void addToGlobalVars(const LlLocationVar& var) {
        globalVars.push_back(var);
    }

    std::vector<LlLocationVar> getGlobalVars() {
        return globalVars;
    }

    std::vector<LlBuilder> getBuilders() {
        return builders;
    }

    std::vector<LlSymbolTable> getSymbolTables() {
        return symbolTables;
    }
};

#endif