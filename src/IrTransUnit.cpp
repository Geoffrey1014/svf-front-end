#include "IrTransUnit.h"
#include "main.h"

LlBuildersList* IrTransUnit::getLlBuilder() {
    if (program.is_used("--verbose"))
        std::cout << "\n==== LlBuildersList:\n" << std::endl;

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
        func->generateLlIr(*builder, *symbolTable);
        if (program.is_used("--intermedial"))
            cout << builder->toString() << endl;

        
        llBuildersList->addBuilder(builder);
        llBuildersList->addSymbolTable(symbolTable);


    }


    return llBuildersList;
}