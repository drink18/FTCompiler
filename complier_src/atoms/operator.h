#ifndef OPERATOR_H
#define OPERATOR_H

#include "../lib/string.h"

#define T Operator_t

typedef enum {
  OP_ADD,
  OP_SUB,
  OP_TIMES,
  OP_DIVIDE,
  OP_MODUS,
  OP_OR,
  OP_AND,
  OP_EQ,
  OP_NE,
  OP_LT,
  OP_LE,
  OP_GT,
  OP_GE,
  OP_NOT,
  OP_NEG  //Çó²¹ÂëµÄÖ¸Áî
} T;

String_t Operator_toString (T);

#undef T

#endif
