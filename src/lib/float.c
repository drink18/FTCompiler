#include <stdio.h>
#include "string.h"
#include "mem.h"
#include "float.h"


#define BUF_SIZE 1024

String_t float_toString (float i)
{
  char *temp;
  Mem_NEW_SIZE(temp, BUF_SIZE);
  /* Note that I initially want to write:
   *   snprintf (temp, BUF_SIZE, "%d", i);
   * but it seems that this version of GCC I'm using has a bug on
   * the function "snprintf". So I've to write this unsafe "sprintf".
   */
  
  sprintf (temp, "0x%x",*(int *)&i);
  return temp;
}
String_t double_toString (double i)
{
  char *temp;
  Mem_NEW_SIZE(temp, BUF_SIZE);
  /* Note that I initially want to write:
   *   snprintf (temp, BUF_SIZE, "%d", i);
   * but it seems that this version of GCC I'm using has a bug on
   * the function "snprintf". So I've to write this unsafe "sprintf".
   */
  
  sprintf (temp, "0x%llx",*(long long int  *)&i);
  return temp;
}

String_t float_toString2 (float i)
{
  char *temp;
  Mem_NEW_SIZE(temp, BUF_SIZE);
  /* Note that I initially want to write:
   *   snprintf (temp, BUF_SIZE, "%d", i);
   * but it seems that this version of GCC I'm using has a bug on
   * the function "snprintf". So I've to write this unsafe "sprintf".
   */
  
  sprintf (temp, "%f",i);
  return temp;
}
String_t double_toString2 (double i)
{
  char *temp;
  Mem_NEW_SIZE(temp, BUF_SIZE);
  /* Note that I initially want to write:
   *   snprintf (temp, BUF_SIZE, "%d", i);
   * but it seems that this version of GCC I'm using has a bug on
   * the function "snprintf". So I've to write this unsafe "sprintf".
   */
  
  sprintf (temp, "%lf",i);
  return temp;
}


#undef BUF_SIZE

