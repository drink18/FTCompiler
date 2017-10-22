#include "../lib/mem.h"
#include "../lib/hash.h"
#include "../lib/assert.h"
#include "../lib/int.h"
#include "../lib/random.h"
#include "../lib/property-list.h"
#include "label.h"

#define T Label_t

static int counter = 0;

struct T
{
  String_t name;
  int hashCode;
  PropertyList_t plist;
};

T Label_new ()
{
  T x;
  Mem_NEW (x);
  x->name = String_concat ("L_", 
                           Int_toString (counter++),
                           0);
  x->hashCode = Random_nextInt ();
  x->plist = PropertyList_new ();
  return x;
}
//this I don't use name 
T Label_new2 (String_t name)
{
  T x;
  Mem_NEW (x);
  x->name= name;
  x->hashCode = Random_nextInt ();
  x->plist = PropertyList_new ();
  return x;
}


int Label_hashCode (T x)
{
  Assert_ASSERT(x);
  return x->hashCode;
}

String_t Label_toString (T x)
{
  Assert_ASSERT (x);
  return (x->name);
}

int Label_equals (T x, T y)
{
  Assert_ASSERT(x);
  Assert_ASSERT(y);
  return x == y;
}

PropertyList_t Label_plist (T x)
{
  Assert_ASSERT(x);
  return x->plist;
}

#undef T
