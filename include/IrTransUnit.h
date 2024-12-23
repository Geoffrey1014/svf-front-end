#ifndef IR_TRANS_UNIT_H
#define IR_TRANS_UNIT_H
#include "Ir/Ir.h"
#include "Ir/IrDecl.h"
#include "LlBuilderList.h"
#include "Ir/IrPreprocInclude.h"
#include "Ir/IrTypeComposite.h"

class IrTransUnit : public Ir {
private:
    std::vector<IrDecl*> declerationList;
    std::vector<IrFunctionDef*> functionList;
    std::vector<IrPreprocInclude*> preprocIncludeList;
    std::vector<IrTypeDef*> typeDefList;
public:
    IrTransUnit(const TSNode& node) : Ir(node) {}
    ~IrTransUnit() {
        for (Ir* f: this->functionList) {
            delete f;
        }
        for (Ir* d: this->declerationList) {
            delete d;
        }
        for (Ir* p: this->preprocIncludeList) {
            delete p;
        }
        for (Ir* t: this->typeDefList) {
            delete t;
        }
    }

    void addToDeclarationList(IrDecl* newDecleration) {
        this->declerationList.push_back(newDecleration);
    }

    void addToFunctionList(IrFunctionDef* newFunction) {
        this->functionList.push_back(newFunction);
    }

    void addToPreprocIncludeList(IrPreprocInclude* newPreprocInclude) {
        this->preprocIncludeList.push_back(newPreprocInclude);
    }

    void addToTypeDefList(IrTypeDef* newTypeDef) {
        this->typeDefList.push_back(newTypeDef);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--transUnit:\n";

        for (Ir* decleration: this->declerationList) {
            prettyString += decleration->prettyPrint(addIndent(indentSpace));
        }

        for (Ir* preprocInclude: this->preprocIncludeList) {
            prettyString += preprocInclude->prettyPrint(addIndent(indentSpace));
        }

        for (Ir* transUnit: this->functionList) {
            prettyString += transUnit->prettyPrint(addIndent(indentSpace));
        }

        for (Ir* typeDef: this->typeDefList) {
            prettyString += typeDef->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }

    LlBuildersList* getLlBuilder();
};

#endif