#ifndef AST_BUILDER_TPP
#define AST_BUILDER_TPP

#include <iostream>
#include <typeinfo>
#include <stack>
#include "ASTBuilder.h"
#include "Ir.h"

// Template function to pop and dynamically cast an element from the AST stack
template <typename T>
T* ASTBuilder::popFromStack() {
    if (this->ast_stack.empty()) {
        std::cerr << "Error: AST stack is empty while attempting to pop" << std::endl;
        return nullptr;
    }

    Ir* top = this->ast_stack.top();
    this->ast_stack.pop();

    if (T* casted = dynamic_cast<T*>(top)) {
        return casted;
    } else {
        std::cerr << "Error: Invalid type on AST stack. Expected " << typeid(T).name() << std::endl;
        delete top;
        return nullptr;
    }
}

#endif 