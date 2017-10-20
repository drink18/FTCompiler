#include "../lib/stack.h"
#include "../lib/error.h"
#include "../control/error-msg.h"
#include "../control/region.h"
#include "check-ast.h"
#include "../lib/hash.h"

// use to store the label: when goto check the label;
static LinkedList_t labelInFun=0;
static Hash_t label_hash=0;


static Stack_t stack = 0;

static void push ()
{
  Stack_push (stack, "");
}

static void pop ()
{
  Stack_pop (stack);
}

static int empty ()
{
  return Stack_isEmpty (stack);
}

static void error (String_t msg, Region_t r)
{
  ErrorMsg_elabError (msg, r);
}
static void error_dupTypeName (String_t id)
{
  error (String_concat
         ("label name must not be redefined: ",
          id,
          0),
         0);
}

static int Check_block (Ast_Block_t);

static int Check_stmOne (Ast_Stm_t s)
{
  switch (s->kind){
  case AST_STM_ASSIGN:
  case AST_STM_END:
  case AST_STM_UPDATE:
  case AST_STM_INTERNAL_FUNC:
    return 0;
  case AST_STM_IF:{
    int r1 = 0, r2 = 0;
    r1 = Check_stmOne (s->u.iff.then);
    if (s->u.iff.elsee)
      r2 = Check_stmOne (s->u.iff.elsee);
    return r1 && r2;
  }
  case AST_STM_EXP:
    return 0;
  case AST_STM_WHILE:{
    push ();
    Check_stmOne (s->u.whilee.body);
    pop ();
    return 0;
  }
  case AST_STM_DO:{
    push ();
    Check_stmOne (s->u.doo.body);
    pop ();
    return 0;
  }
  case AST_STM_FOR:{
    push ();
    Check_stmOne (s->u.forr.body);
    pop ();
    return 0;
  }
  case AST_STM_BREAK:{
    if (empty ())
      error ("\"break\" not in some loop",
	     s->region);
    return 0;
  }
  case AST_STM_CONTINUE:{
    if (empty ())
      error ("\"continue\" not in some loop",
	     s->region);
    return 0;
  }
  case AST_STM_RETURN:
    return 1;
  case AST_STM_BLOCK:{
    return Check_block (s->u.block);
  }
  case AST_STM_GOTO:{
	if(Hash_lookup(label_hash,s->u.gotoo)==0)
		error (String_concat
         (s->u.gotoo," label must be exist before used ",0),
	     s->region);
	return 0;
  }
  case AST_STM_LABEL:{

	Hash_insert(label_hash,s->u.labell.label,(Poly_t)1,error_dupTypeName);
	Check_stmOne (s->u.labell.body);
  	//LinkedList_insertLast(labelInFun,(Poly_t)s->u.labell.label);
    //return Check_block(s->u.labell.body);
    return 1;
  }

  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

static int Check_block (Ast_Block_t b)
{
  LinkedList_t stms = LinkedList_getFirst (b->stms);
  Ast_Stm_t s;
  int hasReturn = 0;

  while (stms){
    s = (Ast_Stm_t)stms->data;
    if (Check_stmOne (s))
      hasReturn = 1;
    stms = stms->next;
  }
  return hasReturn;
}

static int numDragon = 0;

static void Check_funOne (Ast_Fun_t f)
{

  label_hash=Hash_new
  ((tyHashCode)String_hashCode,
   (Poly_tyEquals)String_equals);
  if (String_equals (Id_toString (AstId_toId (f->name)),
                     "main"))
    numDragon++;
  if (!Check_block (f->block))
  	  ;
  /*
  if(!String_equals (Id_toString (AstId_toId (f->type)),
                     "void"))
  	{
   error (String_concat
	   ("main function return type must be void: ",
	    AstId_toString (f->name),
	    0),
	   f->region);
   exit(0);
  	}
  	*/
  	//in our case main doesn't have return
  	/*
    error (String_concat
	   ("no explicit return statement in function: ",
	    AstId_toString (f->name),
	    0),
	   f->region);
	   */
}


static void Check_prog (Ast_Prog_t p)
{
  int t;

  numDragon = 0;
  LinkedList_foreach (p->funcs,
		      (Poly_tyVoid)Check_funOne);
  t = numDragon;
  numDragon = 0;
  switch (t){
  case 0:
    error ("no funcation named \"main\" is given",
	   0);
    break;
  case 1:
    break;
  default:
    error ("multiple funcations named \"main\" "
           "are given, expects one",
	   0);
    break;
  }
  return;
}

void Check_ast (Ast_Prog_t p)
{
  stack = Stack_new ();
  Check_prog (p);
  return;
}
