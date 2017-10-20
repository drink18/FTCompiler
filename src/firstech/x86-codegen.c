#include "../lib/assert.h"
#include "../lib/error.h"
#include "../lib/todo.h"
#include "x86-codegen.h"

#define List_t LinkedList_t

static List_t allStms = 0;
// to check the prevStm
static Machine_Stm_t prevStm=0;
static void emit (X86_Stm_t s)
{
  LinkedList_insertLast (allStms, s);
  return;
}


static List_t getBeforeClearStms ()
{
  List_t t = allStms;
  allStms = 0;
  return t;
}

typedef enum{
  POS_INT,
  POS_GLOBAL,
  POS_STACK,
  POS_MEM
} Pos_t;

typedef struct
{
  Pos_t pos;
  X86_Operand_t op;
} Operand_Result_t;

/* may occupy register ecx, edx.
 * Invariant: ecx: base, edx: offset
 * final: ecx is occupied.
 */
static X86_Operand_t Trans_operand (Machine_Operand_t o)
{
  Assert_ASSERT(o);
  switch (o->kind){
  case MACHINE_OP_INT:
    return X86_Operand_new_int (o->u.intlit);
  case MACHINE_OP_FLOAT:
    return X86_Operand_new_float (o->u.floatlit);
  case MACHINE_OP_DOUBLE:
    return X86_Operand_new_double (o->u.doublelit);
  case MACHINE_OP_GLOBAL:
    return X86_Operand_new_global (o->u.global);
  case MACHINE_OP_INSTACK:{
    if (o->u.index>0)
		/// index by 1
      return X86_Operand_new_inStack (-(o->u.index));
    else if (o->u.index<0){
      return X86_Operand_new_inStack (-(o->u.index-1));
    }
    Error_impossible ();
    return 0;
  }
  case MACHINE_OP_VAR:
    Error_impossible ();
    return 0;
  case MACHINE_OP_STRUCT:{
    emit (X86_Stm_new_moveri
          (X86_EDX,
           o->u.structt.scale
           * o->u.structt.index));
    if (o->u.structt.base>0)
      emit (X86_Stm_new_load (X86_ECX,
                              X86_Operand_new_inStack
                              ((-4)*o->u.structt.base)));
    else if (o->u.structt.base<0)
      emit (X86_Stm_new_load (X86_ECX,
                              X86_Operand_new_inStack
                              ((-4)*(o->u.structt.base-1))));
    else Error_impossible ();

    emit (X86_Stm_new_bop (X86_ECX,
                           OP_ADD,
                           X86_EDX));
    return X86_Operand_new_mem (X86_ECX, 0);
  }
  case MACHINE_OP_ARRAY:{
#if 0
    emit (X86_Stm_new_load (X86_EDX,
                            X86_Operand_new_inStack
                            ((-4)*o->u.array.offset)));
    /* this should go to machine */
    emit (X86_Stm_new_inc (X86_EDX));
    emit (X86_Stm_new_load (X86_ECX,
                            X86_Operand_new_int
                            (o->u.array.scale)));
    emit (Firstech_Stm_new_bop (X86_EDX,
                           OP_TIMES,
                           X86_ECX,X86_EDX));
    if (o->u.structt.base>0)
      emit (X86_Stm_new_load (X86_ECX,
                              X86_Operand_new_inStack
                              ((-4)*o->u.structt.base)));
    else if (o->u.structt.base<0)
      emit (X86_Stm_new_load (X86_ECX,
                              X86_Operand_new_inStack
                              ((-4)*(o->u.structt.base-1))));
    //else Error_impossible ();

    emit (Firstech_Stm_new_bop (X86_ECX,
                           OP_ADD,
                           X86_EDX,X86_ECX));
    return X86_Operand_new_mem (X86_ECX, 0);
#else

    emit (X86_Stm_new_load (X86_EDX,
                            X86_Operand_new_inStack
                            ((-1)*o->u.array.offset)));
    /* this should go to machine */
    //emit (X86_Stm_new_inc (X86_EDX));
    emit (X86_Stm_new_conv (X86_EDX,ITOD));
    emit (Firstech_Stm_new_adr(X86_ECX,
                            X86_Operand_new_global
                            (o->u.array.gv)));
    emit (X86_Stm_new_conv (X86_ECX,ITOD));
    emit (Firstech_Stm_new_bop (X86_ECX,
                           OP_ADD,
                           X86_EDX,X86_EBX));
    //emit (X86_Stm_new_conv (X86_EBX,DTOI));
    return X86_Operand_new_mem (X86_EBX, 0);
#endif
  }
  default:
    printf ("%d\n", o->kind);
    Error_impossible ();
    return 0;
  }
  Error_impossible ();
  return 0;
}

