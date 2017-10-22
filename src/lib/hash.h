#ifndef HASH_H
#define HASH_H

#include "string.h"
#include "poly.h"
#include "linkedList.h"

#define T Hash_t
#define K Poly_t
#define V Poly_t

typedef struct T *T;

typedef V (*tyKV)(K);
typedef void (*tyDup)(K);
typedef void (*tyNotFind)(K);
T Hash_new (int (*hashCode)(K)
            , int (*equals)(K, K));
/* insert a binding: k |-> v into a hash h,
 * with the equality testing function "equals".
 * if such a binding already exists, then call dup(k).
 */
void Hash_insert (T h, 
                  K k, 
                  V v,
                  void (*dup)(K));
LinkedList_t Hash_lookup_list (T h, K x);

V Hash_lookup (T h, 
               K k);
/* lookup the key "k" in the hash "h".
 * if the item not found, then call gen(k) to generate a value
 * insert the binding "k->v" into the hash before returning v.
 */
V Hash_lookupOrInsert (T h, 
                       K k,
                       V (*gen)(K));
/* delete a key k from a hash. Error if k not exists, 
 * call notFound (k).
 */
void Hash_delete (T h, 
                  K x,
                  void (*notFound)(K));
double Hash_loadFactor (T h);
String_t Hash_status (T h);
void Hash_statusAll ();
int Hash_size (T h);
int Hash_numItems (T h);

#undef K
#undef V
#undef T

#endif
