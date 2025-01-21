#ifndef IR_DECL_H
#define IR_DECL_H
#include "IrStatement.h"
#include "IrType.h"
#include "IrExpr.h"

class IrDecl : public IrStatement {
private:
    bool mut;
    IrIdent* name;
    IrType* type;
    IrExpr* expr;
public:
    IrDecl(bool mut, IrIdent* name, IrType* type, IrExpr* expr, const TSNode & node) : mut(mut), IrStatement(node), name(name), type(type), expr(expr) {}
    ~IrDecl() {
        delete name;
        delete type;
        delete expr;
    }

    bool getMut() const {
        return this->mut;
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

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--letDecl:\n";

        // Print the mutability
        if (this->mut) {
            prettyString += ("  " + indentSpace + "|--mutable: true\n");
        } else {
            prettyString += ("  " + indentSpace + "|--mutable: false\n");
        }
        // prettyString += ("  " + indentSpace + "|--mutable: " + (this->mut ? "true" : "false") + "\n");

        // print the parameter's name
        prettyString += ("  " + indentSpace + "|--name: " + *(this->name->getValue()) + "\n");

        // print the parameter's type
        if (this->type) {
            prettyString += this->type->prettyPrint("  " + indentSpace);
        } else {
            prettyString += ("  " + indentSpace + "|--type: inferred\n");
        }

        // print the expression
        if (this->expr) {
            prettyString += this->expr->prettyPrint("  " + indentSpace);
        } else {
            prettyString += ("  " + indentSpace + "|--expr: none\n");
        }
        return prettyString;
    }
};

class IrMutableSpec : public Ir {
private:
public:
    IrMutableSpec(const TSNode& node) : Ir(node) {}
    ~IrMutableSpec() = default;

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--mutable: true\n";
        return prettyString;
    }
};

class IrParamDecl : public Ir {
private:
    IrType* paramType;
    IrIdent* paramName;

public:
    IrParamDecl(IrType* paramType, IrIdent* paramName, const TSNode& node) : Ir(node), paramType(paramType), paramName(paramName) {}
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

    // bool getIsMutable() const {
    //     return this->is_mutable;
    // }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--param\n";
        // prettyString += "  " + indentSpace + "|--mutable: " + (this->is_mutable ? "true" : "false") + "\n";

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
        std::string prettyString = indentSpace + "|--paramList\n";

        // pretty print statement
        for (IrParamDecl* paramDecl: this->paramsList) {
            prettyString += paramDecl->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }

};

class IrFunctionDef : public Ir {
private:
    IrIdent* ident;
    IrParamList* paramList;
    IrType* returnType;
    IrCompoundStmt* compoundStmt;
public:
    IrFunctionDef(IrIdent* ident, IrParamList* paramList, IrType* returnType, IrCompoundStmt* compoundStmt, const TSNode& node) : ident(ident), paramList(paramList), returnType(returnType), compoundStmt(compoundStmt), Ir(node) {}
    ~IrFunctionDef() {
        delete ident;
        delete paramList;
        delete returnType;
        delete compoundStmt;
    }
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--functionItem\n";
        prettyString += ident->prettyPrint("  " + indentSpace);
        prettyString += paramList->prettyPrint("  " + indentSpace);
        prettyString += "  " + indentSpace + "|--returnType\n";
        prettyString += returnType->prettyPrint("    " + indentSpace);
        prettyString += compoundStmt->prettyPrint("  " + indentSpace);
        return prettyString;
    }

    const std::string getFunctionName() const {
        if (ident) return ident->getName();
        return "";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        std::string name = ident->getName();
        LlEmptyStmt* emptyStmt = new LlEmptyStmt();
        builder.appendStatement(name, emptyStmt);
        symbolTable.putOnTable(name, returnType);
        this->compoundStmt->generateLlIr(builder, symbolTable);
        return nullptr;
    }
};
#endif