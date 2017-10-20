#include "../lib/assert.h"
#include "../lib/linkedList.h"
#include "../lib/error.h"
#include "../atoms/atoms.h"
#include "trans-hil.h"

#define List_t LinkedList_t
struct FunInfo_t
{
  Id_t retId;
  Label_t entryLabel;
  Label_t exitLabel;
};

static struct FunInfo_t fun = {0, 0, 0};

static LinkedList_t allStms = 0;

static void emit (Tac_Stm_t s)
{
  Assert_ASSERT(allStms);
  LinkedList_insertLast (allStms, s);
}

static LinkedList_t getBeforeClear ()
{
  LinkedList_t t = allStms;
  allStms = LinkedList_new ();
  return t;
}

static LinkedList_t allDecs = 0;

static Id_t genDec (Id_t ty)
{
  Id_t id;
  Tac_Struct_t new;

  Assert_ASSERT(ty);
  id = Id_newNoName ();
  new = Tac_Struct_new (ty, id);
  LinkedList_insertLast (allDecs, new);
  return id;
}

static List_t getDecs ()
{
  List_t t = allDecs;
  allDecs = LinkedList_new ();
  return t;
}

static Tac_Operand_t Elab_exp (Hil_Exp_t);

static Tac_Operand_t Elab_lval (Hil_Lval_t l)
{
  Assert_ASSERT(l);
  switch (l->kind){
  case HIL_LVAL_VAR:
    return Tac_Operand_new_var (l->u.var);
  case HIL_LVAL_DOT:{
    Tac_Operand_t left = Elab_lval (l->u.dot.lval);
    Assert_ASSERT(left);
    switch (left->kind){
    case TAC_OP_VAR:{
      return Tac_Operand_new_struct (left->u.var,
                                     l->u.dot.var);
    }
    case TAC_OP_STRUCT:
    case TAC_OP_ARRAY:{
      Id_t new = genDec (l->u.dot.lval->ty);
      emit (Tac_Stm_new_move
            (Tac_Operand_new_var (new),
             left));
      return Tac_Operand_new_struct (new,
                                     l->u.dot.var);
    }
    default:
      Error_impossible ();
      return 0;
    }
    Error_impossible ();
    return 0;
  }
  case HIL_LVAL_ARRAY:{
    Tac_Operand_t left = Elab_lval (l->u.array.lval);
    Tac_Operand_t newExp = Elab_exp (l->u.array.exp);
    Id_t eid = genDec (l->u.array.exp->ty);
    emit (Tac_Stm_new_move (Tac_Operand_new_var (eid),
                            newExp));
    Assert_ASSERT(left);
    switch (left->kind){
    case TAC_OP_VAR:
      return Tac_Operand_new_array (left->u.var,
                                    eid);
    case TAC_OP_STRUCT:
    case TAC_OP_ARRAY:{
      Id_t new = genDec (l->u.array.lval->ty);
      emit (Tac_Stm_new_move
            (Tac_Operand_new_var (new),
             left));
      return Tac_Operand_new_array (new, eid);
    }
    default:
      Error_impossible ();
      return 0;
    }
    Error_impossible ();
    return 0;
  }
  default:
    Error_impossible();
    return 0;
  }
  Error_impossible();
  return 0;
}

