#include "Ir.h"
#ifndef IR_STATEMENT_H
#define IR_STATEMENT_H

class IrStatement : public Ir {
public:
    IrStatement(const TSNode& node) : Ir(node) {}
};
#endif