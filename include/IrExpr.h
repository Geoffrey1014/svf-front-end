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
    const std::string name;

public:
    IrIdent(const std::string& name, const TSNode & node) : IrExpr(node), name(name) {}
    ~IrIdent() = default;

    // Getter for name
    const std::string &getValue() const {
        return name;
    }

    bool operator==(const Ir & that) const {
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

    std::string prettyPrint(std::string indentSpace) const {
        return indentSpace + "|--id: " + name + "\n";
    }
    std::string toString() {
        return "IrIdent: " + name;
    }

};

#endif