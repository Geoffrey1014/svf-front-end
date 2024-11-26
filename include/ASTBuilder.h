// AST builder
#include <stack>
#include "IrDecl.h"

class ASTBuilder {
    private:
    std::stack<Ir*> ast_stack;
    std::vector<Ir*> nodes;
    const string* source_code;
    const TSLanguage* language;
public:
    ASTBuilder(const string* source_code, const TSLanguage* language): source_code(source_code), language(language) {
        ast_stack = std::stack<Ir*>();
        nodes = std::vector<Ir*>();
    }

    ~ASTBuilder() {
        for (auto node : nodes) {
            delete node;
        }
        delete source_code;
    }

    std::string* getNodeText(TSNode node) {
        unsigned start = ts_node_start_byte(node);
        unsigned end = ts_node_end_byte(node);
        return new std::string(source_code->substr(start, end - start));
    }


    void exitIdentifier(TSNode cst_node);
    void exitPrimitiveType(TSNode cst_node);
    void exitParameter(TSNode cst_node);

    // Function to create an AST node from a CST node
    Ir* exit_ast_node(TSNode cst_node);

    // travese the tree
    void traverse_tree(TSNode cursor);

    void build(TSNode cursor) {
        // traverse the tree
        traverse_tree(cursor);
    }
};