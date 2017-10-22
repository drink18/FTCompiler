#ifndef TYPE_H
#define TYPE_H

#include "../lib/linkedList.h"
#include "../lib/string.h"
#include "../lib/tuple.h"
#include "../ast/ast-id.h"

#define Id_t AstId_t
#define T Type_t
#define S Type_Struct_t

typedef struct S *S;
struct S
{
  Id_t type;
  Id_t name;
};
S Type_Struct_new (Id_t type, Id_t name);

typedef struct T *T;
struct T
{
  enum {
    TYPE_A_INT, 
	TYPE_A_FLOAT,
	TYPE_A_DOUBLE,
    TYPE_A_STRING,
    TYPE_A_VOID,
    TYPE_A_NS,
    TYPE_A_STRUCT, 
    TYPE_A_ARRAY,
    TYPE_A_PRODUCT,
    TYPE_A_FUN
  }kind;
  union{
    struct{
      Id_t name;
      /* List<S> */
      LinkedList_t fields;
    }structt;
    struct{
      Id_t name;
      Id_t eltType;
    }array;
    /* List<T> */
    LinkedList_t product;
    struct{
      T from;
      T to;
    }fun;
  }u;
};

T Type_new_int ();
T Type_new_float ();
T Type_new_double ();

T Type_new_string ();
T Type_new_ns ();
T Type_new_void ();

T Type_new_struct (Id_t name, LinkedList_t fields);
T Type_new_array (Id_t name, Id_t eltType);
T Type_new_product (T t, ...);
T Type_new_product2 (LinkedList_t prod);
T Type_new_fun (T from, T to);
Id_t Type_dest_array (T);
Id_t Type_searchField (T t, Id_t id);
Tuple_t Type_dest_fun (T);
int Type_equals (T, T);
int Type_equals_int (T);
int Type_equals_string (T);
String_t Type_toString (T);

#undef Id_t
#undef S
#undef T

#endif
