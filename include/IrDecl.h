#include "Ir.h"

class IrMemberDecl : public Ir {
private:
    IrIdent* name;
    IrType* type;

public:
    IrMemberDecl(IrIdent* name, IrType* type, int lineNumber, int colNumber) : Ir(lineNumber, colNumber), name(name), type(type) {}

    std::string getName() {
        return this->name->getValue();
    }

    IrIdent* getIdentName() {
        return this->name;
    }

    IrType* getType() {
        return this->type;
    }
};

class IrExternDecl : public IrMemberDecl {
public:
    IrExternDecl(IrIdent* name) : IrMemberDecl(name, new IrTypeInt(name->getLineNumber(), name->getColNumber()), name->getColNumber(), name->getColNumber()) {}

    std::string semanticCheck(ScopeStack* scopeStack) override {
        return "";
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--" + this->getName() + "\n";
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        return nullptr;
    }
};

class IrFieldDecl : public IrMemberDecl {
public:
    IrFieldDecl(IrIdent* name, IrType* fieldType) : IrMemberDecl(name, fieldType, name->getLineNumber(), name->getColNumber()) {}

};

class IrMethodDecl : public IrMemberDecl {
private:
    std::vector<IrParamDecl*> paramsList;
    IrCodeBlock* methodBody;

public:
    IrMethodDecl(IrType* returnType, std::vector<IrParamDecl*> paramsList,
                 IrCodeBlock* methodBody, IrIdent* name) : IrMemberDecl(name, returnType, name->getLineNumber(), name->getColNumber()), paramsList(paramsList), methodBody(methodBody) {}
 
    std::vector<IrParamDecl*> getParamsList() {
        return this->paramsList;
    }

    void addToParamsList(IrParamDecl* newParam) {
        this->paramsList.push_back(newParam);
    }

    IrCodeBlock* getMethodBody() {
        return this->methodBody;
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        std::string errorMessage = "";

        // give method a new method scope with method returnType
        scopeStack->createNewMethodScope(this->getType());

        // check the params before the code block
        for (auto& paramDecl : this->paramsList) {

            // 1) make sure no duplicate params are declared
            if (scopeStack->checkIfSymbolExistsAtCurrentScope(paramDecl->getParamName()->getValue())) {
                errorMessage += "Duplicate declaration of parameters in method declaration" +
                        " line: " + std::to_string(this->getLineNumber()) + " col: " + std::to_string(this->getColNumber()) + "\n";
            }
            scopeStack->addObjectToCurrentScope(paramDecl->getParamName()->getValue(), paramDecl);

            // 2) check that all the IrParamDecls are valid
            errorMessage += paramDecl->semanticCheck(scopeStack);
        }

        // 3) check that the codeBlock is valid
        errorMessage += this->methodBody->semanticCheck(scopeStack);

        // delete method scope the scope
        scopeStack->deleteCurrentScope();

        return errorMessage;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        std::string methodNameLabel = this->getName();
        LlEmptyStmt* emptyStmt = new LlEmptyStmt();
        builder->appendStatement(methodNameLabel, emptyStmt);
        this->methodBody->generateLlIr(builder, symbolTable);
        return nullptr;
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--method\n";

        // print the name
        prettyString += ("  " + indentSpace + "|--name: " + this->getName() + "\n");

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

class IrParamDecl : public Ir {
private:
    IrType* paramType;
    IrIdent* paramName;

public:
    IrParamDecl(IrType* paramType, IrIdent* paramName) : Ir(paramName->getLineNumber(), paramName->getColNumber()), paramType(paramType), paramName(paramName) {}

    std::string toString() {
        return paramType->toString() + " " + paramName->toString();
    }

    IrType* getParamType() {
        return this->paramType;
    }

    IrIdent* getParamName() {
        return this->paramName;
    }

    std::string semanticCheck(ScopeStack* scopeStack) override {
        return "";
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--param:\n";

        // print the parameter's name
        prettyString += ("  " + indentSpace + "|--name: " + this->paramName->getValue() + "\n");

        // print the parameter's type
        prettyString += this->paramType->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        return nullptr;
    }
};

