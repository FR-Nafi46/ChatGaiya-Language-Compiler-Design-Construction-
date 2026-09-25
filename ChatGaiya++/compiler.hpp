#pragma once
#include <string>
#include <vector>

enum class ValueType { UNKNOWN, INT, FLOAT, STRING, BOOL };
enum class ExprKind { NUMBER, STRING_LITERAL, BOOLEAN, VARIABLE, INPUT, TYPE_NAME, UNARY, BINARY, INDEX };
enum class StmtKind { DECLARATION, ASSIGNMENT, INCREMENT, PRINT, IF, WHILE, BREAK, CONTINUE, BLOCK, INCLUDE };

struct Expr {
    ExprKind kind;
    std::string text;
    Expr* left = nullptr;
    Expr* right = nullptr;
    ValueType type = ValueType::UNKNOWN;
    int line = 1;
    int column = 1;
    Expr(ExprKind k, std::string value = {}, Expr* a = nullptr, Expr* b = nullptr)
        : kind(k), text(std::move(value)), left(a), right(b) {}
};

struct Stmt {
    StmtKind kind;
    std::string name;
    ValueType declaredType = ValueType::UNKNOWN;
    Expr* expr = nullptr;
    std::vector<Stmt*> body;
    std::vector<Stmt*> elseBody;
    std::string op;
    int line = 1;
    int column = 1;
    Stmt(StmtKind k) : kind(k) {}
};

Expr* makeExpr(ExprKind kind, const std::string& text = {}, Expr* left = nullptr, Expr* right = nullptr);
Stmt* makeStmt(StmtKind kind);
ValueType typeFromName(const std::string& name);
std::string typeName(ValueType type);
