#include "parser.hpp"
#include "error_reporting.hpp"
#include "tokens.hpp"
#include <cstdio>
#include <stdexcept>
#include <string>

extern int yylex(void);
Token g_token;

namespace {
class ParseFailure : public std::runtime_error {
public:
    explicit ParseFailure(const std::string& message) : std::runtime_error(message) {}
};

class RecursiveDescentParser {
public:
    RecursiveDescentParser() { advance(); }

    bool parse(std::vector<Stmt*>& program) {
        while (current_.kind != TOK_END) {
            const int beforeLine = current_.line;
            const int beforeColumn = current_.column;
            try {
                program.push_back(statement());
            } catch (const ParseFailure& error) {
                g_errors.add(current_.line, current_.column, error.what());
                synchronize();
                if (current_.kind != TOK_END && current_.line == beforeLine && current_.column == beforeColumn) advance();
            }
        }
        return !g_errors.hasErrors();
    }

private:
    Token current_;

    void advance() {
        yylex();
        current_ = g_token;
    }

    bool accept(int kind) {
        if (current_.kind != kind) return false;
        advance();
        return true;
    }

    void expect(int kind, const std::string& expected) {
        if (current_.kind != kind) fail("expected " + expected);
        advance();
    }

    [[noreturn]] void fail(const std::string& message) const {
        throw ParseFailure(message + ", found '" + (current_.text.empty() ? "end of file" : current_.text) + "'");
    }

    void synchronize() {
        while (current_.kind != TOK_END && current_.kind != ';' && current_.kind != '}') advance();
        if (current_.kind == ';') advance();
    }

    static Stmt* at(Stmt* node, int line, int column) {
        node->line = line;
        node->column = column;
        return node;
    }

    static Expr* at(Expr* node, int line, int column) {
        node->line = line;
        node->column = column;
        return node;
    }

    bool isType(int kind) const {
        return kind == TOK_INT || kind == TOK_FLOAT || kind == TOK_TEXT || kind == TOK_BOOL;
    }

    ValueType parseType() {
        const int kind = current_.kind;
        advance();
        if (kind == TOK_INT) return ValueType::INT;
        if (kind == TOK_FLOAT) return ValueType::FLOAT;
        if (kind == TOK_TEXT) return ValueType::STRING;
        return ValueType::BOOL;
    }

    Stmt* statement() {
        const int line = current_.line;
        const int column = current_.column;
        if (isType(current_.kind)) {
            const ValueType type = parseType();
            if (current_.kind != TOK_IDENTIFIER) fail("expected a variable name");
            const std::string name = current_.text;
            advance();
            Stmt* node = makeStmt(StmtKind::DECLARATION);
            node->name = name;
            node->declaredType = type;
            if (accept('=')) node->expr = expression();
            expect(';', "';'");
            return at(node, line, column);
        }
        if (current_.kind == TOK_IDENTIFIER) {
            const std::string name = current_.text;
            advance();
            if (accept('=')) {
                Stmt* node = makeStmt(StmtKind::ASSIGNMENT);
                node->name = name;
                node->expr = expression();
                expect(';', "';'");
                return at(node, line, column);
            }
            if (current_.kind == TOK_INC || current_.kind == TOK_DEC) {
                const int op = current_.kind;
                advance();
                Stmt* node = makeStmt(StmtKind::INCREMENT);
                node->name = name;
                node->op = op == TOK_INC ? "++" : "--";
                expect(';', "';'");
                return at(node, line, column);
            }
            fail("expected assignment, increment, or decrement after variable name");
        }
        if (current_.kind == TOK_PRINT) {
            advance();
            expect('(', "'('");
            Expr* value = expression();
            expect(')', "')'");
            expect(';', "';'");
            Stmt* node = makeStmt(StmtKind::PRINT);
            node->expr = value;
            return at(node, line, column);
        }
        if (current_.kind == TOK_IF) return ifStatement();
        if (current_.kind == TOK_WHILE) return whileStatement();
        if (current_.kind == TOK_BREAK || current_.kind == TOK_CONTINUE) {
            const bool isBreak = current_.kind == TOK_BREAK;
            advance();
            expect(';', "';'");
            return at(makeStmt(isBreak ? StmtKind::BREAK : StmtKind::CONTINUE), line, column);
        }
        if (current_.kind == TOK_INCLUDE) {
            advance();
            expect(';', "';'");
            return at(makeStmt(StmtKind::INCLUDE), line, column);
        }
        if (current_.kind == '{') return block();
        fail("expected a statement");
    }

