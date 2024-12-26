#ifndef IR_FOR_STMT_H
#define IR_FOR_STMT_H

#include "IrStatement.h"
#include "IrExpr.h"
#include "IrDecl.h"

    // for_statement: $ => seq(
    //   'for',
    //   '(',
    //   $._for_statement_body,
    //   ')',
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
class IrForStmt : public IrStatement {

};

#endif