#include "../lib/assert.h"
#include "../lib/linkedList.h"
#include "../lib/property.h"
#include "../lib/error.h"


#include "trans-tac.h"
#include "../ast/ast.h"
#include "../elaborate/env.h"   //added by LHF



#define List_t LinkedList_t

/* List <Machine_Str_t> */
static List_t strings = 0;
static List_t currentBoard = 0;
extern List_t globals;
extern List_t s_globals;

static Id_t genStr (String_t s)
{
  Id_t id = Id_newNoName ();
  Machine_Str_t str = Machine_Str_new (id, s);
  LinkedList_insertLast (strings, str);
  return id;
}




static void genGlobal(Id_t id, Id_t type,long long int value,bool sta,int size)
{
    //first search then insert
   // Machine_Global_Var_t p;
    List_t p;
	p=currentBoard->next;
	while(p)
		{
             if(Id_equals(id,((Machine_Global_Var_t)(p->data))->var))
			 	return ;
			 p=p->next;
	}
//	printf("genGlobal type=%s\n",Id_toString(type));
    Machine_Global_Var_t str = Machine_global_new (id, Id_toString(type),value,sta,size);
    LinkedList_insertLast(currentBoard,str);
}

static List_t getBeforeClearStrings ()
{
  List_t t = strings;
  strings = 0;
  return t;
}

/* List<Machine_Mask_t> */
static List_t masks = 0;

static void genMask (Id_t name, int size, List_t index)
{
  Machine_Mask_t m =
    Machine_Mask_new (name, size, index);
  LinkedList_insertLast (masks, m);
  return;
}

static List_t getBeforeClearMasks ()
{
  List_t t = masks;
  masks = 0;
  return t;
}


/* id -> int */
static Property_t typeProp = 0;
static Property_t polyArray = 0;
static Property_t indexProp = 0;
//static Property_t indexProp = 0;


/* id -> type*/

static void propInit ()
{
  typeProp = Property_new ((Poly_tyPlist)Id_plist);
  polyArray = Property_new ((Poly_tyPlist)Id_plist);
  indexProp = Property_new ((Poly_tyPlist)Id_plist);
}


static Machine_Operand_t Trans_operand (Tac_Operand_t e)
{
  Assert_ASSERT(e);
  switch (e->kind){
  case TAC_OP_INT:
    return Machine_Operand_new_int (e->u.intlit);
  case TAC_OP_FLOAT:
    return Machine_Operand_new_float (e->u.floatlit);
  case TAC_OP_DOUBLE:
    return Machine_Operand_new_double (e->u.doublelit);
  case TAC_OP_STR:{
    Id_t id = genStr (e->u.strlit);
    return Machine_Operand_new_global (id);
  }
  case TAC_OP_VAR:{
    int index = (int)Property_get (indexProp, e->u.var);

    if (index)
      return Machine_Operand_new_inStack (index);

	//first find in static list then in global list


	Ast_Dec_t b;

	 b = Ast_Dec_looup(globals,AstId_new(e->u.var,Region_bogus ()));
	if(b)
		{

		genGlobal(e->u.var,AstId_toId(b->type),Ast_E_int(b->init),b->sta,1);

	  return Machine_Operand_new_global (e->u.var);
		}
    return Machine_Operand_new_var (e->u.var);
  }
  case TAC_OP_STRUCT:{
    int index0 = (int)Property_get (indexProp,
                                    e->u.structt.base);
    int index = (int)Property_get (indexProp,
                                   e->u.structt.offset);
    Assert_ASSERT(index0);
    Assert_ASSERT(index);
    return Machine_Operand_new_struct
      (index0,
       index,
       /* generate code for 64 bit machine? */
       4);
  }
  case TAC_OP_ARRAY:{
    Ast_Dec_t d;
    Env_Binding_t binding;
	 d = Ast_Dec_looup(globals,AstId_new(e->u.var,Region_bogus ()));
	if(d)
    {
        int array_size = Ast_E_int(d->init->u.newArray.size);
        binding = Genv_lookup(AstId_new(e->u.var,Region_bogus ()));
        if (binding)
        {
            printf("[tac] array_var=%s,type=%s,size=%d\n",AstId_toString(AstId_new(e->u.var,Region_bogus ())),AstId_toString(binding->type->u.array.eltType),array_size);
            genGlobal(e->u.var,AstId_toId(binding->type->u.array.eltType),Ast_E_int(d->init->u.newArray.init),d->sta,array_size);
        }

        //printf("[tac] array_var=%s,type=%s\n",AstId_toString(AstId_new(e->u.var,Region_bogus ())),AstId_toString(b->type));
		//
    }
    int index = (int)Property_get (indexProp,
                                   e->u.array.base);
    int index2 = (int)Property_get (indexProp,
                                    e->u.array.offset);

    Assert_ASSERT(index);
    Assert_ASSERT(index2);
    return Machine_Operand_new_array
      (e->u.var,index, index2, 1);            //modified by LHF
  }
  default:
    Error_impossible ();
    return 0;
  }
  Error_impossible ();
  return 0;
}

