#include <stdio.h>
#include <stdlib.h>
#include "../lib/io.h"
#include "../lib/error.h"
#include "../lib/assert.h"
#include "../lib/trace.h"
#include "control.h"
#include "command-line.h"

typedef enum {
  ERROR_INVALIDARG, 
  ERROR_NOARG, 
  ERROR_NOSWITCH
} ErrorKind_t;

static void error (ErrorKind_t ek);
static void Arg_init ();

static int index = 0;
static int CommandLine_argc = 0;
static char **CommandLine_argv = 0;
static char *currentName = 0;
static char *currentArg = 0;
static char *currentStr = 0;
extern TCCState *s;

static void CommandLine_next ()
{
  if (index>=CommandLine_argc)
    error (ERROR_NOARG);
  currentStr = CommandLine_argv[index++];
}

typedef struct Arg_t
{
  Expert_t level;
  String_t name;
  String_t arg;
  String_t desc;
  void (*action)();
} *Arg_t;


/* all the available args */


static void Arg_setAssert ()
{
  CommandLine_next ();
  if (String_equals (currentStr, "false"))
    Assert_flag = 0;
  else if (String_equals (currentStr, "true"))
    Assert_flag = 1;
  else error (ERROR_INVALIDARG);      
  return;
}
static struct Arg_t Arg_assert = 
  {EXPERT_NORMAL,
   "assert", 
   "{false|true}", 
   "enable assertions",
   Arg_setAssert};

static struct Arg_t Arg_defineS = 
  {EXPERT_NORMAL,
   "D", 
   "{sym=val}", 
   "define 'sym' with value 'val'",
   pre_define_symbol};
static struct Arg_t Arg_undefineS = 
  {EXPERT_NORMAL,
   "U", 
   "{sym}", 
   "undefine 'sym'",
   pre_undefine_symbol};

static struct Arg_t Arg_includePathS = 
{
  EXPERT_NORMAL,
  "I",
  "{sym=val}",
  "add include path",
  add_include_path 
};

static void pre_define_symbol()
{
   CommandLine_next ();
   char *sym=tcc_strdup(currentStr);
   char *value=strchr(sym,'=');
   if(value)
   	  *value++='\0';
   tcc_define_symbol(s,sym,value);
   tcc_free(sym);
}
static void pre_undefine_symbol()
{
   CommandLine_next ();
   tcc_undefine_symbol(s,currentStr);
}

static void add_include_path()
{
    tcc_add_include_path(s, currentArg);
}


/* drop pass*/
static void Arg_setDropPass()
{
  CommandLine_next ();
  Control_dropPass_insert (currentStr);
  return;
}
static struct Arg_t Arg_dropPass = 
  {EXPERT_NORMAL,
   "drop-pass", 
   "<pass>", 
   "drop the compiler pass",
   Arg_setDropPass};

/* dump */
static void Arg_setDump()
{
  CommandLine_next ();
  if (String_equals (currentStr, "ast"))
    Control_dump_insert (DUMP_AST);
  else if (String_equals (currentStr, "hil"))
    Control_dump_insert (DUMP_HIL);
  else if (String_equals (currentStr, "tac"))
    Control_dump_insert (DUMP_TAC);
  else if (String_equals (currentStr, "machine"))
    Control_dump_insert (DUMP_MACHINE);
  else if (String_equals (currentStr, "asm"))
    Control_dump_insert (DUMP_X86);
  else if(String_equals(currentStr,"par"))
  	Control_dump_insert(DUMP_PAR);
  else if(String_equals(currentStr,"cpp"))
  	Control_dump_insert(DUMP_CPP);
  else error (ERROR_INVALIDARG);
  return;
}
static struct Arg_t Arg_dump = 
  {EXPERT_NORMAL,
   "dump", 
   #ifdef _DEBUG
   "{cpp|ast|hil|tac|machine|par|asm}",
   #else
   "{cpp|par|asm}",
   #endif
   
   "which intermediate "
   "language to dump",
   Arg_setDump};

static void Arg_setExpert()
{
  CommandLine_next ();
  if (String_equals (currentStr, "false"))
    Control_expert = EXPERT_NORMAL;
  else if (String_equals (currentStr, "true"))
    Control_expert = EXPERT_EXPERT;
  else if (String_equals (currentStr, "bug"))
    Control_expert = 2;
  else error (ERROR_INVALIDARG);
  return;
}

static struct Arg_t Arg_expert = 
  {EXPERT_EXPERT,
   "expert", 
   "{false|true}", 
   "show expert level switches",
   Arg_setExpert};



static void Arg_setO()
{
  CommandLine_next ();
  Control_o = currentStr;
  return;
}

