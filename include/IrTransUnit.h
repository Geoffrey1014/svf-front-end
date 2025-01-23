#ifndef IR_TRANS_UNIT_H
#define IR_TRANS_UNIT_H
#include "Ir/Ir.h"
#include "LlBuilderList.h"
#include <deque>

class IrTransUnit : public Ir {
private:
    std::deque<Ir*> topLevelNodes; 
    std::vector<IrDecl*> declerationList;
    std::vector<IrFunctionDef*> functionList;
    std::vector<IrPreprocInclude*> preprocIncludeList;
    std::vector<IrTypeDef*> typeDefList;
    std::vector<IrPreprocDef*> preprocDefList;
    std::vector<IrExprStmt*> exprStmtList;

public:
    IrTransUnit(const TSNode& node) : Ir(node) {}
    ~IrTransUnit() {
        for (Ir* item : this->topLevelNodes) {
            delete item;
        }
    }

    void addTopLevelNodeFront(Ir* node) {
        this->topLevelNodes.push_front(node);
        // Categorize nodes 
        if (auto* decl = dynamic_cast<IrDecl*>(node)) {
            this->declerationList.push_back(decl);
        } else if (auto* func = dynamic_cast<IrFunctionDef*>(node)) {
            this->functionList.push_back(func);
        } else if (auto* preprocInclude = dynamic_cast<IrPreprocInclude*>(node)) {
            this->preprocIncludeList.push_back(preprocInclude);
        } else if (auto* typeDef = dynamic_cast<IrTypeDef*>(node)) {
            this->typeDefList.push_back(typeDef);
        } else if (auto* preprocDef = dynamic_cast<IrPreprocDef*>(node)) {
            this->preprocDefList.push_back(preprocDef);
        } else if(auto* exprStmt = dynamic_cast<IrExprStmt*>(node)){
            this->exprStmtList.push_back(exprStmt);
        }
        else {
            std::cerr << "Warning: Skipping unrecognized node." << std::endl;
        }
    }

    // Insert at the back when needed
    void addTopLevelNodeBack(Ir* node) {
        this->topLevelNodes.push_back(node);
    }

    LlBuildersList* getLlBuilder();

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--transUnit:\n";
        for (Ir* node : this->topLevelNodes) {
            prettyString += node->prettyPrint(addIndent(indentSpace));
        }
        return prettyString;
    }

    std::string toString() const override {
        std::string result = "IrTransUnit";
        for (auto node : topLevelNodes) {
            result += "\n" + node->toString();
        }
        return result;
    }
};

#endif
