#include <memory>
#include <iostream>
#include <map>
#include "ASTBuilder.h"

enum class ASTNodeType {
    PrimitiveType,
    Identifier,
    Declaration,
    ParameterList,
    Parameter,
    FunctionDeclarator,
    NumberLiteral,
    ReturnStatement,
    CompoundStatement,
    FunctionDefinition,
    ArgumentList,
    CallExpression,
    TranslationUnit,
    Unknown
};

static std::map<std::string, ASTNodeType> ASTNodeTypeMap = {
    {"primitive_type", ASTNodeType::PrimitiveType},
    {"identifier", ASTNodeType::Identifier},
    {"declaration", ASTNodeType::Declaration},
    {"parameter_list", ASTNodeType::ParameterList},
    {"parameter_declaration", ASTNodeType::Parameter},
    {"function_declarator", ASTNodeType::FunctionDeclarator},
    {"number_literal", ASTNodeType::NumberLiteral},
    {"return_statement", ASTNodeType::ReturnStatement},
    {"compound_statement", ASTNodeType::CompoundStatement},
    {"function_definition", ASTNodeType::FunctionDefinition},
    {"argument_list", ASTNodeType::ArgumentList},
    {"call_expression", ASTNodeType::CallExpression},
    {"translation_unit", ASTNodeType::TranslationUnit}
};  // TODO: replace with ts_language_symbol_for_name & ts_node_symbol




void ASTBuilder::exitPrimitiveType(TSNode cst_node) {
    std::string* node_text = getNodeText(cst_node);
    Ir* node =nullptr;
    if (*node_text == "int") {
        node = new IrTypeInt(&cst_node);
        this->ast_stack.push(node);
        std::cout << node->prettyPrint(" ") << std::endl;
    } 
    else if (*node_text == "void")
    {
        node = new IrTypeVoid(&cst_node);
        this->ast_stack.push(node);
        std::cout << node->prettyPrint(" ") << std::endl;
    }
    else {
        std::cerr << "Error: Unknown primitive type" << std::endl;
    }
}

void ASTBuilder::exitIdentifier(TSNode cst_node) {
    std::string* node_text = getNodeText(cst_node);
    Ir* node = new IrIdent(node_text, &cst_node);
    this->ast_stack.push(node);
    std::cout << node->prettyPrint(" ") << std::endl;
}


void ASTBuilder::exitParameter(TSNode cst_node) {
    Ir* node =nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for parameter declaration" << std::endl;
    }
    // Declare variables outside the switch statement

    IrIdent* paramName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrType* paramType = dynamic_cast<IrType*>( this->ast_stack.top());
    this->ast_stack.pop();


    if (paramType && paramName) {
        node = new IrParamDecl(paramType, paramName, &cst_node);
        this->ast_stack.push(node);
        std::cout << node->prettyPrint(" ") << std::endl;
    } else {
        std::cerr << "Error: Invalid parameter type or name" << std::endl;
    }
}

// Function to create an AST node from a CST node
void ASTBuilder::exit_ast_node(TSNode cst_node) {
    const char* type = ts_node_type(cst_node);
    TSSymbol symbol_type = ts_node_symbol(cst_node);
    std::cout << "Creating AST node: " << ts_language_symbol_name(this->language, symbol_type) << ", symbol_type id:"<< std::to_string(symbol_type) << std::endl;
    ASTNodeType ast_node_type;
    auto it = ASTNodeTypeMap.find(type);
    if (it != ASTNodeTypeMap.end()) {
        ast_node_type = it->second;
    } else {
        ast_node_type = ASTNodeType::Unknown;
    }

    switch (ast_node_type)
    {
        case ASTNodeType::PrimitiveType:
            exitPrimitiveType(cst_node);
            break;
        case ASTNodeType::Identifier:
            exitIdentifier(cst_node);
            break;
        case ASTNodeType::Parameter:
            exitParameter(cst_node);
            break;
        case ASTNodeType::ParameterList:
            /* code */
            break;
        case ASTNodeType::FunctionDeclarator:

            break;
        case ASTNodeType::NumberLiteral:
            /* code */
            break;
        case ASTNodeType::ReturnStatement:
            /* code */
            break;
        case ASTNodeType::CompoundStatement:
            /* code */
            break;
        case ASTNodeType::FunctionDefinition:
            /* code */
            break;
        case ASTNodeType::ArgumentList:
            /* code */
            break;
        case ASTNodeType::CallExpression:
            /* code */
            break;
        case ASTNodeType::TranslationUnit:

            break;
        case ASTNodeType::Unknown:
            /* code */
            break;
        
        default:
            break;
    }

    return;
}

// travese the tree
void ASTBuilder::traverse_tree(TSNode cursor) {

    uint32_t named_child_count = ts_node_named_child_count(cursor);
    for (uint32_t i = 0; i < named_child_count; i++) {
        TSNode child = ts_node_named_child(cursor, i);
        traverse_tree(child);
    }

    // uint32_t child_count = ts_node_child_count(cursor);
    // for (uint32_t i = 0; i < child_count; i++) {
    //     TSNode child = ts_node_child(cursor, i);
    //     traverse_tree(child);
    // }


    exit_ast_node(cursor);

}