#ifndef IR_H
#define IR_H
#include <string>
#include <tree_sitter/api.h> 
#include <vector>
#include <deque>
#include "Ll.h"
#include "LlBuilder.h"
#include "SymbolTable.h"

using namespace std;

class Ir {
    private:
        const TSNode & node;
    public:
    Ir(const TSNode & node): node(node) {}
    virtual ~Ir() = default;

    int getLineNumber() {
        return ts_node_start_point(node).row;
    }

    int getColNumber() {
        return ts_node_start_point(node).column;
    }

    const TSNode& getNode() {
        return node;
    }

    virtual bool operator==(const Ir& other) const {
        return this == &other;
    }

    // virtual std::string semanticCheck(ScopeStack& scopeStack) = 0;
    virtual LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) = 0;
    
    virtual std::string prettyPrint(std::string indentSpace) const =0;
    virtual std::string toString() const = 0;
    
    std::string addIndent(const std::string& baseIndent, int level = 1) const {
        return baseIndent + std::string(level * 2, ' '); // 2 spaces per level
    }

};

class IrExpr : public virtual Ir {
public:
    IrExpr(const TSNode & node) : Ir(node) {}
    ~IrExpr() = default;
    
    std::string toString() const override {
        return "baseIrExpr";
    }

    bool operator==(const IrExpr& other) const {
        // Implementation of operator==
        if (&other == this) {
            return true;
        }
        return false;
    }

    int hashCode() const {
        // Implementation of hashCode
        return 0;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        std::cerr << "IrExpr Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("Error")); 
    }

    virtual const std::string getName() const {
        return "";
    }
};


class IrBinaryExpr : public IrExpr {
private:
    std::string operation;
    IrExpr* leftOperand;
    IrExpr* rightOperand;

public:
    IrBinaryExpr(std::string& operation, IrExpr* leftOperand, IrExpr* rightOperand, const TSNode & node) 
        : Ir(node), IrExpr(node), operation(operation), 
          leftOperand(leftOperand), rightOperand(rightOperand) {}
    ~IrBinaryExpr() {
        delete leftOperand;
        delete rightOperand;
    }
    IrExpr* getLeftOperand() {
        return this->leftOperand;
    }

    IrExpr* getRightOperand() {
        return this->rightOperand;
    }

    std::string& getOperation() {
        return this->operation;
    }

    std::string toString() const override{
        return leftOperand->toString() + " " + operation + " " + rightOperand->toString();
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--binaryExpr\n";

        prettyString += addIndent(indentSpace) + + "|--lhs\n";
        prettyString += this->leftOperand->prettyPrint(addIndent(indentSpace, 2));

        prettyString += addIndent(indentSpace) + "|--op: " + operation + "\n";

        prettyString += addIndent(indentSpace) + "|--rhs\n";
        prettyString += this->rightOperand->prettyPrint(addIndent(indentSpace, 2));

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* left = leftOperand->generateLlIr(builder, symbolTable);
        LlLocation* right = rightOperand->generateLlIr(builder, symbolTable);
        LlLocationVar* result = builder.generateTemp();
        builder.appendStatement(new LlAssignStmtBinaryOp(result, left, operation, right));
        return result;
    }
       
};


class IrType : public Ir {
protected:
    int width = 0;

public:
    IrType(const TSNode& node) : Ir(node) {}
    virtual ~IrType() = default;
    int hashCode() const{ return 0;}
    virtual IrType* clone() const = 0;
    int getWidth() const { return width; }
};

class IrLiteral : public IrExpr {
public:
    IrLiteral(const TSNode& node) : IrExpr(node), Ir(node) {}

    virtual ~IrLiteral() = default;

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        std::cerr << "IrLiteral Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("Error")); 
    }
};

class IrTypeBool : public IrType {

public:
    IrTypeBool(const TSNode& node) : IrType(node) {width = 1;}
    ~IrTypeBool() override = default;

    IrTypeBool* clone() const override {
        return new IrTypeBool(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeBool*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 11; // some arbitrary prime
    }

    std::string toString() const override{
        return "bool";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: bool\n";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return nullptr;
    }
};

class IrTypeVoid : public IrType {

public:
    IrTypeVoid(const TSNode& node) : IrType(node) {}
    ~IrTypeVoid() override = default;

    IrTypeVoid* clone() const override {
        return new IrTypeVoid(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeVoid*>(&that) == nullptr) {
            return false;
        }

        return true;
    }

    int hashCode() const {
        return 13; // some arbitrary prime
    }

    std::string toString() const override{
        return "void";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: void\n";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return nullptr;
    }
};

class IrTypeInt : public IrType {

public:
    IrTypeInt(const TSNode& node) : IrType(node) { width = 4;}
    ~IrTypeInt() override = default;

    IrTypeInt* clone() const override {
        return new IrTypeInt(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeInt*>(&that) == nullptr) {
            return false;
        }

        return true;
    }

    int hashCode() const {
        return 17; // some arbitrary prime
    }

    std::string toString() const override{
        return "int";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: int\n";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return nullptr;
    }
};

class IrTypeString : public IrType {

public:
    IrTypeString(const TSNode& node) : IrType(node) {}
    ~IrTypeString() override = default;

