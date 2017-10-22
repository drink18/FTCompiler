#ifndef MACHINE_H
#define MACHINE_H

#include "../lib/linkedList.h"
#include "../lib/string.h"
#include "../lib/bool.h"

#include "../lib/file.h"
#include "../lib/internalFunc.h"


#include "../atoms/atoms.h"

#define List_t LinkedList_t

#define P Machine_Prog_t
#define Str Machine_Str_t
#define GV Machine_Global_Var_t

#define F Machine_Fun_t
#define S Machine_Stm_t
#define O Machine_Operand_t
#define D Machine_Dec_t
#define Stt Machine_Struct_t
#define M Machine_Mask_t

typedef struct O *O;
typedef struct S *S;
typedef struct Stt *Stt;
typedef struct GV *GV;

typedef struct M *M;
typedef struct F *F;
typedef struct Str *Str;
typedef struct D *D;
typedef struct P *P;
typedef struct var_info
{
 String_t boardname;
 int      var_addr;
}var_info_t;
struct O
{
  enum {
    MACHINE_OP_INT,
	MACHINE_OP_FLOAT,
	MACHINE_OP_DOUBLE,
    MACHINE_OP_GLOBAL,
    MACHINE_OP_INSTACK,
    MACHINE_OP_VAR,
    MACHINE_OP_STRUCT,
    MACHINE_OP_ARRAY

  } kind;
  union{
    int intlit;
	float floatlit;
	double doublelit;
    Id_t global;
    int index;
    Id_t var;
    struct {
      /* stack offset */
      int base;
      int index;
      int scale;
    }structt;
    struct {
      /* stack offset */
      int base;
      /* stack offset */
      int offset;
      int scale;
      Id_t gv;
    }array;
  }u;
};

O Machine_Operand_new_int (int i);
O Machine_Operand_new_float (float i);
O Machine_Operand_new_double (double i);

O Machine_Operand_new_global (Id_t id);
O Machine_Operand_new_var (Id_t id);
O Machine_Operand_new_inStack (int index);
O Machine_Operand_new_struct (int base, int index,
                              int scale);
O Machine_Operand_new_array (Id_t id,int base, int offset,
                             int scale);
void Machine_Operand_print (O);






struct S
{
  enum {
    MACHINE_STM_MOVE,
	MACHINE_STM_MOVE_DTOI,
	MACHINE_STM_MOVE_ITOD,
    MACHINE_STM_BOP,
    MACHINE_STM_UOP,
    MACHINE_STM_CALL,
    MACHINE_STM_IF,
    MACHINE_STM_LABEL,
    MACHINE_STM_JUMP,
    MACHINE_STM_RETURN,
    MACHINE_STM_RETURNVOID,
    MACHINE_STM_NEW_STRUCT,
    MACHINE_STM_NEW_ARRAY,
    MACHINE_STM_NEW_PRIMITIVE,
    MACHINE_STM_UPDATE,
    MACHINE_STM_END
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
      O dest;
    } bop;
    struct {
      O src;
      Operator_t op;
      O dest;
    } uop;
    struct {
      O dest;
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
    O returnn;
    Label_t jump;
    struct {
      O dest;
      Id_t type;
      /* List<O> */
      List_t args;
    }newStruct;
    struct {
      O dest;
      Id_t type;
      int isPtr;
      O size;
      O init;
    }newArray;
	struct{
		  InternalFun iFun;
		  List_t args;
		}infuc;

  } u;
};

S Machine_Stm_new_move (O dest, O src);
S Machine_Stm_new_bop (O dest, O left,
                       Operator_t op, O right);
S Machine_Stm_new_uop (O dest, Operator_t opr, O src);
S Machine_Stm_new_call (O dest, Id_t name, List_t args);
S Machine_Stm_new_if (O, Label_t, Label_t);
S Machine_Stm_new_label (Label_t label);
S Machine_Stm_new_jump (Label_t label);
S Machine_Stm_new_return (O);
S Machine_Stm_new_newStruct (O dest, Id_t ty, List_t);
S Machine_Stm_new_newArray (O dest,
                            Id_t type,
                            int isPtr,
                            O size,
                            O arg);
void Machine_Stm_print (S);
S Machine_Stm_new_internalfunc (InternalFun iFun, List_t args);
S Machine_Stm_new_end ();
S Machine_Stm_new_update ();
S Machine_Stm_new_returnvoid();


/* function */
struct F
{
  List_t globals;
  Id_t type;
  Id_t name;
  /* List<Stt> */
  List_t args;
  /* List<Struct_t> */
  List_t decs;
  /* List<Stm_t> */
  List_t stms;
  Id_t retId;
  Label_t entry;
  Label_t exitt;
  bool inter;
};

F Machine_Fun_new (List_t,Id_t, Id_t, List_t, List_t, List_t,
                   Id_t, Label_t, Label_t,bool inter);
void Machine_Fun_print (F);

/* struct fields & function parameters */
/* this should be rewritten into two different types */
struct Stt
{
  Id_t type;
  Id_t var;
};
Stt Machine_Struct_new (Id_t, Id_t);
void Machine_Struct_print (Stt);


struct Str
{
  Id_t name;
  String_t value;
};
Str Machine_Str_new (Id_t, String_t);
void Machine_Str_print (Str);


struct GV
{
  Id_t var;
  String_t type;
  long long int init_value;
  //static: true
  bool var_static;
  int size;
};
GV Machine_global_new (Id_t name, String_t value,long long int init_value,bool sta,int size);


struct M
{
  Id_t name;
  int size;
  /* List<int> */
  List_t index;
};
M Machine_Mask_new (Id_t name, int size, List_t);
void Machine_Mask_print (M);

struct P
{
  /* List<Str> */
  List_t strings;
  /* List<M> */
  List_t masks;
  List_t funcs;
};

P Machine_Prog_new (List_t strings, List_t masks,
                    List_t funcs);
void Machine_Prog_print (P x, File_t f);

#undef P
#undef Str
#undef F
#undef S
#undef O
#undef M
#undef D
#undef Stt
#undef List_t
#undef GV
#endif
