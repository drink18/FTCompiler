#include "../lib/mem.h"
#include "../lib/todo.h"
#include "../lib/int.h"
#include "../lib/float.h"

#include "../lib/char-buffer.h"
#include "../lib/assert.h"
#include "../control/control.h"
#include "../lib/internalFunc.h"



#include "x86.h"

#define List_t LinkedList_t

#define P X86_Prog_t
#define Str X86_Str_t
#define F X86_Fun_t
#define S X86_Stm_t
#define O X86_Operand_t
#define D X86_Dec_t
#define Stt X86_Struct_t
#define M X86_Mask_t
#define R X86_Register_t


#define ONEM (1024*1024)
#define NUM 16

static char buffer[16*ONEM];

static int writeNums = 0;

static void buffer_init ()
{
  /* int n = (Control_bufferSize>0) */
  /*   ? (Control_bufferSize): NUM; */
  /* n *= ONEM; */
  /* Mem_NEW_SIZE (buffer, n); */
}

static void buffer_final ()
{
  /* buffer = 0; */
}

/* try to speed up this operation on rather large files */
static File_t file = 0;
static void file_init (File_t f)
{
  file = f;
  setbuf (file, buffer);
  return;
}
static void print (String_t s)
{
  writeNums++;
  fprintf (file, "%s", s);
  if (writeNums>1000){
    fflush (file);
    writeNums=0;
  }
  return;
}
void Firstech_Primitive_print(InternalFun r)
{
  switch(r){
   case DELAY:
   	print("delay");
	return;
   case SETR:
   	print("setr");
	return;
   case TRIGGER:
   	print("trigger");
	return;
   case WAIT:
   	print("wait");
	return;
   case TIMER:
   	print("timer");
	return;
   default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return ;

}
int X86_Register_equals (R r1, R r2)
{
  return r1 == r2;
}
void Firstech_Type_transfer(Conv_type r)
{
   switch(r)
   	{
    case DTOI:
	print ("dtoi");
	return ;
	case ITOD:
	print ("itod");
	return ;
	default:
    Error_impossible ();
	return;

    }
   Error_impossible ();
  return;


}
void X86_Register_print (R r)
{
  switch (r){
  case X86_AL:
    print ("%al");
    return;
  case X86_EAX:
    print ("%eax");
    return;
  case X86_EBX:
    print ("%ebx");
    return;
  case X86_ECX:
    print ("%ecx");
    return;
  case X86_EDX:
    print ("%edx");
    return;
  case X86_EDI:
    print ("%edi");
    return;
  case X86_ESI:
    print ("%esi");
    return;
  case X86_EBP:
    print ("%ebp");
    return;
  case X86_ESP:
    print ("%esp");
    return;
  case X86_EIP:
  	print("%eip");
	return ;
  default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}

O X86_Operand_new_int (int i)
{
  O e;
  Mem_NEW (e);
  e->kind = X86_OP_INT;
  e->u.intlit = i;
  return e;
}
O X86_Operand_new_float (float i)
{
  O e;
  Mem_NEW (e);
  e->kind = X86_OP_FLOAT;
  e->u.floatlit = i;
  return e;
}
O X86_Operand_new_double (double i)
{
  O e;
  Mem_NEW (e);
  e->kind = X86_OP_DOUBLE;
  e->u.doublelit = i;
  return e;
}

O X86_Operand_new_global (Id_t id)
{
  O e;
  Mem_NEW (e);
  e->kind = X86_OP_GLOBAL;
  e->u.global = id;
  return e;
}

O X86_Operand_new_inStack (int index)
{
  O e;
  Mem_NEW (e);
  e->kind = X86_OP_INSTACK;
  e->u.index = index;
  return e;
}

O X86_Operand_new_reg (R r)
{
  O e;
  Mem_NEW (e);
  e->kind = X86_OP_REG;
  e->u.reg = r;
  return e;
}

O X86_Operand_new_mem (R base, int offset)
{
  O e;
  Mem_NEW (e);
  e->kind = X86_OP_MEM;
  e->u.mem.base = base;
  e->u.mem.offset = offset;
  return e;
}

int X86_Operand_sameStackSlot (O x, O y)
{
  if (x->kind != X86_OP_INSTACK
      || y->kind != X86_OP_INSTACK)
    return 0;
  return x->u.index == y->u.index;
}

void X86_Operand_print (O o)
{
  Assert_ASSERT(o);
  switch (o->kind){
  case X86_OP_INT:
    print ("$");
    print (Int_toString (o->u.intlit));
    return;
  case X86_OP_FLOAT:
    print ("$F");
    print (float_toString (o->u.floatlit));
    return;
  case X86_OP_DOUBLE:
    print ("$F_");
    print (double_toString2 (o->u.doublelit));
    return;
  case X86_OP_GLOBAL:
    print ("$");
    print (Id_toString (o->u.global));
    return;
  case X86_OP_INSTACK:
    print (Int_toString (o->u.index));
    print ("(%ebp)");
    return;
  case X86_OP_REG:
    X86_Register_print (o->u.reg);
    return;
  case X86_OP_MEM:
    print (Int_toString (4*(o->u.mem.offset)));
    print ("(");
    X86_Register_print (o->u.mem.base);
    print (")");
    return;
  default:
    Error_impossible();
    return;
  }
  Error_impossible();
  return;
}

S X86_Stm_new_moverr (R dest, R src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_MOVERR;
  s->u.moverr.dest = dest;
  s->u.moverr.src = src;
  return s;
}

S X86_Stm_new_moveri (R dest, int src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_MOVERI;
  s->u.moveri.dest = dest;
  s->u.moveri.src = src;
  return s;
}

S Firstech_Stm_new_move (O dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_MOVE;
  s->u.firstechmove.dest = dest;
  s->u.firstechmove.src = src;
  return s;
}

S X86_Stm_new_load (R dest, O src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_LOAD;
  s->u.load.dest = dest;
  s->u.load.src = src;
  return s;
}

S Firstech_Stm_new_adr(R dest,O src)
{
	S s;
	Mem_NEW (s);
	s->kind = FIRSTECH_STM_ADR;
	s->u.load.dest = dest;
	s->u.load.src = src;
	return s;
}

S X86_Stm_new_conv (R src,Conv_type ct)
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_CONV;
  s->u.conv.src = src;
  s->u.conv.ct=ct;
  //s->u.load.src = src;
  return s;
}

S X86_Stm_new_primitive (InternalFun iFun, List_t args)
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_PRIMITIVE;
  s->u.infuc.iFun = iFun;
  s->u.infuc.args = args;
 // s->u.call.args = args;
  return s;
}
S Firstech_Stm_new_end ()
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_END;
 // s->u.call.args = args;
  return s;
}
S Firstech_Stm_new_update ()
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_UPDATE;
 // s->u.call.args = args;
  return s;
}

