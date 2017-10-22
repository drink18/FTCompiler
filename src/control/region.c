#include "../lib/assert.h"
#include "../lib/mem.h"
#include "region.h"

#define T Region_t


T Region_new (Id_t filename,int line)
{
  T t;
  Mem_NEW (t);
  t->filename=filename;
  t->line=line;
  return t;
}

T Region_bogus ()
{
  T t;
  Mem_NEW (t);
  t->line=-1;
  return t;
}

#undef T

