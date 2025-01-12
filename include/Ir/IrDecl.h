#ifndef IR_DECL_H
#define IR_DECL_H

#include "IrStatement.h"
#include "IrType.h"
#include "IrExpr.h"
#include "IrStorageClassSpecifier.h"
#include "IrDeclarator.h"
#include <deque>

class IrFieldDecl : public Ir {
private:
    IrType* type;                        // Type of the field
    IrDeclDeclarator* declarator;        // Field name or declarator
    //int bitfieldSize;                  // Optional bitfield size (-1 if none)

public:
    IrFieldDecl(IrType* type, IrDeclDeclarator* declarator, const TSNode& node)
        : Ir(node), type(type), declarator(declarator){} // bitfieldSize(bitfieldSize)

    ~IrFieldDecl() {
        delete type;
        delete declarator;
    }

    IrType* getType() const { return type; }
    IrDeclDeclarator* getDeclarator() const { return declarator; }

    // int getBitfieldSize() const { return bitfieldSize; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string str = indentSpace + "|--field_declaration:\n";
        str += type->prettyPrint(addIndent(indentSpace));

        if (declarator) {
            str += declarator->prettyPrint(addIndent(indentSpace));
        }
        // if (bitfieldSize != -1) {
        //     str += indentSpace + "  |--bitfield_size: " + std::to_string(bitfieldSize) + "\n";
        // }
        return str;
    }

    std::string toString() const override{
        return type->toString() + " " + declarator->toString();
    }
};

class IrFieldDeclList : public Ir {
private:
    std::deque<IrFieldDecl*> fieldDeclarations; // List of field declarations

public:
    IrFieldDeclList(const TSNode& node) : Ir(node) {}

    ~IrFieldDeclList() {
        for (auto* fieldDecl : fieldDeclarations) {
            delete fieldDecl;
        }
    }

    void addField(IrFieldDecl* fieldDecl) {
        fieldDeclarations.push_front(fieldDecl);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        if (fieldDeclarations.empty()) {
            return "";
        }
        
        std::string str = indentSpace + "|--field_declaration_list:\n";
        for (auto* fieldDecl : fieldDeclarations) {
            str += fieldDecl->prettyPrint(addIndent(indentSpace));
        }
        return str;
    }

    std::string toString() const override{
        std::string str = "";
        for (auto* fieldDecl : fieldDeclarations) {
            str += fieldDecl->toString() + ", ";
        }
        return str;
    }
};

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

    IrType* getParamType() const {
        return this->paramType;
    }

    IrDeclDeclarator* getDeclarator() const {
        return this->declarator;
    }

    std::string toString() const override{
        if (declarator) {
            return paramType->toString() + " " + declarator->toString();
        }
        return paramType->toString();
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--param:\n";

        prettyString += paramType->prettyPrint(addIndent(indentSpace));

        if (declarator) {
            prettyString += declarator->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }
};


class IrParamList : public Ir {
private:
    std::deque<IrParamDecl*> paramsList;

public:
    IrParamList(const TSNode& node) : Ir(node) {}
    ~IrParamList() {
        for (IrParamDecl* param: this->paramsList) {
            delete param;
        }
    }

    std::deque<IrParamDecl*> getParamsList() {
        return this->paramsList;
    }

    void addToParamsList(IrParamDecl* newParam) {
        this->paramsList.push_front(newParam);
    }

    std::string toString() const override{
        std::string paramsString = "";
        for (IrParamDecl* paramDecl: this->paramsList) {
            paramsString += paramDecl->toString() + ", ";
        }
        return paramsString;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--paramList:\n";

        for (IrParamDecl* paramDecl: this->paramsList) {
            prettyString += paramDecl->prettyPrint(addIndent(indentSpace));
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
    IrFunctionDecl(IrDeclDeclarator* declarator, IrParamList* paramsList, const TSNode& node)
        : Ir(node), IrDeclDeclarator(node), declarator(declarator), paramsList(paramsList) {}

    ~IrFunctionDecl() override {
        delete declarator;
        delete paramsList;
    }

    IrDeclDeclarator* getDeclarator() const { return declarator; }

    IrParamList* getParamsList() const { return paramsList; }

    const std::string getName() const override {
        if (declarator) return declarator->getName();
        return "";
    }

    std::string toString() const override{
        return getName() + " (" + paramsList->toString() + ")";
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--function_declarator\n";
        prettyString += declarator->prettyPrint(addIndent(indentSpace));
        prettyString += paramsList->prettyPrint(addIndent(indentSpace));
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

    IrType* getReturnType() const { return returnType; }
    IrFunctionDecl* getFunctionDecl() const { return functionDecl; }
    IrCompoundStmt* getCompoundStmt() const { return compoundStmt; }

    LlComponent* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        std::string name = functionDecl->getName();
        LlEmptyStmt* emptyStmt = new LlEmptyStmt();
        builder.appendStatement(name, emptyStmt);
        this->compoundStmt->generateLlIr(builder, symbolTable);
        return nullptr;
    }

    std::string toString() const override{
        std::string result = returnType->toString() + " ";
        result += functionDecl->toString();
        result += " {\n";
        if (compoundStmt) {
            result += compoundStmt->toString();  // Include the body
        }
        result += "\n}";
        return result;
    }
    std::string getFunctionName() { return functionDecl->toString();}

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--function_definition\n";
        prettyString += returnType->prettyPrint(addIndent(indentSpace));
        prettyString += functionDecl->prettyPrint(addIndent(indentSpace));
        prettyString += compoundStmt->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }
};

class IrInitializerList : public IrExpr {
private:
    std::deque<IrExpr*> elements;

public:
    IrInitializerList(const TSNode& node) : IrExpr(node), Ir(node) {}