static Tac_Operand_t Elab_bop (Hil_Exp_t left,
                               Operator_t op,
                               Hil_Exp_t right,
                               Id_t ty)
{
  switch (op){
  case OP_AND:{
    Tac_Operand_t newLeft;
    Tac_Operand_t newRight;
    Id_t resultId = genDec (ty);
    Tac_Operand_t result = Tac_Operand_new_var (resultId);
    Label_t truee = Label_new (), falsee = Label_new (),
      end = Label_new ();

    newLeft = Elab_exp (left);
    emit (Tac_Stm_new_if (newLeft,
                          truee,
                          falsee));
    emit (Tac_Stm_new_label (truee));
    newRight = Elab_exp (right);
    emit (Tac_Stm_new_move (result, newRight));
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (falsee));
    emit (Tac_Stm_new_move
          (result,
           Tac_Operand_new_int (0)));
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (end));
    return result;
  }
  case OP_OR:{
    Tac_Operand_t newLeft;
    Tac_Operand_t newRight;
    Id_t resultId = genDec (ty);
    Tac_Operand_t result = Tac_Operand_new_var (resultId);
    Label_t truee = Label_new (), falsee = Label_new (),
      end = Label_new ();

    newLeft = Elab_exp (left);
    emit (Tac_Stm_new_if (newLeft,
                          truee,
                          falsee));
    emit (Tac_Stm_new_label (truee));
    emit (Tac_Stm_new_move
          (result,
           Tac_Operand_new_int (1)));
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (falsee));
    newRight = Elab_exp (right);
    emit (Tac_Stm_new_move (result, newRight));
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (end));
    return result;
  }
  default:{
    Tac_Operand_t newLeft = Elab_exp (left);
    Tac_Operand_t newRight = Elab_exp (right);
    Id_t resultId = genDec (ty);
    emit (Tac_Stm_new_bop (resultId,
                           newLeft,
                           op,
                           newRight));
    return Tac_Operand_new_var (resultId);
  }
  }
  Error_impossible ();
  return 0;
}

static Tac_Operand_t Elab_uop (Hil_Exp_t src,
                               Operator_t op,
                               Id_t ty)
{
  switch (op){
  case OP_NOT:{
    Tac_Operand_t newE = Elab_exp (src);
    Id_t resultId = genDec (ty);
    Tac_Operand_t result = Tac_Operand_new_var (resultId);
    Label_t truee = Label_new (), falsee = Label_new (),
      end = Label_new ();

    emit (Tac_Stm_new_if (newE,
                          truee,
                          falsee));
    emit (Tac_Stm_new_label (truee));
    emit (Tac_Stm_new_move (result,
                            Tac_Operand_new_int (0)));
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (falsee));
    emit (Tac_Stm_new_move (result,
                            Tac_Operand_new_int (1)));
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (end));
    return result;
  }
  default:{
    Tac_Operand_t newE = Elab_exp (src);
    Id_t result = genDec (ty);

    emit (Tac_Stm_new_uop (result,
                           op,
                           newE));
    return Tac_Operand_new_var (result);
  }
  }
  Error_impossible ();
  return 0;
}

static Tac_Operand_t Elab_exp (Hil_Exp_t e)
{
  Assert_ASSERT(e);
  printf("<[tac e->kind]>=%d\n",e->kind);
  switch (e->kind){
  case HIL_EXP_BOP:{
    Tac_Operand_t result = Elab_bop (e->u.bop.left,
                                     e->u.bop.op,
                                     e->u.bop.right,
                                     e->ty);
    return result;
  }
  case HIL_EXP_UOP:{
    return Elab_uop (e->u.unary.e,
                     e->u.unary.op,
                     e->ty);
  }
  case HIL_EXP_INTLIT:{
    Tac_Operand_t result =
      Tac_Operand_new_int (e->u.intlit);
    return result;
  }
  case HIL_EXP_FLOATLIT:{
    Tac_Operand_t result =
      Tac_Operand_new_float (e->u.floatlit);
    return result;
  }
  case HIL_EXP_DOUBLELIT:{
    Tac_Operand_t result =
      Tac_Operand_new_double (e->u.doublelit);
    return result;
  }
  case HIL_EXP_STRINGLIT:{
    Tac_Operand_t result =
      Tac_Operand_new_string (e->u.stringlit);
    return result;
  }
  case HIL_EXP_NEW_STRUCT:{
    List_t p =
      LinkedList_getFirst (e->u.newStruct.list);
    Id_t result = genDec (e->ty);
    List_t c = LinkedList_new ();
    while (p){
      Hil_Field_t f = (Hil_Field_t)p->data;
      Tac_Operand_t r = Elab_exp (f->e);
      LinkedList_insertLast (c, r);
      p = p->next;
    }
    emit (Tac_Stm_new_newStruct
          (result,
           e->u.newStruct.type,
           c));
    return Tac_Operand_new_var (result);
  }
  case HIL_EXP_NEW_ARRAY:{
    Id_t result = genDec (e->ty);
    Tac_Operand_t idSize = Elab_exp (e->u.newArray.size);
    Tac_Operand_t idInit = Elab_exp (e->u.newArray.init);
    emit (Tac_Stm_new_newArray
          (result,
           e->u.newArray.type,
           idSize,
           idInit));
    printf("hil_exp_new_array");
    return Tac_Operand_new_var (result);
  }
  case HIL_EXP_LVAL:{
    Tac_Operand_t left = Elab_lval (e->u.lval);
    printf("[HIL]left->kind=%d\n",left->kind);
    switch (left->kind){
    case TAC_OP_INT:
    case TAC_OP_STR:
      Error_impossible();
      return 0;
    case TAC_OP_VAR:
      return left;
    case TAC_OP_STRUCT:
    case TAC_OP_ARRAY:{
       //Id_t result = genDec (e->ty);
       //emit (Tac_Stm_new_move
       //      (Tac_Operand_new_var (result),
        //      left));
      return left;
    }
    default:
      Error_impossible();
      return 0;
    }
    Error_impossible();
    return 0;
  }
  case HIL_EXP_CALL:{
    Id_t result = genDec (e->ty);
    LinkedList_t p = LinkedList_getFirst (e->u.call.args);
    LinkedList_t c = LinkedList_new ();
    while (p){
      Hil_Exp_t current = (Hil_Exp_t)p->data;
      Tac_Operand_t r = Elab_exp (current);
      LinkedList_insertLast (c, r);
      p = p->next;
    }
    emit (Tac_Stm_new_call (result,
                            e->u.call.f,
                            c));
    return Tac_Operand_new_var (result);
  }
  default:
    Error_impossible();
    return 0;
  }
  Error_impossible();
  return 0;
}

