#include <memory>
#include <iostream>
#include <map>
#include <cstring>
#include "ASTBuilder.h"

void ASTBuilder::exitPrimitiveType(const TSNode & cst_node) {
    std::string* node_text = getNodeText(cst_node);
    Ir* node =nullptr;
    if (*node_text == "int") {
        node = new IrTypeInt(cst_node);
        this->ast_stack.push(node);
    } 
    else if (*node_text == "void")
    {
        node = new IrTypeVoid(cst_node);
        this->ast_stack.push(node);
    }
    else {
        std::cerr << "Error: Unknown primitive type" << std::endl;
    }
}

void ASTBuilder::exitIdentifier(const TSNode & cst_node) {
    std::string* node_text = getNodeText(cst_node);
    Ir* node = new IrIdent(node_text, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitParameter(const TSNode & cst_node) {
    Ir* node =nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for parameter declaration" << std::endl;
    }

    IrIdent* paramName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrType* paramType = dynamic_cast<IrType*>( this->ast_stack.top());
    this->ast_stack.pop();


    if (paramType && paramName) {
        node = new IrParamDecl(paramType, paramName, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid parameter type or name" << std::endl;
    }
}

void ASTBuilder::exitDeclaration(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for declaration" << std::endl;
    }

    IrIdent* declName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrType* declType = dynamic_cast<IrType*>( this->ast_stack.top());
    this->ast_stack.pop();

    if (declType && declName) {
        node = new IrDecl(declName, declType, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid declaration type or name" << std::endl;
    }
}

void ASTBuilder::exitParamList(const TSNode & cst_node){

    IrParamList* paramList = new IrParamList(cst_node);
    IrParamDecl* paramDecl = dynamic_cast<IrParamDecl*>(this->ast_stack.top());

    while (paramDecl) {
        this->ast_stack.pop();
        paramList->addToParamsList(paramDecl);
        paramDecl = dynamic_cast<IrParamDecl*>(this->ast_stack.top());
    }

    this->ast_stack.push(paramList);
}

void ASTBuilder::exitFunctionDeclarator(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for function declaration" << std::endl;
    }

    IrParamList* paramList = dynamic_cast<IrParamList*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrIdent* declName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (declName && paramList) {
        node = new IrFunctionDecl(declName, paramList, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid function declaration type or name" << std::endl;
    }
}

void ASTBuilder::exitFunctionDefinition(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 3) {
        std::cerr << "Error: Not enough elements on the stack for function definition" << std::endl;
    }

    IrCompoundStmt* compoundStmt = dynamic_cast<IrCompoundStmt*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrFunctionDecl* functionDecl = dynamic_cast<IrFunctionDecl*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrType* returnType = dynamic_cast<IrType*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (returnType && functionDecl && compoundStmt) {
        node = new IrFunctionDef(returnType, functionDecl, compoundStmt, cst_node);
        this->ast_stack.push(node);
        std::cout << node->prettyPrint(" ") << std::endl;
    } else {
        std::cerr << "Error: Invalid function definition" << std::endl;
    }
}

void ASTBuilder::exitBinaryExpr(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for binary expression" << std::endl;
    }

    IrExpr* rightOperand = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrExpr* leftOperand = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    // Get the operation
    // get the second child of the cst_node
    TSNode second_child = ts_node_child(cst_node, 1);
    std::string* operation = getNodeText(second_child); 
    // std::cout << "Operation: " << *operation << std::endl;

    if (leftOperand && rightOperand) {
        node = new IrBinaryExpr(operation, leftOperand, rightOperand, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid binary expression" << std::endl;
    }
}

void ASTBuilder:: exitReturnStatement(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for return statement" << std::endl;
    }

    IrExpr* result = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (result) {
        node = new IrStmtReturnExpr(result,cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid return statement" << std::endl;
    }
}

void ASTBuilder:: exitCompoundStatement(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for compound statement" << std::endl;
    }

    IrCompoundStmt* compoundStmt = new IrCompoundStmt(cst_node);
    IrStatement* stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
    while (stmt) {
        this->ast_stack.pop();
        compoundStmt->addStmt(stmt);
        stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
    }
    this->ast_stack.push(compoundStmt);
}

void ASTBuilder::exitLiteralNumber(const TSNode & cst_node){
    std::string* node_text = getNodeText(cst_node);
    IrLiteralNumber* node = new IrLiteralNumber(std::stoi(*node_text), cst_node);
    std::cout << node->prettyPrint("") << std::endl;
    this->ast_stack.push(node);
}

// Function to create an AST node from a CST node
void ASTBuilder::exit_cst_node(const TSNode & cst_node) {
    const char* type = ts_node_type(cst_node);
    TSSymbol symbol_type = ts_node_symbol(cst_node);
    
    if (ts_node_is_named(cst_node))
        std::cout << "Exiting CST node: "<< "Named, " << ts_language_symbol_name(this->language, symbol_type) << ", symbol_type id:"<< std::to_string(symbol_type) << std::endl;
    else
        std::cout << "Exiting CST node: " << "Not Named, " << ts_language_symbol_name(this->language, symbol_type) << ", symbol_type id:"<< std::to_string(symbol_type) << std::endl;
    // std::cout << "ts_language_symbol_name: " << ts_language_symbol_name(this->language, symbol_type) << std::endl;
    // std::cout << "ts_language_symbol_for_name: " <<  ts_language_symbol_for_name(this->language, type, std::strlen(type), true) << std::endl;

    switch (symbol_type)
    {
        case 1: // Identifier
            exitIdentifier(cst_node);
            break;
        case 93: // primitive_type
            exitPrimitiveType(cst_node);
            break;
        case 260: // parameter_declaration
            exitParameter(cst_node);
            break;
        case 198: // declaration
            exitDeclaration(cst_node);
            break;
        case 258: // parameter_list
            exitParamList(cst_node);
            break;
        case 230: // function_declarator
            exitFunctionDeclarator(cst_node);
            break;
        case 290: // function_definition
            exitBinaryExpr(cst_node);
            break;
        case 141: // literal_number
            exitLiteralNumber(cst_node);
            break;
        case 275: // return_statement
            exitReturnStatement(cst_node);
            break;
        case 241: // compound_statement
            exitCompoundStatement(cst_node);
            break;
        case 196: // function_definition
            exitFunctionDefinition(cst_node);
            break;
        // case ASTNodeType::ArgumentList:
        //     /* code */
        //     break;
        // case ASTNodeType::CallExpression:
        //     /* code */
        //     break;
        // case ASTNodeType::TranslationUnit:

        //     break;
        // case ASTNodeType::Unknown:
        //     /* code */
        //     break;
        
        default:
            std::cerr << "Error: Unknown CST node type" << std::endl;
            break;
    }

    return;
}

void ASTBuilder::enter_cst_node(const TSNode & cst_node){
    // TSSymbol symbol_type = ts_node_symbol(cst_node);
    // if (symbol_type == 290){
    //     std::cout << "ENTERING CST node: " << ts_language_symbol_name(this->language, symbol_type) <<", " << ts_node_grammar_type(cst_node) << std::endl;
    // }
    
}

void ASTBuilder::traverse_tree(const TSNode & node) {

    enter_cst_node(node);

    uint32_t named_child_count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < named_child_count; i++) {
        TSNode child = ts_node_named_child(node, i);
        traverse_tree(child);
    }

    // uint32_t child_count = ts_node_child_count(node);
    // for (uint32_t i = 0; i < child_count; i++) {
    //     TSNode child = ts_node_child(node, i);
    //     traverse_tree(child);
    // }


    exit_cst_node(node);

}