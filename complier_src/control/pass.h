#ifndef PASS_H
#define PASS_H

#include "../lib/string.h"
#include "../lib/poly.h"
#include "control.h"

typedef struct
{
  String_t name;
  Verbose_t level;
  Poly_t thunk;
  Poly_t (*action)(Poly_t);
} Pass_t;

Pass_t Pass_new (String_t, Verbose_t, Poly_t, Poly_t (*)(Poly_t));
Poly_t Pass_doit (Pass_t *pass);

#endif
