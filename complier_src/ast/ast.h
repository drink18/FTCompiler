#ifndef AST_H
#define AST_H

#include "../lib/linkedList.h"
#include "../lib/file.h"
#include "../lib/bool.h"
#include "../lib/box.h"
#include "../control/region.h"
#include "../lib/internalFunc.h"

#include "ast-id.h"

#define List_t LinkedList_t
#define Id_t AstId_t

#define P Ast_Prog_t
#define T Ast_Type_t
#define Td Ast_Td_t
#define F Ast_Fun_t
#define S Ast_Stm_t
#define L Ast_Lval_t
#define B Ast_Block_t
#define E Ast_Exp_t
#define Af Ast_Field_t
#define D Ast_Dec_t
#define Stt Ast_Struct_t

typedef struct L *L;
typedef struct Af *Af;
typedef struct E *E;
typedef struct Td *Td;
typedef struct T *T;
typedef struct B *B;
typedef struct S *S;
typedef struct Stt *Stt;
typedef struct F *F;
typedef struct D *D;
typedef struct P *P;
typedef struct label_for_boards_t *label_for_boards_t;



struct label_for_boards_t
{
 String_t label_name;
 B body;
};


label_for_boards_t Ast_new_lb();

struct L
{
  enum {
    AST_LVAL_VAR,
    AST_LVAL_DOT,
    AST_LVAL_ARRAY
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
  Region_t region;
};

L Ast_Lval_new_var (Id_t, Region_t);
L Ast_Lval_new_dot (L, Id_t, Region_t);
L Ast_Lval_new_array (L, E, Region_t);
void Ast_Lval_print (L);

struct Af
{
  Id_t id;
  E e;
};
Af Ast_Field_new (Id_t, E);
void Ast_Field_print (Af);
long long  int  Ast_E_int (E e);
D Ast_Dec_looup (List_t l, Id_t var);
typedef enum {
  AST_EXP_ADD,
  AST_EXP_SUB,
  AST_EXP_TIMES,
  AST_EXP_DIVIDE,
  AST_EXP_MODUS,
  AST_EXP_OR,
  AST_EXP_AND,
  AST_EXP_EQ,
  AST_EXP_NE,
  AST_EXP_LT,
  AST_EXP_SL,
  AST_EXP_LE,
  AST_EXP_GT,
  AST_EXP_GE,
  AST_EXP_NOT,
  AST_EXP_NEGATIVE,
  AST_EXP_NULL,
  AST_EXP_INTLIT,
  AST_EXP_FLOATLIT,
  AST_EXP_DOUBLELIT,
  AST_EXP_STRINGLIT,
  AST_EXP_NEW_STRUCT,
  AST_EXP_NEW_ARRAY,
  AST_EXP_LVAL,
  AST_EXP_CALL
} Ast_Exp_Kind_t ;

struct E
{
  Ast_Exp_Kind_t kind;
  union{
    struct {
      E left;
      E right;
    }bop;
    struct {
      E e;
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
      Id_t ty;
      /* List<E> */
      List_t args;
    }call;
    L lval;
  }u;
  Region_t region;
};

E Ast_Exp_new_bop (int kind,
                   E left,
                   E right,
                   Region_t r);
E Ast_Exp_new_unary (Ast_Exp_Kind_t kind,
                     E e,
                     Region_t r);
E Ast_Exp_new_null ();
E Ast_Exp_new_intlit (int);
E Ast_Exp_new_intlit_value (int s);
E Ast_Exp_new_floatlit (float s);
// I add doublelit intlit
E Ast_Exp_new_doublelit_value (double s);
E Ast_Exp_new_intlit_value (int s);


E Ast_Exp_new_floatlit (float s);
E Ast_Exp_new_stringlit (String_t);
E Ast_Exp_new_newArray (Id_t id, E size, E init);
E Ast_Exp_new_newStruct (Id_t id, List_t);
E Ast_Exp_new_call (Id_t f, List_t, Id_t ty);
E Ast_Exp_new_lval (L);
void Ast_Exp_print (E);
S Ast_Stm_new_internalfunc (InternalFun funcKind,List_t args, Region_t r);
S Ast_Stm_new_update (Region_t r);
S Ast_Stm_new_end (Region_t r);
Box_t Ast_Block_box (B b);


struct D
{
  Id_t type;
  Id_t var;
  E init;
  bool sta;
};

D Ast_Dec_new (Id_t type, Id_t var, E init,bool sta);
void Ast_Dec_print (D);

struct B
{
  /* List<Dec_t> */
  List_t decs;
  /* List<Stm_t> */
  List_t stms;
};

B Ast_Block_new (List_t, List_t);
void Ast_Block_print (B);

struct S
{
  enum {
    AST_STM_ASSIGN,
	AST_STM_ASSIGN_ITOD,
	AST_STM_ASSIGN_DTOI,
    AST_STM_EXP,
    AST_STM_IF,
    AST_STM_WHILE,
    AST_STM_DO,
    AST_STM_FOR,
    AST_STM_BREAK,
    AST_STM_CONTINUE,
    AST_STM_RETURN,
    AST_STM_BLOCK,
    AST_STM_GOTO,
    AST_STM_LABEL,
    AST_STM_RESERVED_LABEL,
    AST_STM_END,
    AST_STM_UPDATE,
    AST_STM_INTERNAL_FUNC,
    AST_STM_PRINTF
  }kind;
  union {
    struct {
      L lval;
      E exp;
    } assign;
    E exp;
    struct {
      E condition;
      S then;
      S elsee;
    } iff;
    struct {
      E condition;
      S body;
    } whilee;
    struct {
      E condition;
      S body;
    } doo;
    struct {
      S header;
      E condition;
      S tail;
      S body;
    } forr;
	struct {
      String_t label;
      //B body;
	  S body;
    } labell;
    String_t gotoo;
	String_t print;
    E returnn;
    B block;
	// for internal function args
	struct {
     InternalFun funKind;
	 List_t args;
	}infuc;
  } u;
  Region_t region;
};

S Ast_Stm_new_assign (L, E, Region_t);
S Ast_Stm_new_assign_dtoi (L lval, E e, Region_t r);
S Ast_Stm_new_assign_itod (L lval, E e, Region_t r);


S Ast_Stm_new_label (String_t labelName, S body, Region_t r,int label_kind);


S Ast_Stm_new_exp (E, Region_t);
S Ast_Stm_new_if (E, S, S, Region_t);
S Ast_Stm_new_while (E, S, Region_t);
S Ast_Stm_new_do (E, S, Region_t);
S Ast_Stm_new_for (S, E, S, S, Region_t);
S Ast_Stm_new_break (Region_t);
S Ast_Stm_new_continue (Region_t);
S Ast_Stm_new_return (E, Region_t);
S Ast_Stm_new_goto (String_t, Region_t);
S Ast_Stm_new_end (Region_t );
S Ast_Stm_new_internal_fun(InternalFun kind,List_t args,Region_t r);



S Ast_Stm_new_block (B);
Box_t Ast_Stm_box (S);
void Ast_Stm_print (S);
S Ast_Stm_new_printf (String_t id, Region_t r);
/* function */
struct F
{
  Id_t type;
  Id_t name;
  /* List<Stt> */
  List_t args;
  B block;
  int attr;
  Region_t region;
};

F Ast_Fun_new (Id_t type, Id_t name, List_t args
               , B b, int attr,Region_t r);
Box_t Ast_Fun_box (F);
void Ast_Fun_print (F);

/* struct fields & function parameters */
/* this should be rewritten into two different types */
struct Stt
{
  Id_t type;
  Id_t var;
};
Stt Ast_Struct_new (Id_t, Id_t);
Box_t Ast_Struct_box (Stt);
void Ast_Struct_print (Stt);

/* type definition */
struct T
{
  enum {TYPE_STRUCT,
        TYPE_ARRAY} kind;
  union {
    /* List<structt> */
    List_t structt;
    Id_t array;
  } u;
};

T Ast_Type_new_struct (List_t);
T Ast_Type_new_array (Id_t);
void Ast_Type_print (T);

struct Td
{
  Id_t name;
  T type;
};

Td Ast_Td_new (Id_t, T);
Box_t Ast_Td_box (Td);
void Ast_Td_print (Td);

struct P
{
  /* List<Td> */
  List_t tds;
  //全局变量
  List_t globals;
  /* List<F> */
  List_t funcs;
};

P Ast_Prog_new (List_t tds, List_t globals ,List_t funcs);
Box_t Ast_Prog_box (P x);
void Ast_Prog_print (P x, File_t file);

#undef P
#undef T
#undef Td
#undef F
#undef S
#undef L
#undef B
#undef E
#undef D
#undef Af
#undef Stt

#undef Id_t
#undef List_t

#endif
