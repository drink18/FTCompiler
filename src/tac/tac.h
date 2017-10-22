#ifndef TAC_H
#define TAC_H

#include "../lib/linkedList.h"
#include "../lib/file.h"
#include "../atoms/atoms.h"
#include "../lib/internalFunc.h"

#define List_t LinkedList_t

#define P Tac_Prog_t
#define T Tac_Type_t
#define Td Tac_Td_t
#define F Tac_Fun_t
#define S Tac_Stm_t
#define O Tac_Operand_t
#define Af Tac_Field_t
#define D Tac_Dec_t
#define Stt Tac_Struct_t

typedef struct Af *Af;
typedef struct O *O;
typedef struct Td *Td;
typedef struct T *T;
typedef struct S *S;
typedef struct Stt *Stt;
typedef struct F *F;
typedef struct D *D;
typedef struct P *P;

struct Af
{
  Id_t id;
  Id_t e;
};
Af Tac_Field_new (Id_t, Id_t);
void Tac_Field_print (Af);

struct O
{
  enum {
    TAC_OP_INT,
	TAC_OP_FLOAT,
	TAC_OP_DOUBLE,
    TAC_OP_STR,
    TAC_OP_VAR,
    TAC_OP_STRUCT,
    TAC_OP_ARRAY
  } kind;
  union{
    int intlit;
	float floatlit;
	double doublelit;
    String_t strlit;
    Id_t var;
    struct {
      Id_t base;
      Id_t offset;
    }structt;
    struct {
      Id_t base;
      Id_t offset;
    }array;
  }u;
};

O Tac_Operand_new_int (int i);
O Tac_Operand_new_float (float i);
O Tac_Operand_new_double (double i);

O Tac_Operand_new_string (String_t strlit);
O Tac_Operand_new_var (Id_t id);
O Tac_Operand_new_struct (Id_t base, Id_t offset);
O Tac_Operand_new_array (Id_t base, Id_t offset);
void Tac_Operand_print (O);

struct S
{
  enum {
    TAC_STM_MOVE,
	TAC_STM_MOVE_ITOD,
	TAC_STM_MOVE_DTOI,
    TAC_STM_BOP,
    TAC_STM_UOP,
    TAC_STM_CALL,
    TAC_STM_IF,
    TAC_STM_LABEL,
    TAC_STM_JUMP,
    TAC_STM_RETURN,
    TAC_STM_RETURNVOID,
    TAC_STM_NEW_STRUCT,
    TAC_STM_NEW_ARRAY,
    TAC_STM_PRIMITIVE,
    TAC_STM_END,
    TAC_STM_UPDATE
  } kind;
  union {
    struct {
      O src;
      O dest;
    } move;
    struct {
      O left;
      O right;
      Operator_t op;
      Id_t dest;
    } bop;
    struct {
      O src;
      Operator_t op;
      Id_t dest;
    } uop;
    struct {
      Id_t dest;
      Id_t name;
      /* List<O> */
      List_t args;
    } call;
    struct {
      O src;
      Label_t truee;
      Label_t falsee;
    } iff;
    Label_t label;
    Id_t returnn;
    Label_t jump;
    struct {
      Id_t dest;
      Id_t type;
      /* List<O> */
      List_t args;
    }newStruct;
    struct {
      Id_t dest;
      Id_t type;
      O size;
      O init;
    }newArray;
	struct{
      InternalFun iFun;
      List_t args;
	}infuc;
  } u;
};

S Tac_Stm_new_move (O dest, O src);
S Tac_Stm_new_bop (Id_t dest, O left,
                   Operator_t op, O right);
S Tac_Stm_new_uop (Id_t dest, Operator_t opr, O src);
S Tac_Stm_new_call (Id_t dest, Id_t name, List_t args);
S Tac_Stm_new_if (O, Label_t, Label_t);
S Tac_Stm_new_label (Label_t label);
S Tac_Stm_new_jump (Label_t label);
S Tac_Stm_new_return (Id_t);
S Tac_Stm_new_returnvoid();

S Tac_Stm_new_newStruct (Id_t, Id_t, List_t);
S Tac_Stm_new_newArray (Id_t, Id_t, O size, O arg);
void Tac_Stm_print (S);
S Tac_Stm_new_update ();
S Tac_Stm_new_end ();
S Tac_Stm_new_primitive (InternalFun ifun,List_t t);




/* function */
struct F
{
  Id_t type;
  Id_t name;
  /* List<Stt> */
  List_t args;
  /* List<Stt> */
  List_t decs;
  /* List<Stm_t> */
  List_t stms;
  Id_t retId;
  Label_t entry;
  Label_t exitt;
  int attr;
};

F Tac_Fun_new (Id_t, Id_t, List_t, List_t, List_t,
               Id_t, Label_t, Label_t,int);
void Tac_Fun_print (F);

/* struct fields & function parameters */
/* this should be rewritten into two different types */
struct Stt
{
  Id_t type;
  Id_t var;
};
Stt Tac_Struct_new (Id_t, Id_t);
void Tac_Struct_print (Stt);

/* type definition */
struct T
{
  enum {
    TAC_TYPE_STRUCT,
    TAC_TYPE_ARRAY
  } kind;
  union {
    /* List<structt> */
    List_t structt;
    Id_t array;
  } u;
};

T Tac_Type_new_struct (List_t);
T Tac_Type_new_array (Id_t);
void Tac_Type_print (T);

struct Td
{
  Id_t name;
  T type;
};

Td Tac_Td_new (Id_t, T);
void Tac_Td_print (Td);

struct P
{
  List_t tds;
  List_t funcs;
};

P Tac_Prog_new (List_t types, List_t funcs);
void Tac_Prog_print (P x, File_t f);

#undef P
#undef T
#undef Td
#undef F
#undef S
#undef O
#undef D
#undef Af
#undef Stt
#undef List_t

#endif
