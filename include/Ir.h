#include <string>
#include "Ll.h"
#include "LlBuilder.h"
#include "LlSymbolTable.h"
#include "IrType.h"

class Ir {
private:
    int lineNumber;
    int colNumber;

public:
    Ir(int lineNumber, int colNumber) : lineNumber(lineNumber), colNumber(colNumber) {}

    int getLineNumber() {
        return this->lineNumber;
    }

    int getColNumber() {
        return this->colNumber;
    }

    // virtual std::string semanticCheck(ScopeStack& scopeStack) = 0;

    LlLocation generateLlIr(LlBuilder& builder, LlSymbolTable& symbolTable);

    std::string prettyPrint(std::string indentSpace);
};


class IrIdent : public Ir {
private:
    std::string name;

public:
    IrIdent(std::string name, int lineNumber, int colNumber) : Ir(lineNumber, colNumber), name(name) {}

    std::string getValue() const{
        return this->name;
    }

    // std::string semanticCheck(ScopeStack* scopeStack) {
    //     return "";
    // }

    bool operator==(const Ir& that) const {
        if (&that == this) {
            return true;
        }
        if (auto thatIdent = dynamic_cast<const IrIdent*>(&that)) {
            return this->getValue() == thatIdent->getValue();
        }
        return false;
    }

    int hashCode() const {
        std::hash<std::string> hasher;
        return hasher(this->name);
    }

    std::string prettyPrint(std::string indentSpace) {
        return "";
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        return nullptr;
    }
};





class IrCodeBlock : public Ir {
private:
    std::vector<IrFieldDecl*> fieldsList;
    std::vector<IrStatement*> stmtsList;

public:
    IrCodeBlock(std::vector<IrFieldDecl*> fieldsList, std::vector<IrStatement*> stmtsList, int lineNumber, int colNumber) 
        : Ir(lineNumber, colNumber), fieldsList(fieldsList), stmtsList(stmtsList) {}

    std::vector<IrFieldDecl*> getFieldsList() {
        return this->fieldsList;
    }

    std::vector<IrStatement*> getStmtsList() {
        return this->stmtsList;
    }

    std::string semanticCheck(ScopeStack* scopeStack) {
        std::string errorMessage = "";

        // 1) check that no identifiers declared twice in same scope
        for (IrFieldDecl* fieldDecl: this->fieldsList) {
            if (scopeStack->checkIfSymbolExistsAtCurrentScope(fieldDecl->getName())) {
                errorMessage += "Duplicate declaration in same scope __filename__"+
                        " line: "+std::to_string(fieldDecl->getLineNumber()) + " col: " +std::to_string(fieldDecl->getColNumber()) + "\n";
            }
            scopeStack->addObjectToCurrentScope(fieldDecl->getName(), fieldDecl);

            // make sure each IrFieldDecl is correct
            errorMessage += fieldDecl->semanticCheck(scopeStack);
        }

        // 2) check that each statement is valid
        for (IrStatement* stmt: this->stmtsList) {
            errorMessage += stmt->semanticCheck(scopeStack);
        }

        return errorMessage;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        for (IrFieldDecl* field: this->fieldsList) {
            if(dynamic_cast<IrFieldDeclArray*>(field)){
                field->generateLlIr(builder, symbolTable);
            }
        }
        for (IrStatement* statement: this->stmtsList) {
            if(dynamic_cast<IrStmtContinue*>(statement)){
                if(builder->pickPocket() != nullptr) {
                    dynamic_cast<IrStatement*>(builder->pickPocket())->generateLlIr(builder, symbolTable);
                    builder->emptyPocket();
                }
            }
            statement->generateLlIr(builder, symbolTable);
        }
        return nullptr;
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
