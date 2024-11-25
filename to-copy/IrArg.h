#include "Ir.h"

class IrArg : public Ir {
private:
    IrExpr* argValue;

public:
    IrArg(IrExpr* argValue, int lineNum, int colNum) : Ir(lineNum, colNum) {
        this->argValue = IrExpr::canonicalizeExpr(argValue);
    }

    IrExpr* getArgValue() const{
        return argValue;
    }
};

class IrArgExpr : public IrArg {
public:
    IrArgExpr(IrExpr* expr, int lineNum, int colNum) : IrArg(expr, lineNum, colNum) {}

    // std::string semanticCheck(ScopeStack* scopeStack) {
    //     return dynamic_cast<IrExpr*>(this->getArgValue())->semanticCheck(scopeStack);
    // }

    IrType* getArgumentType() {
        return dynamic_cast<IrExpr*>(this->getArgValue())->getExpressionType();
    }

       bool operator==(const Ir& that) const {
        if (&that == this) {
            return true;
        }
        if (auto thatExpr = dynamic_cast<const IrExpr*>(&that)) {
            return *dynamic_cast<IrExpr*>(this->getArgValue()) == *thatExpr;
        }
        if (auto thatArgExpr = dynamic_cast<const IrArgExpr*>(&that)) {
            return *dynamic_cast<IrExpr*>(this->getArgValue()) == *dynamic_cast<IrExpr*>(thatArgExpr->getArgValue());
        }
        return false;
    }

    int hashCode() const {
        return 7 * dynamic_cast<IrExpr*>(this->getArgValue())->hashCode();
    } 
};


class IrArgString : public IrArg {
public:
    IrArgString(std::string string, int lineNum, int colNum) : IrArg(new std::string(string), lineNum, colNum) {}

    // std::string semanticCheck(ScopeStack* scopeStack) {
    //     return "";
    // }

    bool operator==(const Ir& that) const {
        if (&that == this) {
            return true;
        }
        if (auto thatArgString = dynamic_cast<const IrArgString*>(&that)) {
            return *(dynamic_cast<std::string*>(this->getArgValue())) == *(dynamic_cast<std::string*>(thatArgString->getArgValue()));
        }
        return false;
    }

    int hashCode() const {
        return std::hash<std::string>{}(*(dynamic_cast<std::string*>(this->getArgValue())));
    }

    std::string prettyPrint(std::string indentSpace) const {
        std::string prettyString = indentSpace + "|--argString:\n";
        prettyString += "  " + indentSpace + "|--string: \"" + *(dynamic_cast<std::string*>(this->getArgValue())) + "\"\n";
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) {
        // Placeholder function, assuming external classes LlBuilder and LlSymbolTable
        return nullptr;
    }
};