S X86_Stm_new_store (O dest, R src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_STORE;
  s->u.store.dest = dest;
  s->u.store.src = src;
  return s;
}

S X86_Stm_new_bop (R dest, Operator_t opr,
                   R src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_BOP;
  s->u.bop.src = src;
  s->u.bop.op = opr;
  s->u.bop.dest = dest;
  return s;
}

S Firstech_Stm_new_bop (R dest, Operator_t opr,
                   R src,R result)
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_BOP;
  s->u.fbop.src = src;
  s->u.fbop.op = opr;
  s->u.fbop.dest = dest;
  s->u.fbop.result = result;
  return s;
}


S X86_Stm_new_uop (R dest, Operator_t opr, R src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_UOP;
  s->u.uop.dest = dest;
  s->u.uop.src = src;
  s->u.uop.op = opr;
  return s;
}

S X86_Stm_new_call (Id_t f)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_CALL;
  s->u.call.name = f;
  return s;
}

S X86_Stm_new_cmp (R dest, R src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_CMP;
  s->u.cmp.src = src;
  s->u.cmp.dest = dest;
  return s;
}


S Firstech_Stm_new_cmp (R dest,Operator_t opr, R src ,R result)
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_CMP;
  s->u.fcmp.src = src;
  s->u.fcmp.dest = dest;
  s->u.fcmp.op=opr;
  s->u.fcmp.result = result;
  return s;
}






