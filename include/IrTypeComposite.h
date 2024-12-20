#ifndef IR_TYPE_COMPOSITE_H
#define IR_TYPE_COMPOSITE_H

#include "IrType.h"
#include "IrDeclarator.h"
#include "IrDecl.h"

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
            prettyString += name->prettyPrint(indentSpace + "  ");
        }
        prettyString += fieldDeclList->prettyPrint(indentSpace + "  ");
        return prettyString;
    }

};

class IrTypeDef : public Ir {
private:
    IrType* type;     // The original type being aliased
    IrIdent* alias;   // The alias name

public:
    IrTypeDef(IrType* type, IrIdent* alias, const TSNode& node)
        : Ir(node), type(type), alias(alias) {}

    ~IrTypeDef() {
        delete type;
        delete alias;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--typedef:\n";
        prettyString += type->prettyPrint(indentSpace + "  ");
        prettyString += alias->prettyPrint(indentSpace + "  ");
        return prettyString;
    }
};


#endif
