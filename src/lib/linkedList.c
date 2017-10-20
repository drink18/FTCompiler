#include <stdio.h>
#include <stdarg.h>
#include "assert.h"
#include "error.h"
#include "mem.h"
#include "tuple.h"
#include "linkedList.h"


#define T LinkedList_t
#define P Poly_t

T LinkedList_new ()
{
  T l;

  Mem_NEW (l);
  l->data = 0;
  l->next = 0;
  return l;
}

static T LinkedList_new2 (P x, T l)
{
  T p;
  Mem_NEW (p);
  p->data = x;
  p->next = l;
  return p;
}

int LinkedList_isEmpty (T l)
{
  Assert_ASSERT(l);
  return (0==l->next);
}

int LinkedList_size (T l)
{
  T p;
  int i = 0;
 
  Assert_ASSERT(l);
  p = l->next;
  while (p){
    i++;
    p = p->next;
  }
  return i;
}

void LinkedList_append (T l1, T l2)
{
  LinkedList_t p;
  Assert_ASSERT(l1);
  Assert_ASSERT(l2);
  p = LinkedList_getFirst (l2);
  while (p){
    LinkedList_insertLast (l1, p->data);
    p = p->next;
  }
  return;
}

void LinkedList_appendNode (T l1, T l2)
{
  LinkedList_t tail;

  Assert_ASSERT(l1);
  Assert_ASSERT(l2);
  
  if (l1->next==0){
    l1->next = l2;
    l1->data = l2;
    l2->next = 0;
    return;
  }
  tail = (T)l1->data;
  tail->next = l2;
  l1->data = l2;
  l2->next = 0;  
  return;
}


/* we abuse the head node's "data" field to store 
 * a tail pointer 
 */
void LinkedList_insertFirst (T l, P x)
{
  T t;

  Assert_ASSERT(l);
  t = LinkedList_new2 (x, l->next);
  l->next = t;
  if (l->data == 0)
    l->data = t;
  return;
}

void LinkedList_insertLast (T l, P x)
{
  T tail, p;

  Assert_ASSERT(l);
  //dont't insert null
  if(!x) return ;
  if (l->next == 0){
    LinkedList_insertFirst (l, x);
    return;
  }
  tail = (T)l->data;
  p = LinkedList_new2 (x, 0);
  tail->next = p;
  l->data = p;
  return;
}


T LinkedList_list (P x, ...)
{
  T list = LinkedList_new ();
  va_list ap;
  P current;

  LinkedList_insertLast (list, x);
  va_start(ap, x);
  current = va_arg(ap, P);
  while (current) {
    LinkedList_insertLast (list, current);
    current = va_arg(ap, P);
  }
  va_end(ap);  
  return list;
}

T LinkedList_rev (T l)
{
  LinkedList_t r, p;

  Assert_ASSERT(l);
  p = l->next;
  r = LinkedList_new ();
  while (p){
    LinkedList_insertFirst (r, p->data);
    p = p->next;
  }
  return r;
}

P LinkedList_nth (T l, int n)
{
  T p = l->next;
  if (n<0){
    Error_bug ("invalid argument");
    return 0;
  }
  while (p){
    if (n==0)
      return p->data;
    n--;
    p = p->next;
  }
  return 0;
}

void LinkedList_foreach (T l, void (*f)(P))
{
  T p;

  Assert_ASSERT(l);
  Assert_ASSERT(f);
  p = l->next;
  while (p){
    f (p->data);
    p = p->next;
  }
  return;
}

int LinkedList_exists (T l, P x, Poly_tyEquals f)
{
  T p;

  Assert_ASSERT(l);
  Assert_ASSERT(f);
  p = l->next;
  while (p){
    if (f (x, p->data)){
      return 1;
    }
    p = p->next;
  }
  return 0;
}

int LinkedList_exists2 (T l, P x, Poly_tyEquals f, Poly_tyVoid g)
{
  T p;

  Assert_ASSERT(l);
  Assert_ASSERT(f);
  Assert_ASSERT(g);
  p = l->next;
  while (p){
    if (f (x, p->data)){
      g (p->data);
      return 1;
    }
    p = p->next;
  }
  return 0;
}

T LinkedList_map (T l, Poly_tyId f)
{
  T new, tmp;
  Assert_ASSERT(l);
  Assert_ASSERT(f);
  new = LinkedList_new ();
  tmp = l->next;
  while (tmp){
    LinkedList_insertLast (new, f(tmp->data));
    tmp = tmp->next;
  }
  return new;
}

T LinkedList_getFirst (T l)
{
  Assert_ASSERT(l);
  return l->next;
}
//获取最后一个节点
P LinkedList_getLast (T l)
{
  T t,tail;

  Assert_ASSERT(l);
 // t = LinkedList_new2 (x, l->next);
 // l->next = t;
  if (l->data == 0)
     return 0;
  tail=(T)l->data;
  return tail->data;
}

T LinkedList_filter (T l, Poly_tyPred f)
{
  LinkedList_t tmp, p;
  Assert_ASSERT(l);
  Assert_ASSERT(f);
  tmp = LinkedList_new ();
  p = LinkedList_getFirst (l);
  while (p){
    if (f(p->data))
      LinkedList_insertLast (tmp, p->data);
    p = p->next;
  }
  return tmp;
}

String_t LinkedList_toStringWithLastSep (T l,
                                         String_t sep,
                                         String_t (*f)(P))
{
  T p = LinkedList_getFirst (l);
  String_t s = "[";

  while (p){
    /* this would be rather slow, a more 
     * efficient one should be used here.
     * string buffer? or append string?
     */
    s = String_concat (s, f(p->data), sep, 0);
    p = p->next;
  }
  s = String_concat (s, "]", 0);
  return s;
}

String_t LinkedList_toString (T l,
                              String_t sep,
                              String_t (*f)(P))
{
  T p = LinkedList_getFirst (l);
  String_t s = "[";

  while (p){
    /* this would be rather slow, a more 
     * efficient one should be used here.
     * string buffer? or append string?
     */
    s = (p->next)? (String_concat (s, f(p->data), sep, 0))
      : (String_concat (s, f(p->data), 0));
    p = p->next;
  }
  s = String_concat (s, "]", 0);
  return s;
}


#undef P
#undef T
