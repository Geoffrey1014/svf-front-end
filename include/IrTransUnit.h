#ifndef IR_TRANS_UNIT_H
#define IR_TRANS_UNIT_H
#include "Ir/Ir.h"
#include "Ir/IrDecl.h"

class IrTransUnit : public Ir {
private:
    std::vector<IrDecl*> declarationList;
    std::vector<IrFunctionDef*> functionList;
    std::vector<IrStatement*> statementList;
public:
    IrTransUnit(const TSNode& node) : Ir(node) {}
    ~IrTransUnit() {
        for (Ir* f: this->functionList) {
            delete f;
        }
        for (Ir* d: this->declarationList) {
            delete d;
        }
        for (Ir* s: this->statementList) {
            delete s;
        }
    }   

    void addToDeclarationList(IrDecl* newDeclaration) {
        this->declarationList.push_back(newDeclaration);
    }

    void addToFunctionList(IrFunctionDef* newFunction) {
        this->functionList.push_back(newFunction);
    }

    void addToStatementList(IrStatement* newStatement) {
        this->statementList.push_back(newStatement);
    }
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--transUnit:\n";

        for (Ir* declaration: this->declarationList) {
            prettyString += declaration->prettyPrint("  " + indentSpace);
        }
        for (Ir* transUnit: this->functionList) {
            prettyString += transUnit->prettyPrint("  " + indentSpace);
        }
        for (Ir* transUnit: this->statementList) {
            prettyString += transUnit->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }
};

#endif