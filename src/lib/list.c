#include <stdio.h>
#include <stdarg.h>
#include "assert.h"
#include "mem.h"
/*#include "trace.h"*/
#include "todo.h"
#include "list.h"

#define T List_t
#define P Poly_t

T List_new ()
{
  return 0;
}

T List_new2 (P x, T l)
{
  T t;

  Mem_NEW(t);
  t->data = x;
  t->next = l;
  return t;
}

T List_list (P x, ...)
{
  T temp = List_new2 (x, 0);
  P current;
  va_list ap;

  va_start(ap, x);
  current = va_arg(ap, P);
  while (current) {
    temp = List_new2 (current, temp);
    current = va_arg(ap, P);
  }
  va_end(ap);  
  return List_rev (temp);
}

T List_rev (T l)
{
  T p = List_new ();
  
  while (l){
    p = List_new2 (l->data, p);
    l = l->next;
  }
  return p;
}

int List_isEmpty (T l)
{
  return 0==l;
}

int List_length (T l)
{
  int len = 0;

  while (l){
    l = l->next;
    len++;
  }
  return len;
}

T List_insert (T l, P x, int i)
{
  TODO;
}

T List_insertLast (T l, P x)
{
  TODO;
}

P List_delete (T l, int i)
{
  TODO;
}

P List_lookup (T l, int i)
{
  TODO;
}

void List_foreach (T l, void (*f)(P))
{
  while (l){
    f (l->data);
    l = l->next;
  }
  return;
}

P List_exists (T l, P d, int (*f)(P, P))
{
  while (l){
    if (f(d, l->data))
      return l->data;
    l = l->next;
  }
  return 0;
}

T List_map (T l, P (*f) (P))
{
  List_t p = List_new ();
  while (l){
    p = List_new2 (f(l->data), p);
    l = l->next;
  }
  return List_rev (p);
}

static char *List_toString2 (T l, String_t(*f)(P))
{
  switch ((int)l) {
  case 0:
    return "";
  default:
    return String_concat (f(l->data), ", ", List_toString2(l->next, f), 0);
  }
}

char *List_toString (T l, char *(*f)(P))
{
  return String_concat ("[", List_toString2 (l, f), "]", 0);
}

T List_push (P d, T l)
{
  return List_new2 (d, l);
}

T List_pop (T l)
{
  Assert_ASSERT (l);
  return l->next;
}

P List_top (T l)
{
  Assert_ASSERT (l);
  return l->data;
}

#undef P
#undef T
