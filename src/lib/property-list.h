#ifndef PROPERTY_LIST_H
#define PROPERTY_LIST_H

#include "linkedList.h"

#define T PropertyList_t

typedef LinkedList_t T;
typedef PropertyList_t (*Poly_tyPlist)(void *);

T PropertyList_new ();
/* int PropertyList_equals (T, T); */

#undef T

#endif
