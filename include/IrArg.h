#ifndef IR_ARG_H
#define IR_ARG_H

#include "Ir.h"
#include "IrExpr.h"

class IrArgList : public Ir {
private:
    std::vector<IrExpr*> argsList;
public:
    IrArgList(const TSNode& node) : Ir(node) {}
    ~IrArgList() {
        for (IrExpr* arg: this->argsList) {
            delete arg;
        }
    }   

    std::vector<IrExpr*> getArgsList() {
        return this->argsList;
    }

    void addToArgsList(IrExpr* newArg) {
        this->argsList.push_back(newArg);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--argList\n";

        // pretty print statement
        for (IrExpr* arg: this->argsList) {
            prettyString += arg->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }
};



#endif