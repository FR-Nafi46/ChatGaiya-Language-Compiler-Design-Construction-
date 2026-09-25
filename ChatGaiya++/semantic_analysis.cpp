#include "semantic_analysis.hpp"
#include "error_reporting.hpp"
#include <unordered_set>

namespace {
bool numeric(ValueType t) { return t == ValueType::INT || t == ValueType::FLOAT; }
bool pythonReserved(const std::string& name) {
    static const std::unordered_set<std::string> words = {
        "False", "None", "True", "and", "as", "assert", "async", "await", "break", "class",
        "continue", "def", "del", "elif", "else", "except", "finally", "for", "from", "global",
        "if", "import", "in", "is", "lambda", "nonlocal", "not", "or", "pass", "raise", "return",
        "try", "while", "with", "yield", "input", "print", "str", "int", "float", "_cg_charat"
    };
    return words.count(name) != 0;
}
}

bool SemanticAnalyzer::assignable(ValueType target, ValueType source) const {
    return target == ValueType::UNKNOWN || source == ValueType::UNKNOWN || target == source ||
           (target == ValueType::FLOAT && source == ValueType::INT);
}

const SymbolTable& SemanticAnalyzer::symbols() const { return symbols_; }

bool SemanticAnalyzer::analyze(const std::vector<Stmt*>& program) {
    symbols_ = SymbolTable();
    loopDepth_ = 0;
    analyzeStatements(program);
    return !g_errors.hasErrors();
}

ValueType SemanticAnalyzer::analyzeExpr(Expr* e) {
    if (!e) return ValueType::UNKNOWN;
    switch (e->kind) {
        case ExprKind::NUMBER:
            e->type = e->text.find('.') == std::string::npos ? ValueType::INT : ValueType::FLOAT;
            return e->type;
        case ExprKind::STRING_LITERAL: e->type = ValueType::STRING; return e->type;
        case ExprKind::BOOLEAN: e->type = ValueType::BOOL; return e->type;
        case ExprKind::INPUT: e->type = ValueType::STRING; return e->type;
        case ExprKind::VARIABLE:
            if (!symbols_.contains(e->text)) {
                g_errors.add(e->line, e->column, "use of undeclared variable '" + e->text + "'");
                e->type = ValueType::UNKNOWN;
            } else e->type = symbols_.lookup(e->text);
            return e->type;
        case ExprKind::TYPE_NAME: {
            ValueType inner = analyzeExpr(e->left);
            e->text = typeName(inner);
            e->type = ValueType::STRING;
            return e->type;
        }
        case ExprKind::INDEX: {
            ValueType container = analyzeExpr(e->left);
            ValueType index = analyzeExpr(e->right);
            if (container != ValueType::STRING && container != ValueType::UNKNOWN)
                g_errors.add(e->line, e->column, "indexing is supported only on kotha (string) values");
            if (index != ValueType::INT && index != ValueType::UNKNOWN)
                g_errors.add(e->line, e->column, "string index must be ongko (int)");
            e->type = ValueType::STRING;
            return e->type;
        }
        case ExprKind::UNARY: {
            ValueType a = analyzeExpr(e->left);
            if (e->text == "!") {
                if (a != ValueType::BOOL && a != ValueType::UNKNOWN)
                    g_errors.add(e->line, e->column, "! requires a boolean operand");
                e->type = ValueType::BOOL;
            } else {
                if (!numeric(a) && a != ValueType::UNKNOWN)
                    g_errors.add(e->line, e->column, "unary sign requires a number");
                e->type = a;
            }
            return e->type;
        }
        case ExprKind::BINARY: {
            ValueType a = analyzeExpr(e->left), b = analyzeExpr(e->right);
            const std::string& op = e->text;
            if (op == "&&" || op == "||") {
                if ((a != ValueType::BOOL && a != ValueType::UNKNOWN) || (b != ValueType::BOOL && b != ValueType::UNKNOWN))
                    g_errors.add(e->line, e->column, op + " requires boolean operands");
                e->type = ValueType::BOOL;
            } else if (op == "==" || op == "!=") {
                if (a != b && !(numeric(a) && numeric(b)) && a != ValueType::UNKNOWN && b != ValueType::UNKNOWN)
                    g_errors.add(e->line, e->column, "incompatible types in equality comparison");
                e->type = ValueType::BOOL;
            } else if (op == "<" || op == ">" || op == "<=" || op == ">=") {
                if ((!numeric(a) || !numeric(b)) && a != ValueType::UNKNOWN && b != ValueType::UNKNOWN)
                    g_errors.add(e->line, e->column, "relational operators require numeric operands");
                e->type = ValueType::BOOL;
            } else if (op == "&" || op == "|" || op == "^" || op == "<<" || op == ">>") {
                if ((a != ValueType::INT || b != ValueType::INT) && a != ValueType::UNKNOWN && b != ValueType::UNKNOWN)
                    g_errors.add(e->line, e->column, "bitwise and shift operators require ongko (int) operands");
                e->type = ValueType::INT;
            } else if (op == "+") {
                const bool bothNumbers = numeric(a) && numeric(b);
                const bool bothStrings = a == ValueType::STRING && b == ValueType::STRING;
                const bool mixedStrings = (a == ValueType::STRING) != (b == ValueType::STRING) &&
                    a != ValueType::UNKNOWN && b != ValueType::UNKNOWN;
                if (!bothNumbers && !bothStrings && !mixedStrings && a != ValueType::UNKNOWN && b != ValueType::UNKNOWN)
                    g_errors.add(e->line, e->column, "'+' requires numbers or at least one string operand");
                e->type = bothNumbers ? ((a == ValueType::FLOAT || b == ValueType::FLOAT) ? ValueType::FLOAT : ValueType::INT) :
                          ((bothStrings || mixedStrings) ? ValueType::STRING : ValueType::UNKNOWN);
            } else {
                if ((!numeric(a) || !numeric(b)) && a != ValueType::UNKNOWN && b != ValueType::UNKNOWN)
                    g_errors.add(e->line, e->column, "arithmetic operators require numeric operands");
                e->type = (a == ValueType::FLOAT || b == ValueType::FLOAT || op == "/") ? ValueType::FLOAT : ValueType::INT;
            }
            return e->type;
        }
    }
    return ValueType::UNKNOWN;
}

