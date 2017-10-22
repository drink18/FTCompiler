#include "../lib/mem.h"
#include "../lib/int.h"
#include "../lib/assert.h"
#include "../lib/todo.h"
#include "../lib/float.h"
#include <string.h>

#include "x86_c.h"

#define List_t LinkedList_t
#define Id_t AstId_t

#define P Ast_Prog_t
#define T Ast_Type_t
#define Td Ast_Td_t
#define F Ast_Fun_t
#define S Ast_Stm_t
#define L Ast_Lval_t
#define B Ast_Block_t
#define E Ast_Exp_t
#define D Ast_Dec_t
#define Af Ast_Field_t

static void spaces ();
static int in_for=0;
static File_t file = 0;
static List_t globalvar=0;
extern LinkedList_t labelforboards;
static void print (String_t s)
{
  fprintf (file, "%s", s);
  return;
}







void X86_C_Lval_print (L l)
{
  Assert_ASSERT(l);
  switch (l->kind){
  case AST_LVAL_VAR:
    print (AstId_toString (l->u.var));
    break;
  case AST_LVAL_DOT:
    Ast_Lval_print (l->u.dot.lval);
    print (".");
    print (AstId_toString (l->u.dot.var));
    break;
  case AST_LVAL_ARRAY:
    Ast_Lval_print (l->u.array.lval);
    print ("[");
    X86_C_Exp_print (l->u.array.exp);
    print ("]");
    break;
  default:
    Error_bug ("impossible");
    break;
  }
}



void X86_C_Field_print (Af f)
{
  Assert_ASSERT(f);
  print (AstId_toString (f->id));
  print (" = ");
  X86_C_Exp_print (f->e);
  print ("; ");
  return;
}