    IrTypeString* clone() const override {
        return new IrTypeString(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeString*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 7; // some arbitrary prime
    }

    std::string toString() const override{
        return "string";
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: string\n";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return nullptr;
    }
};

class IrTypeChar : public IrType {
public:
    IrTypeChar(const TSNode& node) : IrType(node) { width = 1;}
    ~IrTypeChar() override = default;

    IrTypeChar* clone() const override {
        return new IrTypeChar(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (dynamic_cast<const IrTypeChar*>(&that) == nullptr) {
            return false;
        }
        return true;
    }

    int hashCode() const {
        return 19; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--type: char\n";
    }

    std::string toString() const override{
        return "char";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return nullptr;
    }
};

class IrTypeArray : public IrType {
private:
    IrType* baseType;
    deque<IrLiteral*> dimension;
    int width = 0;

public:
    IrTypeArray(IrType* baseType, deque<IrLiteral*> dimension, const TSNode& node)
            : IrType(node), baseType(baseType), dimension(dimension) {}
    ~IrTypeArray();

    IrType* getBaseType() const {
        return baseType;
    }

    deque<IrLiteral*> getDimension() const {
        return dimension;
    }

    int getDimensionSize() const {
        return dimension.size();
    }

    IrTypeArray* clone() const override {
        return new IrTypeArray(*this);
    }

