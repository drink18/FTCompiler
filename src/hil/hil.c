#include "../lib/mem.h"
#include "../lib/int.h"
#include "../lib/assert.h"
#include "../control/control.h"
#include "hil.h"

#define List_t LinkedList_t

#define P Hil_Prog_t
#define T Hil_Type_t
#define Ty Hil_Ty_t
#define Td Hil_Td_t
#define F Hil_Fun_t
#define S Hil_Stm_t
#define L Hil_Lval_t
#define E Hil_Exp_t
#define D Hil_Dec_t
#define Stt Hil_Struct_t
#define Af Hil_Field_t

static void spaces ();

static File_t file = 0;

static void print (String_t s)
{
  fprintf (file, "%s", s);
  return;
}

L Hil_Lval_new_var (Id_t var, Id_t ty)
{
  L l;
  Mem_NEW (l);
  l->kind = HIL_LVAL_VAR;
  l->u.var = var;
  l->ty = ty;
  return l;
}

L Hil_Lval_new_dot (L lval, Id_t var, Id_t ty)
{
  L l;
  Mem_NEW (l);
  l->kind = HIL_LVAL_DOT;
  l->u.dot.lval = lval;
  l->u.dot.var = var;
  l->ty = ty;
  return l;
}

L Hil_Lval_new_array (L lval, E e, Id_t ty)
{
  L l;
  Mem_NEW (l);
  l->kind = HIL_LVAL_ARRAY;
  l->u.array.lval = lval;
  l->u.array.exp = e;
  l->ty = ty;
  return l;
}

void Hil_Lval_print (L l)
{
  Assert_ASSERT(l);
  if (Control_showType)
    print ("(");
  switch (l->kind){
  case HIL_LVAL_VAR:
    print (Id_toString (l->u.var));
    break;
  case HIL_LVAL_DOT:
    Hil_Lval_print (l->u.dot.lval);
    print (".");
    print (Id_toString (l->u.dot.var));
    break;
  case HIL_LVAL_ARRAY:
    Hil_Lval_print (l->u.array.lval);
    print ("[");
    Hil_Exp_print (l->u.array.exp);
    print ("]");
    break;
  default:
    Error_bug ("impossible");
    return;
  }
  if (Control_showType){
    print (", @ty=");
    print (Id_toString (l->ty));
    print (")");
  }
  return;
}

/* struct field */
Af Hil_Field_new (Id_t id, E t)
{
  Af e;
  Mem_NEW (e);
  e->id = id;
  e->e = t;
  return e;
}

void Hil_Field_print (Af f)
{
  Assert_ASSERT(f);
  print (Id_toString (f->id));
  print (" = ");
  Hil_Exp_print (f->e);
  print ("; ");
  return;
}

/* expression */
E Hil_Exp_new_bop (Operator_t op,
                   E left,
                   E right)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_BOP;
  e->u.bop.left = left;
  e->u.bop.right = right;
  e->u.bop.op = op;
  e->ty = Id_fromString ("int");
  return e;
}

E Hil_Exp_new_unary (Operator_t op,
                     E x)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_UOP;
  e->u.unary.e = x;
  e->u.unary.op = op;
  e->ty = Id_fromString ("int");
  return e;
}

E Hil_Exp_new_intlit (int i)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_INTLIT;
  e->u.intlit = i;
  e->ty = Id_fromString ("int");
  return e;
}
E Hil_Exp_new_floatlit (float i)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_FLOATLIT;
  e->u.floatlit = i;
  e->ty = Id_fromString ("float");
  return e;
}
E Hil_Exp_new_doublelit (double i)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_DOUBLELIT;
  e->u.doublelit = i;
  e->ty = Id_fromString ("double");
  return e;
}

E Hil_Exp_new_stringlit (String_t s)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_STRINGLIT;
  e->u.stringlit = s;
  e->ty = Id_fromString ("string");
  return e;
}

E Hil_Exp_new_newArray (Id_t id, E size, E init)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_NEW_ARRAY;
  e->u.newArray.type = id;
  e->u.newArray.size = size;
  e->u.newArray.init = init;
  e->ty = id;
  return e;
}

E Hil_Exp_new_newStruct (Id_t id, List_t fields)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_NEW_STRUCT;
  e->u.newStruct.type = id;
  e->u.newStruct.list = fields;
  e->ty = id;
  return e;
}

E Hil_Exp_new_lval (L lval, Id_t ty)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_LVAL;
  e->u.lval = lval;
  e->ty = ty;
  return e;
}

E Hil_Exp_new_call (Id_t f, List_t args, Id_t ty)
{
  E e;
  Mem_NEW (e);
  e->kind = HIL_EXP_CALL;
  e->u.call.f = f;
  e->u.call.args = args;
  e->ty = ty;
  return e;
}

