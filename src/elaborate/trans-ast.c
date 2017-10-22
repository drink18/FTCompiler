#include "../lib/assert.h"
#include "../lib/linkedList.h"
#include "../lib/stack.h"
#include "../lib/error.h"
#include "../lib/string.h"
#include "../lib/tuple.h"
#include "../lib/property.h"
#include "../lib/trace.h"
#include "../atoms/atoms.h"
#include "trans-ast.h"
#include "../lib/mem.h"

#define List_t LinkedList_t

static Stack_t labels = 0;

static Tuple_t pushLabel ()
{
  Label_t entryLabel = Label_new (),
    exitLabel = Label_new ();

  Assert_ASSERT(labels);
  Stack_push (labels, Tuple_new (entryLabel, exitLabel));
  return Tuple_new (entryLabel, exitLabel);
}

static void popLabel ()
{
  Assert_ASSERT(labels);
  Stack_pop (labels);
}

static Tuple_t peekLabel ()
{
  Assert_ASSERT(labels);
  return Stack_top (labels);
}

static Property_t typeProp = 0;
static void propertyInit ()
{
  typeProp = Property_new ((Poly_tyPlist)Id_plist);
}

static void TypeProp_set (Poly_t k, Poly_t v)
{
  Property_set (typeProp, k, v);
}

static Poly_t TypeProp_get (Poly_t k)
{
  Poly_t v = Property_get (typeProp, k);
  if (!v){
    Error_impossible ();
  }
  return v;
}

typedef struct
{
  Hil_Lval_t newLval;
  /* must be some declared type name */
  Id_t newTy;
} Lval_Result_t;

static List_t Elab_block (Ast_Block_t);
static Lval_Result_t Elab_lval (Ast_Lval_t);
static Hil_Exp_t Elab_exp (Ast_Exp_t);
static List_t Elab_stm (Ast_Stm_t);

/* translation of left-value */
static Lval_Result_t Elab_lval (Ast_Lval_t l)
{
  Lval_Result_t bogus = {0, 0};

  Assert_ASSERT(l);
  switch (l->kind){
  case AST_LVAL_VAR:{
    Lval_Result_t result;
    Id_t newId = AstId_toId (l->u.var);
    Id_t ty = TypeProp_get (newId);
    result.newLval =
      Hil_Lval_new_var (newId, ty);
    result.newTy = ty;
    return result;
  }
  case AST_LVAL_DOT:{
    Lval_Result_t result;
    Id_t newVar = AstId_toId (l->u.dot.var);
    Id_t ty = TypeProp_get (newVar);
    Lval_Result_t left = Elab_lval (l->u.dot.lval);
    result.newLval =
      Hil_Lval_new_dot (left.newLval,
                        newVar,
                        ty);
    result.newTy = ty;
    return result;
  }
  case AST_LVAL_ARRAY:{
    Lval_Result_t result;
    Lval_Result_t left = Elab_lval (l->u.array.lval);
    Id_t ty = TypeProp_get (left.newTy);
    result.newLval =
      Hil_Lval_new_array (left.newLval,
                          Elab_exp (l->u.array.exp),
                          ty);
    result.newTy = ty;
    return result;
  }
  default:
    Error_impossible ();
    return bogus;
  }
  Error_impossible ();
  return bogus;
}

static Hil_Field_t Elab_fieldForOne (Ast_Field_t f)
{
  Assert_ASSERT(f);
  return Hil_Field_new (AstId_toId (f->id),
                        Elab_exp (f->e));
}

static Operator_t convertOperator (Ast_Exp_Kind_t k)
{
  switch (k){
  case AST_EXP_ADD:
    return OP_ADD;
  case AST_EXP_SUB:
    return OP_SUB;
  case AST_EXP_TIMES:
    return OP_TIMES;
  case AST_EXP_DIVIDE:
    return OP_DIVIDE;
  case AST_EXP_MODUS:
    return OP_MODUS;
  case AST_EXP_OR:
    return OP_OR;
  case AST_EXP_AND:
    return OP_AND;
  case AST_EXP_EQ:
    return OP_EQ;
  case AST_EXP_NE:
    return OP_NE;
  case AST_EXP_LT:
    return OP_LT;
  case AST_EXP_LE:
    return OP_LE;
  case AST_EXP_GT:
    return OP_GT;
  case AST_EXP_GE:
    return OP_GE;
  case AST_EXP_NOT:
    return OP_NOT;
  case AST_EXP_NEGATIVE:
    return OP_NEG;
  default:
    Error_impossible ();
    return 0;
  };
  Error_impossible ();
  return 0;
}

