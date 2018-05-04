
// #include <tiny_glib.h>

/* Functions defined here are mostly copied from glib/ghash.c of official glib repo
at https://github.com/GNOME/glib, commit 7c1e2b7cef88c9b9685ab88e0b2b7c45e853fd8c ,on Mar 10, 2018*/

#include "tiny_private.h" // for debugging purpose

#include "hash.h"
#include "hashtable.h"
#include "types.h"


#define HASH_TABLE_MIN_SHIFT 3  /* 1 << 3 == 8 buckets */

#define UNUSED_HASH_VALUE 0
#define TOMBSTONE_HASH_VALUE 1
#define HASH_IS_UNUSED(h_) ((h_) == UNUSED_HASH_VALUE)
#define HASH_IS_TOMBSTONE(h_) ((h_) == TOMBSTONE_HASH_VALUE)
#define HASH_IS_REAL(h_) ((h_) >= 2)


typedef struct
{
  GHashTable  *hash_table;
  gpointer     dummy1;
  struct entry *current_entry; //lele: to keep track of current entry at hash_table->table[position]
  int          position;
  gboolean     dummy3;
  int          version;
} RealIter;



// /* Each table size has an associated prime modulo (the first prime
//  * lower than the table size) used to find the initial bucket. Probing
//  * then works modulo 2^n. The prime modulo is necessary to get a
//  * good distribution with poor hash functions.
//  */
// static const gint prime_mod [] =
// {
//   1,          /* For 1 << 0 */
//   2,
//   3,
//   7,
//   13,
//   31,
//   61,
//   127,
//   251,
//   509,
//   1021,
//   2039,
//   4093,
//   8191,
//   16381,
//   32749,
//   65521,      /* For 1 << 16 */
//   131071,
//   262139,
//   524287,
//   1048573,
//   2097143,
//   4194301,
//   8388593,
//   16777213,
//   33554393,
//   67108859,
//   134217689,
//   268435399,
//   536870909,
//   1073741789,
//   2147483647  /* For 1 << 31 */
// };

// static void
// g_hash_table_set_shift (GHashTable *hash_table, gint shift)
// {
//   gint i;
//   guint mask = 0;

//   hash_table->size = 1 << shift;
//   hash_table->mod  = prime_mod [shift];

//   for (i = 0; i < shift; i++)
//     {
//       mask <<= 1;
//       mask |= 1;
//     }

//   hash_table->mask = mask;
// }

// static gint
// g_hash_table_find_closest_shift (gint n)
// {
//   gint i;

//   for (i = 0; n; i++)
//     n >>= 1;

//   return i;
// }



// static void
// g_hash_table_set_shift_from_size (GHashTable *hash_table, gint size)
// {
//   gint shift;

//   shift = g_hash_table_find_closest_shift (size);
//   shift = MAX (shift, HASH_TABLE_MIN_SHIFT);

//   g_hash_table_set_shift (hash_table, shift);
// }


/*
 * g_hash_table_resize:
 * @hash_table: our #GHashTable
 *
 * Resizes the hash table to the optimal size based on the number of
 * nodes currently held. If you call this function then a resize will
 * occur, even if one does not need to occur.
 * Use g_hash_table_maybe_resize() instead.
 *
 * This function may "resize" the hash table to its current size, with
 * the side effect of cleaning up tombstones and otherwise optimizing
 * the probe sequences.
 */
static void
g_hash_table_resize (GHashTable *hash_table)
{
  hashtable_resize(hash_table);
}

/*
 * g_hash_table_maybe_resize:
 * @hash_table: our #GHashTable
 *
 * Resizes the hash table, if needed.
 *
 * Essentially, calls g_hash_table_resize() if the table has strayed
 * too far from its ideal size for its number of nodes.
 */
static inline void
g_hash_table_maybe_resize (GHashTable *hash_table)
{
  gint noccupied = hash_table->noccupied;
  gint size = hash_table->size;

  if ((size > hash_table->entrycount * 4 && size > 1 << HASH_TABLE_MIN_SHIFT) ||
      (size <= noccupied + (noccupied / 16)))
    g_hash_table_resize (hash_table);
}



/* Hash functions.
 */

