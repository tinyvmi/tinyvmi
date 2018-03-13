

#ifndef HASH_H
#define HASH_H


#include "hashtable.h"
#include "types.h"


/* Hash Functions
 */
gboolean g_str_equal    (gconstpointer  v1,
                         gconstpointer  v2);
                         
guint    g_str_hash     (gconstpointer  v);


gboolean g_int_equal    (gconstpointer  v1,
                         gconstpointer  v2);

guint    g_int_hash     (gconstpointer  v);


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
                      gpointer    user_data);




/* Hash Tables */

#define HASH_TABLE_MINI_SIZE 1024
#define HASH_TABLE_FREE_VALUES 1 // always free values

// GHashTable* g_hash_table_new               (GHashFunc       hash_func,
//                                             GEqualFunc      key_equal_func);
// GLIB_AVAILABLE_IN_ALL
// GHashTable* g_hash_table_new_full          (GHashFunc       hash_func,
//                                             GEqualFunc      key_equal_func,
//                                             GDestroyNotify  key_destroy_func,
//                                             GDestroyNotify  value_destroy_func);
#define g_hash_table_new_full(hash_func, key_equal_func, key_destroy_func, value_destroy_func) \
create_hashtable(HASH_TABLE_MINI_SIZE,hash_func, key_equal_func, key_destroy_func, value_destroy_func)

// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_destroy           (GHashTable     *hash_table);


#define g_hash_table_destroy(hash_table) \
hashtable_destroy(hash_table, HASH_TABLE_FREE_VALUES) 


// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_insert            (GHashTable     *hash_table,
//                                             gpointer        key,
//                                             gpointer        value);

#define g_hash_table_insert(hash_table, key, value) \
hashtable_insert(hash_table, key, value)

// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_replace           (GHashTable     *hash_table,
//                                             gpointer        key,
//                                             gpointer        value);
// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_add               (GHashTable     *hash_table,
//                                             gpointer        key);
// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_remove            (GHashTable     *hash_table,
//                                             gconstpointer   key);
// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_remove_all        (GHashTable     *hash_table);
// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_steal             (GHashTable     *hash_table,
//                                             gconstpointer   key);
// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_steal_all         (GHashTable     *hash_table);
// GLIB_AVAILABLE_IN_ALL
// gpointer    g_hash_table_lookup            (GHashTable     *hash_table,
//                                             gconstpointer   key);

#define g_hash_table_lookup(hash_table, key) \
hashtable_search(hash_table, key)

// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_contains          (GHashTable     *hash_table,
//                                             gconstpointer   key);
// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_lookup_extended   (GHashTable     *hash_table,
//                                             gconstpointer   lookup_key,
//                                             gpointer       *orig_key,
//                                             gpointer       *value);
// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_foreach           (GHashTable     *hash_table,
//                                             GHFunc          func,
//                                             gpointer        user_data);
// GLIB_AVAILABLE_IN_ALL
// gpointer    g_hash_table_find              (GHashTable     *hash_table,
//                                             GHRFunc         predicate,
//                                             gpointer        user_data);
// GLIB_AVAILABLE_IN_ALL
// guint       g_hash_table_foreach_remove    (GHashTable     *hash_table,
//                                             GHRFunc         func,
//                                             gpointer        user_data);
// GLIB_AVAILABLE_IN_ALL
// guint       g_hash_table_foreach_steal     (GHashTable     *hash_table,
//                                             GHRFunc         func,
//                                             gpointer        user_data);
// GLIB_AVAILABLE_IN_ALL
// guint       g_hash_table_size              (GHashTable     *hash_table);
// GLIB_AVAILABLE_IN_ALL
// GList *     g_hash_table_get_keys          (GHashTable     *hash_table);
// GLIB_AVAILABLE_IN_ALL
// GList *     g_hash_table_get_values        (GHashTable     *hash_table);
// GLIB_AVAILABLE_IN_2_40
// gpointer *  g_hash_table_get_keys_as_array (GHashTable     *hash_table,
//                                             guint          *length);

// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_iter_init         (GHashTableIter *iter,
//                                             GHashTable     *hash_table);
// GLIB_AVAILABLE_IN_ALL
// gboolean    g_hash_table_iter_next         (GHashTableIter *iter,
//                                             gpointer       *key,
//                                             gpointer       *value);
// GLIB_AVAILABLE_IN_ALL
// GHashTable* g_hash_table_iter_get_hash_table (GHashTableIter *iter);
// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_iter_remove       (GHashTableIter *iter);
// GLIB_AVAILABLE_IN_2_30
// void        g_hash_table_iter_replace      (GHashTableIter *iter,
//                                             gpointer        value);
// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_iter_steal        (GHashTableIter *iter);

// GLIB_AVAILABLE_IN_ALL
// GHashTable* g_hash_table_ref               (GHashTable     *hash_table);
// GLIB_AVAILABLE_IN_ALL
// void        g_hash_table_unref             (GHashTable     *hash_table);

// #ifndef G_DISABLE_DEPRECATED
// #define g_hash_table_freeze(hash_table) ((void)0)
// #define g_hash_table_thaw(hash_table) ((void)0)



#endif /* __G_HASH_H__ */
