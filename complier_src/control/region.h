#ifndef REGION_H
#define REGION_H

#include "../lib/string.h"
#include "../atoms/id.h"

#define T Region_t
struct T
{
  Id_t filename;
  int line;
};

typedef struct Region_t *Region_t;

T Region_bogus ();
T Region_new (Id_t filename,int line);

#undef T

#endif
