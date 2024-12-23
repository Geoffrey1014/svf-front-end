#include "IrTransUnit.h"
#include "main.h"

LlBuildersList* IrTransUnit::getLlBuilder() {
    
    LlBuildersList* llBuildersList = new LlBuildersList();
    for (IrDecl* decl: this->declerationList) {
        if (program.is_used("--verbose"))
            cout << "Decleration: " << (decl->getName()) << endl;
        std::string name = decl->getName();
        LlLocationVar* var = new LlLocationVar(&name);
        llBuildersList->addToGlobalVars(var);
    }

    for (IrFunctionDef* func: this->functionList) {
        if (program.is_used("--verbose"))
            cout << "Function: " << func->toString() << endl;

        LlBuilder* builder = new LlBuilder(func->toString());
    }


    return llBuildersList;
}