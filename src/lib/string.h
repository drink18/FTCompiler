#ifndef STRING_H
#define STRING_H

#define T String_t

typedef char *T;

int String_equals (T x, T y);
T String_new (char *s);
T String_concat (char *s, ...);
/*void String_append (T, T);*/
T String_fromArray (int size, char *array[], char *sep);
int String_size (T s);
T String_toString (T x);
int String_hashCode (T x);
void String_print (T x);
T  String_substr(T x,char sep);
T genFileName (T f, T a);

#undef T

#endif