static void Elab_stm (Hil_Stm_t s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case HIL_STM_ASSIGN:
  case HIL_STM_ASSIGN_DTOI:
  case HIL_STM_ASSIGN_ITOD:
	{
	Tac_Stm_t stm ;
    Tac_Operand_t left = Elab_lval (s->u.assign.lval);
    Tac_Operand_t right = Elab_exp (s->u.assign.exp);
	if(s->kind==HIL_STM_ASSIGN)
      stm=Tac_Stm_new_move (left, right);
	else if(s->kind==HIL_STM_ASSIGN_DTOI)
	  stm=Tac_Stm_new_move_dtoi (left, right);
	else if(s->kind==HIL_STM_ASSIGN_ITOD)
	  stm=Tac_Stm_new_move_itod (left, right);
    emit (stm);
    return;
  }

  case HIL_STM_EXP:{
    Elab_exp (s->u.exp);
    return;
  }
  case HIL_STM_IF:{
    Tac_Operand_t condition =
      Elab_exp (s->u.iff.condition);
    Label_t truee = Label_new (),
      falsee = Label_new (),
      end = Label_new ();
    emit (Tac_Stm_new_if (condition,
                          truee,
                          falsee));
    emit (Tac_Stm_new_label (truee));
    LinkedList_foreach (s->u.iff.then,
                        (Poly_tyVoid)Elab_stm);
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (falsee));
    if (s->u.iff.elsee){
      LinkedList_foreach (s->u.iff.elsee,
                          (Poly_tyVoid)Elab_stm);
    }
    emit (Tac_Stm_new_jump (end));
    emit (Tac_Stm_new_label (end));
    return;
  }
  case HIL_STM_DO:{
    Tac_Operand_t result;
    Label_t start = Label_new ();

    emit (Tac_Stm_new_label (start));
    LinkedList_foreach (s->u.doo.body,
                        (Poly_tyVoid)Elab_stm);
    emit (Tac_Stm_new_label (s->u.doo.entryLabel));
    /* cook the padding, if any */
    if (s->u.doo.padding)
      LinkedList_foreach (s->u.doo.padding,
                          (Poly_tyVoid)Elab_stm);
    else;
    result = Elab_exp (s->u.doo.condition);
    emit (Tac_Stm_new_if (result,
                          start,
                          s->u.doo.exitLabel));
    emit (Tac_Stm_new_label (s->u.doo.exitLabel));
    return;
  }
  case HIL_STM_JUMP:{
    emit (Tac_Stm_new_jump (s->u.jump));
    return;
  }
  case HIL_STM_RETURN:{
    Tac_Operand_t result = Elab_exp (s->u.returnn);
    emit (Tac_Stm_new_move
          (Tac_Operand_new_var (fun.retId),
           result));
    emit (Tac_Stm_new_jump (fun.exitLabel));
    return;
  }
  case HIL_STM_END:
  	emit(Tac_Stm_new_end());
	return ;
  case HIL_STM_UPDATE:
  	emit(Tac_Stm_new_update());
  	return ;
  case HIL_STM_LABEL:
  	emit(Tac_Stm_new_label(s->u.label.labelName));
	LinkedList_foreach (s->u.label.s,
                        (Poly_tyVoid)Elab_stm);
  	return ;
  case HIL_STM_INTERNAL_FUNC:{
    LinkedList_t p = LinkedList_getFirst (s->u.infuc.args);
    LinkedList_t c = LinkedList_new ();
    while (p){
      Hil_Exp_t current = (Hil_Exp_t)p->data;
      Tac_Operand_t r = Elab_exp (current);
      LinkedList_insertLast (c, r);
      p = p->next;
    }
    emit (Tac_Stm_new_primitive(s->u.infuc.iFun,
                            c));

    return;
  }
  default:
    Error_bug ("impossible");
    return;
  }
  Error_bug ("impossible");
  return;
}

