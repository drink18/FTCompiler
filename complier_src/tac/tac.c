#include "../lib/mem.h"
#include "../lib/int.h"
#include "../lib/assert.h"
#include "../lib/todo.h"
#include "tac.h"

#define List_t LinkedList_t

#define P Tac_Prog_t
#define T Tac_Type_t
#define Td Tac_Td_t
#define F Tac_Fun_t
#define S Tac_Stm_t
#define O Tac_Operand_t
#define D Tac_Dec_t
#define Stt Tac_Struct_t
#define Af Tac_Field_t


static File_t file = 0;

static void print (String_t s)
{
  fprintf (file, "%s", s);
}

Af Tac_Field_new (Id_t id, Id_t e)
{
  Af f;
  Mem_NEW (f);
  f->id = id;
  f->e = e;
  return f;
}

void Tac_Field_print (Af f)
{
  print (Id_toString (f->id));
  print (" = ");
  print (Id_toString (f->e));
}

/* expression */
O Tac_Operand_new_int (int i)
{
  O e;
  Mem_NEW (e);
  e->kind = TAC_OP_INT;
  e->u.intlit = i;
  return e;
}
O Tac_Operand_new_float (float i)
{
  O e;
  Mem_NEW (e);
  e->kind = TAC_OP_FLOAT;
  e->u.floatlit = i;
  return e;
}
O Tac_Operand_new_double (double i)
{
  O e;
  Mem_NEW (e);
  e->kind = TAC_OP_DOUBLE;
  e->u.doublelit = i;
  return e;
}

O Tac_Operand_new_string (String_t str)
{
  O e;
  Mem_NEW (e);
  e->kind = TAC_OP_STR;
  e->u.strlit = str;
  return e;
}


O Tac_Operand_new_var (Id_t id)
{
  O e;
  Mem_NEW (e);
  e->kind = TAC_OP_VAR;
  e->u.var = id;
  return e;
}

O Tac_Operand_new_struct (Id_t base, Id_t offset)
{
  O e;
  Mem_NEW (e);
  e->kind = TAC_OP_STRUCT;
  e->u.structt.base = base;
  e->u.structt.offset = offset;
  return e;
}

O Tac_Operand_new_array (Id_t base, Id_t offset)
{
  O e;
  Mem_NEW (e);
  e->kind = TAC_OP_ARRAY;
  e->u.array.base = base;
  e->u.array.offset = offset;
  return e;
}

void Tac_Operand_print (O o)
{
  Assert_ASSERT(o);
  switch (o->kind){
  case TAC_OP_INT:
    print (Int_toString (o->u.intlit));
    return;
  case TAC_OP_STR:
    print ("\"");
    print (o->u.strlit);
    print ("\"");
    return;
  case TAC_OP_VAR:
    print (Id_toString (o->u.var));
    return;
  case TAC_OP_STRUCT:
    print (Id_toString (o->u.structt.base));
    print (".");
    print (Id_toString (o->u.structt.offset));
    return;
  case TAC_OP_ARRAY:
    print (Id_toString (o->u.array.base));
    print ("[");
    print (Id_toString (o->u.array.offset));
    print ("]");
    return;
  default:
    Error_bug ("impossible");
    return;
  }
  Error_bug ("impossible");
  return;
}


S Tac_Stm_new_move (O dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_MOVE;
  s->u.move.dest = dest;
  s->u.move.src = src;
  return s;
}
S Tac_Stm_new_move_itod (O dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_MOVE_ITOD;
  s->u.move.dest = dest;
  s->u.move.src = src;
  return s;
}
S Tac_Stm_new_move_dtoi (O dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_MOVE_DTOI;
  s->u.move.dest = dest;
  s->u.move.src = src;
  return s;
}

S Tac_Stm_new_end()
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_END;
  return s;
}
S Tac_Stm_new_update ()
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_UPDATE;
  return s;
}
S Tac_Stm_new_primitive (InternalFun ifun,List_t t)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_PRIMITIVE;
  s->u.infuc.iFun = ifun;
  s->u.infuc.args = t;
  return s;
}

S Tac_Stm_new_bop (Id_t dest, O left, Operator_t opr,
		   O right)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_BOP;
  s->u.bop.left = left;
  s->u.bop.right = right;
  s->u.bop.op = opr;
  s->u.bop.dest = dest;
  return s;
}

