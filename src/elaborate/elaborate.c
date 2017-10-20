#include "../lib/assert.h"
#include "../lib/linkedList.h"
#include "../lib/stack.h"
#include "../lib/hash.h"
#include "../lib/error.h"
#include "../lib/string.h"
#include "../lib/trace.h"
#include "../control/error-msg.h"
#include "type.h"
#include "env.h"
#include "elaborate.h"


#define List_t LinkedList_t
#define Id_t AstId_t
static AstId_t newFunName;
static int level=0;

static void error (String_t, Region_t);

static Type_t returnTy = 0;
List_t globals=0;

static void error (String_t msg, Region_t r)
{
  ErrorMsg_elabError (msg, r);
}
static void warn (String_t msg, Region_t r)
{
  WarningMsg_elabError (msg, r);
}

static void error_mismatch (Type_t wanted, Type_t got,
			    Region_t r)
{
  error (String_concat
	 ("type mismatch "
	  "expects: ",
	  Type_toString (wanted),
	  " got: ",
	  Type_toString (got),
	  0),
	 r);
}
static void warn_mismatch (Type_t wanted, Type_t got,
			    Region_t r)
{
  warn (String_concat
	 ("type mismatch "
	  "expects: ",
	  Type_toString (wanted),
	  " got: ",
	  Type_toString (got),
	  0),
	 r);
}

static void checkType (Type_t t1, Type_t t2, Region_t r)
{
  if (!Type_equals (t1, t2))
    error_mismatch (t1, t2, r);
}
static void checkTypeWarning (Type_t t1, Type_t t2, Region_t r)
{
  if (!Type_equals (t1, t2))
    warn_mismatch (t1, t2, r);
}

struct Lval_Result_t
{
  Type_t type;
  Ast_Lval_t lval;
};

struct Exp_Result_t
{
  Type_t type;
  Ast_Exp_t exp;
};

static Ast_Block_t Elab_block (Ast_Block_t);
static struct Lval_Result_t Elab_lval (Ast_Lval_t);
static struct Exp_Result_t Elab_exp (Ast_Exp_t);

static struct Lval_Result_t Elab_lval (Ast_Lval_t l)
{
  struct Lval_Result_t bogus
    = {Type_new_int (), l};

  Assert_ASSERT(l);
  printf("[elab_lval]=%d\n",l->kind);
  switch (l->kind){
  case AST_LVAL_VAR:{
    Env_Binding_t binding;
    struct Lval_Result_t result;

    binding = Venv_lookupMustExist (l->u.var);
    result.type = binding->type;
    result.lval = Ast_Lval_new_var (binding->fresh,
				    l->region);
    return result;
  }
  case AST_LVAL_DOT:{
    Env_Binding_t bindingTid, bindingVar;
    struct Lval_Result_t r, result;
    Type_t ty;
    Id_t sid, tid;

    r = Elab_lval (l->u.dot.lval);
    ty = r.type;
    if (ty->kind != TYPE_A_STRUCT){
      error ("struct type expected in left-value",
	     AstId_dest (l->u.dot.var));
      return bogus;
    }
    sid = ty->u.structt.name;
    tid = Type_searchField (ty, l->u.dot.var);
    if (!tid){
      error (String_concat
             ("struct has no this field: ",
              AstId_toString (l->u.dot.var),
              0),
	     AstId_dest (l->u.dot.var));
      return bogus;
    }
    bindingTid = Tenv_lookupMustExist (tid);
    bindingVar = Senv_lookup (sid, l->u.dot.var);
    result.type = bindingTid->type;
    result.lval = Ast_Lval_new_dot (r.lval,
				    bindingVar->fresh,
				    l->region);
    return result;
  }
  case AST_LVAL_ARRAY:{
    struct Lval_Result_t r;
    struct Exp_Result_t re;
    struct Lval_Result_t result;
    Env_Binding_t binding;
    Type_t t;
    AstId_t tid;

    r = Elab_lval (l->u.array.lval);
    t = r.type;
    tid = Type_dest_array (t);
    binding = Tenv_lookupMustExist (tid);
    re = Elab_exp (l->u.array.exp);
    checkType (Type_new_int (), re.type, l->region);
    result.type = binding->type;
    result.lval = Ast_Lval_new_array (r.lval,
				      re.exp,
				      l->region);
    return result;
  }
  default:
    Error_bug ("impossible");
    return bogus;
  }
  Error_bug ("impossible");
  return bogus;
}

