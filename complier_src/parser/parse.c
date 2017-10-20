#include "../lib/assert.h"
#include "../lib/string.h"
#include "../lib/hash.h"
#include "../lib/error.h"
#include "../lib/linkedList.h"
#include "../lib/tuple.h"
#include "../lib/trace.h"
#include "../lib/internalFunc.h"
#include "../control/error-msg.h"
#include "../control/region.h"
#include "../main/main-main.h"
#include "../lib/mem.h"
#include "parse.h"
#include "../lib/wavefile.h"
#define CheckBoard(kind) (kind>=TOKEN_MAINB&&kind<=TOKEN_GR)
#define CheckBox(kind) (kind>=TOKEN_M&&kind<=TOKEN_E4)
#define CheckRx(kind) (kind>=TOKEN_RX1&&kind<=TOKEN_RX4)
#define BOX_NUM (TOKEN_E4-TOKEN_M)
static Token_t current = 0;
/* tyTable: String_t -> 1 */
static Hash_t tyTable = 0;

LinkedList_t labelforboards=0;
static LinkedList_t funListX86=0;
static Ast_Block_t x86Block=0;
static LinkedList_t globDecs=0;

static Ast_Fun_t funInLoop=0;//find in globalVar loop
static int nocall=0;
LinkedList_t wave_file_list=0;
static InternalFun convertInteranlFunc(int kind)
{
   switch(kind)
   	{
       case TOKEN_DEALY:
	   	return DELAY;
	   case TOKEN_TRIGGER:
	   	return TRIGGER;
	   case TOKEN_WAIT:
	   	return WAIT;
	   case TOKEN_SETR:
	   	return  SETR;
	   case TOKEN_TIMER:
	   	return TIMER;
	   default:
	   	Error_bug ("impossible");
		return 0;

   }
   Error_bug ("impossible");
   return 0;
}


static void advance ();
static Token_t eatToken (int kind);

static Ast_Exp_Kind_t convert (int);

static Ast_Lval_t Parse_lval ();
static Ast_Exp_t Parse_exp ();
static Ast_Stm_t Parse_stm ();
static Ast_Block_t Parse_block ();
static LinkedList_t Parse_globalDefinitionList();

static void error (String_t msg, Region_t r)
{
  ErrorMsg_syntaxError (msg, r);
}

static Region_t region = 0;

static void error_dupTypeName (String_t id)
{
  error (String_concat
         ("type name must not be redefined: ",
          id,
          0),
         region);
}

static void advance ()
{
  next ();
  Token_t temp;

  Mem_NEW(temp);
  temp->kind = tok;
  temp->region = Region_new(Id_fromString (String_new(file->filename)),file->line_num);
  current=temp;
}


static Token_t eatToken (int kind)
{
  Token_t token;
  String_t litr=get_tok_str(tok,&tokc);
  token = current;
  if(kind==TOKEN_ID)
  {
     if(current->kind<TOKEN_ID)
	 tcc_error("expect identifier ");
  }
  //check if is a valid type
  else if(kind==TOKEN_TYPE)
  {
     if(current->kind!=TOK_DOUBLE&&current->kind!=TOK_INT&&current->kind!=TOK_VOID)
     tcc_error("expect type(double,int,void) ");
  }
  else if (current->kind != kind)

     tcc_error ("expect (%d)%s,(got (%d)\"%s\")",kind,get_tok_str(kind,NULL),tok,get_tok_str(tok,&tokc));
     //tcc_error ("expect (%d)%s,(got (%d)\"%s\")",kind,get_tok_str(kind,NULL),current->kind,get_tok_str(current->kind,NULL));
  advance ();
  return token;
}

static AstId_t convertToken (Token_t t)
{
  Assert_ASSERT (t->kind == TOKEN_ID);
  return AstId_fromString (get_tok_str(t->kind,NULL), t->region);
}

LinkedList_t Parse_parameters ()
{
  LinkedList_t list = LinkedList_new ();
  Ast_Exp_t e;

  if (current->kind == TOKEN_RPAREN)
    return list;
  e = Parse_exp ();
  LinkedList_insertLast (list, e);
  while (current->kind == TOKEN_COMMER){
    advance ();
    e = Parse_exp ();
    LinkedList_insertLast (list, e);
  }
  return list;
}

LinkedList_t Parse_fieldList ()
{
  LinkedList_t list = LinkedList_new ();
  Ast_Exp_t e;
  AstId_t id;
  Token_t s;
  Ast_Field_t f;

  while (current->kind != TOKEN_RBRACE){
    s = eatToken (TOKEN_ID);
    id = convertToken (s);
    eatToken(TOKEN_ASSIGN);
    e = Parse_exp ();
    eatToken (TOKEN_SEMI);
    f = Ast_Field_new (id, e);
    LinkedList_insertLast (list, f);
  }
  return list;
}

