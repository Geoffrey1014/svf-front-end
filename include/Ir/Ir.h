#ifndef IR_H
#define IR_H
#include <string>
#include <tree_sitter/api.h> 
#include <vector>
#include "Ll.h"
#include "LlLocationStruct.h"
#include "LlBuilder.h"
#include "SymbolTable.h"

using namespace std;

class Ir {
    private:
        const TSNode & node;
    public:
    Ir(const TSNode & node): node(node) {}
    virtual ~Ir() = default;

    int getLineNumber() {
        return ts_node_start_point(node).row;
    }

    int getColNumber() {
        return ts_node_start_point(node).column;
    }

    const TSNode& getNode() {
        return node;
    }

    virtual bool operator==(const Ir& other) const {
        return this == &other;
    }

    // virtual std::string semanticCheck(ScopeStack& scopeStack) = 0;
    virtual LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable){
        std::cerr << "Ir Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("Error")); 
    };
    
    virtual std::string prettyPrint(std::string indentSpace) const =0;
    virtual std::string toString() const = 0;
    
    std::string addIndent(const std::string& baseIndent, int level = 1) const {
        return baseIndent + std::string(level * 2, ' '); // 2 spaces per level
    }

};

class IrExpr : public virtual Ir {
public:
    IrExpr(const TSNode & node) : Ir(node) {}
    ~IrExpr() = default;
    
    std::string toString() const override {
        return "baseIrExpr";
    }

    bool operator==(const IrExpr& other) const {
        // Implementation of operator==
        return true;
    }

    int hashCode() const {
        // Implementation of hashCode
        return 0;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        std::cerr << "IrExpr Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("Error")); 
    }

    virtual const std::string getName() const {
        return "";
    }
};


class IrBinaryExpr : public IrExpr {
private:
    std::string operation;
    IrExpr* leftOperand;
    IrExpr* rightOperand;

public:
    IrBinaryExpr(std::string& operation, IrExpr* leftOperand, IrExpr* rightOperand, const TSNode & node) 
        : Ir(node), IrExpr(node), operation(operation), 
          leftOperand(leftOperand), rightOperand(rightOperand) {}
    ~IrBinaryExpr() {
        delete leftOperand;
        delete rightOperand;
    }
    IrExpr* getLeftOperand() {
        return this->leftOperand;
    }

    IrExpr* getRightOperand() {
        return this->rightOperand;
    }

    std::string& getOperation() {
        return this->operation;
    }

    std::string toString() const override{
        return leftOperand->toString() + " " + operation + " " + rightOperand->toString();
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--binaryExpr\n";

        prettyString += addIndent(indentSpace) + + "|--lhs\n";
        prettyString += this->leftOperand->prettyPrint(addIndent(indentSpace, 2));

        prettyString += addIndent(indentSpace) + "|--op: " + operation + "\n";

        prettyString += addIndent(indentSpace) + "|--rhs\n";
        prettyString += this->rightOperand->prettyPrint(addIndent(indentSpace, 2));

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* left = leftOperand->generateLlIr(builder, symbolTable);
        LlLocation* right = rightOperand->generateLlIr(builder, symbolTable);
        LlLocationVar* result = builder.generateTemp();
        builder.appendStatement(new LlAssignStmtBinaryOp(result, left, operation, right));
        return result;
    }
       
};


class IrType : public Ir {
protected:
    int width = 0;

public:
    IrType(const TSNode& node) : Ir(node) {}
    virtual ~IrType() = default;
    int hashCode() const{ return 0;}
    virtual IrType* clone() const = 0;
    int getWidth() const { return width; }
};

class IrTypeBool : public IrType {

public:
    IrTypeBool(const TSNode& node) : IrType(node) {width = 1;}
    ~IrTypeBool() override = default;

    IrTypeBool* clone() const override {
        return new IrTypeBool(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeBool*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 11; // some arbitrary prime
    }

    std::string toString() const override{
        return "bool";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: bool\n";
    }
};

class IrTypeVoid : public IrType {

public:
    IrTypeVoid(const TSNode& node) : IrType(node) {}
    ~IrTypeVoid() override = default;

    IrTypeVoid* clone() const override {
        return new IrTypeVoid(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeVoid*>(&that) == nullptr) {
            return false;
        }

        return true;
    }

    int hashCode() const {
        return 13; // some arbitrary prime
    }

    std::string toString() const override{
        return "void";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: void\n";
    }
};

class IrTypeInt : public IrType {

public:
    IrTypeInt(const TSNode& node) : IrType(node) { width = 4;}
    ~IrTypeInt() override = default;

    IrTypeInt* clone() const override {
        return new IrTypeInt(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeInt*>(&that) == nullptr) {
            return false;
        }

        return true;
    }

    int hashCode() const {
        return 17; // some arbitrary prime
    }

    std::string toString() const override{
        return "int";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: int\n";
    }
};

class IrTypeString : public IrType {

public:
    IrTypeString(const TSNode& node) : IrType(node) {}
    ~IrTypeString() override = default;

    IrTypeString* clone() const override {
        return new IrTypeString(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeString*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 7; // some arbitrary prime
    }

    std::string toString() const override{
        return "string";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: string\n";
    }
};

class IrTypeChar : public IrType {
public:
    IrTypeChar(const TSNode& node) : IrType(node) { width = 1;}
    ~IrTypeChar() override = default;

    IrTypeChar* clone() const override {
        return new IrTypeChar(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeChar*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 19; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: char\n";
    }

    std::string toString() const override{
        return "char";
    }
};

class IrLiteral;
class IrTypeArray : public IrType {
private:
    IrType* baseType;
    deque<IrLiteral*> dimension;
    int width = 0;

public:
    IrTypeArray(IrType* baseType, deque<IrLiteral*> dimension, const TSNode& node)
            : IrType(node), baseType(baseType), dimension(dimension) {}
    ~IrTypeArray();

    IrType* getBaseType() const {
        return baseType;
    }

    deque<IrLiteral*> getDimension() const {
        return dimension;
    }

    int getDimensionSize() const {
        return dimension.size();
    }

    IrTypeArray* clone() const override {
        return new IrTypeArray(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (auto thatTypeArray = dynamic_cast<const IrTypeArray*>(&that)) {
            if (this->baseType != thatTypeArray->baseType) {
                return false;
            }
            if (this->dimension.size() != thatTypeArray->dimension.size()) {
                return false;
            }
            for (int i = 0; i < this->dimension.size(); i++) {
                if (this->dimension[i] != thatTypeArray->dimension[i]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    int hashCode() const;

    std::string prettyPrint(std::string indentSpace) const override ;

    std::string toString() const override;

};

#endif