    ~IrInitializerList() {
        for (IrExpr* expr : elements) {
            delete expr;
        }
    }

    void addElement(IrExpr* expr) {
        elements.push_front(expr);
    }

    std::deque<IrExpr*> getElements() const {
        return elements;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--initializer_list\n";
        for (IrExpr* expr : elements) {
            prettyPrint += expr->prettyPrint(addIndent(indentSpace));
        }
        return prettyPrint;
    }

    std::string toString() const override {
        std::string str = "{ ";
        for (auto expr : elements) {
            str += expr->toString() + ", ";
        }
        str += "}";
        return str;
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
        prettyString += declarator->prettyPrint(addIndent(indentSpace));
        prettyString += initializer->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        return declarator->toString() + " = " + initializer->toString();
    }

    LlComponent* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlComponent* compo = declarator->generateLlIr(builder, symbolTable);
        LlLocationVar* location = dynamic_cast<LlLocationVar*>(compo);
        LlComponent* init = initializer->generateLlIr(builder, symbolTable);
        LlAssignStmtRegular* assignStmt = new LlAssignStmtRegular(location, init);
        builder.appendStatement(assignStmt);
        return location;
    }
};

// declaration
class IrDecl : public IrStatement {
private:
    IrType* type;                        
    IrStorageClassSpecifier* specifier;

    // Store exactly ONE of these (whichever applies):
    IrInitDeclarator* initDecl;      
    IrDeclDeclarator* simpleDecl;      

public:
    // Constructor for an initialized declarator (e.g. int a=10)
    IrDecl(IrType* type, IrStorageClassSpecifier* specifier, IrInitDeclarator* initDecl,
           const TSNode& node)
        : IrStatement(node), type(type), specifier(specifier), initDecl(initDecl), simpleDecl(nullptr) {}

    // Constructor for a simple declarator (e.g. int a)
    IrDecl(IrType* type, IrStorageClassSpecifier* specifier, IrDeclDeclarator* simpleDecl,
           const TSNode& node)
        : IrStatement(node), type(type), specifier(specifier),
          initDecl(nullptr), simpleDecl(simpleDecl) {}

    ~IrDecl() override {
        delete type;
        delete specifier;
        delete initDecl;
        delete simpleDecl;
    }

    IrType* getType() const {
        return type;
    }

    IrStorageClassSpecifier* getSpecifier() const {
        return specifier;
    }

    IrInitDeclarator* getInitDecl() const {
        return initDecl;
    }

    IrDeclDeclarator* getSimpleDecl() const {
        return simpleDecl;
    }

    std::string getName() const {
        if (initDecl) {
            return initDecl->getDeclarator()->getName();
        } else if (simpleDecl) {
            return simpleDecl->getName();
        }
        return "";
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--declaration:\n";

        if (specifier) {
            prettyString += specifier->prettyPrint(addIndent(indentSpace));
        }
        if (type) {
            prettyString += type->prettyPrint(addIndent(indentSpace));
        }

        if (initDecl) {
            prettyString += initDecl->prettyPrint(addIndent(indentSpace));
        } else if (simpleDecl) {
            prettyString += simpleDecl->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }

    std::string toString() const override {
        std::string str;
        if (specifier) {
            str += specifier->getValue() + " ";
        }
        if (type) {
            str += type->toString();
        }
        if (initDecl) {
            str += " " + initDecl->toString();
        } else if (simpleDecl) {
            str += " " + simpleDecl->toString();
        }
        return str;
    }

    LlComponent* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        if (IrTypeArray* arrayType = dynamic_cast<IrTypeArray*>(type)) {
            if (simpleDecl) {
                LlComponent *compo = simpleDecl->generateLlIr(builder, symbolTable);
                LlLocation* location = dynamic_cast<LlLocationVar*>(compo);
                symbolTable.putOnTable(*(location->getVarName()), arrayType);
            }
        }
        else if (IrTypeInt* intType = dynamic_cast<IrTypeInt*>(type)){
            if (simpleDecl) {
                LlComponent *compo = simpleDecl->generateLlIr(builder, symbolTable);
                LlLocation* location = dynamic_cast<LlLocationVar*>(compo);
                symbolTable.putOnTable(*(location->getVarName()), intType);
            }
            else if (initDecl) {
                LlComponent *compo = initDecl->generateLlIr(builder, symbolTable);
                LlLocation* location = dynamic_cast<LlLocationVar*>(compo);
                symbolTable.putOnTable(*(location->getVarName()), intType);
            }
        }
        return nullptr;
    }
};


class IrMultiDecl : public IrStatement {
private:
    std::deque<IrDecl*> decls;

public:
    IrMultiDecl(const TSNode& node)
        : IrStatement(node) {}

    ~IrMultiDecl() override {
        for (auto* d : decls) {
            delete d;
        }
    }

    // Add an IrDecl to this container
    void addDeclaration(IrDecl* decl) {
        decls.push_back(decl);
    }

    // Ownership-transfer method
    std::deque<IrDecl*> releaseDeclarations() {
        // Move the entire 'decls' out to a temporary. 
        // 'decls' will become empty.
        std::deque<IrDecl*> temp = std::move(decls); 
        // now 'decls' is empty, so ~IrMultiDecl won't delete these pointers
        return temp;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string result = indentSpace + "|--multiDecl:\n";
        for (auto* d : decls) {
            result += d->prettyPrint(addIndent(indentSpace));
        }
        return result;
    }

    std::string toString() const override {
        std::string out;
        for (auto* d : decls) {
            out += d->toString() + ";\n";
        }
        return out;
    }

    LlComponent* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        for (auto* d : decls) {
            d->generateLlIr(builder, symbolTable);
        }
        return nullptr;
    }
};

#endif