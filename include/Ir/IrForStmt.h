#ifndef IR_FOR_STMT_H
#define IR_FOR_STMT_H

#include "IrStatement.h"
#include "IrExpr.h"
#include "IrDecl.h"
#include "IrNonBinaryExpr.h"

    // for_statement: $ => seq(
    //   'for',
    //   '(', $._for_statement_body, ')',
    //   field('body', $.statement),
    // ),
    // _for_statement_body: $ => seq(
    //   choice(
    //     field('initializer', $.declaration),
    //     seq(field('initializer', optional(choice($.expression, $.comma_expression))), ';'),
    //   ),
    //   field('condition', optional(choice($.expression, $.comma_expression))),
    //   ';',
    //   field('update', optional(choice($.expression, $.comma_expression))),
    // ),
    // for ( i = 0; i < a; i += 1 ) {
    // a = a - 1;
    // }
class IrForStmt : public IrStatement {
private:
    IrAssignExpr* initializer;
    IrExpr* condition;
    IrExpr* update;
    IrStatement* body;

public:
    IrForStmt(IrAssignExpr* initializer, IrExpr* condition, IrExpr* update, IrStatement* body, const TSNode& node)
        : IrStatement(node), initializer(initializer), condition(condition), update(update), body(body) {}
    ~IrForStmt() {
        delete initializer;
        delete condition;
        delete update;
        delete body;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--forStmt\n";

        if (initializer) {
            prettyString += addIndent(indentSpace) + "|--initializer\n";
            prettyString += initializer->prettyPrint(addIndent(indentSpace, 2));
        }
        if (condition) {
            prettyString += addIndent(indentSpace) + "|--condition\n";
            prettyString += condition->prettyPrint(addIndent(indentSpace, 2));
        }
        if (update) {
            prettyString += addIndent(indentSpace) + "|--update\n";
            prettyString += update->prettyPrint(addIndent(indentSpace, 2));
        }
        prettyString += addIndent(indentSpace) + "|--body\n";
        prettyString += body->prettyPrint(addIndent(indentSpace, 2));

        return prettyString;
    }

    std::string toString() const override {
        std::string result = "for (";

        result += initializer->toString() + "; ";
        result += condition->toString()+ "; ";
        result += update->toString()+ ") ";

        if (body) {
            result += "\t\n" + body->toString();
        } else {
            result += "{}";
        }
        return result;
    }

    LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override {
        if (initializer) {
            initializer->generateLlIr(builder, symbolTable);
        }


        std::string* forLabel = new std::string();
        forLabel->append("FOR_");
        forLabel->append(builder.generateLabel());

        std::string* bodyLabel = new std::string();
        bodyLabel->append("FOR_BODY_");
        bodyLabel->append(*forLabel);
        
        std::string* endLabel = new std::string();
        endLabel->append("FOR_END_");
        endLabel->append(*forLabel);

        LlEmptyStmt* emptyStmtFor = new LlEmptyStmt();
        builder.appendStatement(*forLabel, emptyStmtFor);

        LlLocation* conditionVar = this->condition->generateLlIr(builder, symbolTable);
        LlJumpConditional* conditionalJump = new LlJumpConditional(bodyLabel,conditionVar);
        builder.appendStatement(conditionalJump);
        LlJumpUnconditional* jumpToForEnd = new LlJumpUnconditional(endLabel);
        builder.appendStatement(jumpToForEnd);

        LlEmptyStmt* emptyStmtForBody = new LlEmptyStmt();
        builder.appendStatement(*bodyLabel, emptyStmtForBody);
        if (body) {
            body->generateLlIr(builder, symbolTable);
        }

        update->generateLlIr(builder, symbolTable);

        LlJumpUnconditional* jumpToFor = new LlJumpUnconditional(forLabel);
        builder.appendStatement(jumpToFor);

        LlEmptyStmt* emptyStmtForEnd = new LlEmptyStmt();
        builder.appendStatement(*endLabel, emptyStmtForEnd);

        return nullptr;
    }
};

#endif