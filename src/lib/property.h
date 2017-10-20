#ifndef PROPERTY_H
#define PROPERTY_H

#include "poly.h"
#include "string.h"
#include "property-list.h"

#define T Property_t
#define V Poly_t
#define P PropertyList_t

typedef struct T *T;

T Property_new (P(*)(V));
void Property_set (T prop, V k, V v);
V Property_get (T, V k);
void Property_clear (T prop);
String_t Property_status ();

#undef T
#undef V
#undef P

#endif
