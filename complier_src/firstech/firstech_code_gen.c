#include "../lib/mem.h"
#include "../lib/todo.h"
#include "../lib/int.h"
#include "../lib/float.h"

#include "../lib/char-buffer.h"
#include "../lib/assert.h"
#include "../control/control.h"
#include "../lib/internalFunc.h"
#include "../lib/wavefile.h"

#include "Firstech_code_gen.h"
#include <windows.h>

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
#define GV Machine_Global_Var_t
#define W wavefile_t

#define ONEM (1024*1024)
#define NUM 16
#define INIT_SIZE 1025
//wave data / param/ one line most char is 20
// is char is over 20 filter anyway
#define MAX_LINE_CHAR 20

//wave file list
extern LinkedList_t wave_file_list;
static Hash_t boxssec=0;


static char buffer[16*ONEM];
static Hash_t  varhash=0;
static int writeNums = 0;
static Section *cur_text_section=0;
static int text_ind;
static int ram_addr;
static int const_1_addr=0;
static List_t prog_list=0;
String_t boxs[]={"M","E1","E2","E3","E4"};

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
void Firstech_Primitive_opcode(S s)
{

  List_t args=s->u.infuc.args;
  O o,o1;
  int i;
  long long int instr_code;
  InternalFun r	=s->u.infuc.iFun;
  ram_data *rd;
  switch(r){
   case DELAY:
   	{
	 args=args->next;
	 o=args->data;
	 if(o->kind==X86_OP_INT)
	 {

      instr_code=0x7;
	  instr_code=instr_code<<41|(o->u.intlit);
	  gen_le48(instr_code);

	 }
	 else if(o->kind==X86_OP_DOUBLE)
	 {
		instr_code=0x7;
	  instr_code=instr_code<<41|((int)o->u.doublelit);
	  gen_le48(instr_code);

	 }
	 else if(o->kind==X86_OP_INSTACK)
     {
       i=o->u.index&0xFFF;
       instr_code=0x6;
	   instr_code=instr_code<<41|(i<<12)|EBP;
	   gen_le48(instr_code);
     } else if(o->kind==X86_OP_GLOBAL)
     {
		relocation_sym *temp;
		if(!(temp=Hash_lookup(cur_text_section->hash_table,Id_toString (o->u.global))))
		 Error_impossible();

		instr_code=0xd;
		instr_code=instr_code<<40|temp->addr;
		gen_le48(instr_code);

	 }
	 else 	ErrorMsg_CodegenError("delay argument error,please check\n");

	return;
   	}
   case SETR:
   	{
	args=args->next;
	o=args->data;
	o1=args->next->data;
	 if(o->kind==X86_OP_INT)
	 {
	  if(o1->kind==X86_OP_INT)
	  	{
      instr_code=0xb;
	  instr_code=instr_code<<41|(o1->u.intlit<<12)|o->u.intlit;
	  gen_le48(instr_code);
	  	}
	   else  if(o1->kind==X86_OP_GLOBAL)
	  	{
      instr_code=0xa;
	  relocation_sym *temp;
		if(!(temp=Hash_lookup(cur_text_section->hash_table,Id_toString (o1->u.global))))
		 Error_impossible();
	  instr_code=instr_code<<41|(temp->addr<<12)|o->u.intlit;
	  gen_le48(instr_code);
	  	}
	  else if(o1->kind==X86_OP_INSTACK)
	  	{
	  	gen_le24(0x154<<12|(o1->u.index&0xFFF));
		gen_le24(EBP<<12|o->u.intlit);
	  	//instr_code=0x15;
		//instr_code=instr_code<<40|o1->u.intlit<<12|EBP;
		//i=o1->u.intlit<<12|EBP;
	  //instr_code=instr_code<<40|(i<<12)|o->u.intlit;
	  //gen_le48(instr_code);

	  	}
	  /*
	   else if(o1->kind==X86_OP_DOUBLE)
	   	{
			//first search in hash table
	   if(!(rd=check_ram_value(cur_text_section->ram_init_data,int_to_float(o1->u.doublelit))))
	   	{
	   rd=ram_data_new_val("CONST_Float",CONST_VAR,int_to_float(o->u.doublelit));
	   LinkedList_insertLast(cur_text_section->ram_init_data,rd);
	   	}

	   }*/
	   else ErrorMsg_CodegenError("setr second argument error\n");

	  } else if(o->kind==X86_OP_INSTACK)
	 	{
				  if(o1->kind==X86_OP_INT)
	  	{
	  	//mov %INT ,%EEX
	  	//mov %EEX,%op1
	  	instr_code=0xb;
	  instr_code=instr_code<<41|(o1->u.intlit<<12)|EEX;
	  gen_le48(instr_code);

		gen_le24(0x148<<12|(o->u.index&0xFFF));
		gen_le24(EEX<<12|EBP);

	  	}
	   else  if(o1->kind==X86_OP_GLOBAL)
	  	{
      instr_code=0x148<<12|(o->u.index&0xFFF);
	  relocation_sym *temp;
		if(!(temp=Hash_lookup(cur_text_section->hash_table,Id_toString (o1->u.global))))
		 Error_impossible();
	  instr_code=instr_code<<24|(temp->addr<<12)|EBP;
	  gen_le48(instr_code);
	  	}
	  else if(o1->kind==X86_OP_INSTACK)
	  	{
	  	//mov (%op1) ,%EEX
	  	//mov (%op2),(%EEX) (both indirect addr )

		gen_le24(0x154<<12|(o->u.index&0xFFF));
		gen_le24(EBP<<12|EEX);

	  	gen_le24(0x15C<<12|(o1->u.index&0xFFF));
		gen_le24(EBP<<12|EEX);

	  	}


	 }

	 else  	ErrorMsg_CodegenError("setr first argument error\n");

	return;
   	}
   case TRIGGER:
   	{
	 args=args->next;
	 o=args->data;
	 if(o->kind==X86_OP_INSTACK)
	 {
	  instr_code=0x11;
	  instr_code=instr_code<<41|((o->u.index&0xFFF)<<12)|EBP;
	  gen_le48(instr_code);
	 }else if(o->kind==X86_OP_INT)
	 {
	   instr_code=0x10;
	  instr_code=instr_code<<41|o->u.intlit;
	  gen_le48(instr_code);

	 }else if(o->kind==X86_OP_DOUBLE)
	 {
		instr_code=0x10;
	  instr_code=instr_code<<41|(int)o->u.doublelit;
	  gen_le48(instr_code);
	 }
	 else ErrorMsg_CodegenError("trigger  argument error\n");
    return;
   }
   case WAIT:
   	{
	args=args->next;
	o=args->data;
	o1=args->next->data;
	 if(o->kind==X86_OP_INT)
	 {
	  if(o1->kind==X86_OP_INT)
	  	{
      instr_code=0x4<<1|o->u.intlit;
	  instr_code=instr_code<<41|o1->u.intlit;
	  gen_le48(instr_code);
	  	}
	  else if(o1->kind==X86_OP_INSTACK)
	  	{
	  	 TODO;

	  	} else ErrorMsg_CodegenError("wait second argument error\n");

	 }
	 else  	ErrorMsg_CodegenError("wait first argument error\n");
	return;
   	}
   case TIMER:
    {
	args=args->next;
	o=args->data;
	if(o->kind==X86_OP_INT)
	{
	      if(o->u.intlit)
	      {
			instr_code=0xa<<2|o->u.intlit;
			gen_le48(instr_code<<40);
			return ;
		  }
		  o1=args->next->data;
          if(o1->kind==X86_OP_INSTACK)
          {
			instr_code=0xa<<2|o->u.intlit;
			i=(o1->u.index&0xFFF)<<12|EBP;
	  		instr_code=instr_code<<40|i;
			gen_le48(instr_code);
		  }
		  else if(o1->kind==X86_OP_GLOBAL)
		  {
		   relocation_sym *temp;
		  if(!(temp=Hash_lookup(cur_text_section->hash_table,Id_toString (o1->u.global))))
		 	Error_impossible();
			instr_code=0xa<<3|o->u.intlit<1|1;
	  		instr_code=instr_code<<39|temp->addr;
			gen_le48(instr_code);

		  }
		  else if(o1->kind==X86_OP_INT)
		  {
              instr_code=0xb;
	  		  instr_code=instr_code<<41|(o1->u.intlit<<12)|EEX;
	          gen_le48(instr_code);

              ///conv  i to double
              instr_code=0x16;
	  		  instr_code=instr_code<<41|EEX;
	          gen_le48(instr_code);

			  instr_code=0xa<<3|o->u.intlit<1|1;
	  		  instr_code=instr_code<<39|EEX;
			  gen_le48(instr_code);

		  }
		  else
		  ErrorMsg_CodegenError("timer second argument error\n");
	}
	else ErrorMsg_CodegenError("timer first argument error\n");
	return;
   	}
   default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return ;

}

int Firstech_Register_transfer (R r)
{
  switch (r){
  case X86_EAX:
    return EAX;
  case X86_EBX:
    return EBX;
  case X86_ECX:
    return ECX;
  case X86_EDX:
    return EDX;
  case X86_EBP:
    return EBP;
  case X86_ESP:
    return ESP;
  case X86_EIP:
	return EIP;
  default:
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}






static unsigned char  Cmp_bop_type (Operator_t o)
{
  switch (o&0xFFFF){
  case OP_ADD:
    return 0;
  case OP_SUB:
    return 1;
  case OP_TIMES:
    return 2;
  case OP_DIVIDE:
    return 3;
  case OP_GT:

	return 0;
  case OP_GE:
	return 3;
  case OP_LT:

    return 1;
  case OP_LE:
    return 4;
  case OP_NE:
    return 5;
  case OP_EQ:

    return 2;
  case OP_NEG:
    TODO;
    return 0;
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













static void error_dupTypeName (String_t id)
{
	ErrorMsg_CodegenError(String_concat
         ("type name must not be redefined: ",
          id,
          0));
}

Section *Section_new()
{
   Section *s;
   Mem_NEW (s);
   Mem_NEW_SIZE (s->data, init_alloc_size);
   s->data_allocated=init_alloc_size;
   s->ram_init_data=LinkedList_new();
   s->reloc=LinkedList_new();
   s->instr_total=0;
   s->hash_table=Hash_new
    ((tyHashCode)String_hashCode,
     (Poly_tyEquals)String_equals);
  return s;
}
ram_data *ram_data_new()
{
	ram_data *s;
	Mem_NEW (s);
	return s;
}
ram_data *ram_data_new_val(String_t ram_name,ram_kind kind,long long int init_value,int size)
{
	ram_data *s;
	Mem_NEW (s);
	int ram_addr1;
	ram_addr1=ram_addr+1;
	//if(ram_addr1>) need boundary check
	s->ram_name=ram_name;
    s->ram_type=kind;
    s->ram_val_addr=ram_addr1;
    s->init_value=init_value;
    s->size = size;
	ram_addr=ram_addr+size;
	return s;
}

relocation_sym *relocation_sym_new(sym_kind kind,int addr)
{
	relocation_sym *s;
	Mem_NEW (s);
	s->kind=kind;
	s->addr=addr;
	return s;


}
reloc_table *sym_need_reloc(S stm)
{

  reloc_table *s;
  Mem_NEW (s);
  s->instr_line=text_ind/6;
  s->stm=stm;

  return s;

}

/* realloc section and set its content to zero */
static void section_realloc(Section *sec, unsigned long new_size)
{
    unsigned long size;
    unsigned char *data;

    size = sec->data_allocated;
    if (size == 0)
        size = 1;
    while (size < new_size)
        size = size * 2;
	//
    Mem_REALLOC(data,sec->data, size);
    if (!data)
        ErrorMsg_CodegenError("memory full");
  //  memset(data + sec->data_allocated, 0, size - sec->data_allocated);
    sec->data = data;
    sec->data_allocated = size;
}

void g(int c)
{
    int ind1;
    ind1 = text_ind + 1;
    if (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[text_ind] = c;
    text_ind = ind1;
}

void o(unsigned int c)
{
    while (c) {
        g(c);
        c = c >> 8;
    }
}
void gen_le40(long long int c)
{
    g(c>>32);
    g(c>>24);
    g(c>>16);
	g(c>>8);
    g(c);
}
void gen_le24(int c)
{

  g(c>>16);
  g(c>>8);
  g(c);
}
void gen_le48(long long int c)
{
  g(c>>40);
  g(c>>32);
  g(c>>24);
  g(c>>16);
  g(c>>8);
  g(c);
}

long long int int_to_float(double i)
{
    return *(long long int *)&i;
}
int machine_kind_trasfer(String_t t)
{
      if(!strcmp(t,"int"))
	     return GLOBAL_VAR_INT;
	  else if(!strcmp(t,"float"))
	  	 return GLOBAL_VAR_FLOAT;
	  else if(!strcmp(t,"double"))
	  	 return GLOBAL_VAR_DOUBLE;
	  else
	  	ErrorMsg_CodegenError("machine kind trasfer error\n");

}
/*
int getline(char **s,int *size,FILE *fp)
{
   if(!*s)
   	 Mem_NEW_SIZE(*s,INIT_SIZE);
   char *p=*s;
   int i=0;
   while((c=getchar())!=EOF&&c!='\n')
   	{
   	 if(i>INIT_SIZE)
   	 p[i++]=c;
   	}
   if(c==EOF&&i==0) return EOF;
   if(c=='\n')
   	p[i]='\n';

}
*/
void put_wave_param(W w)
{
	String_t file_tag;

   //analysis file tag
   file_tag=w->wavefile_tag;
   if(!strcmp(file_tag,"GRAD"))
   {
   		print("GRAD");
		print(" ");
		print(w->wavefile_name);
		print("\n");

   }
   else if(!strcmp(file_tag,"TX"))
   {
		print("TX");
		print(" ");
		print(Int_toString(w->ch_no));
		print(" ");
		print(Int_toString(w->bram_no));
		print(" ");
		print(w->wavefile_name);
		print(" ");
		print(w->wavename);
		print("\n");
   }

}
void gen_wave_param(W w)
{
   FILE *fp;
   char tempStr[40];
  // char *StrLine=NULL;
   char StrLine[1024];
   int bram_sel_wr_addr,bram_wr_data_i,bram_wr_data_h,bram_wr_data_h2,bram_wr_data_h3,bram_wr_data_value,j,i,bram_sel_wr_value;
   int   gradP=0,grad=0;
   float calc_step;
   int  calc_step_i;
   int len,num;
   //points stands for calc and lut
   int lut_addr,points;
   String_t file_tag;

   //analysis file tag
   file_tag=w->wavefile_tag;
   print("#");
   print(file_tag);
   print(" data start\n");
   if(!strcmp(file_tag,"GRAD"))
   {
		bram_wr_data_i=GRAD_RAM_ADDR1;
		bram_wr_data_h=GRAD_RAM_ADDR2;
		grad=1;
   }
   else if(!strcmp(file_tag,"GRADPRAM"))
   {
		bram_wr_data_i=GRAD_PRAM_ADDR1;
		bram_wr_data_h=GRAD_PRAM_ADDR2;
		bram_wr_data_h2=GRAD_PRAM_ADDR3;
		bram_wr_data_h3=GRAD_PRAM_ADDR4;
		gradP=1;

   } else
   	{
   bram_sel_wr_addr=TX1_BRAM_WR_SEL;
   bram_sel_wr_value=0x0;
   bram_wr_data_i=TX1_BRAM_WDATA_I;
   bram_wr_data_h=TX1_BRAM_WDATA_H;
   	}
   //Not defined yet;
   fp=fopen(w->wavefile_name,"r");
   if(!fp)
   Error_bug (String_concat
               ("fail to open file: ",
                w->wavefile_name,
                0));
   /*
   while(!feof(fp))
   	{
       if((MAX_LINE_CHAR!=(num=fread(StrLine,1,MAX_LINE_CHAR,fp))&&ferror(fp)))
  		Error_bug(String_concat
               ("error occur read file: ",
                w->wavefile_name,
                0));

	   i=0;
	   	//skip the space
	   while(StrLine[i]==' '||StrLine[i]=='\t'||StrLine[i]=='\n')
		  	i++;

		  //start of line
		  if(!gradP)
		  {
		  if(grad)
		  {
		  calc_step=atof(StrLine+i);
		  bram_wr_data_value=*(int *)&calc_step;
		  }
		  else bram_wr_data_value=atoi ((StrLine+i));
		  print(Int_toString4x(bram_wr_data_i));
		  print(",");
	      print(Int_toString4x(bram_wr_data_value&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h));
		  print(",");
	      print(Int_toString4x(bram_wr_data_value>>16));
	      print("\n");
		  }
		  else
		  {
		  	j=0;
			while(StrLine[i]!=',')
			{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
			}
			tempStr[j]='\0';
			if(!strcmp(tempStr,"CALC"))
			{
				j=0;
				i++;
			   while(StrLine[i]!=',')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   calc_step=atof(tempStr);
			   calc_step_i=*(int *)&calc_step;
			   j=0;
			   i++;
			   while(StrLine[i]>='0'&&StrLine[i]<='9')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   points=atoi(tempStr);
			   //write to file
		  print(Int_toString4x(bram_wr_data_i));
		  print(",");
	      print(Int_toString4x(calc_step_i&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h));
		  print(",");
	      print(Int_toString4x(calc_step_i>>16));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h2));
		  print(",");
	      print(Int_toString4x(points&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h3));
		  print(",");
	      print(Int_toString4x(points>>16));
	      print("\n");

			}
			else if(!strcmp(tempStr,"LUT"))
			{
			   j=0;
			   i++;
			   while(StrLine[i]!=',')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   lut_addr=atoi(tempStr);
			   j=0;
			   i++;
			   while(StrLine[i]>='0'&&StrLine[i]<='9')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   points=atoi(tempStr);
			  			   //write to file
		  print(Int_toString4x(bram_wr_data_i));
		  print(",");
	      print(Int_toString4x(lut_addr&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h));
		  print(",");
	      print(Int_toString4x((points<<2)&0xFFFF |lut_addr>>16));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h2));
		  print(",");
	      print(Int_toString4x(points>>14));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h3));
		  print(",");
	      print(Int_toString4x(0));
	      print("\n");

			}

		  }
	   	}

   }
   */

     while (fgets(StrLine,MAX_LINE_CHAR,fp)||!feof(fp))
      {
      	  i=0;
		 // if() break;
          //if(!fgets(StrLine,1024,fp)&&feof(fp)) break;
		  // is space
		  while(StrLine[i]==' '||StrLine[i]=='\t')
		  	i++;
		  if(StrLine[i]=='\n'||StrLine[i]=='\0') continue;
          // FIx when one line over MAX_LINE_CHAR char

		  //
		  if(!gradP)
		  {
		  if(grad)
		  {
		  calc_step=atof(StrLine+i);
		  bram_wr_data_value=*(int *)&calc_step;
		  }
		  else bram_wr_data_value=atoi ((StrLine+i));
		  print(Int_toString4x(bram_wr_data_i));
		  print(",");
	      print(Int_toString4x(bram_wr_data_value&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h));
		  print(",");
	      print(Int_toString4x(bram_wr_data_value>>16));
	      print("\n");
		  }
		  else
		  {
		  	j=0;
			while(StrLine[i]!=',')
			{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
			}
			tempStr[j]='\0';
			if(!strcmp(tempStr,"CALC"))
			{
				j=0;
				i++;
			   while(StrLine[i]!=',')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   calc_step=atof(tempStr);
			   calc_step_i=*(int *)&calc_step;
			   j=0;
			   i++;
			   while(StrLine[i]>='0'&&StrLine[i]<='9')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   points=atoi(tempStr);
			   //write to file
		  print(Int_toString4x(bram_wr_data_i));
		  print(",");
	      print(Int_toString4x(calc_step_i&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h));
		  print(",");
	      print(Int_toString4x(calc_step_i>>16));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h2));
		  print(",");
	      print(Int_toString4x(points&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h3));
		  print(",");
	      print(Int_toString4x(points>>16));
	      print("\n");

			}
			else if(!strcmp(tempStr,"LUT"))
			{
			   j=0;
			   i++;
			   while(StrLine[i]!=',')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   lut_addr=atoi(tempStr);
			   j=0;
			   i++;
			   while(StrLine[i]>='0'&&StrLine[i]<='9')
				{
               //copy to tempStr
               tempStr[j++]=StrLine[i++];
				}
			   tempStr[j]='\0';
			   points=atoi(tempStr);
			  			   //write to file
		  print(Int_toString4x(bram_wr_data_i));
		  print(",");
	      print(Int_toString4x(lut_addr&0xFFFF));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h));
		  print(",");
	      print(Int_toString4x((points<<2)&0xFFFF |lut_addr>>16));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h2));
		  print(",");
	      print(Int_toString4x(points>>14));
	      print("\n");
		  print(Int_toString4x(bram_wr_data_h3));
		  print(",");
	      print(Int_toString4x(0));
	      print("\n");

			}

		  }
		  //end the line
          while(!strchr(StrLine,'\n')&&StrLine[0]!='\0')
		  {
		  memset(StrLine,0,sizeof(char)*MAX_LINE_CHAR);
		  fgets(StrLine,MAX_LINE_CHAR,fp);

          }
      }
	 if(ferror(fp))
		#ifdef _DEBUG
		 printf("error occur when use fgets\n");
		#endif

	 fclose(fp);
   print("#");
   print(file_tag);
   print(" data end\n");
   print("\n");

}
#define SPLIT_CHAR_NUM 40
#define SPLIT_CHAR '='
void print_header(String_t title)
{
   int i=0,start=(SPLIT_CHAR_NUM-strlen(title))/2;
   for(i=0;i<SPLIT_CHAR_NUM;i++)
   	printf("%c",SPLIT_CHAR);
   printf("\n");
   printf("*");
   for(i=0;i<start;i++)
   printf(" ");
   printf("%s\n",title);
   for(i=0;i<SPLIT_CHAR_NUM;i++)
   	printf("%c",SPLIT_CHAR);
   printf("\n\n");
}
void gen_firstech_file(Section *s)
{
    int nn;
   List_t p;
   ram_data *ram_data_temp;
   int ram_data_1,ram_data_2,ram_data_3,ram_data_4,seq_start,
   ram_data_addr,seq_config1,seq_config2,seq_config3,instr_p=0,ram_total=0;
   unsigned char *data_p=s->data;
   if(!strcmp(s->name,"main"))
   	{
     ram_data_1=MAINCTRL_CTRL_RAM_DATA_1;
	 ram_data_2=MAINCTRL_CTRL_RAM_DATA_2;
	 ram_data_3=MAINCTRL_CTRL_RAM_DATA_3;
	 ram_data_4=MAINCTRL_CTRL_RAM_DATA_4;
	 ram_data_addr=MAINCTRL_CTRL_RAM_ADDR;

	 seq_config1 = MAINCTRL_SEQ_CONFIG1;
	 seq_config2 = MAINCTRL_SEQ_CONFIG2;
	 seq_config3 = MAINCTRL_SEQ_CONFIG3;
   }
   else if(!strcmp(s->name,"tx1"))
   	{
     ram_data_1=TX1_RAM_DATA_1;
	 ram_data_2=TX1_RAM_DATA_2;
	 ram_data_3=TX1_RAM_DATA_3;
	 ram_data_4=TX1_RAM_DATA_4;
	 ram_data_addr=TX1_RAM_ADDR;

	 seq_config1 = TX1_SEQ_CONFIG1;
	 seq_config2 = TX1_SEQ_CONFIG2;
	 seq_config3 = TX1_SEQ_CONFIG3;

   }else if(!strcmp(s->name,"gradR"))
	{
     ram_data_1=GRADR_RAM_DATA_1;
	 ram_data_2=GRADR_RAM_DATA_2;
	 ram_data_3=GRADR_RAM_DATA_3;
	 ram_data_4=GRADR_RAM_DATA_4;
	 ram_data_addr=GRADR_RAM_ADDR;

	 seq_config1 = GRADR_SEQ_CONFIG1;
	 seq_config2 = GRADR_SEQ_CONFIG2;
	 seq_config3 = GRADR_SEQ_CONFIG3;

   }else if(!strcmp(s->name,"gradP"))
	{
     ram_data_1=GRADP_RAM_DATA_1;
	 ram_data_2=GRADP_RAM_DATA_2;
	 ram_data_3=GRADP_RAM_DATA_3;
	 ram_data_4=GRADP_RAM_DATA_4;
	 ram_data_addr=GRADP_RAM_ADDR;

	 seq_config1 = GRADP_SEQ_CONFIG1;
	 seq_config2 = GRADP_SEQ_CONFIG2;
	 seq_config3 = GRADP_SEQ_CONFIG3;

   }else if(!strcmp(s->name,"gradS"))
	{
     ram_data_1=GRADS_RAM_DATA_1;
	 ram_data_2=GRADS_RAM_DATA_2;
	 ram_data_3=GRADS_RAM_DATA_3;
	 ram_data_4=GRADS_RAM_DATA_4;
	 ram_data_addr=GRADS_RAM_ADDR;

	 seq_config1 = GRADS_SEQ_CONFIG1;
	 seq_config2 = GRADS_SEQ_CONFIG2;
	 seq_config3 = GRADS_SEQ_CONFIG3;

   }
   else if(!strcmp(s->name,"rx1"))
	{
     ram_data_1=RX1_RAM_DATA_1;
	 ram_data_2=RX1_RAM_DATA_2;
	 ram_data_3=RX1_RAM_DATA_3;
	 ram_data_4=RX1_RAM_DATA_4;
	 ram_data_addr=RX1_RAM_ADDR;

	 seq_config1 = RX1_SEQ_CONFIG1;
	 seq_config2 = RX1_SEQ_CONFIG2;
	 seq_config3 = RX1_SEQ_CONFIG3;

   }
   else TODO;




   p=s->ram_init_data->next;
   print("#");
   print(s->name);
   print("ram init data start\n");

   while(p)
   {

    ram_data_temp=(ram_data *)p->data;
	#ifdef _DEBUG
    printf("ram_data:value %llx,ram_data:addr %d,ram_data_type:%d\n",ram_data_temp->init_value,ram_data_temp->ram_val_addr,ram_data_temp->ram_type);

    #endif
    for(nn=0;nn < ram_data_temp->size;nn++)
    {
	print(Int_toString4x(ram_data_1));
	print(",");
	print(Int_toString4x(ram_data_temp->init_value&0xFFFF));
	print("\n");

	print(Int_toString4x(ram_data_2));
	print(",");
	print(Int_toString4x(ram_data_temp->init_value>>16));
	print("\n");

    print(Int_toString4x(ram_data_3));
	print(",");
	print(Int_toString4x(ram_data_temp->init_value>>32));
	print("\n");

    print(Int_toString4x(ram_data_4));
	print(",");
	print(Int_toString4x(ram_data_temp->init_value>>48));
	print("\n");

	print(Int_toString4x(ram_data_addr));
	print(",");
	print(Int_toString4x(ram_data_temp->ram_val_addr+nn));
 	print("\n");
    }
	p=p->next;
	ram_total++;
   }

   if(ram_total>MAX_RAM_SIZE)
   {
       printf("error:board %s dataram exceed  %d\n\n",s->name,MAX_RAM_SIZE);

	   fclose(file);
	   exit(-1);
   }
   print("#");
   print(s->name);
   print("ram init data end\n");
   print("\n");
   print("#");
   print(s->name);
   print("seq config start\n");
   while(instr_p<s->instr_total)
   {

	instr_p*=6;
	print(Int_toString4x(seq_config1));
	print(",");
	print(Int_toString4x((unsigned int)data_p[instr_p+4]<<8|data_p[instr_p+5]));
	print("\n");
	print(Int_toString4x(seq_config2));
	print(",");
	print(Int_toString4x((unsigned int)data_p[instr_p+2]<<8|data_p[instr_p+3]));
	print("\n");
	print(Int_toString4x(seq_config3));
	print(",");
	print(Int_toString4x((unsigned int)data_p[instr_p]<<8|data_p[instr_p+1]));
 	print("\n");
	instr_p/=6;
	instr_p++;
   }


   if(s->instr_total>MAX_INST_NUM)
   {
       printf("error:board %s instr exceed  %d\n\n",s->name,MAX_INST_NUM);

	   fclose(file);
	   exit(-1);
   }
   printf("board %s dataram %d (%.2f%%),instr num %d (%.2f%%)\n\n",s->name,ram_total,ram_total*100.0/MAX_RAM_SIZE,s->instr_total,(s->instr_total*100.0/MAX_INST_NUM));
   print("#");
   print(s->name);
   print("seq config end\n");

}

