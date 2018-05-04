

#ifndef TINY_MEMORY_CACHE_H
#define TINY_MEMORY_CACHE_H

/** 
*	memory cache
*
*/

#include "tiny_private.h"


// //
// // Physical memory cache 

typedef int64_t *mem_key_t;


// struct mem_cache_entry {
//     addr_t paddr;
//     uint32_t length;
//     time_t last_updated;
//     time_t last_used;
//     void *data;
// };
// typedef struct mem_cache_entry *mem_cache_entry_t;

// struct mem_cache_line{
// 	mem_key_t key;
// 	mem_cache_entry_t entry;
// };
// typedef struct mem_cache_line *mem_cache_line_t;

// struct mem_cache_array{
// 	int count;
//     // int capacity;
// 	struct mem_cache_line **cache_lines;
// };

// typedef struct mem_cache_array *mem_cache_t;



// // implementation

// /**
//  * memory cache lines 
//  *  
//  */

// static inline mem_cache_t create_new_mem_cache(int capacity);

// status_t mem_cache_remove(mem_cache_t mem_cache, mem_key_t key);

// mem_cache_entry_t mem_cache_lookup(mem_cache_t mem_cache,mem_key_t key);

// int mem_cache_size(mem_cache_t mem_cache);

// mem_cache_t mem_cache_increase_capacity(mem_cache_t mem_c, int factor);

// status_t mem_cache_insert(mem_cache_t mem_cache,mem_key_t key,mem_cache_entry_t entry);					  



/**
 * LRU list 
 * list for most recent pages
 */

struct tiny_list_node{
    // struct tiny_list_node *previous;
	// struct tiny_list_node *next;
    int next;
    int previous;
	mem_key_t data;
};

typedef struct tiny_list_node *tiny_list_node_t;


struct tiny_list{  // TODO: implement as queue
	int size;
    int capacity;
    tiny_list_node_t *nodes;
	int head;   // index of head
	int tail;   // index of tail
	// tiny_list_node_t head;
	// tiny_list_node_t tail;
};

typedef struct tiny_list *tiny_list_t;

tiny_list_t create_new_list(int capacity);

status_t tiny_list_free(tiny_list_t list);

tiny_list_node_t tiny_list_last(tiny_list_t mem_cache_lru);

// status_t tiny_list_foreach(tiny_list_t list,void * remove_entry,mem_key_t mem_k);

tiny_list_node_t tiny_list_find_custom(tiny_list_t lru, mem_key_t paddr);

// remove the ele from the list, but don't free the ele
void tiny_list_unlink(tiny_list_t list, tiny_list_node_t node);

// put an existing node to head.
void tiny_list_push_head_link(tiny_list_t list, tiny_list_node_t node);

// static inline tiny_list_t tiny_list_remove_link(tiny_list_t lru, tiny_list_node_t last);

tiny_list_t tiny_list_remove(tiny_list_t list,mem_key_t key);

// pop last element, return its value. don't free the value
gpointer tiny_list_pop_last(tiny_list_t list);


// remove the element totally.
tiny_list_t tiny_list_remove_last(tiny_list_t list);

tiny_list_t tiny_list_increase_capacity(tiny_list_t list, int factor);

tiny_list_t tiny_list_prepend(tiny_list_t list, mem_key_t key);



/**
 * initial the memory cache and lru list
*/
// void memory_cache_init(vmi_instance_t vmi,unsigned long age_limit);

// void memory_cache_init(
//     vmi_instance_t vmi,
//     void *(*get_data) (vmi_instance_t,
//                        addr_t,
//                        uint32_t),
//     void (*release_data) (void *,
//                           size_t),
//     unsigned long age_limit);


// void *memory_cache_insert(vmi_instance_t vmi, addr_t paddr);

// void memory_cache_remove(
//     vmi_instance_t vmi,
//     addr_t paddr);
    
// void memory_cache_destroy(vmi_instance_t vmi);
// //


void memory_cache_init(
    vmi_instance_t vmi,
    void *(*get_data) (vmi_instance_t,
                       addr_t,
                       uint32_t),
    void (*release_data) (void *,
                          size_t),
    unsigned long age_limit);

void *memory_cache_insert(
    vmi_instance_t vmi,
    addr_t paddr);

void memory_cache_remove(
    vmi_instance_t vmi,
    addr_t paddr);

void memory_cache_destroy(
    vmi_instance_t vmi);


#endif // TINY_MEMORY_CACHE_H