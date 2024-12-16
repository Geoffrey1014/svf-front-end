#ifndef IR_DECL_H
#define IR_DECL_H
#include "IrStatement.h"
#include "IrType.h"
#include "IrExpr.h"
#include "IrStorageClassSpecifier.h"
#include "IrDeclarator.h"

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
class IrFunctionDecl : public IrDeclarator {
private:
    IrParamList* paramsList;
    IrIdent* name;
public:
    IrFunctionDecl(IrIdent* name, IrParamList* paramsList, const TSNode& node) 
        : Ir(node), IrDeclarator(node), name(name), paramsList(paramsList) {}
        
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
    IrType* type;                     
    IrStorageClassSpecifier* specifier; 
    IrDeclarator* declarator;          

public:
    // Constructor for declarations with declarator and type
    IrDecl(IrDeclarator* declarator, IrType* type, IrStorageClassSpecifier* specifier, const TSNode& node)
        : IrStatement(node), declarator(declarator), type(type), specifier(specifier) {}
    
    ~IrDecl() {
        delete declarator;
        delete type;
        delete specifier;
    }

    IrDeclarator* getDeclarator() const { return declarator; }
    IrType* getType() const { return type; }
    IrStorageClassSpecifier* getSpecifier() const { return specifier; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--declaration:\n";
        
        if(specifier){
            prettyString += specifier->prettyPrint("  " + indentSpace);
        }
            
        prettyString += type->prettyPrint("  " + indentSpace);
        
        prettyString += declarator->prettyPrint("  " + indentSpace);
        
        return prettyString;
    }
};
    
class IrInitDecl : public IrDecl {
private:
    IrExpr* initValue; 

public:
    IrInitDecl(IrIdent* name, IrType* type, IrExpr* initValue, IrStorageClassSpecifier* specifier, const TSNode& node)
        : IrDecl(name, type, specifier, node), initValue(initValue) {}

    ~IrInitDecl() {
        delete initValue;
    }

    IrExpr* getInitValue() const { return initValue; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = IrDecl::prettyPrint(indentSpace);
        prettyString += "  " + indentSpace + "|--init_declarator:\n";
        if (initValue) {
            prettyString += initValue->prettyPrint("    " + indentSpace);
        } 
        return prettyString;
    }
};
#endif