void Hil_Exp_print (E e)
{
  Assert_ASSERT (e);
  printf("[e->kind]=%d\n",e->kind);
  switch (e->kind){
  case HIL_EXP_BOP:
    Hil_Exp_print (e->u.bop.left);
    print (Operator_toString (e->u.bop.op));
    Hil_Exp_print (e->u.bop.right);
    break;
  case HIL_EXP_UOP:
    print (Operator_toString (e->u.unary.op));
    Hil_Exp_print (e->u.unary.e);
    break;
  case HIL_EXP_INTLIT:
    print (Int_toString (e->u.intlit));
    break;
  case HIL_EXP_STRINGLIT:
    print ("\"");
    print (e->u.stringlit);
    print ("\"");
    break;
  case HIL_EXP_NEW_STRUCT:
    print (Id_toString(e->u.newStruct.type));
    print ("{");
    LinkedList_foreach (e->u.newStruct.list,
                        (Poly_tyVoid)Hil_Field_print);
    print ("}");
    break;
  case HIL_EXP_NEW_ARRAY:
    print (Id_toString (e->u.newArray.type));
    print ("[");
    Hil_Exp_print (e->u.newArray.size);
    print ("] of (");
    Hil_Exp_print (e->u.newArray.init);
    print (")");
    break;
  case HIL_EXP_CALL:
    print (Id_toString (e->u.call.f));
    print ("(");
    LinkedList_foreach (e->u.call.args,
                        (Poly_tyVoid)Hil_Exp_print);
    print (")");
    break;
  case HIL_EXP_LVAL:
    Hil_Lval_print (e->u.lval);
    break;
  default:
    printf ("%d", e->kind);
    Error_impossible ();
    break;
  }
  /* to do: output type information,
   * controlled by a flag -type
   */
  return;
}


S Hil_Stm_new_assign (L lval, E e)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_ASSIGN;
  s->u.assign.lval = lval;
  s->u.assign.exp = e;
  return s;
}
S Hil_Stm_new_assign_dtoi (L lval, E e)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_ASSIGN_DTOI;
  s->u.assign.lval = lval;
  s->u.assign.exp = e;
  return s;
}
S Hil_Stm_new_assign_itod (L lval, E e)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_ASSIGN_ITOD;
  s->u.assign.lval = lval;
  s->u.assign.exp = e;
  return s;
}

S Hil_Stm_new_exp (E e)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_EXP;
  s->u.exp = e;
  return s;
}

S Hil_Stm_new_if (E e, List_t t, List_t f)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_IF;
  s->u.iff.condition = e;
  s->u.iff.then = t;
  s->u.iff.elsee = f;
  return s;
}

S Hil_Stm_new_do (E e, List_t t, Label_t lf, Label_t le
                  , List_t padding)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_DO;
  s->u.doo.condition = e;
  s->u.doo.body = t;
  s->u.doo.entryLabel = lf;
  s->u.doo.exitLabel = le;
  s->u.doo.padding = padding;
  return s;
}

S Hil_Stm_new_jump (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_JUMP;
  s->u.jump = label;
  return s;
}

S Hil_Stm_new_return (E e)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_RETURN;
  s->u.returnn = e;
  return s;
}
S Hil_Stm_new_end ()
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_END;
  //s->u.returnn = e;
  return s;
}
S Hil_Stm_new_update ()
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_UPDATE;
  //s->u.returnn = e;
  return s;
}
S Hil_Stm_new_label (Label_t labelName,List_t stm)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_LABEL;
  s->u.label.labelName=labelName;
  s->u.label.s=stm;
   return s;
}

S Hil_Stm_new_goto ()
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_UPDATE;
  //s->u.returnn = e;
  return s;
}

S Hil_Stm_new_internalfunc (InternalFun ifun,List_t t)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_INTERNAL_FUNC;
  s->u.infuc.iFun = ifun;
  s->u.infuc.args = t;
  return s;
}

/*
S Hil_Stm_new_internalfunc (E e)
{
  S s;
  Mem_NEW (s);
  s->kind = HIL_STM_RETURN;
  s->u.returnn = e;
  return s;
}
*/
static int current = 0;
static void indent ()
{
  current += 2;
}

static void unindent ()
{
  current -= 2;
}

static void spaces ()
{
  int i = current;
  while (i){
    print (" ");
    i--;
  }
}

