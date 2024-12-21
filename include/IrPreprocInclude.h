#ifndef IR_PREPROC_INCLUDE_H
#define IR_PREPROC_INCLUDE_H

#include "Ir.h"
#include "IrLiteral.h"

class IrPreprocInclude : public Ir {
private:
    IrLiteralString* path;

public:
    IrPreprocInclude(IrLiteralString* path, const TSNode& node) : Ir(node), path(path) {}
    ~IrPreprocInclude() {
        delete path;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--preprocInclude\n";
        prettyString += path->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};

#endif