S X86_Stm_new_label (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_LABEL;
  s->u.label = label;
  return s;
}

S X86_Stm_new_je (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_JE;
  s->u.je = label;
  return s;
}

S Firstech_Stm_new_je (Label_t label,R conditionAddr)
{
  S s;
  Mem_NEW (s);
  s->kind = FIRSTECH_STM_JE;
  s->u.fje.je = label;
  s->u.fje.fcon=conditionAddr;
  return s;
}

S X86_Stm_new_jl (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_JL;
  s->u.jl = label;
  return s;
}

S X86_Stm_new_jump (Label_t label)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_JUMP;
  s->u.jump = label;
  return s;
}

S X86_Stm_new_push (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_PUSH;
  s->u.push = r;
  return s;
}

S X86_Stm_new_neg (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_NEG;
  s->u.neg = r;
  return s;
}

S X86_Stm_new_setl (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_SETL;
  s->u.setAny = r;
  return s;
}

S X86_Stm_new_setle (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_SETLE;
  s->u.setAny = r;
  return s;
}

S X86_Stm_new_setg (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_SETG;
  s->u.setAny = r;
  return s;
}

S X86_Stm_new_setge (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_SETGE;
  s->u.setAny = r;
  return s;
}

S X86_Stm_new_sete (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_SETE;
  s->u.setAny = r;
  return s;
}

S X86_Stm_new_setne (X86_Register_t r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_SETNE;
  s->u.setAny = r;
  return s;
}

S X86_Stm_new_extendAl ()
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_EXTENDAL;
  return s;
}

S X86_Stm_new_return ()
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_RETURN;
  return s;
}

S X86_Stm_new_cltd ()
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_CLTD;
  return s;
}

S X86_Stm_new_xor (R dest, R src)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_XOR;
  s->u.xor.dest = dest;
  s->u.xor.src = src;
  return s;
}

S X86_Stm_new_not (R r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_NOT;
  s->u.not = r;
  return s;
}

S X86_Stm_new_inc (R r)
{
  S s;
  Mem_NEW (s);
  s->kind = X86_STM_INC;
  s->u.inc = r;
  return s;
}

