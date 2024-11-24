#include <string>
#include <tree_sitter/api.h> // Add this line to include the definition of TSNode
using namespace std;

class ASTNode {
    private:
        TSNode node;
        std::string type;
    public:
    ASTNode(TSNode node) {
        this->node = node;
    }

    virtual std::string prettyPrint(std::string indentSpace) const =0;

    std::string toString() {
        return "ASTNode: " + this->type + ": " + std::to_string(ts_node_start_point(this->node).row) + ", " + std::to_string(ts_node_start_point(this->node).column) + " - " + std::to_string(ts_node_end_point(this->node).row) + ", " + std::to_string(ts_node_end_point(this->node).column);
    }
};