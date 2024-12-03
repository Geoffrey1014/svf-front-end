#include "IrStatement.h"
#include "IrType.h"
#include "IrExpr.h"
#ifndef IR_DECL_H
#define IR_DECL_H

class IrDecl : public Ir {
private:
    IrIdent* name;
    IrType* type;

public:
    IrDecl(IrIdent* name, IrType* type, const TSNode & node) : Ir(node), name(name), type(type) {}

    const std::string* getName() const {
        return this->name->getValue();
    }

    IrIdent* getIdentName() const{
        return this->name;
    }

    IrType* getType() const{
        return this->type;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--declaration:\n";

        // print the parameter's name
        prettyString += ("  " + indentSpace + "|--name: " + *(this->name->getValue()) + "\n");

        // print the parameter's type
        prettyString += this->type->prettyPrint("  " + indentSpace);

        return prettyString;
    }
};


class IrFieldDecl : public IrDecl {
public:
    IrFieldDecl(IrIdent* name, IrType* fieldType, const TSNode& node) : IrDecl(name, fieldType, node) {}

};


class IrCodeBlock : public Ir {
private:
    std::vector<IrFieldDecl*> fieldsList;
    std::vector<IrStatement*> stmtsList;

public:
    IrCodeBlock(std::vector<IrFieldDecl*> fieldsList, std::vector<IrStatement*> stmtsList, const TSNode& node) 
        : Ir(node), fieldsList(fieldsList), stmtsList(stmtsList) {}

    std::vector<IrFieldDecl*> getFieldsList() {
        return this->fieldsList;
    }

    std::vector<IrStatement*> getStmtsList() {
        return this->stmtsList;
    }

    std::string prettyPrint(std::string indentSpace) const override  {
        std::string prettyString = indentSpace + "|--codeBlock:\n";

        // pretty print statement
        for (IrFieldDecl* fieldDecl: this->fieldsList) {
            prettyString += fieldDecl->prettyPrint("  " + indentSpace);
        }

        // pretty print field decl
        for (IrStatement* statement: this->stmtsList) {
            prettyString += statement->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }
};

class IrParamDecl : public Ir {
private:
    IrType* paramType;
    IrIdent* paramName;

public:
    IrParamDecl(IrType* paramType, IrIdent* paramName, const TSNode& node) : Ir(node), paramType(paramType), paramName(paramName) {}

    std::string toString() {
        return paramType->toString() + " " + paramName->toString();
    }

    IrType* getParamType() {
        return this->paramType;
    }

    IrIdent* getParamName() {
        return this->paramName;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--param:\n";

        // print the parameter's name
        prettyString += ("  " + indentSpace + "|--name: " + *(this->paramName->getValue()) + "\n");

        // print the parameter's type
        prettyString += this->paramType->prettyPrint("  " + indentSpace);

        return prettyString;
    }
};

class IrParamList : public Ir {
private:
    std::vector<IrParamDecl*> paramsList;

public:
    IrParamList(const TSNode& node) : Ir(node) {}

    std::vector<IrParamDecl*> getParamsList() {
        return this->paramsList;
    }

    void addToParamsList(IrParamDecl* newParam) {
        this->paramsList.push_back(newParam);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--paramList:\n";

        // pretty print statement
        for (IrParamDecl* paramDecl: this->paramsList) {
            prettyString += paramDecl->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }

};

class IrFunctionDecl : public Ir {
private:
    IrParamList* paramsList;
    IrIdent* name;
public:
    IrFunctionDecl(IrIdent* name, IrParamList* paramsList,
                  const TSNode& node) : name(name), paramsList(paramsList), Ir(node){}


    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--function_declarator\n";
        prettyString += name->prettyPrint("  " + indentSpace);
        prettyString += paramsList->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};


#endif