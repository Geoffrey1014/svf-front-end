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
            cout << "Function: " << func->getFunctionName() << endl;
        LlBuilder* builder = new LlBuilder(func->getFunctionName());


        for (IrParamDecl* p: func->getFunctionDecl()->getParamsList()->getParamsList()) {
            std::string name = p->getDeclarator()->getName();
            builder->addParam(new LlLocationVar(&name));
        }

        LlSymbolTable* symbolTable = new LlSymbolTable(func->getFunctionName());
        
        

        llBuildersList->addBuilder(builder);
        llBuildersList->addSymbolTable(symbolTable);


    }


    return llBuildersList;
}