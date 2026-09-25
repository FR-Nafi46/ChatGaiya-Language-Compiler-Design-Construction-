#include "compiler.hpp"

Expr* makeExpr(ExprKind kind, const std::string& text, Expr* left, Expr* right) {
    return new Expr(kind, text, left, right);
}

Stmt* makeStmt(StmtKind kind) { return new Stmt(kind); }

ValueType typeFromName(const std::string& name) {
    if (name == "ongko") return ValueType::INT;
    if (name == "dhoshomik") return ValueType::FLOAT;
    if (name == "kotha") return ValueType::STRING;
    if (name == "ho") return ValueType::BOOL;
    return ValueType::UNKNOWN;
}

std::string typeName(ValueType type) {
    switch (type) {
        case ValueType::INT: return "ongko";
        case ValueType::FLOAT: return "dhoshomik";
        case ValueType::STRING: return "kotha";
        case ValueType::BOOL: return "ho";
        default: return "unknown";
    }
}
