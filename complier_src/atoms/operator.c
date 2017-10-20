#include "../lib/error.h"
#include "operator.h"

#define T Operator_t

String_t Operator_toString (T t)
{
  switch (t){
  case OP_ADD:
    return "+";
  case OP_SUB:
    return "-";
  case OP_TIMES:
    return "*";
  case OP_DIVIDE:
    return "/";
  case OP_MODUS:
    return "%";
  case OP_OR:
    return "||";
  case OP_AND:
    return "&&";
  case OP_EQ:
    return "==";
  case OP_NE:
    return "!=";
  case OP_LT:
    return "<";
  case OP_LE:
    return "<=";
  case OP_GT:
    return ">";
  case OP_GE:
    return ">=";
  case OP_NOT:
    return "!";
  case OP_NEG:
    return "-";
  default:
    Error_bug ("impossible");
    return "";
  }
  Error_bug ("impossible");
  return "";
}

#undef T
