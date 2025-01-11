#include "IrTransUnit.h"
#include "main.h"

LlBuildersList* IrTransUnit::getLlBuilder() {

    LlBuildersList* llBuildersList = new LlBuildersList();

    LlBuilder* builderGlobal = new LlBuilder(("global_decl"));
    SymbolTable* symbolTableGlobal = new SymbolTable("global_decl");
    for (IrDecl* decl: this->declerationList) {
        decl->generateLlIr(*builderGlobal, *symbolTableGlobal);
    }

    llBuildersList->addBuilder(builderGlobal);
    llBuildersList->addSymbolTable(symbolTableGlobal);

    for (IrFunctionDef* func: this->functionList) {

        LlBuilder* builder = new LlBuilder(func->getFunctionName());

        for (IrParamDecl* p: func->getFunctionDecl()->getParamsList()->getParamsList()) {
            std::string name = p->getDeclarator()->getName();
            builder->addParam(new LlLocationVar(&name));
        }

        SymbolTable* symbolTable = new SymbolTable(func->getFunctionName());
        func->generateLlIr(*builder, *symbolTable);
        if (program.is_used("--intermedial")){
            cout << symbolTable->toString() << endl;
            cout << builder->toString() << endl;
        }
            

        
        llBuildersList->addBuilder(builder);
        llBuildersList->addSymbolTable(symbolTable);


    }


    return llBuildersList;
}