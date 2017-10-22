#include <stdarg.h>
#include "../lib/mem.h"
#include "../lib/list-pair.h"
#include "../lib/assert.h"
#include "../lib/error.h"
#include "type.h"

#define Id_t AstId_t
#define T Type_t
#define S Type_Struct_t

S Type_Struct_new (Id_t type, Id_t name)
{
  S t;
  Mem_NEW (t);
  t->type = type;
  t->name = name;
  return t;
}

String_t Type_Struct_toString (S t)
{
  Assert_ASSERT (t);

  return String_concat (AstId_toString (t->type),
                        " ",
                        AstId_toString (t->name),
                        0);
}

static struct T Type_int_internal =
  {TYPE_A_INT};

static struct T Type_string_internal =
  {TYPE_A_STRING};

static struct T Type_ns_internal =
  {TYPE_A_NS};

static struct T Type_float_internal =
  {TYPE_A_FLOAT};

static struct T Type_double_internal =
  {TYPE_A_DOUBLE};


static struct T Type_void_internal =
  {TYPE_A_VOID};


static T Type_int = (T)&Type_int_internal;
static T Type_string = (T)&Type_string_internal;
static T Type_ns = (T)&Type_ns_internal;
static T Type_float = (T)&Type_float_internal;
static T Type_double = (T)&Type_double_internal;
static T Type_void = (T)&Type_void_internal;

T Type_new_int ()
{
  return Type_int;
}
T Type_new_float ()
{
  return Type_float;
}
T Type_new_double ()
{
  return Type_double;
}

T Type_new_string ()
{
  return Type_string;
}

T Type_new_ns ()
{
  return Type_ns;
}
T Type_new_void ()
{
  return Type_void;
}


T Type_new_struct (Id_t name, LinkedList_t list)
{
  T t;
  Mem_NEW (t);
  t->kind = TYPE_A_STRUCT;
  t->u.structt.name = name;
  t->u.structt.fields = list;
  return t;
}

T Type_new_array (Id_t name, Id_t eltType)
{
  T t;
  Mem_NEW (t);
  t->kind = TYPE_A_ARRAY;
  t->u.array.name = name;
  t->u.array.eltType = eltType;
  return t;
}

T Type_new_product (T x, ...)
{
  LinkedList_t list = LinkedList_new ();
  va_list ap;
  T current, t;

  LinkedList_insertLast (list, x);
  va_start (ap, x);
  current = va_arg (ap, T);
  while (current){
    LinkedList_insertLast (list, current);
    current = va_arg (ap, T);
  }
  va_end(ap);
  Mem_NEW (t);
  t->kind = TYPE_A_PRODUCT;
  t->u.product = list;
  return t;
}

T Type_new_product2 (LinkedList_t list)
{
  T t;
  Mem_NEW (t);
  t->kind = TYPE_A_PRODUCT;
  t->u.product = list;
  return t;
}

T Type_new_fun (T from, T to)
{
  T t;
  Mem_NEW (t);
  t->kind = TYPE_A_FUN;
  t->u.fun.from = from;
  t->u.fun.to = to;
  return t;
}

int Type_equals (T t1, T t2)
{
  Assert_ASSERT(t1);
  Assert_ASSERT(t2);

  switch (t1->kind){
  case TYPE_A_INT:
    switch (t2->kind){
    case TYPE_A_INT:
      return 1;  	
    default:
      return 0;
    }
  case TYPE_A_FLOAT:
    switch (t2->kind){
    case TYPE_A_FLOAT:
      return 1;
    default:
      return 0;
    }
  case TYPE_A_DOUBLE:
    switch (t2->kind){
    case TYPE_A_DOUBLE:
      return 1;
    default:
      return 0;
    }	
  case TYPE_A_STRING:
    switch (t2->kind){
    case TYPE_A_STRING:
      return 1;
    default:
      return 0;
    }
  case TYPE_A_NS:
    Error_bug ("impossible");
    return 0;    
  case TYPE_A_STRUCT:
    switch (t2->kind){
    case TYPE_A_NS:
      return 1;
    case TYPE_A_STRUCT:
      if (AstId_equals (t1->u.structt.name, 
                        t2->u.structt.name))
        return 1;
      return 0;
    default:
      return 0;
    }
  case TYPE_A_ARRAY:
    switch (t2->kind){
    case TYPE_A_NS:
      return 1;
    case TYPE_A_ARRAY:
      if (AstId_equals (t1->u.array.name,
                        t2->u.array.name))
        return 1;
      return 0;
    default:
      return 0;
    }
  case TYPE_A_PRODUCT:
    switch (t2->kind){
    case TYPE_A_PRODUCT:{
      ListPair_t pairs = ListPair_new (t1->u.product,
                                       t2->u.product);
      if (!pairs)
        return 0;
      return ListPair_forall 
        (pairs, 
         (int (*)(Poly_t, Poly_t))Type_equals);
    }
    default:
      return 0;
    }
  case TYPE_A_FUN:
    Error_bug ("impossible");
    return 0;
  case TYPE_A_VOID:
  	return 0;
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

int Type_equals_int (T t)
{
  return Type_equals (Type_int, t);
}

int Type_equals_string (T t)
{
  return Type_equals (Type_string, t);
}

int Type_equals_ns (T t)
{
  return Type_equals (Type_ns, t);
}

String_t Type_toString (T t)
{
  Assert_ASSERT(t);
  switch (t->kind){
  case TYPE_A_VOID:
    return "void";
  case TYPE_A_INT:
    return "int";
  case TYPE_A_STRING:
    return "string";
  case TYPE_A_NS:
    return "null";
  case TYPE_A_FLOAT:
    return "float";
  case TYPE_A_DOUBLE:
    return "double";		
  case TYPE_A_STRUCT:
    return String_concat
      ("struct",
       AstId_toString (t->u.structt.name),
       "{",
       LinkedList_toString 
       (t->u.structt.fields, 
        "; ",
        (Poly_tyToString)Type_Struct_toString),
       "}",
       0);
  case TYPE_A_ARRAY:
    return String_concat 
      (AstId_toString (t->u.array.name), 
       "[] of ", 
       AstId_toString (t->u.array.eltType),
       0);
  case TYPE_A_PRODUCT:
    return LinkedList_toString 
      (t->u.product, 
       ", ", 
       (Poly_tyToString)Type_toString);
  case TYPE_A_FUN:
    return (String_concat 
	    (Type_toString (t->u.fun.from),
	     " -> ",
	     Type_toString (t->u.fun.to),
	     0));
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

Id_t Type_searchField (T t, Id_t id)
{
  LinkedList_t structt;
  Type_Struct_t s;

  if (t->kind != TYPE_A_STRUCT)
    Error_bug ("should be checked in caller");
  structt = LinkedList_getFirst (t->u.structt.fields);
  while (structt){
    s = (Type_Struct_t)structt->data;
    if (AstId_equals (id, s->name))
      return s->type;
    structt = structt->next;
  }
  return 0;
}

Id_t Type_dest_array (T t)
{
  Assert_ASSERT (t);
  switch (t->kind){
  case TYPE_A_ARRAY:
    return t->u.array.eltType;
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

Tuple_t Type_dest_fun (T t)
{
  Assert_ASSERT (t);
  switch (t->kind){
  case TYPE_A_FUN:
    return Tuple_new (t->u.fun.from, t->u.fun.to);
  default:
    Error_bug ("impossible");
    return 0;
  }
  Error_bug ("impossible");
  return 0;
}

#undef Id_t
#undef T
#undef S
