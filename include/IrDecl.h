#ifndef IR_DECL_H
#define IR_DECL_H
#include "IrStatement.h"
#include "IrType.h"
#include "IrExpr.h"

class IrDecl : public Ir {
private:
    IrIdent* name;
    IrType* type;
    bool is_mutable;

public:
    IrDecl(IrIdent* name, IrType* type, bool is_mutable, const TSNode & node) : Ir(node), name(name), type(type) {}
    ~IrDecl() {
        delete name;
        delete type;
    }
    const std::string* getName() const {
        return this->name->getValue();
    }

    IrIdent* getIdentName() const{
        return this->name;
    }

    IrType* getType() const{
        return this->type;
    }

    bool getIsMutable() const {
        return this->is_mutable;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--declaration:\n";

        // Print the mutability
        prettyString += ("  " + indentSpace + "|--mutable: " + (this->is_mutable ? "true" : "false") + "\n");

        // print the parameter's name
        prettyString += ("  " + indentSpace + "|--name: " + *(this->name->getValue()) + "\n");

        // print the parameter's type
        if (this->type) {
            prettyString += this->type->prettyPrint("  " + indentSpace);
        } else {
            prettyString += ("  " + indentSpace + "|--type: inferred\n");
        }

        return prettyString;
    }
};

class IrParamDecl : public Ir {
private:
    IrType* paramType;
    IrIdent* paramName;
    bool is_mutable;

public:
    IrParamDecl(IrType* paramType, IrIdent* paramName, bool is_mutable, const TSNode& node) : Ir(node), paramType(paramType), paramName(paramName) {}
    ~IrParamDecl() {
        delete paramType;
        delete paramName;
    }
    std::string toString() {
        return paramType->toString() + " " + paramName->toString();
    }

    IrType* getParamType() {
        return this->paramType;
    }

    IrIdent* getParamName() {
        return this->paramName;
    }

    bool getIsMutable() const {
        return this->is_mutable;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--param:\n";
        prettyString += "  " + indentSpace + "|--mutable: " + (this->is_mutable ? "true" : "false") + "\n";

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
    ~IrParamList() {
        for (IrParamDecl* param: this->paramsList) {
            delete param;
        }
    }

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
    ~IrFunctionDecl(){
        delete name;
        delete paramsList;
    }


    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--function_declarator\n";
        prettyString += name->prettyPrint("  " + indentSpace);
        prettyString += paramsList->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};


class IrFunctionDef : public Ir {
private:
    IrType* returnType;
    IrFunctionDecl* functionDecl;
    IrCompoundStmt* compoundStmt;
public:
    IrFunctionDef(IrType* returnType ,IrFunctionDecl* functionDecl, IrCompoundStmt* compoundStmt, const TSNode& node) : returnType(returnType), functionDecl(functionDecl), compoundStmt(compoundStmt), Ir(node) {}
    ~IrFunctionDef() {
        delete returnType;
        delete functionDecl;
        delete compoundStmt;
    }
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--function_definition\n";
        prettyString += returnType->prettyPrint("  " + indentSpace);
        prettyString += functionDecl->prettyPrint("  " + indentSpace);
        prettyString += compoundStmt->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};
#endif