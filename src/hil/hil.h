#ifndef HIL_H
#define HIL_H

#include "../lib/linkedList.h"
#include "../lib/file.h"
#include "../atoms/atoms.h"
#include "../lib/internalFunc.h"

#define List_t LinkedList_t

#define P Hil_Prog_t
#define T Hil_Type_t
#define Ty Hil_Ty_t
#define Td Hil_Td_t
#define F Hil_Fun_t
#define S Hil_Stm_t
#define L Hil_Lval_t
#define E Hil_Exp_t
#define Af Hil_Field_t
#define D Hil_Dec_t
#define Stt Hil_Struct_t

typedef struct L *L;
typedef struct Af *Af;
typedef struct E *E;
typedef struct Td *Td;
typedef struct T *T;
typedef struct Ty *Ty;
typedef struct S *S;
typedef struct Stt *Stt;
typedef struct F *F;
typedef struct D *D;
typedef struct P *P;

struct Ty
{
  Id_t name;
};

Ty Hil_Ty_new (Id_t);
void Hil_Ty_print ();

struct L
{
  enum {
    HIL_LVAL_VAR,
    HIL_LVAL_DOT,
    HIL_LVAL_ARRAY
  }kind;
  union{
    Id_t var;
    struct{
      L lval;
      Id_t var;
    }dot;
    struct {
      L lval;
      E exp;
    }array;
  }u;
  Id_t ty;
};

L Hil_Lval_new_var (Id_t, Id_t ty);
L Hil_Lval_new_dot (L, Id_t, Id_t ty);
L Hil_Lval_new_array (L, E, Id_t ty);
void Hil_Lval_print (L);

struct Af
{
  Id_t id;
  E e;
};
Af Hil_Field_new (Id_t, E);
void Hil_Field_print (Af);

typedef enum {
  HIL_EXP_BOP,
  HIL_EXP_UOP,
  HIL_EXP_INTLIT,
  HIL_EXP_FLOATLIT,
  HIL_EXP_DOUBLELIT,
  HIL_EXP_STRINGLIT,
  HIL_EXP_NEW_STRUCT,
  HIL_EXP_NEW_ARRAY,
  HIL_EXP_LVAL,
  HIL_EXP_CALL
} Hil_Exp_Kind_t ;

struct E
{
  Hil_Exp_Kind_t kind;
  union{
    struct {
      E left;
      E right;
      Operator_t op;
    }bop;
    struct {
      E e;
      Operator_t op;
    }unary;
    struct {
      Id_t type;
      E size;
      E init;
    }newArray;
    struct {
      Id_t type;
      /* List_t<Ast_Field_t> */
      List_t list;
    }newStruct;
    int intlit;
	float floatlit;
	double doublelit;
    String_t stringlit;
    struct{
      Id_t f;
      /* List<E> */
      List_t args;
    }call;
    L lval;
  }u;
  Id_t ty;
};

E Hil_Exp_new_bop (Operator_t op,
                   E left,
                   E right);
E Hil_Exp_new_unary (Operator_t op,
                     E e);
E Hil_Exp_new_intlit (int);
E Hil_Exp_new_doublelit (double i);

E Hil_Exp_new_stringlit (String_t);
E Hil_Exp_new_newArray (Id_t id, E size, E init);
E Hil_Exp_new_newStruct (Id_t id, List_t);
E Hil_Exp_new_call (Id_t, List_t, Id_t ty);
E Hil_Exp_new_lval (L, Id_t ty);
S Hil_Stm_new_internalfunc (InternalFun ifun,List_t t);

void Hil_Exp_print (E);

struct S
{
  enum {
    HIL_STM_ASSIGN,
	HIL_STM_ASSIGN_DTOI,
	HIL_STM_ASSIGN_ITOD,
    HIL_STM_EXP,
    HIL_STM_IF,
    HIL_STM_DO,
    HIL_STM_JUMP,
    HIL_STM_RETURN,
    HIL_STM_INTERNAL_FUNC,
    HIL_STM_END,
    HIL_STM_UPDATE,
    HIL_STM_LABEL
  } kind;
  union {
    struct {
      L lval;
      E exp;
    } assign;
    E exp;
    struct {
      E condition;
      /* List<S> */
      List_t then;
      /* List<S> */
      List_t elsee;
    } iff;
    struct {
      E condition;
      /* List<S> */
      List_t body;
      Label_t entryLabel;
      Label_t exitLabel;
      /* paddling the last part s2 in
       *   for (s1; e; s2)
       */
      List_t padding;
    } doo;
	struct {
    Label_t labelName;
	List_t s;
	}label;

	struct{
      InternalFun iFun;
      List_t args;
	}infuc;
    E returnn;
    Label_t jump;
  } u;
};


S Hil_Stm_new_assign (L, E);
S Hil_Stm_new_assign_dtoi(L, E);
S Hil_Stm_new_assign_itod (L, E);

S Hil_Stm_new_exp (E);
S Hil_Stm_new_if (E, List_t, List_t);
S Hil_Stm_new_do (E, List_t, Label_t entryLabel
                  , Label_t exitLabel
                  , List_t);
S Hil_Stm_new_jump (Label_t label);
S Hil_Stm_new_return (E);
void Hil_Stm_print (S);
S Hil_Stm_new_update ();
S Hil_Stm_new_end ();
S Hil_Stm_new_label (Label_t labelName,List_t stm);


/* function */
struct F
{
  Id_t type;
  Id_t name;
  /* List<Stt> */
  List_t args;
  /* List<Struct_t> */
  List_t decs;
  /* List<Stm_t> */
  List_t stms;
  int attr;
};

F Hil_Fun_new (Id_t, Id_t, List_t, List_t, List_t,int);
void Hil_Fun_print (F);

/* struct fields & function parameters */
/* this should be rewritten into two different types */
struct Stt
{
  Id_t type;
  Id_t var;
};
Stt Hil_Struct_new (Id_t, Id_t);
void Hil_Struct_print (Stt);

/* type definition */
struct T
{
  enum {
    HIL_TYPE_STRUCT,
    HIL_TYPE_ARRAY
  } kind;
  union {
    /* List<structt> */
    List_t structt;
    Id_t array;
  } u;
};

T Hil_Type_new_struct (List_t);
T Hil_Type_new_array (Id_t);
void Hil_Type_print (T);

struct Td
{
  Id_t name;
  T type;
};

Td Hil_Td_new (Id_t, T);
void Hil_Td_print (Td);

struct P
{
  List_t tds;
  List_t funcs;
};

P Hil_Prog_new (List_t types, List_t funcs);
void Hil_Prog_print (P x, File_t f);

#undef P
#undef T
#undef Ty
#undef Td
#undef F
#undef S
#undef L
#undef E
#undef D
#undef Af
#undef Stt
#undef List_t

#endif
