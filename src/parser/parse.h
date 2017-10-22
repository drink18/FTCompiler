#ifndef PARSE_H
#define PARSE_H

#include "../lib/string.h"
#include "../ast/ast.h"
#define TOKEN_ASSIGN '='
#define TOKEN_LPAREN '('
#define TOKEN_RPAREN ')'
#define TOKEN_LBRACE '{'
#define TOKEN_RBRACE '}'
#define TOKEN_DOT '.'
#define TOKEN_COLON ':'
#define TOKEN_COMMER ','
#define TOKEN_LBRACK '['
#define TOKEN_RBRACK ']'
#define TOKEN_ADD '+'
#define TOKEN_MINUS '-'
#define TOKEN_TIMES '*'
#define TOKEN_DIVIDE '/'
#define TOKEN_PERCENT '%'
#define TOKEN_NOT '!'
#define TOKEN_SEMI ';'
#define TOKEN_AND '&'
#define TOKEN_OR '|'

//must above this
#define TOKEN_ID (TOKEN_OF+1)

#define T Token_t

typedef struct T *T;

struct T
{
  int kind;
  Region_t region;
};

static LinkedList_t Parse_declarationList(int flag);
Ast_Prog_t Parse_parse (String_t file);
#undef T
#endif