static Ast_Exp_t Parse_lvalOrCall ()
{
  Token_t s;
  AstId_t id;

  s = eatToken(TOKEN_ID);
  id = convertToken (s);
  switch (current->kind){
  case TOKEN_LBRACK:
  case TOKEN_DOT:{
    Ast_Lval_t lval;

    lval = Parse_lval (s);
    return Ast_Exp_new_lval (lval);
  }
  case TOKEN_LPAREN:{
    LinkedList_t args = LinkedList_new ();

    advance ();
    args = Parse_parameters ();
    eatToken (TOKEN_RPAREN);
    return Ast_Exp_new_call (id, args, 0);
  }
  default:{
    Ast_Lval_t lval = Ast_Lval_new_var (id, s->region);
    return Ast_Exp_new_lval (lval);
  }
  }
  Error_bug ("impossible");
  return 0;
}

static Ast_Exp_t Parse_arrayOrStruct ()
{
  Token_t s;
  AstId_t id;

  s = eatToken(TOKEN_ID);
  id = convertToken (s);
  switch (current->kind){
  case TOKEN_LBRACK:{
    Ast_Exp_t e1, e2;

    advance ();
    e1 = Parse_exp ();
    eatToken (TOKEN_RBRACK);
    eatToken (TOKEN_OF);
    e2 = Parse_exp ();
    return Ast_Exp_new_newArray (id,
                                 e1,
                                 e2);
  }
  case TOKEN_LBRACE:{
    LinkedList_t fields = LinkedList_new ();

    advance ();
    fields = Parse_fieldList ();
    eatToken (TOKEN_RBRACE);
    return Ast_Exp_new_newStruct (id, fields);
  }
  default:{
    error  (String_concat ("expects: { or [\n",
                           "but got: ",
                           get_tok_str (tok,&tokc),
                           0),
            s->region);
    return 0;
  }
  }
  Error_impossible();
  return 0;
}

