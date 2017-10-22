#include <stdio.h>

#include "gc.h"
#include "mem.h"

int Mem_allocated = 0;
int Mem_initFlag = 0;

/* the accumulated time should include GC time */
static clock_t totalClocks = 0;

void Mem_init ()
{
  GC_INIT ();
  totalClocks = 0;
}

void *Mem_alloc (unsigned int size)
{
  void *p;
  clock_t start, finish;
  start = clock ();
  p = GC_MALLOC(size);
  if (0==p)
    Error_error("allocation failed\n");
  Mem_allocated += size;
  finish = clock ();
  totalClocks += (finish-start);
  return p;
}
void *Mem_realloc (void *ptr,unsigned int size)
{
  void *p;
  clock_t start, finish;
  start = clock ();
  p = GC_REALLOC(ptr,size);
  if (0==p)
    Error_error("reallocation failed\n");
  Mem_allocated += size;
  finish = clock ();
  totalClocks += (finish-start);
  return p;
}

clock_t Mem_getClock ()
{
  return totalClocks;
}

#define ONEM (1024*1024)
void Mem_status (FILE *fp)
{
  unsigned long int total, sinceLast;
  
  total = GC_get_total_bytes();
  sinceLast = GC_get_bytes_since_gc();
  fprintf (fp,"Heap status:\n"
          "  Total allocation: %lu bytes (~%ldM)\n"
          "  Allocation since last GC: %ld bytes (~%ldM)\n", 
          total, total/ONEM,
          sinceLast, sinceLast/ONEM);
          
  Mem_allocated = 0;
  return;
}


