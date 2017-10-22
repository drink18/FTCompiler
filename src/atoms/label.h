#ifndef LABEL_H
#define LABEL_H

#include "../lib/string.h"
#include "../lib/property-list.h"

#define T Label_t

typedef struct T *T;

T Label_new ();
T Label_new2 (String_t name);

int Label_hashCode (T x);
String_t Label_toString (T x);
int Label_equals (T, T);
PropertyList_t Label_plist (T);

#undef T

#endif
