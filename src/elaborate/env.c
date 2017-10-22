#include "../lib/assert.h"
#include "../lib/mem.h"
#include "../lib/hash.h"
#include "../lib/tuple.h"
#include "../lib/stack.h"
#include "../control/region.h"
#include "../control/error-msg.h"
#include "env.h"

#define Id_t AstId_t
#define Tenv_t Hash_t
//Global variable
#define Senv_t Hash_t
#define Genv_t Hash_t

#define Venv_t Stack_t



Env_Binding_t Env_Binding_new (Type_t ty, Id_t fresh)
{
  Env_Binding_t t;
  Mem_NEW (t);
  t->type = ty;
  t->fresh = fresh;
  return t;
}

static void error (String_t s, Region_t r)
{
  ErrorMsg_elabError (s, r);
}

static void error_dupTid (Id_t k)
{
  error (String_concat
         ("duplicate type variable: ",
          AstId_toString (k),
          0),
         AstId_dest (k));
  return;
}

static Tenv_t tenv = 0;
static Genv_t genv=0;
static Genv_t genv_b=0;

void Genv_init ()
{

   genv = Hash_new ((tyHashCode)AstId_hashCode,
                   (Poly_tyEquals)AstId_equals);

}

void Tenv_init ()
{
  tenv = Hash_new ((tyHashCode)AstId_hashCode,
                   (Poly_tyEquals)AstId_equals);
  {
    Id_t intId =
      AstId_fromString ("int",
                        Region_bogus ());
    Hash_insert (tenv,
                 intId,
                 Env_Binding_new (Type_new_int (),
                                  intId),
                 (tyDup)error_dupTid);
  }
  {
    Id_t stringId =
      AstId_fromString ("string",
                        Region_bogus ());
    Hash_insert (tenv,
                 stringId,
                 Env_Binding_new (Type_new_string (),
                                  stringId),
                 (tyDup)error_dupTid);
  }
  {
    Id_t voidId =
      AstId_fromString ("void",
                        Region_bogus ());
    Hash_insert (tenv,
                 voidId,
                 Env_Binding_new (Type_new_void (),
                                  voidId),
                 (tyDup)error_dupTid);
  }
  {
    Id_t floatId =
      AstId_fromString ("float",
                        Region_bogus ());
    Hash_insert (tenv,
                 floatId,
                 Env_Binding_new (Type_new_float(),
                                  floatId),
                 (tyDup)error_dupTid);
  }
  //doube 
  {
    Id_t doubleId =
      AstId_fromString ("double",
                        Region_bogus ());
    Hash_insert (tenv,
                 doubleId,
                 Env_Binding_new (Type_new_double(),
                                  doubleId),
                 (tyDup)error_dupTid);
  }
  return;
}

void Tenv_insert (Id_t id, Type_t type)
{
  Hash_insert (tenv,
               id,
               Env_Binding_new (type,
                                AstId_newNoName ()),
               (tyDup)error_dupTid);
}

Env_Binding_t Tenv_lookup (Id_t id)
{
  return Hash_lookup (tenv, id);
}
Env_Binding_t Genv_lookup (Id_t id)
{
  if(!genv)
    return 0;
  return Hash_lookup (genv, id);
}
void Genv_restore()
{
  if(genv_b)
  genv=genv_b;

}
void Genv_clear()
{
  //if have cleared don't cleared more 
  if(genv)
  {
  genv_b=genv;
  genv=0;
  }
}

Env_Binding_t Tenv_lookupMustExist (Id_t id)
{
  Env_Binding_t r = (Env_Binding_t)Tenv_lookup (id);
  if (!r){
    error (String_concat
           ("unbound type variable: ",
            AstId_toString (id),
            0),
           AstId_dest (id));
    return Env_Binding_new (Type_new_int (),
                            AstId_bogus ());
  }
  return r;
}

static Venv_t venv = 0;

static void error_dupVid (Id_t k)
{
  error (String_concat
         ("duplicate variable: ",
          AstId_toString (k),
          0),
         AstId_dest (k));
  return;
}

void Venv_init ()
{
  Hash_t h;

  venv = Stack_new ();
  h = Hash_new ((tyHashCode)AstId_hashCode,
                (Poly_tyEquals)AstId_equals);
  /* build all libraries */
  {
    Id_t idIo_printi = AstId_fromString ("printi",
                                         Region_bogus ());
    Hash_insert (h,
                 idIo_printi,
                 Env_Binding_new
                 (Type_new_fun (Type_new_product
                                (Type_new_int(),
                                 0),
                                Type_new_int ()),
                  idIo_printi),
                 (tyDup)error_dupVid);
    Stack_push (venv, h);
  }
  {
    Id_t idIo_print = AstId_fromString ("prints",
                                        Region_bogus ());
    Hash_insert (h,
                 idIo_print,
                 Env_Binding_new
                 (Type_new_fun (Type_new_product
                                (Type_new_string(),
                                 0),
                                Type_new_int ()),
                  idIo_print),
                 (tyDup)error_dupVid);
    Stack_push (venv, h);
  }
  return;
}