static Machine_Stm_t Trans_stm (Tac_Stm_t s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case TAC_STM_MOVE:
    return Machine_Stm_new_move
      (Trans_operand (s->u.move.dest),
       Trans_operand (s->u.move.src));
  case TAC_STM_MOVE_DTOI:
  	return Machine_Stm_new_move_dtoi
      (Trans_operand (s->u.move.dest),
       Trans_operand (s->u.move.src));
  case TAC_STM_MOVE_ITOD:
  	return Machine_Stm_new_move_itod
      (Trans_operand (s->u.move.dest),
       Trans_operand (s->u.move.src));
  case TAC_STM_BOP:{
    int index = (int)Property_get (indexProp,
                                   s->u.bop.dest);
    Machine_Operand_t dest;
    dest = (index)? (Machine_Operand_new_inStack (index)):
      (Machine_Operand_new_global (s->u.bop.dest));
    return Machine_Stm_new_bop
      (dest,
       Trans_operand (s->u.bop.left),
       s->u.bop.op,
       Trans_operand (s->u.bop.right));
  }
  case TAC_STM_UOP:{
    int index = (int)Property_get (indexProp,
                                   s->u.uop.dest);
    Machine_Operand_t dest =
      (index)? (Machine_Operand_new_inStack (index)):
      (Machine_Operand_new_global (s->u.uop.dest));
    return Machine_Stm_new_uop
      (dest,
       s->u.uop.op,
       Trans_operand (s->u.uop.src));
  }
  case TAC_STM_CALL:{
    int index =
      (int)Property_get (indexProp, s->u.call.dest);
    Machine_Operand_t dest =
      (index)? (Machine_Operand_new_inStack (index)):
      (Machine_Operand_new_global (s->u.newStruct.dest));
    List_t newArgs =
      LinkedList_map (s->u.call.args,
                      (Poly_tyId)Trans_operand);
    return Machine_Stm_new_call (dest,
                                 s->u.call.name,
                                 newArgs);
  }
  case TAC_STM_PRIMITIVE:
  {
   List_t newArgs =
      LinkedList_map (s->u.infuc.args,
                      (Poly_tyId)Trans_operand);
    return Machine_Stm_new_internalfunc (s->u.infuc.iFun,
                                 newArgs);

  }
  case TAC_STM_END:
  	return Machine_Stm_new_end();
  case TAC_STM_UPDATE:
  	return Machine_Stm_new_update();
  case TAC_STM_IF:
    return Machine_Stm_new_if
      (Trans_operand (s->u.iff.src),
       s->u.iff.truee,
       s->u.iff.falsee);
  case TAC_STM_LABEL:
    return Machine_Stm_new_label (s->u.label);
  case TAC_STM_JUMP:
    return Machine_Stm_new_jump (s->u.jump);
  case TAC_STM_RETURN:{
    int index =
      (int)Property_get (indexProp, s->u.returnn);
    if (index)
      return Machine_Stm_new_return
        (Machine_Operand_new_inStack (index));
    return Machine_Stm_new_return
      (Machine_Operand_new_global (s->u.returnn));
  }
  case TAC_STM_RETURNVOID:
  	return Machine_Stm_new_returnvoid ();
  case TAC_STM_NEW_STRUCT:{
    int index = (int)Property_get (indexProp,
                                   s->u.newStruct.dest);
    Machine_Operand_t dest =
      (index)? (Machine_Operand_new_inStack (index)):
      (Machine_Operand_new_global (s->u.newStruct.dest));
    List_t newArgs =
      LinkedList_map (s->u.newStruct.args,
                      (Poly_tyId)Trans_operand);
    return Machine_Stm_new_newStruct (dest,
                                      s->u.newStruct.type,
                                      newArgs);
  }
  case TAC_STM_NEW_ARRAY:{
    int isPtr;
    int index = (int)Property_get (indexProp,
                                   s->u.newArray.dest);
    Machine_Operand_t dest =
      (index)? (Machine_Operand_new_inStack (index)):
      (Machine_Operand_new_global (s->u.newArray.dest));
    int v = (int)Property_get (polyArray,
                               s->u.newArray.type);
    isPtr = (v)? 1: 0;
    return Machine_Stm_new_newArray
      (dest,
       s->u.newArray.type,
       isPtr,
       Trans_operand (s->u.newArray.size),
       Trans_operand (s->u.newArray.init));
  }
  default:
    Error_impossible ();
    return 0;
  }
  Error_impossible ();
  return 0;
}

