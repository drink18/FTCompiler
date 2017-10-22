#include "../lib/assert.h"
#include "../lib/linkedList.h"
#include "../lib/error.h"
#include "../lib/trace.h"
#include "lift-dec.h"

#define List_t LinkedList_t
#define Id_t AstId_t

static List_t allDecs = 0;

void genDecs (List_t l)
{
  LinkedList_append (allDecs, l);
}

static Ast_Block_t Elab_block (Ast_Block_t);
static Ast_Block_t Elab_label (Ast_Block_t fs);
static Ast_Stm_t Elab_stm (Ast_Stm_t s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case AST_STM_ASSIGN:
  case AST_STM_ASSIGN_ITOD:
  case AST_STM_ASSIGN_DTOI:
	{
    return s;
  }
  case AST_STM_EXP:{
    return s;
  }
  case AST_STM_IF:{
    return Ast_Stm_new_if (s->u.iff.condition,
			   Elab_stm (s->u.iff.then),
			   Elab_stm (s->u.iff.elsee),
			   s->region);
  }
  case AST_STM_WHILE:{
    return Ast_Stm_new_while (s->u.whilee.condition,
			      Elab_stm (s->u.whilee.body),
			      s->region);
  }
  case AST_STM_DO:{
    return Ast_Stm_new_do (s->u.doo.condition,
			   Elab_stm (s->u.doo.body),
			   s->region);
  }
  case AST_STM_FOR:{
    return Ast_Stm_new_for (Elab_stm(s->u.forr.header),
			    s->u.forr.condition,
			    Elab_stm(s->u.forr.tail),
			    Elab_stm(s->u.forr.body),
			    s->region);
  }
  case AST_STM_BREAK:
  case AST_STM_END:
  case AST_STM_UPDATE:
  case AST_STM_GOTO:
  case AST_STM_INTERNAL_FUNC:
    return s;
  case AST_STM_CONTINUE:
    return s;
  case AST_STM_RETURN:{
    return s;
  }
  case AST_STM_BLOCK:
    return Ast_Stm_new_block (Elab_block (s->u.block));

  case AST_STM_LABEL:
  	//return s;
   return Ast_Stm_new_label(s->u.labell.label,  Elab_stm (s->u.labell.body), s->region,0);
    return ;
  default:
    Error_bug ("impossible");
    return s;
  }
  Error_bug ("impossible");
  return s;
}

static Ast_Block_t Elab_block (Ast_Block_t b)
{
  List_t stms;

  Assert_ASSERT(b);
  genDecs (b->decs);
  stms = LinkedList_map (b->stms,
			 (Poly_tyId)Elab_stm);
  return Ast_Block_new (LinkedList_new (), stms);
}

static Ast_Fun_t Elab_funForOne (Ast_Fun_t f)
{
  Ast_Block_t b;

  Assert_ASSERT(f);
  allDecs = LinkedList_new ();
  b = Elab_block (f->block);
  b = Ast_Block_new (allDecs, b->stms);
  return Ast_Fun_new (f->type,
		      f->name,
		      f->args,
		      b,
		      f->attr,
		      f->region);
}
static Ast_Block_t Elab_label (Ast_Block_t fs)
{

  Ast_Block_t b;
  Assert_ASSERT(fs);
  allDecs = LinkedList_new ();
  b = Elab_block (fs);
  b = Ast_Block_new (allDecs, b->stms);
  return b;
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

static Ast_Prog_t Lift_dec_traced (Ast_Prog_t p)
{
  List_t funcs;

  Assert_ASSERT(p);
  funcs = Elab_funDecs (p->funcs);
  return Ast_Prog_new (p->tds,p->globals, funcs);
}

static void Trace_arg (Ast_Prog_t p)
{
  Ast_Prog_print (p, stdout);
  return;
}

Ast_Prog_t Lift_dec (Ast_Prog_t p)
{
  Ast_Prog_t r;
  Trace_TRACE ("Lift_dec",
               Lift_dec_traced,
               (p),
               Trace_arg,
               r,
               Trace_arg);
  return r;
}

#undef List_t
#undef Id_t
