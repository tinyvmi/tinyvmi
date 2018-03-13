
#ifndef TINY_CACHE_H
#define TINY_CACHE_H
//#include "tiny_private.h"
// #include <tiny_glib.h>
#include <tiny_glib/hash.h>
#include <tiny_glib/hashtable.h>
#include <tiny_glib/slist.h>

/**for cache, lele 2014.11.14 */

// typedef void* gpointer;
// typedef const void *gconstpointer;

#define NOOP

#define MAX_V2P_CACHE 1280
#define MAX_MEM_CACHE 1280

/* Custom 128-bit key functions */
struct key_128 {
    uint64_t low;
    uint64_t high;
};
typedef struct key_128 *key_128_t;

//
// Virtual address --> Physical address cache implementation
struct v2p_cache_entry {
    addr_t pa;
    addr_t last_used;
};

typedef struct v2p_cache_entry *v2p_cache_entry_t;

struct v2p_cache_line{
	key_128_t key;
	v2p_cache_entry_t entry;
};

typedef struct v2p_cache_line *v2p_cache_line_t;

struct v2p_cache_array{
	int count;
	struct v2p_cache_line **cache_lines;
};

typedef struct v2p_cache_array *v2p_cache_t;




/* 
 *
 */
 
// Those function are implemented as an array for the simplicity.
v2p_cache_entry_t tiny_cache_lookup(v2p_cache_t v2p_c,key_128_t  key);
status_t tiny_cache_insert(v2p_cache_t v2p_c, key_128_t  key,v2p_cache_entry_t entry);
status_t tiny_cache_remove(v2p_cache_t v2p_c,key_128_t key);
void tiny_cache_remove_all(v2p_cache_t v2p_c);
void tiny_cache_destroy(v2p_cache_t v2p_c);

// This function borrowed from cityhash-1.0.3
 int key_128_equals( key_128_t key1,  key_128_t key2);

/*
 * Initialize an already allocated key with the given values.
 * This is for performance!
 */
void key_128_init(vmi_instance_t vmi, key_128_t key, uint64_t low, uint64_t high);

key_128_t key_128_build (vmi_instance_t vmi, uint64_t low, uint64_t high);


#if ENABLE_ADDRESS_CACHE == 1

/** TODO: pid cache, to be implemented ****/
void pid_cache_init(vmi_instance_t vmi);
void pid_cache_destroy(vmi_instance_t vmi);
void pid_cache_set(vmi_instance_t vmi, vmi_pid_t pid, addr_t dtb);
void pid_cache_flush(vmi_instance_t vmi);
status_t pid_cache_get(vmi_instance_t vmi, vmi_pid_t pid, addr_t *dtb);
status_t pid_cache_del(vmi_instance_t vmi, vmi_pid_t pid);


/** TODO: sym cache, to be implemented ****/
void sym_cache_init(vmi_instance_t vmi);
void sym_cache_destroy(vmi_instance_t vmi);
void sym_cache_set(vmi_instance_t vmi, addr_t base_addr, vmi_pid_t pid, const char *sym, addr_t va);
void sym_cache_flush(vmi_instance_t vmi);
status_t sym_cache_get(vmi_instance_t vmi, addr_t base_addr, vmi_pid_t pid, const char *sym, addr_t *va);
status_t sym_cache_del(vmi_instance_t vmi, addr_t base_addr, vmi_pid_t pid, char *sym);


/** TODO: rva cache to be implemented ****/
void rva_cache_init(vmi_instance_t vmi);
void rva_cache_destroy(vmi_instance_t vmi);
void rva_cache_set(vmi_instance_t vmi, addr_t base_addr, addr_t dtb, addr_t rva, char *sym);
void rva_cache_flush(vmi_instance_t vmi);
status_t rva_cache_get(vmi_instance_t vmi, addr_t base_addr, addr_t dtb, addr_t rva, char **sym);
status_t rva_cache_del(vmi_instance_t vmi, addr_t base_addr, addr_t dtb, addr_t rva);

#else

#define pid_cache_init(...)     NOOP
#define pid_cache_destroy(...)  NOOP
#define pid_cache_set(...)      NOOP
#define pid_cache_flush(...)    NOOP
#define pid_cache_get(...) VMI_FAILURE
#define pid_cache_del(...) VMI_FAILURE

#define sym_cache_init(...)     NOOP
#define sym_cache_destroy(...)  NOOP
#define sym_cache_set(...)      NOOP
#define sym_cache_flush(...)    NOOP
#define sym_cache_get(...) VMI_FAILURE
#define sym_cache_del(...) VMI_FAILURE

#define rva_cache_init(...)     NOOP
#define rva_cache_destroy(...)  NOOP
#define rva_cache_set(...)      NOOP
#define rva_cache_flush(...)    NOOP
#define rva_cache_get(...) VMI_FAILURE
#define rva_cache_del(...) VMI_FAILURE

