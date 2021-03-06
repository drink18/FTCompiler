#include "../lib/mem.h"
#include "../lib/int.h"
#include "../lib/assert.h"
#include "../lib/hash.h"
#include "../lib/triple.h"
#include "machine.h"

#define List_t LinkedList_t

#define P Machine_Prog_t
#define T Machine_Type_t
#define Str Machine_Str_t
// GV stands for global value
#define GV Machine_Global_Var_t

#define M Machine_Mask_t
#define F Machine_Fun_t
#define S Machine_Stm_t
#define O Machine_Operand_t
#define D Machine_Dec_t
#define Stt Machine_Struct_t


static File_t file = 0;

static void print (String_t s)
{
  fprintf (file, "%s", s);
}

O Machine_Operand_new_int (int i)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_INT;
  e->u.intlit = i;
  return e;
}

O Machine_Operand_new_float (float i)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_FLOAT;
  e->u.floatlit = i;
  return e;
}
O Machine_Operand_new_double (double i)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_DOUBLE;
  e->u.doublelit = i;
  return e;
}


O Machine_Operand_new_global (Id_t id)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_GLOBAL;
  e->u.global = id;
  return e;
}

O Machine_Operand_new_inStack (int index)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_INSTACK;
  e->u.index = index;
  return e;
}

O Machine_Operand_new_var (Id_t id)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_VAR;
  e->u.var = id;
  return e;
}

O Machine_Operand_new_struct (int base, int index,
                              int scale)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_STRUCT;
  e->u.structt.base = base;
  e->u.structt.index = index;
  e->u.structt.scale = scale;
  return e;
}

O Machine_Operand_new_array (Id_t var,int base, int offset,
                             int scale)
{
  O e;
  Mem_NEW (e);
  e->kind = MACHINE_OP_ARRAY;
  e->u.array.base = base;
  e->u.array.offset = offset;
  e->u.array.scale = scale;
  e->u.array.gv = var;
  return e;
}

void Machine_Operand_print (O o)
{
  Assert_ASSERT(o);
  switch (o->kind){
  case MACHINE_OP_INT:
    print (Int_toString (o->u.intlit));
    return;
  case MACHINE_OP_GLOBAL:
    print ("@G_");
    print (Id_toString (o->u.global));
    return;
  case MACHINE_OP_VAR:
    print (Id_toString (o->u.var));
    return;
  case MACHINE_OP_INSTACK:
    print ("@S_");
    print (Int_toString (o->u.index));
    return;
  case MACHINE_OP_STRUCT:
    print ("@S_");
    print (Int_toString (o->u.structt.base));
    print (".");
    print (Int_toString (o->u.structt.index));
    print (".");
    print (Int_toString (o->u.structt.scale));
    return;
  case MACHINE_OP_ARRAY:
    print ("@S_");
    print (Int_toString (o->u.array.base));
    print ("[");
    print ("@S_");
    print (Int_toString (o->u.array.offset));
    print (", ");
    print (Int_toString (o->u.array.scale));
    print ("]");
    return;
  default:
    Error_impossible();
    return;
  }
  Error_impossible();
  return;
}


S Machine_Stm_new_move (O dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_MOVE;
  s->u.move.dest = dest;
  s->u.move.src = src;
  return s;
}
S Machine_Stm_new_move_dtoi (O dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_MOVE_DTOI;
  s->u.move.dest = dest;
  s->u.move.src = src;
  return s;
}
S Machine_Stm_new_move_itod (O dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_MOVE_ITOD;
  s->u.move.dest = dest;
  s->u.move.src = src;
  return s;
}



S Machine_Stm_new_bop (O dest, O left, Operator_t opr,
                       O right)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_BOP;
  s->u.bop.left = left;
  s->u.bop.right = right;
  s->u.bop.op = opr;
  s->u.bop.dest = dest;
  return s;
}

S Machine_Stm_new_uop (O dest, Operator_t opr, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_UOP;
  s->u.uop.dest = dest;
  s->u.uop.src = src;
  s->u.uop.op = opr;
  return s;
}

S Machine_Stm_new_call (O dest, Id_t f, List_t args)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_CALL;
  s->u.call.dest = dest;
  s->u.call.name = f;
  s->u.call.args = args;
  return s;
}

