#include "../lib/mem.h"
#include "../lib/int.h"
#include "../lib/double.h"
#include "../lib/assert.h"
#include "../lib/todo.h"
#include "../lib/string.h"

#include "ast.h"

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
#define D Ast_Dec_t
#define Af Ast_Field_t

static void spaces ();

static File_t file = 0;

static void print (String_t s)
{
  fprintf (file, "%s", s);
  return;
}

L Ast_Lval_new_var (Id_t var, Region_t r)
{
  L l;
  Mem_NEW (l);
  l->kind = AST_LVAL_VAR;
  l->u.var = var;
  l->region = r;
  return l;
}

L Ast_Lval_new_dot (L lval, Id_t var, Region_t r)
{
  L l;
  Mem_NEW (l);
  l->kind = AST_LVAL_DOT;
  l->u.dot.lval = lval;
  l->u.dot.var = var;
  l->region = r;
  return l;
}

L Ast_Lval_new_array (L lval, E e, Region_t r)
{
  L l;
  Mem_NEW (l);
  l->kind = AST_LVAL_ARRAY;
  l->u.array.lval = lval;
  l->u.array.exp = e;
  l->region = r;
  return l;
}

void Ast_Lval_print (L l)
{
  Assert_ASSERT(l);
  switch (l->kind){
  case AST_LVAL_VAR:
    print (AstId_toString (l->u.var));
    break;
  case AST_LVAL_DOT:
    Ast_Lval_print (l->u.dot.lval);
    print (".");
    print (AstId_toString (l->u.dot.var));
    break;
  case AST_LVAL_ARRAY:
    Ast_Lval_print (l->u.array.lval);
    print ("[");
    Ast_Exp_print (l->u.array.exp);
    print ("]");
    break;
  default:
    Error_bug ("impossible");
    break;
  }
}

/* struct field */
Af Ast_Field_new (Id_t id, E t)
{
  Af e;
  Mem_NEW (e);
  e->id = id;
  e->e = t;
  return e;
}

void Ast_Field_print (Af f)
{
  Assert_ASSERT(f);
  print (AstId_toString (f->id));
  print (" = ");
  Ast_Exp_print (f->e);
  print ("; ");
  return;
}

/* expression */
E Ast_Exp_new_bop (int kind,
                   E left,
                   E right,
                   Region_t r)
{
  E e;
  Mem_NEW (e);
  e->kind = kind;

  e->u.bop.left = left;
  e->u.bop.right = right;
  e->region = r;
  return e;
}

E Ast_Exp_new_unary (Ast_Exp_Kind_t kind,
                     E x,
                     Region_t r)
{
  E e;
  Mem_NEW (e);
  e->kind = kind;
  e->u.unary.e = x;
  e->region = r;
  return e;
}


E Ast_Exp_new_null ()
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_NULL;
  return e;
}

E Ast_Exp_new_intlit (int s)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_INTLIT;
  e->u.intlit = s;
  return e;
}
E Ast_Exp_new_intlit_value (int s)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_INTLIT;
  e->u.intlit = s;
  return e;
}

E Ast_Exp_new_floatlit (float s)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_FLOATLIT;
  e->u.floatlit = s;
  return e;
}
E Ast_Exp_new_doublelit_value (double s)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_DOUBLELIT;
  e->u.doublelit = s;
  return e;
}

E Ast_Exp_new_stringlit (String_t s)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_STRINGLIT;
  e->u.stringlit = s;
  return e;
}

E Ast_Exp_new_newArray (Id_t id, E size, E init)
{
  E e;
  Mem_NEW (e);

  e->kind = AST_EXP_NEW_ARRAY;
  e->u.newArray.type = id;
  e->u.newArray.size = size;
  e->u.newArray.init = init;
  return e;
}

E Ast_Exp_new_newStruct (Id_t id, List_t fields)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_NEW_STRUCT;
  e->u.newStruct.type = id;
  e->u.newStruct.list = fields;
  return e;
}

E Ast_Exp_new_lval (L lval)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_LVAL;
  e->u.lval = lval;
  return e;
}