    bool operator==(const Ir& that) const override{
        if (&that == this) {
            return true;
        }
        if (auto thatTypeArray = dynamic_cast<const IrTypeArray*>(&that)) {
            if (this->baseType != thatTypeArray->baseType) {
                return false;
            }
            if (this->dimension.size() != thatTypeArray->dimension.size()) {
                return false;
            }
            for (int i = 0; i < this->dimension.size(); i++) {
                if (this->dimension[i] != thatTypeArray->dimension[i]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    int hashCode() const;

    std::string prettyPrint(std::string indentSpace) const override ;

    std::string toString() const override;

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return nullptr;
    }

};


class IrLiteralBool : public IrLiteral {
private:
    bool value;

public:
    IrLiteralBool(bool value, const TSNode& node) : IrLiteral(node), Ir(node), value(value) {}
    ~IrLiteralBool() override = default;
    IrType* getExpressionType() {
        return new IrTypeBool(getNode());
    }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--boolLiteral\n";
        prettyPrint += addIndent(indentSpace)+ "|--value: " + (this->value ? "true" : "false") + "\n";
        return prettyPrint;
    }

    std::string toString() {
        return "IrLiteralBool";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLiteralBool *llLiteralBool = new LlLiteralBool(this->value);
        LlLocationVar *llLocationVar = builder.generateTemp();
        LlAssignStmt* llAssignStmt = new LlAssignStmtRegular(llLocationVar, llLiteralBool);
        builder.appendStatement(llAssignStmt);
        return llLocationVar;
    }
};


class IrLiteralChar : public IrLiteral {
private:
    char value;

public:
    IrLiteralChar(char value, const TSNode& node) : IrLiteral(node), Ir(node), value(value) {}
    ~IrLiteralChar() override = default;
    IrType* getExpressionType() {
        return new IrTypeVoid(getNode());
    }

    std::string prettyPrint(std::string indentSpace) {
        std::string prettyPrint = indentSpace + "|--charLiteral\n";
        prettyPrint += addIndent(indentSpace) + "|--value: " + this->value + "\n";
        return prettyPrint;
    }

    std::string toString() const override{
        return "IrLiteralChar";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLiteralChar *llLiteral = new LlLiteralChar(this->value);
        LlLocationVar * llLocationVar = builder.generateTemp();
        LlAssignStmt* llAssignStmt = new LlAssignStmtRegular(llLocationVar, llLiteral);
        builder.appendStatement(llAssignStmt);
        return llLocationVar;
    }
};


class IrLiteralNumber : public IrLiteral {
private:
    int value;

public:
    IrLiteralNumber(long value, const TSNode& node) : IrLiteral(node), Ir(node), value(value) {}
    ~IrLiteralNumber() override = default;
    int getValue() {
        return this->value;
    }

    IrType* getExpressionType() {
        return new IrTypeInt(getNode());
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--NumberLiteral\n";
        prettyPrint += addIndent(indentSpace) + "|--value: " + std::to_string(this->value) + "\n";
        return prettyPrint;
    }

    std::string toString() const override{
        return std::to_string(value); // "IrLiteralNumber";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLiteralInt * llLiteralInt = new LlLiteralInt(this->value);
        LlLocationVar * llLocationVar = builder.generateTemp();
        LlAssignStmt* llAssignStmt = new LlAssignStmtRegular(llLocationVar, llLiteralInt);
        builder.appendStatement(llAssignStmt);
        return llLocationVar;
    }
};

class IrLiteralStringContent : public IrLiteral {
private:
    std::string value;

public:
    IrLiteralStringContent(const std::string& value, const TSNode& node)
        : IrLiteral(node), Ir(node), value(value) {}

        ~IrLiteralStringContent() override = default;

    const std::string& getValue() const {
        return value;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        return indentSpace + "|--stringContent: " + value + "\n";
    }

    std::string toString() const override{
        return "IrLiteralStringContent";
    }
};


class IrLiteralString : public IrLiteral {
private:
    IrLiteralStringContent* stringContent;

public:
    IrLiteralString(IrLiteralStringContent* stringContent, const TSNode& node)
        : IrLiteral(node), Ir(node), stringContent(stringContent) {}

    ~IrLiteralString() {
        delete stringContent;
    }

    const std::string& getValue() const {
        return stringContent->getValue();
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyPrint = indentSpace + "|--StringLiteral\n";
        prettyPrint += stringContent->prettyPrint(addIndent(indentSpace));
        return prettyPrint;
    }

    std::string toString() const override{
        return "IrLiteralString";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLiteralString* llLiteral = new LlLiteralString(new string(this->stringContent->getValue()));
        LlLocationVar * llLocationVar = builder.generateTemp();
        LlAssignStmt* llAssignStmt = new LlAssignStmtRegular(llLocationVar, llLiteral);
        builder.appendStatement(llAssignStmt);
        return llLocationVar;
    }
};


class IrArgList : public Ir {
private:
    std::deque<IrExpr*> argsList;
public:
    IrArgList(const TSNode& node) : Ir(node) {}
    ~IrArgList() {
        for (IrExpr* arg: this->argsList) {
            delete arg;
        }
    }   

    std::deque<IrExpr*> getArgsList() {
        return this->argsList;
    }

    void addToArgsList(IrExpr* newArg) {
        this->argsList.push_front(newArg);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--argList:\n";

        for (IrExpr* arg: this->argsList) {
            prettyString += arg->prettyPrint(addIndent(indentSpace)); 
        }

        return prettyString;
    }

    std::string toString() const override{
        std::string argsString = "";
        for (IrExpr* arg: this->argsList) {
            argsString += arg->toString() + ", ";
        }
        return argsString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        for (IrExpr* arg: this->argsList) {
            arg->generateLlIr(builder, symbolTable);
        }
        return nullptr;
    }
};

class IrPreprocInclude : public Ir {
private:
    IrLiteralString* path;

public:
    IrPreprocInclude(IrLiteralString* path, const TSNode& node) : Ir(node), path(path) {}
    ~IrPreprocInclude() {
        delete path;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--preprocInclude\n";
        prettyString += path->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        return "#include " + path->getValue();
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
    }
};


class IrDeclDeclarator : public virtual Ir {
public:
    IrDeclDeclarator(const TSNode& node) : Ir(node) {}
    virtual const std::string getName() const {
        static const std::string emptyString = "";
        return emptyString;
    }
};

class IrAbstractPointerDeclarator : public IrDeclDeclarator {
private:
    IrDeclDeclarator* baseDeclarator;
public:
    IrAbstractPointerDeclarator(IrDeclDeclarator* base, const TSNode& node)
        : Ir(node), IrDeclDeclarator(node), baseDeclarator(base) {}

    ~IrAbstractPointerDeclarator() {
        delete baseDeclarator;
    }

    const std::string getName() const override {
        if (baseDeclarator) {
            return baseDeclarator->getName();
        }
        return "";
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string str = indentSpace + "|--abstract_pointer_declarator: *\n";
        if(baseDeclarator){
            str += baseDeclarator->prettyPrint(addIndent(indentSpace));
        }
        return str;
    }

    std::string toString() const override{
        if (baseDeclarator) {
            return baseDeclarator->toString() + "*";
        }
        return "*";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
    }
};

class IrIdent : public IrDeclDeclarator, public IrExpr {
private:
    const std::string name;
    bool isTypeAlias;

public:
    IrIdent(const std::string& name, const TSNode & node, bool isTypeAlias = false) : Ir(node), IrDeclDeclarator(node), IrExpr(node), name(name) {}
    ~IrIdent() = default;

    const std::string & getValue() const {
        return name;
    }

    bool operator==(const Ir & that) const override{
        if (&that == this) {
            return true;
        }
        if (auto thatIdent = dynamic_cast<const IrIdent *>(&that)) {
            return this->name == thatIdent->name;
        }
        return false;
    }

    int hashCode() const {
        std::hash<std::string> hasher;
        return hasher(this->name);
    }
    bool isType() const { return isTypeAlias; }
    void markAsTypeAlias() { isTypeAlias = true; }

    std::string prettyPrint(std::string indentSpace) const override{
        return indentSpace + "|--id: " + name + "\n";
    }

    const std::string getName() const override {
        return name;
    }

    std::string toString() const override{
        return name;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        return new LlLocationVar(new std::string(name));
    }

};

class IrPreprocArg : public Ir {
public:
    std::string text;

    IrPreprocArg(const std::string& t, const TSNode& node)
        : Ir(node), text(t) {}
    
    ~IrPreprocArg() = default;

    std::string prettyPrint(std::string indentSpace) const override {
        return indentSpace + "|-- preproc_arg: " + text + "\n";
    }

    std::string toString() const override {
        return "PreprocArg(" + text + ")";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
    }
};

class IrPreprocDef : public Ir {
public:
    IrIdent* name;  
    IrPreprocArg* value; 

    IrPreprocDef(IrIdent* n, const TSNode& node, 
                 IrPreprocArg* v = nullptr)
        : Ir(node), name(n), value(v) {}
    
    ~IrPreprocDef() {
        delete name;
        delete value; 
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string result = indentSpace + "|-- preproc_def: " + name->getValue();
        if (value) result += " " + value->text;
        result += "\n";
        return result;
    }

    std::string toString() const override {
        return "#define " + name->toString() + (value ? " " + value->text : "");
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
    }
};


class IrNonBinaryExpr : public IrExpr {
public:
    IrNonBinaryExpr(const TSNode & node) : IrExpr(node) {}
};

class IrCallExpr : public IrNonBinaryExpr {
private:
    IrIdent* functionName;
    IrArgList* argList;

public:
    IrCallExpr(IrIdent* functionName, IrArgList* argList, const TSNode & node) 
        : Ir(node), IrNonBinaryExpr(node), functionName(functionName), argList(argList) {}
    ~IrCallExpr() {
        delete functionName;
        delete argList;
    }
    IrIdent* getFunctionName() {
        return this->functionName;
    }

    IrArgList* getArgList() {
        return this->argList;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--callExpr\n";

        prettyString += addIndent(indentSpace) + "|--functionName\n";
        prettyString += this->functionName->prettyPrint(addIndent(indentSpace, 2));
        prettyString += this->argList->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }

    std::string toString() const override{
        return functionName->toString() + " (" + argList->toString() + ")";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        std::vector<LlComponent*> argsList;
        for(auto& arg : this->argList->getArgsList()) {
            argsList.push_back(arg->generateLlIr(builder, symbolTable));
        }
        LlLocationVar* returnLocation = builder.generateTemp();

        LlMethodCallStmt* methodCallStmt = new LlMethodCallStmt(functionName->getName(), argsList, returnLocation);
        builder.appendStatement(methodCallStmt);
        return returnLocation;
    }
};

class IrAssignExpr : public IrNonBinaryExpr {
private:
    IrExpr* lhs;
    IrExpr* rhs;
    std::string op;
public:
    IrAssignExpr(IrExpr* lhs, IrExpr* rhs, const std::string& op, const TSNode& node)
        : Ir(node), IrNonBinaryExpr(node), lhs(lhs), rhs(rhs), op(op) {}

    ~IrAssignExpr() {
        delete lhs;
        delete rhs;
    }

    IrExpr* getLhs() const { return lhs; }
    IrExpr* getRhs() const { return rhs; }
    std::string getOp() const { return op; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--assignExpr\n";
        prettyString += addIndent(indentSpace) + "|--lhs\n";
        prettyString += lhs->prettyPrint(addIndent(indentSpace, 2));
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += addIndent(indentSpace) + "|--rhs\n";
        prettyString += rhs->prettyPrint(addIndent(indentSpace, 2));
        return prettyString;
    }

    std::string toString() const override {
        return lhs->toString() + " " + op + " " + rhs->toString();
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* left = lhs->generateLlIr(builder, symbolTable);
        LlLocation* right = rhs->generateLlIr(builder, symbolTable);
        LlLocation* location = dynamic_cast<LlLocation*>(left);
        std::string operation = op;
        if (op != "=") {
            operation = op.substr(0, op.size() - 1);
            LlAssignStmtBinaryOp* assignStmt = new LlAssignStmtBinaryOp(location, left, operation, right);
            builder.appendStatement(assignStmt);
        }
        else {
            LlAssignStmtRegular* assignStmt = new LlAssignStmtRegular(location, right);
            builder.appendStatement(assignStmt);
        }
        return nullptr;
    }

};

class IrFieldExpr : public IrNonBinaryExpr {
private:
    IrExpr* baseExpr;
    IrIdent* fieldName;
    bool isArrow; 

public:
    IrFieldExpr(IrExpr* base, IrIdent* field, bool isArrow, const TSNode & node) 
      : Ir(node), IrNonBinaryExpr(node), baseExpr(base), fieldName(field), isArrow(isArrow) {}

    ~IrFieldExpr() override {
        delete baseExpr;
        delete fieldName;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string op = isArrow ? "->" : ".";
        std::string prettyString = indentSpace + "|--field_expression\n";
        prettyString += baseExpr->prettyPrint(addIndent(indentSpace));
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += fieldName->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        std::string op = isArrow ? "->" : ".";
        return baseExpr->toString() + op + fieldName->toString();
    }   

    IrExpr* getBaseExpr() const { return baseExpr; }
    IrIdent* getFieldName() const { return fieldName; }
    bool getIsArrow() const { return isArrow; }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* base = baseExpr->generateLlIr(builder, symbolTable);
        //IrFieldExpr codegen is not fully implemented
        return base;    
    }

};

class IrPointerExpr : public IrNonBinaryExpr {
private:
    IrExpr* argument;
    bool isAddressOf;   // true if operator is '&'
    bool isDereference; // true if operator is '*'

public:
    IrPointerExpr(IrExpr* arg, bool addressOf, bool dereference, const TSNode & node)
        : Ir(node), IrNonBinaryExpr(node), argument(arg), isAddressOf(addressOf), isDereference(dereference) {}

