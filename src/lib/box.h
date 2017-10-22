#ifndef BOX_H
#define BOX_H

#include "poly.h"
#include "string.h"
#include "file.h"
#include "linkedList.h"


#define T Box_t

typedef struct T *T;

T Box_str (String_t str);
T Box_h (T, ...);
T Box_hlist (LinkedList_t);
T Box_v (T, ...);
T Box_vlist (LinkedList_t);
T Box_indent (T, int);
void Box_print (T, void (*)(String_t));
void Box_output (T, File_t);

#undef T

#endif
