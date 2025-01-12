#ifndef IR_TYPE_COMPOSITE_H
#define IR_TYPE_COMPOSITE_H

#include "IrType.h"
#include "IrDeclarator.h"
#include "IrDecl.h"

class IrTypeIdent : public IrType {
    private:
        const std::string name;
    public:
        IrTypeIdent(const std::string& name, const TSNode& node) 
            : IrType(node), name(name) {}
        ~IrTypeIdent() = default;

        IrTypeIdent* clone() const override {
            return new IrTypeIdent(*this);
        }

        const std::string& getName() const {
            return name;
        }

        bool operator==(const Ir& that) const override{
            if (&that == this) {
                return true;
            }
            if (auto thatIdent = dynamic_cast<const IrTypeIdent*>(&that)) {
                return this->name == thatIdent->name;
            }
            return false;
        }

        int hashCode() const {
            std::hash<std::string> hasher;
            return hasher(this->name);
        }

        std::string prettyPrint(std::string indentSpace) const override{
            return indentSpace + "|--typeId: " + name + "\n";
        }

        std::string toString() const override{
            return name;
        }
};

class IrPointerType : public IrType {
private:
    IrType* baseType;

public:
    IrPointerType(IrType* baseType, const TSNode& node) 
        : IrType(node), baseType(baseType) {}

    ~IrPointerType() { delete baseType; }

    std::string toString() const override {
        return baseType->toString() + "*";
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string result = indentSpace + "|--pointer: *\n";
        result += baseType->prettyPrint(addIndent(indentSpace));
        return result;
    }

    IrType* getBaseType() const {
        return baseType;
    }

    IrPointerType* clone() const override {
        return new IrPointerType(*this);
    }
};

// Comment: maybe refactor the IrType (add one layer for primitive types or ...)
class IrTypeStruct : public IrType {
private:
    IrIdent* name;                       
    IrFieldDeclList* fieldDeclList;      // List of field declarations

public:
    IrTypeStruct(IrIdent* name, IrFieldDeclList* fieldDeclList, const TSNode& node)
        : IrType(node), name(name), fieldDeclList(fieldDeclList) {}

    ~IrTypeStruct() {
        delete name;
        delete fieldDeclList;
    }

    IrTypeStruct* clone() const override {
        return new IrTypeStruct(*this);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--type: struct\n";
        if (name) {
            prettyString += name->prettyPrint(addIndent(indentSpace));
        }
        prettyString += fieldDeclList->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        std::string str = "struct ";
        if (name) {
            str += name->toString();
        }
        return str + " {" + fieldDeclList->toString() + "}";
    }
};

class IrTypeDef : public IrType {
private:
    IrType* type;        // The original type being aliased
    // IrIdent* alias;   // The alias name
    IrTypeIdent* alias;   // The alias name

public:
    IrTypeDef(IrType* type, IrTypeIdent* alias, const TSNode& node)
        : IrType(node), type(type), alias(alias) {}

    ~IrTypeDef() {
        delete type;
        delete alias;
    }

    IrType* getbaseType() const {
        return type;
    }

    IrTypeIdent* getAliasName() const {
        return alias;
    }

    IrTypeDef* clone() const override {
        return new IrTypeDef(*this);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--typedef:\n";
        prettyString += type->prettyPrint(addIndent(indentSpace));
        prettyString += alias->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        return "typedef " + type->toString() + " " + alias->toString();
    }
};


#endif
