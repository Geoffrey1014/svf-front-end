// AST builder
#include <stack>
#include "TypeAST.h"


class ASTBuilder {
    private:
    std::stack<shared_ptr<ASTNode>> ast_stack;
    string* source_code;
public:
    ASTBuilder(string* source_code): source_code(source_code) {
        ast_stack = std::stack<shared_ptr<ASTNode>>();
    }

    ~ASTBuilder() {
    }

    // Function to create an AST node from a CST node
    std::shared_ptr<ASTNode> create_ast_node(TSNode cst_node);

    // travese the tree
    void traverse_tree(TSNode cursor);

    void build(TSNode cursor) {
        // traverse the tree
        traverse_tree(cursor);
    }
};