S Tac_Stm_new_uop (Id_t dest, Operator_t opr, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_UOP;
  s->u.uop.dest = dest;
  s->u.uop.src = src;
  s->u.uop.op = opr;
  return s;
}

S Tac_Stm_new_call (Id_t dest, Id_t f, List_t args)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_CALL;
  s->u.call.dest = dest;
  s->u.call.name = f;
  s->u.call.args = args;
  return s;
}


S Tac_Stm_new_if (O src, Label_t t, Label_t f)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_IF;
  s->u.iff.src = src;
  s->u.iff.truee = t;
  s->u.iff.falsee = f;
  return s;
}

S Tac_Stm_new_label (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_LABEL;
  s->u.label = label;
  return s;
}

S Tac_Stm_new_jump (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_JUMP;
  s->u.jump = label;
  return s;
}

S Tac_Stm_new_newStruct (Id_t dest, Id_t type,
			 List_t args)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_NEW_STRUCT;
  s->u.newStruct.dest = dest;
  s->u.newStruct.type = type;
  s->u.newStruct.args = args;
  return s;
}

S Tac_Stm_new_newArray (Id_t dest, Id_t type,
                        O size, O init)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_NEW_ARRAY;
  s->u.newArray.dest = dest;
  s->u.newArray.type = type;
  s->u.newArray.size = size;
  s->u.newArray.init = init;
  return s;
}

S Tac_Stm_new_return (Id_t id)
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_RETURN;
  s->u.returnn = id;
  return s;
}
S Tac_Stm_new_returnvoid ()
{
  S s;
  Mem_NEW (s);
  s->kind = TAC_STM_RETURNVOID;
  return s;
}

static void space2()
{
}

static void space4 ()
{
  print ("\t");
}

void Tac_Stm_print (S s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case TAC_STM_MOVE:
  case TAC_STM_MOVE_ITOD:
  case TAC_STM_MOVE_DTOI:
    space4 ();
    Tac_Operand_print (s->u.move.dest);
    print (" = ");
    Tac_Operand_print (s->u.move.src);
    break;
  case TAC_STM_BOP:
    space4 ();
    print (Id_toString (s->u.bop.dest));
    print (" = ");
    Tac_Operand_print (s->u.bop.left);
    print (Operator_toString (s->u.bop.op));
    Tac_Operand_print (s->u.bop.right);
    break;
  case TAC_STM_UOP:
    space4 ();
    print (Id_toString (s->u.uop.dest));
    print (" = ");
    print (Operator_toString (s->u.uop.op));
    Tac_Operand_print (s->u.uop.src);
    break;
  case TAC_STM_CALL:
    space4 ();
    print (Id_toString (s->u.call.dest));
    print (" = ");
    print (Id_toString (s->u.call.name));
    print ("(");
    LinkedList_foreach (s->u.call.args,
                        (Poly_tyVoid)Tac_Operand_print);
    print (")");
    break;
  case TAC_STM_IF:
    space4 ();
    print ("if (");
    Tac_Operand_print (s->u.iff.src);
    print ("; ");
    print (Label_toString (s->u.iff.truee));
    print (", ");
    print (Label_toString (s->u.iff.falsee));
    print (")");
    break;
  case TAC_STM_LABEL:
    space2();
    print (Label_toString (s->u.label));
    print (":");
    break;
  case TAC_STM_JUMP:
    space4 ();
    print ("jump ");
    print (Label_toString (s->u.jump));
    break;
  case TAC_STM_NEW_STRUCT:{
    LinkedList_t p =
      LinkedList_getFirst (s->u.newStruct.args);
    space4 ();
    print (Id_toString (s->u.newStruct.dest));
    print (" = NEW_STRUCT (");
    print (Id_toString (s->u.newStruct.type));
    print ("; ");
    while (p){
      Tac_Operand_print (p->data);
      if (p->next)
        print (", ");
      p = p->next;
    }
    print (")");
    break;
  }
  case TAC_STM_NEW_ARRAY:{
    space4 ();
    print (Id_toString (s->u.newArray.dest));
    print (" = NEW_ARRAY (");
    print (Id_toString (s->u.newArray.type));
    print ("; ");
    Tac_Operand_print (s->u.newArray.size);
    print (", ");
    Tac_Operand_print (s->u.newArray.init);
    print (")");
    break;
  }
  case TAC_STM_RETURN:
    space4 ();
    print ("return ");
    print (Id_toString (s->u.returnn));
    break;
  default:
    printf ("%d", s->kind);
    Error_bug ("impossible");
    break;
  }
  if (s->kind != TAC_STM_LABEL)
    print (";");
  print ("\n");
  return;
}

