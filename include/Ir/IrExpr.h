#ifndef IR_EXPR_H
#define IR_EXPR_H
#include "Ir.h"

class IrExpr : public virtual Ir {
public:
    IrExpr(const TSNode & node) : Ir(node) {}
    ~IrExpr() = default;
    std::string toString() const{
        return "IrExpr";
    }

    bool operator==(const IrExpr& other) const {
        // Implementation of operator==
        return true;
    }

    int hashCode() const {
        // Implementation of hashCode
        return 0;
    }
};

#endif