static ram_data * check_ram_value(List_t list,long long int init_value)
{
   Assert_ASSERT(list);
   List_t p=list->next;
   ram_data *s;
   while(p)
   	{
      s=(ram_data *)p->data;
	  if(s->ram_type==CONST_VAR&&s->init_value==init_value)
	  return s;
      p=p->next;
   }
   return 0;
}

void gen_fun(F f)
{
    ///This code is used for test  assert
    char *charP=NULL;
    Assert_ASSERT(charP);


	///
	//reloc indicate this fun reloc sym;
	List_t stm_p,p1,p,p2,reloc=LinkedList_new();
	int reg,temp;
	S stm;
	ram_data *rd;
	relocation_sym *rel_sym;
	reloc_table *reloc_table_tmp;
	char str[2];
	str[1]='\0';
	unsigned long long int instr=0;
	int i = LinkedList_size (f->decs);
	int opcode;
    if (i){
	 // o(0x1); /*subl */
	 // long int a=0;
	  /*gen ram data*/
	 str[0]=i+0x30;
	  rd=ram_data_new_val(String_concat("CONST_",str,0),CONST_VAR,int_to_float((double)i),1);

	 // rd->ram_type=gv->type;
	 /*
	  Hash_insert (cur_text_section->hash_table, rd->ram_name,
                 relocation_sym_new(CONST_VALUE,rd->ram_val_addr),
                 (tyDup)error_dupTypeName);
     */
	  LinkedList_insertLast(cur_text_section->ram_init_data,rd);
	  //gen_le40((ESP<<24)|(rd->ram_val_addr<<12)|ESP);
	  // instr=0x1;
	  // instr=instr<<42|ESP<<24|(rd->ram_val_addr<<12)|ESP;
	   //instr=instr<<12|ESP;
	   //gen_le48(instr);
	   gen_le24(1<<16|ESP);
	   gen_le24(rd->ram_val_addr<<12|ESP);
    }
	//alllocate local global;
List_t	global_var_p;
GV gv;
  if(f->globals)
   {
     //function name
     global_var_p=f->globals->next;
	 while(global_var_p)
	 {
	 gv=(GV)global_var_p->data;
	 if(!Hash_lookup(cur_text_section->hash_table,Id_toString(gv->var)))
	 {

      rd=ram_data_new_val(Id_toString(gv->var),machine_kind_trasfer(gv->type),gv->init_value,gv->size);

	  Hash_insert (cur_text_section->hash_table, rd->ram_name,
                 relocation_sym_new(GLOBAL_VAR_SYM,rd->ram_val_addr),
                 (tyDup)error_dupTypeName);
      //
	  LinkedList_insertLast(cur_text_section->ram_init_data,rd);
	 }
      global_var_p=global_var_p->next;
	}
  	}





	stm_p=f->stms->next;
	while(stm_p)
	 {

     stm=(S)stm_p->data;
	 Assert_ASSERT(stm);
	 switch (stm->kind){
	 case FIRSTECH_STM_CONV:
	 	{
	   	instr=0x16|stm->u.conv.ct;
		reg=Firstech_Register_transfer(stm->u.conv.src);
		instr=instr<<41|reg;
	    gen_le48(instr);
	    }
		break;
	 case X86_STM_MOVERR:
	   TODO;
	   break;
	 case X86_STM_MOVERI:
	   TODO;
	   break;
	 case X86_STM_LOAD:
		   {
	   O o=stm->u.load.src;
	   reg=Firstech_Register_transfer(stm->u.load.dest);
	   if(o->kind==X86_OP_INSTACK)
	   {
		  int i=o->u.index;
		  //0x00154 means op2 indirect op1 direct
		  gen_le24((i&0x00000fff)|0x00154000);
		  gen_le24(EBP<<12|reg);
	   }
	   else if(o->kind == X86_OP_MEM)
       {
           int i=o->u.mem.offset;
		   int base_reg = Firstech_Register_transfer(o->u.mem.base);
		   //0x00154 means op2 indirect op1 direct
		   gen_le24((i&0x00000fff)|0x00154000);
		   gen_le24(base_reg<<12|reg);
       }
	   else if(o->kind==X86_OP_INT)
	   {
	   ///only 2^29 will extern to 32bit
	   temp = o->u.intlit;

	   instr=0xb;
	   instr=instr<<29|(temp&0x1FFFFFFF);
	   instr=instr<<12|reg;
	   gen_le48(instr);
	   }
	   else if(o->kind==X86_OP_DOUBLE)
	   {
       //first search in hash table
	   if(!(rd=check_ram_value(cur_text_section->ram_init_data,int_to_float(o->u.doublelit))))
	   	{
	   rd=ram_data_new_val("CONST_Float",CONST_VAR,int_to_float(o->u.doublelit),1);
	   LinkedList_insertLast(cur_text_section->ram_init_data,rd);
	   	}
	   gen_le24(0x00140000);
	   gen_le24(rd->ram_val_addr<<12|reg);
	   }
	   else if(o->kind==X86_OP_GLOBAL)
	   {
	   relocation_sym *temp;
		if(!(temp=Hash_lookup(cur_text_section->hash_table,Id_toString (o->u.global))))
		 Error_impossible();
		 gen_le24(0x00140000);
	   gen_le24(temp->addr<<12|reg);
	   }
	   }
	   break;
	  case FIRSTECH_STM_ADR:
	  {
		  O o=stm->u.firstechadr.src;
		  reg=Firstech_Register_transfer(stm->u.firstechadr.dest);
		  if(o->kind==X86_OP_GLOBAL)
		  {
		  	   relocation_sym *temp;
		  	   if(!(temp=Hash_lookup(cur_text_section->hash_table,Id_toString (o->u.global))))
		  		 Error_impossible();


		  	 			instr=0xb;
		  	 			instr=instr<<29|((temp->addr)&0x1FFFFFFF);
		  	 			instr=instr<<12|reg;
		  	 			gen_le48(instr);
		  }
		  else Error_impossible();
	  }
	   break;
	  case FIRSTECH_STM_MOVE:
		   {
	   O o=stm->u.firstechmove.src;
	   O o1=stm->u.firstechmove.dest;
	   if(o->kind==X86_OP_INT)
		   {
		   temp = o->u.intlit;
		   if(temp>268435455||temp<-268435456)
		   ErrorMsg_CodegenError("int exceed 29bit\n");

			if(o1->kind==X86_OP_GLOBAL)
			{
				relocation_sym *temp1;
		  	if(!(temp1=Hash_lookup(cur_text_section->hash_table,Id_toString (o1->u.global))))
		   	Error_impossible();

		   gen_le48((long long int)0xb<<41|temp<<12|temp1->addr);

			}
			else if(o1->kind==X86_OP_REG)
			{
			reg=Firstech_Register_transfer(o1->u.reg);
			instr=0xb;
			instr=instr<<29|(temp&0x1FFFFFFF);
			instr=instr<<12|reg;

			gen_le48(instr);
			}
			else Error_impossible();

	   }
	   else if(o->kind==X86_OP_DOUBLE)
		   {
		   //check if ram have the CONST
		   if(!(rd=check_ram_value(cur_text_section->ram_init_data,int_to_float(o->u.doublelit))))
		  {
		  rd=ram_data_new_val("CONST_Float",CONST_VAR,int_to_float(o->u.doublelit),1);
	    LinkedList_insertLast(cur_text_section->ram_init_data,rd);
		   	}
			 if(o1->kind==X86_OP_GLOBAL)
			 {
				 relocation_sym *temp1;
		   if(!(temp1=Hash_lookup(cur_text_section->hash_table,Id_toString (o1->u.global))))
			Error_impossible();
		   gen_le24(0x00140000);
		   gen_le24(rd->ram_val_addr<<12|temp1->addr);

			 }
			 else if(o1->kind==X86_OP_REG)
			 {
			 reg=Firstech_Register_transfer(o1->u.reg);
			 gen_le24(0x00140000);
			 gen_le24(rd->ram_val_addr<<12|reg);
			 } else Error_impossible();

	   }
	   }
	   break;
	  case FIRSTECH_STM_BOP:
		   {
	   int type,addr1,addr2;
	   unsigned long long int instr=0;
	   type=Cmp_bop_type (stm->u.fbop.op);
	   addr1=Firstech_Register_transfer (stm->u.fbop.src);
	   addr2=Firstech_Register_transfer (stm->u.fbop.dest);
	   temp=Firstech_Register_transfer (stm->u.fbop.result);
	  // instr=type<<40|temp<<24
	   instr=type;
	   instr=instr<<16|temp;
	   instr=instr<<24|addr2<<12|addr1;

	   gen_le48(instr);
	   //gen_le24(0xFFF);
	   //gen_le24(addr1);
	  }
	   break;
	 case X86_STM_STORE:
	   {
	   O o=stm->u.store.dest;
	   reg=Firstech_Register_transfer(stm->u.store.src);
	   if(o->kind==X86_OP_INSTACK)
	   {
		  int i=o->u.index;
		  //0x00154 means op2 indirect op1 direct
		  gen_le24((i&0x00000fff)|0x00148000);
		  gen_le24(reg<<12|EBP);
	   }

	   else if(o->kind==X86_OP_GLOBAL)
	   {
	   relocation_sym *temp;
		if(!(temp=Hash_lookup(cur_text_section->hash_table,Id_toString (o->u.global))))
		 Error_impossible();
		 gen_le24(0x00140000);
	   gen_le24(reg<<12|temp->addr);
	   }
	   else if(o->kind==X86_OP_MEM)   //added by LHF
       {
		//   o->u.mem.base
        //   gen_le24(reg<<12|ECX);
		   int i=o->u.mem.offset;
		   int base_reg = Firstech_Register_transfer(o->u.mem.base);
		   //0x00154 means op2 indirect op1 direct
		   gen_le24((i&0x00000fff)|0x00148000);
		   gen_le24(reg<<12|base_reg);
       }
	   else Error_impossible();
	   }
	   break;
	 case FIRSTECH_STM_CMP:
		   {
	   int type,addr1,addr2;
	   instr=0;
	   type=Cmp_bop_type (stm->u.fcmp.op);
	   type|=0x30; //cmp
	   addr1=Firstech_Register_transfer (stm->u.fcmp.src);
	   addr2=Firstech_Register_transfer (stm->u.fcmp.dest);
	   temp=Firstech_Register_transfer (stm->u.fcmp.result);
	   //gen_le48((long long int)type<<39|temp<<24|addr2<<12|addr1);
	   instr=type;
	   instr=instr<<15|temp;
	   instr=instr<<24|addr2<<12|addr1;
	   gen_le48(instr);
	  // gen_le24(type<<15|temp<<24);
	 //  gen_le24(addr2<<12|addr1);
	 }
	   break;
	 case X86_STM_UOP:
	   TODO;
	  // X86_Operator_print (s->u.uop.op);
	  // X86_Register_print (s->u.uop.dest);
	   break;
	 case X86_STM_CALL:

		//print ("pushl %EIP"); subl $1,%esp
		gen_le24(0x1<<16|ESP);
		gen_le24(const_1_addr<<12|ESP);
		//mov eip,(%esp)
		gen_le24(0x00148000);
		gen_le24(EIP<<12|ESP);
		///need reloc

		if(!(rel_sym=Hash_lookup(cur_text_section->hash_table,Id_toString(stm->u.call.name))))
		{
		 LinkedList_insertLast(reloc,sym_need_reloc(stm));
		 gen_le48((long long int)0xa<<40);
		}
		else
		gen_le48((long long int)0xa<<40|rel_sym->addr<<12);
	   break;

	 case X86_STM_CMP:
	   TODO;
	   break;
	 case X86_STM_LABEL:
	   /// check if the hash table have label then insert
	   if(!Hash_lookup(cur_text_section->hash_table,Label_toString (stm->u.label)))
		   Hash_insert(cur_text_section->hash_table,Label_toString (stm->u.label),
		   relocation_sym_new(LABEL_SYM,(text_ind)/6),
					(tyDup)error_dupTypeName);
	  // print (Label_toString (s->u.label));
	   break;
	 case X86_STM_JE:
	   TODO;
	   break;
	case FIRSTECH_STM_JE:
	   //first look up
	   reg=Firstech_Register_transfer(stm->u.fje.fcon);
	   if(!(rel_sym=Hash_lookup(cur_text_section->hash_table,Label_toString (stm->u.fje.je))))

	   {

	   LinkedList_insertLast(reloc,sym_need_reloc(stm));
	   gen_le48((long long int)0x8<<40|reg);
	   }

	   else
	   {
		  //je
		   gen_le48((long long int)0x8<<40|rel_sym->addr<<12|reg);

	   }
	   break;
	 case X86_STM_JL:
	   TODO;
	   //print ("jl ");
	   //print (Label_toString (s->u.je));
	   break;
	 case X86_STM_JUMP:

	   if(!(rel_sym=Hash_lookup(cur_text_section->hash_table,Label_toString (stm->u.jump))))

	   {

	   LinkedList_insertLast(reloc,sym_need_reloc(stm));
	   gen_le48((long long int)0xa<<40);
	   }

	   else
	   {
		  //jmp
		   gen_le48((long long int)0xa<<40|rel_sym->addr<<12);

	   }
	   break;
	 case X86_STM_PUSH:

	   //print ("pushl ");
	  //subl $1, %esp
	  gen_le24(0x1<<16|ESP);
      gen_le24(const_1_addr<<12|ESP);
	  // gen_le48((long long int)0x1<<40|(ESP<<24)|(const_1_addr<<12)|ESP);
	  //movl r,(%esp)
	  reg=Firstech_Register_transfer(stm->u.push);
	  //instr=0x00148000<<24;
	  //gen_le48(instr|reg<<12|ESP);
	   gen_le24(0x00148000);
	   gen_le24(reg<<12|ESP);
	   break;
	 case X86_STM_NEG:
	  // TODO;
	   print ("negl ");
	   X86_Register_print (stm->u.neg);
	   break;
	 case X86_STM_SETL:
	   TODO;
	  // space4 ();
	  // print ("setl ");
	  // X86_Register_print (s->u.setAny);
	   break;
	 case X86_STM_XOR:
	   TODO;
	   //space4 ();
	   //print ("xorl ");
	  // X86_Register_print (s->u.xor.src);
	  // print (", ");
	  // X86_Register_print (s->u.xor.dest);
	   break;
	 case X86_STM_RETURN:

	   // leave and return
	   //movl %ebp, %esp
	   gen_le24(0x00140000);
	   gen_le24(EBP<<12|ESP);
	   //movl (%esp), %ebp
	   gen_le24(0x00154000);
	   gen_le24(ESP<<12|EBP);
	   //addl $1, %esp
	   instr=0;
	   instr=ESP;
	   instr=instr<<24|const_1_addr<<12|ESP;
	   //gen_le48((long long int)(ESP<<24)|(const_1_addr<<12)|ESP);
	   gen_le48(instr);


	   //print ("\n\tret");
	   //movl (%esp), %eip
	   gen_le24(0x00154000);
	   gen_le24(ESP<<12|EIP);
	   //addl $1, %esp
	  // gen_le48((long long int)(ESP<<24)|(const_1_addr<<12)|ESP);
		gen_le48(instr);
	   //jmp  (%eip)
	   gen_le48((long long int)0xB<<40|EIP<<12);

	   break;
	 case X86_STM_CLTD:
	   TODO;
	   break;
	 case X86_STM_INC:
	   //addl $1, %edx
	   printf("inc=<%d>\n",X86_STM_INC);
	   instr=0;
	   instr=EDX;
	   instr=instr<<24|const_1_addr<<12|EDX;
	   break;
	 case FIRSTECH_STM_END:
	   //end
	   gen_le48((long long int)0x9<<42);
	   break;
	 case FIRSTECH_STM_UPDATE:
	   //update
	   gen_le48((long long int)0x7<<42);
	   break;
	 case FIRSTECH_STM_PRIMITIVE:
		Firstech_Primitive_opcode(stm);
	 	break;

	 default:
	   Error_impossible();
	   break;
	 	}
		 stm_p=stm_p->next;
	  }
	 //relocation syms when funs add to below
   F fun;
   p1=reloc->next;
   while(p1)
   {
     reloc_table_tmp=(reloc_table *)p1->data;
	 stm=reloc_table_tmp->stm;
	 if(stm->kind==X86_STM_JUMP)
	 {
      if(rel_sym=Hash_lookup(cur_text_section->hash_table,Label_toString (stm->u.jump)))
      {
 		//*(unsigned int *)(cur_text_section->data[reloc_table_tmp->instr_line*6+2])|=(rel_sym->addr)<<12;
		///8-15 use 12~15
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+4]|=((rel_sym->addr&0xF)<<4);
		//16-23	use 16~23
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+3]|=((rel_sym->addr&0xFF0)>>4);
		//24-31 use 24~27
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+2]|=((rel_sym->addr&0xF000)>>12);

#ifdef _DEBUG
		printf("jump label %s find addr is %d\n",Label_toString (stm->u.jump),rel_sym->addr);
#endif
	  }
	  else
	  	{
	  	fprintf(stderr,"JUMP label %s not find\n",Label_toString (stm->u.jump));
        Error_impossible();
	  	}

	 }
	 else if(stm->kind==FIRSTECH_STM_JE)
	 {
	if(rel_sym=Hash_lookup(cur_text_section->hash_table,Label_toString (stm->u.fje.je)))
      {
 				cur_text_section->data[(reloc_table_tmp->instr_line*6)+4]|=((rel_sym->addr&0xF)<<4);
		//16-23	use 16~23
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+3]|=((rel_sym->addr&0xFF0)>>4);
		//24-31 use 24~27
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+2]|=((rel_sym->addr&0xF000)>>12);
#ifdef _DEBUG

		printf("je label find addr is %d\n",rel_sym->addr);
