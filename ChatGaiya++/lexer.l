%option noyywrap nounput noinput never-interactive
%x BLOCK_COMMENT
%{
#define YY_NO_UNISTD_H 1
#include "tokens.hpp"
#include "error_reporting.hpp"
#include <string>

static int lexerLine = 1;
static int lexerColumn = 1;
static int tokenLine = 1;
static int tokenColumn = 1;

static int emitToken(int kind, const char* text, int length) {
    g_token.kind = kind;
    g_token.text.assign(text, static_cast<std::size_t>(length));
    g_token.line = tokenLine;
    g_token.column = tokenColumn;
    return kind;
}

#define YY_USER_ACTION do { \
    tokenLine = lexerLine; \
    tokenColumn = lexerColumn; \
    for (int i = 0; i < yyleng; ++i) { \
        if (yytext[i] == '\n') { ++lexerLine; lexerColumn = 1; } \
        else { ++lexerColumn; } \
    } \
} while (0);
%}

DIGIT       [0-9]
IDSTART     [A-Za-z_]
IDCONT      [A-Za-z0-9_]
NUMBER      ({DIGIT}+\.{DIGIT}*|\.{DIGIT}+|{DIGIT}+)
STRING      (\"([^\\\"\n]|\\.)*\"|\'([^\\\'\n]|\\.)*\')

%%
[ \t\r\n]+                       ;
"//"[^\n]*                       ;
"/*"                              BEGIN(BLOCK_COMMENT);
<BLOCK_COMMENT>"*/"               BEGIN(INITIAL);
<BLOCK_COMMENT>[^*\n]+            ;
<BLOCK_COMMENT>\*                  ;
<BLOCK_COMMENT>\n                  ;
<BLOCK_COMMENT><<EOF>>             { g_errors.add(lexerLine, lexerColumn, "unterminated block comment"); BEGIN(INITIAL); return emitToken(TOK_INVALID, "", 0); }
"#include"[^\n]*                 ;
"shuru_kor"                       return emitToken(TOK_INCLUDE, yytext, yyleng);
"ongko"                           return emitToken(TOK_INT, yytext, yyleng);
"dhoshomik"                       return emitToken(TOK_FLOAT, yytext, yyleng);
"kotha"                           return emitToken(TOK_TEXT, yytext, yyleng);
"ho"                              return emitToken(TOK_BOOL, yytext, yyleng);
"hasa"                            return emitToken(TOK_TRUE, yytext, yyleng);
"misa"                            return emitToken(TOK_FALSE, yytext, yyleng);
"zodi"                            return emitToken(TOK_IF, yytext, yyleng);
"noile"                           return emitToken(TOK_ELSE, yytext, yyleng);
"tokon"                           return emitToken(TOK_ELSE, yytext, yyleng);
"zotokkhon"                       return emitToken(TOK_WHILE, yytext, yyleng);
"tham"                            return emitToken(TOK_BREAK, yytext, yyleng);
"chol"                            return emitToken(TOK_CONTINUE, yytext, yyleng);
"lo"                              return emitToken(TOK_INPUT, yytext, yyleng);
"ko"                              return emitToken(TOK_PRINT, yytext, yyleng);
"ki_type"                         return emitToken(TOK_TYPEOF, yytext, yyleng);
"&&"                              return emitToken(TOK_AND, yytext, yyleng);
"||"                              return emitToken(TOK_OR, yytext, yyleng);
"=="                              return emitToken(TOK_EQ, yytext, yyleng);
"!="                              return emitToken(TOK_NE, yytext, yyleng);
"<="                              return emitToken(TOK_LE, yytext, yyleng);
">="                              return emitToken(TOK_GE, yytext, yyleng);
"<<"                              return emitToken(TOK_SHL, yytext, yyleng);
">>"                              return emitToken(TOK_SHR, yytext, yyleng);
"++"                              return emitToken(TOK_INC, yytext, yyleng);
"--"                              return emitToken(TOK_DEC, yytext, yyleng);
{STRING}                          return emitToken(TOK_STRING, yytext, yyleng);
{NUMBER}                          return emitToken(TOK_NUMBER, yytext, yyleng);
{IDSTART}{IDCONT}*                return emitToken(TOK_IDENTIFIER, yytext, yyleng);
[{}()\[\];,+*/%<>!=&|^~-]        return emitToken(static_cast<unsigned char>(yytext[0]), yytext, yyleng);
.                                 { g_errors.add(tokenLine, tokenColumn, std::string("invalid character '") + yytext[0] + "'"); return emitToken(TOK_INVALID, yytext, yyleng); }
<<EOF>>                           { g_token.kind = TOK_END; g_token.text.clear(); g_token.line = lexerLine; g_token.column = lexerColumn; return TOK_END; }
%%
