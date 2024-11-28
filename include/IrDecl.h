#include "IrStatement.h"
#include "IrType.h"
#ifndef IR_DECL_H
#define IR_DECL_H

class IrMemberDecl : public Ir {
private:
    IrIdent* name;
    IrType* type;

public:
    IrMemberDecl(IrIdent* name, IrType* type, const TSNode & node) : Ir(node), name(name), type(type) {}

    const std::string* getName() const {
        return this->name->getValue();
    }

    IrIdent* getIdentName() const{
        return this->name;
    }

    IrType* getType() const{
        return this->type;
    }
};


class IrFieldDecl : public IrMemberDecl {
public:
    IrFieldDecl(IrIdent* name, IrType* fieldType, const TSNode& node) : IrMemberDecl(name, fieldType, node) {}

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

    std::string prettyPrint(std::string indentSpace) {
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

class IrMethodDecl : public IrMemberDecl {
private:
    std::vector<IrParamDecl*> paramsList;
    IrCodeBlock* methodBody;

public:
    IrMethodDecl(IrType* returnType, std::vector<IrParamDecl*> paramsList,
                 IrCodeBlock* methodBody, IrIdent* name, const TSNode& node) : IrMemberDecl(name, returnType, node), paramsList(paramsList), methodBody(methodBody) {}
 
    std::vector<IrParamDecl*> getParamsList() {
        return this->paramsList;
    }

    void addToParamsList(IrParamDecl* newParam) {
        this->paramsList.push_back(newParam);
    }

    IrCodeBlock* getMethodBody() {
        return this->methodBody;
    }



    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--method\n";

        // print the name
        prettyString += ("  " + indentSpace + "|--name: " + *(this->getName()) + "\n");

        // print the params
        prettyString += ("    " + indentSpace + "|--paramsList:\n");
        for (auto& paramDecl : this->paramsList) {
            prettyString += paramDecl->prettyPrint("      " + indentSpace);
        }

        // print the block
        prettyString += this->methodBody->prettyPrint("    " + indentSpace);

        return prettyString;
    }
};


#endif