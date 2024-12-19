#ifndef IR_DECL_H
#define IR_DECL_H
#include "IrStatement.h"
#include "IrType.h"
#include "IrExpr.h"
#include "IrStorageClassSpecifier.h"
#include "IrDeclarator.h"

class IrParamDecl : public Ir {
private:
    IrType* paramType;                 // The type of the parameter
    IrDeclDeclarator* declarator;      // Can represent a name or abstract declarator

public:
    IrParamDecl(IrType* paramType, IrDeclDeclarator* declarator, const TSNode& node)
        : Ir(node), paramType(paramType), declarator(declarator) {}

    ~IrParamDecl() {
        delete paramType;
        delete declarator;
    }

    // Getter for type
    IrType* getParamType() const {
        return this->paramType;
    }

    // Getter for declarator
    IrDeclDeclarator* getDeclarator() const {
        return this->declarator;
    }

    // Convert to string
    std::string toString() const {
        if (declarator) {
            return paramType->toString() + " " + declarator->toString();
        }
        return paramType->toString();
    }

    // Pretty print the parameter
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--param:\n";

        // Print the type
        prettyString += this->paramType->prettyPrint("  " + indentSpace);

        // Print the declarator if it exists
        if (declarator) {
            prettyString += declarator->prettyPrint("  " + indentSpace);
        }

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
class IrFunctionDecl : public IrDeclDeclarator {
private:
    IrDeclDeclarator* declarator;  // The base declarator (e.g., function name, could include pointers)
    IrParamList* paramsList;   // List of function parameters

public:
    // Constructor
    IrFunctionDecl(IrDeclDeclarator* declarator, IrParamList* paramsList, const TSNode& node)
        : Ir(node), IrDeclDeclarator(node), declarator(declarator), paramsList(paramsList) {}

    // Destructor
    ~IrFunctionDecl() override {
        delete declarator;
        delete paramsList;
    }

    // Getter for the declarator (function name or base)
    IrDeclDeclarator* getDeclarator() const { return declarator; }

    // Getter for the parameter list
    IrParamList* getParamsList() const { return paramsList; }

    // Pretty print the function declarator
    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--function_declarator\n";
        prettyString += declarator->prettyPrint(indentSpace + "  ");
        prettyString += paramsList->prettyPrint(indentSpace + "  ");
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

class IrInitDeclarator : public Ir {
private:
    IrDeclDeclarator* declarator;  // Variable name or declarator
    IrExpr* initializer;       // Expression (e.g., value 2)

public:
    IrInitDeclarator(IrDeclDeclarator* declarator, IrExpr* initializer, const TSNode& node)
        : Ir(node), declarator(declarator), initializer(initializer) {}

    ~IrInitDeclarator() override {
        delete declarator;
        delete initializer;
    }

    IrDeclDeclarator* getDeclarator() const { return declarator; }
    IrExpr* getInitializer() const { return initializer; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--init_declarator:\n";
        prettyString += declarator->prettyPrint(indentSpace + "  ");
        prettyString += initializer->prettyPrint(indentSpace + "  ");
        return prettyString;
    }
};

// declaration
class IrDecl : public IrStatement {
private:
    IrType* type;                        
    IrStorageClassSpecifier* specifier;   
    std::vector<IrInitDeclarator*> initDeclarators;      
    IrDeclDeclarator* simpleDeclarator;      

public:
    // Constructor for multiple initialized declarators
    IrDecl(IrType* type, IrStorageClassSpecifier* specifier, const std::vector<IrInitDeclarator*>& initDecls, const TSNode& node)
        : IrStatement(node), type(type), specifier(specifier), initDeclarators(initDecls), simpleDeclarator(nullptr) {}

    // Constructor for a simple single declarator
    IrDecl(IrType* type, IrStorageClassSpecifier* specifier, IrDeclDeclarator* declarator, const TSNode& node)
        : IrStatement(node), type(type), specifier(specifier), simpleDeclarator(declarator) {}

    ~IrDecl() override {
        delete type;
        delete specifier;
        for (auto* initDecl : initDeclarators) {
            delete initDecl;
        }
        delete simpleDeclarator;
    }

    // Getter methods
    IrType* getType() const { return type; }
    IrStorageClassSpecifier* getSpecifier() const { return specifier; }
    const std::vector<IrInitDeclarator*>& getInitDeclarators() const { return initDeclarators; }
    IrDeclDeclarator* getSimpleDeclarator() const { return simpleDeclarator; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--declaration:\n";

        if (specifier) {
            prettyString += specifier->prettyPrint(indentSpace + "  ");
        }

        prettyString += type->prettyPrint(indentSpace + "  ");

        if (!initDeclarators.empty()) {
            for (auto* initDecl : initDeclarators) {
                prettyString += initDecl->prettyPrint(indentSpace + "  ");
            }
        } else if (simpleDeclarator) {
            // If no initDecls, print the simpleDeclarator
            prettyString += simpleDeclarator->prettyPrint(indentSpace + "  ");
        }

        return prettyString;
    }
};

#endif