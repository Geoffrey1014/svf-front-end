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

    std::string* getNodeText(TSNode node) {
        unsigned start = ts_node_start_byte(node);
        unsigned end = ts_node_end_byte(node);
        return new std::string(source_code->substr(start, end - start));
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