#endif
	  }else Error_impossible();

	 }
	 else if(stm->kind==X86_STM_CALL)
	 {

	   String_t call_name=Id_toString(stm->u.call.name);
	   int addr;
	   if(rel_sym=Hash_lookup(cur_text_section->hash_table,call_name))
	   	{
	   	  addr =rel_sym->addr;
		 cur_text_section->data[(reloc_table_tmp->instr_line*6)+4]|=(addr&0xF)<<4;
		//16-23	use 16~23
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+3]|=(addr&0xFF0)>>4;
		//24-31 use 24~27
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+2]|=(addr&0xF000)>>12;


	   }
	   else
	   	{
       p2=prog_list->next;
	   while(p2)
	   	{
	   	 fun=(F)p2->data;
         if(!strcmp(call_name,String_concat ("_",
                           Id_toString (fun->name),
                           0)))
         	{
         	 addr=(text_ind)/6;
		  Hash_insert(cur_text_section->hash_table,call_name,
		   relocation_sym_new(FUNC_SYM,addr),
					(tyDup)error_dupTypeName);
        cur_text_section->data[(reloc_table_tmp->instr_line*6)+4]|=(addr&0xF)<<4;
		//16-23	use 16~23
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+3]|=(addr&0xFF0)>>4;
		//24-31 use 24~27
		cur_text_section->data[(reloc_table_tmp->instr_line*6)+2]|=(addr&0xF000)>>12;
#ifdef _DEBUG
			printf("call addr find %d\n",(text_ind)/6);
#endif
 			//subl $1,%esp
		    //gen_le48((long long int)0x1<<40|(ESP<<24)|(const_1_addr<<12)|ESP);
			instr=1;
	   		instr=instr<<16|ESP;
	   		instr=instr<<24|const_1_addr<<12|ESP;
			gen_le48(instr);
			//mov ebp,(%esp)
			gen_le24(0x00148000);
			gen_le24(EBP<<12|ESP);
			//movl %esp %ebp
			gen_le24(0x00140000);
	   		gen_le24(ESP<<12|EBP);
			gen_fun(fun);
			break;
         	}
		 p2=p2->next;
	   }

	 	}

	 }
		 p1=p1->next;
   }
}