S Machine_Stm_new_internalfunc (InternalFun iFun, List_t args)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_NEW_PRIMITIVE;
  s->u.infuc.iFun = iFun;
  s->u.infuc.args = args;
 // s->u.call.args = args;
  return s;
}
S Machine_Stm_new_end ()
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_END;
 // s->u.call.args = args;
  return s;
}
S Machine_Stm_new_update ()
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_UPDATE;
 // s->u.call.args = args;
  return s;
}


S Machine_Stm_new_if (O src, Label_t t, Label_t f)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_IF;
  s->u.iff.src = src;
  s->u.iff.truee = t;
  s->u.iff.falsee = f;
  return s;
}

S Machine_Stm_new_label (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_LABEL;
  s->u.label = label;
  return s;
}

S Machine_Stm_new_jump (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_JUMP;
  s->u.jump = label;
  return s;
}
S Machine_Stm_new_returnvoid()
{

  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_RETURNVOID;
  return s;

}
S Machine_Stm_new_newStruct (O dest, Id_t type,
                             List_t args)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_NEW_STRUCT;
  s->u.newStruct.dest = dest;
  s->u.newStruct.type = type;
  s->u.newStruct.args = args;
  return s;
}

S Machine_Stm_new_newArray (O dest, Id_t type,
                            int isPtr,
                            O size, O init)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_NEW_ARRAY;
  s->u.newArray.dest = dest;
  s->u.newArray.type = type;
  s->u.newArray.isPtr = isPtr;
  s->u.newArray.size = size;
  s->u.newArray.init = init;
  return s;
}

S Machine_Stm_new_return (O id)
{
  S s;
  Mem_NEW (s);
  s->kind = MACHINE_STM_RETURN;
  s->u.returnn = id;
  return s;
}

static void space2()
{
}

static void space4 ()
{
  print ("\t");
}

void Machine_Stm_print (S s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case MACHINE_STM_MOVE:
  case MACHINE_STM_MOVE_DTOI:
  case MACHINE_STM_MOVE_ITOD:
    space4 ();
    Machine_Operand_print (s->u.move.dest);
    print (" = ");
    Machine_Operand_print (s->u.move.src);
    break;
  case MACHINE_STM_BOP:
    space4 ();
    Machine_Operand_print (s->u.bop.dest);
    print (" = ");
    Machine_Operand_print (s->u.bop.left);
    print (Operator_toString (s->u.bop.op));
    Machine_Operand_print (s->u.bop.right);
    break;
  case MACHINE_STM_UOP:
    space4 ();
    Machine_Operand_print (s->u.uop.dest);
    print (" = ");
    print (Operator_toString (s->u.uop.op));
    Machine_Operand_print (s->u.uop.src);
    break;
  case MACHINE_STM_CALL:{
    LinkedList_t p;
    space4 ();
    Machine_Operand_print (s->u.call.dest);
    print (" = ");
    print (Id_toString (s->u.call.name));
    print ("(");
    p = LinkedList_getFirst (s->u.call.args);
    while (p){
      Machine_Operand_t op = (Machine_Operand_t)p->data;
      Machine_Operand_print(op);
      if (p->next)
        print (", ");
      p = p->next;
    }
    print (")");
    break;
  }
  case MACHINE_STM_IF:
    space4 ();
    print ("if (");
    Machine_Operand_print (s->u.iff.src);
    print ("; ");
    print (Label_toString (s->u.iff.truee));
    print (", ");
    print (Label_toString (s->u.iff.falsee));
    print (")");
    break;
  case MACHINE_STM_LABEL:
    space2();
    print (Label_toString (s->u.label));
    print (":");
    break;
  case MACHINE_STM_JUMP:
    space4 ();
    print ("jump ");
    print (Label_toString (s->u.jump));
    break;
  case MACHINE_STM_NEW_STRUCT:{
    LinkedList_t p =
      LinkedList_getFirst (s->u.newStruct.args);
    space4 ();
    Machine_Operand_print (s->u.newStruct.dest);
    print (" = NEW_STRUCT (");
    print (Id_toString (s->u.newStruct.type));
    print ("; ");
    while (p){
      Machine_Operand_print (p->data);
      if (p->next)
        print (", ");
      p = p->next;
    }
    print (")");
    break;
  }
  case MACHINE_STM_NEW_ARRAY:{
    space4 ();
    Machine_Operand_print (s->u.newArray.dest);
    print (" = NEW_ARRAY (");
    print (Id_toString (s->u.newArray.type));
    print ("; isPtr=");
    print (Int_toString (s->u.newArray.isPtr));
    print (", ");
    Machine_Operand_print (s->u.newArray.size);
    print (", ");
    Machine_Operand_print (s->u.newArray.init);
    print (")");
    break;
  }
  case MACHINE_STM_RETURN:
    space4 ();
    print ("return ");
    Machine_Operand_print (s->u.returnn);
    break;
  default:
    Error_impossible();
    break;
  }
  if (s->kind != MACHINE_STM_LABEL)
    print (";");
  print ("\n");
  return;
}

