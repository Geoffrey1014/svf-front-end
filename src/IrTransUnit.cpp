#include "IrTransUnit.h"
#include "main.h"

LlBuildersList* IrTransUnit::getLlBuilder() {

    LlBuildersList* llBuildersList = new LlBuildersList();

    // Process function definitions
    for (IrFunctionDef* func: this->functionList) {

        // Create a builder for the function
        LlBuilder* builder = new LlBuilder(func->getFunctionName());

        // Create a symbol table for the function, with the global symbol table as its parent
        SymbolTable* symbolTable = new SymbolTable(func->getFunctionName(), nullptr);

        // for (IrParamDecl* p: func->getFunctionDecl()->getParamsList()->getParamsList()) {
        //     std::string name = p->getDeclarator()->getName();
        //     builder->addParam(new LlLocationVar(&name));
        // }

        // Generate LL IR for the function
        func->generateLlIr(*builder, *symbolTable);
        
        llBuildersList->addBuilder(builder);
        llBuildersList->addSymbolTable(symbolTable);
        // // Debugging
        // if (program.is_used("--intermedial")){
        //     cout << symbolTable->toString() << endl;
        //     cout << builder->toString() << endl;
        // }
    }


    return llBuildersList;
}