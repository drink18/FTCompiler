#ifndef TOKEN_H
#define TOKEN_H
#include "../control/region.h"
#include "../lib/string.h"

/*
enum Token_kind {
  TOKEN_ZERO = 0,
  TOKEN_ADD,
  TOKEN_AND,
  TOKEN_ASSIGN,
  TOKEN_AT,
  TOKEN_BREAK,
  TOKEN_COMMER,
  TOKEN_CONTINUE,
  TOKEN_DIVIDE,
  TOKEN_DO,
  TOKEN_DOT,
  TOKEN_COLON,
  TOKEN_ELSE,
  TOKEN_EQ,
  TOKEN_EOF,
  TOKEN_FOR,
  TOKEN_GE,
  TOKEN_GT,
  TOKEN_ID,
  TOKEN_IF,
  TOKEN_INTLIT,
  TOKEN_FLOATLIT,
  TOKEN_DOUBLELIT,
  TOKEN_LBRACE,
  TOKEN_LBRACK,
  TOKEN_LE,
  TOKEN_SHIFTLEFT,
  TOKEN_LPAREN,
  TOKEN_LT,
  TOKEN_MINUS,
  TOKEN_NEQ,
  TOKEN_NOT,
  TOKEN_NULL,
  TOKEN_OF,
  TOKEN_OR,
  TOKEN_PERCENT,
  TOKEN_RBRACE,
  TOKEN_RBRACK,
  TOKEN_RETURN,
  TOKEN_RPAREN,
  TOKEN_SEMI,
  TOKEN_STRINGLIT,
  TOKEN_TIMES,
  TOKEN_TYPENAME,
  TOKEN_WHILE,
  TOKEN_GOTO,
  TOKEN_END,
  TOKEN_DEALY,
  TOKEN_UPDATE,
  TOKEN_TRIGGER,
  TOKEN_WAIT,
  TOKEN_TIMER,
  TOKEN_SETR,
  TOKEN_COMMON,
  TOKEN_CONST,
  TOKEN_PRINTF,
  TOKEN_IMPORT
};

*/

typedef enum Token_kind Token_kind;

#define T Token_t

typedef struct T *T;

struct T
{
  int kind;
  Region_t region;
};

T Token_new (int kind,
             int line
            );

#undef T

#endif
