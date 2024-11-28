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

    std::string* getNodeText(const TSNode & node) {
        unsigned start = ts_node_start_byte(node);
        unsigned end = ts_node_end_byte(node);
        return new std::string(source_code->substr(start, end - start));
    }


    void exitIdentifier(const TSNode & cst_node);
    void exitPrimitiveType(const TSNode & cst_node);
    void exitParameter(const TSNode & cst_node);
    void exitDeclaration(const TSNode & cst_node);

    void exit_cst_node(const TSNode & cst_node);
    void enter_cst_node(const TSNode & cst_node);

    // travese the tree
    void traverse_tree(const TSNode & cursor);

    void build(const TSNode & root) {
        // traverse the tree
        traverse_tree(root);
    }
};