#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "string.h"
#include "poly.h"

#define T LinkedList_t
#define P Poly_t

typedef struct T *T;
struct T
{
  Poly_t data;
  LinkedList_t next;
};

T LinkedList_new ();
int LinkedList_isEmpty (T l);
int LinkedList_size (T l);
void LinkedList_insertFirst (T l, P x);
void LinkedList_insertLast (T l, P x);
void LinkedList_foreach (T l, void (*f) (P));
void LinkedList_append (T l1, T l2);
void LinkedList_appendNode (T l1, T l2);
T LinkedList_map (T l, Poly_tyId f);
T LinkedList_list (P x, ...);
P LinkedList_nth (T l, int n);
T LinkedList_rev (T l);
P LinkedList_getLast (T l);

int LinkedList_exists (T l, P x, Poly_tyEquals f);
/* find the first element y, which satisfied f(x, y), and 
 * apply g(y) before return.
 */
int LinkedList_exists2 (T l, P x, Poly_tyEquals f, Poly_tyVoid g);
T LinkedList_filter (T l, Poly_tyPred f);
T LinkedList_getFirst (T l);
String_t LinkedList_toString (T list, 
                              String_t sep, 
                              String_t (*f)(P));
String_t LinkedList_toStringNoLastSep (T list, 
                                       String_t sep, 
                                       String_t (*f)(P));

#undef P
#undef T

#endif
