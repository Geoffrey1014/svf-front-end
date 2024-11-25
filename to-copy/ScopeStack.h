#include <stack>
#include <string>
#include <unordered_map>
#include "Ir.h"

class ScopeStack {
private:
    struct SymbolTable {
        std::unordered_map<std::string, Ir*> hashtable;
        SymbolTable* parentScope;
        IrType* scopeReturnType;
        bool isLoop;

        SymbolTable() : parentScope(nullptr), scopeReturnType(nullptr), isLoop(false) {}
        SymbolTable(SymbolTable* parentScope) : parentScope(parentScope), scopeReturnType(nullptr), isLoop(false) {}
        SymbolTable(SymbolTable* parentScope, IrType* scopeReturnType) : parentScope(parentScope), scopeReturnType(scopeReturnType), isLoop(false) {}
        SymbolTable(SymbolTable* parentScope, bool isLoop) : parentScope(parentScope), scopeReturnType(nullptr), isLoop(isLoop) {}
    };

    std::stack<SymbolTable*> stack;

public:
    ScopeStack() {}

    void addObjectToCurrentScope(std::string id, Ir* object) {
        stack.top()->hashtable[id] = object;
    }

    bool checkIfSymbolExistsAtAnyScope(std::string id) {
        for (SymbolTable* e = stack.top(); e != nullptr; e = e->parentScope) {
            if (e->hashtable.find(id) != e->hashtable.end()) {
                return true;
            }
        }
        return false;
    }

    bool checkIfSymbolExistsAtCurrentScope(std::string id) {
        return stack.top()->hashtable.find(id) != stack.top()->hashtable.end();
    }

    void addSymbolToGlobalScope(std::string id, Ir* object) {
        SymbolTable* globalScope = stack.top();
        while (globalScope->parentScope != nullptr) {
            globalScope = globalScope->parentScope;
        }

        globalScope->hashtable[id] = object;
    }

    bool checkIfSymbolExistsInGlobalScope(std::string id) {
        SymbolTable* globalScope = stack.top();
        while (globalScope->parentScope != nullptr) {
            globalScope = globalScope->parentScope;
        }

        return globalScope->hashtable.find(id) != globalScope->hashtable.end();
    }

    Ir* getSymbol(std::string id) {
        for (SymbolTable* e = stack.top(); e != nullptr; e = e->parentScope) {
            if (e->hashtable.find(id) != e->hashtable.end()) {
                return e->hashtable[id];
            }
        }
        return nullptr;
    }

    SymbolTable* deleteCurrentScope() {
        if (!stack.empty()) {
            delete stack.top();
            stack.pop();
        }
        return nullptr;
    }

    SymbolTable* createNewBlockScope() {
        if (stack.empty()) {
            SymbolTable* globalScope = new SymbolTable();
            stack.push(globalScope);
            return globalScope;
        } else {
            SymbolTable* parentScope = stack.top();
            SymbolTable* newScope = new SymbolTable(parentScope);
            stack.push(newScope);
            return newScope;
        }
    }

    SymbolTable* createNewMethodScope(IrType* methodType) {
        SymbolTable* parentScope = stack.top();
        SymbolTable* newScope = new SymbolTable(parentScope, methodType);
        stack.push(newScope);
        return newScope;
    }

    SymbolTable* createNewLoopScope() {
        SymbolTable* parentScope = stack.top();
        SymbolTable* newScope = new SymbolTable(parentScope, true);
        stack.push(newScope);
        return newScope;
    }

    IrType* getScopeReturnType() {
        for (SymbolTable* e = stack.top(); e != nullptr; e = e->parentScope) {
            if (e->scopeReturnType != nullptr) {
                return e->scopeReturnType;
            }
        }
        return nullptr;
    }

    bool isScopeForALoop() {
        for (SymbolTable* e = stack.top(); e != nullptr; e = e->parentScope) {
            if (e->isLoop) {
                return true;
            }
        }
        return false;
    }

    ~ScopeStack() {
        while (!stack.empty()) {
            delete stack.top();
            stack.pop();
        }
    }
};
