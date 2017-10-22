#include <stdio.h>
#include "string.h"
#include "mem.h"
#include "int.h"

const int Int_zero = 0;

#define BUF_SIZE 1024

String_t Int_toString (int i)
{
  char *temp;
  Mem_NEW_SIZE(temp, BUF_SIZE);
  /* Note that I initially want to write:
   *   snprintf (temp, BUF_SIZE, "%d", i);
   * but it seems that this version of GCC I'm using has a bug on
   * the function "snprintf". So I've to write this unsafe "sprintf".
   */
  sprintf (temp, "%d", i);
  return temp;
}

String_t longInt_toString (long long int i)
{
  char *temp;
  Mem_NEW_SIZE(temp, BUF_SIZE);
  /* Note that I initially want to write:
   *   snprintf (temp, BUF_SIZE, "%d", i);
   * but it seems that this version of GCC I'm using has a bug on
   * the function "snprintf". So I've to write this unsafe "sprintf".
   */
  sprintf (temp, "0x%llx", i);
  return temp;
}

//I want 4,for comptible with old NMR software so rubbish
String_t Int_toString4x (int i)
{
  char *temp;
  Mem_NEW_SIZE(temp, BUF_SIZE);
  /* Note that I initially want to write:
   *   snprintf (temp, BUF_SIZE, "%d", i);
   * but it seems that this version of GCC I'm using has a bug on
   * the function "snprintf". So I've to write this unsafe "sprintf".
   */
  sprintf (temp, "%08x", i&0xFFFF);
  return temp;
}

#undef BUF_SIZE