static struct Exp_Result_t Elab_exp (Ast_Exp_t e)
{
  struct Exp_Result_t bogus = {Type_new_int (), e};
  printf("elab_exp kind=%d\n",e->kind);
  Assert_ASSERT(e);
  switch (e->kind){
  case AST_EXP_ADD:
  case AST_EXP_SUB:
  case AST_EXP_TIMES:
  case AST_EXP_DIVIDE:
  case AST_EXP_MODUS:
    {

    struct Exp_Result_t r1, r2, result;
    Type_t tint = Type_new_double() ;
	Ast_Exp_t e1,e2;
	double result_value,op1,op2;

    r1 = Elab_exp (e->u.bop.left);
	r2 = Elab_exp (e->u.bop.right);
	e1=(r1.exp);
	e2=(r2.exp);



//	if(r2.type!=Type_new_double() &&r2.type!=Type_new_int())



	///this part is for automatic calc before down in the board
    if(e1->kind==AST_EXP_DOUBLELIT||e1->kind==AST_EXP_INTLIT)
    {
    	 op1=(e1->kind==AST_EXP_DOUBLELIT)?e1->u.doublelit:e1->u.intlit;

         if(e2->kind==AST_EXP_DOUBLELIT||e2->kind==AST_EXP_INTLIT)
         {
           op2=(e2->kind==AST_EXP_DOUBLELIT)?e2->u.doublelit:e2->u.intlit;
		    switch(e->kind)
            {
             case AST_EXP_ADD:
			 	result_value=op1+op2;
				break;
  			 case AST_EXP_SUB:
			 	result_value=op1-op2;
				break;
             case AST_EXP_TIMES:
			 	result_value=op1*op2;
				break;
             case AST_EXP_DIVIDE:
			 	result_value=op1/op2;
				break;
            /*
             case AST_EXP_MODUS:
			 	result_value=(op1)%(op2);
				break;
			*/
			 default:
			 	Error_bug ("impossible");
			}
			if(e1->kind==AST_EXP_INTLIT&&e2->kind==AST_EXP_INTLIT)
			{
				result.exp=Ast_Exp_new_intlit_value((int)result_value);
			    result.type = Type_new_int();
			}
			else
			{
			result.exp=Ast_Exp_new_doublelit_value(result_value);
			result.type = tint;
				}
   			return result;
		 }
		}
	//checkType (tint, r1.type, e->region);
	//checkType (tint, r2.type, e->region);

	    if(!Type_equals(r1.type,tint))
	    {
		if(r1.type->kind==TYPE_A_INT)
		{
		  if(r1.exp->kind==AST_EXP_INTLIT)
	       r1.exp=Ast_Exp_new_doublelit_value((double)(r1.exp->u.intlit));
		  else if(r1.exp->kind==AST_EXP_LVAL)
		  	e->kind|=0x10000;

		}
	    }
		if(!Type_equals(r2.type,tint))
		{
        if(r2.type->kind==TYPE_A_INT)
        {
          if(r2.exp->kind==AST_EXP_INTLIT)
			r2.exp=Ast_Exp_new_doublelit_value((double)(r2.exp->u.intlit));
		  else if(r2.exp->kind==AST_EXP_LVAL)
		  	e->kind|=0x20000;
		}
		}
	//return type is double
    result.exp = Ast_Exp_new_bop (e->kind,
				  r1.exp,
				  r2.exp,
				  e->region);
    result.type = tint;
    return result;
  }
  //add for support Shift left AHA so much work to do for generate for Machine code
  case AST_EXP_SL:
  	{
	struct Exp_Result_t result,r1,r2;
	Type_t tint = Type_new_int() ;
   	Ast_Exp_t e1,e2;
	int op1,op2;
	r1 = Elab_exp (e->u.bop.left);
	r2 = Elab_exp (e->u.bop.right);
	if(r1.type==Type_new_int() ||r1.type==Type_new_double())
    {
       op1=(r1.type==Type_new_int())?r1.exp->u.intlit:(int)(r1.exp->u.doublelit);
	   if(r2.type==Type_new_int() ||r2.type==Type_new_double())
	   op2=(r2.type==Type_new_int())?r2.exp->u.intlit:(int)(r2.exp->u.doublelit);
       else error("shift argument must be int",e->region);


       if(e->kind==AST_EXP_SL)
       	{
         result.exp=Ast_Exp_new_intlit_value(op1<<op2);
		 result.type = tint;
 		 return result;
	    }

	}
	else error("shift argument must be int",e->region);
	}
  case AST_EXP_OR:
  case AST_EXP_AND:
  case AST_EXP_LT:
  case AST_EXP_LE:
  case AST_EXP_GT:
  case AST_EXP_GE:
  case AST_EXP_EQ:
  case AST_EXP_NE:{
    struct Exp_Result_t r1, r2, result;
    Type_t ty1, ty2;

    result.type = Type_new_int ();
    result.exp = e;
    r1 = Elab_exp (e->u.bop.left);
    r2 = Elab_exp (e->u.bop.right);
    ty1 = r1.type;
    ty2 = r2.type;
    if (ty1->kind == TYPE_A_STRING
	|| ty2->kind == TYPE_A_STRING){
      error ("string type can not be compared",
	     e->region);
      return result;
    }
    if (ty1->kind == TYPE_A_NS
	&& ty2->kind == TYPE_A_NS)
      return result;
    //checkType (ty1, ty2, e->region);

	if (!Type_equals (ty1, ty2))
	{
		if(ty1->kind==TYPE_A_INT)
		{
		  if(r1.exp->kind==AST_EXP_INTLIT)
	       r1.exp=Ast_Exp_new_doublelit_value((double)(r1.exp->u.intlit));
		  else if(r1.exp->kind==AST_EXP_LVAL)
		  	e->kind|=0x10000;

		}
        else if(ty2->kind==TYPE_A_INT)
        {
          if(r2.exp->kind==AST_EXP_INTLIT)
			r2.exp=Ast_Exp_new_doublelit_value((double)(r2.exp->u.intlit));
		  else if(r2.exp->kind==AST_EXP_LVAL)
		  	e->kind|=0x20000;
		}
	}
    result.exp = Ast_Exp_new_bop (e->kind,
				  r1.exp,
				  r2.exp,
				  e->region);
    return result;
  }
  case AST_EXP_NOT:
  {
    struct Exp_Result_t r1, result;
    Type_t ty, tint = Type_new_int ();

    r1 = Elab_exp (e->u.unary.e);
    ty = r1.type;
    checkType (tint, ty, e->region);
    result.type = tint;
    result.exp = Ast_Exp_new_unary (e->kind,
				    r1.exp,
				    e->region);
    return result;
  }
  case AST_EXP_NEGATIVE:
  {
  	//for support double type
    struct Exp_Result_t r1, result;
	Ast_Exp_t  e1=e->u.unary.e;
    Type_t ty, tint = Type_new_int ();


	if(e1->kind==AST_EXP_DOUBLELIT)
	{
				result.exp=Ast_Exp_new_doublelit_value(e1->u.doublelit*(-1.0));
			    result.type = Type_new_double();
				return result;


	}
	else if(e1->kind==AST_EXP_INTLIT)
	{
			    result.exp=Ast_Exp_new_intlit_value(e1->u.intlit*-1);
			    result.type = Type_new_int();
				return result;

	}
	 r1 = Elab_exp (e->u.unary.e);
    ty = r1.type;
    //checkType (tint, ty, e->region);
    result.type = ty;
    result.exp = Ast_Exp_new_unary (e->kind,
				    r1.exp,
				    e->region);
    return result;


  }
  case AST_EXP_NULL:{
    struct Exp_Result_t result;
    result.type = Type_new_ns();
    result.exp = e;
    return result;
  }
  case AST_EXP_INTLIT:{
    struct Exp_Result_t result;
    result.type = Type_new_int ();
    result.exp = e;
    return result;
  }
  case AST_EXP_FLOATLIT:{
    struct Exp_Result_t result;
    result.type = Type_new_float ();
    result.exp = e;
    return result;
  }
  case AST_EXP_DOUBLELIT:
  {
    struct Exp_Result_t result;
    result.type = Type_new_double ();
    result.exp = e;
    return result;
  }
  case AST_EXP_STRINGLIT:{
    struct Exp_Result_t result;
    result.type = Type_new_string ();
    result.exp = e;
    return result;
  }
  case AST_EXP_NEW_STRUCT:{
    Env_Binding_t binding;
    Type_t ty;
    LinkedList_t fieldsTy, fieldsExpTy, fieldsNew;
    struct Exp_Result_t result;

    binding = Tenv_lookupMustExist (e->u.newStruct.type);
    ty = binding->type;
    if (ty->kind != TYPE_A_STRUCT){
      error ("expects struct type in allocation",
	     AstId_dest (e->u.newStruct.type));
      return bogus;
    }
    fieldsTy = LinkedList_getFirst (ty->u.structt.fields);
    fieldsExpTy =LinkedList_getFirst (e->u.newStruct.list);
    fieldsNew = LinkedList_new ();
    while (fieldsExpTy && fieldsTy){
      Ast_Field_t field;
      Type_Struct_t tf;
      struct Exp_Result_t re;
      Type_t ty1;
      Id_t fresh;

      field = (Ast_Field_t)fieldsExpTy->data;
      tf = (Type_Struct_t)fieldsTy->data;
      if (!AstId_equals (tf->name, field->id)){
        error (String_concat
	       ("type of id in struct and init mismatch\n",
		"expects: ",
		AstId_toString (tf->name),
		"\nbut got: ",
		AstId_toString (field->id),
		0),
	       AstId_dest (field->id));
	return bogus;
      }
      re = Elab_exp (field->e);
      ty1 = re.type;
      checkType (Tenv_lookup(tf->type)->type, ty1,
		 AstId_dest (field->id));
      fresh = Senv_lookup (ty->u.structt.name,
			   field->id)->fresh;
      LinkedList_insertLast (fieldsNew,
			     Ast_Field_new (fresh,
					    re.exp));
      fieldsExpTy = fieldsExpTy->next;
      fieldsTy = fieldsTy->next;
    }
    if (fieldsExpTy || fieldsTy){
      error ("initialization and type mismatch",
	     Region_bogus ());
      return bogus;
    }
    result.exp = Ast_Exp_new_newStruct (binding->fresh,
					fieldsNew);
    result.type = ty;
    return result;
  }
  case AST_EXP_NEW_ARRAY:{
    struct Exp_Result_t r1, r2, result;
    Type_t tye, tint = Type_new_int (),
      initTy, arrayTy, eltTy;
    Env_Binding_t arrayBinding, eltBinding;
    Id_t fresh;

    r1 = Elab_exp (e->u.newArray.size);
    tye = r1.type;
    checkType (tint, tye,
	       AstId_dest (e->u.newArray.type));
    arrayBinding =
      Tenv_lookupMustExist (e->u.newArray.type);
    arrayTy = arrayBinding->type;
    fresh = arrayBinding->fresh;
    if (arrayTy->kind != TYPE_A_ARRAY){
      error ("array type expected",
	     AstId_dest (e->u.newArray.type));
      return bogus;
    }
    eltBinding =
      Tenv_lookupMustExist (arrayTy->u.array.eltType);
    eltTy = eltBinding->type;
    r2 = Elab_exp (e->u.newArray.init);
    initTy = r2.type;
    checkType (eltTy, initTy,
	       AstId_dest (e->u.newArray.type));
    result.type = arrayTy;
    result.exp = Ast_Exp_new_newArray (fresh,
				       r1.exp,
				       r2.exp);
    printf("i am here\n");
    return result;
  }
  case AST_EXP_LVAL:{
    struct Lval_Result_t r;
    struct Exp_Result_t result;

    r = Elab_lval (e->u.lval);
    result.type = r.type;
    result.exp = Ast_Exp_new_lval (r.lval);
    return result;
  }
  case AST_EXP_CALL:{
    Env_Binding_t funNameBinding;
    Type_t ty, retTy, argsFrom;
    List_t argsTy, args, argsResult;
    Tuple_t tuple;
    struct Exp_Result_t result;
	Ast_Exp_t exp1;
    AstId_t id;

    funNameBinding = Venv_lookupMustExist (e->u.call.f);
    ty = funNameBinding->type;
    if (ty->kind == TYPE_A_FUN){
      tuple = Type_dest_fun (ty);
      argsFrom = Tuple_first (tuple);
      retTy = Tuple_second (tuple);
    }
    else{
      argsFrom = Type_new_product (Type_new_int (), 0);
      retTy = Type_new_int ();
    }
    if (argsFrom->kind != TYPE_A_PRODUCT)
      Error_bug ("compiler bug");
    argsTy = LinkedList_getFirst (argsFrom->u.product);
    args = LinkedList_getFirst (e->u.call.args);
    argsResult = LinkedList_new ();
    while (argsTy && args){
      Type_t ty;
      struct Exp_Result_t r;

      ty = (Type_t)argsTy->data;
	  //


	  exp1=args->data;
	  if(ty==Type_new_double ()&&exp1->kind==AST_EXP_INTLIT)
	  {
		exp1->kind=AST_EXP_DOUBLELIT;
		exp1->u.doublelit =exp1->u.intlit;

	  }
	  r = Elab_exp (args->data);
      LinkedList_insertLast (argsResult, r.exp);
      checkType (ty, r.type,
		 AstId_dest (e->u.call.f));
      argsTy = argsTy->next;
      args = args->next;
    }
    if (argsTy || args)
      error ("type mismatch in function call",
	     AstId_dest (e->u.call.f));
    result.type = retTy;
    switch (retTy->kind){
    case TYPE_A_INT:
      id = AstId_fromString ("int", Region_bogus ());
      break;
    case TYPE_A_STRING:
      id = AstId_fromString ("string", Region_bogus ());
      break;
	case TYPE_A_FLOAT:
      id = AstId_fromString ("float", Region_bogus ());
      break;
    default:
      id = 0;
      break;
    }
    result.exp = Ast_Exp_new_call (funNameBinding->fresh,
				   argsResult,
                                   id);
    return result;
  }
  default:
    *((int *)0) = 0;
    Error_bug ("impossible");
    return bogus;
  }
  Error_bug ("impossible");
  return bogus;
}

