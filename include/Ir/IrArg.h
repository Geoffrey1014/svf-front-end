#ifndef IR_ARG_H
#define IR_ARG_H

#include "Ir.h"
#include "IrExpr.h"

class IrArgList : public Ir {
private:
    std::deque<IrExpr*> argsList;
public:
    IrArgList(const TSNode& node) : Ir(node) {}
    ~IrArgList() {
        for (IrExpr* arg: this->argsList) {
            delete arg;
        }
    }   

    std::deque<IrExpr*> getArgsList() {
        return this->argsList;
    }

    void addToArgsList(IrExpr* newArg) {
        this->argsList.push_front(newArg);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--argList:\n";

        for (IrExpr* arg: this->argsList) {
            prettyString += arg->prettyPrint(addIndent(indentSpace)); 
        }

        return prettyString;
    }

    std::string toString() const{
        std::string argsString = "";
        for (IrExpr* arg: this->argsList) {
            argsString += arg->toString() + ", ";
        }
        return argsString;
    }
};

#endif