static Hil_Exp_t Elab_exp (Ast_Exp_t e)
{
  Assert_ASSERT(e);
  switch (e->kind&0xFFFF){
  case AST_EXP_ADD:
  case AST_EXP_SUB:
  case AST_EXP_TIMES:
  case AST_EXP_DIVIDE:
  case AST_EXP_MODUS:
  case AST_EXP_OR:
  case AST_EXP_AND:
  case AST_EXP_LT:
  case AST_EXP_LE:
  case AST_EXP_GT:
  case AST_EXP_GE:
  case AST_EXP_EQ:
  case AST_EXP_NE:{
    Hil_Exp_t result =
      Hil_Exp_new_bop (convertOperator (e->kind&0xFFFF)|(e->kind&0xF0000),
                       Elab_exp (e->u.bop.left),
                       Elab_exp (e->u.bop.right));
    return result;
  }
  case AST_EXP_NOT:
  case AST_EXP_NEGATIVE:{
    Hil_Exp_t result =
      Hil_Exp_new_unary (convertOperator(e->kind),
                         Elab_exp (e->u.unary.e));
    return result;
  }
  case AST_EXP_NULL:{
    return Hil_Exp_new_intlit (0);
  }
  case AST_EXP_INTLIT:{
    return Hil_Exp_new_intlit (e->u.intlit);
  }
  case AST_EXP_FLOATLIT:
  	{
    return Hil_Exp_new_floatlit (e->u.floatlit);
  }
  case AST_EXP_DOUBLELIT:
  	{
    return Hil_Exp_new_doublelit (e->u.doublelit);
  }
  case AST_EXP_STRINGLIT:{
    return Hil_Exp_new_stringlit
      (e->u.stringlit);
  }
  case AST_EXP_NEW_STRUCT:{
    Hil_Exp_t result =
      Hil_Exp_new_newStruct
      (AstId_toId (e->u.newStruct.type),
       LinkedList_map (e->u.newStruct.list,
                       (Poly_tyId)Elab_fieldForOne));
    return result;
  }
  case AST_EXP_NEW_ARRAY:{
    Hil_Exp_t result =
      Hil_Exp_new_newArray
      (AstId_toId (e->u.newArray.type),
       Elab_exp (e->u.newArray.size),
       Elab_exp (e->u.newArray.init));
    return result;
  }
  case AST_EXP_LVAL:{
    Lval_Result_t left = Elab_lval (e->u.lval);
    Hil_Exp_t result =
      Hil_Exp_new_lval (left.newLval, left.newTy);
    return result;
  }
  case AST_EXP_CALL:{
    Id_t fname = AstId_toId (e->u.call.f);
    Id_t ty = (e->u.call.ty)?
      AstId_toId(e->u.call.ty): TypeProp_get (fname);
    Hil_Exp_t result =
      Hil_Exp_new_call
      (fname,
       LinkedList_map (e->u.call.args,
                       (Poly_tyId)Elab_exp),
       ty);
    return result;
  }
  default:
    Error_impossible();
    return 0;
  }
  Error_impossible();
  return 0;
}

