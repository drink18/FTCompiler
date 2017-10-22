/**************************************************
*  This is the hash function for the whole project
*  copyright @firstech.co.ltd.
*  Created by yuanjian,contact author by email:ahczbhht@163.com
*
*  
* revised list           
* DATA                 AUTHOR       Revised 
*2015.12.10         yuanjian       Hash_lookup function because isn't use
							use the hash value of K,find use 
							whole search in the Hash Table
							So is can't use the high-efficiency of Hash
							Table ,Probable have problem,but not find yet
													
**************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "int.h"
#include "double.h"
#include "mem.h"
#include "linkedList.h"
#include "string.h"
#include "error.h"
#include "triple.h"
#include "todo.h"
#include "hash.h"

#define INIT_SIZE 4096
#define INIT_MASK 4095
#define INIT_LOAD_FACTOR (0.25)

#define T Hash_t
#define K Poly_t
#define V Poly_t




struct T
{
  LinkedList_t *buckets;
  int (*hashCode)(K);
  int (*equals)(K, K);
  int numItems;
  int size;
  int mask;
  double load;
};

T Hash_new (int (*hashCode)(K)
            , int (*equals)(K, K))
{
  int i;
  T h;

  Mem_NEW (h);
  Mem_NEW_SIZE (h->buckets, INIT_SIZE);
  for (i=0; i<INIT_SIZE; i++)
    *(h->buckets + i) = LinkedList_new ();
  h->hashCode = hashCode;
  h->equals = equals;
  h->numItems = 0;
  h->size = INIT_SIZE;
  h->mask = INIT_MASK;
  h->load = INIT_LOAD_FACTOR;
  return h;
}

static double currentLoad (T h)
{
  return 1.0*h->numItems/h->size;
}

T Hash_clone (T old)
{
  T h;
  int i;
  int newSize = old->size * 2;

  Mem_NEW (h);
  Mem_NEW_SIZE (h->buckets, newSize);

  for (i=0; i<newSize; i++)
    *(h->buckets + i) = LinkedList_new ();
  h->numItems = old->numItems + 1;
  h->size = newSize;
  h->mask = newSize-1;
  h->hashCode = old->hashCode;
  h->equals = old->equals;
  return h;
}

typedef struct
{
  int insertions;
  int lookups;
  int links;
  int longest;
  int maxSize;
  double maxLoad;
} Status_t;

static Status_t all = {0, 0, 0, 0, 0, 0.0};

void Hash_insert (T h
                  , K k
                  , V v
                  , void (*dup)(K))
{
  int initHc, hc;
  LinkedList_t list;

  Assert_ASSERT (h);
  all.insertions++;
  if (Hash_lookup (h, k)){
    dup (k);
  }
  initHc = h->hashCode (k);
  hc = initHc & (h->mask);
  list = *(h->buckets + hc);
  LinkedList_insertFirst (list,
                          Triple_new (k,
                                      (Poly_t)initHc, v));
  h->numItems++;
  if (currentLoad(h) >= h->load){
    LinkedList_t *oldBuckets, *newBuckets;
    int oldSize, newSize;
    int i;

    oldBuckets = h->buckets;
    oldSize = h->size;
    newSize = oldSize * 2;
    Mem_NEW_SIZE (newBuckets, newSize);
    for (i=0; i<newSize; i++)
      *(newBuckets + i) = LinkedList_new ();
    h->buckets = newBuckets;
    h->size = newSize;
    h->mask = newSize - 1;
    for (i=0; i<oldSize; i++){
      LinkedList_t list;
      list = *(oldBuckets+i);
      list = LinkedList_getFirst (list);
      while (list){
        Triple_t t = (Triple_t)(list->data);
        int initHc = (int)Triple_second (t);

        LinkedList_insertLast
          (*(newBuckets+(initHc &h->mask))
           , t);
        list = list->next;
      }
    }
  }
  return;
}

static int longestChain (T h)
{
  int max = 0;
  int current;
  int i;

  for (i=0; i<h->size; i++){
    current = LinkedList_size (*(h->buckets+i));
    if (current > max)
      max = current;
  }
  return max;
}

static int numEmptyBuckets (T h)
{
  int empty = 0;
  int i;
  Assert_ASSERT(h);

  for (i=0; i<h->size; i++){
    if (LinkedList_isEmpty (*(h->buckets+i)))
      empty++;
  }
  return empty;
}

String_t Hash_status (T h)
{
  int empty;
  String_t s;

  Assert_ASSERT(h);
  empty = numEmptyBuckets (h);
  s = String_concat
    ("number of items are: ",
     Int_toString (h->numItems),
     "\nnumber of buckets: ",
     Int_toString (h->size),
     "\nnumber of empty buckets: ",
     numEmptyBuckets (h),
     "\nnumber of not empty buckets: ",
     Int_toString (h->size-empty),
     "\nlongest chain size: ",
     Int_toString (longestChain (h)),
     "\ncrrent load factor: ",
     Double_toString (h->load),
     "\ndefault load factor: ",
     Double_toString (INIT_LOAD_FACTOR),
     0);
  return s;
}

V Hash_lookup (T h, K x)
{
  int hc;
  int i;
  int link = 0;
  int size;
  double load;

  Assert_ASSERT (h);

  all.lookups++;
  size = Hash_size (h);
  if (size>all.maxSize)
    all.maxSize = size;
  load = 1.0 * Hash_numItems (h)/size;
  if (load>all.maxLoad)
    all.maxLoad = load;

  hc = h->hashCode(x);
  hc &= h->mask;
  //This I find hc is non't used why?
  ///  I used it
 // for (i=0; i<h->size; i++){
    LinkedList_t list = *(h->buckets+hc);
    list = LinkedList_getFirst (list);
    while (list){
      Triple_t t = (Triple_t)(list->data);
      K key = Triple_first (t);
      V value = Triple_third (t);

      link++;
      if (h->equals (x, key)){
        all.links += link;
        if (link>all.longest)
          all.longest = link;
        return value;
      }
      list = list->next;
    }
 // }
  all.links += link;
  if (link>all.longest)
    all.longest = link;
  return 0;
}
LinkedList_t Hash_lookup_list (T h, K x)
{
  int hc;
  int i;
  int link = 0;
  int size;
  double load;

  Assert_ASSERT (h);

  all.lookups++;
  size = Hash_size (h);
  if (size>all.maxSize)
    all.maxSize = size;
  load = 1.0 * Hash_numItems (h)/size;
  if (load>all.maxLoad)
    all.maxLoad = load;

  hc = h->hashCode(x);
  hc &= h->mask;
  //This I find hc is non't used why?
  ///  I used it
 // for (i=0; i<h->size; i++){
    LinkedList_t list = *(h->buckets+hc);
    list = LinkedList_getFirst (list);
    while (list){
      Triple_t t = (Triple_t)(list->data);
      K key = Triple_first (t);
      V value = Triple_third (t);

      link++;
      if (h->equals (x, key)){
        all.links += link;
        if (link>all.longest)
          all.longest = link;
        return list;
      }
      list = list->next;
    }
 // }
  all.links += link;
  if (link>all.longest)
    all.longest = link;
  return 0;
}

V Hash_lookupOrInsert (T h
                       , K k
                       , V (*gen)(K))
{
  V r;

  if ((r = Hash_lookup (h, k)))
    return r;
  r = gen (k);
  Hash_insert (h, k, r, 0);
  return r;
}

void Hash_delete (T h
                  , K x
                  , void (*notFound)(K))
{
// TODO;
int hc;
 int i;
 int link = 0;
 int size;
 double load;
 LinkedList_t list;
 Assert_ASSERT (h);
 //first looup exist;

 hc = h->hashCode(x);
 hc &= h->mask;
 //This I find hc is non't used why?
 ///  I used it
// for (i=0; i<h->size; i++){
   list = *(h->buckets+hc);
   list = LinkedList_getFirst (list);
   while (list){
	 Triple_t t = (Triple_t)(list->data);
	 K key = Triple_first (t);
	 V value = Triple_third (t);

	 link++;
	 if (h->equals (x, key)){
	 	///find equals;
	 	h->numItems++;
		*(h->buckets+hc)=LinkedList_new ();	
		return ;
	 }
	 list = list->next;
   }
 
 notFound(x);

}

int Hash_size (T h)
{
  Assert_ASSERT (h);
  return h->size;
}

int Hash_numItems (T h)
{
  Assert_ASSERT(h);
  return h->numItems;
}

void Hash_statusAll (FILE *fp)
{
  fprintf (fp,"%s\n", "Hash table status:");
  fprintf (fp,"  Num of insertions: %d\n", all.insertions);
  fprintf (fp,"  Num of lookups   : %d\n", all.lookups);
  fprintf (fp,"  Num of links     : %d\n", all.links);
  fprintf (fp,"  Longest chain    : %d\n", all.longest);
  fprintf (fp,"  Max hash size    : %d\n", all.maxSize);
  fprintf (fp,"  Max load factor  : %lf\n", all.maxLoad);
  fprintf (fp,"  Average position : %lf\n",
          1.0*all.links/all.lookups);
  return;
}

#undef V
#undef K
#undef T
