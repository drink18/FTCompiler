#include "control.h"
#include "verbose.h"
#include "pass.h"

Pass_t Pass_new (String_t name, Verbose_t level, Poly_t
                 thunk, Poly_t (*a)(Poly_t))
{
  Pass_t x = {name, level, thunk, a};
  return x;
}

Poly_t Pass_doit (Pass_t *p)
{
  int r;

  if (Control_mayDropPass (p->name))
    return p->thunk;
  Verbose_TRACE (p->name, p->action,
                 (p->thunk), r, p->level);
  return (Poly_t)r;
}
