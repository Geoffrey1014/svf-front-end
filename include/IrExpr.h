#ifndef IR_EXPR_H
#define IR_EXPR_H
#include "Ir.h"

class IrExpr : public Ir {
public:
    IrExpr(const TSNode & node) : Ir(node) {}
    ~IrExpr() = default;
    std::string toString() {
        return "IrExpr";
    }

    bool operator==(const IrExpr& other) const {
        // Implementation of operator==
        return true;
    }

    int hashCode() const {
        // Implementation of hashCode
        return 0;
    }
};


class IrIdent : public IrExpr {
private:
    const std::string* name;

public:
    IrIdent(const std::string* name, const TSNode & node) : IrExpr(node), name(name) {}
    ~IrIdent() {
        delete name;
    }

    const std::string* getValue() const{
        return name;
    }

    bool operator==(const Ir & that) const {
        if (&that == this) {
            return true;
        }
        if (auto thatIdent = dynamic_cast<const IrIdent*>(&that)) {
            return *(this->getValue()) == *(thatIdent->getValue());
        }
        return false;
    }

    int hashCode() const {
        std::hash<std::string> hasher;
        return hasher(*this->name);
    }

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--id: " + *name + "\n";
    }
    std::string toString() {
        return "IrIdent: " + *name;
    }

};

class IrLoopExpr : public IrExpr {
private:
    Ir* body;

public:
    IrLoopExpr(Ir* body, const TSNode & node)
        : IrExpr(node), body(body) {}

    ~IrLoopExpr() {
        delete body;
    }

    Ir* getBody() const {
        return this->body;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--loop expression:\n";
        prettyString += this->body->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};

class IrWhileExpr : public IrExpr {
private:
    Ir* condition;
    Ir* body;

public:
    IrWhileExpr(Ir* condition, Ir* body, const TSNode & node)
        : IrExpr(node), condition(condition), body(body) {}

    ~IrWhileExpr() {
        delete condition;
        delete body;
    }

    Ir* getCondition() const {
        return this->condition;
    }

    Ir* getBody() const {
        return this->body;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--while expression:\n";
        prettyString += this->condition->prettyPrint("  " + indentSpace);
        prettyString += this->body->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};

class IrForExpr : public IrExpr {
private:
    IrIdent* ident;
    IrExpr* condition;
    Ir* body;

public:
    IrForExpr(IrIdent* ident, IrExpr* condition, Ir* body, const TSNode & node)
        : IrExpr(node), ident(ident), condition(condition), body(body) {}

    ~IrForExpr() {
        delete ident;
        delete condition;
        delete body;
    }

    IrIdent* getIdent() const {
        return this->ident;
    }

    IrExpr* getCondition() const {
        return this->condition;
    }

    Ir* getBody() const {
        return this->body;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--for expression:\n";
        prettyString += this->ident->prettyPrint("  " + indentSpace);
        prettyString += this->condition->prettyPrint("  " + indentSpace);
        prettyString += this->body->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};

class IrMatchArm : public IrExpr {
private:
    Ir* pattern;
    Ir* guard;
    Ir* body;

public:
    IrMatchArm(Ir* pattern, Ir* guard, Ir* body, const TSNode & node)
        : IrExpr(node), pattern(pattern), guard(guard), body(body) {}

    ~IrMatchArm() {
        delete pattern;
        delete guard;
        delete body;
    }

    Ir* getPattern() const {
        return this->pattern;
    }

    Ir* getGuard() const {
        return this->guard;
    }

    Ir* getBody() const {
        return this->body;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--match arm:\n";
        prettyString += this->pattern->prettyPrint("  " + indentSpace);
        if (this->guard) {
            prettyString += this->guard->prettyPrint("  " + indentSpace);
        }
        prettyString += this->body->prettyPrint("  " + indentSpace);
        return prettyString;
    }
};

class IrMatchExpr : public IrExpr {
private:
    Ir* matchExpr;
    std::vector<IrMatchArm*> arms;

public:
    IrMatchExpr(Ir* matchExpr, const std::vector<IrMatchArm*>& arms, const TSNode & node)
        : IrExpr(node), matchExpr(matchExpr), arms(arms) {}

    ~IrMatchExpr() {
        delete matchExpr;
        for (auto arm : arms) {
            delete arm;
        }
    }

    Ir* getMatchExpr() const {
        return this->matchExpr;
    }

    const std::vector<IrMatchArm*>& getArms() const {
        return this->arms;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--match expression:\n";
        prettyString += this->matchExpr->prettyPrint("  " + indentSpace);
        for (auto arm : arms) {
            prettyString += arm->prettyPrint("  " + indentSpace);
        }
        return prettyString;
    }
};

class IrIfExpr : public IrExpr {
private:
    Ir* condition;
    Ir* thenBlock;
    Ir* elseBlock;
public:
    IrIfExpr(Ir* condition, Ir* thenBlock, Ir* elseBlock, const TSNode & node)
        : IrExpr(node), condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}

    ~IrIfExpr() {
        delete condition;
        delete thenBlock;
        delete elseBlock;
    }

    Ir* getCondition() const {
        return this->condition;
    }

    Ir* getThenBlock() const {
        return this->thenBlock;
    }

    Ir* getElseBlock() const {
        return this->elseBlock;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--if expression:\n";
        prettyString += this->condition->prettyPrint("  " + indentSpace);
        prettyString += this->thenBlock->prettyPrint("  " + indentSpace);
        if (this->elseBlock) {
            prettyString += this->elseBlock->prettyPrint("  " + indentSpace);
        }
        return prettyString;
    }
};

class IrRangeExpr : public IrExpr {
private:
    Ir* start;
    Ir* end;
public:
    IrRangeExpr(Ir* start, Ir* end, const TSNode & node)
        : IrExpr(node), start(start), end(end) {}

    ~IrRangeExpr() {
        delete start;
        delete end;
    }

    Ir* getStart() const {
        return this->start;
    }

    Ir* getEnd() const {
        return this->end;
    }

    std::string prettyPrint(std::string indentSpace) const override {
        std::string prettyString = indentSpace + "|--range expression:\n";
        prettyString += "  " + indentSpace + "|--start:\n";
        prettyString += this->start->prettyPrint("    " + indentSpace);
        prettyString += "  " + indentSpace + "|--end:\n";
        prettyString += this->end->prettyPrint("    " + indentSpace);
        return prettyString;
    }
};

#endif