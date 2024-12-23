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

        const std::string& getName() const {
            return name;
        }

        bool operator==(const Ir& that) const {
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

        std::string prettyPrint(std::string indentSpace) const {
            return indentSpace + "|--typeId: " + name + "\n";
        }
};

// Comment: maybe refactor the IrType (add one layer for primitive types or ...)
class IrTypeStruct : public IrType {
private:
    IrIdent* name;                       // Optional struct name
    IrFieldDeclList* fieldDeclList;      // List of field declarations

public:
    IrTypeStruct(IrIdent* name, IrFieldDeclList* fieldDeclList, const TSNode& node)
        : IrType(node), name(name), fieldDeclList(fieldDeclList) {}

    ~IrTypeStruct() {
        delete name;
        delete fieldDeclList;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--type: struct\n";
        if (name) {
            prettyString += name->prettyPrint(addIndent(indentSpace));
        }
        prettyString += fieldDeclList->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

};

class IrTypeDef : public Ir {
private:
    IrType* type;     // The original type being aliased
    // IrIdent* alias;   // The alias name
    IrTypeIdent* alias;   // The alias name

public:
    IrTypeDef(IrType* type, IrTypeIdent* alias, const TSNode& node)
        : Ir(node), type(type), alias(alias) {}

    ~IrTypeDef() {
        delete type;
        delete alias;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--typedef:\n";
        prettyString += type->prettyPrint(addIndent(indentSpace));
        prettyString += alias->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }
};


#endif
