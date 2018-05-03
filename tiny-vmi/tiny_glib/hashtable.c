/* Copyright (C) 2004 Christopher Clark <firstname.lastname@cl.cam.ac.uk> */

/*
 * There are duplicates of this code in:
 *  - tools/blktap2/drivers/hashtable.c
 */

#include "tiny_private.h" // for debugging: DBG_START
#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

/*
Credit for primes table: Aaron Krowne
 http://br.endernet.org/~akrowne/
 http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
*/
static const unsigned int primes[] = {
53, 97, 193, 389,
769, 1543, 3079, 6151,
12289, 24593, 49157, /* For 1 << 16 */
98317, 
196613, 393241, 786433, 1572869,
3145739, 6291469, 12582917, 25165843,
50331653, 100663319, 201326611, 402653189,
805306457, 1610612741  /* For 1 << 31 */
};
const unsigned int prime_table_length = sizeof(primes)/sizeof(primes[0]);
const unsigned int max_load_factor = 65; /* percentage */

/*****************************************************************************/
struct hashtable *
create_hashtable(unsigned int minsize,
                 unsigned int (*hashf) (void*),
                 int (*eqf) (void*,void*), 
                 void (*key_destroy_f) (void *),   //lele
                    void (*value_destroy_f) (void *) //lele
                    )
{
    struct hashtable *h;
    unsigned int pindex, size = primes[0];

    DBG_START;

    /* Check requested hashtable isn't too large */
    if (minsize > (1u << 30)) return NULL;

    /* Enforce size as prime */
    for (pindex=0; pindex < prime_table_length; pindex++) {
        if (primes[pindex] > minsize) { size = primes[pindex]; break; }
    }

    h = (struct hashtable *)calloc(1, sizeof(struct hashtable));
    if (NULL == h)
        goto err0;
    h->table = (struct entry **)calloc(size, sizeof(struct entry *));
    if (NULL == h->table)
        goto err1;

    h->size  = size;
    h->primeindex   = pindex;
    h->entrycount   = 0;
    h->noccupied = 0;
    h->hashfn       = hashf;
    h->eqfn         = eqf;
    h->key_destroy_func   = key_destroy_f;
    h->value_destroy_func = value_destroy_f;
    h->loadlimit    = (unsigned int)(((uint64_t)size * max_load_factor) / 100);

    DBG_DONE;
    return h;

err1:
   free(h);
err0:
    DBG_DONE;
   return NULL;
}

/*****************************************************************************/
unsigned int
hash(struct hashtable *h, void *k)
{
    /* Aim to protect against poor hash functions by adding logic here
     * - logic taken from java 1.4 hashtable source */    
    // DBG_START;
    if (h->hashfn == NULL){
        errprint("%s: hash table has no hash function initialized\n");
        return 0;
    }
    unsigned int i = h->hashfn(k);
    if (i == 0){
        errprint("%s: hashfn returned 0 as hash\n");
        return 0;
    }
    i += ~(i << 9);
    i ^=  ((i >> 14) | (i << 18)); /* >>> */
    i +=  (i << 4);
    i ^=  ((i >> 10) | (i << 22)); /* >>> */
    
    // DBG_DONE;
    
    return i;
}

/*****************************************************************************/
static int
hashtable_expand(struct hashtable *h)
{
    /* Double the size of the table to accomodate more entries */
    struct entry **newtable;
    struct entry *e;
    struct entry **pE;
    unsigned int newsize, i, index;

    DBG_START;

    /* Check we're not hitting max capacity */
    if (h->primeindex == (prime_table_length - 1)) return 0;
    newsize = primes[++(h->primeindex)];

    newtable = (struct entry **)calloc(newsize, sizeof(struct entry*));
    if (NULL != newtable)
    {
        /* This algorithm is not 'stable'. ie. it reverses the list
         * when it transfers entries between the tables */
        for (i = 0; i < h->size; i++) {
            while (NULL != (e = h->table[i])) {
                h->table[i] = e->next;
                index = indexFor(newsize,e->h);
                e->next = newtable[index];
                newtable[index] = e;
            }
        }
        free(h->table);
        h->table = newtable;
    }
    /* Plan B: realloc instead */
    else 
    {
        newtable = (struct entry **)
                   realloc(h->table, newsize * sizeof(struct entry *));
        if (NULL == newtable) { (h->primeindex)--; return 0; }
        h->table = newtable;
        memset(newtable[h->size], 0, newsize - h->size);
        for (i = 0; i < h->size; i++) {
            for (pE = &(newtable[i]), e = *pE; e != NULL; e = *pE) {
                index = indexFor(newsize,e->h);
                if (index == i)
                {
                    pE = &(e->next);
                }
                else
                {
                    *pE = e->next;
                    e->next = newtable[index];
                    newtable[index] = e;
                }
            }
        }
    }
    h->size = newsize;
    h->loadlimit   = (unsigned int)
        (((uint64_t)newsize * max_load_factor) / 100);
    
    DBG_DONE;

    return -1;
}

