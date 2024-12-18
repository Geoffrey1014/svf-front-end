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

class IrMatchExpr : public IrExpr {
private:
    IrIdent* lhs;
    IrExpr* rhs;
public:
    IrMatchExpr(IrIdent* lhs, IrExpr* rhs, const TSNode & node) : IrExpr(node), lhs(lhs), rhs(rhs) {}
    ~IrMatchExpr() {
        delete lhs;
        delete rhs;
    }

    IrIdent* getLhs() {
        return lhs;
    }

    IrExpr* getRhs() {
        return rhs;
    }

    bool operator==(const Ir & that) const {
        if (&that == this) {
            return true;
        }
        if (auto thatMatchExpr = dynamic_cast<const IrMatchExpr*>(&that)) {
            return *(this->getLhs()) == *(thatMatchExpr->getLhs()) && *(this->getRhs()) == *(thatMatchExpr->getRhs());
        }
        return false;
    }

    int hashCode() const {
        return 17; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        std::string prettyString = indentSpace + "|--matchExpr\n";

        // pretty print the lhs
        prettyString += this->lhs->prettyPrint("  " + indentSpace);

        // pretty print the rhs
        prettyString += this->rhs->prettyPrint("  " + indentSpace);

        return prettyString;
    }
    std::string toString() {
        return "IrMatchExpr";
    }
};

class IrLoopExpr : public IrExpr {
private:
    IrIdent* lhs;
    IrExpr* rhs;
public:
    IrLoopExpr(IrIdent* lhs, IrExpr* rhs, const TSNode & node) : IrExpr(node), lhs(lhs), rhs(rhs) {}
    ~IrLoopExpr() {
        delete lhs;
        delete rhs;
    }

    IrIdent* getLhs() {
        return lhs;
    }

    IrExpr* getRhs() {
        return rhs;
    }

    bool operator==(const Ir & that) const {
        if (&that == this) {
            return true;
        }
        if (auto thatLoopExpr = dynamic_cast<const IrLoopExpr*>(&that)) {
            return *(this->getLhs()) == *(thatLoopExpr->getLhs()) && *(this->getRhs()) == *(thatLoopExpr->getRhs());
        }
        return false;
    }

    int hashCode() const {
        return 19; // some arbitrary prime
    }

    std::string prettyPrint(std::string indentSpace) const {
        std::string prettyString = indentSpace + "|--loopExpr\n";

        // pretty print the lhs
        prettyString += this->lhs->prettyPrint("  " + indentSpace);

        // pretty print the rhs
        prettyString += this->rhs->prettyPrint("  " + indentSpace);

        return prettyString;
    }
    std::string toString() {
        return "IrLoopExpr";
    }
};

#endif