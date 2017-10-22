#include "../lib/assert.h"
#include "../lib/linkedList.h"
#include "../lib/error.h"
#include "../lib/trace.h"
#include "dead-code.h"

#define List_t LinkedList_t

static List_t Elab_stms (List_t);

static Hil_Stm_t Elab_stm (Hil_Stm_t s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case HIL_STM_END:
  case HIL_STM_UPDATE:
  case HIL_STM_INTERNAL_FUNC:
  //case HIL_STM_:
    return s;
  case HIL_STM_ASSIGN:
  case HIL_STM_ASSIGN_DTOI:
  case HIL_STM_ASSIGN_ITOD:
    return s;
  case HIL_STM_EXP:
    return s;
  case HIL_STM_IF:
    return Hil_Stm_new_if (s->u.iff.condition,
                           Elab_stms (s->u.iff.then),
                           (s->u.iff.elsee)?
                           Elab_stms (s->u.iff.elsee): 0);
  case HIL_STM_DO:
    return Hil_Stm_new_do (s->u.doo.condition,
                           Elab_stms (s->u.doo.body),
                           s->u.doo.entryLabel,
                           s->u.doo.exitLabel,
                           s->u.doo.padding);
  case HIL_STM_JUMP:
    return s;
  case HIL_STM_LABEL:
    return s;
  case HIL_STM_RETURN:
    Error_bug ("impossible");
    return 0;
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

static List_t Elab_stms (List_t stms)
{
  List_t p, result = LinkedList_new ();

  Assert_ASSERT(stms);
  p = LinkedList_getFirst (stms);
  while (p){
    Hil_Stm_t s = (Hil_Stm_t)p->data;
    if (s->kind == HIL_STM_RETURN){
      LinkedList_insertLast (result, s);
      break;
    }
    LinkedList_insertLast (result, Elab_stm (s));
    p = p->next;
  }
  return result;
}

static Hil_Fun_t Elab_funForOne (Hil_Fun_t f)
{
  Assert_ASSERT(f);
  return Hil_Fun_new (f->type,
                      f->name,
                      f->args,
                      f->decs,
                      Elab_stms (f->stms),
                      f->attr
                      );
}

static List_t Elab_funcs (List_t fs)
{
  Assert_ASSERT(fs);
  return LinkedList_map (fs,
                         (Poly_tyId)Elab_funForOne);
}

static Hil_Prog_t Hil_deadCodeTraced (Hil_Prog_t p)
{
  Assert_ASSERT(p);
  return Hil_Prog_new (p->tds,
                       Elab_funcs (p->funcs));
}

static void Trace_arg (Hil_Prog_t p)
{
  Hil_Prog_print (p, stdout);
  return;
}

Hil_Prog_t Hil_deadCode (Hil_Prog_t p)
{
  Hil_Prog_t r;
  Trace_TRACE("Hil_deadCode",
              Hil_deadCodeTraced,
              (p),
              Trace_arg,
              r,
              Trace_arg);
  return r;
}

#undef List_t