void X86_C_Exp_print (E e)
{
  Assert_ASSERT (e);
  switch (e->kind){
  case AST_EXP_ADD:
    X86_C_Exp_print (e->u.bop.left);
    print (" + ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_SUB:
    X86_C_Exp_print (e->u.bop.left);
    print (" - ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_TIMES:
    X86_C_Exp_print (e->u.bop.left);
    print (" * ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_DIVIDE:
    X86_C_Exp_print (e->u.bop.left);
    print (" / ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_MODUS:
    X86_C_Exp_print (e->u.bop.left);
    print (" %% ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_OR:
    X86_C_Exp_print (e->u.bop.left);
    print (" || ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_AND:
    X86_C_Exp_print (e->u.bop.left);
    print (" && ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_EQ:
    X86_C_Exp_print (e->u.bop.left);
    print (" == ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_NE:
    X86_C_Exp_print (e->u.bop.left);
    print (" != ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_LT:
    X86_C_Exp_print (e->u.bop.left);
    print (" < ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_LE:
    X86_C_Exp_print (e->u.bop.left);
    print (" <= ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_GT:
    X86_C_Exp_print (e->u.bop.left);
    print (" > ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_GE:
    X86_C_Exp_print (e->u.bop.left);
    print (" >= ");
    X86_C_Exp_print (e->u.bop.right);
    return;
  case AST_EXP_SL:
    X86_C_Exp_print (e->u.bop.left);
    print (" << ");
    X86_C_Exp_print (e->u.bop.right);
    return;

  case AST_EXP_NOT:
    print (" ! ");
    X86_C_Exp_print (e->u.unary.e);
    return;
  case AST_EXP_NEGATIVE:
    print (" - ");
    X86_C_Exp_print (e->u.unary.e);
    return;
  case AST_EXP_NULL:
    print (" null ");
    return;
  case AST_EXP_INTLIT:
    print (Int_toString (e->u.intlit));
    return;
  case AST_EXP_STRINGLIT:
    print ("\"");
    print (e->u.stringlit);
    print ("\"");
    return;
  case AST_EXP_NEW_STRUCT:
    print (AstId_toString(e->u.newStruct.type));
    print ("{");
    LinkedList_foreach (e->u.newStruct.list,
                        (Poly_tyVoid)Ast_Field_print);
    print ("}");
    return;
  case AST_EXP_NEW_ARRAY:
    print (AstId_toString (e->u.newArray.type));
    print ("[");
    X86_C_Exp_print (e->u.newArray.size);
    print ("] of (");
    X86_C_Exp_print (e->u.newArray.init);
    print (")");
    return;
  case AST_EXP_CALL:
  	/*
    print (AstId_toString (e->u.call.f));
    print ("(");
    LinkedList_foreach (e->u.call.args,
                        (Poly_tyVoid)X86_C_Exp_print);
    print (")");
    */
    return;
  case AST_EXP_LVAL:
    X86_C_Lval_print (e->u.lval);
    return;
  case AST_EXP_FLOATLIT:
  	print (float_toString2(e->u.floatlit));
    return;
  case AST_EXP_DOUBLELIT:
  	print(double_toString2(e->u.doublelit));
  	return ;
  default:
    printf ("%d", e->kind);
    Error_impossible ();
    return;
  }
  Error_impossible ();
  return;
}




static int current = 0;
static void indent ()
{
  current += 2;
}

static void unindent ()
{
  current -= 2;
}

static void spaces ()
{
  int i = current;
  while (i){
    print (" ");
    i--;
  }
}

Box_t X86_C_Stm_box (S s)
{
  return Box_str ("<junk>;");
}
void X86_C_board_label_print (label_for_boards_t s)
{
    print (s->label_name);
	print (":\n");

	X86_C_Block_print (s->body);

}

void X86_C_Stm_print (S s)
{
  Assert_ASSERT(s);
  switch (s->kind){
  case AST_STM_ASSIGN:{
    spaces ();
    X86_C_Lval_print (s->u.assign.lval);
    print (" = ");
    X86_C_Exp_print (s->u.assign.exp);
	if(!in_for)
    print (";");
    break;
  }
  case AST_STM_EXP:{
    spaces ();
    X86_C_Exp_print (s->u.exp);
    print (";");
    break;
  }
  case AST_STM_PRINTF:{
    spaces ();
    print ("printf");
	print(s->u.print);
    break;
  }
  case AST_STM_IF:
    spaces ();
    print ("if (");
    X86_C_Exp_print (s->u.iff.condition);
    print (")\n");
    indent ();
    X86_C_Stm_print (s->u.iff.then);
    unindent ();
    if (s->u.iff.elsee){
      spaces ();
      print ("else\n");
      indent ();
      X86_C_Stm_print (s->u.iff.elsee);
      unindent ();
    }
    break;
  case AST_STM_WHILE:
    spaces ();
    print ("while (");
    X86_C_Exp_print (s->u.whilee.condition);
    print (")\n");
    indent ();
    X86_C_Stm_print (s->u.whilee.body);
    unindent ();
    break;
  case AST_STM_DO:
    spaces ();
    print ("do {\n");
    indent ();
    X86_C_Stm_print (s->u.doo.body);
    unindent ();
    spaces ();
    print ("} while (");
    X86_C_Exp_print (s->u.doo.condition);
    break;
  case AST_STM_FOR:
    spaces ();
    print ("for (");
	in_for=1;
    X86_C_Stm_print (s->u.forr.header);
    print ("; ");
    X86_C_Exp_print (s->u.forr.condition);
    print ("; ");
    X86_C_Stm_print (s->u.forr.tail);
	in_for=0;
    print (")");
    X86_C_Stm_print (s->u.forr.body);
    break;
  case AST_STM_BREAK:
    spaces ();
    print ("break;");
    break;
  case AST_STM_CONTINUE:
    spaces ();
    print ("continue;");
    break;
  case AST_STM_RETURN:
    spaces ();
    print ("return ");
    X86_C_Exp_print (s->u.returnn);
    print (";");
    break;
  case AST_STM_BLOCK:
    X86_C_Block_print (s->u.block);
    break;
  case AST_STM_GOTO:
  	spaces ();
    print ("goto ");
    print (s->u.gotoo);
	print (";");
    break;
  case AST_STM_LABEL:
  	//spaces ();
    //print ("goto ");
    if((s->u.labell.body)->kind==AST_STM_END)
	print("return 0;\n");
    print (s->u.labell.label);
	print (":\n");
	X86_C_Stm_print (s->u.labell.body);
    break;
  case AST_STM_RESERVED_LABEL:
  	break;
  case AST_STM_UPDATE:
  case AST_STM_INTERNAL_FUNC:
  	break;
  case AST_STM_END:
  	//Happen end
  	//spaces ();
    print ("return 1;");

    break;
  default:
    printf ("%d", s->kind);
    Error_impossible ();
    break;
  }
  print ("\n");
  return;
}





Box_t X86_C_Dec_box (D d)
{
  Assert_ASSERT (d);

  return Box_h (Box_str (AstId_toString (d->type)),
                Box_str (AstId_toString (d->var)),
                (d->init)?
                (Box_str (" = <junk>;")):
                Box_str (";"),
                0);
}

void X86_C_Dec_print (D d)
{
  Assert_ASSERT (d);
  spaces ();
  print (AstId_toString (d->type));
  print (" ");
  print (AstId_toString (d->var));
  if (d->init){
    print (" = ");
    X86_C_Exp_print (d->init);
  }
  print (";\n");
  return;
}


Box_t X86_C_Block_box (B b)
{
  Box_t b1, b2, b3;

  Assert_ASSERT(b);
  b1 = Box_str ("{");
  b2 = Box_vlist (LinkedList_map (b->decs,
                                  (Poly_tyId)X86_C_Dec_box));
  b3 = Box_vlist (LinkedList_map (b->stms,
                                  (Poly_tyId)X86_C_Stm_box));
  b2 = Box_v (b2, b3, 0);
  b3 = Box_str ("}");
  return Box_v (b1, Box_indent (b2, 2), b3, 0);
}

void X86_C_Block_print (B b)
{
  Assert_ASSERT(b);
  spaces ();
  print ("{\n");
  indent ();
  LinkedList_foreach (b->decs, (Poly_tyVoid)X86_C_Dec_print);
  LinkedList_foreach (b->stms, (Poly_tyVoid)X86_C_Stm_print);
  unindent ();
  spaces ();
  print ("}");
  return;
}



Box_t X86_C_Fun_box (F f)
{
  Assert_ASSERT(f);
  Box_t b1 = Box_h
    (Box_str(AstId_toString (f->type)),
     Box_str(AstId_toString (f->name)),
     Box_str ("("),
     Box_hlist (LinkedList_map
                (f->args,
                 (Poly_tyId)Ast_Struct_box)),
     Box_str (")"),
     0);
  Box_t b2 = Ast_Block_box (f->block);
  return Box_v (b1, b2, 0);
}

void X86_C_Fun_print (F f)
{
  Assert_ASSERT(f);
  // I only expand the main func;
  if(strcmp(AstId_toString (f->name),"main")) return;
  D d;
  S s;
  List_t p;
  //labels_for_boards lb;
  int first_if=1;
  char str[1025];
  int read_count;
  print("int __is_digit(char c)\n");
  print("{\n");
  print("return c<='9'&&c>='0';\n");
  print("}\n");
  print ("int");
  print (" ");
  print (AstId_toString (f->name));
  print ("(");
  print ("int argc,char **argv");
  //LinkedList_foreach (f->args,
  //                    (Poly_tyVoid)Ast_Struct_print);
  print (")\n");
  FILE *add_code_fp=fopen("add-code.txt","r");
  if(!add_code_fp)
  	Error_bug ("can't open add-code file");
  spaces ();
  print ("{\n");
  while(!feof(add_code_fp)){
     read_count=fread(str,sizeof(char),1024,add_code_fp);
	 str[read_count]='\0';
     print(str);
  }
  fclose(add_code_fp);
  p= globalvar->next;
  while(p)
  	{
    d=(D)(p->data);
	if(first_if)
		{
		print("if");
		first_if=!first_if;
		}
	else print("else if");
	print("(!strcmp(\"");

    print (AstId_toString (d->var));
	print("\",");
    print("__indent))");
	print("\n");
	print (AstId_toString (d->var));
    print("=");
	print("__param_value");
	print(";");
	print("\n");
	p=p->next;
  }
  print ("}\n");
  indent ();
  LinkedList_foreach (labelforboards, (Poly_tyVoid)X86_C_board_label_print);

  LinkedList_foreach (f->block->decs, (Poly_tyVoid)Ast_Dec_print);
  LinkedList_foreach (f->block->stms, (Poly_tyVoid)X86_C_Stm_print);

  unindent ();
  spaces ();
  print ("}");
  print ("\n\n");
  find_print_block(f->block);


  return;
  }
int find_print_stm(S s)
{

	 Assert_ASSERT(s);
	 switch (s->kind){
	 case AST_STM_PRINTF:
	 	return 1;
	 case AST_STM_IF:
	   find_print_stm (s->u.iff.then);
	   if (s->u.iff.elsee){
		 find_print_stm (s->u.iff.elsee);
	   }
	   return 0;
	   break;
	 case AST_STM_WHILE:
	   find_print_stm (s->u.whilee.body);
	   return 0;
	   break;
	 case AST_STM_DO:
	   find_print_stm (s->u.doo.body);
	   break;
	 case AST_STM_FOR:
	   find_print_stm (s->u.forr.body);
	   return 0;
	   break;

	 case AST_STM_BLOCK:
	   find_print_block (s->u.block);
	   return 0;
	   break;
	 default:
	  // printf ("%d", s->kind);
	  // Error_impossible ();
	  return 0;
	   break;

	 }

	 return;


}
int find_print_block(B b)
{

   List_t p,pre;
   S s ;
   pre=b->stms;
   p=b->stms->next;
 while(p)
 	{
 	   //  Elab_stm(p);
         s=(S)p->data;
	     if(s->kind==AST_STM_LABEL&&(s->u.labell.body)->kind==AST_STM_PRINTF)
	     {
         // free(s->u.labell.body);
		  p=p->next;
		  s->u.labell.body=(S)p->data;
		  continue;

		 }
		 if(find_print_stm(s))
		 {
		 pre->next=p->next;
	//	 free(p); error why?
		 p=pre->next;
		 continue;
		 }
		 else
		 pre=p;
        p=p->next;
 }


}
Box_t X86_C_Struct_box (Ast_Struct_t x)
{
  Assert_ASSERT(x);
  return Box_str (String_concat
                  (AstId_toString (x->type),
                   " ",
                   AstId_toString (x->var),
                   ";",
                   0));
}

void X86_C_Struct_print (Ast_Struct_t x)
{
  Assert_ASSERT(x);
  spaces ();
  print (AstId_toString (x->type));
  print (" ");
  print (AstId_toString (x->var));
  print (";\n");
}




Box_t X86_C_Type_box (T x)
{
  Assert_ASSERT (x);
  switch (x->kind){
  case TYPE_STRUCT:{
    Box_t b =
      Box_h
      (Box_str ("{"),
       Box_hlist (LinkedList_map
                  (x->u.structt,
                   (Poly_tyId)Ast_Struct_box)),
       Box_str ("}"),
       0);
    return b;
  }
  case TYPE_ARRAY:
    return Box_str (String_concat
                    (AstId_toString (x->u.array),
                     "[]",
                     0));
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

void X86_C_Type_print (T x)
{
  Assert_ASSERT (x);
  switch (x->kind){
  case TYPE_STRUCT:
    print ("{");
    LinkedList_foreach (x->u.structt,
                        (Poly_tyVoid)Ast_Struct_print);
    print ("};");
    return;
  case TYPE_ARRAY:
    print (AstId_toString (x->u.array));
    print ("[];");
    return;
  default:
    Error_bug ("impossible");
    return;
  }
  Error_bug ("impossible");
  return;
}




Box_t X86_C_Td_box (Td ds)
{
  return Box_h (Box_str (String_concat
                         ("type ",
                          AstId_toString (ds->name),
                          " = ",
                          0)),
                Ast_Type_box (ds->type),
                Box_str (";"),
                0);
}

void X86_C_Td_print (Td ds)
{
  print ("type ");
  print (AstId_toString (ds->name));
  print (" = ");
  Ast_Type_print (ds->type);
  print ("\n");
  return;
}



Box_t X86_C_Prog_box (P x)
{
  LinkedList_t boxes =
    LinkedList_map (x->tds,
                    (Poly_tyId)Ast_Td_box);
  LinkedList_t boxes2 =
    LinkedList_map (x->funcs,
                    (Poly_tyId)Ast_Fun_box);
  LinkedList_append (boxes, boxes2);
  return Box_vlist (boxes);
}

void X86_C_Prog_print (P x, File_t f)
{
  Assert_ASSERT(x);
  file = f;
  globalvar=x->globals;
  print("#include<stdio.h>\n");
  print("#include<stdlib.h>\n");
  LinkedList_foreach (x->tds,
                      (Poly_tyVoid)Ast_Td_print);
  LinkedList_foreach(x->globals,(Poly_tyVoid)X86_C_Dec_print);
  print ("\n");

  LinkedList_foreach (x->funcs,
                      (Poly_tyVoid)X86_C_Fun_print);
}














#undef P
#undef T
#undef Td
#undef F
#undef S
#undef L
#undef B
#undef D
#undef E

#undef Id_t
#undef List_t
