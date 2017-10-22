#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "mem.h"
#include "assert.h"
#include "string.h"
#include "file.h"

#define T String_t

T String_new (char *s)
{
  char *temp;
  int size = strlen(s) +1;
  Mem_NEW_SIZE(temp, size);
  return strcpy(temp, s);
}
//not include end
T String_new_end (char *s,char *end)
{
  char *temp;
  int size = (end-s)/sizeof(char) +1;
  Mem_NEW_SIZE(temp, size);
  strncpy(temp, s,size-1);
  temp[size-1]='\0';
  return temp;
}

T genFileName (T f, T a)
{
  return String_concat (f, ".", a, 0);
}

T String_concat (char *s, ...)
{
  int totalSize = 0;
  char *current = s;
  char *temp, *head;
	
  va_list ap;
  va_start(ap, s);
  while(current) {
    totalSize += strlen (current);
    current = va_arg (ap, char *);
  }
  va_end(ap);


  Mem_NEW_SIZE(temp, (totalSize+1));
  head = temp;
  current = s;
  va_start(ap, s);
  while(current) {
    strcpy (temp, current);
    temp += strlen(current);
    current = va_arg (ap, char *);
  }
  return head;
}

T String_fromArray (int size, char *array[], char *sep)
{
  int i = 0;
  int totalSize = 0;
  char *temp, *head;

  while (i<size) {
    totalSize += strlen (array[i]);
    i++;
  }

  Mem_NEW_SIZE (temp, (totalSize + 1 + size * strlen(sep)));
  head = temp;
  i = 0;
  while (i<size) {
    strcpy (temp, array[i]);
    temp += strlen(array[i]);
    strcpy (temp, sep);
    temp += strlen (sep);
    i++;
  }
  return head;
}

int String_equals (T x, T y)
{
  return (0==strcmp(x, y));
}
T  String_substr(T x,char sep)
{
    int size;
    char *p=strrchr(x,sep),*substr;
    if(p)
    	{
	size=(p-x)/sizeof(char);
	Mem_NEW_SIZE(substr,size+1);
	strncpy(substr,x,size);
	substr[size]='\0';
	return substr;
    	}
	//do nothing
	return x;

}
T  FilePathStrp(T x)
{
    int size;
    char *p=strrchr(x,'\\'),*substr;
    if(p)
    	{
	Mem_NEW_SIZE(substr,strlen(x));
	strcpy(substr,p+1);
	//substr[size]='\0';
	return substr;
    	}
	//do nothing
	return x;

}

String_t String_toString (T x)
{
  return x;
}

int String_size (T x)
{
  return strlen (x);
}

/* This function implements the algorithm on p.57
 * of "The Practice of Programming".
 */
#define MULTIPLIER 31
int String_hashCode (T x)
{
  int h = 0;

  Assert_ASSERT(x);
  while (*x) {
    h = h*MULTIPLIER + (unsigned)*x++;
  }
  return h;
}


void String_print (T x)
{
  Assert_ASSERT(x);
  printf ("%s", x);
}

#undef BUF_SIZE

#undef T