    ~IrPointerExpr() override {
        delete argument;
    }

    IrExpr* getArgument() const { return argument; }
    bool getIsAddressOf() const { return isAddressOf; }
    bool getIsDereference() const { return isDereference; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string op = isAddressOf ? "&" : "*";
        std::string prettyString = indentSpace + "|--pointer_expression\n";

        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        if (argument) {
            prettyString += argument->prettyPrint(addIndent(indentSpace));
        }
        
        return prettyString;
    }

    std::string toString() const override{
        std::string op = isAddressOf ? "&" : "*";
        return op + argument->toString();
    }
};

class IrParenthesizedExpr : public IrNonBinaryExpr {
private:
    IrExpr* innerExpr;

public:
    IrParenthesizedExpr(IrExpr* expr, const TSNode & node) 
        : Ir(node), IrNonBinaryExpr(node), innerExpr(expr) {}

    ~IrParenthesizedExpr() {
        delete innerExpr;
    }

    IrExpr* getInnerExpr() const { return innerExpr; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--parenthesizedExpr\n";
        if (innerExpr) {
            prettyString += innerExpr->prettyPrint(addIndent(indentSpace));
        }
        return prettyString;
    }

    std::string toString() const override {
        return "(" + innerExpr->toString() + ")";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        return innerExpr->generateLlIr(builder, symbolTable);
    }
};

class IrUnaryExpr : public IrNonBinaryExpr {
private:
    std::string op;  // Operator (e.g., '-', '!')
    IrExpr* argument;

public:
    IrUnaryExpr(const std::string& op, IrExpr* arg, const TSNode &node) 
        : Ir(node), IrNonBinaryExpr(node), op(op), argument(arg) {}