static void genBop (Machine_Operand_t left,
                    Operator_t op,
                    Machine_Operand_t right,
                    Machine_Operand_t dest)
{
  switch (op&0xFFFF){
  case OP_ADD:
  case OP_SUB:
  case OP_TIMES:
  case OP_DIVIDE:
  	{
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;
    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));
    newRight = Trans_operand(right);
    emit (X86_Stm_new_load (X86_EDX,
                            newRight));

    newDest = Trans_operand (dest);


    emit (Firstech_Stm_new_bop (X86_EDX,
                           op,
                           X86_EAX ,
                           X86_EBX));
	emit (X86_Stm_new_store (newDest,
                           X86_EBX));

    return;
  }
  /*
  {
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));
    newRight = Trans_operand(right);
    emit (X86_Stm_new_load (X86_ECX,
                            newRight));
    emit (X86_Stm_new_cltd ());
    emit (X86_Stm_new_uop (X86_ECX,
                           OP_DIVIDE,
                           X86_ECX));
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }
  */
  case OP_MODUS:{
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));
    newRight = Trans_operand(right);
    emit (X86_Stm_new_load (X86_ECX,
                            newRight));
    emit (X86_Stm_new_cltd ());
    emit (X86_Stm_new_uop (X86_ECX,
                           OP_DIVIDE,
                           X86_ECX));
    emit (X86_Stm_new_moverr (X86_EAX, X86_EDX));
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }
  case OP_LT:
  case OP_LE:
  case OP_GT:
  case OP_GE:
  case OP_EQ:
  case OP_NE:
  	{
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));

    newRight = Trans_operand(right);
	emit (X86_Stm_new_load (X86_EDX,
                            newRight));
	newDest = Trans_operand(dest);
    emit (X86_Stm_new_load (X86_EBX,
                            newDest));
	if(op>>16==1)
	emit (X86_Stm_new_conv (X86_EAX,ITOD));
	else if(op>>16==2)
	emit (X86_Stm_new_conv (X86_EDX,ITOD));
    emit (Firstech_Stm_new_cmp (X86_EDX,
                           op,
                           X86_EAX,
                           X86_EBX));
    //emit (X86_Stm_new_setl (X86_AL));
   // emit (X86_Stm_new_extendAl ());

    //emit (X86_Stm_new_store (newDest,
    //                         X86_EAX));
    return;
  }
  /*
  case OP_LE:
  {
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
   // emit (X86_Stm_new_load (X86_EAX,
    //                        newLeft));
    newRight = Trans_operand (right);
    //emit (X86_Stm_new_load (X86_EDX,
     //                       newRight));
    //emit (X86_Stm_new_cmp (X86_EAX,
    //                       X86_EDX));
    emit (X86_Stm_new_setle (X86_AL));
    emit (X86_Stm_new_extendAl ());
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }
  case OP_GT:{
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));
    newRight = Trans_operand(right);
    emit (X86_Stm_new_load (X86_EDX,
                            newRight));
    emit (X86_Stm_new_cmp (X86_EAX,
                           X86_EDX));
    emit (X86_Stm_new_setg (X86_AL));
    emit (X86_Stm_new_extendAl ());
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }
  case OP_GE:{
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));
    newRight = Trans_operand(right);
    emit (X86_Stm_new_load (X86_EDX,
                            newRight));
    emit (X86_Stm_new_cmp (X86_EAX,
                           X86_EDX));
    emit (X86_Stm_new_setge (X86_AL));
    emit (X86_Stm_new_extendAl ());
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }
  case OP_EQ:{
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));
    newRight = Trans_operand(right);
    emit (X86_Stm_new_load (X86_EDX,
                            newRight));
    emit (X86_Stm_new_cmp (X86_EAX,
                           X86_EDX));
    emit (X86_Stm_new_sete (X86_AL));
    emit (X86_Stm_new_extendAl ());
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }

  case OP_NE:{
    X86_Operand_t newLeft;
    X86_Operand_t newRight;
    X86_Operand_t newDest;

    newLeft = Trans_operand (left);
    emit (X86_Stm_new_load (X86_EAX,
                            newLeft));
    newRight = Trans_operand(right);
    emit (X86_Stm_new_load (X86_EDX,
                            newRight));
    emit (X86_Stm_new_cmp (X86_EAX,
                           X86_EDX));
    emit (X86_Stm_new_setne (X86_AL));
    emit (X86_Stm_new_extendAl ());
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }
  */
  default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}