/**
 * g_str_equal:
 * @v1: (not nullable): a key
 * @v2: (not nullable): a key to compare with @v1
 *
 * Compares two strings for byte-by-byte equality and returns %TRUE
 * if they are equal. It can be passed to g_hash_table_new() as the
 * @key_equal_func parameter, when using non-%NULL strings as keys in a
 * #GHashTable.
 *
 * Note that this function is primarily meant as a hash table comparison
 * function. For a general-purpose, %NULL-safe string comparison function,
 * see g_strcmp0().
 *
 * Returns: %TRUE if the two keys match
 */
gboolean
g_str_equal (gconstpointer v1,
             gconstpointer v2)
{
  const gchar *string1 = v1;
  const gchar *string2 = v2;

  return strcmp (string1, string2) == 0;
}

/**
 * g_str_hash:
 * @v: (not nullable): a string key
 *
 * Converts a string to a hash value.
 *
 * This function implements the widely used "djb" hash apparently
 * posted by Daniel Bernstein to comp.lang.c some time ago.  The 32
 * bit unsigned hash value starts at 5381 and for each byte 'c' in
 * the string, is updated: `hash = hash * 33 + c`. This function
 * uses the signed value of each byte.
 *
 * It can be passed to g_hash_table_new() as the @hash_func parameter,
 * when using non-%NULL strings as keys in a #GHashTable.
 *
 * Note that this function may not be a perfect fit for all use cases.
 * For example, it produces some hash collisions with strings as short
 * as 2.
 *
 * Returns: a hash value corresponding to the key
 */
guint
g_str_hash (gconstpointer v)
{
  const signed char *p;
  // guint32 h = 5381;
  guint h = 5381;

  DBG_START;

  for (p = v; *p != '\0'; p++)
    h = (h << 5) + h + *p;

  return h;
}

/**
 * g_int_equal:
 * @v1: (not nullable): a pointer to a #gint key
 * @v2: (not nullable): a pointer to a #gint key to compare with @v1
 *
 * Compares the two #gint values being pointed to and returns
 * %TRUE if they are equal.
 * It can be passed to g_hash_table_new() as the @key_equal_func
 * parameter, when using non-%NULL pointers to integers as keys in a
 * #GHashTable.
 *
 * Note that this function acts on pointers to #gint, not on #gint
 * directly: if your hash table's keys are of the form
 * `GINT_TO_POINTER (n)`, use g_direct_equal() instead.
 *
 * Returns: %TRUE if the two keys match.
 */
gboolean
g_int_equal (gconstpointer v1,
             gconstpointer v2)
{
  return *((const gint*) v1) == *((const gint*) v2);
}

/**
 * g_int_hash:
 * @v: (not nullable): a pointer to a #gint key
 *
 * Converts a pointer to a #gint to a hash value.
 * It can be passed to g_hash_table_new() as the @hash_func parameter,
 * when using non-%NULL pointers to integer values as keys in a #GHashTable.
 *
 * Note that this function acts on pointers to #gint, not on #gint
 * directly: if your hash table's keys are of the form
 * `GINT_TO_POINTER (n)`, use g_direct_hash() instead.
 *
 * Returns: a hash value corresponding to the key.
 */
guint
g_int_hash (gconstpointer v)
{
  DBG_START;

  //cite: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
  gint x = *(const gint*)v;

  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;

  // return *(const gint*) v;
}

/**
 * g_int64_equal:
 * @v1: (not nullable): a pointer to a #gint64 key
 * @v2: (not nullable): a pointer to a #gint64 key to compare with @v1
 *
 * Compares the two #gint64 values being pointed to and returns
 * %TRUE if they are equal.
 * It can be passed to g_hash_table_new() as the @key_equal_func
 * parameter, when using non-%NULL pointers to 64-bit integers as keys in a
 * #GHashTable.
 *
 * Returns: %TRUE if the two keys match.
 *
 * Since: 2.22
 */
gboolean
g_int64_equal (gconstpointer v1,
               gconstpointer v2)
{
  // guint keyHash1 = ((guint) v1) < 2 ? 2 : (guint) v1;
  // guint keyHash2 = ((guint) v2) < 2 ? 2 : (guint) v2;

  return *((const gint64*) v1) == *((const gint64*) v2);
  // return keyHash1 == keyHash2;
}