    ~IrUnaryExpr() {
        delete argument;
    }

    IrExpr* getArgument() const { return argument; }
    std::string getOperator() const { return op; }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--unaryExpr\n";
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += argument->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override {
        return op + argument->toString();
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* arg = argument->generateLlIr(builder, symbolTable);
        LlLocation* returnLocation = builder.generateTemp();
        LlAssignStmtUnaryOp* unaryOp = new LlAssignStmtUnaryOp(returnLocation, arg, new std::string(op));
        builder.appendStatement(unaryOp);
        return returnLocation;
    }
};


class IrStatement : public Ir {
public:
    IrStatement(const TSNode& node) : Ir(node) {}
    virtual ~IrStatement() = default;
    std::string toString() const override{
        return "IrStatement";
    }
    
    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        std::cerr << "Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("")); // Return empty location
    }
};

class IrStmtReturn : public IrStatement {
public:
    IrStmtReturn(const TSNode& node) : IrStatement(node) {}
    virtual ~IrStmtReturn() = default;
    
    virtual LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        std::cerr << "Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("")); // Return empty location
    }
};

class IrStmtReturnExpr : public IrStmtReturn {
private:
    IrExpr* result;

public:
    IrStmtReturnExpr(IrExpr* result, const TSNode& node) : IrStmtReturn(node), result(result) {}
    virtual ~IrStmtReturnExpr() {
        delete result;
    }
    // IrType* getExpressionType() override {
    //     return this->result->getExpressionType();
    // }

    std::string prettyPrint(std::string indentSpace)const override {
        std::string prettyString = indentSpace + "|--returnExpr\n";

        prettyString += this->result->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }
    std::string toString() const override{
        string s = "return " + result->toString();
        return s;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLocation* resultVar = this->result->generateLlIr(builder, symbolTable);
        LlReturn* returnStmt = new LlReturn(resultVar);
        builder.appendStatement(returnStmt);
        return nullptr;
    }
};

class IrStmtReturnVoid : public IrStmtReturn {
public:
    IrStmtReturnVoid(const TSNode& node) : IrStmtReturn(node) {}
    virtual ~IrStmtReturnVoid() = default;
    // IrType* getExpressionType() override {
    //     return new IrTypeVoid(this->getLineNumber(), this->getColNumber());
    // }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--returnVoid\n";
        return prettyString;
    }
    std::string toString() const override{
        return "IrStmtReturnVoid";
    }
    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlReturn* returnStmt = new LlReturn(nullptr);
        builder.appendStatement(returnStmt);
        return nullptr;
    }
};

class IrCompoundStmt : public IrStatement {
private:
    std::deque<IrStatement*> stmtsList;
public:
    IrCompoundStmt(const TSNode& node)
        : IrStatement(node),
          stmtsList() {}
    virtual ~IrCompoundStmt() {
        for (IrStatement* stmt: this->stmtsList) {
            delete stmt;
        }
    }

    void addStmtToFront(IrStatement* stmt) {
        this->stmtsList.push_front(stmt);
    }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--compoundStmt:\n";

        for (IrStatement* statement: this->stmtsList) {
            prettyString += statement->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }
    std::string toString() const override{
        std::string s = "";
        for (IrStatement* statement: this->stmtsList) {
            s += statement->toString() + "\n";
        }
        return s;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        for (IrStatement* stmt: this->stmtsList) {
            stmt->generateLlIr(builder, symbolTable);
        }
        return nullptr;
    }
};

class IrExprStmt : public IrStatement {
private:
    IrExpr* expr;
public:
    IrExprStmt(IrExpr* expr, const TSNode& node) : IrStatement(node), expr(expr) {}
    virtual ~IrExprStmt() {
        delete expr;
    }

    IrExpr* getExpr() {
        return this->expr;
    }

    std::string prettyPrint(std::string indentSpace)const override  {
        std::string prettyString = indentSpace + "|--exprStmt\n";
        prettyString += this->expr->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        string s = expr->toString();
        return s;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return expr->generateLlIr(builder, symbolTable);
    }
};

class IrElseClause : public IrStatement {
private:
    IrStatement* alternative;

public:
    IrElseClause(IrStatement* alternative, const TSNode& node)
        : IrStatement(node), alternative(alternative) {}

    ~IrElseClause() {
        delete alternative;
    }

    IrStatement* getAlternative() const {
        return alternative;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--elseClause\n";
        prettyString += alternative->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override {
        std::string result = "else " + alternative->toString();
        return result;
    }

    virtual LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        alternative->generateLlIr(builder, symbolTable);
        return nullptr;
    }
};