int hashtable_set_optimal_size(struct hashtable *h){

    unsigned int minsize;
    unsigned int size;
    unsigned int pindex;

    DBG_START;

    minsize = h->entrycount * 2;
    /* Check requested hashtable isn't too large */
    if (minsize > (1u << 30)) return NULL;

    /* Enforce size as prime */

    for (pindex=0; pindex < prime_table_length; pindex++) {
        if (primes[pindex] > minsize) { size = primes[pindex]; break; }
    }

    h->primeindex = pindex;
    h->size = size;
    h->loadlimit    = (unsigned int)(((uint64_t)size * max_load_factor) / 100);

    DBG_DONE;

    return -1;
}
/*************
different with hashtable_expand: it can also shrink hashtable.
****************************************************************/
int
hashtable_resize(struct hashtable *h)
{
    /* Double the size of the table to accomodate more entries */
    struct entry **newtable;
    struct entry *e;
    struct entry **pE;
    unsigned int newsize, i, index;
    DBG_START;
    /* Check we're not hitting max capacity */
    if (h->primeindex == (prime_table_length - 1)) return 0;
    // newsize = primes[++(h->primeindex)];
    newsize = hashtable_set_optimal_size(h);
    h->noccupied = 0; // recount the occupied nodes.

    newtable = (struct entry **)calloc(newsize, sizeof(struct entry*));
    if (NULL != newtable)
    {
        /* This algorithm is not 'stable'. ie. it reverses the list
         * when it transfers entries between the tables */
        for (i = 0; i < h->size; i++) {
            while (NULL != (e = h->table[i])) {  // iterate through the header of list in old table h->table[i], copy to newtable[index] according to the hash value of each node in list.
                h->table[i] = e->next;  // set old table list to next node.
                index = indexFor(newsize,e->h);
                e->next = newtable[index]; //insert to header of list.
                if (newtable[index] == NULL){
                    h->noccupied ++;
                }
                newtable[index] = e;
            }
        }
        free(h->table);
        h->table = newtable;
    }
    /* Plan B: realloc instead */
    else 
    {
        newtable = (struct entry **)
                   realloc(h->table, newsize * sizeof(struct entry *));
        if (NULL == newtable) { (h->primeindex)--; return 0; }
        h->table = newtable;
        memset(newtable[h->size], 0, newsize - h->size); // set to 0
        for (i = 0; i < h->size; i++) {
            // iterate through one old list.
            for (pE = &(newtable[i]), e = *pE; e != NULL; e = *pE) {
                // for each list header, find new index in newtable
                index = indexFor(newsize,e->h);

                if (newtable[index] == NULL){
                        h->noccupied ++;
                }

                // if index not changed, keep the list as before, no change.
                if (index == i){
                    pE = &(e->next);
                }else{  
                // if index changed, insert node to head of list.
                // then keep to copy the next entry of current list i.
                    *pE = e->next;
                    e->next = newtable[index];
                    newtable[index] = e;
                }
            }
        }
    }
    h->size = newsize;
    h->loadlimit   = (unsigned int)
        (((uint64_t)newsize * max_load_factor) / 100);

    DBG_DONE;
    return -1;
}

/*****************************************************************************/
unsigned int
hashtable_count(struct hashtable *h)
{
    return h->entrycount;
}