static Tac_Fun_t Elab_funForOne (Hil_Fun_t f)
{
  Assert_ASSERT(f);
  fun.entryLabel = Label_new ();
  fun.exitLabel = Label_new ();


  if(f->attr==0x01)
  	LinkedList_foreach (f->stms,
                      (Poly_tyVoid)Elab_stm);
  else	{
  if(String_equals (Id_toString ( f->type),
                     "void"))
  	{
  	LinkedList_foreach (f->stms,
                      (Poly_tyVoid)Elab_stm);
   emit(Tac_Stm_new_returnvoid());
  	}
  else
  	{
  fun.retId = genDec (f->type);
  LinkedList_foreach (f->stms,
                      (Poly_tyVoid)Elab_stm);
  emit (Tac_Stm_new_label (fun.exitLabel));
  emit (Tac_Stm_new_return (fun.retId));

  	}
  	}
  LinkedList_append (f->decs,
                     getDecs ());
  return Tac_Fun_new (f->type,
                      f->name,
                      f->args,
                      f->decs,
                      getBeforeClear (),
                      fun.retId,
                      fun.entryLabel,
                      fun.exitLabel,
                      f->attr);
}

static List_t Elab_funcs (List_t fs)
{
  Assert_ASSERT(fs);
  allStms = LinkedList_new ();
  allDecs = LinkedList_new ();
  return LinkedList_map (fs,
                         (Poly_tyId)Elab_funForOne);
}

static Tac_Struct_t Elab_struct (Hil_Struct_t t)
{
  Assert_ASSERT(t);
  return Tac_Struct_new (t->type,
                         t->var);
}

static Tac_Type_t Elab_type (Hil_Type_t t)
{
  Assert_ASSERT(t);
  switch (t->kind){
  case HIL_TYPE_STRUCT:
    return Tac_Type_new_struct
      (LinkedList_map (t->u.structt,
                       (Poly_tyId)Elab_struct));
  case HIL_TYPE_ARRAY:
    return Tac_Type_new_array (t->u.array);
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

static Tac_Td_t Elab_tdOne(Hil_Td_t t)
{
  Assert_ASSERT(t);
  return Tac_Td_new (t->name,
                     Elab_type (t->type));
}

static List_t Elab_tds (List_t t)
{
  Assert_ASSERT(t);
  return LinkedList_map (t,
                         (Poly_tyId)Elab_tdOne);
}

static Tac_Prog_t Hil_transTraced (Hil_Prog_t p)
{
  List_t tds, funcs;

  Assert_ASSERT(p);
  tds = Elab_tds (p->tds);
  funcs = Elab_funcs (p->funcs);
  return Tac_Prog_new (tds,
                       funcs);
}

Tac_Prog_t Hil_trans (Hil_Prog_t p)
{
  return Hil_transTraced (p);
}

#undef List_t