F Tac_Fun_new (Id_t type, Id_t name, List_t args,
               List_t decs, List_t stms,
               Id_t retId,
               Label_t entry, Label_t exitt,int attr)
{
  F f;
  Mem_NEW (f);
  f->type = type;
  f->name = name;
  f->args = args;
  f->decs = decs;
  f->stms = stms;
  f->retId = retId;
  f->entry = entry;
  f->exitt = exitt;
  f->attr= attr;
  return f;
}

static void Tac_Dec_print(Tac_Struct_t);

void Tac_Fun_print (F f)
{
  Assert_ASSERT(f);
  print (Id_toString (f->type));
  print (" ");
  print (Id_toString (f->name));
  print ("(");
  LinkedList_foreach (f->args,
                      (Poly_tyVoid)Tac_Struct_print);
  print (")\n{\n");
  LinkedList_foreach (f->decs,
                      (Poly_tyVoid)Tac_Dec_print);
  print ("\n");
  print ("Fentry = ");
  print (Label_toString (f->entry));
  print (", Fexit = ");
  print (Label_toString (f->exitt));
  print ("\n");
  LinkedList_foreach (f->stms,
                      (Poly_tyVoid)Tac_Stm_print);
  print ("}\n\n");
  return;
}


Tac_Struct_t Tac_Struct_new (Id_t type, Id_t var)
{
  Tac_Struct_t x;
  Mem_NEW (x);
  x->type = type;
  x->var = var;
  return x;
}

void Tac_Struct_print (Tac_Struct_t x)
{
  Assert_ASSERT(x);
  print (Id_toString (x->type));
  print (" ");
  print (Id_toString (x->var));
  print (";\n");
}

static void Tac_Dec_print (Tac_Struct_t x)
{
  Assert_ASSERT(x);
  print ("\t");
  print (Id_toString (x->type));
  print (" ");
  print (Id_toString (x->var));
  print (";\n");
}

T Tac_Type_new_struct (List_t structt)
{
  T t;
  Mem_NEW (t);
  t->kind = TAC_TYPE_STRUCT;
  t->u.structt = structt;
  return t;
}

T Tac_Type_new_array (Id_t array)
{
  T t;
  Mem_NEW (t);
  t->kind = TAC_TYPE_ARRAY;
  t->u.array = array;
  return t;
}

void Tac_Type_print (T x)
{
  Assert_ASSERT (x);
  switch (x->kind){
  case TAC_TYPE_STRUCT:
    print ("{");
    LinkedList_foreach (x->u.structt,
                        (Poly_tyVoid)Tac_Struct_print);
    print ("};");
    return;;
  case TAC_TYPE_ARRAY:
    print (Id_toString (x->u.array));
    print ("[];");
    return;
  default:
    printf ("%d\n", x->kind);
    Error_bug ("impossible");
    return;
  }
  Error_bug ("impossible");
  return;
}


Td Tac_Td_new (Id_t name, T type)
{
  Td d;
  Mem_NEW (d);
  d->name = name;
  d->type = type;
  return d;
}

void Tac_Td_print (Td ds)
{
  print ("type ");
  print (Id_toString (ds->name));
  print (" = ");
  Tac_Type_print (ds->type);
  print ("\n");
  return;
}

P Tac_Prog_new (List_t tds, List_t funcs)
{
  P p;
  Mem_NEW (p);
  p->tds = tds;
  p->funcs = funcs;
  return p;
}

void Tac_Prog_print (P x, File_t f)
{
  Assert_ASSERT(x);
  Assert_ASSERT(f);
  file = f;
  LinkedList_foreach (x->tds,
                      (Poly_tyVoid)Tac_Td_print);
  print ("\n");
  LinkedList_foreach (x->funcs,
                      (Poly_tyVoid)Tac_Fun_print);
}

#undef P
#undef T
#undef Td
#undef F
#undef S
#undef D
#undef O
#undef Stt
#undef List_t