void PAR_Fun_print (F f)
{
  int n;
  Assert_ASSERT(f);
  List_t  p,p1;
  Machine_Global_Var_t v;
  double value;
  Poly_t var_addr_list;
  Section *s;
  relocation_sym *rs;
  //if is extend box ,flag=1
  int i=0;
  if(f->globals)
  	{
  	 p=f->globals->next;
	 while(p)
	 {

	   v=(GV)p->data;
	   //only export non static var
	   if(v->var_static)
	   goto done;
	   var_addr_list=Hash_lookup(varhash,Id_toString(v->var));
	   if(var_addr_list)
	   goto done;
	   print(":");
	   print (v->type);
	   space4();
	   print (Id_toString(v->var));
	   if(v->size > 1)                // added by LHF
	   {
	       print("[");
	       print(Int_toString(v->size));
	       print("]");
	   }

	   print(",");
	   if(!strcmp(v->type,"double"))
	   {
                  value=*(double *)&v->init_value;
				  print(double_toString2(value));
	   }
	   else
	   print (Int_toString((int)(v->init_value)));

	   for(i=0;i<5;i++)
	   {
	   p1=Hash_lookup(boxssec,boxs[i]);
	   p1=p1->next;
	   if(p1)
	   {
	   print(" ");
	   print(":");
	   print("board");
	   }
	   else
	   break;
	   while(p1)
	   	{
	   	s=(Section *)p1->data;
	   	rs=Hash_lookup (s->hash_table,Id_toString(v->var));
			if(rs)
			{
			   print(",");
			   print (s->name);
			   print("(");
			   print(Int_toString(rs->addr));
			   print(")");

		   	}
			p1=p1->next;
	 	}

	   print(" ");
	   print("Spectrometer");
	   print(",");
	   print(boxs[i]);

	   }
	   if(v->size > 1)
       {
	   	print("\n[");
            for(n =0;n < v->size;n++)                // added by LHF
            {
                if(n % 8)
                {
                    print(" , ");
                }
                else if(n==0)
                {

                    print("\n");
                }
                else 
                {

                    print(",\n");
                }
                if(!strcmp(v->type,"double"))
                {
                    value=*(double *)&v->init_value;
                    print(double_toString2(value));
                }
                else
                    print (Int_toString((int)(v->init_value)));


            }
			print("\n]");
        }
        print("\n");


	   Hash_insert(varhash,Id_toString(v->var),(Poly_t)1,error_dupTypeName);
	   done:
	   p=p->next;
	 }
  }
}

