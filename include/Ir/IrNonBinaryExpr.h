#ifndef IR_NON_BINARY_EXPR_H
#define IR_NON_BINARY_EXPR_H
#include "IrExpr.h"
#include "IrDeclarator.h"
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
    IrCallExpr(IrIdent* functionName, IrArgList* argList, const TSNode & node) 
        : Ir(node), IrNonBinaryExpr(node), functionName(functionName), argList(argList) {}
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
        prettyString += addIndent(indentSpace) + "|--functionName\n";
        prettyString += this->functionName->prettyPrint(addIndent(indentSpace, 2));

        // print the argument list
        prettyString += this->argList->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }

    std::string toString() const{
        return functionName->toString() + " (" + argList->toString() + ")";
    }
};

class IrAssignExpr : public IrNonBinaryExpr {
private:
    IrExpr* lhs;
    IrExpr* rhs;
public:
    IrAssignExpr(IrExpr* lhs, IrExpr* rhs, const TSNode & node) 
        : Ir(node), IrNonBinaryExpr(node), lhs(lhs), rhs(rhs) {}
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
        prettyString += addIndent(indentSpace) + "|--lhs\n";
        prettyString += this->lhs->prettyPrint(addIndent(indentSpace, 2));

        // pretty print the rhs
        prettyString += addIndent(indentSpace) + "|--rhs\n";
        prettyString += this->rhs->prettyPrint(addIndent(indentSpace, 2));

        return prettyString;
    }    

    std::string toString() const {
        return lhs->toString() + " = " + rhs->toString();
    }

};

class IrFieldExpr : public IrNonBinaryExpr {
private:
    IrExpr* baseExpr;
    IrIdent* fieldName;
    bool isArrow; 

public:
    IrFieldExpr(IrExpr* base, IrIdent* field, bool isArrow, const TSNode & node) 
      : Ir(node), IrNonBinaryExpr(node), baseExpr(base), fieldName(field), isArrow(isArrow) {}

    ~IrFieldExpr() override {
        delete baseExpr;
        delete fieldName;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string op = isArrow ? "->" : ".";
        std::string prettyString = indentSpace + "|--field_expression\n";
        prettyString += baseExpr->prettyPrint(addIndent(indentSpace));
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += fieldName->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() {
        std::string op = isArrow ? "->" : ".";
        return baseExpr->toString() + op + fieldName->toString();
    }   

    IrExpr* getBaseExpr() const { return baseExpr; }
    IrIdent* getFieldName() const { return fieldName; }
    bool getIsArrow() const { return isArrow; }
};

class IrPointerExpr : public IrNonBinaryExpr {
private:
    IrExpr* argument;
    bool isAddressOf;   // true if operator is '&'
    bool isDereference; // true if operator is '*'

public:
    IrPointerExpr(IrExpr* arg, bool addressOf, bool dereference, const TSNode & node)
        : Ir(node), IrNonBinaryExpr(node), argument(arg), isAddressOf(addressOf), isDereference(dereference) {}

    ~IrPointerExpr() override {
        delete argument;
    }

    IrExpr* getArgument() const { return argument; }
    bool getIsAddressOf() const { return isAddressOf; }
    bool getIsDereference() const { return isDereference; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string op = isAddressOf ? "&" : "*";
        std::string prettyString = indentSpace + "|--pointer_expression\n";

        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        if (argument) {
            prettyString += argument->prettyPrint(addIndent(indentSpace));
        }
        
        return prettyString;
    }

    std::string toString() const{
        std::string op = isAddressOf ? "&" : "*";
        return op + argument->toString();
    }
};

class IrParenthesizedExpr : public IrNonBinaryExpr {
private:
    IrExpr* innerExpr;

public:
    IrParenthesizedExpr(IrExpr* expr, const TSNode & node) 
        : Ir(node), IrNonBinaryExpr(node), innerExpr(expr) {}

    ~IrParenthesizedExpr() {
        delete innerExpr;
    }

    IrExpr* getInnerExpr() const { return innerExpr; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--parenthesizedExpr\n";
        if (innerExpr) {
            prettyString += innerExpr->prettyPrint(addIndent(indentSpace));
        }
        return prettyString;
    }

    std::string toString() const override {
        return "(" + innerExpr->toString() + ")";
    }
};

class IrUnaryExpr : public IrNonBinaryExpr {
private:
    std::string op;  // Operator (e.g., '-', '!')
    IrExpr* argument;

public:
    IrUnaryExpr(const std::string& op, IrExpr* arg, const TSNode &node) 
        : Ir(node), IrNonBinaryExpr(node), op(op), argument(arg) {}

    ~IrUnaryExpr() {
        delete argument;
    }

    IrExpr* getArgument() const { return argument; }
    std::string getOperator() const { return op; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--unaryExpr\n";
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += argument->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override {
        return op + argument->toString();
    }
};

#endif