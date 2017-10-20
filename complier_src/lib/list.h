#ifndef LIST_H
#define LIST_H

#include "poly.h"
#include "string.h"

#define T List_t
#define P Poly_t

typedef struct T *T;
struct T
{
  P data;
  T next;
};

T List_new ();
T List_new2 (P x, T l);
T List_list (P x, ...);
int List_isEmpty (T l);
int List_length (T l);
T List_insert (T l, P x, int i);
T List_insertLast (T l, P x);
P List_delete (T l, int i);
P List_lookup (T l, int i);
void List_foreach (T l, void (*f) (P));
P List_exists (T l, P d, int (*f)(P, P));
T List_map (T l, P (*f) (P));
T List_getFirst (T l);
T List_rev (T l);
T List_push (P d, T l);
T List_pop (T l);
P List_top (T l);
String_t List_toString (T l, String_t(*f)(P));

#undef P
#undef T

#endif
