#ifndef IR_SUBSCRIPT_EXPR_H
#define IR_SUBSCRIPT_EXPR_H

#include "IrExpr.h"
    // subscript_expression: $ => prec(PREC.SUBSCRIPT, seq(
    //   field('argument', $.expression),
    //   '[',
    //   field('index', $.expression),
    //   ']',
    // )),
class IrSubscriptExpr : public IrNonBinaryExpr {
private:
    IrExpr* baseExpr;   // The array or object being indexed
    IrExpr* indexExpr;  // The index expression

public:
    IrSubscriptExpr(IrExpr* baseExpr, IrExpr* indexExpr, const TSNode& node)
        : Ir(node), IrNonBinaryExpr(node), baseExpr(baseExpr), indexExpr(indexExpr) {}

    ~IrSubscriptExpr() {
        delete baseExpr;
        delete indexExpr;
    }

    IrExpr* getBaseExpr() const {
        return baseExpr;
    }

    IrExpr* getIndexExpr() const {
        return indexExpr;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--subscript_expression\n";

        if (baseExpr) {
            prettyString += baseExpr->prettyPrint(addIndent(indentSpace));
        } else {
            prettyString += addIndent(indentSpace) + "|--Error: Missing base expression\n";
        }

        prettyString += addIndent(indentSpace) + "|--index\n";
        // Print index expression
        if (indexExpr) {
             prettyString += indexExpr->prettyPrint(addIndent(indentSpace, 2));
        } else {
            prettyString += addIndent(indentSpace, 2) + "|--Error: Missing index expression\n";
        }
        return prettyString;
    }

    std::string toString() const override{
        return baseExpr->toString() + "[" + indexExpr->toString() + "]";
    }

    LlComponent* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlComponent* indexLocation = indexExpr->generateLlIr(builder, symbolTable);
        std::string* baseName = new std::string(baseExpr->toString());
        LlLocationArray* array = new LlLocationArray(baseName, indexLocation);
        return array;
    }
};

#endif