static Ast_Stm_t Elab_stm (Ast_Stm_t s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case AST_STM_ASSIGN:{
    struct Lval_Result_t left;
    struct Exp_Result_t right;

    left = Elab_lval (s->u.assign.lval);
    right = Elab_exp (s->u.assign.exp);
	if(left.type->kind==TYPE_A_INT&&right.exp->kind==AST_EXP_DOUBLELIT)
	{

	return Ast_Stm_new_assign
      (left.lval, Ast_Exp_new_intlit_value((int)(right.exp->u.doublelit)), s->region);

	}
    checkTypeWarning (left.type, right.type, s->region);
	if(left.type->kind==TYPE_A_INT&&right.type->kind==TYPE_A_DOUBLE)
	return Ast_Stm_new_assign_dtoi
      (left.lval, right.exp, s->region);
	else if(left.type->kind==TYPE_A_DOUBLE&&right.type->kind==TYPE_A_INT)
	return Ast_Stm_new_assign_itod(left.lval, right.exp, s->region);
    else return Ast_Stm_new_assign
      (left.lval, right.exp, s->region);
  }
  case AST_STM_EXP:{
    struct Exp_Result_t right;

    right = Elab_exp (s->u.exp);
    return Ast_Stm_new_exp
      (right.exp, s->region);
  }
  case AST_STM_IF:{
    struct Exp_Result_t re;
    Ast_Stm_t thenn, elsee;

    re = Elab_exp (s->u.iff.condition);
    checkType (Type_new_int(), re.type, s->region);
    thenn = Elab_stm (s->u.iff.then);
    elsee = (s->u.iff.elsee)?
      (Elab_stm (s->u.iff.elsee)):
      (Ast_Stm_new_block
       (Ast_Block_new (LinkedList_new (),
                       LinkedList_new ())));
    return Ast_Stm_new_if (re.exp,
			   thenn,
			   elsee,
			   s->region);
  }
  case AST_STM_WHILE:{
    struct Exp_Result_t re;
    Ast_Stm_t stms;

    re = Elab_exp (s->u.whilee.condition);
    checkType (Type_new_int(), re.type, s->region);
    stms = Elab_stm (s->u.whilee.body);
    return Ast_Stm_new_while (re.exp, stms, s->region);
  }
  case AST_STM_DO:{
    struct Exp_Result_t re;
    Ast_Stm_t stms;

    re = Elab_exp(s->u.doo.condition);
    checkType (Type_new_int(), re.type, s->region);
    stms = Elab_stm (s->u.doo.body);
    return Ast_Stm_new_do (re.exp, stms, s->region);
  }
  case AST_STM_FOR:{
    struct Exp_Result_t re;
    Ast_Stm_t header, tail, body;

    header = Elab_stm (s->u.forr.header);
    re = Elab_exp (s->u.forr.condition);
    checkType (Type_new_int (), re.type, s->region);
    tail = Elab_stm (s->u.forr.tail);
    body = Elab_stm (s->u.forr.body);
    return Ast_Stm_new_for (header,
			    re.exp,
			    tail,
			    body,
			    s->region);
  }
  case AST_STM_BREAK:
    return s;
  case AST_STM_CONTINUE:
  case AST_STM_END:
  case AST_STM_UPDATE:
  case AST_STM_GOTO:
    return s;
  case AST_STM_INTERNAL_FUNC:
  		{
	List_t	args = LinkedList_getFirst (s->u.infuc.args);
    List_t argsResult = LinkedList_new ();
    while  (args){
      Type_t ty;
      struct Exp_Result_t r;

       r = Elab_exp (args->data);
      LinkedList_insertLast (argsResult, r.exp);
       args = args->next;
    }
   return Ast_Stm_new_internal_fun(s->u.infuc.funKind,argsResult,s->region);
  }
  case AST_STM_RETURN:{
    struct Exp_Result_t re = Elab_exp (s->u.returnn);
    Assert_ASSERT(returnTy);
    checkType (returnTy, re.type, s->region);
    return Ast_Stm_new_return (re.exp, s->region);
  }
  case AST_STM_BLOCK:
    return Ast_Stm_new_block (Elab_block (s->u.block));
  case AST_STM_LABEL:
  	return Ast_Stm_new_label(s->u.labell.label,  Elab_stm(s->u.labell.body), s->region,0);
  default:
    Error_bug ("impossible");
    return s;
  }
  Error_bug ("impossible");
  return s;
}

