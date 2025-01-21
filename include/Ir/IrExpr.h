#ifndef IR_EXPR_H
#define IR_EXPR_H
#include "Ir.h"

class IrExpr : public virtual Ir {
public:
    IrExpr(const TSNode & node) : Ir(node) {}
    ~IrExpr() = default;
    
    std::string toString() const override {
        return "baseIrExpr";
    }

    bool operator==(const IrExpr& other) const {
        // Implementation of operator==
        return true;
    }

    int hashCode() const {
        // Implementation of hashCode
        return 0;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        std::cerr << "IrExpr Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("Error")); 
    }

    virtual const std::string getName() const {
        return "";
    }
};

#endif // IR_EXPR_H