/**
 * g_int64_hash:
 * @v: (not nullable): a pointer to a #gint64 key
 *
 * Converts a pointer to a #gint64 to a hash value.
 *
 * It can be passed to g_hash_table_new() as the @hash_func parameter,
 * when using non-%NULL pointers to 64-bit integer values as keys in a
 * #GHashTable.
 *
 * Returns: a hash value corresponding to the key.
 *
 * Since: 2.22
 */
guint
g_int64_hash (gconstpointer v)
{

  // DBG_START;
  guint keyHash;

  if (v == NULL){
    errprint("%s: key is null\n", __FUNCTION__);
    return NULL;
  }

  // dbprint(VMI_DEBUG_TEST, "%s: key pointer: %p(%p)\n", __FUNCTION__, v, (const gint64 *)v);

  // cite: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
  gint64 x =  (guint) *(const gint64*) v;

    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);

  keyHash = (guint) x;

  // dbprint(VMI_DEBUG_TEST, "%s: key hash value is:\n", __FUNCTION__);

  // dbprint(VMI_DEBUG_TEST, "\t%u\n", keyHash);

  // return (guint) *(const gint64*) v;
  return (guint) keyHash;
}


/**
 * g_memdup:
 * @mem: the memory to copy.
 * @byte_size: the number of bytes to copy.
 *
 * Allocates @byte_size bytes of memory, and copies @byte_size bytes into it
 * from @mem. If @mem is %NULL it returns %NULL.
 *
 * Returns: a pointer to the newly-allocated copy of the memory, or %NULL if @mem
 *  is %NULL.
 */
gpointer
g_memdup (gconstpointer mem,
          guint         byte_size)
{
  gpointer new_mem;

  DBG_START;

  if (mem && byte_size != 0)
    {
      new_mem = g_malloc (byte_size);
      memcpy (new_mem, mem, byte_size);
    }
  else
    new_mem = NULL;

  DBG_DONE;

  return new_mem;
}

/*
 * g_hash_table_insert_internal:
 * @hash_table: our #GHashTable
 * @key: the key to insert
 * @value: the value to insert
 * @keep_new_key: if %TRUE and this key already exists in the table
 *   then call the destroy notify function on the old key.  If %FALSE
 *   then call the destroy notify function on the new key.
 *
 * Implements the common logic for the g_hash_table_insert() and
 * g_hash_table_replace() functions.
 *
 * Do a lookup of @key. If it is found, replace it with the new
 * @value (and perhaps the new @key). If it is not found, create
 * a new node.
 *
 * Returns: %TRUE if the key did not exist yet
 */
static gboolean
g_hash_table_insert_internal (GHashTable *hash_table,
                              gpointer    key,
                              gpointer    value,
                              gboolean    keep_new_key)
{
  guint key_hash;
  guint node_index;

  gboolean already_exists = FALSE;

  DBG_START;

  struct entry * e = hashtable_search_entry(hash_table, key);

  if (NULL != e){
    // found same key exists.
    already_exists = TRUE;

    if (keep_new_key){
      // free old key and insert new key
      gpointer oldkey = e->k;
      gpointer oldvalue = e->v;
      e->k = key;
      e->v = value;

      if (hash_table->key_destroy_func){
        hash_table->key_destroy_func(oldkey);
      }else{
        free(oldkey);
      }
      if (hash_table->value_destroy_func){
        hash_table->value_destroy_func(oldvalue);
      }else{
        free(oldvalue);
      }

    }else{
      // free new key and old value
      // update old key with new value

      if (hash_table->key_destroy_func){
        hash_table->key_destroy_func(key);
      }else{
        free(key);
      }
      if (hash_table->value_destroy_func){
        hash_table->value_destroy_func(e->v);
      }else{
        free(e->v);
      }
      // free(key);
      // free(e->v);
      e->v = value;
    }

  }else{
    // no key exists in the table, insert it
    hashtable_insert(hash_table, key, value);
  }

  DBG_DONE;
  
  return !already_exists;

}


/**
 * g_hash_table_insert:
 * @hash_table: a #GHashTable
 * @key: a key to insert
 * @value: the value to associate with the key
 *
 * Inserts a new key and value into a #GHashTable.
 *
 * If the key already exists in the #GHashTable its current
 * value is replaced with the new value. If you supplied a
 * @value_destroy_func when creating the #GHashTable, the old
 * value is freed using that function. If you supplied a
 * @key_destroy_func when creating the #GHashTable, the passed
 * key is freed using that function.
 *
 * Starting from GLib 2.40, this function returns a boolean value to
 * indicate whether the newly added value was already in the hash table
 * or not.
 *
 * Returns: %TRUE if the key did not exist yet
 */
