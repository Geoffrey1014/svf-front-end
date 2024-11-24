#include "Ir.h"


class IrType : public Ir {
public:
    IrType(int lineNumber, int colNumber) : Ir(lineNumber, colNumber) {}
    virtual ~IrType() = default; // Ensure a virtual destructor for proper cleanup of derived classes


    // std::string semanticCheck(ScopeStack scopeStack) {
    //     return "";
    // }

    int hashCode() const;
};

class IrTypeBool : public IrType {
public:
    IrTypeBool(int lineNumber, int colNumber) : IrType(lineNumber, colNumber) {}

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
        return 17; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: bool\n";
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        // Placeholder function, assuming external classes LlBuilder and LlSymbolTable
        return nullptr;
    }
};


class IrTypeInt : public IrType {
public:
    IrTypeInt(int lineNumber, int colNumber) : IrType(lineNumber, colNumber) {}

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
        return 11; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--type: int\n";
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        // Placeholder function, assuming external classes LlBuilder and LlSymbolTable
        return nullptr;
    }
};


class IrTypeVoid : public IrType {
public:
    IrTypeVoid(int lineNumber, int colNumber) : IrType(lineNumber, colNumber) {}

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

    LlLocation* generateLlIr(LlBuilder builder, LlSymbolTable symbolTable) {
        // Placeholder function, assuming external classes LlBuilder and LlSymbolTable
        return nullptr;
    }
};

class IrTypeString : public IrType {
public:
    IrTypeString(int lineNumber, int colNumber) : IrType(lineNumber, colNumber) {}

    std::string getTypeName() {
        return "String";
    }

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

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        // Placeholder function, assuming external classes LlBuilder and LlSymbolTable
        return nullptr;
    }

    // Other methods specific to string type...
};