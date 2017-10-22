#include "../lib/string.h"
#include "../lib/assert.h"
#include "../lib/mem.h"
#include "../lib/trace.h"
#include "../lib/tuple.h"
#include "../lib/poly.h"
#include "../lib/int.h"
#include "control.h"


typedef struct Flag_t *Flag_t;
struct Flag_t
{
  String_t name;
  /* <String_t, String_t> */
  Tuple_t (*toString)();
  void (*reset)();
};

Flag_t Flag_new (String_t name, 
                 Tuple_t (*toString)(), 
                 void (*reset)())
{
  Flag_t f;

  Mem_NEW (f);
  f->name = name;
  f->toString = toString;
  f->reset = reset;
  return f;
}

static struct LinkedList_t allFlagsHead = {0, 0};
static LinkedList_t allFlags = &allFlagsHead;

#define Flag_add(name, f)                                      \
  LinkedList_insertLast(allFlags,                              \
                        Flag_new (name,                        \
                                  f##ToString,                 \
                                  f##Reset));





/* assert */
Tuple_t Control_assertToString()
{
  if (Assert_flag)
    return Tuple_new ("true", "false");
  return Tuple_new ("false", "false");
}

void Control_assertReset ()
{
  Assert_flag = 0;
}


/* drop pass */
static struct LinkedList_t head = {0, 0};
static LinkedList_t Control_dropPass = &head;
static LinkedList_t Control_dropPassDefault = 0;
static Tuple_t Control_dropPassToString ()
{
  return Tuple_new (LinkedList_toString 
                    (Control_dropPass,
                     ", ",
                     (Poly_tyToString)String_toString),
                    "[]");
}

static void Control_dropPassReset ()
{
  Control_dropPass = Control_dropPassDefault;
}

int Control_mayDropPass (String_t name)
{
  return LinkedList_exists (Control_dropPass,
                            name,
                            (Poly_tyEquals)String_equals);
}

void Control_dropPass_insert (String_t name)
{
  LinkedList_insertLast (Control_dropPass, name);
}

/* dump */
static LinkedList_t Control_dump = 0;
static LinkedList_t Control_dumpDefault = 0;
static String_t Control_dumpFlagToString (Dump_t d)
{
  switch (d){
  case DUMP_AST:
    return "ast";
  case DUMP_HIL:
    return "hil";
  case DUMP_TAC:
    return "tac";
  case DUMP_MACHINE:
    return "machine";
  case DUMP_X86:
    return "x86";
  default:
    Error_bug ("impossible");
    return "<bogus>";
  }
  Error_impossible ();
  return "<bogus>";
}
String_t Control_dumpToString2 ()
{
  if (!Control_dump)
    return "[]";
  return LinkedList_toString
    (Control_dump,
     ", ",
     (Poly_tyToString)Control_dumpFlagToString);
}

Tuple_t Control_dumpToString ()
{
  return Tuple_new (Control_dumpToString2 (), "[]");
}

void Control_dumpReset ()
{
  Control_dump = Control_dumpDefault;
}

void Control_dump_insert (Dump_t il)
{
  if (!Control_dump)
    Control_dump = LinkedList_new ();
  LinkedList_insertLast (Control_dump, (Poly_t)il);
  return;
}

int Control_dump_lookup (Dump_t il)
{
  LinkedList_t p;
  
  if (!Control_dump)
    return 0;
  p = LinkedList_getFirst (Control_dump);
  while (p){
    if ((Dump_t)p->data == il)
      return 1;
    p = p->next;
  }
  return 0;
}

Expert_t Control_expert = EXPERT_NORMAL;
Expert_t Control_expertDefault = EXPERT_NORMAL;
String_t Control_expertToString2 ()
{
  switch (Control_expert){
  case EXPERT_NORMAL:
    return "false";
  case EXPERT_EXPERT:
    return "true";
  case 2:
    return "bug";
  default:
    Error_bug ("impossible");
    return "<bogus>";
  }
}

Tuple_t Control_expertToString ()
{
  return Tuple_new (Control_expertToString2 (), "false");
}

void Control_expertReset ()
{
  Control_expert = Control_expertDefault;
}

/* o */
String_t Control_o = 0;
static String_t Control_oDefault = 0;
static Tuple_t Control_oToString()
{
  return Tuple_new ((Control_o)? Control_o: "\"\"", 
                    "\"\"");
}
static void Control_oReset ()
{
  Control_o = Control_oDefault;
}

/* show type */
int Control_showType = 0;
static int Control_showTypeDefault = 0;
static Tuple_t Control_showTypeToString()
{
  return Tuple_new (Control_showType? "true": "false",
                    "false");
}
static void Control_showTypeReset ()
{
  Control_showType = Control_showTypeDefault;
}

/* trace */
LinkedList_t Control_trace = 0;
LinkedList_t Control_traceDefault = 0;
String_t Control_traceToString2()
{
  return LinkedList_toString
    (Trace_allFuncs (), 
     ", ",
     (Poly_tyToString)String_toString);
}
Tuple_t Control_traceToString ()
{
  return Tuple_new (Control_traceToString2(), "[]");
}
void Control_traceReset ()
{
  Trace_reset ();
}


Verbose_t Control_verbose = VERBOSE_SILENT;
Verbose_t Control_verboseDefault = VERBOSE_SILENT;
String_t Control_verboseToString2()
{
  switch (Control_verbose){
  case VERBOSE_SILENT:
    return "0";
  case VERBOSE_PASS:
    return "1";
  case VERBOSE_SUBPASS:
    return "2";
  case VERBOSE_DETAIL:
    return "3";
  default:
    Error_bug ("impossible");
    return "<bogus>";
  }
}
Tuple_t Control_verboseToString()
{
  return Tuple_new (Control_verboseToString2(), "0");
}
void Control_verboseReset ()
{
  Control_verbose = Control_verboseDefault;
}

int Control_order (int l1, int l2)
{
  return l1<=l2;
}

void Control_init ()
{
  Flag_add("assert flag: ", Control_assert);
  Flag_add("drop pass flag: ", Control_dropPass);
  Flag_add("dump flag: ", Control_dump);
  Flag_add("expert flag: ", Control_expert);
  Flag_add("output name flag: ", Control_o);
  Flag_add("show type flag: ", Control_showType);
  Flag_add("trace flag: ", Control_trace);
  Flag_add("verbose flag: ", Control_verbose);
  return;
}

void Control_printFlags ()
{
  LinkedList_t p = LinkedList_getFirst (allFlags);
  Flag_t f;
  Tuple_t tuple;

  printf ("Flag setttings:\n");
  while (p){
    f = (Flag_t)p->data;
    printf ("   %s\n", f->name);
    tuple = f->toString ();
    printf ("      current: %s\n", 
            (String_t)Tuple_first(tuple));
    printf ("      default: %s\n", 
            (String_t)Tuple_second (tuple));
    p = p->next;
  }
  return;
}