Id_t Venv_insert (Id_t id, Type_t ty)
{
  Hash_t h;
  Id_t fresh = AstId_newNoName ();
  
  Assert_ASSERT(id);
  Assert_ASSERT(ty);
  h = Stack_top (venv);
  Hash_insert (h,
               id,
               Env_Binding_new (ty,
                                fresh),
               (tyDup)error_dupVid);
  return fresh;
}
Id_t Venv_insert_fresh(Id_t id, Type_t ty,Id_t fresh)
{
  Hash_t h;
  //Id_t fresh = AstId_newNoName ();
  
  Assert_ASSERT(id);
  Assert_ASSERT(ty);
  h = Stack_top (venv);
  Hash_insert (h,
               id,
               Env_Binding_new (ty,
                                fresh),
               (tyDup)error_dupVid);
  return fresh;
}


void Genv_insert (Id_t id, Type_t ty)
{

  Assert_ASSERT(id);
  Assert_ASSERT(ty);

  Hash_insert (genv,
               id,
               Env_Binding_new (ty,
                                id),
               (tyDup)error_dupTid);

}

Env_Binding_t Venv_lookup (Id_t id)
{
  LinkedList_t list = LinkedList_getFirst (venv);
  Hash_t hash;
  Env_Binding_t r;

  while (list){
    hash = (Hash_t)list->data;
    r = Hash_lookup (hash, id);
    if (r)
      return r;
    list = list->next;
  }
  return 0;
}
/*
  first look if in the local hash
  then look if in the global hash
  AHA

*/
Env_Binding_t Venv_lookupMustExist (Id_t id)
{
  Env_Binding_t b;
  b = Venv_lookup (id);
  if (!b){
  	b= Genv_lookup(id);  // AHA not easy MAN
	if(!b)
		{
    error (String_concat
           ("unbound variable: ",
            AstId_toString (id),
            0),
           AstId_dest (id));
    return Env_Binding_new (Type_new_int (),
                            AstId_bogus ());
		}
  }
  return b;
}

Env_Binding_t Genv_lookupMustExist (Id_t id)
{
  Env_Binding_t b;
  b = Genv_lookup (id);
  if (!b){
    error (String_concat
           ("unbound variable: ",
            AstId_toString (id),
            0),
           AstId_dest (id));
    return Env_Binding_new (Type_new_int (),
                            AstId_bogus ());
  }
  return b;
}


void Venv_enterScope ()
{
  Hash_t h;

  h = Hash_new ((tyHashCode)AstId_hashCode,
                (Poly_tyEquals)AstId_equals);
  Stack_push (venv, h);
}

void Venv_exitScope ()
{
  Stack_pop (venv);
}

static Senv_t senv = 0;

static int Skey_hashCode (Tuple_t k)
{
  Assert_ASSERT(k);
  return AstId_hashCode (Tuple_first (k))
    + AstId_hashCode (Tuple_second (k));
}

static int Skey_equals (Tuple_t k1, Tuple_t k2)
{
  Assert_ASSERT (k1);
  Assert_ASSERT (k2);
  return AstId_equals (Tuple_first (k1), Tuple_first (k2))
    && AstId_equals (Tuple_second (k1), Tuple_second(k2));
}

static void error_dupField (Tuple_t k)
{
  AstId_t sid, fid;

  Assert_ASSERT(k);
  sid = Tuple_first (k);
  fid = Tuple_second (k);
  error (String_concat
         ("duplicate struct field: ",
          AstId_toString (fid),
          " in ",
          AstId_toString (sid),
          0),
         AstId_dest (fid));
}

void Senv_init ()
{
  senv = Hash_new ((tyHashCode)Skey_hashCode,
                   (Poly_tyEquals)Skey_equals);
}

void Senv_insert (Id_t sid, Id_t fid)
{
  Hash_insert (senv,
               Tuple_new (sid, fid),
               Env_Binding_new (0,
                                AstId_newNoName ()),
               (tyDup)error_dupField);
}

Env_Binding_t Senv_lookup (Id_t sid, Id_t fid)
{
  return Hash_lookup (senv, Tuple_new (sid, fid));
}

#undef Id_t
#undef Tenv_t
#undef Venv_t
#undef Senv_t
