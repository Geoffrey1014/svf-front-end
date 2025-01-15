#include "IrTransUnit.h"
#include "main.h"

LlBuildersList* IrTransUnit::getLlBuilder() {

    LlBuildersList* llBuildersList = new LlBuildersList();

    LlBuilder* builderGlobal = new LlBuilder(("globalBuilder"));

    SymbolTable* symbolTableGlobal = new SymbolTable("global");
    
    for(IrTypeDef* typeDef: this->typeDefList){
        typeDef->generateLlIr(*builderGlobal, *symbolTableGlobal);
    }

    // JWPersonal Note: we may delete this IrDecl loop as for different types of declarations 
    // we may need to handle it in different ways later
    for (IrDecl* decl: this->declerationList) {
        // Not all declarations are global
        if(auto* func = dynamic_cast<IrFunctionDef*>(decl)){
            continue;
        }
        decl->generateLlIr(*builderGlobal, *symbolTableGlobal);
    }
    // Add global builder and symbol table to the list
    llBuildersList->addBuilder(builderGlobal);
    llBuildersList->addSymbolTable(symbolTableGlobal);

    // Process function definitions (function level)
    for (IrFunctionDef* func: this->functionList) {

        // Create a builder for the function
        LlBuilder* builder = new LlBuilder(func->getFunctionName());

        // Create a symbol table for the function, with the global symbol table as its parent
        SymbolTable* symbolTable = new SymbolTable(func->getFunctionName(), symbolTableGlobal);

        for (IrParamDecl* p: func->getFunctionDecl()->getParamsList()->getParamsList()) {
            std::string name = p->getDeclarator()->getName();
            builder->addParam(new LlLocationVar(&name));
        }
        // Generate LL IR for the function
        func->generateLlIr(*builder, *symbolTable);
        
        llBuildersList->addBuilder(builder);
        llBuildersList->addSymbolTable(symbolTable);
    }

    return llBuildersList;
}