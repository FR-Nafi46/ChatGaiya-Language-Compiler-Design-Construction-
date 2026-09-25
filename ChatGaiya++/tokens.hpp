#pragma once
#include <string>

enum TokenKind {
    TOK_END = 0,
    TOK_IDENTIFIER = 256,
    TOK_NUMBER,
    TOK_STRING,
    TOK_INT,
    TOK_FLOAT,
    TOK_TEXT,
    TOK_BOOL,
    TOK_TRUE,
    TOK_FALSE,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_INPUT,
    TOK_PRINT,
    TOK_TYPEOF,
    TOK_INCLUDE,
    TOK_AND,
    TOK_OR,
    TOK_EQ,
    TOK_NE,
    TOK_LE,
    TOK_GE,
    TOK_SHL,
    TOK_SHR,
    TOK_INC,
    TOK_DEC,
    TOK_INVALID
};

struct Token {
    int kind = TOK_END;
    std::string text;
    int line = 1;
    int column = 1;
};

extern Token g_token;
