
#ifndef TINY_GLIB_H
#define TINY_GLIB_H

// #include <unistd.h>
#include <stdbool.h> 
// #include "tiny_private.h"

#define TRUE 1
#define FALSE 0

#define g_malloc0 malloc
#define g_malloc malloc
#define g_free free 

typedef bool gboolean;
typedef void* gpointer;
typedef const void *gconstpointer;

// typedef struct _GSList GSList;
// typedef struct tiny_list_node GSList;

// struct _GSList
// {
//   gpointer data;
//   GSList *next;
// };

typedef struct g_slist GSList;

typedef struct _GHashTable  GHashTable;


struct g_slist{
	GSList *next;
	gpointer data;
};


// struct _GHashTable
// {
//   gint             size;
//   gint             mod;
//   guint            mask;
//   gint             nnodes;
//   gint             noccupied;  /* nnodes + tombstones */

//   gpointer        *keys;
//   guint           *hashes;
//   gpointer        *values;

//   GHashFunc        hash_func;
//   GEqualFunc       key_equal_func;
//   gint             ref_count;
// #ifndef G_DISABLE_ASSERT
//   /*
//    * Tracks the structure of the hash table, not its contents: is only
//    * incremented when a node is added or removed (is not incremented
//    * when the key or data of a node is modified).
//    */
//   int              version;
// #endif
//   GDestroyNotify   key_destroy_func;
//   GDestroyNotify   value_destroy_func;
// };


//list for most recent pages
GSList* g_slist_last(GSList * mem_cache_lru);

// status_t g_list_foreach(GSList * list, void * remove_entry, mem_cache_t mem_c);

void g_slist_free(GSList * list);

GSList* g_slist_find_custom(GSList * lru, gpointer paddr);

GSList* g_slist_remove_link(GSList * lru, gpointer last);

GSList * g_slist_remove(GSList * list, gpointer key);
GSList * g_slist_prepend(GSList * list, gpointer key);





#endif //TINY_GLIB_H