static Ast_Struct_t Elab_checkOneFunArg (Ast_Struct_t t)
{
  Env_Binding_t binding;

  Assert_ASSERT(t);
  binding = Tenv_lookupMustExist (t->type);
  Venv_insert (t->var, binding->type);
  return Ast_Struct_new
    (binding->fresh,
     Venv_lookupMustExist (t->var)->fresh);
}

static Ast_Dec_t Elab_checkOneDec (Ast_Dec_t t)
{
  Env_Binding_t binding;
  Ast_Dec_t dec_new;
  Id_t fresh2;
  Type_t ty;
  String_t 	strfun;

  char level_str[10];
  Assert_ASSERT(t);
  binding = Tenv_lookupMustExist (t->type);
  if (!binding){
    return t;
  }
  ty = binding->type;

  if(t->sta)  //change the name
  {
	strfun=AstId_toString(newFunName);
    itoa(level,level_str,10);
	fresh2=AstId_fromString(String_concat(strfun,"@",AstId_toString(t->var),"@",level_str,0),Region_bogus());
	Venv_insert_fresh(t->var,ty,fresh2);
	//InsertInGList(newID,t->init,true);
	//LinkedList_insertLast(globals,);
  }
  else
  fresh2 = Venv_insert (t->var, ty);
  if (t->init){

	struct Exp_Result_t result;
    result = Elab_exp (t->init);
    if (!Type_equals (ty, result.type)){
      error (String_concat
             ("initialization expression type error"
              "\nexpects: ",
              Type_toString (ty),
              "\nbut got: ",
              Type_toString (result.type),
              0),
	     0);
      return t;
    }
	dec_new= Ast_Dec_new (binding->fresh, fresh2, result.exp,t->sta);
  	}

    else  dec_new= Ast_Dec_new (binding->fresh, fresh2, 0,t->sta);
  if(t->sta)
  {
  LinkedList_insertLast(globals,dec_new);
  return NULL;
  }
  return dec_new;

}

