// AST builder

#ifndef AST_BUILDER_H
#define AST_BUILDER_H
#include <stack>
#include "IrDecl.h"
#include "IrExpr.h"
#include "IrLiteral.h"
#include "IrBinaryExpr.h"
#include "IrArg.h"
#include "IrStatement.h"
#include "IrNonBinaryExpr.h"
#include "IrTransUnit.h"
#include "main.h"



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
    void exitMutableSpec(const TSNode &cst_node);
    void exitPrimitiveType(const TSNode &cst_node);
    void exitArrayType(const TSNode &cst_node);
    void exitParameter(const TSNode & cst_node);
    void exitDeclaration(const TSNode & cst_node);
    void exitParamList(const TSNode & cst_node);
    void exitFunctionDefinition(const TSNode & cst_node);
    void exitBinaryExpr(const TSNode & cst_node);
    void exitUnaryExpr(const TSNode & cst_node);
    void exitReturnStatement(const TSNode & cst_node);
    void exitCompoundStatement(const TSNode & cst_node);
    void exitLiteralNumber(const TSNode & cst_node);    
    void exitArgList(const TSNode & cst_node);
    void exitCallExpr(const TSNode & cst_node);
    void exitMatchExpr(const TSNode &cst_node);
    void exitLoopExpr(const TSNode &cst_node);
    void exitWhileExpr(const TSNode &cst_node);
    void exitForExpr(const TSNode &cst_node);
    void exitAssignExpr(const TSNode &cst_node);
    void exitIfExpr(const TSNode &cst_node);
    void exitElseClause(const TSNode &cst_node);
    void exitRangeExpr(const TSNode &cst_node);
    void exitParenthesizedExpr(const TSNode &cst_node);
    void exitIndexExpr(const TSNode &cst_node);
    void exitExprStmt(const TSNode & cst_node);
    void exitTransUnit(const TSNode & cst_node);


    void exit_cst_node(const TSNode & cst_node);
    void enter_cst_node(const TSNode & cst_node);

    // travese the tree
    void traverse_tree(const TSNode & cursor);

    /// build the AST, return the root node. The user should delete the returned pointer.
    /// @param cst_root the root node of the CST
    /// @return the root node of the AST
    Ir* build(const TSNode & cst_root) {
        traverse_tree(cst_root);
        root_node = ast_stack.top();
        ast_stack.pop();
        return root_node;
    }
};
#endif