gboolean g_hash_table_insert (GHashTable *hash_table, gpointer key, gpointer        value){
  
  DBG_START;
  gboolean ret = g_hash_table_insert_internal (hash_table, key, value, FALSE);
  DBG_DONE;
  return ret;
}


/**
 * g_hash_table_replace:
 * @hash_table: a #GHashTable
 * @key: a key to insert
 * @value: the value to associate with the key
 *
 * Inserts a new key and value into a #GHashTable similar to
 * g_hash_table_insert(). The difference is that if the key
 * already exists in the #GHashTable, it gets replaced by the
 * new key. If you supplied a @value_destroy_func when creating
 * the #GHashTable, the old value is freed using that function.
 * If you supplied a @key_destroy_func when creating the
 * #GHashTable, the old key is freed using that function.
 *
 * Starting from GLib 2.40, this function returns a boolean value to
 * indicate whether the newly added value was already in the hash table
 * or not.
 *
 * Returns: %TRUE if the key did not exist yet
 */
gboolean
g_hash_table_replace (GHashTable *hash_table,
                      gpointer    key,
                      gpointer    value)
{
  return g_hash_table_insert_internal (hash_table, key, value, TRUE);
}


/**
 * g_hash_table_remove:
 * @hash_table: a #GHashTable
 * @key: the key to remove
 *
 * Removes a key and its associated value from a #GHashTable.
 *
 * If the #GHashTable was created using g_hash_table_new_full(), the
 * key and value are freed using the supplied destroy functions, otherwise
 * you have to make sure that any dynamically allocated values are freed
 * yourself.
 *
 * Returns: %TRUE if the key was found and removed from the #GHashTable
 */
gboolean  g_hash_table_remove(GHashTable *hash_table,gconstpointer   key){
  gpointer *value = hashtable_remove(hash_table, key);
  // value would be null if not found.
  return (value != NULL);
}

/**
 * g_hash_table_size:
 * @hash_table: a #GHashTable
 *
 * Returns the number of elements contained in the #GHashTable.
 *
 * Returns: the number of key/value pairs in the #GHashTable.
 */
guint
g_hash_table_size (GHashTable *hash_table)
{
  g_return_val_if_fail (hash_table != NULL, 0);

  return hash_table->entrycount;
}

/*
 * g_hash_table_remove_head_entry:
 * @hash_table: our #GHashTable
 * @node i: pointer to node to remove
 * @entry: entry to remove in the list of table[i]
 * @notify: %TRUE if the destroy notify handlers are to be called
 *
 * Removes a node from the hash table and updates the node count.
 * The node is replaced by a tombstone. No table resize is performed.
 *
 * If @notify is %TRUE then the destroy notify functions are called
 * for the key and value of the hash node.
 */
static void
g_hash_table_remove_head_entry (GHashTable   *hash_table,
                          gint          i,
                          gboolean      notify)
{

  gpointer key;
  gpointer value;

  struct entry *e = hash_table->table[i];

  hash_table->table[i] = e->next; // remove entry from entry list.

  // hash_table->nnodes--;
  hash_table->entrycount--;
  
  key = e->k;
  value = e->v;

  // /* Erect tombstone */
  // e->h = TOMBSTONE_HASH_VALUE;

  // /* Be GC friendly */
  // e->k = NULL;
  // e->v = NULL;

  free(e);
  

  if (notify && hash_table->key_destroy_func)
    hash_table->key_destroy_func (key);

  if (notify && hash_table->value_destroy_func)
    hash_table->value_destroy_func (value);

}


/*
 * g_hash_table_foreach_remove_or_steal:
 * @hash_table: a #GHashTable
 * @func: the user's callback function
 * @user_data: data for @func
 * @notify: %TRUE if the destroy notify handlers are to be called
 *
 * Implements the common logic for g_hash_table_foreach_remove()
 * and g_hash_table_foreach_steal().
 *
 * Iterates over every node in the table, calling @func with the key
 * and value of the node (and @user_data). If @func returns %TRUE the
 * node is removed from the table.
 *
 * If @notify is true then the destroy notify handlers will be called
 * for each removed node.
 */