static Ast_Dec_t Elab_checkglobalOneDec (Ast_Dec_t t)
{
  Env_Binding_t binding;
  Ast_Dec_t newDec;
  Type_t ty;

  Assert_ASSERT(t);
  printf("(%s)\n",AstId_toString(t->type));
  binding = Tenv_lookupMustExist (t->type);
  if (!binding){
    return t;
  }
  printf("(%s)-1\n",AstId_toString(t->type));
  ty = binding->type;

  Genv_insert (t->var, ty);
  //printf("genv_insert var=%s,type=%s\n",AstId_toString(t->var),AstId_toString(ty->u.array.eltType));
  //printf("binding->type.kind=%d,name=%s,eltType=%s\n",ty->kind,AstId_toString(ty->u.array.name),AstId_toString(ty->u.array.eltType));
  if (t->init){
    struct Exp_Result_t result;

    result = Elab_exp (t->init);
    printf("--(%d)--(%d)\n",ty->kind,result.type->kind);
    if (!Type_equals (ty, result.type)){
      error (String_concat
             ("genv ",AstId_toString(t->var)," initialization expression type error"
              "\nexpects: ",
              Type_toString (ty),
              "\nbut got: ",
              Type_toString (result.type),
              0),
	     0);
      return t;
    }
     newDec=Ast_Dec_new (binding->fresh,
			t->var,
			result.exp,t->sta);
  }
  else newDec=Ast_Dec_new (binding->fresh, t->var, 0,t->sta);
  printf("(%s %s)-2\n",AstId_toString(newDec->type),AstId_toString(newDec->var));
  LinkedList_insertLast(globals,newDec);

  return newDec;
}

