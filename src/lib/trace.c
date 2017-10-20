#include <string.h>
#include <stdio.h>
#include "../lib/string.h"
#include "../lib/linkedList.h"
#include "trace.h"

#define STEP 3

static int indent = 0;

static LinkedList_t names = 0;

void Trace_indent ()
{
  indent += STEP;
}

void Trace_unindent ()
{
  indent -= STEP;
}

void Trace_spaces ()
{
  int i = indent;
  while (i--)
    printf (" ");
}

int Trace_lookup (char *s)
{
  if (!names)
    names = LinkedList_new ();
  return LinkedList_exists (names, 
                            s, 
                            (Poly_tyEquals)String_equals);
}

void Trace_insert (char *s)
{
  if (!names)
    names = LinkedList_new ();
  LinkedList_insertFirst(names, s);
}

LinkedList_t Trace_allFuncs ()
{
  if (!names)
    return LinkedList_new ();
  return names;
}

void Trace_reset ()
{
  names = 0;
}

String_t Trace_junk ()
{
  return "<Trace_junk>";
}

String_t Trace_junk2 (void *s)
{
  return "<Trace_junk>";
}