E Ast_Exp_new_call (Id_t f, List_t args, Id_t ty)
{
  E e;
  Mem_NEW (e);
  e->kind = AST_EXP_CALL;
  e->u.call.f = f;
  e->u.call.ty = ty;
  e->u.call.args = args;
  return e;
}

void Ast_Exp_print (E e)
{
  Assert_ASSERT (e);
  switch (e->kind){
  case AST_EXP_ADD:
    Ast_Exp_print (e->u.bop.left);
    print (" + ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_SUB:
    Ast_Exp_print (e->u.bop.left);
    print (" - ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_TIMES:
    Ast_Exp_print (e->u.bop.left);
    print (" * ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_DIVIDE:
    Ast_Exp_print (e->u.bop.left);
    print (" / ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_MODUS:
    Ast_Exp_print (e->u.bop.left);
    print (" %% ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_OR:
    Ast_Exp_print (e->u.bop.left);
    print (" || ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_AND:
    Ast_Exp_print (e->u.bop.left);
    print (" && ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_EQ:
    Ast_Exp_print (e->u.bop.left);
    print (" == ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_NE:
    Ast_Exp_print (e->u.bop.left);
    print (" != ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_LT:
    Ast_Exp_print (e->u.bop.left);
    print (" < ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_LE:
    Ast_Exp_print (e->u.bop.left);
    print (" <= ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_GT:
    Ast_Exp_print (e->u.bop.left);
    print (" > ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_GE:
    Ast_Exp_print (e->u.bop.left);
    print (" >= ");
    Ast_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_NOT:
    print (" ! ");
    Ast_Exp_print (e->u.unary.e);
    return;
  case AST_EXP_NEGATIVE:
    print (" - ");
    Ast_Exp_print (e->u.unary.e);
    return;
  case AST_EXP_NULL:
    print (" null ");
    return;
  case AST_EXP_INTLIT:
    print (Int_toString (e->u.intlit));
    return;
  case AST_EXP_DOUBLELIT:
    print (Double_toString (e->u.doublelit));
    return;
  case AST_EXP_STRINGLIT:
    print ("\"");
    print (e->u.stringlit);
    print ("\"");
    return;
  case AST_EXP_NEW_STRUCT:
    print (AstId_toString(e->u.newStruct.type));
    print ("{");
    LinkedList_foreach (e->u.newStruct.list,
                        (Poly_tyVoid)Ast_Field_print);
    print ("}");
    return;
  case AST_EXP_NEW_ARRAY:
    print (AstId_toString (e->u.newArray.type));
    print ("[");
    Ast_Exp_print (e->u.newArray.size);
    print ("] of (");
    Ast_Exp_print (e->u.newArray.init);
    print (")");
    return;
  case AST_EXP_CALL:
    print (AstId_toString (e->u.call.f));
    print ("(");
    LinkedList_foreach (e->u.call.args,
                        (Poly_tyVoid)Ast_Exp_print);
    print (")");
    return;
  case AST_EXP_LVAL:
    Ast_Lval_print (e->u.lval);
    return;
  default:
    printf ("%d", e->kind);
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}


S Ast_Stm_new_assign (L lval, E e, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_ASSIGN;
  s->u.assign.lval = lval;
  s->u.assign.exp = e;
  s->region = r;
  return s;
}
S Ast_Stm_new_assign_dtoi (L lval, E e, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_ASSIGN_DTOI;
  s->u.assign.lval = lval;
  s->u.assign.exp = e;
  s->region = r;
  return s;
}
S Ast_Stm_new_assign_itod (L lval, E e, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_ASSIGN_ITOD;
  s->u.assign.lval = lval;
  s->u.assign.exp = e;
  s->region = r;
  return s;
}


S Ast_Stm_new_label (String_t labelName, S body, Region_t r,int label_kind)
{
  S s;
  Mem_NEW (s);
  if(label_kind)
  s->kind = AST_STM_RESERVED_LABEL;
  else
  s->kind = AST_STM_LABEL;
  s->u.labell.label = labelName;
  s->u.labell.body = body;
  s->region = r;
  return s;
}
label_for_boards_t Ast_new_lb()
{
 label_for_boards_t s;
 Mem_NEW (s);
 return s;
}

S Ast_Stm_new_exp (E e, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_EXP;
  s->u.exp = e;
  s->region = r;
  return s;
}

S Ast_Stm_new_if (E e, S t, S f, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_IF;
  s->u.iff.condition = e;
  s->u.iff.then = t;
  s->u.iff.elsee = f;
  s->region = r;
  return s;
}

S Ast_Stm_new_while (E e, S t, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_WHILE;
  s->u.whilee.condition = e;
  s->u.whilee.body = t;
  s->region = r;
  return s;
}

S Ast_Stm_new_do (E e, S t, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_DO;
  s->u.doo.condition = e;
  s->u.doo.body = t;
  s->region = r;
  return s;
}

S Ast_Stm_new_for (S header, E e, S tail, S body, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_FOR;
  s->u.forr.header = header;
  s->u.forr.condition = e;
  s->u.forr.tail = tail;
  s->u.forr.body = body;
  s->region = r;
  return s;
}

S Ast_Stm_new_break (Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_BREAK;
  s->region = r;
  return s;
}

S Ast_Stm_new_continue (Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_CONTINUE;
  s->region = r;
  return s;
}
S Ast_Stm_new_end (Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_END;
  s->region = r;
  return s;
}
S Ast_Stm_new_update (Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_UPDATE;
  s->region = r;
  return s;
}

S Ast_Stm_new_return (E e, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_RETURN;
  s->u.returnn = e;
  s->region = r;
  return s;
}
S Ast_Stm_new_internal_fun(InternalFun kind,List_t args,Region_t r)
{

 S s;
  Mem_NEW (s);
  s->kind = AST_STM_INTERNAL_FUNC;
  s->u.infuc.funKind=kind;
  s->u.infuc.args= args;
  s->region = r;
  return s;

}
S Ast_Stm_new_goto (String_t id, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_GOTO;
  s->u.gotoo=id;
  s->region = r;
  return s;
}

S Ast_Stm_new_printf (String_t id, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_PRINTF;
  s->u.print=id;
  s->region = r;
  return s;
}

S Ast_Stm_new_internalfunc (InternalFun funcKind,List_t args, Region_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_INTERNAL_FUNC;
  s->u.infuc.funKind=funcKind;
  s->u.infuc.args=args;
  s->region = r;
  return s;
}

S Ast_Stm_new_block (B b)
{
  S s;
  Mem_NEW (s);
  s->kind = AST_STM_BLOCK;
  s->u.block = b;
  return s;
}

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

Box_t Ast_Stm_box (S s)
{
  return Box_str ("<junk>;");
}

void Ast_Stm_print (S s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case AST_STM_ASSIGN:{
    spaces ();
    Ast_Lval_print (s->u.assign.lval);
    print (" = ");
    Ast_Exp_print (s->u.assign.exp);
    print (";");
    break;
  }
  case AST_STM_EXP:{
    spaces ();
    Ast_Exp_print (s->u.exp);
    print (";");
    break;
  }
  case AST_STM_IF:
    spaces ();
    print ("if (");
    Ast_Exp_print (s->u.iff.condition);
    print (")\n");
    indent ();
    Ast_Stm_print (s->u.iff.then);
    unindent ();
    if (s->u.iff.elsee){
      spaces ();
      print ("else\n");
      indent ();
      Ast_Stm_print (s->u.iff.elsee);
      unindent ();
    }
    break;
  case AST_STM_WHILE:
    spaces ();
    print ("while (");
    Ast_Exp_print (s->u.whilee.condition);
    print (")\n");
    indent ();
    Ast_Stm_print (s->u.whilee.body);
    unindent ();
    break;
  case AST_STM_DO:
    spaces ();
    print ("do {\n");
    indent ();
    Ast_Stm_print (s->u.doo.body);
    unindent ();
    spaces ();
    print ("} while (");
    Ast_Exp_print (s->u.doo.condition);
    break;
  case AST_STM_FOR:
    spaces ();
    print ("for (");
    Ast_Stm_print (s->u.forr.header);
    print ("; ");
    Ast_Exp_print (s->u.forr.condition);
    print ("; ");
    Ast_Stm_print (s->u.forr.tail);
    print (")");
    Ast_Stm_print (s->u.forr.body);
    break;
  case AST_STM_BREAK:
    spaces ();
    print ("break;");
    break;
  case AST_STM_CONTINUE:
    spaces ();
    print ("continue;");
    break;
  case AST_STM_RETURN:
    spaces ();
    print ("return ");
    Ast_Exp_print (s->u.returnn);
    print (";");
    break;
  case AST_STM_BLOCK:
    Ast_Block_print (s->u.block);
    break;
  default:
    printf ("%d", s->kind);
    Error_impossible ();
    break;
  }
  print ("\n");
  return;
}

/* declarations */
D Ast_Dec_new (Id_t type, Id_t var, E init,bool sta)
{
  D d;
  Mem_NEW (d);
  d->type = type;
  d->var = var;
  d->init = init;
  d->sta=sta;
  return d;
}

D Ast_Dec_looup (List_t l, Id_t var)
{
  D d;
  List_t p=l->next;
  while(p)
  	{
  	   d=(D)p->data;
	   if(AstId_equals(var,d->var))
	   	 return d;

       p=p->next;
  }
  return 0;
}
long long  int  Ast_E_int (E e)
{


  long long int a;
  if(!e) return 0;
  switch(e->kind)
   {
   case AST_EXP_INTLIT:
   	    a=e->u.intlit;
   	    break;
   case AST_EXP_FLOATLIT :
   	    a=*(int *)&(e->u.floatlit);
   	    break;
  case AST_EXP_DOUBLELIT :
   	    a=*(long long int  *)&(e->u.doublelit);
   	    break;
   default:
   	 a=0;
  }
  return a;
}

Box_t Ast_Dec_box (D d)
{
  Assert_ASSERT (d);

  return Box_h (Box_str (AstId_toString (d->type)),
                Box_str (AstId_toString (d->var)),
                (d->init)?
                (Box_str (" = <junk>;")):
                Box_str (";"),
                0);
}

void Ast_Dec_print (D d)
{
  Assert_ASSERT (d);
  spaces ();
  print (AstId_toString (d->type));
  print (" ");
  print (AstId_toString (d->var));
  if (d->init){
    print (" = ");
    Ast_Exp_print (d->init);
  }
  print (";\n");
  return;
}

B Ast_Block_new (List_t decs, List_t stms)
{
  B b;
  Mem_NEW (b);
  b->decs = decs;
  b->stms = stms;
  return b;
}

Box_t Ast_Block_box (B b)
{
  Box_t b1, b2, b3;

  Assert_ASSERT(b);
  b1 = Box_str ("{");
  b2 = Box_vlist (LinkedList_map (b->decs,
                                  (Poly_tyId)Ast_Dec_box));
  b3 = Box_vlist (LinkedList_map (b->stms,
                                  (Poly_tyId)Ast_Stm_box));
  b2 = Box_v (b2, b3, 0);
  b3 = Box_str ("}");
  return Box_v (b1, Box_indent (b2, 2), b3, 0);
}

void Ast_Block_print (B b)
{
  Assert_ASSERT(b);
  spaces ();
  print ("{\n");
  indent ();
  LinkedList_foreach (b->decs, (Poly_tyVoid)Ast_Dec_print);
  LinkedList_foreach (b->stms, (Poly_tyVoid)Ast_Stm_print);
  unindent ();
  spaces ();
  print ("}");
  return;
}

F Ast_Fun_new (Id_t type, Id_t name, List_t args, B block,int attr,Region_t r)
{
  F f;
  Mem_NEW (f);
  f->type = type;
  f->name = name;
  f->args = args;
  f->block = block;
  f->region = r;
  f->attr=attr;
  return f;
}

Box_t Ast_Fun_box (F f)
{
  Assert_ASSERT(f);
  Box_t b1 = Box_h
    (Box_str(AstId_toString (f->type)),
     Box_str(AstId_toString (f->name)),
     Box_str ("("),
     Box_hlist (LinkedList_map
                (f->args,
                 (Poly_tyId)Ast_Struct_box)),
     Box_str (")"),
     0);
  Box_t b2 = Ast_Block_box (f->block);
  return Box_v (b1, b2, 0);
}

void Ast_Fun_print (F f)
{
  Assert_ASSERT(f);
  print (AstId_toString (f->type));
  print (" ");
  print (AstId_toString (f->name));
  print ("(");
  LinkedList_foreach (f->args,
                      (Poly_tyVoid)Ast_Struct_print);
  print (")\n");
  Ast_Block_print (f->block);
  print ("\n\n");
  return;
}

Ast_Struct_t Ast_Struct_new (Id_t type, Id_t var)
{
  Ast_Struct_t x;
  Mem_NEW (x);
  x->type = type;
  x->var = var;
  return x;
}

Box_t Ast_Struct_box (Ast_Struct_t x)
{
  Assert_ASSERT(x);
  return Box_str (String_concat
                  (AstId_toString (x->type),
                   " ",
                   AstId_toString (x->var),
                   ";",
                   0));
}

void Ast_Struct_print (Ast_Struct_t x)
{
  Assert_ASSERT(x);
  spaces ();
  print (AstId_toString (x->type));
  print (" ");
  print (AstId_toString (x->var));
  print (";\n");
}

T Ast_Type_new_struct (List_t structt)
{
  T t;
  Mem_NEW (t);
  t->kind = TYPE_STRUCT;
  t->u.structt = structt;
  return t;
}

T Ast_Type_new_array (Id_t array)
{
  T t;
  Mem_NEW (t);
  t->kind = TYPE_ARRAY;
  t->u.array = array;
  return t;
}

Box_t Ast_Type_box (T x)
{
  Assert_ASSERT (x);
  switch (x->kind){
  case TYPE_STRUCT:{
    Box_t b =
      Box_h
      (Box_str ("{"),
       Box_hlist (LinkedList_map
                  (x->u.structt,
                   (Poly_tyId)Ast_Struct_box)),
       Box_str ("}"),
       0);
    return b;
  }
  case TYPE_ARRAY:
    return Box_str (String_concat
                    (AstId_toString (x->u.array),
                     "[]",
                     0));
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

void Ast_Type_print (T x)
{
  Assert_ASSERT (x);
  switch (x->kind){
  case TYPE_STRUCT:
    print ("{");
    LinkedList_foreach (x->u.structt,
                        (Poly_tyVoid)Ast_Struct_print);
    print ("};");
    return;
  case TYPE_ARRAY:
    print (AstId_toString (x->u.array));
    print ("[];");
    return;
  default:
    Error_bug ("impossible");
    return;
  }
  Error_bug ("impossible");
  return;
}


Td Ast_Td_new (Id_t name, T type)
{
  Td d;
  Mem_NEW (d);
  d->name = name;
  d->type = type;
  return d;
}

Box_t Ast_Td_box (Td ds)
{
  return Box_h (Box_str (String_concat
                         ("type ",
                          AstId_toString (ds->name),
                          " = ",
                          0)),
                Ast_Type_box (ds->type),
                Box_str (";"),
                0);
}

void Ast_Td_print (Td ds)
{
  print ("type ");
  print (AstId_toString (ds->name));
  print (" = ");
  Ast_Type_print (ds->type);
  print ("\n");
  return;
}

P Ast_Prog_new (List_t tds, List_t globals ,List_t funcs)
{
  P p;
  Mem_NEW (p);
  p->tds = tds;
  p->globals=globals;
  p->funcs = funcs;
  return p;
}

Box_t Ast_Prog_box (P x)
{
  LinkedList_t boxes =
    LinkedList_map (x->tds,
                    (Poly_tyId)Ast_Td_box);
  LinkedList_t boxes2 =
    LinkedList_map (x->funcs,
                    (Poly_tyId)Ast_Fun_box);
  LinkedList_append (boxes, boxes2);
  return Box_vlist (boxes);
}

void Ast_Prog_print (P x, File_t f)
{
  Assert_ASSERT(x);
  file = f;
  LinkedList_foreach (x->tds,
                      (Poly_tyVoid)Ast_Td_print);
  print ("\n");
  LinkedList_foreach (x->funcs,
                      (Poly_tyVoid)Ast_Fun_print);
}

#undef P
#undef T
#undef Td
#undef F
#undef S
#undef L
#undef B
#undef D
#undef E

#undef Id_t
#undef List_t
