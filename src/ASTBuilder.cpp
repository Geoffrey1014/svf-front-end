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
};

// Function to create an AST node from a CST node
std::shared_ptr<ASTNode> ASTBuilder::create_ast_node(TSNode cst_node) {
    const char* type = ts_node_type(cst_node);
    TSSymbol symbol_type = ts_node_symbol(cst_node);
    std::cout << "Creating AST node: " << type << ", symbol_type id:"<< std::to_string(symbol_type) << std::endl;
    ASTNodeType ast_node_type = ASTNodeTypeMap[type];
    std::shared_ptr<ASTNode> n;
    char* s = ts_node_string(cst_node);
    unsigned start = ts_node_start_byte(cst_node);
    unsigned end = ts_node_end_byte(cst_node);

    // Assuming `source_code` is a string containing the original source code
    std::string source_code_substring = source_code->substr(start, end - start);

    std::cout << "string:" << source_code_substring << std::endl;

    switch (ast_node_type)
    {
    case ASTNodeType::PrimitiveType:
        std::cout << "PrimitiveType" << std::endl;
        
        
        // if (ts_node_string_is(cst_node, "void"))
        // {
        //     n = std::make_shared<ASTTypeVoid>(cst_node);
        //     std::cout << n->prettyPrint(" ") << std::endl;
        // }
        
        
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

// traverse all children
uint32_t named_child_count = ts_node_named_child_count(cursor);
for (uint32_t i = 0; i < named_child_count; i++) {
    TSNode child = ts_node_named_child(cursor, i);
    traverse_tree(child);
}
create_ast_node(cursor);

}