class IrIfStmt : public IrStatement {
private:
    IrParenthesizedExpr* condition;  
    IrStatement* thenBody;
    IrElseClause* elseBody;  
public:
    IrIfStmt(IrParenthesizedExpr* condition, IrStatement* thenBody, IrElseClause* elseBody, const TSNode& node)
        : IrStatement(node), condition(condition), thenBody(thenBody), elseBody(elseBody) {}

    ~IrIfStmt() {
        delete condition;
        delete thenBody;
        delete elseBody;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--ifStmt\n";

        // Print condition
        prettyString += addIndent(indentSpace) + "|--condition\n";
        prettyString += condition->prettyPrint(addIndent(indentSpace, 2));

        // Print thenBody (if block)
        prettyString += addIndent(indentSpace) + "|--consequence\n";
        prettyString += thenBody->prettyPrint(addIndent(indentSpace, 2));

        // Print elseBody (if exists)
        if (elseBody) {
            prettyString += addIndent(indentSpace) + "|--else\n";
            prettyString += elseBody->prettyPrint(addIndent(indentSpace, 2));
        }

        return prettyString;
    }

    std::string toString() const override {
        std::string result = "if " + condition->toString() + " " + thenBody->toString();
        if (elseBody) {
            result += " " + elseBody->toString();
        }
        return result;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLocation* conditionVar = this->condition->generateLlIr(builder, symbolTable);

        std::string label = builder.generateLabel();
        std::string* ifThenLabel = new std::string();
        ifThenLabel->append("if.then.");
        ifThenLabel->append(label);
        
        std::string* endLabel = new std::string();
        endLabel->append("if.end.");
        endLabel->append(label);

        
        LlJumpConditional* conditionalJump = new LlJumpConditional(ifThenLabel,conditionVar);
        builder.appendStatement(conditionalJump);

        if (elseBody) {
            std::string* elseLabel = new std::string();
            elseLabel->append("if.else.");
            elseLabel->append(label);
            LlEmptyStmt* emptyStmtElse = new LlEmptyStmt();
            LlJumpUnconditional *jumpUnconditionalToElse = new LlJumpUnconditional(elseLabel);
            builder.appendStatement(jumpUnconditionalToElse);
            builder.appendStatement(*elseLabel, emptyStmtElse);
            elseBody->generateLlIr(builder, symbolTable);
        }
        
        LlJumpUnconditional *jumpUnconditionalToEnd = new LlJumpUnconditional(endLabel);
        builder.appendStatement(jumpUnconditionalToEnd);

        // add the label to the if body block
        LlEmptyStmt* emptyStmt = new LlEmptyStmt();
        builder.appendStatement(*ifThenLabel, emptyStmt);
        thenBody->generateLlIr(builder, symbolTable);


        // append end if label
        LlEmptyStmt* endIfEmptyStmt = new LlEmptyStmt();
        builder.appendStatement(*endLabel, endIfEmptyStmt);
        return nullptr;
    }
};


class IrStorageClassSpecifier : public Ir {
private:
    std::string specifier;

public:
    IrStorageClassSpecifier(const std::string& specifier, const TSNode& node)
        : Ir(node), specifier(specifier) {}

    const std::string& getValue() const {
        return this->specifier;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        return indentSpace + "|--storageClassSpecifier: " + this->specifier + "\n";
    }

    std::string toString() const override{
        return "IrStorageClassSpecifier: " + this->specifier;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
    }
};


class IrFieldDecl : public Ir {
private:
    IrType* type;                        // Type of the field
    IrDeclDeclarator* declarator;        // Field name or declarator

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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* declaratorComponent = declarator->generateLlIr(builder, symbolTable);
        LlLocationVar* location = dynamic_cast<LlLocationVar*>(declaratorComponent);
        symbolTable.putOnVarTable(*location->getVarName(), type);
        return location;
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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
    }
};

class IrParamDecl : public Ir {
private:
    IrType* paramType;                 // The type of the parameter
    // can be unnamed parameters or void
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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement
        return nullptr;
    }
};

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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        return new LlLocationVar(new std::string(getName()));
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

    IrType* getReturnType() const { return returnType; }
    IrFunctionDecl* getFunctionDecl() const { return functionDecl; }
    IrCompoundStmt* getCompoundStmt() const { return compoundStmt; }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
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
    IrDeclDeclarator* declarator; 
    IrExpr* initializer;     

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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* compo = declarator->generateLlIr(builder, symbolTable);
        LlLocationVar* location = dynamic_cast<LlLocationVar*>(compo);
        LlLocation* init = initializer->generateLlIr(builder, symbolTable);
        LlAssignStmtRegular* assignStmt = new LlAssignStmtRegular(location, init);
        builder.appendStatement(assignStmt);
        return location;
    }
};

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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        auto handleDeclaration = [&](IrType* type) {
            // simpleDecl most case is identifier (id)
            if (simpleDecl) {
                LlLocation *location = simpleDecl->generateLlIr(builder, symbolTable);
                symbolTable.putOnVarTable(*(location->getVarName()), type);
            } 
            else if (initDecl) {
                LlLocation *location = initDecl->generateLlIr(builder, symbolTable);
                symbolTable.putOnVarTable(*(location->getVarName()), type);
            }
        };
        if(auto castType = dynamic_cast<IrType*>(type)){
            handleDeclaration(type);
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

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        for (auto* d : decls) {
            d->generateLlIr(builder, symbolTable);
        }
        return nullptr;
    }
};