static void X86_Operator_print (Operator_t o)
{
  switch (o&0xFFFF){
  case OP_ADD:
    print ("addl ");
    return;
  case OP_SUB:
    print ("subl ");
    return;
  case OP_TIMES:
    print ("imull ");
    return;
  case OP_DIVIDE:
    print ("idivl ");
    return;
  case OP_GT:
    print ("cmpgt ");
	return;
  case OP_GE:
    print ("cmpge ");
	return;
  case OP_LT:
    print ("cmplt ");
    return;
  case OP_LE:
    print ("cmple ");
    return;
  case OP_NE:
    print ("cmpne ");
    return;
  case OP_EQ:
    print ("cmpeg ");
    return;
  case OP_NEG:
    print ("cmpneg ");
    return;
  default:
    TODO;
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}

static void space2()
{
}

static void space4 ()
{
  print ("\t");
}

void Firstech_Stm_print (S s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case X86_STM_MOVERR:
    space4 ();
    print ("movl ");
    X86_Register_print (s->u.moverr.src);
    print (", ");
    X86_Register_print (s->u.moverr.dest);
    break;
  case FIRSTECH_STM_ADR:
	  space4 ();
	  print ("adr ");
	  X86_Register_print (s->u.firstechadr.dest);
	  print (", (");
	  X86_Operand_print (s->u.firstechadr.src);
	  print (")");
	break;
  case X86_STM_MOVERI:
    space4 ();
    print ("movl $");
    print (Int_toString (s->u.moveri.src));
    print (", ");
    X86_Register_print (s->u.moveri.dest);
    break;
  case FIRSTECH_STM_CONV:
    space4 ();
    print ("conv_");
	Firstech_Type_transfer(s->u.conv.ct);
    space4 ();
    X86_Register_print (s->u.conv.src);
    break;
  case X86_STM_LOAD:
    space4 ();
    print ("movl ");
    X86_Operand_print (s->u.load.src);
    print (", ");
    X86_Register_print (s->u.load.dest);
    break;
   case FIRSTECH_STM_MOVE:
    space4 ();
    print ("movl ");
    X86_Operand_print (s->u.firstechmove.src);
    print (", ");
    X86_Operand_print (s->u.firstechmove.dest);
    break;
   case FIRSTECH_STM_BOP:
    space4 ();
    X86_Operator_print (s->u.fbop.op);
    X86_Register_print (s->u.fbop.src);
    print (", ");
    X86_Register_print (s->u.fbop.dest);
	print (", ");
	X86_Register_print (s->u.fbop.result);
    break;
  case X86_STM_STORE:
    space4 ();
    print ("movl ");
    X86_Register_print (s->u.store.src);
    print (", ");
    X86_Operand_print (s->u.store.dest);
    break;
  case X86_STM_BOP:
    space4 ();
    X86_Operator_print (s->u.bop.op);
    X86_Register_print (s->u.bop.src);
    print (", ");
    X86_Register_print (s->u.bop.dest);
    break;
  case FIRSTECH_STM_CMP:
    space4 ();
    X86_Operator_print (s->u.fcmp.op);
    X86_Register_print (s->u.fcmp.src);
    print (", ");
    X86_Register_print (s->u.fcmp.dest);
	print (", ");
	X86_Register_print (s->u.fcmp.result);
    break;
  case X86_STM_UOP:
    space4 ();
    X86_Operator_print (s->u.uop.op);
    X86_Register_print (s->u.uop.dest);
    break;
  case X86_STM_CALL:
    space4 ();
	 //print ("pushl %EIP");
    print ("subl $1, %esp");
	print("\n\t");
	print("movl ");
    X86_Register_print (X86_EIP);
	print(", (%esp)");
	print("\n\t");
    print ("jmp ");
    print (Id_toString (s->u.call.name));
    break;

  case X86_STM_CMP:
    space4 ();
    print ("cmpl ");
    //X86_Operator_print
    X86_Register_print (s->u.cmp.src);
    print (", ");
    X86_Register_print (s->u.cmp.dest);
    break;

  case X86_STM_LABEL:
    space2();
    print (Label_toString (s->u.label));
    print (":");
    break;
  case X86_STM_JE:
    space4 ();
    print ("je ");
    print (Label_toString (s->u.je));
    break;
 case FIRSTECH_STM_JE:
    space4 ();
    print ("je ");
    print (Label_toString (s->u.fje.je));
	print (", ");
	X86_Register_print (s->u.fje.fcon);
    break;
  case X86_STM_JL:
    space4 ();
    print ("jl ");
    print (Label_toString (s->u.je));
    break;
  case X86_STM_JUMP:
    space4 ();
    print ("jmp ");
    print (Label_toString (s->u.jump));
    break;
  case X86_STM_PUSH:
    space4 ();

    //print ("pushl ");
    print ("subl $1, %esp");
	print("\n\t");
	print("movl ");
    X86_Register_print (s->u.push);
	print(", (%esp)");
    break;
  case X86_STM_NEG:
    space4 ();
    print ("negl ");
    X86_Register_print (s->u.neg);
    break;
  case X86_STM_SETL:
    space4 ();
    print ("setl ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETLE:
    space4 ();
    print ("setle ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETG:
    space4 ();
    print ("setg ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETGE:
    space4 ();
    print ("setge ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETE:
    space4 ();
    print ("sete ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETNE:
    space4 ();
    print ("setne ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_XOR:
    space4 ();
    print ("xorl ");
    X86_Register_print (s->u.xor.src);
    print (", ");
    X86_Register_print (s->u.xor.dest);
    break;
  case X86_STM_EXTENDAL:
    space4 ();
    print ("cbtw\n");
    space4 ();
    print ("cwtl");
    break;
  case X86_STM_NOT:{
    space4 ();
    print ("notl ");
    X86_Register_print (s->u.not);
    break;
  }
  case X86_STM_RETURN:
    space4 ();
	// leave and return
	print("movl %ebp, %esp");
	print ("\n\t");
	print("movl (%esp), %ebp");
	print ("\n\t");
	print("addl $1, %esp");

    //print ("\n\tret");
    print("\n\t");
	print("movl (%esp), %eip");
	print ("\n\t");
	print("addl $1, %esp");
	print ("\n\t");
	print("jmp  (%eip)");
    break;
  case X86_STM_CLTD:
    space4 ();
    print ("cltd");
    break;
  case X86_STM_INC:
    space4 ();
    print ("incl ");
    X86_Register_print (s->u.inc);
    break;
  case FIRSTECH_STM_END:
  	space4 ();
    print ("end ");
  	break;
  case FIRSTECH_STM_UPDATE:
  	space4 ();
    print ("update ");
  	break;
  case FIRSTECH_STM_PRIMITIVE:
  		{
  	space4 ();
    //print ("update ");
	Firstech_Primitive_print(s->u.infuc.iFun);
	//X86_Operand_print (s->u.move.dest);
	List_t args=s->u.infuc.args->next;
	while (args){
	  print(" ");
	  X86_Operand_print(args->data);

      args = args->next;
    }
  	break;}
  default:
    Error_impossible();
    break;
  }
  print ("\n");
  return;
}
void X86_Stm_print (S s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case X86_STM_MOVERR:
    space4 ();
    print ("movl ");
    X86_Register_print (s->u.moverr.src);
    print (", ");
    X86_Register_print (s->u.moverr.dest);
    break;
  case X86_STM_MOVERI:
    space4 ();
    print ("movl $");
    print (Int_toString (s->u.moveri.src));
    print (", ");
    X86_Register_print (s->u.moveri.dest);
    break;
  case X86_STM_LOAD:
    space4 ();
    print ("movl ");
    X86_Operand_print (s->u.load.src);
    print (", ");
    X86_Register_print (s->u.load.dest);
    break;
  case X86_STM_STORE:
    space4 ();
    print ("movl ");
    X86_Register_print (s->u.store.src);
    print (", ");
    X86_Operand_print (s->u.store.dest);
    break;
  case X86_STM_BOP:
    space4 ();
    X86_Operator_print (s->u.bop.op);
    X86_Register_print (s->u.bop.src);
    print (", ");
    X86_Register_print (s->u.bop.dest);
    break;
  case X86_STM_UOP:
    space4 ();
    X86_Operator_print (s->u.uop.op);
    X86_Register_print (s->u.uop.dest);
    break;
  case X86_STM_CALL:
    space4 ();
    print ("call ");
    print (Id_toString (s->u.call.name));
    break;
  case X86_STM_CMP:
    space4 ();
    print ("cmpl ");
    X86_Register_print (s->u.cmp.src);
    print (", ");
    X86_Register_print (s->u.cmp.dest);
    break;
  case X86_STM_LABEL:
    space2();
    print (Label_toString (s->u.label));
    print (":");
    break;
  case X86_STM_JE:
    space4 ();
    print ("je ");
    print (Label_toString (s->u.je));
    break;
  case X86_STM_JL:
    space4 ();
    print ("jl ");
    print (Label_toString (s->u.je));
    break;
  case X86_STM_JUMP:
    space4 ();
    print ("jmp ");
    print (Label_toString (s->u.jump));
    break;
  case X86_STM_PUSH:
    space4 ();
    print ("pushl ");
    X86_Register_print (s->u.push);
    break;
  case X86_STM_NEG:
    space4 ();
    print ("negl ");
    X86_Register_print (s->u.neg);
    break;
  case X86_STM_SETL:
    space4 ();
    print ("setl ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETLE:
    space4 ();
    print ("setle ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETG:
    space4 ();
    print ("setg ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETGE:
    space4 ();
    print ("setge ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETE:
    space4 ();
    print ("sete ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_SETNE:
    space4 ();
    print ("setne ");
    X86_Register_print (s->u.setAny);
    break;
  case X86_STM_XOR:
    space4 ();
    print ("xorl ");
    X86_Register_print (s->u.xor.src);
    print (", ");
    X86_Register_print (s->u.xor.dest);
    break;
  case X86_STM_EXTENDAL:
    space4 ();
    print ("cbtw\n");
    space4 ();
    print ("cwtl");
    break;
  case X86_STM_NOT:{
    space4 ();
    print ("notl ");
    X86_Register_print (s->u.not);
    break;
  }
  case X86_STM_RETURN:
    space4 ();
    print ("leave\n\tret");
    break;
  case X86_STM_CLTD:
    space4 ();
    print ("cltd");
    break;
  case X86_STM_INC:
    space4 ();
    print ("incl ");
    X86_Register_print (s->u.inc);
    break;
  default:
    Error_impossible();
    break;
  }
  print ("\n");
  return;
}

F X86_Fun_new (List_t globals,Id_t type, Id_t name, List_t args,
               List_t decs, List_t stms,
               Id_t retId,
               Label_t entry, Label_t exitt,bool inter)
{
  F f;
  Mem_NEW (f);
  f->globals = globals;
  f->type = type;
  f->name = name;
  f->args = args;
  f->decs = decs;
  f->stms = stms;
  f->retId = retId;
  f->entry = entry;
  f->exitt = exitt;
  f->inter=inter;
  return f;
}


static void X86_Dec_print(X86_Struct_t);

void X86_Fun_print (F f)
{
  Assert_ASSERT(f);
  print ("\t.text\n");
  print ("\t.globl _");
  print (Id_toString (f->name));
  print ("\n_");
  print (Id_toString (f->name));
  print (":\n");
  print ("\tpushl %ebp\n");
  print ("\tmovl %esp, %ebp\n");
  {
    int i = LinkedList_size (f->decs);
    if (i){
      print ("\tsubl $");
      print (Int_toString (4*i));
      print (", %esp\n");
    }
  }
  LinkedList_foreach (f->stms,
                      (Poly_tyVoid)X86_Stm_print);
  print ("\n\n");
  return;
}
void Firstech_Global_print(Machine_Global_Var_t v)
{

   print (Id_toString(v->var));
   print(":\t");
   print (v->type);
   space4();
   print (longInt_toString(v->init_value));
   print("\n");

}
void Firstech_Fun_print (F f)
{
  Assert_ASSERT(f);
  if(f->globals)
  	{
  	  print ("\t.data\n");
      print ("\t.globl _");
      print (Id_toString (f->name));
	  print(" board\n");
      print ("\n_");
  	  //print (Id_toString (f->name));
  	    LinkedList_foreach (f->globals,
                      (Poly_tyVoid)Firstech_Global_print);

  }

  print ("\t.text\n");
  print ("\t.globl _");
  print (Id_toString (f->name));
  print ("\n_");
  print (Id_toString (f->name));
  print (":\n");

  if(!f->globals)
  	{
  //print ("\tpushl %ebp\n");
  print ("\tsubl $1, %esp\n");
  print ("\tmovl %ebp, (%esp)\n");
  //end push
  print ("\tmovl %esp, %ebp\n");
  	}
  {
  	//decs size; aha in firstech platform addr is add by 1
    int i = LinkedList_size (f->decs);
    if (i){
      print ("\tsubl $");
      print (Int_toString (i));
      print (", %esp\n");
    }
  }
  LinkedList_foreach (f->stms,
                      (Poly_tyVoid)Firstech_Stm_print);
  print ("\n\n");
  return;
}


Stt X86_Struct_new (Id_t type, Id_t var)
{
  Stt x;
  Mem_NEW (x);
  x->type = type;
  x->var = var;
  return x;
}

void X86_Struct_print (Stt x)
{
  Assert_ASSERT(x);
  print (Id_toString (x->type));
  print (" ");
  print (Id_toString (x->var));
  print (";\n");
}

static void X86_Dec_print (Stt x)
{
  Assert_ASSERT(x);
  print ("\t");
  print (Id_toString (x->type));
  print (" ");
  print (Id_toString (x->var));
  print (";\n");
}


Str X86_Str_new (Id_t name, String_t value)
{
  Str d;
  Mem_NEW (d);
  d->name = name;
  d->value = value;
  return d;
}
//// why need convert
static String_t convert (String_t s)
{
  CharBuffer_t buffer = CharBuffer_new ();

  while (*s){
    char c = *s;
    switch (c){
    case '\n':
      CharBuffer_append (buffer, '\\');
      CharBuffer_append (buffer, 'n');
      break;
    case '\t':
      CharBuffer_append (buffer, '\\');
      CharBuffer_append (buffer, 't');
      break;
    case '\\':
      CharBuffer_append (buffer, '\\');
      CharBuffer_append (buffer, '\\');
      break;
    case '\"':
      CharBuffer_append (buffer, '\\');
      CharBuffer_append (buffer, '\"');
      break;
    default:
      CharBuffer_append (buffer, c);
      break;
    }
    s++;
  }
  return CharBuffer_toStringBeforeClear (buffer);
}

void X86_Str_print (Str s)
{
  print (Id_toString (s->name));
  print (":\n\t.string \"");
  print (convert (s->value));
  print ("\"\n");
  return;
}

static void printStrs (LinkedList_t strings)
{
  if (LinkedList_isEmpty (strings))
    return;
  print ("\t.data\n");
  LinkedList_foreach (strings,
                      (Poly_tyVoid)X86_Str_print);
}

M X86_Mask_new (Id_t name, int size, List_t index)
{
  M m;
  Mem_NEW (m);
  m->name = name;
  m->size = size;
  m->index = index;
  return m;
}

void X86_Mask_print (M m)
{
  List_t p;

  Assert_ASSERT(m);
  print (Id_toString (m->name));
  print (":\n\t.int ");
  print (Int_toString (m->size));
  if (LinkedList_isEmpty (m->index)){
    print ("\n");
    return;
  }
  print (", ");
  p = LinkedList_getFirst (m->index);
  while (p){
    print (Int_toString ((int)p->data));
    if (p->next)
      print (", ");
    p = p->next;
  }
  print ("\n");
  return;
}

static void printMask (LinkedList_t ms)
{
  if (LinkedList_isEmpty (ms))
    return;
  print ("\t.data\n"
         "\t.align 8\n");
  LinkedList_foreach (ms,
		      (Poly_tyVoid)X86_Mask_print);
}

P X86_Prog_new (List_t strings, List_t masks,
                List_t funcs)
{
  P p;
  Mem_NEW (p);
  p->strings = strings;
  p->masks = masks;
  p->funcs = funcs;
  return p;
}

void X86_Prog_print (P x, File_t f)
{
  Assert_ASSERT(x);
  Assert_ASSERT(f);
  file_init (f);
  buffer_init ();

  printStrs (x->strings);

  print ("\n");
  printMask (x->masks);

  print ("\n");
  LinkedList_foreach (x->funcs,
                      (Poly_tyVoid)X86_Fun_print);
  buffer_final ();
}

void Firstech_Prog_print (P x, File_t f)
{
  Assert_ASSERT(x);
  Assert_ASSERT(f);
  file_init (f);
  buffer_init ();

  // this print string data;
  printStrs (x->strings);

  print ("\n");
  printMask (x->masks);

  print ("\n");
  LinkedList_foreach (x->funcs,
                      (Poly_tyVoid)Firstech_Fun_print);
  buffer_final ();
}


#undef P
#undef Str
#undef F
#undef S
#undef O
#undef M
#undef D
#undef Stt
#undef List_t
