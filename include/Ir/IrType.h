#ifndef IR_TYPE_H
#define IR_TYPE_H
#include "Ir.h"

class IrType : public Ir {
    public:
        IrType(const TSNode& node) : Ir(node) {}
        virtual ~IrType() = default;
        int hashCode() const{ return 0;}
        virtual IrType* clone() const = 0;
};

class IrTypeBool : public IrType {
public:
    IrTypeBool(const TSNode& node) : IrType(node) {}
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
    IrTypeInt(const TSNode& node) : IrType(node) {}
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
    IrTypeChar(const TSNode& node) : IrType(node) {}
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

public:
    IrTypeArray(IrType* baseType, deque<IrLiteral*> dimension, const TSNode& node)
            : IrType(node), baseType(baseType), dimension(dimension) {}
    ~IrTypeArray() {
        delete baseType;
//        for (auto* dim : dimension) {
//            delete dim;
//        }
    }

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