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

    // virtual size_t hashCode() const = 0;
    // virtual string semanticCheck(ScopeStack& scopeStack) = 0;

    virtual LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) = 0;
    virtual string prettyPrint(string indentSpace) const =0;
    virtual string toString() const = 0;
    
    string addIndent(const string& baseIndent, int level = 1) const {
        return baseIndent + string(level * 2, ' '); // 2 spaces per level
    }

};

class IrExpr : public virtual Ir {
public:
    IrExpr(const TSNode & node) : Ir(node) {}
    ~IrExpr() = default;
    
    string toString() const override {
        return "baseIrExpr";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        cerr << "IrExpr Error: generateLlIr not implemented for " << typeid(*this).name() << endl;
        return new LlLocationVar(new string("Error")); 
    }

    virtual const string getName() const {
        return "";
    }
};


class IrBinaryExpr : public IrExpr {
private:
    const string operation;
    IrExpr* leftOperand;
    IrExpr* rightOperand;

public:
    IrBinaryExpr(string& operation, IrExpr* leftOperand, IrExpr* rightOperand, const TSNode & node) 
        : Ir(node), IrExpr(node), operation(operation), 
          leftOperand(leftOperand), rightOperand(rightOperand) {}
    ~IrBinaryExpr() {
        delete leftOperand;
        delete rightOperand;
    }
    const IrExpr* getLeftOperand() const{
        return this->leftOperand;
    }

    const IrExpr* getRightOperand() const{
        return this->rightOperand;
    }

    const string& getOperation() const{
        return this->operation;
    }

    string toString() const override{
        return leftOperand->toString() + " " + operation + " " + rightOperand->toString();
    }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--binaryExpr\n";

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
    virtual IrType* clone() const = 0;
    int getWidth() const { return width; }
};

class IrLiteral : public IrExpr {
public:
    IrLiteral(const TSNode& node) : IrExpr(node), Ir(node) {}

