#ifndef IR_LITERAL_H
#define IR_LITERAL_H
#include "IrExpr.h"

class IrLiteral : public IrExpr {
public:
    IrLiteral(const TSNode& node) : IrExpr(node), Ir(node) {}

    virtual ~IrLiteral() = default;

    virtual LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        std::cerr << "IrLiteral Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("")); // Return empty location
    }
};


class IrLiteralBool : public IrLiteral {
private:
    bool value;

public:
    IrLiteralBool(bool value, const TSNode& node) : IrLiteral(node), Ir(node), value(value) {}
    ~IrLiteralBool() = default;
    IrType* getExpressionType() {
        return new IrTypeBool(getNode());
    }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--boolLiteral\n";
        prettyPrint += addIndent(indentSpace)+ "|--value: " + (this->value ? "true" : "false") + "\n";
        return prettyPrint;
    }

    std::string toString() {
        return "IrLiteralBool";
    }

    LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        LlLiteralBool *llLiteralBool = new LlLiteralBool(this->value);
        LlLocationVar *llLocationVar = builder.generateTemp();
        LlAssignStmtRegular* regularAssignment  = new LlAssignStmtRegular(llLocationVar, llLiteralBool);
        builder.appendStatement(regularAssignment);
        return llLocationVar;
    }
};


class IrLiteralChar : public IrLiteral {
private:
    char value;

public:
    IrLiteralChar(char value, const TSNode& node) : IrLiteral(node), Ir(node), value(value) {}
    ~IrLiteralChar() = default;
    IrType* getExpressionType() {
        return new IrTypeVoid(getNode());
    }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--charLiteral\n";
        prettyPrint += addIndent(indentSpace) + "|--value: " + this->value + "\n";
        return prettyPrint;
    }

    std::string toString() const{
        return "IrLiteralChar";
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        LlLiteralChar *llLiteralChar = new LlLiteralChar(this->value);
        LlLocationVar *llLocationVar = builder->generateTemp();
        LlAssignStmtRegular* regularAssignment  = new LlAssignStmtRegular(llLocationVar, llLiteralChar);
        builder->appendStatement(regularAssignment);
        return llLocationVar;
    }
};


class IrLiteralNumber : public IrLiteral {
private:
    long value;

public:
    IrLiteralNumber(long value, const TSNode& node) : IrLiteral(node), Ir(node), value(value) {}
    ~IrLiteralNumber() = default;
    long getValue() {
        return this->value;
    }

    IrType* getExpressionType() {
        return new IrTypeInt(getNode());
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--NumberLiteral\n";
        prettyPrint += addIndent(indentSpace) + "|--value: " + std::to_string(this->value) + "\n";
        return prettyPrint;
    }

    std::string toString() const{
        return std::to_string(value); // "IrLiteralNumber";
    }

    LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        LlLiteralInt * llLiteralInt = new LlLiteralInt(this->value);
        LlLocationVar *llLocationVar = builder.generateTemp();
        LlAssignStmtRegular* regularAssignment  = new LlAssignStmtRegular(llLocationVar, llLiteralInt);
        builder.appendStatement(regularAssignment);
        return llLocationVar;
    }
};

class IrLiteralStringContent : public IrLiteral {
private:
    std::string value;

public:
    IrLiteralStringContent(const std::string& value, const TSNode& node)
        : IrLiteral(node), Ir(node), value(value) {}

    const std::string& getValue() const {
        return value;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        return indentSpace + "|--stringContent: " + value + "\n";
    }

    std::string toString() const{
        return "IrLiteralStringContent";
    }
};


class IrLiteralString : public IrLiteral {
private:
    IrLiteralStringContent* stringContent;

public:
    IrLiteralString(IrLiteralStringContent* stringContent, const TSNode& node)
        : IrLiteral(node), Ir(node), stringContent(stringContent) {}

    ~IrLiteralString() {
        delete stringContent;
    }

    const std::string& getValue() const {
        return stringContent->getValue();
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--StringLiteral\n";
        prettyPrint += stringContent->prettyPrint(addIndent(indentSpace));
        return prettyPrint;
    }

    std::string toString() const{
        return "IrLiteralString";
    }

    LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override{
        LlLiteralString * llLiteralString = new LlLiteralString(new string(this->stringContent->getValue()));
        LlLocationVar *llLocationVar = builder.generateTemp();
        LlAssignStmtRegular* regularAssignment  = new LlAssignStmtRegular(llLocationVar, llLiteralString);
        builder.appendStatement(regularAssignment);
        return llLocationVar;
    }
};

#endif