void SemanticAnalyzer::analyzeStatements(const std::vector<Stmt*>& list) {
    for (Stmt* s : list) {
        if (!s) continue;
        switch (s->kind) {
            case StmtKind::DECLARATION: {
                if (pythonReserved(s->name)) {
                    g_errors.add(s->line, s->column, "variable name '" + s->name + "' conflicts with Python syntax or generated code");
                    break;
                }
                if (!symbols_.declare(s->name, s->declaredType)) {
                    g_errors.add(s->line, s->column, "variable already declared: '" + s->name + "'");
                    break;
                }
                if (s->expr) {
                    ValueType source = analyzeExpr(s->expr);
                    if (s->expr->kind != ExprKind::INPUT && !assignable(s->declaredType, source))
                        g_errors.add(s->line, s->column, "cannot initialize " + typeName(s->declaredType) + " with " + typeName(source));
                }
                break;
            }
            case StmtKind::ASSIGNMENT: {
                if (!symbols_.contains(s->name)) {
                    g_errors.add(s->line, s->column, "assignment to undeclared variable '" + s->name + "'");
                    analyzeExpr(s->expr);
                    break;
                }
                ValueType source = analyzeExpr(s->expr), target = symbols_.lookup(s->name);
                s->declaredType = target;
                if (s->expr && s->expr->kind != ExprKind::INPUT && !assignable(target, source))
                    g_errors.add(s->line, s->column, "cannot assign " + typeName(source) + " to " + typeName(target));
                break;
            }
            case StmtKind::INCREMENT:
                if (!symbols_.contains(s->name)) g_errors.add(s->line, s->column, "increment of undeclared variable '" + s->name + "'");
                else if (!numeric(symbols_.lookup(s->name))) g_errors.add(s->line, s->column, s->op + " requires a numeric variable");
                break;
            case StmtKind::PRINT: analyzeExpr(s->expr); break;
            case StmtKind::IF: {
                ValueType cond = analyzeExpr(s->expr);
                if (cond != ValueType::BOOL && cond != ValueType::UNKNOWN) g_errors.add(s->line, s->column, "if condition must be boolean");
                analyzeStatements(s->body); analyzeStatements(s->elseBody); break;
            }
            case StmtKind::WHILE: {
                ValueType cond = analyzeExpr(s->expr);
                if (cond != ValueType::BOOL && cond != ValueType::UNKNOWN) g_errors.add(s->line, s->column, "while condition must be boolean");
                ++loopDepth_;
                analyzeStatements(s->body);
                --loopDepth_;
                break;
            }
            case StmtKind::BLOCK: analyzeStatements(s->body); break;
            case StmtKind::BREAK:
                if (loopDepth_ == 0) g_errors.add(s->line, s->column, "tham (break) is only valid inside a loop");
                break;
            case StmtKind::CONTINUE:
                if (loopDepth_ == 0) g_errors.add(s->line, s->column, "chol (continue) is only valid inside a loop");
                break;
            case StmtKind::INCLUDE: break;
        }
    }
}