static struct Arg_t Arg_o = 
  {EXPERT_NORMAL,
   "o", 
   "<file>", 
   "set the output file name",
   Arg_setO};


static void Arg_setShowType()
{
  Control_showType = 1;
  return;
}
static struct Arg_t Arg_showType = 
  {EXPERT_EXPERT,
   "showtype", 
   "{false|true}",
   "display type information in ILs", 
   Arg_setShowType};

static void Arg_setTrace()
{
  CommandLine_next ();
  Trace_insert (currentStr);
  return;
}
static struct Arg_t Arg_trace = 
  {EXPERT_EXPERT,
   "trace", 
   "<name>", 
   "to trace a function",
   Arg_setTrace};

static void Arg_setVerbose()
{
  CommandLine_next ();
  if (String_equals (currentStr, "0"))
    Control_verbose = VERBOSE_SILENT;
  else if (String_equals (currentStr, "1"))
    Control_verbose = VERBOSE_PASS;
  else if (String_equals (currentStr, "2"))
    Control_verbose = VERBOSE_SUBPASS;
  else if (String_equals (currentStr, "3"))
    Control_verbose = VERBOSE_DETAIL;
  else error (ERROR_INVALIDARG);
  return;
}
static struct Arg_t Arg_verbose = 
  {EXPERT_NORMAL,
   "verbose", 
   "{0|1|2|3}", 
   "how verbose to be",
   Arg_setVerbose};


static struct LinkedList_t allArgsHead = {0, 0};
static LinkedList_t allArgs = &allArgsHead;

#define cookArg(a)                              \
  LinkedList_insertLast (allArgs, &a)

static void Arg_init ()
{
#ifdef _DEBUG
  cookArg (Arg_assert);
  cookArg (Arg_dropPass);
  cookArg (Arg_verbose);
#endif
  cookArg(Arg_defineS);
  cookArg(Arg_undefineS); 	
  cookArg (Arg_dump);
  cookArg (Arg_expert);
  cookArg (Arg_o);
  cookArg (Arg_showType);
  cookArg (Arg_trace);
  cookArg(Arg_includePathS);
  
  return;
}

#define LEFT_SIZE 35
#define INDENT_SIZE 3

static void Arg_printOne (Arg_t arg)
{
  int left;

  if (!Control_order (arg->level, Control_expert))
    return;
  left = INDENT_SIZE + 1+ String_size (arg->name)
    + 1 + String_size (arg->arg)+1;
  Io_printSpaces (INDENT_SIZE);
  printf ("-%s", arg->name);
  printf (" ");
  printf ("%s", arg->arg);
  if (left>LEFT_SIZE) {
    printf ("\n");
    Io_printSpaces (LEFT_SIZE);
  }
  else Io_printSpaces (LEFT_SIZE-left);
  printf (" %s\n", arg->desc);
  return;
}

static void Arg_print ()
{
  printf ("available switches are:\n");
  LinkedList_foreach (allArgs, (Poly_tyVoid)Arg_printOne);
  return;
}

static void error (ErrorKind_t ek)
{
  switch (ek){
  case ERROR_NOSWITCH:
    printf ("unknown switch: %s\n", currentName);
    Arg_print ();
    break;
  case ERROR_NOARG:
    printf ("no argument is given to switch: %s\n"
            "expects arg: %s\n", currentName, currentArg);
    break;
  case ERROR_INVALIDARG:
    printf ("invalid arg for switch: %s\n"
            "expects: %s\n"
            "but got: %s\n", currentName, currentArg, currentStr);
    break;
  default:
    Error_bug ("impossible");
    break;
  }
  exit (1);
}

static int Arg_searchAndSet (char *arg)
{
  LinkedList_t args = LinkedList_getFirst (allArgs);
  Arg_t a;
  char inputArg[2];
  inputArg[0] = arg[0];
  inputArg[1] = 0;

  while (args){
    a = (Arg_t)args->data;
    if (String_equals (a->name, inputArg)){
      // currentArg = a->arg;
      currentArg = arg + 1;
      a->action ();
      return 1;
    }
    args = args->next;
  }
  return 0;
}

LinkedList_t CommandLine_doarg (int argc, char **argv)
{
  char *inputName;
  LinkedList_t files = LinkedList_new ();

  Arg_init ();
  Control_init ();
  CommandLine_argc = argc;
  CommandLine_argv = argv;
  for (index=0; index<argc; ){
    inputName = argv[index++];
    if (inputName[0] == '-'){
      currentName = inputName;
      if (Arg_searchAndSet (inputName+1))
        ;
      else {
        error (ERROR_NOSWITCH);
      }
    }
    else LinkedList_insertLast (files, inputName);
  }
  if (Control_order (VERBOSE_SUBPASS, Control_verbose))
    Control_printFlags ();
  return files;
}
