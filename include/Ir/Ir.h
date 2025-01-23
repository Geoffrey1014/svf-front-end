#ifndef IR_H
#define IR_H
#include <string>
#include <tree_sitter/api.h> 
#include <vector>
#include "Ll.h"
#include "LlLocationStruct.h"
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
    virtual LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable){
        std::cerr << "Ir Error: generateLlIr not implemented for " << typeid(*this).name() << std::endl;
        return new LlLocationVar(new std::string("Error")); 
    };
    
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
        return true;
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

#endif