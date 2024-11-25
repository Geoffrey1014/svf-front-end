#include <memory>
#include <iostream>
#include "ASTBuilder.h"
#include <map>

enum class ASTNodeType {
    PrimitiveType,
    Identifier,
    ParameterList,
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
    {"parameter_list", ASTNodeType::ParameterList},
    {"function_declarator", ASTNodeType::FunctionDeclarator},
    {"number_literal", ASTNodeType::NumberLiteral},
    {"return_statement", ASTNodeType::ReturnStatement},
    {"compound_statement", ASTNodeType::CompoundStatement},
    {"function_definition", ASTNodeType::FunctionDefinition},
    {"argument_list", ASTNodeType::ArgumentList},
    {"call_expression", ASTNodeType::CallExpression},
    {"translation_unit", ASTNodeType::TranslationUnit},
    {"unknown", ASTNodeType::Unknown}
};  // TODO: replace with ts_language_symbol_for_name & ts_node_symbol

// Function to create an AST node from a CST node
std::shared_ptr<ASTNode> ASTBuilder::create_ast_node(TSNode cst_node) {
    const char* type = ts_node_type(cst_node);
    TSSymbol symbol_type = ts_node_symbol(cst_node);
    std::cout << "Creating AST node: " << type << ", symbol_type id:"<< std::to_string(symbol_type) << std::endl;
    ASTNodeType ast_node_type = ASTNodeTypeMap[type];
    std::shared_ptr<ASTNode> n;

    std::string* node_text;

    switch (ast_node_type)
    {
        case ASTNodeType::PrimitiveType:
            std::cout << "PrimitiveType" << std::endl;
            node_text = getNodeText(cst_node);
            
            if (*node_text == "int") {
                n = std::make_shared<ASTTypeInt>(cst_node);
                std::cout << n->prettyPrint(" ") << std::endl;
            } 
            else if (*node_text == "void")
            {
                n = std::make_shared<ASTTypeVoid>(cst_node);
                std::cout << n->prettyPrint(" ") << std::endl;
            }
            
            break;
        case ASTNodeType::Identifier:
            std::cout << "Identifier" << std::endl;
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

    return n;
}

// travese the tree
void ASTBuilder::traverse_tree(TSNode cursor) {

uint32_t named_child_count = ts_node_named_child_count(cursor);
for (uint32_t i = 0; i < named_child_count; i++) {
    TSNode child = ts_node_named_child(cursor, i);
    traverse_tree(child);
}
create_ast_node(cursor);

}