struct Block_Result_t
{
  List_t args;
  List_t decs;
  List_t stms;
};

static Ast_Block_t Elab_block (Ast_Block_t b)
{
  List_t decs, stms;

  Assert_ASSERT(b);
  Venv_enterScope ();
  level++;
  decs = LinkedList_map (b->decs,
			 (Poly_tyId)Elab_checkOneDec);
  stms = LinkedList_map (b->stms,
			 (Poly_tyId)Elab_stm);
  level--;
  Venv_exitScope ();
  return Ast_Block_new (decs, stms);
}

static struct Block_Result_t
Elab_topBlock (List_t args, Ast_Block_t b)
{
  struct Block_Result_t result;

  Assert_ASSERT(args);
  Assert_ASSERT(b);

  Venv_enterScope ();
  result.args =
    LinkedList_map (args,
		    (Poly_tyId)Elab_checkOneFunArg);
  result.decs =
    LinkedList_map (b->decs,
		    (Poly_tyId)Elab_checkOneDec);
  result.stms =
    LinkedList_map (b->stms,
		    (Poly_tyId)Elab_stm);
  Venv_exitScope ();
  return result;
}

static Ast_Fun_t Elab_funRound2ForOne (Ast_Fun_t f)
{
  Env_Binding_t bindingRetTy, bindingFunName;
  struct Block_Result_t b;


  Assert_ASSERT (f);
  bindingRetTy = Tenv_lookupMustExist (f->type);
  bindingFunName = Venv_lookupMustExist (f->name);
  returnTy = (bindingRetTy)?
    (bindingRetTy->type): (Type_new_int ());
  if(f->attr==0x01)
  	{
  newFunName=  f->name;
  //Genv_restore();
  	}
  else
  	{
  // Genv_clear();
   newFunName = bindingFunName->fresh;
  	}
  b = Elab_topBlock (f->args, f->block);
  return Ast_Fun_new (bindingRetTy->fresh,
		      newFunName,
		      b.args,
		      Ast_Block_new (b.decs,
				     b.stms),f->attr,
		      f->region);
}

