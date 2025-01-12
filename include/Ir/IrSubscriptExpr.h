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
    int level = 0;

public:
    IrSubscriptExpr(IrExpr* baseExpr, IrExpr* indexExpr, const TSNode& node)
        : Ir(node), IrNonBinaryExpr(node), baseExpr(baseExpr), indexExpr(indexExpr){}

    ~IrSubscriptExpr() {
        delete baseExpr;
        delete indexExpr;
    }

    void setLevel(int l){
        level = l;
    }

    const std::string getName() const override {
        return baseExpr->getName();
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
        std::string* baseName = new std::string(baseExpr->getName());

        IrType* type = symbolTable.getFromTable(*baseName);
        IrTypeArray* arrayType = dynamic_cast<IrTypeArray*>(type);

        int width = arrayType->getBaseType()->getWidth();

        deque<IrLiteral*> dims = arrayType->getDimension();
        int arrSize = dims.size();

        // accordding to level, get the level-th dimension,
        // TODO: can noly handel 2 dims array now. need to be modified

        LlLocation *location = builder.generateTemp();
        if(level == 1){
            LlComponent* indexLocation = indexExpr->generateLlIr(builder, symbolTable);
            LlStatement* stmt = new LlAssignStmtBinaryOp(location, indexLocation, "*", new LlLiteralInt(width));
            builder.appendStatement(stmt);

        }
        else{
            IrLiteral* dim = dims[arrSize-level+1];
            LlLiteralInt* llDim = new LlLiteralInt(dynamic_cast<IrLiteralNumber*>(dim)->getValue()*width);
            LlComponent* indexLocation = indexExpr->generateLlIr(builder, symbolTable);
            LlStatement* stmt = new LlAssignStmtBinaryOp(location, indexLocation, "*", llDim);
            builder.appendStatement(stmt);
            return location;
        }

        LlLocation *location2 = dynamic_cast<LlLocation *>(baseExpr->generateLlIr(builder, symbolTable));

        LlLocation *location3 = builder.generateTemp();
        LlStatement* stmt2 = new LlAssignStmtBinaryOp(location3, location2, "+", location);
        builder.appendStatement(stmt2);

        return new LlLocationArray(baseName, location3);
    }
};

#endif
