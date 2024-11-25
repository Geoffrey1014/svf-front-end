#include "Ir.h"
#ifndef IR_TYPE_H
#define IR_TYPE_H

class IrType : public Ir {
    public:
        IrType(TSNode* node) : Ir(node) {}
        virtual ~IrType() = default;
        int hashCode() const;
};

class IrTypeBool : public IrType {
public:
    IrTypeBool(TSNode* node) : IrType(node) {}

    bool operator==(const Ir& that) const {
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

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: bool\n";
    }
};

class IrTypeVoid : public IrType {
public:
    IrTypeVoid(TSNode* node) : IrType(node) {}

    bool operator==(const Ir& that) const {
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

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: void\n";
    }
};

class IrTypeInt : public IrType {
public:
    IrTypeInt(TSNode* node) : IrType(node) {}

    bool operator==(const Ir& that) const {
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

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: int\n";
    }
};

class IrTypeString : public IrType {
public:
    IrTypeString(TSNode* node) : IrType(node) {}

    bool operator==(const Ir& that) const {
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

     std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: string\n";
    }

    // Other methods specific to string type...
};
#endif