static Ast_Exp_Kind_t convert (int kind)
{
  switch (kind){
  case TOK_SHL:
  	return AST_EXP_SL;
  case TOK_LT:
    return AST_EXP_LT;
  case TOK_LE:
    return AST_EXP_LE;
  case TOK_GE:
    return AST_EXP_GE;
  case TOK_GT:
    return AST_EXP_GT;
  case TOK_EQ:
    return AST_EXP_EQ;
  case TOK_NE:
    return AST_EXP_NE;
  case TOKEN_ADD:
    return AST_EXP_ADD;
  case TOKEN_MINUS:
    return AST_EXP_SUB;
  case TOKEN_TIMES:
    return AST_EXP_TIMES;
  case TOKEN_DIVIDE:
    return AST_EXP_DIVIDE;
  case TOKEN_PERCENT:
    return AST_EXP_MODUS;
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

static Ast_Exp_t Parse_factor ()
{
  Ast_Exp_t e;
  String_t s;
  //if(current->kind>TOKEN_ID)
  //return Parse_lvalOrCall ();
    if(current->kind > TOKEN_ID)
    {
        if(Hash_lookup(tyTable,get_tok_str(current->kind,NULL)))
        {
            return Parse_arrayOrStruct();
        }
        else
        {
            return Parse_lvalOrCall();
        }

    }
  switch (current->kind){
  case TOKEN_LPAREN:
    advance ();
    e = Parse_exp ();
    eatToken (TOKEN_RPAREN);
    return e;
  case TOK_CINT:
    advance ();
    return Ast_Exp_new_intlit (tokc.ui);
  case TOK_CFLOAT:
    advance ();
    return Ast_Exp_new_floatlit (tokc.f);
  case TOK_CDOUBLE:
    advance ();
    return Ast_Exp_new_doublelit_value (tokc.d);
  case TOK_DOUBLE:
  case TOK_INT:
      return Parse_arrayOrStruct ();

  /*
  not meaningful in current platform
  case TOKEN_STRINGLIT:
    s = current->lexeme;
    advance ();
    return Ast_Exp_new_stringlit (s);
*/
  case TOK_NULL:
    advance ();
    return Ast_Exp_new_null ();
  default:
    tcc_error ("expects: (integer,identifier,string,null),got (\"%s\")",
              get_tok_str(tok,&tokc));
    return (Poly_t)0;
  }
  Error_bug ("impossible");
  return (Poly_t)0;
}

static Ast_Exp_t Parse_unary ()
{
  Ast_Exp_t e;
  LinkedList_t all = LinkedList_new ();

  while (current->kind == TOKEN_MINUS
         || current->kind == TOKEN_NOT){
    Region_t r = current->region;
    LinkedList_insertFirst
      (all,
       Tuple_new ((Poly_t)current->kind, r));
    advance ();
  }
  e = Parse_factor ();
  all = LinkedList_getFirst (all);
  while (all){
    Tuple_t tuple = (Tuple_t)all->data;
    int kind = (int)Tuple_first (tuple);
    Region_t r = Tuple_second (tuple);
    if (kind == TOKEN_MINUS){
	  //let's do quick
	  if(e->kind==AST_EXP_DOUBLELIT)
	  e=Ast_Exp_new_doublelit_value (e->u.doublelit*-1);
      else if(e->kind==AST_EXP_INTLIT)
	  e=Ast_Exp_new_intlit (e->u.intlit*-1);
      else e = Ast_Exp_new_unary (AST_EXP_NEGATIVE,
                             e,
                             r);
    }
    else if (kind == TOKEN_NOT)
      e = Ast_Exp_new_unary (AST_EXP_NOT, e, r);
    else Error_bug ("impossible");
    all = all->next;
  }
  return e;
}

static Ast_Exp_t Parse_term ()
{
  Ast_Exp_t e1, e2;
  int kind;

  e1 = Parse_unary ();
  while (current->kind == TOKEN_TIMES
         || current->kind == TOKEN_DIVIDE
         || current->kind == TOKEN_PERCENT){
    Region_t r = current->region;
    kind = current->kind;
    advance ();
    e2 = Parse_unary ();
    e1 = Ast_Exp_new_bop (convert (kind),
                          e1,
                          e2,
                          r);
  }
  return e1;
}

static Ast_Exp_t Parse_expr ()
{
  Ast_Exp_t e1, e2;
  int kind;

  e1 = Parse_term ();
  while (current->kind == TOKEN_ADD
         || current->kind == TOKEN_MINUS){
    Region_t r = current->region;
    kind = current->kind;
    advance ();
    e2 = Parse_term ();
    e1 = Ast_Exp_new_bop (convert (kind),
                          e1,
                          e2,
                          r);
  }
  return e1;
}
static Ast_Exp_t Parse_shift ()
{

 Ast_Exp_t e1, e2;
 int kind;

 e1 = Parse_expr ();
 while (current->kind == TOK_SHL){
   Region_t r = current->region;
   kind = current->kind;
   advance ();
   e2 = Parse_expr ();
   e1 = Ast_Exp_new_bop (convert (kind),
						 e1,
						 e2,
						 r);
 }
 return e1;

}


static Ast_Exp_t Parse_rel ()
{
  Ast_Exp_t e1, e2;
  int kind;

  e1=Parse_shift();
  while (current->kind == TOK_LT
         || current->kind == TOK_LE
         || current->kind == TOK_GE
         || current->kind == TOK_GT){
    Region_t r = current->region;
    kind = current->kind;
    advance ();
    e2 = Parse_shift();
    e1 = Ast_Exp_new_bop (convert (kind),
                          e1,
                          e2,
                          r);
  }
  return e1;
}


static Ast_Exp_t Parse_equality ()
{
  Ast_Exp_t e1, e2;
  int kind;

  e1 = Parse_rel ();
  while (current->kind == TOK_EQ
         || current->kind == TOK_NE){
    Region_t r = current->region;
    kind = current->kind;
    advance ();
    e2 = Parse_rel ();
    e1 = Ast_Exp_new_bop (convert(kind),
                          e1,
                          e2,
                          r);
  }
  return e1;
}

static Ast_Exp_t Parse_join ()
{
  Ast_Exp_t e1, e2;

  e1 = Parse_equality ();
  while (current->kind == TOKEN_AND){
    Region_t r = current->region;
    advance ();
    e2 = Parse_equality ();
    e1 = Ast_Exp_new_bop (AST_EXP_AND,
                          e1,
                          e2,
                          r);
  }
  return e1;
}


static Ast_Exp_t Parse_bool ()
{
  Ast_Exp_t e1, e2;

  e1 = Parse_join ();
  while (current->kind == TOKEN_OR){
    Region_t r = current->region;
    advance ();
    e2 = Parse_join ();
    e1 = Ast_Exp_new_bop (AST_EXP_OR,
                          e1,
                          e2,
                          r);
  }
  return e1;
}

static Ast_Exp_t Parse_exp ()
{
  return Parse_bool ();
}

static Ast_Lval_t Parse_lval (Token_t first)
{
  Ast_Lval_t lval;
  AstId_t id;
  Ast_Exp_t exp;
  Region_t r;

  if (!first){
    first = eatToken (TOKEN_ID);
  }

  r = first->region;
  id = convertToken (first);
  lval = Ast_Lval_new_var (id, r);
  for (;;){
    switch (current->kind){
    case TOKEN_DOT:
      advance ();
      first = eatToken(TOKEN_ID);
      id = convertToken (first);
      lval = Ast_Lval_new_dot (lval, id, r);
      break;
    case TOKEN_LBRACK:
      advance ();
      exp = Parse_exp ();
      eatToken (TOKEN_RBRACK);
      lval = Ast_Lval_new_array (lval, exp, r);
      break;

    default:
      return lval;
    }
  }
  Error_bug ("impossible");
  return 0;
}

static Ast_Stm_t Parse_for ()
{
  Ast_Exp_t condition;
  Ast_Stm_t init, tail, body;
  Token_t t;

  advance ();
  t = eatToken (TOKEN_LPAREN);
  init = Parse_stm ();
  condition = Parse_exp ();
  eatToken (TOKEN_SEMI);
  tail = Parse_stm ();
  eatToken (TOKEN_RPAREN);
  body = Parse_stm ();
  return Ast_Stm_new_for (init,
                          condition,
                          tail,
                          body,
                          t->region);
}

static Ast_Stm_t Parse_do ()
{
  Ast_Exp_t condition;
  Ast_Stm_t body;
  Token_t t;

  advance ();
  body = Parse_stm ();
  t = eatToken (TOK_WHILE);
  eatToken (TOKEN_LPAREN);
  condition = Parse_exp ();
  eatToken (TOKEN_RPAREN);
  eatToken (TOKEN_SEMI);
  return Ast_Stm_new_do (condition,
                         body,
                         t->region);
}

static Ast_Stm_t Parse_while ()
{
  Ast_Exp_t condition;
  Ast_Stm_t body;
  Token_t t;

  advance ();
  t = eatToken (TOKEN_LPAREN);
  condition = Parse_exp ();
  eatToken (TOKEN_RPAREN);
  body = Parse_stm ();
  return Ast_Stm_new_while (condition,
                            body,
                            t->region);
}

static Ast_Stm_t Parse_if ()
{
  Ast_Exp_t condition;
  Ast_Stm_t then, elsee = 0;
  Token_t t;

  advance ();
  t = eatToken (TOKEN_LPAREN);
  condition = Parse_exp ();
  eatToken (TOKEN_RPAREN);
  then = Parse_stm ();
  if (current->kind == TOK_ELSE){
    advance ();
    elsee = Parse_stm ();
  }
  return Ast_Stm_new_if (condition,
                         then,
                         elsee,
                         t->region);
}

// add support for label
// statment form : label : statement;
void AddBoardLabel(String_t name,Ast_Block_t labelBlock)
{
	label_for_boards_t lb;
	lb=Ast_new_lb();
	if(labelBlock==NULL)
	lb->body=Parse_block();  // use {} for block
	else
	lb->body=labelBlock;
	//Ast_Stm_new_block (labelBlock);
    lb->label_name=name;

	LinkedList_insertLast (labelforboards,lb);

}
static Ast_Stm_t Parse_assignment ()
{
  Ast_Lval_t lvalfirst;
  Ast_Exp_t exp;
  Token_t t;
  Ast_Lval_t lval=0;
  AstId_t id;
  Region_t r;
  Ast_Block_t labelBlock;
  Token_t firstId,secondId;
  int thechar;
  label_for_boards_t lb;
  String_t name;
  firstId = eatToken (TOKEN_ID);
 // lval = Parse_lval (firstId)
  r = firstId->region;
  id = convertToken (firstId);
  name=AstId_toString(id);

  lvalfirst = Ast_Lval_new_var (id, r);
  LinkedList_t boxlist=0,p;
  int i,firstflag=1;
  int box_nset[BOX_NUM+1];
  for (;;){
    switch (current->kind){
    case TOKEN_DOT:
      advance ();
      firstId = eatToken(TOKEN_ID);
      id = convertToken (firstId);
      lval = Ast_Lval_new_dot (lvalfirst, id, r);
      break;
    case TOKEN_LBRACK:
      advance ();
      exp = Parse_exp ();
      eatToken (TOKEN_RBRACK);
      lval = Ast_Lval_new_array (lvalfirst, exp, r);
      break;
	case TOKEN_COMMER:

		//more than one box or board
		//init 0:for not set
		for(i=0;i<=BOX_NUM;i++) box_nset[i]=1;
		for(i=0;i<=BOX_NUM;)
		{



		if(CheckBox(firstId->kind))
		{
		   if(box_nset[firstId->kind-TOKEN_M])
		   	{
		    box_nset[firstId->kind-TOKEN_M]=0;
			if(firstflag)
			{
			    boxlist=LinkedList_new();
				LinkedList_insertLast(boxlist,name);
				firstflag=0;

			}
			else
			{
			LinkedList_insertLast(boxlist,name);
			}
			i++;

		   	}
		    else
			ErrorMsg_syntaxError(String_concat("extend box ",name," already set",0),current->region);
		if(current->kind==TOKEN_COLON)
		{
					advance();
					secondId=eatToken(TOKEN_ID);
					break;

		}else if(current->kind==TOKEN_COMMER)
		{
		            advance();
					firstId = eatToken (TOKEN_ID);
					id = convertToken (firstId);
  					name=AstId_toString(id);
					continue;
		}else ErrorMsg_syntaxError("expect split char '\,' ",current->region);

		}
		else  ErrorMsg_syntaxError("Please specify correct box prefix:[M,E1,E2,E3,E4]",current->region);

		}

		if(CheckRx(secondId->kind))
		{
		    p=boxlist->next;
        	eatToken(TOKEN_COLON);
			labelBlock=Parse_block();  // use {} for block

			while(p)
			{
				AddBoardLabel(String_concat(p->data,":",get_tok_str(secondId->kind,NULL),0),labelBlock);
				p=p->next;
			}

			return Ast_Stm_new_label(0, 0, 0,1);
		}
		else
		ErrorMsg_syntaxError("extend box only support Rx board",current->region);

		break;
    case TOKEN_COLON:
		advance ();
		if(CheckBox(firstId->kind))
		{
   			secondId=eatToken(TOKEN_ID);
			if(CheckRx(secondId->kind))
			{
        	eatToken(TOKEN_COLON);
			AddBoardLabel(String_concat(name,":",get_tok_str(secondId->kind,NULL),0),NULL);
			return Ast_Stm_new_label(0, 0, 0,1);
			}
			else
			ErrorMsg_syntaxError("extend box only support Rx board",current->region);

		}
        else if(CheckBoard(firstId->kind))
			{
			AddBoardLabel(name,NULL);
			return Ast_Stm_new_label(0, 0, 0,1);
			}
		else
			{
			Ast_Stm_t s= Parse_stm ();
		return Ast_Stm_new_label(name, s, firstId->region,0);
			}
   case TOKEN_ASSIGN:
   	   advance ();
   	   exp = Parse_exp ();
  	   eatToken (TOKEN_SEMI);
       return Ast_Stm_new_assign((lval==0)?lvalfirst:lval, exp, r);
   case TOKEN_LPAREN:
   	{
    LinkedList_t args = LinkedList_new ();

    advance ();
    args = Parse_parameters ();
    eatToken (TOKEN_RPAREN);
	eatToken (TOKEN_SEMI);
	nocall=0;
	return Ast_Stm_new_exp(Ast_Exp_new_call (id, args, 0),r);

    }
    default:
    ErrorMsg_syntaxError("unrecongnise statements ",current->region);
	//Error_bug ("impossible");
    //return lval;
    }
  }


}
static Ast_Stm_t Parse_stm ()
{
  Ast_Exp_t e;
  Ast_Block_t b;
  Token_t id;
  InternalFun iFun;

  if(tok>TOKEN_ID)
  	return Parse_assignment ();
  switch (current->kind){
  /*
  case TOKEN_PRINTF:
  {
  	id=current;
	Region_t r = id->region;
	advance();
    return Ast_Stm_new_printf (id->lexeme,r);
  }
  */
  case TOK_IF:
    return Parse_if ();
  case TOK_WHILE:
    return Parse_while();
  case TOK_DO:
    return Parse_do ();
  case TOK_FOR:
    return Parse_for ();
  case TOK_BREAK:{
    Region_t r = current->region;
    advance ();
    eatToken (TOKEN_SEMI);
    return Ast_Stm_new_break (r);
  }
  case TOK_CONTINUE:{
    Region_t r = current->region;
    advance ();
    eatToken (TOKEN_SEMI);
    return Ast_Stm_new_continue (r);
  }
   case TOK_END:{
    Region_t r = current->region;
    advance ();
    eatToken (TOKEN_SEMI);
    return Ast_Stm_new_end (r);
  }
   case TOKEN_UPDATE:{
    Region_t r = current->region;
    advance ();
    eatToken (TOKEN_SEMI);
    return Ast_Stm_new_update (r);
  }
  case TOK_RETURN:{
    Region_t r = current->region;
    advance ();
    e = Parse_exp ();
    eatToken(TOKEN_SEMI);
    return Ast_Stm_new_return (e, r);
  }
  case TOKEN_LBRACE:
    b = Parse_block ();
    return Ast_Stm_new_block (b);
  case TOK_GOTO:{
  	Region_t r = current->region;
	advance ();
	id=eatToken(TOKEN_ID);
    eatToken(TOKEN_SEMI);
    return Ast_Stm_new_goto(String_new(get_tok_str(id->kind,NULL)),r);
  	}
  case TOKEN_DEALY:
  case TOKEN_TIMER:
  case TOKEN_TRIGGER:
  case TOKEN_SETR:
  case TOKEN_WAIT:
	{

  	LinkedList_t args = LinkedList_new ();
  	Region_t r = current->region;
	iFun=convertInteranlFunc(current->kind);
	advance ();
	id=eatToken(TOKEN_LPAREN);


	///advance ();
	args = Parse_parameters ();
	eatToken (TOKEN_RPAREN);
    eatToken(TOKEN_SEMI);

    return Ast_Stm_new_internalfunc(iFun,args,r);
  	}
  default:

    return (Poly_t)0;
  }
  Error_bug ("");

  return (Poly_t)0;
}




LinkedList_t Parse_statementList ()
{
  LinkedList_t list = LinkedList_new ();
  Ast_Stm_t s;

  for (;;){

    s = Parse_stm ();

    if (s)
    	{
    	if(s->kind == AST_STM_RESERVED_LABEL)
			{
				continue;
			}

        LinkedList_insertLast (list, s);
    	}
    else return list;
  }
  Error_bug ("impossible");
  return 0;
}


static Ast_Block_t Parse_block ()
{
  LinkedList_t decs, stms;
  Ast_Block_t b;

  eatToken (TOKEN_LBRACE);
  decs = Parse_declarationList (0);
  stms = Parse_statementList ();
  eatToken (TOKEN_RBRACE);
  b = Ast_Block_new (decs, stms);
  return b;
}

static LinkedList_t Parse_arguments ()
{
  LinkedList_t list = LinkedList_new ();
  Token_t type, name;
  AstId_t id1, id2;

  if (current->kind < TOK_IDENT)
    return list;
  type = eatToken (TOKEN_TYPE);
  name = eatToken (TOKEN_ID);
  id1 = convertToken (type);
  id2 = convertToken (name);
  LinkedList_insertLast (list, Ast_Struct_new (id1, id2));
  while (current->kind == TOKEN_COMMER){
    advance ();
    type = eatToken (TOKEN_TYPE);
    name = eatToken (TOKEN_ID);
    id1 = convertToken (type);
    id2 = convertToken (name);
    LinkedList_insertLast (list, Ast_Struct_new (id1, id2));
  }
  return list;
}

static LinkedList_t Parse_functionDefinitionList ()
{
  LinkedList_t gl,list = LinkedList_new (), paras;
  globDecs= LinkedList_new ();

  Token_t type, name;
  AstId_t id1, id2;
  Ast_Fun_t f;
  Ast_Block_t b;
  Region_t r;
  label_for_boards_t s;
  LinkedList_t p;
  while (current->kind != TOK_EOF){
  	funInLoop=0;
  	gl= Parse_declarationList(1);
	if(gl)
	{
	LinkedList_append(globDecs,gl);
	}

    if(funInLoop)
    {
	LinkedList_insertLast (list, funInLoop);
	continue;
    }

    r = current->region;

    type = eatToken (TOKEN_TYPE);
    id1 = convertToken (type);
    name = eatToken (TOKEN_ID);
    id2 = convertToken (name);

    eatToken (TOKEN_LPAREN);
    paras = Parse_arguments ();
    eatToken (TOKEN_RPAREN);
	nocall=FUNC_WITH_NO_FUNC;
    b = Parse_block ();
    if(CheckBoard(name->kind)) nocall=FUNC_FOR_BOARD;
    f = Ast_Fun_new (id1, id2, paras, b,nocall, r);

    LinkedList_insertLast (list, f);
  }
  p=labelforboards->next;
  paras=LinkedList_new ();
  while(p)
  {
    s=(Ast_Stm_t)p->data;
	id1= AstId_fromString("void",Region_bogus ());
    id2= AstId_fromString(s->label_name,Region_bogus ());
	f = Ast_Fun_new (id1, id2, paras, s->body,FUNC_FOR_BOARD,Region_bogus ());
    LinkedList_insertLast (list, f);
    p=p->next;
  }

  return list;
}

static LinkedList_t Parse_structBody ()
{
  Token_t s1, s2;
  AstId_t type, name;
  LinkedList_t list = LinkedList_new ();

  while (current->kind == TOKEN_ID){
    s1 = eatToken (TOKEN_ID);
    type = convertToken (s1);
    s2 = eatToken (TOKEN_ID);
    name = convertToken (s2);
    eatToken (TOKEN_SEMI);
	return list;
    LinkedList_insertLast (list, Ast_Struct_new (type,
                                                 name));
  }
}
static int Parse_type(bool *atr)
{

   int typefound=0;
   char *retc;

   while(1)
   {
   switch(tok)
   	{
		case TOK_STATIC:
		(*atr)|=VT_STATIC;
		advance();
		continue;
		break;
		case TOK_INT:
		return 1;
		break;
		case TOK_DOUBLE:
		return 1;
		break;
		default:
			retc = get_tok_str(tok,NULL);
			if(retc != NULL)
			{
				if(Hash_lookup(tyTable,retc))
				{
					return 1;
				}
				if(*atr==VT_STATIC)
					tcc_error("expect valid type(int,double)");
				else return 0;
			}
			else
			{
				return 0;
			}
    }
    }

}
/*********************
*	flag:1  parse global decl
*     flag:0  parse local  decl
**********************/
static LinkedList_t Parse_declarationList(int flag)
{
  LinkedList_t list = LinkedList_new (),paras,b;
  Token_t type, name;
  AstId_t id1, id2;
  Ast_Exp_t init = 0;
  Region_t r;
  bool static_set;

  for (;;){
  	static_set=0;
	if(!Parse_type(&static_set))
	{

		return list;
	}
	type=current;


	id1 = convertToken (type);


	advance();
	VariableNameStart:
    name = eatToken (TOKEN_ID);


    id2 = convertToken (name);


    if (current->kind == TOKEN_ASSIGN){
      advance ();
	  //if(current->kind!=TOKEN_INTLIT&&current->kind!=TOKEN_FLOATLIT)
      init = Parse_exp ();
	//when  I do parse if double a=3; transfer 3 to double 3
     	if(type->kind==TOK_DOUBLE&&init->kind==AST_EXP_INTLIT)
     	{
			  init->kind = AST_EXP_DOUBLELIT;
  			  init->u.doublelit =init->u.intlit ;
	 	}


    }
	///funs
    if(flag&&current->kind == TOKEN_LPAREN)
	{
	advance();
    paras = Parse_arguments ();
    eatToken (TOKEN_RPAREN);
	nocall=FUNC_WITH_NO_FUNC;
    b = Parse_block ();
	if(CheckBoard(name->kind)) nocall=FUNC_FOR_BOARD;
    funInLoop = Ast_Fun_new (id1, id2, paras, b, nocall,r);
	return list;
	}
	if(current->kind == TOKEN_COMMER)
	{
		LinkedList_insertLast (list,
                           Ast_Dec_new (id1, id2, init, static_set));

		advance();
		goto VariableNameStart;
	}
    eatToken (TOKEN_SEMI);
    LinkedList_insertLast (list,
                           Ast_Dec_new (id1, id2, init,static_set));
    init = 0;
  }
  return list;

}

static LinkedList_t Parse_waveList()
{
	int bram_no,ch_no;
	String_t wavefile;
	wavefile_t w_t;
   while (current->kind == TOKEN_IMPORT){
   	advance ();
    if(current->kind==TOKEN_GRAD_WAVE)
    {
		advance ();
		if(current->kind!=TOK_STR)
		{
			tcc_error("expect wavefile ");
		}
		else
		{
			if(_access(get_tok_str (tok,&tokc), 0))
			error (String_concat
             ("can't access file ",
              get_tok_str (tok,&tokc),
              0),
             current->region);

		}
		LinkedList_insertFirst(wave_file_list,wavefile_new_grad(String_concat(get_tok_str (tok,&tokc),
              0)));
		advance ();

	}
	else if(current->kind==TOKEN_RF_WAVE)
	{
	advance ();
    if(current->kind==TOK_CINT)
    {
		ch_no=tokc.ui;
	} else {error (String_concat
             ("expects: Channnel No"
              "but got: ",
              get_tok_str (tok,&tokc),
              0),
             current->region);}
	advance ();
	if(current->kind==TOK_CINT)
    {
		bram_no=tokc.ui;
	} else {error (String_concat
             ("expects: Bram No"
              "but got: ",
              get_tok_str (tok,&tokc),
              0),
             current->region);}
	advance ();
	if(current->kind!=TOK_STR)
	{
		tcc_error("expect wavefile ");
	}
	else
	{
			if(_access(get_tok_str (tok,&tokc), 0))
			error (String_concat
             ("can't access file ",
              get_tok_str (tok,&tokc),
              0),
             current->region);

	}
	wavefile=String_concat(get_tok_str (tok,&tokc),0);
	advance ();
	if(current->kind<TOKEN_ID)
	{
		tcc_error("expect wavename ");
	}
	LinkedList_insertFirst(wave_file_list,wavefile_new_rf(wavefile,bram_no,ch_no,String_concat(get_tok_str (tok,&tokc),
              0)));
	advance ();
	}
	else
	{
		error (String_concat
             ("expects: wave tag {GRAD|TX}"
              "but got: ",
              get_tok_str (tok,&tokc),
              0),
             current->region);
	}

   	}

}

// list record 结构体 或者 数组类型 的名称以及详细信息
static LinkedList_t Parse_typeDefinitionList ()
{
  Token_t s;
  AstId_t id, id2;
  Ast_Type_t type;
  LinkedList_t list = LinkedList_new ();
  LinkedList_t structBody;

  while (current->kind == TOKEN_TYPENAME){
    advance ();
    s = eatToken (TOKEN_ID);
    printf("insert hc[%d] %s\n",s->kind,get_tok_str(s->kind,NULL));
    Hash_insert (tyTable, String_new(get_tok_str(s->kind,NULL)),
                 (Poly_t)1,
                 (tyDup)error_dupTypeName);
    id = convertToken (s);
    eatToken (TOKEN_ASSIGN);
    switch (current->kind){
    case TOKEN_LBRACE:
      advance ();
      structBody = Parse_structBody ();
      eatToken (TOKEN_RBRACE);
      type = Ast_Type_new_struct (structBody);
      break;
    case TOK_INT:
    case TOK_DOUBLE:{
      Token_t t;
      t = eatToken (TOKEN_TYPE);
      id2 = convertToken (t);
      eatToken (TOKEN_LBRACK);
      eatToken (TOKEN_RBRACK);
      type = Ast_Type_new_array (id2);
      break;
    }
    default:
      error (String_concat
             ("expects: identifier or {\n"
              "but got: ",
              get_tok_str (tok,&tokc),
              0),
             current->region);

    }
    eatToken (TOKEN_SEMI);
    LinkedList_insertLast (list, Ast_Td_new (id, type));
  }
  return list;
}

static Ast_Prog_t Parse_parseTraced (String_t file1)
{

  int i;
  LinkedList_t types, funcs;

  //write in main in face is function
  labelforboards=LinkedList_new ();
  wave_file_list=LinkedList_new();
  ch=file->buf_ptr[0];
  tok_flags=TOK_FLAG_BOL|TOK_FLAG_BOF;
  parse_flags=PARSE_FLAG_PREPROCESS|PARSE_FLAG_TOK_NUM;
  Id_init ();
  advance ();
  Parse_waveList();
  tyTable = Hash_new ((tyHashCode)String_hashCode
                      , (Poly_tyEquals)String_equals);
  Hash_insert (tyTable, "int", (Poly_t)1,
               (Poly_tyVoid)error_dupTypeName);
  Hash_insert (tyTable, "string", (Poly_t)1,
               (Poly_tyVoid)error_dupTypeName);
  // add type void
  Hash_insert (tyTable, "void", (Poly_t)2,
               (Poly_tyVoid)error_dupTypeName);
  Hash_insert (tyTable, "float", (Poly_t)1,
               (Poly_tyVoid)error_dupTypeName);
  Hash_insert (tyTable, "double", (Poly_t)3,
               (Poly_tyVoid)error_dupTypeName);
  types = Parse_typeDefinitionList ();
  funcs = Parse_functionDefinitionList ();
  return Ast_Prog_new (types,globDecs, funcs);
}

static void Trace_arg (String_t file)
{
  printf ("%s", file);
  return;
}

static void Trace_result (Ast_Prog_t p)
{
  Ast_Prog_print (p, stdout);
  return;
}

Ast_Prog_t Parse_parse (String_t file)
{
  Ast_Prog_t r;
  Trace_TRACE("Parse_parse",
              Parse_parseTraced,
              (file),
              Trace_arg,
              r,
              Trace_result);
  return r;
}
