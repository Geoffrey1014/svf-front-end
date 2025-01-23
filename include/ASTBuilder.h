// AST builder

#ifndef AST_BUILDER_H
#define AST_BUILDER_H
#include <stack>
#include "Ir/IrDecl.h"
#include "Ir/IrLiteral.h"
#include "Ir/IrArg.h"
#include "Ir/IrStatement.h"
#include "Ir/IrNonBinaryExpr.h"
#include "IrTransUnit.h"
#include "Ir/IrPreprocInclude.h"
#include "Ir/IrStorageClassSpecifier.h"
#include "Ir/IrSubscriptExpr.h"
#include "Ir/IrDeclarator.h"
#include "Ir/IrTypeComposite.h"
#include "Ir/IrPreproc.h"
#include "Ir/IrForStmt.h"

#include "main.h"


class ASTBuilder {
    private:
    std::stack<Ir*> ast_stack;
    const std::string* source_code;
    const TSLanguage* language;
    Ir* root_node;
    int arraylevel = 0;
public:
    ASTBuilder(const string* source_code, const TSLanguage* language)
        : source_code(source_code), language(language), ast_stack(),root_node(nullptr) {
    }

    ~ASTBuilder() {
        delete source_code;
    }

    template <typename T>
    T* popFromStack(const TSNode& cst_node);
    
    std::string getNodeText(const TSNode &cst_node);

    void exitIdentifier(const TSNode & cst_node);
    void exitPrimitiveType(const TSNode & cst_node);
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
    void exitStringContent(const TSNode & cst_node);
    void exitLiteralString(const TSNode & cst_node);
    void exitPreprocInclude(const TSNode & cst_node);
    void exitStorageClassSpecifier(const TSNode & cst_node);
    void exitArrayDeclarator(const TSNode & cst_node);
    void exitSubscriptExpression(const TSNode & cst_node);
    void exitInitDeclarator(const TSNode & cst_node);
    void exitParameter(const TSNode & cst_node);
    void exitAbstractPointerDeclarator(const TSNode & cst_node);
    void exitPointerDeclarator(const TSNode & cst_node);
    void exitFieldDeclaration(const TSNode & cst_node);
    void exitFieldDeclarationList(const TSNode & cst_node);
    void exitStructSpecifier(const TSNode & cst_node);
    void exitTypeDefinition(const TSNode & cst_node);
    void exitTypeIdentifier(const TSNode & cst_node);
    void exitFieldExpression(const TSNode & cst_node);
    void exitPointerExpression(const TSNode & cst_node);
    void exitPreprocArg(const TSNode & cst_node);
    void exitPreprocDef(const TSNode & cst_node);
    void exitInitList(const TSNode & cst_node);
    void exitParenthesizedExpr(const TSNode & cst_node);
    void exitIfStatement(const TSNode & cst_node);
    void exitElseClause(const TSNode & cst_node);
    void exitForStatement(const TSNode & cst_node);

    void debugStackState() const;

    void exitTransUnit(const TSNode & cst_node);



    void exit_cst_node(const TSNode & cst_node);
    void enter_cst_node(const TSNode & cst_node);

    // travese the tree
    void traverse_tree(const TSNode & cursor);

    /// build the AST, return the root node. The user should delete the returned pointer.
    /// @param cst_root the root node of the CST
    /// @return the root node of the AST
    Ir* build(const TSNode &cst_root) {
        traverse_tree(cst_root);
        if (ast_stack.empty()) {
            std::cerr << "Error: AST stack is empty after traversal." << std::endl;
            return nullptr;
        }
        root_node = ast_stack.top();
        ast_stack.pop();
        return root_node;
    }
};

#include "ASTBuilder.tpp"
#endif