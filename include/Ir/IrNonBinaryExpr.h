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

        prettyString += addIndent(indentSpace) + "|--functionName\n";
        prettyString += this->functionName->prettyPrint(addIndent(indentSpace, 2));
        prettyString += this->argList->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }

    std::string toString() const{
        return functionName->toString() + " (" + argList->toString() + ")";
    }

    LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override {
        std::vector<LlComponent*> argsList;
        for(auto& arg : this->argList->getArgsList()) {
            argsList.push_back(arg->generateLlIr(builder, symbolTable));
        }
        LlLocationVar* returnLocation = builder.generateTemp();

        LlMethodCallStmt* methodCallStmt = new LlMethodCallStmt(functionName->getName(), argsList, returnLocation);
        builder.appendStatement(methodCallStmt);
        return returnLocation;
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

        prettyString += addIndent(indentSpace) + "|--lhs\n";
        prettyString += this->lhs->prettyPrint(addIndent(indentSpace, 2));

        prettyString += addIndent(indentSpace) + "|--rhs\n";
        prettyString += this->rhs->prettyPrint(addIndent(indentSpace, 2));

        return prettyString;
    }    

    std::string toString() const {
        return lhs->toString() + " = " + rhs->toString();
    }

    LlLocation* generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable) override {
        LlLocation* left = lhs->generateLlIr(builder, symbolTable);
        LlComponent* right = rhs->generateLlIr(builder, symbolTable);
        LlAssignStmtRegular* assignStmt = new LlAssignStmtRegular(left, right);
        builder.appendStatement(assignStmt);
        return left;
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

#endif