class IrForStmt : public IrStatement {
private:
    IrAssignExpr* initializer;
    IrExpr* condition;
    IrExpr* update;
    IrStatement* body;

public:
    IrForStmt(IrAssignExpr* initializer, IrExpr* condition, IrExpr* update, IrStatement* body, const TSNode& node)
        : IrStatement(node), initializer(initializer), condition(condition), update(update), body(body) {}
    ~IrForStmt() {
        delete initializer;
        delete condition;
        delete update;
        delete body;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--forStmt\n";

        if (initializer) {
            prettyString += addIndent(indentSpace) + "|--initializer\n";
            prettyString += initializer->prettyPrint(addIndent(indentSpace, 2));
        }
        if (condition) {
            prettyString += addIndent(indentSpace) + "|--condition\n";
            prettyString += condition->prettyPrint(addIndent(indentSpace, 2));
        }
        if (update) {
            prettyString += addIndent(indentSpace) + "|--update\n";
            prettyString += update->prettyPrint(addIndent(indentSpace, 2));
        }
        prettyString += addIndent(indentSpace) + "|--body\n";
        prettyString += body->prettyPrint(addIndent(indentSpace, 2));

        return prettyString;
    }

    std::string toString() const override {
        std::string result = "for (";

        result += initializer->toString() + "; ";
        result += condition->toString()+ "; ";
        result += update->toString()+ ") ";

        if (body) {
            result += "\t\n" + body->toString();
        } else {
            result += "{}";
        }
        return result;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        if (initializer) {
            initializer->generateLlIr(builder, symbolTable);
        }

        std::string forLable = builder.generateLabel();
        std::string* condLabel = new std::string();
        condLabel->append("for.cond.");
        condLabel->append(forLable);

        std::string* bodyLabel = new std::string();
        bodyLabel->append("for.body.");
        bodyLabel->append(forLable);
        
        std::string* endLabel = new std::string();
        endLabel->append("for.end.");
        endLabel->append(forLable);

        LlEmptyStmt* emptyStmtFor = new LlEmptyStmt();
        builder.appendStatement(*condLabel, emptyStmtFor);

        LlLocation* conditionVar = this->condition->generateLlIr(builder, symbolTable);
        LlJumpConditional* conditionalJump = new LlJumpConditional(bodyLabel,conditionVar);
        builder.appendStatement(conditionalJump);
        LlJumpUnconditional* jumpToForEnd = new LlJumpUnconditional(endLabel);
        builder.appendStatement(jumpToForEnd);

        LlEmptyStmt* emptyStmtForBody = new LlEmptyStmt();
        builder.appendStatement(*bodyLabel, emptyStmtForBody);
        if (body) {
            body->generateLlIr(builder, symbolTable);
        }

        std::string* incLabel = new std::string();
        incLabel->append("for.inc.");
        incLabel->append(forLable);
        LlEmptyStmt* emptyStmtForInc = new LlEmptyStmt();
        builder.appendStatement(*incLabel, emptyStmtForInc);
        update->generateLlIr(builder, symbolTable);

        LlJumpUnconditional* jumpToFor = new LlJumpUnconditional(condLabel);
        builder.appendStatement(jumpToFor);

        LlEmptyStmt* emptyStmtForEnd = new LlEmptyStmt();
        builder.appendStatement(*endLabel, emptyStmtForEnd);

        return nullptr;
    }
};


class IrSubscriptExpr : public IrNonBinaryExpr {
private:
    IrExpr* baseExpr;   // The array or object being indexed
    IrExpr* indexExpr;  // The index expression
    int level = 0;

public:
    IrSubscriptExpr(IrExpr* baseExpr, IrExpr* indexExpr, const TSNode& node)
        : Ir(node), IrNonBinaryExpr(node), baseExpr(baseExpr), indexExpr(indexExpr){}

    ~IrSubscriptExpr() {
        delete baseExpr;
        delete indexExpr;
    }

    void setLevel(int l){
        level = l;
    }

    const std::string getName() const override {
        return baseExpr->getName();
    }

    IrExpr* getBaseExpr() const {
        return baseExpr;
    }

    IrExpr* getIndexExpr() const {
        return indexExpr;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--subscript_expression\n";

        if (baseExpr) {
            prettyString += baseExpr->prettyPrint(addIndent(indentSpace));
        } else {
            prettyString += addIndent(indentSpace) + "|--Error: Missing base expression\n";
        }

        prettyString += addIndent(indentSpace) + "|--index\n";
        // Print index expression
        if (indexExpr) {
             prettyString += indexExpr->prettyPrint(addIndent(indentSpace, 2));
        } else {
            prettyString += addIndent(indentSpace, 2) + "|--Error: Missing index expression\n";
        }
        return prettyString;
    }

