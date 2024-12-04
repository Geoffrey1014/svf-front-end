#ifndef IR_NON_BINARY_EXPR_H
#define IR_NON_BINARY_EXPR_H
#include "IrExpr.h"
#include "IrArg.h"

class IrNonBinaryExpr : public IrExpr {
public:
    IrNonBinaryExpr(const TSNode & node) : IrExpr(node) {}
};

class IrCallExpr : public IrNonBinaryExpr {
private:
    IrIdent* functionName;
    IrArgList* argList;

public:
    IrCallExpr(IrIdent* functionName, IrArgList* argList, const TSNode & node) : IrNonBinaryExpr(node), functionName(functionName), argList(argList) {}
    ~IrCallExpr() {
        delete functionName;
        delete argList;
    }
    IrIdent* getFunctionName() {
        return this->functionName;
    }

    IrArgList* getArgList() {
        return this->argList;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--callExpr\n";

        // print the function name
        prettyString += "  " + indentSpace + "|--functionName\n";
        prettyString += this->functionName->prettyPrint("    " + indentSpace);

        // print the argument list
        prettyString += this->argList->prettyPrint("  " + indentSpace);

        return prettyString;
    }
};

class IrAssignExpr : public IrNonBinaryExpr {
private:
    IrExpr* lhs;
    IrExpr* rhs;
public:
    IrAssignExpr(IrExpr* lhs, IrExpr* rhs, const TSNode & node) : IrNonBinaryExpr(node), lhs(lhs), rhs(rhs) {}
    ~IrAssignExpr() {
        delete lhs;
        delete rhs;
    }
    IrExpr* getLhs() {
        return this->lhs;
    }

    IrExpr* getRhs() {
        return this->rhs;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--assignExpr\n";

        // pretty print the lhs
        prettyString += "  " + indentSpace + "|--lhs\n";
        prettyString += this->lhs->prettyPrint("    " + indentSpace);

        // pretty print the rhs
        prettyString += "  " + indentSpace + "|--rhs\n";
        prettyString += this->rhs->prettyPrint("    " + indentSpace);

        return prettyString;
    }    

};

#endif