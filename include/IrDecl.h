#ifndef IR_DECL_H
#define IR_DECL_H
#include "IrStatement.h"
#include "IrType.h"
#include "IrExpr.h"
#include "IrStorageClassSpecifier.h"
#include "IrArrayDeclarator.h"

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
    std::string toString() const {
        // Show only the type if the name is null
        return paramName ? paramType->toString() + " " + paramName->toString() : paramType->toString();
    }

    IrType* getParamType() {
        return this->paramType;
    }

    IrIdent* getParamName() {
        return this->paramName;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--param:\n";

        // Include the name only if it exists
        if (paramName) {
            prettyString += "  " + indentSpace + "|--name: " + paramName->getValue() + "\n";
        }

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

// function_declarator
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

    // Method to get the name of the function
    const std::string& getValue() const {
        return name->getValue();
    }

    // Accessor for the parameter list
    IrParamList* getParamsList() const {
        return paramsList;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--function_declarator\n";
        prettyString += name->prettyPrint("  " + indentSpace);
        prettyString += paramsList->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};

// function_definition
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

class IrDecl : public IrStatement {
private:
    IrIdent* name;                      // Optional
    IrType* type;                       // Mandatory
    IrStorageClassSpecifier* specifier; // Optional
    IrFunctionDecl* functionDecl;       // Optional
    IrArrayDeclarator* arrayDeclarator; // Optional

public:
    // Constructor for variable declarations
    IrDecl(IrIdent* name, IrType* type, IrStorageClassSpecifier* specifier, const TSNode& node)
        : IrStatement(node), name(name), type(type), specifier(specifier), functionDecl(nullptr), arrayDeclarator(nullptr) {}

    // Constructor for function declarations
    IrDecl(IrFunctionDecl* functionDecl, IrType* type, IrStorageClassSpecifier* specifier, const TSNode& node)
        : IrStatement(node), name(nullptr), type(type), specifier(specifier), functionDecl(functionDecl), arrayDeclarator(nullptr) {}

    // Constructor for array declarators
    IrDecl(IrArrayDeclarator* arrayDeclarator, IrType* type, IrStorageClassSpecifier* specifier, const TSNode& node)
        : IrStatement(node), name(nullptr), type(type), specifier(specifier), functionDecl(nullptr), arrayDeclarator(arrayDeclarator) {}

    ~IrDecl() {
        delete name;
        delete type;
        delete specifier;
        delete functionDecl;
        delete arrayDeclarator;
    }

    IrIdent* getIdentName() const { return name; }
    IrType* getType() const { return type; }
    IrStorageClassSpecifier* getSpecifier() const { return specifier; }
    IrFunctionDecl* getFunctionDecl() const { return functionDecl; }
    IrArrayDeclarator* getArrayDeclarator() const { return arrayDeclarator; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--declaration:\n";

        if (specifier) {
            prettyString += specifier->prettyPrint("  " + indentSpace);
        }

        if (name) {
            prettyString += "  " + indentSpace + "|--name: " + name->getValue() + "\n";
        }

        if (type) {
            prettyString += type->prettyPrint("  " + indentSpace);
        } else {
            std::cout << "Warning: Missing type specifier in declaration" << std::endl;
        }

        if (functionDecl) {
            prettyString += functionDecl->prettyPrint("  " + indentSpace);
        }

        if (arrayDeclarator) {
            prettyString += arrayDeclarator->prettyPrint("  " + indentSpace);
        }

        return prettyString;
    }
};

#endif