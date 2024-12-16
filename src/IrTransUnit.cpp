#include "IrTransUnit.h"

LlBuildersList* IrTransUnit::getLlBuilder() {
    LlBuildersList* llBuildersList = new LlBuildersList();
    for (IrDecl* decl: this->declerationList) {
        LlLocationVar* var = new LlLocationVar((decl->getName()));
        llBuildersList->addToGlobalVars(var);
        cout << "Decleration: " << decl->getColNumber() << endl;
    }

    for (IrFunctionDef* func: this->functionList) {
        cout << "Function: " << func->getColNumber() << endl;
    }


    return llBuildersList;
}