static void genUop (Machine_Operand_t src,
                    Operator_t op,
                    Machine_Operand_t dest)
{
  switch (op){
  case OP_NEG:{
    X86_Operand_t newSrc, newDest;

    newSrc = Trans_operand(src);
    emit (X86_Stm_new_load (X86_EAX,
                            newSrc));
    emit (X86_Stm_new_neg (X86_EAX));
    newDest = Trans_operand(dest);
    emit (X86_Stm_new_store (newDest,
                             X86_EAX));
    return;
  }
  default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}

/* Convention for binary operation:
 *   x   bop y
 *   |       |
 *   \/      \/
 *  eax     edx
 * For some operation, this has the advantage of
 * simplifying the generated instructions.
 */
static void Trans_stm (Machine_Stm_t s)
{


  Assert_ASSERT(s);
  switch (s->kind){
  case MACHINE_STM_MOVE:
  case MACHINE_STM_MOVE_DTOI:
  case MACHINE_STM_MOVE_ITOD:
	{
    X86_Stm_t s1, s2,s3;
    X86_Operand_t newSrc, newDest;
    newSrc = Trans_operand (s->u.move.src);
	newDest = Trans_operand (s->u.move.dest);
	s3=LinkedList_getLast(allStms);
	/*
	if((newSrc->kind==X86_OP_INT||newSrc->kind==X86_OP_DOUBLE)&&newDest->kind!=X86_OP_INSTACK)
	{
	//firstech move move from src to dest directly
       emit(Firstech_Stm_new_move(newDest,newSrc)) ;
       return ;
	}
	*/
	if(s3&&(s3->kind==FIRSTECH_STM_BOP||s3->kind==FIRSTECH_STM_CMP))
	{
    printf("aaaaaaaaaaaaaaaaaa\n");
 //    emit(X86_Stm_new_store (newDest, X86_EBX));
//	 return ;
	}
    s1 = X86_Stm_new_load (X86_EAX, newSrc);
    emit (s1);
	if(s->kind==MACHINE_STM_MOVE_DTOI)
	emit (X86_Stm_new_conv (X86_EAX,DTOI));
	else if(s->kind==MACHINE_STM_MOVE_ITOD)
	emit (X86_Stm_new_conv (X86_EAX,ITOD));
    newDest = Trans_operand (s->u.move.dest);
    s2 = X86_Stm_new_store (newDest, X86_EAX);
    emit (s2);
    return;
  }
  case MACHINE_STM_BOP:{
    genBop (s->u.bop.left,
            s->u.bop.op,
            s->u.bop.right,
            s->u.bop.dest);
    return;
  }
  case MACHINE_STM_UOP:{
    genUop (s->u.uop.src,
            s->u.uop.op,
            s->u.uop.dest);
    return;
  }
  case MACHINE_STM_CALL:{
    int n = 0;
    List_t args = LinkedList_getFirst
      (LinkedList_rev (s->u.call.args));
    X86_Operand_t newDest = Trans_operand (s->u.call.dest);
    while (args){
      Machine_Operand_t src =
        (Machine_Operand_t)args->data;
      X86_Operand_t newSrc = Trans_operand (src);
      emit (X86_Stm_new_load (X86_EAX, newSrc));
      emit (X86_Stm_new_push (X86_EAX));
      n++;
      args = args->next;
    }
    emit (X86_Stm_new_call
          (Id_fromString
           (String_concat ("_",
                           Id_toString (s->u.call.name),
                           0))));
	//our ram addr add by 1,aha ya ...
    emit (X86_Stm_new_load (X86_EDX,
                            X86_Operand_new_double ((double)n)));
    emit (Firstech_Stm_new_bop (X86_EDX,
                           OP_ADD,
                           X86_ESP,
                           X86_ESP));
    emit (X86_Stm_new_store (newDest, X86_EAX));
    return;
  }
  case MACHINE_STM_IF:{
  	X86_Stm_t s3;
  	s3=LinkedList_getLast(allStms);
	 X86_Operand_t newSrc = Trans_operand(s->u.iff.src);
	if(s3->kind==FIRSTECH_STM_CMP)
		{
       emit (Firstech_Stm_new_je (s->u.iff.truee,X86_EBX));
	   emit (X86_Stm_new_jump (s->u.iff.falsee));
	   return ;
	}

    emit (X86_Stm_new_load (X86_EAX, newSrc));
   // emit (X86_Stm_new_load (X86_EDX,
   //                         X86_Operand_new_int (0)));
   // emit (X86_Stm_new_cmp (X86_EAX, X86_EDX));
    emit (Firstech_Stm_new_je (s->u.iff.truee,X86_EAX));
    emit (X86_Stm_new_jump (s->u.iff.falsee));
    return;
  }
  case MACHINE_STM_LABEL:
    emit (X86_Stm_new_label (s->u.label));
    return;
  case MACHINE_STM_JUMP:
    emit (X86_Stm_new_jump (s->u.jump));
    return;
  case MACHINE_STM_RETURN:{
    X86_Operand_t newSrc
      = Trans_operand (s->u.returnn);

    emit (X86_Stm_new_load (X86_EAX, newSrc));
    emit (X86_Stm_new_return ());
    return;
  }
  case MACHINE_STM_RETURNVOID:
  	emit (X86_Stm_new_return ());
	return;
  case MACHINE_STM_NEW_PRIMITIVE:{
    //int n = 0;

	List_t newArgs =
      LinkedList_map (s->u.infuc.args,
                      (Poly_tyId)Trans_operand);
    emit (X86_Stm_new_primitive (s->u.infuc.iFun, newArgs));

    return;
  }
  case MACHINE_STM_END:
  	emit(Firstech_Stm_new_end());
    return ;
  case MACHINE_STM_UPDATE:
  	emit(Firstech_Stm_new_update());
    return ;
  case MACHINE_STM_NEW_STRUCT:{
    int n = 0;
    X86_Operand_t newDest;
    List_t args =
      LinkedList_getFirst
      (LinkedList_rev(s->u.newStruct.args));
    while (args){
      Machine_Operand_t src =
        (Machine_Operand_t)args->data;
      X86_Operand_t newSrc = Trans_operand (src);
      emit (X86_Stm_new_load (X86_EAX, newSrc));
      emit (X86_Stm_new_push (X86_EAX));
      n++;
      args = args->next;
    }
    emit (X86_Stm_new_load (X86_EAX,
                            X86_Operand_new_int (n)));
    emit (X86_Stm_new_push (X86_EAX));
    emit (X86_Stm_new_load (X86_EAX,
                            X86_Operand_new_global
                            (s->u.newStruct.type)));
    emit (X86_Stm_new_push (X86_EAX));
    emit (X86_Stm_new_call
          (Id_fromString ("_Dragon_alloc_struct")));
    emit (X86_Stm_new_load (X86_EDX,
                            X86_Operand_new_int (8+4*n)));
    emit (X86_Stm_new_bop (X86_ESP,
                           OP_ADD,
                           X86_EDX));
    newDest = Trans_operand (s->u.newStruct.dest);
    emit (X86_Stm_new_store (newDest, X86_EAX));
    return;
  }
  case MACHINE_STM_NEW_ARRAY:{
    X86_Operand_t newDest, newSize, newInit;

    newInit = Trans_operand (s->u.newArray.init);
    emit (X86_Stm_new_load (X86_EAX,
                            newInit));
    emit (X86_Stm_new_push (X86_EAX));
    newSize = Trans_operand (s->u.newArray.size);
    emit (X86_Stm_new_load (X86_EAX,
                            newSize));
    emit (X86_Stm_new_push (X86_EAX));
    emit (X86_Stm_new_load
          (X86_EAX,
           X86_Operand_new_int (s->u.newArray.isPtr)));
    emit (X86_Stm_new_push (X86_EAX));
    emit (X86_Stm_new_call
          (Id_fromString ("_Dragon_alloc_array")));
    emit (X86_Stm_new_load (X86_EDX,
                            X86_Operand_new_int (12)));
    emit (X86_Stm_new_bop (X86_ESP,
                           OP_ADD,
                           X86_EDX));
    newDest = Trans_operand (s->u.newArray.dest);
    emit (X86_Stm_new_store (newDest, X86_EAX));
    return;
  }
  default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}

static List_t Trans_stms (List_t stms)
{
  List_t newStms;
  Assert_ASSERT(stms);
  allStms = LinkedList_new ();
  LinkedList_foreach (stms,
                      (Poly_tyVoid)Trans_stm);
  newStms = getBeforeClearStms ();
  return newStms;
}

static X86_Struct_t Trans_struct (Machine_Struct_t str)
{
  Assert_ASSERT(str);
  return X86_Struct_new (str->type,
                         str->var);
}

static X86_Fun_t Trans_func (Machine_Fun_t f)
{
  List_t stms;

  Assert_ASSERT(f);
  stms = Trans_stms (f->stms);
  return X86_Fun_new
    (f->globals,
     f->type,
     f->name,
     LinkedList_map (f->args,
                     (Poly_tyId)Trans_struct),
     LinkedList_map (f->decs,
                     (Poly_tyId)Trans_struct),
     stms,
     f->retId,
     f->entry,
     f->exitt,
     f->inter);
}

static X86_Str_t Trans_str (Machine_Str_t s)
{
  Assert_ASSERT(s);
  return X86_Str_new (s->name,
                      s->value);
}

static X86_Mask_t Trans_mask (Machine_Mask_t m)
{
  Assert_ASSERT(m);
  return X86_Mask_new (m->name,
                       m->size,
                       m->index);
}

static X86_Prog_t X86_codegenTraced (Machine_Prog_t p)
{
  List_t strs, masks, funcs;
  Assert_ASSERT(p);
  strs = LinkedList_map (p->strings,
                         (Poly_tyId)Trans_str);
  masks = LinkedList_map (p->masks,
                          (Poly_tyId)Trans_mask);
  funcs = LinkedList_map (p->funcs,
                          (Poly_tyId)Trans_func);
  return X86_Prog_new (strs,
                       masks,
                       funcs);
}

X86_Prog_t X86_codegen (Machine_Prog_t p)
{
  return X86_codegenTraced (p);
}

#undef List_t
