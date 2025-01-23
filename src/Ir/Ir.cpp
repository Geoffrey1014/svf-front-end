//
// Created by weigang on 11/1/2025.
//

#include "Ir/Ir.h"

int IrTypeArray::hashCode() const {
    int hash = 0;
    for (auto* dim : dimension) {
        hash += dim->hashCode();
    }
    return baseType->hashCode() + hash;
}

std::string IrTypeArray::prettyPrint(std::string indentSpace) const {
    std::string prettyString = indentSpace + "|--type: array\n";
    prettyString += baseType->prettyPrint(addIndent(indentSpace));
    for (auto* dim : dimension) {
    prettyString += dim->prettyPrint(addIndent(indentSpace));
    }
    return prettyString;
}

std::string IrTypeArray::toString() const{
    std::string str = baseType->toString();
    for (auto* dim : dimension) {
        str += "[" + dim->toString() + "]";
    }
    return str;
}

IrTypeArray::~IrTypeArray() {
    delete baseType;
    for (auto* dim : dimension) {
        delete dim;
    }
}