// AST builder
#include <stack>
#include "IrDecl.h"
#include "IrExpr.h"
#include "IrLiteral.h"
#include "IrBinaryExpr.h"
#include "IrArg.h"
#include "IrStatement.h"
#include "IrNonBinaryExpr.h"

#ifndef AST_BUILDER_H
#define AST_BUILDER_H

class ASTBuilder {
    private:
    std::stack<Ir*> ast_stack;
    const string* source_code;
    const TSLanguage* language;
    Ir* root_node;
public:
    ASTBuilder(const string* source_code, const TSLanguage* language)
        : source_code(source_code), language(language), ast_stack(),root_node(nullptr) {
    }

    ~ASTBuilder() {
        delete source_code;
    }

    std::string* getNodeText(const TSNode & node) {
        unsigned start = ts_node_start_byte(node);
        unsigned end = ts_node_end_byte(node);
        return new std::string(source_code->substr(start, end - start));
    }


    void exitIdentifier(const TSNode & cst_node);
    void exitPrimitiveType(const TSNode & cst_node);
    void exitParameter(const TSNode & cst_node);
    void exitDeclaration(const TSNode & cst_node);
    void exitParamList(const TSNode & cst_node);
    void exitFunctionDeclarator(const TSNode & cst_node);
    void exitFunctionDefinition(const TSNode & cst_node);
    void exitBinaryExpr(const TSNode & cst_node);
    void exitUnaryExpr(const TSNode & cst_node);
    void exitReturnStatement(const TSNode & cst_node);
    void exitCompoundStatement(const TSNode & cst_node);
    void exitLiteralNumber(const TSNode & cst_node);    
    void exitArgList(const TSNode & cst_node);
    void exitCallExpr(const TSNode & cst_node);
    void exitAssignExpr(const TSNode & cst_node);
    void exitExprStmt(const TSNode & cst_node);


    void exit_cst_node(const TSNode & cst_node);
    void enter_cst_node(const TSNode & cst_node);

    // travese the tree
    void traverse_tree(const TSNode & cursor);

    Ir* build(const TSNode & cst_root) {
        // traverse the tree
        traverse_tree(cst_root);
        return root_node;
    }
};
#endif