    virtual ~IrLiteral() = default;

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        cerr << "IrLiteral Error: generateLlIr not implemented for " << typeid(*this).name() << endl;
        return new LlLocationVar(new string("Error")); 
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
        return dynamic_cast<const IrTypeBool*>(&that) != nullptr;
    }

    string toString() const override{
        return "bool";
    }

    string prettyPrint(string indentSpace) const override{
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
        return dynamic_cast<const IrTypeVoid*>(&that) != nullptr;
    }

    string toString() const override{
        return "void";
    }

    string prettyPrint(string indentSpace) const override{
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
        if (auto thatTypeInt = dynamic_cast<const IrTypeInt*>(&that)) {
            return this->width == thatTypeInt->width;
        }
        return false;
    }

    string toString() const override{
        return "int";
    }

    string prettyPrint(string indentSpace) const override{
        return indentSpace + "|--type: int\n";
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
        return dynamic_cast<const IrTypeChar*>(&that) != nullptr;
    }

    string prettyPrint(string indentSpace) const override{
        return indentSpace + "|--type: char\n";
    }

    string toString() const override{
        return "char";
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
        return dynamic_cast<const IrTypeString*>(&that) != nullptr;
    }

    string toString() const override{
        return "string";
    }

    string prettyPrint(string indentSpace) const override{
        return indentSpace + "|--type: string\n";
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
            for (size_t i = 0; i < this->dimension.size(); i++) {
                if (this->dimension[i] != thatTypeArray->dimension[i]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    string prettyPrint(string indentSpace) const override ;

    string toString() const override;

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

    bool operator==(const Ir& that) const override{
        if (auto thatLiteralBool = dynamic_cast<const IrLiteralBool*>(&that)) {
            return this->value == thatLiteralBool->value;
        }
        return false;
    }

    string prettyPrint(string indentSpace)  const override{
        string prettyPrint = indentSpace + "|--boolLiteral\n";
        prettyPrint += addIndent(indentSpace)+ "|--value: " + (this->value ? "true" : "false") + "\n";
        return prettyPrint;
    }

    string toString() const override{
        return std::string(1, value);
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

    bool operator==(const Ir& that) const override{
        if (auto thatLiteralChar = dynamic_cast<const IrLiteralChar*>(&that)) {
            return this->value == thatLiteralChar->value;
        }
        return false;
    }

    string prettyPrint(string indentSpace)  const override{
        string prettyPrint = indentSpace + "|--charLiteral\n";
        prettyPrint += addIndent(indentSpace) + "|--value: '" + this->value + "'\n";
        return prettyPrint;
    }

    string toString() const override{
        return "'" + std::string(1, value) + "'";
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

    bool operator==(const Ir& that) const override{
        if (auto thatLiteralNumber = dynamic_cast<const IrLiteralNumber*>(&that)) {
            return this->value == thatLiteralNumber->value;
        }
        return false;
    }

    string prettyPrint(string indentSpace) const override {
        string prettyPrint = indentSpace + "|--NumberLiteral\n";
        prettyPrint += addIndent(indentSpace) + "|--value: " + to_string(this->value) + "\n";
        return prettyPrint;
    }

    string toString() const override{
        return to_string(value); // "IrLiteralNumber";
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
    string value;

public:
    IrLiteralStringContent(const string& value, const TSNode& node)
        : IrLiteral(node), Ir(node), value(value) {}

        ~IrLiteralStringContent() override = default;

    const string& getValue() const {
        return value;
    }

    bool operator==(const Ir& that) const override{
        if (auto thatLiteralStringContent = dynamic_cast<const IrLiteralStringContent*>(&that)) {
            return this->value == thatLiteralStringContent->value;
        }
        return false;
    }

    string prettyPrint(string indentSpace) const override {
        return indentSpace + "|--stringContent: " + value + "\n";
    }

    string toString() const override{
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

    const string& getValue() const {
        return stringContent->getValue();
    }

    bool operator==(const Ir& that) const override{
        if (auto thatLiteralString = dynamic_cast<const IrLiteralString*>(&that)) {
            return *this->stringContent == *thatLiteralString->stringContent;
        }
        return false;
    }

    string prettyPrint(string indentSpace) const override {
        string prettyPrint = indentSpace + "|--StringLiteral\n";
        prettyPrint += stringContent->prettyPrint(addIndent(indentSpace));
        return prettyPrint;
    }

    string toString() const override{
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
    deque<IrExpr*> argsList;
public:
    IrArgList(const TSNode& node) : Ir(node) {}
    ~IrArgList() {
        for (IrExpr* arg: this->argsList) {
            delete arg;
        }
    }   

    deque<IrExpr*> getArgsList() {
        return this->argsList;
    }

    void addToArgsList(IrExpr* newArg) {
        this->argsList.push_front(newArg);
    }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--argList:\n";

        for (IrExpr* arg: this->argsList) {
            prettyString += arg->prettyPrint(addIndent(indentSpace)); 
        }

        return prettyString;
    }

    string toString() const override{
        string argsString = "";
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--preprocInclude\n";
        prettyString += path->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override{
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
    virtual const string getName() const {
        static const string emptyString = "";
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

    const string getName() const override {
        if (baseDeclarator) {
            return baseDeclarator->getName();
        }
        return "";
    }

    string prettyPrint(string indentSpace) const override {
        string str = indentSpace + "|--abstract_pointer_declarator: *\n";
        if(baseDeclarator){
            str += baseDeclarator->prettyPrint(addIndent(indentSpace));
        }
        return str;
    }

    string toString() const override{
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
    const string name;
    bool isTypeAlias;

public:
    IrIdent(const string& name, const TSNode & node, bool isTypeAlias = false) : Ir(node), IrDeclDeclarator(node), IrExpr(node), name(name) {}
    ~IrIdent() = default;

    const string & getValue() const {
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

    bool isType() const { return isTypeAlias; }
    void markAsTypeAlias() { isTypeAlias = true; }

    string prettyPrint(string indentSpace) const override{
        return indentSpace + "|--id: " + name + "\n";
    }

    const string getName() const override {
        return name;
    }

    string toString() const override{
        return name;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        return new LlLocationVar(new string(name));
    }

};

class IrPreprocArg : public Ir {
public:
    string text;

    IrPreprocArg(const string& t, const TSNode& node)
        : Ir(node), text(t) {}
    
    ~IrPreprocArg() = default;

    string prettyPrint(string indentSpace) const override {
        return indentSpace + "|-- preproc_arg: " + text + "\n";
    }

    string toString() const override {
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

    string prettyPrint(string indentSpace) const override {
        string result = indentSpace + "|-- preproc_def: " + name->getValue();
        if (value) result += " " + value->text;
        result += "\n";
        return result;
    }

    string toString() const override {
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--callExpr\n";

        prettyString += addIndent(indentSpace) + "|--functionName\n";
        prettyString += this->functionName->prettyPrint(addIndent(indentSpace, 2));
        prettyString += this->argList->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }

    string toString() const override{
        return functionName->toString() + " (" + argList->toString() + ")";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        vector<LlComponent*> argsList;
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
    string op;
public:
    IrAssignExpr(IrExpr* lhs, IrExpr* rhs, const string& op, const TSNode& node)
        : Ir(node), IrNonBinaryExpr(node), lhs(lhs), rhs(rhs), op(op) {}

    ~IrAssignExpr() {
        delete lhs;
        delete rhs;
    }

    IrExpr* getLhs() const { return lhs; }
    IrExpr* getRhs() const { return rhs; }
    const string& getOp() const { return op; }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--assignExpr\n";
        prettyString += addIndent(indentSpace) + "|--lhs\n";
        prettyString += lhs->prettyPrint(addIndent(indentSpace, 2));
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += addIndent(indentSpace) + "|--rhs\n";
        prettyString += rhs->prettyPrint(addIndent(indentSpace, 2));
        return prettyString;
    }

    string toString() const override {
        return lhs->toString() + " " + op + " " + rhs->toString();
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* left = lhs->generateLlIr(builder, symbolTable);
        LlLocation* right = rhs->generateLlIr(builder, symbolTable);
        // If RHS is a pointer dereference, load into a temp first
        if (dynamic_cast<LlLocationDeref*>(right)) {
            LlLocation* temp = builder.generateTemp();
            LlAssignStmtRegular* derefLoad = new LlAssignStmtRegular(temp, right);
            builder.appendStatement(derefLoad);
            right = temp;
        }
        // If LHS is a pointer dereference, assign *t0 = value directly
        if (dynamic_cast<LlLocationDeref*>(left)) {
            LlAssignStmtDeref* storeStmt = new LlAssignStmtDeref(left, right);
            builder.appendStatement(storeStmt);
            return nullptr;
        }

        LlLocation* location = dynamic_cast<LlLocation*>(left);        
        string operation = op;
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

    string prettyPrint(string indentSpace) const override {
        string op = isArrow ? "->" : ".";
        string prettyString = indentSpace + "|--field_expression\n";
        prettyString += baseExpr->prettyPrint(addIndent(indentSpace));
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += fieldName->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override{
        string op = isArrow ? "->" : ".";
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

    string prettyPrint(string indentSpace) const override {
        string op = isAddressOf ? "&" : "*";
        string prettyString = indentSpace + "|--pointer_expression\n";

        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        if (argument) {
            prettyString += argument->prettyPrint(addIndent(indentSpace));
        }
        
        return prettyString;
    }

    string toString() const override{
        string op = isAddressOf ? "&" : "*";
        return op + argument->toString();
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* arg = argument->generateLlIr(builder, symbolTable);
        LlLocation* returnLocation = builder.generateTemp();
        if (isAddressOf) {
            LlAssignStmtAddr* load = new LlAssignStmtAddr(returnLocation, arg);
            builder.appendStatement(load);
        }
        else if (isDereference) {
            // Store pointer address in temp before dereferencing
            LlAssignStmtRegular* assignStmtRegular = new LlAssignStmtRegular(returnLocation, arg);
            builder.appendStatement(assignStmtRegular);

            return new LlLocationDeref(returnLocation);
        }
        return returnLocation;
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--parenthesizedExpr\n";
        if (innerExpr) {
            prettyString += innerExpr->prettyPrint(addIndent(indentSpace));
        }
        return prettyString;
    }

    string toString() const override {
        return "(" + innerExpr->toString() + ")";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        return innerExpr->generateLlIr(builder, symbolTable);
    }
};

class IrUnaryExpr : public IrNonBinaryExpr {
private:
    string op;  // Operator (e.g., '-', '!')
    IrExpr* argument;

public:
    IrUnaryExpr(const string& op, IrExpr* arg, const TSNode &node) 
        : Ir(node), IrNonBinaryExpr(node), op(op), argument(arg) {}

    ~IrUnaryExpr() {
        delete argument;
    }

    IrExpr* getArgument() const { return argument; }
    const string& getOperator() const { return op; }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--unaryExpr\n";
        prettyString += addIndent(indentSpace) + "|--op: " + op + "\n";
        prettyString += argument->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override {
        return op + argument->toString();
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        LlLocation* arg = argument->generateLlIr(builder, symbolTable);
        LlLocation* returnLocation = builder.generateTemp();
        LlAssignStmtUnaryOp* unaryOp = new LlAssignStmtUnaryOp(returnLocation, arg, new string(op));
        builder.appendStatement(unaryOp);
        return returnLocation;
    }
};


class IrStatement : public Ir {
public:
    IrStatement(const TSNode& node) : Ir(node) {}
    virtual ~IrStatement() = default;
    string toString() const override{
        return "IrStatement";
    }
    
    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        cerr << "Error: generateLlIr not implemented for " << typeid(*this).name() << endl;
        return new LlLocationVar(new string("")); // Return empty location
    }
};

class IrStmtReturn : public IrStatement {
public:
    IrStmtReturn(const TSNode& node) : IrStatement(node) {}
    virtual ~IrStmtReturn() = default;
    
    virtual LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        cerr << "Error: generateLlIr not implemented for " << typeid(*this).name() << endl;
        return new LlLocationVar(new string("")); // Return empty location
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

    string prettyPrint(string indentSpace)const override {
        string prettyString = indentSpace + "|--returnExpr\n";

        prettyString += this->result->prettyPrint(addIndent(indentSpace));

        return prettyString;
    }
    string toString() const override{
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

    string prettyPrint(string indentSpace)const override  {
        string prettyString = indentSpace + "|--returnVoid\n";
        return prettyString;
    }
    string toString() const override{
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
    deque<IrStatement*> stmtsList;
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

    string prettyPrint(string indentSpace)const override  {
        string prettyString = indentSpace + "|--compoundStmt:\n";

        for (IrStatement* statement: this->stmtsList) {
            prettyString += statement->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }
    string toString() const override{
        string s = "";
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

    string prettyPrint(string indentSpace)const override  {
        string prettyString = indentSpace + "|--exprStmt\n";
        prettyString += this->expr->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override{
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--elseClause\n";
        prettyString += alternative->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override {
        string result = "else " + alternative->toString();
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--ifStmt\n";

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

    string toString() const override {
        string result = "if " + condition->toString() + " " + thenBody->toString();
        if (elseBody) {
            result += " " + elseBody->toString();
        }
        return result;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        LlLocation* conditionVar = this->condition->generateLlIr(builder, symbolTable);

        string label = builder.generateLabel();

        
        string* endLabel = new string();
        endLabel->append("if.end.");
        endLabel->append(label);

        string* elseLabel = new string();
        elseLabel->append("if.else.");
        elseLabel->append(label);

        if (elseBody) {
            LlJumpConditional *jumpUnconditionalToElse = new LlJumpConditional(elseLabel, conditionVar);
            builder.appendStatement(jumpUnconditionalToElse);

        }


        thenBody->generateLlIr(builder, symbolTable);
        builder.appendStatement(new LlJumpUnconditional(endLabel));

        if (elseBody) {
            LlEmptyStmt* emptyStmtElse = new LlEmptyStmt();
            builder.appendStatement(*elseLabel, emptyStmtElse);
            elseBody->generateLlIr(builder, symbolTable);
        }

        // append end if label
        LlEmptyStmt* endIfEmptyStmt = new LlEmptyStmt();
        builder.appendStatement(*endLabel, endIfEmptyStmt);
        return nullptr;
    }
};


class IrStorageClassSpecifier : public Ir {
private:
    string specifier;

public:
    IrStorageClassSpecifier(const string& specifier, const TSNode& node)
        : Ir(node), specifier(specifier) {}

    const string& getValue() const {
        return this->specifier;
    }

    string prettyPrint(string indentSpace) const override {
        return indentSpace + "|--storageClassSpecifier: " + this->specifier + "\n";
    }

    string toString() const override{
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

    string prettyPrint(string indentSpace) const override {
        string str = indentSpace + "|--field_declaration:\n";
        str += type->prettyPrint(addIndent(indentSpace));

        if (declarator) {
            str += declarator->prettyPrint(addIndent(indentSpace));
        }
        // if (bitfieldSize != -1) {
        //     str += indentSpace + "  |--bitfield_size: " + to_string(bitfieldSize) + "\n";
        // }
        return str;
    }

    string toString() const override{
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
    deque<IrFieldDecl*> fieldDeclarations; // List of field declarations

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

    string prettyPrint(string indentSpace) const override {
        if (fieldDeclarations.empty()) {
            return "";
        }
        
        string str = indentSpace + "|--field_declaration_list:\n";
        for (auto* fieldDecl : fieldDeclarations) {
            str += fieldDecl->prettyPrint(addIndent(indentSpace));
        }
        return str;
    }

    string toString() const override{
        string str = "";
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

    string toString() const override{
        if (declarator) {
            return paramType->toString() + " " + declarator->toString();
        }
        return paramType->toString();
    }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--param:\n";

        prettyString += paramType->prettyPrint(addIndent(indentSpace));

        if (declarator) {
            prettyString += declarator->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        symbolTable.putOnVarTable(declarator->getName(), paramType);
        return nullptr;
    }
};


class IrParamList : public Ir {
private:
    deque<IrParamDecl*> paramsList;

public:
    IrParamList(const TSNode& node) : Ir(node) {}
    ~IrParamList() {
        for (IrParamDecl* param: this->paramsList) {
            delete param;
        }
    }

    deque<IrParamDecl*> getParamsList() {
        return this->paramsList;
    }

    void addToParamsList(IrParamDecl* newParam) {
        this->paramsList.push_front(newParam);
    }

    string toString() const override{
        string paramsString = "";
        for (IrParamDecl* paramDecl: this->paramsList) {
            paramsString += paramDecl->toString() + ", ";
        }
        return paramsString;
    }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--paramList:\n";

        for (IrParamDecl* paramDecl: this->paramsList) {
            prettyString += paramDecl->prettyPrint(addIndent(indentSpace));
        }

        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override{
        for (IrParamDecl* param: this->paramsList) {
            param->generateLlIr(builder, symbolTable);
        }
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

    const string getName() const override {
        if (declarator) return declarator->getName();
        return "";
    }

    string toString() const override{
        return getName() + " (" + paramsList->toString() + ")";
    }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--function_declarator\n";
        prettyString += declarator->prettyPrint(addIndent(indentSpace));
        prettyString += paramsList->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        paramsList->generateLlIr(builder, symbolTable);
        return new LlLocationVar(new string(getName()));
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
        string name = functionDecl->getName();
        LlEmptyStmt* emptyStmt = new LlEmptyStmt();
        builder.appendStatement(name, emptyStmt);
        functionDecl->generateLlIr(builder, symbolTable);
        this->compoundStmt->generateLlIr(builder, symbolTable);
        return nullptr;
    }

    string toString() const override{
        string result = returnType->toString() + " ";
        result += functionDecl->toString();
        result += " {\n";
        if (compoundStmt) {
            result += compoundStmt->toString();  // Include the body
        }
        result += "\n}";
        return result;
    }
    string getFunctionName() { return functionDecl->toString();}

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--function_definition\n";
        prettyString += returnType->prettyPrint(addIndent(indentSpace));
        prettyString += functionDecl->prettyPrint(addIndent(indentSpace));
        prettyString += compoundStmt->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }
};

class IrInitializerList : public IrExpr {
private:
    deque<IrExpr*> elements;

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

    deque<IrExpr*> getElements() const {
        return elements;
    }

    string prettyPrint(string indentSpace) const override {
        string prettyPrint = indentSpace + "|--initializer_list\n";
        for (IrExpr* expr : elements) {
            prettyPrint += expr->prettyPrint(addIndent(indentSpace));
        }
        return prettyPrint;
    }

    string toString() const override {
        string str = "{ ";
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--init_declarator:\n";
        prettyString += declarator->prettyPrint(addIndent(indentSpace));
        prettyString += initializer->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override{
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

    string getName() const {
        if (initDecl) {
            return initDecl->getDeclarator()->getName();
        } else if (simpleDecl) {
            return simpleDecl->getName();
        }
        return "";
    }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--declaration:\n";

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

    string toString() const override {
        string str;
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
    deque<IrDecl*> decls;

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
        decls.push_front(decl);
    }

    // Ownership-transfer method
    deque<IrDecl*> releaseDeclarations() {
        // Move the entire 'decls' out to a temporary. 
        // 'decls' will become empty.
        deque<IrDecl*> temp = std::move(decls); 
        // now 'decls' is empty, so ~IrMultiDecl won't delete these pointers
        return temp;
    }

    string prettyPrint(string indentSpace) const override {
        string result = indentSpace + "|--multiDecl:\n";
        for (auto* d : decls) {
            result += d->prettyPrint(addIndent(indentSpace));
        }
        return result;
    }

    string toString() const override {
        string out;
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--forStmt\n";

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

    string toString() const override {
        string result = "for (";

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
        // each for-loop gets its own unique label--forLabel
        string forLabel = builder.generateLabel();
        string* condLabel = new string("for.cond." + forLabel);
        string* bodyLabel = new string("for.body." + forLabel);
        string* incLabel = new string("for.inc." + forLabel); // increment e.g. i +=1
        string* endLabel = new string("for.end." + forLabel);

        // Condition Check Block
        LlEmptyStmt* emptyStmtFor = new LlEmptyStmt();
        builder.appendStatement(*condLabel, emptyStmtFor);

        LlLocation* conditionVar = this->condition->generateLlIr(builder, symbolTable);
        LlJumpConditional* conditionalJump = new LlJumpConditional(endLabel,conditionVar);        
        builder.appendStatement(conditionalJump);

        // Loop Body Block
        LlEmptyStmt* emptyStmtForBody = new LlEmptyStmt();
        builder.appendStatement(*bodyLabel, emptyStmtForBody);
        if (body) {
            body->generateLlIr(builder, symbolTable);
        }

        // Update Block
        LlEmptyStmt* emptyStmtForInc = new LlEmptyStmt();
        builder.appendStatement(*incLabel, emptyStmtForInc);
        update->generateLlIr(builder, symbolTable);
        
        // Jump back to condition
        LlJumpUnconditional* jumpToFor = new LlJumpUnconditional(condLabel);
        builder.appendStatement(jumpToFor);

        // End Block
        LlEmptyStmt* emptyStmtForEnd = new LlEmptyStmt();
        builder.appendStatement(*endLabel, emptyStmtForEnd);

        return nullptr;
    }
};

class IrWhileStmt : public IrStatement {
    private:
        IrParenthesizedExpr* condition;
        IrStatement* body;
    
    public:
        IrWhileStmt(IrParenthesizedExpr* condition, IrStatement* body, const TSNode& node)
            : IrStatement(node), condition(condition), body(body) {}
    
        ~IrWhileStmt() {
            delete condition;
            delete body;
        }
    
        string prettyPrint(string indentSpace) const override {
            string prettyString = indentSpace + "|--whileStmt\n";
    
            prettyString += addIndent(indentSpace) + "|--condition\n";
            prettyString += condition->prettyPrint(addIndent(indentSpace, 2));
    
            prettyString += addIndent(indentSpace) + "|--body\n";
            prettyString += body->prettyPrint(addIndent(indentSpace, 2));
    
            return prettyString;
        }
    
        string toString() const override {
            return "while " + condition->toString() + " {" + body->toString(); + "}";
        }
    
        LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
            // Generate IR for while loop
            string loopLabel = builder.generateLabel();
            string* condLabel = new string("while.cond." + loopLabel);
            string* bodyLabel = new string("while.body." + loopLabel);
            string* endLabel = new string("while.end." + loopLabel);
    
            // Condition Block
            LlEmptyStmt* emptyStmtWhile = new LlEmptyStmt();
            builder.appendStatement(*condLabel, emptyStmtWhile);
    
            LlLocation* conditionVar = this->condition->generateLlIr(builder, symbolTable);
            LlJumpConditional* conditionalJump = new LlJumpConditional(endLabel, conditionVar);
            builder.appendStatement(conditionalJump);
    
            // Body Block
            LlEmptyStmt* emptyStmtWhileBody = new LlEmptyStmt();
            builder.appendStatement(*bodyLabel, emptyStmtWhileBody);
            if (body) {
                body->generateLlIr(builder, symbolTable);
            }
    
            // Jump back to condition
            LlJumpUnconditional* jumpToWhile = new LlJumpUnconditional(condLabel);
            builder.appendStatement(jumpToWhile);
    
            // End Block
            LlEmptyStmt* emptyStmtWhileEnd = new LlEmptyStmt();
            builder.appendStatement(*endLabel, emptyStmtWhileEnd);
    
            return nullptr;
}
};
    

class IrBreakStmt : public IrStatement {
public:
    IrBreakStmt(const TSNode& node) : IrStatement(node) {}

    string prettyPrint(string indentSpace) const override {
        return indentSpace + "|--breakStmt\n";
    }

    string toString() const override {
        return "break;";
    }

    // LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
    //     string* breakLabel = new string("break.");  
    //     LlJumpUnconditional* breakJump = new LlJumpUnconditional(breakLabel);
    //     builder.appendStatement(breakJump);
    //     return nullptr;
    // }
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

    const string getName() const override {
        return baseExpr->getName();
    }

    IrExpr* getBaseExpr() const {
        return baseExpr;
    }

    IrExpr* getIndexExpr() const {
        return indexExpr;
    }

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--subscript_expression\n";

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

    string toString() const override{
        return baseExpr->toString() + "[" + indexExpr->toString() + "]";
    }

    LlLocation* generateLlIr(LlBuilder& builder, SymbolTable& symbolTable) override {
        string baseName = baseExpr->getName();
        IrType* type = symbolTable.getFromVarTable(baseName);
        IrTypeArray* arrayType = dynamic_cast<IrTypeArray*>(type);

        if (!arrayType) {
            cerr << "Error: " << baseName << " is not an array." << endl;
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
                cerr << "Error: Too many subscripts for array " << baseName << endl;
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
        return new LlLocationArray(new string(baseName), offsetTemp);
    }
};


class IrTypeIdent : public IrType {
    private:
        string name;
    public:
        IrTypeIdent(string& name, const TSNode& node)
            : IrType(node), name(name) {}
        ~IrTypeIdent() = default;

        IrTypeIdent* clone() const override {
            return new IrTypeIdent(*this);
        }

        const string& getName() const {
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

        string prettyPrint(string indentSpace) const override{
            return indentSpace + "|--typeId: " + name + "\n";
        }

        string toString() const override{
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

    string toString() const override {
        return baseType->toString() + "*";
    }

    string prettyPrint(string indentSpace) const override {
        string result = indentSpace + "|--pointer: *\n";
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--type: struct\n";
        if (name) {
            prettyString += name->prettyPrint(addIndent(indentSpace));
        }
        prettyString += fieldDeclList->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override{
        string str = "struct ";
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

    string prettyPrint(string indentSpace) const override {
        string prettyString = indentSpace + "|--typedef:\n";
        prettyString += type->prettyPrint(addIndent(indentSpace));
        prettyString += alias->prettyPrint(addIndent(indentSpace));
        return prettyString;
    }

    string toString() const override{
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