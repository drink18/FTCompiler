#include "assert.h"
#include "mem.h"
#include "property.h"
#include "linkedList.h"
#include "poly.h"
#include "int.h"
#include "double.h"
#include "tuple.h"

#define T Property_t

struct T
{
  int i;
  LinkedList_t plists;
  PropertyList_t (*getPlist)(Poly_t);
};

static int counter = 0;
static int numGets = 0;
static int numLinks = 0;

T Property_new (PropertyList_t (*get)(Poly_t))
{
  T t;

  Mem_NEW (t);
  t->i = counter++;
  t->plists = LinkedList_new ();
  t->getPlist = get; 
  return t;
}

void Property_set (T prop, Poly_t k, Poly_t v)
{
  Tuple_t tuple;
  PropertyList_t plist;

  Assert_ASSERT(prop);
  Assert_ASSERT(k);
  plist = prop->getPlist (k);
  Assert_ASSERT(plist);
  tuple = Tuple_new ((Poly_t)prop, v);
  LinkedList_insertFirst (plist, tuple);
  LinkedList_insertFirst (prop->plists, plist);
  return;
}

Poly_t Property_get (T prop, Poly_t k)
{
  Tuple_t t;
  PropertyList_t plist;
  LinkedList_t p;

  Assert_ASSERT(prop);
  Assert_ASSERT(k);
  plist = prop->getPlist (k);
  p = LinkedList_getFirst (plist);
  numGets++;
  while (p){
    numLinks++;
    t = (Tuple_t)p->data;
    if (Tuple_first(t) == prop)
      return Tuple_second (t);
    p = p->next;
  }
  return 0;
}

void Property_clear (T prop)
{
  LinkedList_t plists;
  PropertyList_t prev;
  Tuple_t tuple;
  
  Assert_ASSERT(prop);
  plists = LinkedList_getFirst (prop->plists);
  while (plists){
    prev = (PropertyList_t)plists->data;
    while (prev->next){
      tuple = (Tuple_t)prev->next->data;
      if (Tuple_first (tuple)==prop){
        prev->next = prev->next->next;
        continue;
      }
      prev = prev->next;
    }
    plists = plists->next;
  }  
}

String_t Property_status ()
{
  String_t average;
  if (numGets)
    average = Double_toString (numLinks*1.0/numGets);
  else average = "0.0";
  return String_concat 
    ("plist peeks: ", 
     Int_toString (numGets),
     ", average position: ",
     average,
     0);
}

#undef T