    Stmt* block() {
        const int line = current_.line;
        const int column = current_.column;
        expect('{', "'{'");
        Stmt* node = makeStmt(StmtKind::BLOCK);
        while (current_.kind != '}' && current_.kind != TOK_END) {
            const int beforeLine = current_.line;
            const int beforeColumn = current_.column;
            try {
                node->body.push_back(statement());
            } catch (const ParseFailure& error) {
                g_errors.add(current_.line, current_.column, error.what());
                synchronize();
                if (current_.kind != TOK_END && current_.kind != '}' && current_.line == beforeLine && current_.column == beforeColumn) advance();
            }
        }
        expect('}', "'}'");
        return at(node, line, column);
    }

    Stmt* ifStatement() {
        const int line = current_.line;
        const int column = current_.column;
        advance();
        expect('(', "'('");
        Expr* condition = expression();
        expect(')', "')'");
        Stmt* thenBlock = block();
        Stmt* node = makeStmt(StmtKind::IF);
        node->expr = condition;
        node->body = thenBlock->body;
        if (accept(TOK_ELSE)) {
            if (current_.kind == TOK_IF) node->elseBody.push_back(ifStatement());
            else node->elseBody = block()->body;
        }
        return at(node, line, column);
    }

    Stmt* whileStatement() {
        const int line = current_.line;
        const int column = current_.column;
        advance();
        expect('(', "'('");
        Expr* condition = expression();
        expect(')', "')'");
        Stmt* body = block();
        Stmt* node = makeStmt(StmtKind::WHILE);
        node->expr = condition;
        node->body = body->body;
        return at(node, line, column);
    }

    int precedence(int kind) const {
        switch (kind) {
            case TOK_OR: return 1;
            case TOK_AND: return 2;
            case '|': return 3;
            case '^': return 4;
            case '&': return 5;
            case TOK_EQ: case TOK_NE: return 6;
            case '<': case '>': case TOK_LE: case TOK_GE: return 7;
            case TOK_SHL: case TOK_SHR: return 8;
            case '+': case '-': return 9;
            case '*': case '/': case '%': return 10;
            default: return 0;
        }
    }

    std::string operatorText(const Token& token) const {
        switch (token.kind) {
            case TOK_AND: return "&&";
            case TOK_OR: return "||";
            case TOK_EQ: return "==";
            case TOK_NE: return "!=";
            case TOK_LE: return "<=";
            case TOK_GE: return ">=";
            case TOK_SHL: return "<<";
            case TOK_SHR: return ">>";
            default: return token.text;
        }
    }

    Expr* expression(int minimum = 1) {
        Expr* left = unary();
        while (precedence(current_.kind) >= minimum) {
            const Token op = current_;
            const int opPrecedence = precedence(op.kind);
            advance();
            Expr* right = expression(opPrecedence + 1);
            left = at(makeExpr(ExprKind::BINARY, operatorText(op), left, right), op.line, op.column);
        }
        return left;
    }

    Expr* unary() {
        if (current_.kind == '!' || current_.kind == '-' || current_.kind == '+') {
            const Token op = current_;
            advance();
            return at(makeExpr(ExprKind::UNARY, op.text, unary()), op.line, op.column);
        }
        Expr* result = primary();
        while (current_.kind == '[') {
            const int line = current_.line;
            const int column = current_.column;
            advance();
            Expr* index = expression();
            expect(']', "']'");
            result = at(makeExpr(ExprKind::INDEX, {}, result, index), line, column);
        }
        return result;
    }

    Expr* primary() {
        const Token token = current_;
        if (current_.kind == TOK_NUMBER) {
            advance();
            return at(makeExpr(ExprKind::NUMBER, token.text), token.line, token.column);
        }
        if (current_.kind == TOK_STRING) {
            advance();
            return at(makeExpr(ExprKind::STRING_LITERAL, token.text), token.line, token.column);
        }
        if (current_.kind == TOK_TRUE || current_.kind == TOK_FALSE) {
            advance();
            return at(makeExpr(ExprKind::BOOLEAN, token.kind == TOK_TRUE ? "True" : "False"), token.line, token.column);
        }
        if (current_.kind == TOK_IDENTIFIER) {
            advance();
            return at(makeExpr(ExprKind::VARIABLE, token.text), token.line, token.column);
        }
        if (current_.kind == TOK_INPUT) {
            advance();
            expect('(', "'('");
            expect(')', "')'");
            return at(makeExpr(ExprKind::INPUT), token.line, token.column);
        }
        if (current_.kind == TOK_TYPEOF) {
            advance();
            expect('(', "'('");
            Expr* inner = expression();
            expect(')', "')'");
            return at(makeExpr(ExprKind::TYPE_NAME, {}, inner), token.line, token.column);
        }
        if (accept('(')) {
            Expr* inner = expression();
            expect(')', "')'");
            return inner;
        }
        fail("expected an expression");
    }
};
}

bool Parser::parse(std::vector<Stmt*>& program) {
    return RecursiveDescentParser().parse(program);
}