    std::string toString() const override{
        return baseExpr->toString() + "[" + indexExpr->toString() + "]";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        std::string baseName = baseExpr->getName();
        IrType* type = symbolTable.getFromVarTable(baseName);
        IrTypeArray* arrayType = dynamic_cast<IrTypeArray*>(type);

        if (!arrayType) {
            std::cerr << "Error: " << baseName << " is not an array." << std::endl;
            return nullptr;
        }

        deque<IrLiteral*> dims = arrayType->getDimension();
        int elemWidth = arrayType->getBaseType()->getWidth();
        int arrSize = dims.size();

        IrExpr* currentExpr = this;
        int currentLevel = arrSize; 

        int cumulativeMultiplier = elemWidth;

        LlLocation* offsetTemp = nullptr;

        while (auto* sub = dynamic_cast<IrSubscriptExpr*>(currentExpr)) {
            if (currentLevel <= 0) {
                std::cerr << "Error: Too many subscripts for array " << baseName << std::endl;
                return nullptr;
            }

            // Get the size of the current dimension
            IrLiteral* dimLiteral = dims[currentLevel - 1];
            int dimSize = dynamic_cast<IrLiteralNumber*>(dimLiteral)->getValue();

            LlLocation* indexLocation = sub->getIndexExpr()->generateLlIr(builder, symbolTable);

            LlLocation* mulTemp = builder.generateTemp();
            LlLiteralInt* multiplierLiteral = new LlLiteralInt(cumulativeMultiplier);
            builder.appendStatement(new LlAssignStmtBinaryOp(mulTemp, indexLocation, "*", multiplierLiteral));

            if (!offsetTemp) {
                offsetTemp = mulTemp;
            } else {
                LlLocation* addTemp = builder.generateTemp();
                builder.appendStatement(new LlAssignStmtBinaryOp(addTemp, offsetTemp, "+", mulTemp));
                offsetTemp = addTemp;
            }
            cumulativeMultiplier *= dimSize;

            currentExpr = sub->getBaseExpr();
            currentLevel--;
        }
        return new LlLocationArray(new std::string(baseName), offsetTemp);
    }
};


class IrTypeIdent : public IrType {
    private:
        const std::string name;
    public:
        IrTypeIdent(const std::string& name, const TSNode& node) 
            : IrType(node), name(name) {}
        ~IrTypeIdent() = default;

        IrTypeIdent* clone() const override {
            return new IrTypeIdent(*this);
        }

        const std::string& getName() const {
            return name;
        }

        bool operator==(const Ir& that) const override{
            if (&that == this) {
                return true;
            }
            if (auto thatIdent = dynamic_cast<const IrTypeIdent*>(&that)) {
                return this->name == thatIdent->name;
            }
            return false;
        }

        int hashCode() const {
            std::hash<std::string> hasher;
            return hasher(this->name);
        }

        std::string prettyPrint(std::string indentSpace) const override{
            return indentSpace + "|--typeId: " + name + "\n";
        }

        std::string toString() const override{
            return name;
        }

        LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
            LlLocation* location = new LlLocationTypeAlias(&name);
            return location;
        }
};

class IrPointerType : public IrType {
private:
    IrType* baseType;

public:
    IrPointerType(IrType* baseType, const TSNode& node) 
        : IrType(node), baseType(baseType) {}

    ~IrPointerType() { delete baseType; }

    std::string toString() const override {
        return baseType->toString() + "*";
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string result = indentSpace + "|--pointer: *\n";
        result += baseType->prettyPrint(addIndent(indentSpace));
        return result;
    }

    IrType* getBaseType() const {
        return baseType;
    }

    IrPointerType* clone() const override {
        return new IrPointerType(*this);
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        return nullptr;
    }
};

// Comment: maybe refactor the IrType (add one layer for primitive types or ...)
class IrTypeStruct : public IrType {
private:
    IrIdent* name;                       
    IrFieldDeclList* fieldDeclList;      // List of field declarations

public:
    IrTypeStruct(IrIdent* name, IrFieldDeclList* fieldDeclList, const TSNode& node)
        : IrType(node), name(name), fieldDeclList(fieldDeclList) {}

    ~IrTypeStruct() {
        delete name;
        delete fieldDeclList;
    }

    IrTypeStruct* clone() const override {
        return new IrTypeStruct(*this);
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--type: struct\n";
        if (name) {
            prettyString += name->prettyPrint(addIndent(indentSpace));
        }
        prettyString += fieldDeclList->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        std::string str = "struct ";
        if (name) {
            str += name->toString();
        }
        return str + " {" + fieldDeclList->toString() + "}";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        // TODO: Implement this
        return nullptr;
    }
};

class IrTypeDef : public Ir {
private:
    IrType* type;        // The original type being aliased
    IrTypeIdent* alias;   // The alias name

public:
    IrTypeDef(IrType* type, IrTypeIdent* alias, const TSNode& node)
        : Ir(node), type(type), alias(alias) {}

    ~IrTypeDef() {
        delete type;
        delete alias;
    }

    IrType* getbaseType() const {
        return type;
    }

    IrTypeIdent* getName() const {
        return alias;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--typedef:\n";
        prettyString += type->prettyPrint(addIndent(indentSpace));
        prettyString += alias->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    std::string toString() const override{
        return "typedef " + type->toString() + " " + alias->toString();
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        IrTypeStruct* structType = dynamic_cast<IrTypeStruct*>(type);
        if (structType) {
            LlLocation *compo = alias->generateLlIr(builder, symbolTable);          
            LlLocationTypeAlias* location = dynamic_cast<LlLocationTypeAlias*>(compo);           
            symbolTable.putOnTypeDefTable(*location->getAliasTypeName(), structType);
        }
        return nullptr;     
    }
};


#endif