static Type_t Elab_getTypeOfOneArg (Ast_Struct_t t)
{
  Assert_ASSERT(t);
  return Tenv_lookupMustExist (t->type)->type;
}

static void Elab_funRound1ForOne (Ast_Fun_t f)
{
  Type_t retTy, fTy;
  List_t argsTy;

  Assert_ASSERT(f);
  retTy = Tenv_lookupMustExist (f->type)->type;
  argsTy = LinkedList_map
    (f->args,
     (Poly_tyId)Elab_getTypeOfOneArg);
  fTy = Type_new_fun (Type_new_product2 (argsTy), retTy);
  Venv_insert (f->name, fTy);
  return;
}

static List_t Elab_funDecs (List_t fs)
{
  List_t list;

  Assert_ASSERT(fs);
  Venv_init ();

  LinkedList_foreach (fs,
		      (Poly_tyVoid)Elab_funRound1ForOne);
  list = LinkedList_map
    (fs,
     (Poly_tyId)Elab_funRound2ForOne);
  return list;
}

static Id_t structName = 0;

static Ast_Struct_t Elab_checkOneField (Ast_Struct_t x)
{
  Env_Binding_t binding1, binding2;
  Id_t newType, newVar;

  Assert_ASSERT (x);
  binding1 = Tenv_lookup (x->type);
  binding2 = Senv_lookup (structName, x->var);
  if (!binding1){
    error (String_concat
           ("unbound type name: ",
            AstId_toString (x->type),
            0),
	   AstId_dest (x->type));
    return x;
  }
  newType = binding1->fresh;
  newVar = binding2->fresh;
  return Ast_Struct_new (newType, newVar);
}

