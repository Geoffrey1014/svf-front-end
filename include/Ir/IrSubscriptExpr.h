#ifndef IR_SUBSCRIPT_EXPR_H
#define IR_SUBSCRIPT_EXPR_H

#include "IrExpr.h"
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
        std::string baseName = baseExpr->getName();
        IrType* type = symbolTable.getFromTable(baseName);
        IrTypeArray* arrayType = dynamic_cast<IrTypeArray*>(type);

        if (!arrayType) {
            std::cerr << "Error: " << baseName << " is not an array." << std::endl;
            return nullptr;
        }

        deque<IrLiteral*> dims = arrayType->getDimension();
        int elemWidth = arrayType->getBaseType()->getWidth();
        int arrSize = dims.size();

        IrExpr* currentExpr = this;
        int currentLevel = arrSize; 

        int cumulativeMultiplier = elemWidth;

        LlLocation* offsetTemp = nullptr;

        while (auto* sub = dynamic_cast<IrSubscriptExpr*>(currentExpr)) {
            if (currentLevel <= 0) {
                std::cerr << "Error: Too many subscripts for array " << baseName << std::endl;
                return nullptr;
            }

            // Get the size of the current dimension
            IrLiteral* dimLiteral = dims[currentLevel - 1];
            int dimSize = dynamic_cast<IrLiteralNumber*>(dimLiteral)->getValue();

            LlComponent* indexLocation = sub->getIndexExpr()->generateLlIr(builder, symbolTable);

            LlLocation* mulTemp = builder.generateTemp();
            LlLiteralInt* multiplierLiteral = new LlLiteralInt(cumulativeMultiplier);
            builder.appendStatement(new LlAssignStmtBinaryOp(mulTemp, indexLocation, "*", multiplierLiteral));

            if (!offsetTemp) {
                offsetTemp = mulTemp;
            } else {
                LlLocation* addTemp = builder.generateTemp();
                builder.appendStatement(new LlAssignStmtBinaryOp(addTemp, offsetTemp, "+", mulTemp));
                offsetTemp = addTemp;
            }
            cumulativeMultiplier *= dimSize;

            currentExpr = sub->getBaseExpr();
            currentLevel--;
        }
        return new LlLocationArray(new std::string(baseName), offsetTemp);
    }
};

#endif