static List_t Elab_stm (Ast_Stm_t s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case AST_STM_ASSIGN:
  case AST_STM_ASSIGN_ITOD:
  case AST_STM_ASSIGN_DTOI:	{
  	Hil_Stm_t result;
    Lval_Result_t left =
      Elab_lval (s->u.assign.lval);
	if(s->kind==AST_STM_ASSIGN_ITOD)
     result=
      Hil_Stm_new_assign_itod
      (left.newLval,
       Elab_exp (s->u.assign.exp));
	else if(s->kind==AST_STM_ASSIGN_DTOI)
		result=
      Hil_Stm_new_assign_dtoi
      (left.newLval,
       Elab_exp (s->u.assign.exp));
	else result=
      Hil_Stm_new_assign
      (left.newLval,
       Elab_exp (s->u.assign.exp));
    return LinkedList_list (result, 0);
  }

  case AST_STM_EXP:{
    Hil_Stm_t result =
      Hil_Stm_new_exp
      (Elab_exp (s->u.exp));
    return LinkedList_list (result, 0);
  }
  case AST_STM_IF:{
    Hil_Stm_t result =
      Hil_Stm_new_if (Elab_exp (s->u.iff.condition),
                      Elab_stm (s->u.iff.then),
                      (s->u.iff.elsee)?
                      Elab_stm (s->u.iff.elsee):
                      0);
    return LinkedList_list (result, 0);
  }
  case AST_STM_WHILE:{
    Ast_Stm_t new =
      Ast_Stm_new_if
      (s->u.whilee.condition,
       Ast_Stm_new_do(s->u.whilee.condition,
                      s->u.whilee.body,
                      s->region),
       0,
       s->region);
    return Elab_stm (new);
  }
  case AST_STM_DO:{
    Tuple_t tuple = pushLabel ();
    Hil_Stm_t result =
      Hil_Stm_new_do (Elab_exp (s->u.doo.condition),
                      Elab_stm (s->u.doo.body),
                      Tuple_first (tuple),
                      Tuple_second (tuple),
                      0);
    popLabel ();
    return LinkedList_list (result, 0);
  }
  case AST_STM_FOR:{
    Tuple_t tuple = pushLabel ();
    Hil_Exp_t newCond = Elab_exp (s->u.forr.condition);
    List_t listHeader = Elab_stm (s->u.forr.header);
    List_t listBody = Elab_stm (s->u.forr.body);
    List_t listTail = Elab_stm (s->u.forr.tail);
    Hil_Stm_t thenStm =
      Hil_Stm_new_do (newCond,
                      listBody,
                      Tuple_first (tuple),
                      Tuple_second (tuple),
                      listTail);
    Hil_Stm_t newStm =
      Hil_Stm_new_if (newCond,
                      LinkedList_list (thenStm, 0),
                      (List_t)0);
    popLabel ();
    LinkedList_insertLast (listHeader, newStm);
    return listHeader;
  }
  case AST_STM_BREAK:{
    Tuple_t tuple = peekLabel ();
    Label_t exitLabel = Tuple_second (tuple);
    return LinkedList_list (Hil_Stm_new_jump (exitLabel),
                            0);
  }
  case AST_STM_CONTINUE:{
    Tuple_t tuple = peekLabel ();
    Label_t entryLabel = Tuple_first (tuple);
    return LinkedList_list (Hil_Stm_new_jump (entryLabel),
                            0);
  }
  case AST_STM_RETURN:{
    Hil_Stm_t new =
      Hil_Stm_new_return (Elab_exp (s->u.returnn));
    return LinkedList_list (new, 0);
  }
  case AST_STM_END:
  		{
   Hil_Stm_t new =
      Hil_Stm_new_end ();
    return LinkedList_list (new, 0);
  	}
  case AST_STM_GOTO:
  		{
	Label_t gotoLabel=Label_new2(s->u.labell.label);

	//Mem_NEW (gotoLabel);
   Hil_Stm_t new =
       Hil_Stm_new_jump (gotoLabel) ;
    return LinkedList_list (new, 0);
  	}
  case AST_STM_UPDATE:
  		{
  	Hil_Stm_t new =
      Hil_Stm_new_update ();
    return LinkedList_list (new, 0);
  	}
  case AST_STM_BLOCK:
    return Elab_block (s->u.block);
  case AST_STM_INTERNAL_FUNC:{
  	  Hil_Stm_t new =
      Hil_Stm_new_internalfunc
      (s->u.infuc.funKind,
       LinkedList_map (s->u.infuc.args,
                       (Poly_tyId)Elab_exp));
    return LinkedList_list (new, 0);
  	}
  /**wait for develop**/
   case AST_STM_LABEL:
   	{
  	Hil_Stm_t new =
      Hil_Stm_new_label (Label_new2(s->u.labell.label),Elab_stm(s->u.labell.body));
    return LinkedList_list (new, 0);
  	}
  default:
    Error_impossible();
    return 0;
  }
  Error_impossible();
  return 0;
}