static Ast_Td_t Elab_typeDecsRound2ForOne (Ast_Td_t td)
{
  Id_t name, newName;
  Ast_Type_t type, newType;

  name = td->name;
  type = td->type;
  newName = Tenv_lookupMustExist (name)->fresh;
  structName = name;
  switch (type->kind){
  case TYPE_STRUCT:{
    List_t new;
    new = LinkedList_map
      (type->u.structt,
       (Poly_tyId)Elab_checkOneField);
    newType = Ast_Type_new_struct (new);
    break;
  }
  case TYPE_ARRAY:{
    Env_Binding_t binding;
    Id_t newId = AstId_bogus ();

    binding = Tenv_lookup (type->u.array);
    if (!binding)
      error (String_concat
             ("unbound type name: ",
              AstId_toString (type->u.array),
              0),
	     AstId_dest (type->u.array));
    else newId = binding->fresh;
    newType = Ast_Type_new_array (newId);
    break;
  }
  default:
    Error_bug ("impossible");
    return 0;
  }
  return Ast_Td_new (newName, newType);
}

static List_t Elab_typeDecsRound2 (List_t tdecs)
{
  return LinkedList_map
    (tdecs,
     (Poly_tyId)Elab_typeDecsRound2ForOne);
}

static Type_Struct_t Elab_convertAstStruct (Ast_Struct_t x)
{
  Assert_ASSERT(x);
  Assert_ASSERT(structName);
  Senv_insert (structName, x->var);
  return Type_Struct_new (x->type, x->var);
}

static void Elab_typeDecsRound1ForOne (Ast_Td_t td)
{
  Id_t name;
  Ast_Type_t type;

  Assert_ASSERT(td);
  name = td->name;
  structName = name;
  type = td->type;
  switch (type->kind){
  case TYPE_STRUCT:{
    List_t result;
    Type_t newType;
    result = LinkedList_map
      (type->u.structt,
       (Poly_tyId)Elab_convertAstStruct);
    newType = Type_new_struct (name, result);
    Tenv_insert (name, newType);
    return;
  }
  case TYPE_ARRAY:{
    Type_t newTy;
    newTy = Type_new_array (name, type->u.array);
    Tenv_insert (name, newTy);
    printf("Ast_Td_t -> Type_t\n (%s %s)",AstId_toString(type->u.array),AstId_toString(name));
    return;
  }
  default:
    Error_impossible();
    return;
  }
  Error_bug ("impossible");
  return;
}

static void Elab_typeDecsRound1 (List_t tdecs)
{
  LinkedList_foreach
    (tdecs, (Poly_tyVoid)Elab_typeDecsRound1ForOne);
}

static List_t Elab_typeDecs (List_t tdecs)
{
  List_t newTdecs;

  Assert_ASSERT(tdecs);
  Tenv_init ();
  Senv_init ();
  Elab_typeDecsRound1 (tdecs);
  newTdecs = Elab_typeDecsRound2 (tdecs);
  return newTdecs;
}

static Ast_Prog_t Elaborate_ast_traced (Ast_Prog_t p)
{
  int i;
  List_t tds, funcs,glob;
  //create globals
  globals=LinkedList_new();
  Assert_ASSERT(p);
  Genv_init();

  tds = Elab_typeDecs (p->tds);
   glob =
    LinkedList_map (p->globals,
		    (Poly_tyId)Elab_checkglobalOneDec);
  funcs = Elab_funDecs (p->funcs);

  ErrorMsg_errorExit ();
  return Ast_Prog_new (tds, globals,funcs);
}

static void Trace_arg (Ast_Prog_t p)
{
  Ast_Prog_print (p, stdout);
  return;
}

Ast_Prog_t Elaborate_ast (Ast_Prog_t p)
{
  Ast_Prog_t r;
  Trace_TRACE("elaborate",
              Elaborate_ast_traced,
              (p),
              Trace_arg,
              r,
              Trace_arg);
  return r;
}

#undef List_t
#undef Id_t