/*****************************************************************************/
int
hashtable_insert(struct hashtable *h, void *k, void *v)
{
    /* This method allows duplicate keys - but they shouldn't be used */
    unsigned int index;
    struct entry *e;
    DBG_START;
    if (++(h->entrycount) > h->loadlimit)
    {
        /* Ignore the return value. If expand fails, we should
         * still try cramming just this value into the existing table
         * -- we may not have memory for a larger table, but one more
         * element may be ok. Next time we insert, we'll try expanding again.*/
        hashtable_expand(h);
    }
    e = (struct entry *)calloc(1, sizeof(struct entry));
    if (NULL == e) { 
        --(h->entrycount); 
        dbprint(VMI_DEBUG_TEST, "%s: error calloc entry\n", __FUNCTION__); 
        return 0; 
    } /*oom*/

    e->h = hash(h,k);
    index = indexFor(h->size,e->h);
    e->k = k;
    e->v = v;
    e->next = h->table[index]; // insert to header!
    if (h->table[index] == NULL){
        h->noccupied ++; // the index i being occupied for the first time.
    }
    h->table[index] = e;
    DBG_DONE;
    return -1;
}

/*****************************************************************************/
void * /* returns value associated with key */
hashtable_search(struct hashtable *h, void *k)
{
    struct entry *e;
    unsigned int hashvalue, index;

    DBG_START;

    if (h == NULL){
        errprint("%s: hash table is NULL. cannot do search\n", __FUNCTION__);
        goto error_exit;
    }

    hashvalue = hash(h,k);

    // dbprint(VMI_DEBUG_TEST, "%s: got hashvalue: 0x%x\n", __FUNCTION__, hashvalue);
    if (hashvalue == 0){ // doubel check hash value, when hash() fails, it returns 0
        errprint("%s: hashvalue invalid. cannot do search\n", __FUNCTION__);
        goto error_exit;
    }
    index = indexFor(h->size,hashvalue);

    // dbprint(VMI_DEBUG_TEST, "%s: got index: %d\n", __FUNCTION__, index);
    
    e = h->table[index];

    // dbprint(VMI_DEBUG_TEST, "%s: got entry pointer: 0x%p\n", __FUNCTION__, e);
    
    while (NULL != e)
    {

        // dbprint(VMI_DEBUG_TEST, "%s: check hash and value for entry: 0x%p\n", __FUNCTION__, e);
    
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k))) {

            DBG_DONE;
            return e->v;
        }
        e = e->next;
    }
error_exit:

    DBG_DONE;
    return NULL;
}

/*****************************************************************************/
struct entry * /* returns entry associated with key */
hashtable_search_entry(struct hashtable *h, void *k)
{
    struct entry *e;
    unsigned int hashvalue, index;
    hashvalue = hash(h,k);
    index = indexFor(h->size,hashvalue);
    e = h->table[index];
    while (NULL != e)
    {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k))) return e;
        e = e->next;
    }
    return NULL;
}

/*****************************************************************************/
void * /* returns value associated with key */
hashtable_remove(struct hashtable *h, void *k)
{
    /* TODO: consider compacting the table when the load factor drops enough,
     *       or provide a 'compact' method. */

    struct entry *e;
    struct entry **pE;
    void *v;
    unsigned int hashvalue, index;

    hashvalue = hash(h,k);
    //index = indexFor(h->size,hash(h,k)); // ? hashvalue not reused?
    index = indexFor(h->size,hashvalue);
    pE = &(h->table[index]);
    e = *pE;
    while (NULL != e)
    {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k)))
        {
            *pE = e->next;
            h->entrycount--;
            v = e->v;
            // freekey(e->k);
            // free(e);
            h->key_destroy_func (e->k);
            h->value_destroy_func(e->v);
            free(e);
            return v;
        }
        pE = &(e->next);
        e = e->next;
    }
    return NULL;
}

/*****************************************************************************/
/* destroy */
void
hashtable_destroy(struct hashtable *h, int free_values)
{
    unsigned int i;
    struct entry *e, *f;
    struct entry **table = h->table;
    if (free_values)
    {
        for (i = 0; i < h->size; i++)
        {
            e = table[i];
            while (NULL != e)
            { 
                f = e; 
                e = e->next; 
                // freekey(f->k); 
                // free(f->v); 
                // free(f); 
                h->key_destroy_func(f->k);
                h->value_destroy_func(f->v);
                free(f);
            }
        }
    }
    else
    {
        for (i = 0; i < h->size; i++)
        {
            e = table[i];
            while (NULL != e)
            { 
                f = e;
                e = e->next; 
                // freekey(f->k); 
                // free(f); 
                h->key_destroy_func(f->k);
                // h->value_destroy_func(f->v);
                free(f);
            }
        }
    }
    free(h->table);
    free(h);
}

/*
 * Copyright (c) 2002, Christopher Clark
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * * Neither the name of the original author; nor the names of any contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
