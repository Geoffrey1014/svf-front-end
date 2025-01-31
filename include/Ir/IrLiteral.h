#ifndef IR_LITERAL_H
#define IR_LITERAL_H
#include "IrExpr.h"

class IrLiteral : public IrExpr {
public:
    IrLiteral(const TSNode& node) : IrExpr(node) {}
};


class IrLiteralBool : public IrLiteral {
private:
    bool value;

public:
    IrLiteralBool(bool value, const TSNode& node) : IrLiteral(node), value(value) {}
    ~IrLiteralBool() = default;
    // IrType* getExpressionType() {
    //     return new IrTypeBool(this->getLineNumber(), this->getColNumber());
    // }


    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--boolLiteral\n";
        prettyPrint += "  " + indentSpace + "|--value: " + (this->value ? "true" : "false") + "\n";
        return prettyPrint;
    }

};


class IrLiteralChar : public IrLiteral {
private:
    char value;
public:
    IrLiteralChar(char value, const TSNode& node)
        : IrLiteral(node), value(value) {}

    std::string prettyPrint(std::string indentSpace) const override {
        return indentSpace + "|--char_literal: '" + value + "'\n";
    }

    char getValue() const {
        return value;
    }
};


class IrLiteralNumber : public IrLiteral {
private:
    long value;

public:
    IrLiteralNumber(long value, const TSNode& node) : IrLiteral(node), value(value) {}
    ~IrLiteralNumber() = default;
    long getValue() {
        return this->value;
    }

    // IrType* getExpressionType() {
    //     return new IrTypeInt(this->getLineNumber(), this->getColNumber());
    // }


    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--NumberLiteral\n";
        prettyPrint += "  " + indentSpace + "|--value: " + std::to_string(this->value) + "\n";
        return prettyPrint;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLiteralInt * llLiteralInt = new LlLiteralInt(this->value);
        LlLocationVar * llLocationVar = builder.generateTemp();
        LlAssignStmt* llAssignStmt = new LlAssignStmtRegular(llLocationVar, llLiteralInt);
        builder.appendStatement(llAssignStmt);
        return llLocationVar;
    }

};


class IrLiteralString : public IrLiteral {
private:
    std::string* value;

public:
    IrLiteralString(std::string* value,const TSNode& node) : IrLiteral(node), value(value) {}
    ~IrLiteralString() {
        delete value;
    }
    std::string* getValue() {
        return this->value;
    }

    // IrType* getExpressionType() {
    //     return new IrTypeString(this->getLineNumber(), this->getColNumber());
    // }


    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--StringLiteral\n";
        prettyPrint += "  " + indentSpace + "|--value: " + *(this->value) + "\n";
        return prettyPrint;
    }

};

class IrStringContent : public Ir {
private:
    std::string* value;
public:
    IrStringContent(std::string* value, const TSNode& node) : Ir(node), value(value) {}
    ~IrStringContent() {
        delete value;
    }
    std::string* getValue() {
        return this->value;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--StringContent\n";
        prettyPrint += "  " + indentSpace + "|--value: " + *(this->value) + "\n";
        return prettyPrint;
    }
};

#endif