#ifndef IR_STORAGE_CLASS_SPECIFIER_H
#define IR_STORAGE_CLASS_SPECIFIER_H

#include "Ir.h"

class IrStorageClassSpecifier : public Ir {
private:
    std::string specifier;

public:
    IrStorageClassSpecifier(const std::string& specifier, const TSNode& node)
        : Ir(node), specifier(specifier) {}

    const std::string& getSpecifier() const {
        return this->specifier;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        return indentSpace + "|--storageClassSpecifier: " + this->specifier + "\n";
    }
};

#endif