#ifndef AST_BUILDER_TPP
#define AST_BUILDER_TPP

#include <iostream>
#include <typeinfo>
#include <stack>
#include <stdexcept>
#include "ASTBuilder.h"
#include "Ir/Ir.h"

///////// Try Catch Print Template ////////
template <typename T>
T* ASTBuilder::popFromStack(const TSNode& node) {
    if (this->ast_stack.empty()) {
        throw std::runtime_error("Error: AST stack is empty while attempting to pop node:\n" + getNodeText(node));
    }

    Ir* top = this->ast_stack.top();
    this->ast_stack.pop();

    if (T* casted = dynamic_cast<T*>(top)) {
        return casted;
    } else {
        std::string errorMessage = "Error: Invalid type on AST stack.\n";
        errorMessage += "Expected type: " + std::string(typeid(T).name()) + "\n";
        errorMessage += "Actual type: " + std::string(typeid(*top).name()) + "\n";
        errorMessage += "Node content: " + getNodeText(node) + "\n";
        delete top; // Clean up the invalid node
        throw std::runtime_error(errorMessage);
    }
}

#endif