static int argIndex = -1;
static Machine_Struct_t Trans_arg (Tac_Struct_t t)
{
  Assert_ASSERT(t);
  Property_set (indexProp, t->var, (Poly_t)argIndex--);
  return Machine_Struct_new (t->type,
                             t->var);
}

static int localIndex = 1;
static Machine_Struct_t Trans_dec (Tac_Struct_t t)
{
  Assert_ASSERT(t);
  Property_set (indexProp, t->var, (Poly_t)localIndex++);
  return Machine_Struct_new (t->type,
                             t->var);
}


static Machine_Fun_t Trans_funcOne (Tac_Fun_t f)
{
  List_t newArgs, newDecs, newStms,globals=0;
  globals=LinkedList_new ();
  currentBoard=globals;
  Assert_ASSERT(f);
  argIndex = -1;
  newArgs = LinkedList_map (f->args,
                            (Poly_tyId)Trans_arg);
  localIndex = 1;
  newDecs = LinkedList_map (f->decs,
                            (Poly_tyId)Trans_dec);
  newStms = LinkedList_map (f->stms,
                            (Poly_tyId)Trans_stm);
  return Machine_Fun_new (globals,
  	                      f->type,
                          f->name,
                          newArgs,
                          newDecs,
                          newStms,
                          f->retId,
                          f->entry,
                          f->exitt,
                          f->attr&0x01);

}

static void maskEachTd (Tac_Td_t td)
{
  Assert_ASSERT(td);
  Property_set (typeProp, td->name, (Poly_t)1);
}

static void cookStructFields (Id_t name, List_t l)
{
  int maskSize = 0;
  int counter = 0;
  List_t p;
  List_t index = LinkedList_new ();
  int indexForEachField = 1;

  Assert_ASSERT(l);
  p = LinkedList_getFirst (l);
  while (p){
    Tac_Struct_t stt = (Tac_Struct_t)p->data;
    Property_set (indexProp,
                  stt->var,
                  (Poly_t)indexForEachField++);
    if (Property_get (typeProp, stt->type)){
      maskSize++;
      LinkedList_insertLast (index, (Poly_t)counter);
    }
    counter++;
    p = p->next;
  }
  genMask (name, maskSize, index);
  return;
}

static void cookEachTd (Tac_Td_t td)
{
  Id_t name;
  Tac_Type_t type;

  Assert_ASSERT(td);
  name = td->name;
  type = td->type;
  switch (type->kind){
  case TAC_TYPE_STRUCT:{
    cookStructFields (name, type->u.structt);
    return;
  }
  case TAC_TYPE_ARRAY:{
    int v = (int)Property_get (typeProp, type->u.array);
    if (v)
      Property_set (polyArray, name, (Poly_t)1);
    return;
  }
  default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}

static void Trans_type (List_t tds)
{
  Assert_ASSERT(tds);
  LinkedList_foreach (tds,
                      (Poly_tyVoid)maskEachTd);
  LinkedList_foreach (tds,
                      (Poly_tyVoid)cookEachTd);
  return;
}

static Machine_Prog_t Trans_tacTraced (Tac_Prog_t p)
{
  List_t funcs;

  Assert_ASSERT(p);
  strings = LinkedList_new ();
  masks = LinkedList_new ();
  propInit ();
  Trans_type (p->tds);
  funcs = LinkedList_map (p->funcs,
                          (Poly_tyId)Trans_funcOne);
  return Machine_Prog_new (getBeforeClearStrings (),
                           getBeforeClearMasks (),
                           funcs);
}

Machine_Prog_t Trans_tac (Tac_Prog_t p)
{
  return Trans_tacTraced (p);
}

#undef List_t