static List_t Elab_block (Ast_Block_t b)
{
  /* result: List<List<Hil_Stm_t>>
   * stms:   List<Hil_Stm_t>
   */
  List_t result, p, stms = LinkedList_new ();

  Assert_ASSERT(b);
  result = LinkedList_map (b->stms,
                           (Poly_tyId)Elab_stm);
  p = LinkedList_getFirst (result);
  while (p){
    List_t tmp;

    tmp = (List_t)p->data;
    tmp = LinkedList_getFirst (tmp);
    while (tmp){
      LinkedList_insertLast (stms, tmp->data);
      tmp = tmp->next;
    }
    p = p->next;
  }
  return stms;
}

static Hil_Struct_t Elab_oneArg (Ast_Struct_t s)
{
  Id_t type;
  Id_t var;
  Assert_ASSERT(s);
  type = AstId_toId (s->type);
  var = AstId_toId (s->var);
  Property_set (typeProp,
                var,
                type);
  return Hil_Struct_new (type, var);
}

static Hil_Fun_t Elab_funForOne (Ast_Fun_t f)
{
  List_t args, decs, stms;
  Id_t newName, newType;

  Assert_ASSERT (f);
  newName = AstId_toId (f->name);
  newType = AstId_toId (f->type);
  TypeProp_set (newName, newType);
  args = LinkedList_map
    (f->args,
     (Poly_tyId)Elab_oneArg);
  decs = LinkedList_map
    (f->block->decs,
     (Poly_tyId)Elab_oneArg);
  stms = Elab_block (f->block);
  return Hil_Fun_new (newType,
                      newName,
                      args,
                      decs,
                      stms,
                      f->attr
                      );
}


static List_t Elab_funDecs (List_t fs)
{
  List_t list;

  Assert_ASSERT(fs);
  list = LinkedList_map
    (fs,
     (Poly_tyId)Elab_funForOne);
  return list;
}

static Hil_Struct_t Elab_checkOneField (Ast_Struct_t x)
{
  Id_t type;
  Id_t var;

  Assert_ASSERT(x);
  type = AstId_toId (x->type);
  var = AstId_toId (x->var);
  TypeProp_set (var, type);
  return Hil_Struct_new (type, var);
}

static Hil_Td_t Elab_typeDecsForOne (Ast_Td_t td)
{
  AstId_t name;
  Id_t newName;
  Ast_Type_t type;
  Hil_Type_t newType;

  name = td->name;
  newName = AstId_toId (name);
  type = td->type;
  switch (type->kind){
  case TYPE_STRUCT:{
    List_t new;
    new = LinkedList_map
      (type->u.structt,
       (Poly_tyId)Elab_checkOneField);
    newType = Hil_Type_new_struct (new);
    break;
  }
  case TYPE_ARRAY:{
    Id_t newId = AstId_toId (type->u.array);
    TypeProp_set (newName, newId);
    newType = Hil_Type_new_array (newId);
    break;
  }
  default:
    Error_impossible ();
    return 0;
  }
  return Hil_Td_new (newName, newType);
}

static List_t Elab_typeDecs (List_t tdecs)
{
  Assert_ASSERT(tdecs);
  return LinkedList_map
    (tdecs,
     (Poly_tyId)Elab_typeDecsForOne);
}

static Hil_Prog_t Elaborate_ast_traced (Ast_Prog_t p)
{
  List_t tds, funcs;

  Assert_ASSERT(p);
  labels = Stack_new ();
  propertyInit ();
 LinkedList_map
    (p->globals,
     (Poly_tyId)Elab_oneArg);
  tds = Elab_typeDecs (p->tds);
  funcs = Elab_funDecs (p->funcs);
  return Hil_Prog_new (tds, funcs);
}

static void Trace_arg (Ast_Prog_t p)
{
  Ast_Prog_print (p, stdout);
  return;
}

static void Trace_result (Hil_Prog_t p)
{
  Hil_Prog_print (p, stdout);
  return;
}

Hil_Prog_t Trans_ast (Ast_Prog_t p)
{
  Hil_Prog_t r;
  Trace_TRACE("Trans_ast",
              Elaborate_ast_traced,
              (p),
              Trace_arg,
              r,
              Trace_result);
  return r;
}

#undef List_t
