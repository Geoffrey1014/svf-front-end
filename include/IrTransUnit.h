#ifndef IR_TRANS_UNIT_H
#define IR_TRANS_UNIT_H
#include "Ir.h"
#include "IrDecl.h"

class IrTransUnit : public Ir {
private:
    std::vector<IrDecl*> declerationList;
    std::vector<IrFunctionDef*> functionList;
public:
    IrTransUnit(const TSNode& node) : Ir(node) {}
    ~IrTransUnit() {
        for (Ir* f: this->functionList) {
            delete f;
        }
        for (Ir* d: this->declerationList) {
            delete d;
        }
    }   

    void addToDeclerationList(IrDecl* newDecleration) {
        this->declerationList.push_back(newDecleration);
    }

    void addToFunctionList(IrFunctionDef* newFunction) {
        this->functionList.push_back(newFunction);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--transUnit:\n";

        // pretty print statement
        for (Ir* transUnit: this->functionList) {
            prettyString += transUnit->prettyPrint("  " + indentSpace);
        }
        for (Ir* decleration: this->declerationList) {
            prettyString += decleration->prettyPrint("  " + indentSpace);
        }
        return prettyString;
    }
};

#endif