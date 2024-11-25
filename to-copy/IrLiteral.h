#include "Ir.h"

class IrLiteral : public IrExpr {
public:
    IrLiteral(int lineNumber, int colNumber) : IrExpr(lineNumber, colNumber) {}
};


class IrLiteralBool : public IrLiteral {
private:
    bool value;

public:
    IrLiteralBool(bool value, int lineNumber, int colNumber) : IrLiteral(lineNumber, colNumber), value(value) {}

    IrType* getExpressionType() {
        return new IrTypeBool(this->getLineNumber(), this->getColNumber());
    }

    // std::string semanticCheck(ScopeStack* scopeStack) {
    //     return "";
    // }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--boolLiteral\n";
        prettyPrint += "  " + indentSpace + "|--value: " + (this->value ? "true" : "false") + "\n";
        return prettyPrint;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        return nullptr;
    }
};


class IrLiteralChar : public IrLiteral {
private:
    char value;

public:
    IrLiteralChar(char value, int lineNumber, int colNumber) : IrLiteral(lineNumber, colNumber), value(value) {}

    IrType* getExpressionType() {
        // it's definitely not of type void but it
        // is also not of type int or type bool
        return new IrTypeVoid(this->getLineNumber(), this->getColNumber());
    }

    // std::string semanticCheck(ScopeStack* scopeStack) {
    //     return "";
    // }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--charLiteral\n";
        prettyPrint += "  " + indentSpace + "|--value: " + this->value + "\n";
        return prettyPrint;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        return nullptr;
    }
};


class IrLiteralInt : public IrLiteral {
private:
    long value;

public:
    IrLiteralInt(long value, int lineNumber, int colNumber) : IrLiteral(lineNumber, colNumber), value(value) {}

    long getValue() {
        return this->value;
    }

    IrType* getExpressionType() {
        return new IrTypeInt(this->getLineNumber(), this->getColNumber());
    }

    // std::string semanticCheck(ScopeStack* scopeStack) {
    //     return "";
    // }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--IntLiteral\n";
        prettyPrint += "  " + indentSpace + "|--value: " + std::to_string(this->value) + "\n";
        return prettyPrint;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
       return nullptr;
    }
};


class IrLiteralString : public IrLiteral {
private:
    std::string value;

public:
    IrLiteralString(std::string value, int lineNumber, int colNumber) : IrLiteral(lineNumber, colNumber), value(value) {}

    std::string getValue() {
        return this->value;
    }

    IrType* getExpressionType() {
        return new IrTypeString(this->getLineNumber(), this->getColNumber());
    }

    // std::string semanticCheck(ScopeStack* scopeStack) {
    //     return "";
    // }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--StringLiteral\n";
        prettyPrint += "  " + indentSpace + "|--value: " + this->value + "\n";
        return prettyPrint;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        return nullptr;
    }
};