void Hil_Stm_print (S s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case HIL_STM_ASSIGN:
  case HIL_STM_ASSIGN_DTOI:
  case HIL_STM_ASSIGN_ITOD:

    spaces ();
    Hil_Lval_print (s->u.assign.lval);
    print (" = ");
    Hil_Exp_print (s->u.assign.exp);
    print (";");
    break;
  case HIL_STM_EXP:
    spaces ();
    Hil_Exp_print (s->u.exp);
    print (";");
    break;
  case HIL_STM_IF:
    spaces ();
    print ("if (");
    Hil_Exp_print (s->u.iff.condition);
    print (")\n");
    indent ();
    LinkedList_foreach (s->u.iff.then,
                        (Poly_tyVoid)Hil_Stm_print);
    unindent ();
    if (s->u.iff.elsee){
      spaces ();
      print ("else\n");
      indent ();
      LinkedList_foreach (s->u.iff.elsee,
                          (Poly_tyVoid)Hil_Stm_print);
      unindent ();
    }
    break;
  case HIL_STM_DO:
    spaces ();
    print ("do {\n");
    indent ();
    LinkedList_foreach (s->u.doo.body,
                        (Poly_tyVoid)Hil_Stm_print);
    unindent ();
    spaces ();
    print ("} while (");
    Hil_Exp_print (s->u.doo.condition);
    print (", ");
    print ("Lentry=");
    print (Label_toString (s->u.doo.entryLabel));
    print (" Lexit=");
    print (Label_toString (s->u.doo.exitLabel));
    print (", ");
    print ("padding=");
    if (s->u.doo.padding){
      LinkedList_foreach (s->u.doo.padding,
                          (Poly_tyVoid)Hil_Stm_print);
    }
    else print (".");
    print (")");
    break;
  case HIL_STM_JUMP:
    spaces ();
    print ("jump ");
    print (Label_toString (s->u.jump));
    break;
  case HIL_STM_RETURN:
    spaces ();
    print ("return ");
    Hil_Exp_print (s->u.returnn);
    print (";");
    break;
  default:
    printf ("%d", s->kind);
    Error_bug ("impossible");
    break;
  }
  print ("\n");
  return;
}

F Hil_Fun_new (Id_t type, Id_t name, List_t args,
               List_t decs, List_t stms,int attr)
{
  F f;
  Mem_NEW (f);
  f->type = type;
  f->name = name;
  f->args = args;
  f->decs = decs;
  f->stms = stms;
  f->attr=attr;
  return f;
}

void Hil_Fun_print (F f)
{
  Assert_ASSERT(f);
  print (Id_toString (f->type));
  print (" ");
  print (Id_toString (f->name));
  print ("(");
  LinkedList_foreach (f->args,
                      (Poly_tyVoid)Hil_Struct_print);
  print (")\n{\n");
  indent ();
  LinkedList_foreach (f->decs,
                      (Poly_tyVoid)Hil_Struct_print);
  print ("\n");
  LinkedList_foreach (f->stms,
                      (Poly_tyVoid)Hil_Stm_print);
  unindent ();
  print ("}\n\n");
  return;
}


Hil_Struct_t Hil_Struct_new (Id_t type, Id_t var)
{
  Hil_Struct_t x;
  Mem_NEW (x);
  x->type = type;
  x->var = var;
  return x;
}

void Hil_Struct_print (Hil_Struct_t x)
{
  Assert_ASSERT(x);
  spaces ();
  print (Id_toString (x->type));
  print (" ");
  print (Id_toString (x->var));
  print (";\n");
  return;
}

T Hil_Type_new_struct (List_t structt)
{
  T t;
  Mem_NEW (t);
  t->kind = HIL_TYPE_STRUCT;
  t->u.structt = structt;
  return t;
}

T Hil_Type_new_array (Id_t array)
{
  T t;
  Mem_NEW (t);
  t->kind = HIL_TYPE_ARRAY;
  t->u.array = array;
  return t;
}

void Hil_Type_print (T x)
{
  Assert_ASSERT (x);
  switch (x->kind){
  case HIL_TYPE_STRUCT:
    print ("{");
    LinkedList_foreach (x->u.structt,
                        (Poly_tyVoid)Hil_Struct_print);
    print ("};");
    return;;
  case HIL_TYPE_ARRAY:
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


Td Hil_Td_new (Id_t name, T type)
{
  Td d;
  Mem_NEW (d);
  d->name = name;
  d->type = type;
  return d;
}

void Hil_Td_print (Td ds)
{
  print ("type ");
  print (Id_toString (ds->name));
  print (" = ");
  Hil_Type_print (ds->type);
  print ("\n");
  return;
}

P Hil_Prog_new (List_t tds, List_t funcs)
{
  P p;
  Mem_NEW (p);
  p->tds = tds;
  p->funcs = funcs;
  return p;
}

void Hil_Prog_print (P x, File_t f)
{
  Assert_ASSERT(x);
  Assert_ASSERT(f);
  file = f;
  LinkedList_foreach (x->tds,
                      (Poly_tyVoid)Hil_Td_print);
  print ("\n");
  LinkedList_foreach (x->funcs,
                      (Poly_tyVoid)Hil_Fun_print);
}

#undef P
#undef T
#undef Ty
#undef Td
#undef F
#undef S
#undef L
#undef D
#undef E
#undef Stt
#undef List_t
