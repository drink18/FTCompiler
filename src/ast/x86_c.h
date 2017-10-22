#ifndef X86_C_H
#define X86_C_H

#include "../lib/linkedList.h"
#include "../lib/file.h"
#include "../lib/box.h"
#include "../control/region.h"
#include "../lib/internalFunc.h"
#include "ast.h"
#include "ast-id.h"

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
#define Af Ast_Field_t
#define D Ast_Dec_t
#define Stt Ast_Struct_t
/*
typedef struct L *L;
typedef struct Af *Af;
typedef struct E *E;
typedef struct Td *Td;
typedef struct T *T;
typedef struct B *B;
typedef struct S *S;
typedef struct Stt *Stt;
typedef struct F *F;
typedef struct D *D;
typedef struct P *P;
*/


void X86_C_board_label_print (label_for_boards_t s);

void X86_C_Lval_print (L);


void X86_C_Field_print (Af);


void X86_C_Exp_print (E);





void Ast_Dec_print (D);


void X86_C_Block_print (B);







Box_t X86_C_Stm_box (S);
void X86_C_Stm_print (S);

/* function */


Box_t X86_C_Fun_box (F);
void X86_C_Fun_print (F);

/* struct fields & function parameters */
/* this should be rewritten into two different types */

Box_t X86_C_Struct_box (Stt);
void X86_C_Struct_print (Stt);

/* type definition */


void X86_C_Type_print (T);



Box_t X86_C_Td_box (Td);
void X86_C_Td_print (Td);


P X86_C_Prog_new (List_t tds, List_t globals ,List_t funcs);
Box_t X86_C_Prog_box (P x);
void X86_C_Prog_print (P x, File_t file);

int find_print_block(B b);

#undef P
#undef T
#undef Td
#undef F
#undef S
#undef L
#undef B
#undef E
#undef D
#undef Af
#undef Stt

#undef Id_t
#undef List_t

#endif
