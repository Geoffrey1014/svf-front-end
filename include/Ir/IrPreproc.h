#ifndef IR_PREPROC_H
#define IR_PREPROC_H

#include "Ir.h"

class IrPreprocArg : public Ir {
public:
    std::string text;

    IrPreprocArg(const std::string& t, const TSNode& node)
        : Ir(node), text(t) {}
    
    ~IrPreprocArg() = default;

    std::string prettyPrint(std::string indentSpace) const override {
        return indentSpace + "|-- preproc_arg: " + text + "\n";
    }

    std::string toString() const override {
        return "PreprocArg(" + text + ")";
    }
};

class IrPreprocDef : public Ir {
public:
    IrIdent* name;  
    IrPreprocArg* value; 

    IrPreprocDef(IrIdent* n, const TSNode& node, 
                 IrPreprocArg* v = nullptr)
        : Ir(node), name(n), value(v) {}
    
    ~IrPreprocDef() {
        delete name;
        delete value; 
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string result = indentSpace + "|-- preproc_def: " + name->getValue();
        if (value) result += " " + value->text;
        result += "\n";
        return result;
    }

    std::string toString() const override {
        return "#define " + name->toString() + (value ? " " + value->text : "");
    }
};

#endif
