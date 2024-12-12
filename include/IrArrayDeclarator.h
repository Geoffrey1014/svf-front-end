#ifndef IR_ARRAY_DECLARATOR_H
#define IR_ARRAY_DECLARATOR_H

    // _declarator: $ => choice(
    //   $.attributed_declarator,
    //   $.pointer_declarator,
    //   $.function_declarator,
    //   $.array_declarator,
    //   $.parenthesized_declarator,
    //   $.identifier,
    // ),
    // array_declarator: $ => prec(1, seq(
    //   field('declarator', $._declarator),
    //   '[',
    //   repeat(choice($.type_qualifier, 'static')),
    //   field('size', optional(choice($.expression, '*'))),
    //   ']',
    // )),    
class IrArrayDeclarator : public IrExpr {
private:
    IrExpr* baseDeclarator; // Nested declarator or identifier
    IrExpr* sizeExpr;       // Size expression for the current dimension

public:
    IrArrayDeclarator(IrExpr* baseDeclarator, IrExpr* sizeExpr, const TSNode& node)
        : IrExpr(node), baseDeclarator(baseDeclarator), sizeExpr(sizeExpr) {}

    ~IrArrayDeclarator() {
        delete baseDeclarator;
        delete sizeExpr;
    }

    // Getter for the base declarator
    IrExpr* getBaseDeclarator() const {
        return baseDeclarator;
    }

    // Getter for the size expression
    IrExpr* getSizeExpr() const {
        return sizeExpr;
    }

    // Pretty print method to adhere to the CST hierarchy
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--array_declarator\n";

        if (baseDeclarator) {
            prettyString += baseDeclarator->prettyPrint("    " + indentSpace);
        } else{
            std::cout << "Error: Invalid base declarator" << std::endl;
        }

        if (sizeExpr)
        {
            prettyString += sizeExpr->prettyPrint("    " + indentSpace);
        }
        
        return prettyString;
    }
};

#endif
