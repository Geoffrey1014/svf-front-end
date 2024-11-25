#include "Ir.h"

class IrLocation : public IrExpr {
protected:
    IrIdent* varName;
    IrType* varType;

public:
    IrLocation(IrIdent* varName, int lineNumber, int colNumber) : IrExpr(lineNumber, colNumber), varName(varName) {}

    IrIdent* getLocationName() const{
        return this->varName;
    }

    IrType* getLocationType() const{
        return this->varType;
    }

    void setLocationType(IrType* type) {
        this->varType = type;
    }

    bool operator==(const Ir& that) const {
        if (&that == this) {
            return true;
        }
        // if (auto thatLocation = dynamic_cast<const IrLocation*>(&that)) {
        //     return *(this->getLocationName()) == *(thatLocation->getLocationName()) && *(this->getLocationType()) == *(thatLocation->getLocationType());
        // }
        return false;
    }

    int hashCode() const {
        return this->varName->hashCode() * this->varType->hashCode();
    }
};


class IrLocationArray : public IrLocation {
private:
    IrExpr* elementIndex;

public:
    IrLocationArray(IrExpr* elementIndex, IrIdent* varName, int lineNumber, int colNumber) : IrLocation(varName, lineNumber, colNumber), elementIndex(IrExpr::canonicalizeExpr(elementIndex)) {}

    IrExpr* getElementIndex() {
        return elementIndex;
    }

    void setElementIndex(IrExpr* elementIndex) {
        this->elementIndex = elementIndex;
    }

    IrType* getExpressionType() override {
        return this->varType;
    }

    bool equals(IrLocation* that) override {
        if (that == this) {
            return true;
        }
        return (this->getLocationName() == that->getLocationName());
    }

    int hashCode() override {
        // hash the it according to its location name
        return std::hash<std::string>{}(this->getLocationName());
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|__arrayAccess\n";

        // print the name
        prettyString += ("  " + indentSpace + "|__name: " + this->varName->getValue() + "\n");

        // print the type
        prettyString += this->varType->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        if(dynamic_cast<IrLocationArray*>(this->elementIndex)){
            LlLocation* indexExpressionTemp = this->elementIndex->generateLlIr(builder, symbolTable);
            LlLocation* arrayReturnTemp = builder->generateTemp();
            LlAssignStmtRegular* arrValueAssignment = new LlAssignStmtRegular(arrayReturnTemp, indexExpressionTemp);
            builder->appendStatement(arrValueAssignment);
            LlLocationArray* locationArray = new LlLocationArray(this->varName->getValue(), dynamic_cast<LlLocationVar*>(arrayReturnTemp));
            return locationArray;
        }
        else{
            LlLocation* indexExpressionTemp = this->elementIndex->generateLlIr(builder, symbolTable);
            LlLocationArray* locationArray = new LlLocationArray(this->varName->getValue(), dynamic_cast<LlLocationVar*>(indexExpressionTemp));
            return locationArray;
        }
    }
};


class IrLocationVar : public IrLocation {
protected:
    Ir* irDeclObject; // IrFieldDeclArray, IrFieldDeclVar, IrParamDecl

public:
    IrLocationVar(IrIdent* varName, int lineNumber, int colNumber) : IrLocation(varName, lineNumber, colNumber) {}

    void setIrDecl(Ir* irDeclObject) {
        this->irDeclObject = irDeclObject;
    }

    Ir* getIrDecl() {
        return this->irDeclObject;
    }

    IrType* getExpressionType() override {
        return this->varType;
    }

    bool equals(IrLocation* that) override {
        if (that == this) {
            return true;
        }
        if (dynamic_cast<IrArgExpr*>(that)) {
            // if that is an instance of IrLocationVar
            if(dynamic_cast<IrLocation*>(dynamic_cast<IrArgExpr*>(that)->getArgValue()))
                return this->getLocationName() == dynamic_cast<IrLocationVar*>(dynamic_cast<IrArgExpr*>(that)->getArgValue())->getLocationName();
        }
        if (dynamic_cast<IrLocationVar*>(that)) {
            return (this->getLocationName() == dynamic_cast<IrLocationVar*>(that)->getLocationName());
        }
        return false;
    }

    int hashCode() override {
        return std::hash<std::string>{}(this->getLocationName());
    }

    std::string prettyPrint(std::string indentSpace) override {
        std::string prettyString = indentSpace + "|--locationVar\n";

        // pretty print name
        prettyString += "  " + indentSpace + "|--name: " + this->varName->getValue() + "\n";

        // pretty print the type
        prettyString += this->varType->prettyPrint("  " + indentSpace);

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder* builder, LlSymbolTable* symbolTable) override {
        LlLocationVar* locationTemp = new LlLocationVar(this->varName->getValue());
        return locationTemp;
    }
};