static guint
g_hash_table_foreach_remove_or_steal (GHashTable *hash_table,
                                      GHRFunc     func,
                                      gpointer    user_data,
                                      gboolean    notify)
{
  guint deleted = 0;
  gint i;
#ifndef G_DISABLE_ASSERT
  gint version = hash_table->version;
#endif

  for (i = 0; i < hash_table->size; i++)
  {

      struct entry *head_entry = hash_table->table[i]; // rm the head entry of table[i]

      while (head_entry != NULL){

        guint node_hash = head_entry->h;
        gpointer node_key = head_entry->k;
        gpointer node_value = head_entry->v;
        
        if (HASH_IS_REAL (node_hash) &&
            (* func) (node_key, node_value, user_data))
        {
            g_hash_table_remove_head_entry (hash_table, i, notify);
            deleted++;
        }

        head_entry = head_entry->next;

        hash_table->table[i] = head_entry;

      }

  }

  g_hash_table_maybe_resize (hash_table);

#ifndef G_DISABLE_ASSERT
  if (deleted > 0)
    hash_table->version++;
#endif

  return deleted;
}

/**
 * g_hash_table_foreach_remove:
 * @hash_table: a #GHashTable
 * @func: the function to call for each key/value pair
 * @user_data: user data to pass to the function
 *
 * Calls the given function for each key/value pair in the
 * #GHashTable. If the function returns %TRUE, then the key/value
 * pair is removed from the #GHashTable. If you supplied key or
 * value destroy functions when creating the #GHashTable, they are
 * used to free the memory allocated for the removed keys and values.
 *
 * See #GHashTableIter for an alternative way to loop over the
 * key/value pairs in the hash table.
 *
 * Returns: the number of key/value pairs removed
 */
guint
g_hash_table_foreach_remove (GHashTable *hash_table,
                             GHRFunc     func,
                             gpointer    user_data)
{
  g_return_val_if_fail (hash_table != NULL, 0);
  g_return_val_if_fail (func != NULL, 0);

  return g_hash_table_foreach_remove_or_steal (hash_table, func, user_data, TRUE);
}

/**
 * g_hash_table_foreach_steal:
 * @hash_table: a #GHashTable
 * @func: the function to call for each key/value pair
 * @user_data: user data to pass to the function
 *
 * Calls the given function for each key/value pair in the
 * #GHashTable. If the function returns %TRUE, then the key/value
 * pair is removed from the #GHashTable, but no key or value
 * destroy functions are called.
 *
 * See #GHashTableIter for an alternative way to loop over the
 * key/value pairs in the hash table.
 *
 * Returns: the number of key/value pairs removed.
 */
guint
g_hash_table_foreach_steal (GHashTable *hash_table,
                            GHRFunc     func,
                            gpointer    user_data)
{
  g_return_val_if_fail (hash_table != NULL, 0);
  g_return_val_if_fail (func != NULL, 0);

  return g_hash_table_foreach_remove_or_steal (hash_table, func, user_data, FALSE);
}

/**
 * g_hash_table_foreach:
 * @hash_table: a #GHashTable
 * @func: the function to call for each key/value pair
 * @user_data: user data to pass to the function
 *
 * Calls the given function for each of the key/value pairs in the
 * #GHashTable.  The function is passed the key and value of each
 * pair, and the given @user_data parameter.  The hash table may not
 * be modified while iterating over it (you can't add/remove
 * items). To remove all items matching a predicate, use
 * g_hash_table_foreach_remove().
 *
 * See g_hash_table_find() for performance caveats for linear
 * order searches in contrast to g_hash_table_lookup().
 */