#endif

#if ENABLE_V2P_CACHE == 1 

void v2p_cache_init(vmi_instance_t vmi);
void v2p_cache_destroy(vmi_instance_t vmi);
void v2p_cache_set(vmi_instance_t vmi, addr_t va, addr_t dtb, addr_t pa);
void v2p_cache_flush(vmi_instance_t vmi, addr_t dtb);
status_t v2p_cache_get(vmi_instance_t vmi, addr_t va, addr_t dtb, addr_t *pa);
status_t v2p_cache_del(vmi_instance_t vmi, addr_t va, addr_t dtb);

v2p_cache_entry_t v2p_cache_entry_create (vmi_instance_t vmi, addr_t pa);

// Below are wrapper functions for external API access to the cache

void vmi_v2pcache_add(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb,
    addr_t pa);

void vmi_v2pcache_flush(
    vmi_instance_t vmi, addr_t dtb);


#else

#define v2p_cache_init(...)     NOOP
#define v2p_cache_destroy(...)  NOOP
#define v2p_cache_set(...)      NOOP
#define v2p_cache_flush(...)    NOOP
#define v2p_cache_get(...) VMI_FAILURE
#define v2p_cache_del(...) VMI_FAILURE

#endif

#if ENABLE_SHM_SNAPSHOT == 1 && ENABLE_ADDRESS_CACHE == 1

void v2m_cache_init(vmi_instance_t vmi);
void v2m_cache_destroy(vmi_instance_t vmi);
void v2m_cache_set(vmi_instance_t vmi, addr_t va, pid_t pid, addr_t ma, uint64_t length);
void v2m_cache_flush(vmi_instance_t vmi);
status_t v2m_cache_get(vmi_instance_t vmi, addr_t va, pid_t pid, addr_t *ma, uint64_t *length);
status_t v2m_cache_del(vmi_instance_t vmi, addr_t va, pid_t pid);

#else

#define v2m_cache_init(...)     NOOP
#define v2m_cache_destroy(...)  NOOP
#define v2m_cache_set(...)      NOOP
#define v2m_cache_flush(...)    NOOP
#define v2m_cache_get(...) VMI_FAILURE
#define v2m_cache_del(...) VMI_FAILURE

#endif

	
/** 
*	memory cache
*
*/


// //
// // Physical memory cache 

typedef int64_t *mem_key_t;

struct tiny_list_node{
	struct tiny_list_node *next;
	mem_key_t data;
};

typedef struct tiny_list_node *tiny_list_node_t;


struct tiny_list{
	int size;
	tiny_list_node_t head;
	tiny_list_node_t tail;
};
typedef struct tiny_list *tiny_list_t;


struct mem_cache_entry {
    addr_t paddr;
    uint32_t length;
    time_t last_updated;
    time_t last_used;
    void *data;
};
typedef struct mem_cache_entry *mem_cache_entry_t;

struct mem_cache_line{
	mem_key_t key;
	mem_cache_entry_t entry;
};
typedef struct mem_cache_line *mem_cache_line_t;

struct mem_cache_array{
	int count;
	struct mem_cache_line **cache_lines;
};

typedef struct mem_cache_array *mem_cache_t;



// implementation

//memory cache lines
status_t mem_cache_remove(mem_cache_t mem_cache, mem_key_t key);

mem_cache_entry_t mem_cache_lookup(mem_cache_t mem_cache,mem_key_t key);

int mem_cache_size(mem_cache_t mem_cache);
		
status_t mem_cache_insert(mem_cache_t mem_cache,mem_key_t key,mem_cache_entry_t entry);					  


//list for most recent pages
tiny_list_node_t tiny_list_last(tiny_list_t mem_cache_lru);

status_t tiny_list_foreach(tiny_list_t list,void * remove_entry,mem_cache_t mem_c);

status_t tiny_list_free(tiny_list_t list);

tiny_list_node_t tiny_list_find_custom(tiny_list_t lru, mem_key_t paddr);

tiny_list_t tiny_list_remove_link(tiny_list_t lru, tiny_list_node_t last);

tiny_list_t tiny_list_remove(tiny_list_t list,mem_key_t key);
tiny_list_t tiny_list_prepend(tiny_list_t list,mem_key_t key);

// void memory_cache_init(vmi_instance_t vmi,unsigned long age_limit);

void memory_cache_init(
    vmi_instance_t vmi,
    void *(*get_data) (vmi_instance_t,
                       addr_t,
                       uint32_t),
    void (*release_data) (void *,
                          size_t),
    unsigned long age_limit);


void *memory_cache_insert(vmi_instance_t vmi, addr_t paddr);

void memory_cache_remove(
    vmi_instance_t vmi,
    addr_t paddr);
    
void memory_cache_destroy(vmi_instance_t vmi);
//





#endif // TINY_CACHE_H