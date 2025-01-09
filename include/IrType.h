#ifndef IR_TYPE_H
#define IR_TYPE_H
#include "Ir.h"
#include "IrExpr.h"

class IrType : public Ir {
    public:
        IrType(const TSNode& node) : Ir(node) {}
        virtual ~IrType() = default;
        int hashCode() const;
};

class IrTypeBool : public IrType {
public:
    IrTypeBool(const TSNode& node) : IrType(node) {}

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
    IrTypeVoid(const TSNode& node) : IrType(node) {}

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
    IrTypeInt(const TSNode& node) : IrType(node) {}

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


class IrTypeFloat : public IrType {
public:
    IrTypeFloat(const TSNode& node) : IrType(node) {}

    bool operator==(const Ir& that) const {
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeFloat*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 19; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: float\n";
    }
};

class IrTypeUInt : public IrType {
public:
    IrTypeUInt(const TSNode& node) : IrType(node) {}

    bool operator==(const Ir& that) const {
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeUInt*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 23; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: uint\n";
    }
};

class IrTypeString : public IrType {
public:
    IrTypeString(const TSNode& node) : IrType(node) {}

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

class IrTypeChar : public IrType {
public:
    IrTypeChar(const TSNode& node) : IrType(node) {}

    bool operator==(const Ir& that) const {
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeChar*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 3; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: char\n";
    }
};

class IrTypeArray : public IrType {
private:
    IrType* elementType;
    IrExpr* arrayLength;
public:
    IrTypeArray(IrType* elementType, IrExpr* arrayLength, const TSNode& node)
        : elementType(elementType), arrayLength(arrayLength), IrType(node) {}
    ~IrTypeArray() {
        delete elementType;
        delete arrayLength;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--array_type\n";
        prettyString += elementType->prettyPrint("  " + indentSpace);
        prettyString += arrayLength->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};

class IrTypeUnit : public IrType {
    private:
    public:
        IrTypeUnit(const TSNode& node) : IrType(node) {}

        bool operator==(const Ir& that) const {
            if (&that == this) {
                return true;
            }
            if (dynamic_cast<const IrTypeUnit*>(&that) == nullptr) {
                return false;
            }
            return true;
        }

        int hashCode() const {
            return 29; // some arbitrary prime
        }

        std::string prettyPrint(std::string indentSpace) const {
            return indentSpace + "|--type: unit\n";
        }
};
#endif