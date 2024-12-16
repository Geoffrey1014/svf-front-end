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
            prettyString += baseExpr->prettyPrint("    " + indentSpace);
        } else {
            prettyString += "    " + indentSpace + "|--Error: Missing base expression\n";
        }

        // Print index expression
        if (indexExpr) {
            prettyString += indexExpr->prettyPrint("    " + indentSpace);
        } else {
            prettyString += "    " + indentSpace + "|--Error: Missing index expression\n";
        }

        return prettyString;
    }
};

#endif