F Machine_Fun_new (List_t globals, Id_t type, Id_t name, List_t args,
                   List_t decs, List_t stms,
                   Id_t retId,
                   Label_t entry, Label_t exitt,bool inter)
{
  F f;
  Mem_NEW (f);
  f->inter=inter;
  f->globals = globals;
  f->type = type;
  f->name = name;
  f->args = args;
  f->decs = decs;
  f->stms = stms;
  f->retId = retId;
  f->entry = entry;
  f->exitt = exitt;
  return f;
}


static void Machine_Dec_print(Machine_Struct_t);

void Machine_Fun_print (F f)
{
  Assert_ASSERT(f);
  print (Id_toString (f->type));
  print (" ");
  print (Id_toString (f->name));
  print ("(");
  LinkedList_foreach (f->args,
                      (Poly_tyVoid)Machine_Struct_print);
  print (")\n{\n");
  LinkedList_foreach (f->decs,
                      (Poly_tyVoid)Machine_Dec_print);
  print ("\n");
  print ("Fentry = ");
  print (Label_toString (f->entry));
  print (", Fexit = ");
  print (Label_toString (f->exitt));
  print ("\n");
  LinkedList_foreach (f->stms,
                      (Poly_tyVoid)Machine_Stm_print);
  print ("}\n\n");
  return;
}


Stt Machine_Struct_new (Id_t type, Id_t var)
{
  Stt x;
  Mem_NEW (x);
  x->type = type;
  x->var = var;
  return x;
}

void Machine_Struct_print (Stt x)
{
  Assert_ASSERT(x);
  print (Id_toString (x->type));
  print (" ");
  print (Id_toString (x->var));
  print (";\n");
}

static void Machine_Dec_print (Stt x)
{
  Assert_ASSERT(x);
  print ("\t");
  print (Id_toString (x->type));
  print (" ");
  print (Id_toString (x->var));
  print (";\n");
}


Str Machine_Str_new (Id_t name, String_t value)
{
  Str d;
  Mem_NEW (d);
  d->name = name;
  d->value = value;
  return d;
}
GV Machine_global_new (Id_t name, String_t value,long long int init_value,bool sta,int size)
{
  GV d;
  Mem_NEW (d);
  d->var = name;
  d->type = value;
  d->init_value= init_value;
  d->var_static=sta;
  d->size = size;
  return d;
}

void Machine_Str_print (Str s)
{
  print ("string ");
  print (Id_toString (s->name));
  print (" = \"");
  print (s->value);
  print ("\";\n");
  return;
}

M Machine_Mask_new (Id_t name, int size, List_t index)
{
  M m;
  Mem_NEW (m);
  m->name = name;
  m->size = size;
  m->index = index;
  return m;
}

void Machine_Mask_print (M m)
{
  List_t p;

  Assert_ASSERT(m);
  print ("int [] ");
  print (Id_toString (m->name));
  print (" = {");
  print (Int_toString (m->size));
  print ("; ");
  p = LinkedList_getFirst (m->index);
  while (p){
    print (Int_toString ((int)p->data));
    if (p->next)
      print (", ");
    p = p->next;
  }
  print ("};\n");
  return;
}

P Machine_Prog_new (List_t strings, List_t masks,
                    List_t funcs)
{
  P p;
  Mem_NEW (p);
 // p->globals =
  p->strings = strings;
  p->masks = masks;
  p->funcs = funcs;
  return p;
}

void Machine_Prog_print (P x, File_t f)
{
  Assert_ASSERT(x);
  Assert_ASSERT(f);
  file = f;
  LinkedList_foreach (x->strings,
                      (Poly_tyVoid)Machine_Str_print);
  print ("\n");
  LinkedList_foreach (x->masks,
                      (Poly_tyVoid)Machine_Mask_print);
  print ("\n");
  LinkedList_foreach (x->funcs,
                      (Poly_tyVoid)Machine_Fun_print);
}


#undef P
#undef T
#undef Str
#undef M
#undef F
#undef S
#undef D
#undef O
#undef Stt
#undef List_t
#undef GV