void Par_print(List_t x,String_t filename)
{
	Assert_ASSERT(x);
    Assert_ASSERT(filename);
    String_t onlyfilename;
    int pathsize=0;
	TCHAR szPath[MAX_PATH];
	varhash=Hash_new((tyHashCode)String_hashCode,     (Poly_tyEquals)String_equals);
	print(":SRC");
	space4();
	if(!strrchr(filename,'\\'))
	{
		if(!(pathsize=GetFullPathName(filename,MAX_PATH,szPath,NULL)))
  		{
	     printf("Get Full Filename failded (%d)\n",GetLastError());
		 exit(1);
  		}
		onlyfilename=genFileName (szPath, "src");
	}
	//onlyfilename=FilePathStrp(filename);
    else  onlyfilename=genFileName (filename, "src");
	print(onlyfilename);
	print("\n");
	LinkedList_foreach (x,
                      (Poly_tyVoid)PAR_Fun_print);
}


void Firstech_Code_gen (P x, String_t outfilename,int type)
{
  Assert_ASSERT(x);
  Assert_ASSERT(outfilename);
  int i;
  String_t fname,boxname;
  if(type==0)
  {
	fname=genFileName (outfilename, "par");
  }
  else
  {
	fname=genFileName (outfilename, "fcode");
  }
  FILE* file = fopen (fname, "w+");
   if (!file)
    Error_bug (String_concat
               ("fail to open file: ",
                fname,
                0));

  //prog list

  prog_list=LinkedList_new();
  boxssec=Hash_new
    ((tyHashCode)String_hashCode,
     (Poly_tyEquals)String_equals);
  //add box to hash table
  Hash_insert(boxssec,"M",LinkedList_new(),error_dupTypeName);
  Hash_insert(boxssec,"E1",LinkedList_new(),error_dupTypeName);
  Hash_insert(boxssec,"E2",LinkedList_new(),error_dupTypeName);
  Hash_insert(boxssec,"E3",LinkedList_new(),error_dupTypeName);
  Hash_insert(boxssec,"E4",LinkedList_new(),error_dupTypeName);

  List_t internals=LinkedList_new();
 // int ram_addr_start;
  /*first init prog_list*/
  F f;
  List_t p=x->funcs->next,global_var_p,stm_p,p1;
  while (p){
  	f=(F)p->data;
    if (f->inter)
		LinkedList_insertLast (internals, p->data);
	else
	   LinkedList_insertLast (prog_list, p->data);
    p = p->next;
  }
 /*end */
 Section *s;
 ram_data *rd;
 List_t sec_temp;

 reloc_table *reloc_table_tmp;
 long long int temp;

 S stm;
 GV gv;
 char *point;
 p=internals->next;
 while(p)
 {
   s=Section_new();
   cur_text_section=s;
   text_ind=0; //when use add 1
   f=(F)p->data;

   if(f->globals)
   {
     //function name
     if((point=strchr(Id_toString (f->name),':'))!=NULL)
     {
		boxname=String_new_end(Id_toString (f->name),point);
		s->name=String_new(point+1);
	 }
	 else
	 {
	 boxname="M";
     s->name=Id_toString (f->name);
	 }
	 sec_temp=Hash_lookup(boxssec,boxname);
	 global_var_p=f->globals->next;
	 ram_addr=init_ram_addr;
	 while(global_var_p)
	 {


	  gv=(GV)global_var_p->data;
//	  printf("<%s %s>\n",gv->type,Id_toString(gv->var));
      rd=ram_data_new_val(Id_toString(gv->var),machine_kind_trasfer(gv->type),gv->init_value,gv->size);
	  Hash_insert (s->hash_table, rd->ram_name,
                 relocation_sym_new(GLOBAL_VAR_SYM,rd->ram_val_addr),
                 (tyDup)error_dupTypeName);
      //
      if(type)
	  LinkedList_insertLast(s->ram_init_data,rd);
      global_var_p=global_var_p->next;
	 }
	if(type)
	{
	//0x3f80000 is int 1

		rd=ram_data_new_val("CONST_1",CONST_VAR,0x3ff0000000000000LL,1);
		const_1_addr=rd->ram_val_addr;
		LinkedList_insertLast(s->ram_init_data,rd);
		//EBP ESP value
		rd=ram_data_new();
		rd->ram_name="ebp_init_value";
		rd->ram_type=CONST_VAR;
		rd->ram_val_addr=EBP;
		rd->init_value=INIT_EBP_ADDR;
		rd->size = 1;
		LinkedList_insertLast(s->ram_init_data,rd);
		//ESP
		rd=ram_data_new();
		rd->ram_name="esp_init_value";
		rd->ram_type=CONST_VAR;
		rd->ram_val_addr=ESP;
		rd->init_value=INIT_ESP_ADDR;
		rd->size = 1;
		LinkedList_insertLast(s->ram_init_data,rd);
		gen_fun(f);
		s->instr_total=(text_ind)/6;
	}
	LinkedList_insertLast (sec_temp, s);
  }
   p=p->next;
 }




  file_init (file);
  buffer_init ();
  //first we process the wave and params
  if(type==0)
  {

	Par_print(internals,outfilename);
	print("\n");
  p=wave_file_list->next;
  while(p)
  {
    put_wave_param((W)p->data);
    p=p->next;
  }
	fclose (file);
	return ;
  }
  /*
  p=wave_file_list->next;
  while(p)
  {
    gen_wave_param((W)p->data);
    p=p->next;
  }
  */
  //end
 for(i=0;i<5;i++)
 {
  p=Hash_lookup(boxssec,boxs[i]);
  p=p->next;

  if(p)
  {
    print_header(String_concat("Resource Usage of ",boxs[i]," box",0));
  	print("$");
    print(boxs[i]);
	print("\n");
  }
  while(p)
  {

	  s=(Section *)p->data;
      gen_firstech_file(s);
      p=p->next;
  }
 }
  buffer_final ();
  fclose (file);
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
#undef GV
#undef W