void
g_hash_table_foreach (GHashTable *hash_table,
                      GHFunc      func,
                      gpointer    user_data)
{
  gint i;
// #ifndef G_DISABLE_ASSERT
//   gint version;
// #endif

  g_return_if_fail (hash_table != NULL);
  g_return_if_fail (func != NULL);

// #ifndef G_DISABLE_ASSERT
//   version = hash_table->version;
// #endif


    // struct entry *e;
    // unsigned int hashvalue, index;
    // hashvalue = hash(h,k);
    // index = indexFor(h->size,hashvalue);
    // e = h->table[index];
    // while (NULL != e)
    // {
    //     /* Check hash value to short circuit heavier comparison */
    //     if ((hashvalue == e->h) && (h->eqfn(k, e->k))) return e->v;
    //     e = e->next;
    // }
  struct entry *e;

  for (i = 0; i < hash_table->size; i++)
    {
      e = hash_table->table[i];
      
      // guint node_hash = hash_table->hashes[i];
      // gpointer node_key = hash_table->keys[i];
      // gpointer node_value = hash_table->values[i];
      while (NULL != e){
        guint node_hash = e->h;
        gpointer node_key = e->k;
        gpointer node_value = e->v;

        if (HASH_IS_REAL (node_hash))
          (* func) (node_key, node_value, user_data);
        
        e = e->next;
      }

// #ifndef G_DISABLE_ASSERT
//       g_return_if_fail (version == hash_table->version);
// #endif
    }
}


/**
 * g_hash_table_iter_init:
 * @iter: an uninitialized #GHashTableIter
 * @hash_table: a #GHashTable
 *
 * Initializes a key/value pair iterator and associates it with
 * @hash_table. Modifying the hash table after calling this function
 * invalidates the returned iterator.
 * |[<!-- language="C" -->
 * GHashTableIter iter;
 * gpointer key, value;
 *
 * g_hash_table_iter_init (&iter, hash_table);
 * while (g_hash_table_iter_next (&iter, &key, &value))
 *   {
 *     // do something with key and value
 *   }
 * ]|
 *
 * Since: 2.16
 */
void
g_hash_table_iter_init (GHashTableIter *iter,
                        GHashTable     *hash_table)
{
  RealIter *ri = (RealIter *) iter;

  g_return_if_fail (iter != NULL);
  g_return_if_fail (hash_table != NULL);

  ri->hash_table = hash_table;
  ri->position = -1;
  ri->current_entry = NULL;
#ifndef G_DISABLE_ASSERT
  ri->version = hash_table->version;
#endif
}


/**
 * g_hash_table_iter_next:
 * @iter: an initialized #GHashTableIter
 * @key: (out) (optional): a location to store the key
 * @value: (out) (optional) (nullable): a location to store the value
 *
 * Advances @iter and retrieves the key and/or value that are now
 * pointed to as a result of this advancement. If %FALSE is returned,
 * @key and @value are not set, and the iterator becomes invalid.
 *
 * Returns: %FALSE if the end of the #GHashTable has been reached.
 *
 * Since: 2.16
 */
gboolean
g_hash_table_iter_next (GHashTableIter *iter,
                        gpointer       *key,
                        gpointer       *value)
{
  RealIter *ri = (RealIter *) iter;
  gint position;

  g_return_val_if_fail (iter != NULL, FALSE);
#ifndef G_DISABLE_ASSERT
  g_return_val_if_fail (ri->version == ri->hash_table->version, FALSE);
#endif
  g_return_val_if_fail (ri->position < ri->hash_table->size, FALSE);

  position = ri->position;
  struct entry * cur_entry = ri->current_entry;

  do{
      if (cur_entry == NULL){ // end of a list, increase position
        position++;
        if (position >= ri->hash_table->size){
            ri->position = position;
            ri->current_entry = NULL;
            return FALSE;
        }else{
          cur_entry = ri->hash_table->table[position];
        } 
      
      }else { // not the end of list, go to next.
        cur_entry = cur_entry->next;
      }
  } while (cur_entry == NULL);

  if (key != NULL)
    *key = cur_entry -> k;
  if (value != NULL)
    *value = cur_entry -> v;

  ri->position = position;
  ri->current_entry = cur_entry;

  return TRUE;
}


/**
 * g_hash_table_lookup:
 * @hash_table: a #GHashTable
 * @key: the key to look up
 *
 * Looks up a key in a #GHashTable. Note that this function cannot
 * distinguish between a key that is not present and one which is present
 * and has the value %NULL. If you need this distinction, use
 * g_hash_table_lookup_extended().
 *
 * Returns: (nullable): the associated value, or %NULL if the key is not found
 */

gpointer g_hash_table_lookup (GHashTable *hash_table, gconstpointer key){

  gpointer ret;

  // DBG_START;

  ret = hashtable_search(hash_table, key);

  // DBG_DONE;

  return ret;
}
