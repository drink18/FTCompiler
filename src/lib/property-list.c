#include "property-list.h"

#define T PropertyList_t

T PropertyList_new ()
{
  return LinkedList_new ();
}

int PropertyList_equals (T p1, T p2)
{
  return p1 == p2;
}

#undef T
