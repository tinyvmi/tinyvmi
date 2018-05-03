
#include "tiny_private.h"

#include "glib_compat.h"

 void * get_data_callback (
    vmi_instance_t vmi,
    addr_t paddr,
    uint32_t length){
	return xen_get_memory(vmi,paddr,length);
}

  void release_data_callback(
    void * data, size_t length){
	munmap(data,length);
 }

// static unsigned int
// g_int64_hash(
//     void * v)
// {
//     return (unsigned int) * (const int64_t *) v;
// }

// static int
// g_int64_equal(
//     void* v1,
//     void* v2)
// {
//     return *((const int64_t *) v1) == *((const int64_t *) v2);
// }

//---------------------------------------------------------
// Internal implementation functions

//memory cache lines
static int get_key_mem_index(mem_cache_t mem_c, mem_key_t key){
	int i,found=0;
	//printf("--searching the mem key:<paddr>=<0x%lx>, in %s\n",*key,__FUNCTION__);
	if(mem_c->count<=0){
	//	printf("--nothing in mem cache, in %s\n",__FUNCTION__);
		return -1;
	}
	
	for(i=0;i<mem_c->count;i++){
	//	printf("---searching the key:%d<paddr>=<0x%lx>, in %s\n",i,*(mem_c->cache_lines[i]->key),__FUNCTION__);
		if(g_int64_equal(mem_c->cache_lines[i]->key,key)){
	//	printf("---hit the key:%d<paddr>=<0x%lx>, in %s\n",i,*(mem_c->cache_lines[i]->key),__FUNCTION__);
			found=1;
			return i;
		}
	}
	if(!found) {
	
	//printf("---no key match 0x%lxin %s\n",*key,__FUNCTION__);
	return -1;}
}

status_t mem_cache_remove(mem_cache_t mem_c, mem_key_t key){
	int key_i,i;
	key_i=get_key_mem_index(mem_c,key);
	if(key_i>=0){
		free(mem_c->cache_lines[key_i]->entry->data);
		free(mem_c->cache_lines[key_i]->entry);
		free(mem_c->cache_lines[key_i]->key);
		free(mem_c->cache_lines[key_i]);
		for(i=key_i;i<mem_c->count;i++){
			mem_c->cache_lines[i]=mem_c->cache_lines[i+1];
		}
		mem_c->count--;
		return VMI_SUCCESS;
	}else{
		return VMI_FAILURE;
	}
}

mem_cache_entry_t mem_cache_lookup(mem_cache_t mem_c,mem_key_t key){
	
	int key_i;
	//printf("---searching the key:<paddr>=<0x%lx>, in %s\n",*key,__FUNCTION__);
	key_i=get_key_mem_index(mem_c,key);
	if(key_i<0) return NULL;
	return mem_c->cache_lines[key_i]->entry;
}

int mem_cache_size(mem_cache_t mem_c){
	return mem_c->count;
}

status_t mem_cache_insert(mem_cache_t mem_c,mem_key_t key,mem_cache_entry_t entry){
	 
     DBG_START;
     status_t res = VMI_FAILURE;

	 if(mem_c->count<MAX_MEM_CACHE){
	 
		//printf("---mem key set:line[%d],paddr=0x%lx, in %s\n",mem_c->count,*key,__FUNCTION__);
		mem_c->cache_lines[mem_c->count]=(mem_cache_line_t)malloc(sizeof(struct mem_cache_line));
		//printf("---mem key set:line[%d],allocated, in %s\n",mem_c->count,__FUNCTION__);
		
		mem_c->cache_lines[mem_c->count]->key=key;
		//printf("---mem key set:line[%d],set key=0x%lx, in %s\n",mem_c->count,*key,__FUNCTION__);
		mem_c->cache_lines[mem_c->count]->entry=entry;
		//printf("---mem key set:line[%d],set entry(paddr=0x%lx), in %s\n",mem_c->count,entry->paddr,__FUNCTION__);
		
		mem_c->count++;
		//printf("---mem set (count=%d), in %s\n",mem_c->count,__FUNCTION__);
		res = VMI_SUCCESS;
	}else{
        printf("TODO: need to find a way to remove the least used cache lines; in %s, cache size: %d; \n",
            mem_c->count,__FUNCTION__);
		errprint("ERROR: mem cache out of bound. ");

	}

    DBG_DONE;
    return res;

}


//list for most recent pages

tiny_list_t create_new_list(int capacity){
    
    tiny_list_t list = (tiny_list_t)malloc(sizeof(struct tiny_list));
    list->nodes = (tiny_list_node_t *)malloc(capacity * sizeof(tiny_list_node_t));
    list->capacity = capacity;
    list->head = -1;
    list->tail = -1;
    list->size = 0;

    return list;

}
tiny_list_node_t tiny_list_last(tiny_list_t list){
    if(list->tail == -1){
        errprint("tiny_list_last: ERROR, list has no element yet.");
    }
    return list->nodes[list->tail];
}

status_t tiny_list_foreach(tiny_list_t list,void * remove_entry, mem_cache_t memory_cache){

}

status_t tiny_list_free(tiny_list_t list){

    if (!list)
        return VMI_SUCCESS;

	int node_index = list->head;
    tiny_list_node_t node=list->nodes[node_index];

    int next_index ;
	tiny_list_node_t next;
	
	while(node_index != -1){

        next_index = node->next;
		next = list->nodes[next_index];

		free(node->data);
		free(node);

        list->size --;
        node_index = next_index;
        node = next;
	}
    
    list->head = -1;
    list->tail = -1;

    if(list->size != 0) {
        errprint("ERROR, list size should be 0 after clean. in %s", __FUNCTION__);
    }

	return VMI_SUCCESS;
}

tiny_list_node_t tiny_list_find_custom(tiny_list_t list,
                mem_key_t key){
				
    int node_index = list->head;
	tiny_list_node_t node;	
    
	while(node_index != -1){
        node = list->nodes[node_index];

		if((*(node->data))==(*key)){
			return node;
		}
		node_index = node->next;
	}
	return NULL;
}

/**
 * remove an existing node in the list
 * node must be in the list, if not, will be undefined behavior.
*/

static inline tiny_list_t tiny_list_remove_link(tiny_list_t list, tiny_list_node_t node){

    // remove it from list:

    // first, get its next node and pre node
    int next_node = node->next;
    int pre_node = node->previous;

    // if next_node and pre_node are both -1, then remove the only node
    if (pre_node == -1 && next_node == -1){
        list->tail = -1;
        list->head = -1;
    }

    // if next_node is -1, then the node to remove is tail
    // need to reset tail to previous node
    else if (next_node == -1){
        list->tail = pre_node;
        list->nodes[pre_node]->next = -1;
    }
    
    // if pre_node is -1, then the node to remove is head
    // need to reset head to next node
    else if (pre_node == -1){
        list->head = next_node;
        list->nodes[next_node] -> previous = -1; 
    }

    // otherwise, is internal nodes, remove it and connect two neibougher
    else{
        list->nodes[pre_node]->next = next_node;
        list->nodes[next_node]->previous = pre_node;
    }

    free(node->data);
    free(node);

    list->size --;

    return list;

}

/*
*  remove the key in the list
*   - if key not found, do nothing to list
*   - return the new list
*/
tiny_list_t tiny_list_remove(tiny_list_t list,mem_key_t key){

	// tiny_list_node_t node=list->head;
	// tiny_list_node_t pre=list->head;
    
    //tiny_list_node_t node = list->nodes[list->head];
    tiny_list_node_t node;

    int node_index = list->head;

	while(node_index != -1){

        node = list->nodes[node_index];

		if((*(node->data))==(*key)){
            // found the key
            return tiny_list_remove_link(list,node);
		}
        // key not found, go to next
		node_index = node->next;
	}
	return list;
}

tiny_list_t tiny_list_remove_last(tiny_list_t list){

    int old_tail = list->tail;

    if (old_tail == -1){
        errprint("ERROR: no tail node yet");
    }

    tiny_list_node_t old_last_node = list->nodes[old_tail];

    if(old_last_node->next != -1){
        printf("%s\n", __FUNCTION__);
        errprint("ERROR: tail's next should be -1");
    }

    int new_tail = old_last_node->previous;

    //maintain the list struct with new tail and old tail
    free(old_last_node->data);
    free(old_last_node);
    list->size --;

    list->tail = new_tail;
    if(new_tail != -1) {
        list->nodes[new_tail] -> next = -1;
    }
    
	return list;
}

tiny_list_t tiny_list_increase_capacity(tiny_list_t list, int factor){

    int i = 0;

    int old_capacity = list->capacity;
    int new_capacity = list->capacity * factor;

    // malloc the space of new list
    tiny_list_node_t * new_list_nodes = (tiny_list_node_t *) malloc (new_capacity * sizeof (tiny_list_node_t));

    // copy the old list to new list, free the old list  nodes
    // copy the whole array to new array:
    for (i=0; i < old_capacity; i++){
        new_list_nodes[i] = list->nodes[i];
    }

    list->capacity = new_capacity;

    free(list->nodes);
    list->nodes = new_list_nodes;

	return list;
}

tiny_list_t tiny_list_prepend(tiny_list_t list,mem_key_t key){

    DBG_START;
	
	tiny_list_node_t new_node=(tiny_list_node_t) malloc(sizeof(struct tiny_list_node));
	
	//printf("-- allocate tiny_list_node, in %s\n",__FUNCTION__);
	
	new_node->data=key;
	
    // since it will be the head, the pre should be -1 (null)
    new_node->previous = -1; //important: check this for list head.

	//printf("-- set node data=0x%lx, in %s\n",*key,__FUNCTION__);
	
	list->size++;

	if (list->size > list->capacity) {  
        // we have more nodes than the capacity of the queue
        // either:
        //      - replace tail with new node, set head to it
        // or:
        //      - create a bigger queue with 2X capacity

        list = tiny_list_remove_last(list);

        // TODO: fixme: increase capacity when necessary.
        // list = tiny_list_increase_capacity(list, 2);
    }

    // now insert the node to head
	if(list->size == 1){ // newnode is now the only node on list.
		printf("-- set tail and head to new_node, in %s\n",__FUNCTION__);
	    list->head = 0;
        list->tail = 0;

        new_node->next = -1;
        list->nodes[0] = new_node;

	}else {
        // we have free space, insert one node and set head to new node
	    printf("-- set head, in %s\n",__FUNCTION__);

        // move forward the head index
        int old_head = list->head;
        list->head = (list->head + 1) % list->capacity;

        if (list->head == list-> tail){
            errprint("ERROR: list->head == list->tail, reach max capacity? should not happend here\n");
        }

        // maintain the data structure for old head and new head:

        list->nodes[old_head]->previous = list->head;
        
        new_node->next = old_head;
        new_node->previous = -1;
		list->nodes[list->head] = new_node;

	}
    
	DBG_DONE;

	return list;
}


//implementation
			
 void
mem_cache_entry_free(
    gpointer data)
{
    mem_cache_entry_t entry = (mem_cache_entry_t) data;

    if (entry) {
        release_data_callback(entry->data, entry->length);
        free(entry);
    }
}

 void *
get_memory_data(
    vmi_instance_t vmi,
    addr_t paddr,
    uint32_t length)
{
    return get_data_callback(vmi, paddr, length);
}

 void
remove_entry(
    mem_key_t key,
    mem_cache_t cache)
{
    mem_cache_t mem_cache = cache;

    mem_cache_remove(mem_cache, key);
	
    free(key);
}

 void
clean_cache(
    vmi_instance_t vmi)
{
	int i;
/*
     while (g_queue_get_length(vmi->memory_cache_lru) > vmi->memory_cache_size_max / 2) {
        gint64 *paddr = g_queue_pop_tail(vmi->memory_cache_lru);

        g_hash_table_remove(vmi->memory_cache, paddr);
        g_free(paddr);
    }

    dbprint(VMI_DEBUG_MEMCACHE, "--MEMORY cache cleanup round complete (cache size = %u)\n",
            g_hash_table_size(vmi->memory_cache));
            
*/

    dbprint(VMI_DEBUG_MEMCACHE,"->-> now in %s\n", __FUNCTION__);

    if (vmi->memory_cache_lru){

    	tiny_list_free(vmi->memory_cache_lru);
        dbprint(VMI_DEBUG_MEMCACHE,"%s: tiny list cleared\n", __FUNCTION__);
    }else{
        
        dbprint(VMI_DEBUG_MEMCACHE,"%s: tiny list is empty.\n", __FUNCTION__);
    }
    
	mem_cache_t mem_c=vmi->memory_cache;

    if (!mem_c){
        dbprint(VMI_DEBUG_MEMCACHE,"%s: mem cache is empty\n", __FUNCTION__);
    }else{

        for(i=0;i<mem_c->count;i++){
            dbprint(VMI_DEBUG_MEMCACHE,"---searching the key:%d<paddr>=<0x%lx>, in %s\n",i,mem_c->cache_lines[i]->key,__FUNCTION__);
            free(mem_c->cache_lines[i]->key);
            
            mem_cache_entry_free(mem_c->cache_lines[i]->entry);
            
            //free(mem_c->cache_lines[i]->entry);
            free(mem_c->cache_lines[i]);
            
        }
        dbprint(VMI_DEBUG_MEMCACHE,"%s: mem cache cleared\n", __FUNCTION__);
   
    }             
    dbprint(VMI_DEBUG_MEMCACHE,"--MEMORY cache cleanup round complete, in %s \n",__FUNCTION__);

}

 void *
validate_and_return_data(
    vmi_instance_t vmi,
    mem_cache_entry_t entry)
{
    time_t now = time(NULL);

    if (vmi->memory_cache_age &&
        (now - entry->last_updated > vmi->memory_cache_age)) {
        printf("--MEMORY cache refresh 0x%"PRIx64"\n", entry->paddr);
        release_data_callback(entry->data, entry->length);
        entry->data = get_memory_data(vmi, entry->paddr, entry->length);
        entry->last_updated = now;

        struct tiny_list_node* lru_entry = tiny_list_find_custom(vmi->memory_cache_lru,
                &entry->paddr);
        mem_key_t* key = lru_entry->data;
        vmi->memory_cache_lru = tiny_list_remove(vmi->memory_cache_lru, key);
        vmi->memory_cache_lru = tiny_list_prepend(vmi->memory_cache_lru, key);
    }
    entry->last_used = now;
    return entry->data;
}

mem_cache_entry_t create_new_entry (vmi_instance_t vmi, addr_t paddr,
        uint32_t length)
{

    // sanity check - are we getting memory outside of the physical memory range?
    // 
    // This does not work with a Xen PV VM during page table lookups, because
    // cr3 > [physical memory size]. It *might* not work when examining a PV
    // snapshot, since we're not sure where the page tables end up. So, we
    // just do it for a HVM guest.
    //
    // TODO: perform other reasonable checks

   // if (vmi->hvm && (paddr + length - 1 > vmi->size)) {
   //     printf("--requesting PA [0x%"PRIx64"] beyond memsize [0x%"PRIx64"]\n",
   //             paddr + length, vmi->size);
   //     printf("\tpaddr: %"PRIx64", length %"PRIx32", vmi->size %"PRIx64"\n", paddr, length,
   //             vmi->size);
   //     return 0;
   // }
    dbprint(VMI_DEBUG_MEMCACHE, "->-> now in %s\n", __FUNCTION__);

    mem_cache_entry_t entry =
        (mem_cache_entry_t)
        malloc(sizeof(struct mem_cache_entry));

    entry->paddr = paddr;
    entry->length = length;
    entry->last_updated = time(NULL);
    entry->last_used = entry->last_updated;
    entry->data = get_memory_data(vmi, paddr, length);


    // TODO: fixme
    // should not clean cache. 
    // the cache should clean itself if found itself full of capacity when inserting new lines.

    if (vmi->memory_cache_lru -> size >= vmi->memory_cache_size_max) {
        dbprint(VMI_DEBUG_MEMCACHE, "%s: cache size more than max(%d), now clean it\n", __FUNCTION__, vmi->memory_cache_size_max);
        clean_cache(vmi);
    }

    dbprint(VMI_DEBUG_MEMCACHE, "%s: done\n\n", __FUNCTION__);
    
    return entry;
}

	
//---------------------------------------------------------
// External API functions


// void
// memory_cache_init(
//     vmi_instance_t vmi,unsigned long age_limit)
// {
void
memory_cache_init(
    vmi_instance_t vmi,
    void *(*get_data) (vmi_instance_t,
                       addr_t,
                       uint32_t),
    void (*release_data) (void *,
                          size_t),
    unsigned long age_limit) {
    
	///////////////////////////
	// the old tinyvmi with libvmi-0.10.1
	printf("\nin %s\n",__FUNCTION__);

    if(!vmi){
        errprint("%s: Error: no vmi instance\n", __FUNCTION__);
        return ;
    }
    
    if (vmi->memory_cache){
        dbprint(VMI_DEBUG_MEMCACHE, "%s: WARNING: memory_cache exists before init\n", __FUNCTION__);
        free(vmi->memory_cache);
    }

    vmi->memory_cache = (mem_cache_t) malloc(sizeof(struct mem_cache_array));
	
	mem_cache_line_t mem_cache_lines=(mem_cache_line_t *)malloc(MAX_MEM_CACHE*sizeof(mem_cache_line_t));
	
	vmi->memory_cache->cache_lines=mem_cache_lines;
	vmi->memory_cache->count=0;
	
    tiny_list_t list = create_new_list(INIT_SIZE_MEM_CACHE_LRU);


    vmi->memory_cache_lru = list; 
	vmi->memory_cache_lru->head=NULL;
	vmi->memory_cache_lru->tail=NULL;
	//if(vmi->memory_cache_lru->head==NULL)	printf("head=tail=NULL\n");
	//else printf("error, head not NULL\n");
    vmi->memory_cache_age = age_limit;
    // vmi->memory_cache_size = 0;
    vmi->memory_cache_size_max = MAX_PAGE_CACHE_SIZE;

	/////////////////////////////////
	// v v v libvmi-tkl

	// vmi->memory_cache =
    //     g_hash_table_new_full(g_int64_hash, g_int64_equal,
    //                           g_free,
                            //   memory_cache_entry_free);
    // vmi->memory_cache_lru = g_queue_new();
    // vmi->memory_cache_age = age_limit;
    // vmi->memory_cache_size_max = MAX_PAGE_CACHE_SIZE;
    vmi->get_data_callback = get_data;
    vmi->release_data_callback = release_data;

    dbprint(VMI_DEBUG_MEMCACHE, "done %s\n", __FUNCTION__);
}



void *
memory_cache_insert(
    vmi_instance_t vmi,
    addr_t paddr)
{
    DBG_START;
    mem_cache_entry_t entry = NULL;
    addr_t paddr_aligned = paddr & ~(((addr_t) vmi->page_size) - 1);

    if (paddr != paddr_aligned) {
        printf("Memory cache request for non-aligned page\n");
        return NULL;
    }
	//printf("->-> now in %s\n",__FUNCTION__);
    mem_key_t key = &paddr;
    if ((entry = mem_cache_lookup(vmi->memory_cache, key)) != NULL) {
        dbprint(VMI_DEBUG_MEMCACHE, "--MEMORY cache hit 0x%"PRIx64"\n", paddr);
        return validate_and_return_data(vmi, entry);
    } else {

        dbprint(VMI_DEBUG_MEMCACHE, "--MEMORY cache set 0x%"PRIx64"\n", paddr);

        entry = create_new_entry(vmi, paddr, vmi->page_size);
        if (!entry) {
            printf("create_new_entry failed\n");
            return 0;
        }//else{
         //   printf("create_new_entry for %lx,in %s\n",paddr,__FUNCTION__);
		//}

        key = (mem_key_t) malloc(sizeof(int64_t));
        *key = paddr;
		
        mem_cache_insert(vmi->memory_cache, key, entry);

        mem_key_t key2 =(mem_key_t) malloc(sizeof(int64_t));

        *key2 = paddr;
		
		//printf("---key2 for list=0x%lx, in %s\n",*key2,__FUNCTION__);
        vmi->memory_cache_lru = tiny_list_prepend(vmi->memory_cache_lru, key2);
        // vmi->memory_cache_size++;
        
        dbprint(VMI_DEBUG_MEMCACHE, "--MEMORY cache set DONE\n");

        return entry->data;
    }
}


void memory_cache_remove(
    vmi_instance_t vmi,
    addr_t paddr)
{
    addr_t paddr_aligned = paddr & ~(((addr_t) vmi->page_size) - 1);

    if (paddr != paddr_aligned) {
        errprint("Memory cache request for non-aligned page\n");
        return;
    }

    // gint64 *key = (gint64*)&paddr;
    // g_hash_table_remove(vmi->memory_cache, key);

    mem_key_t key = (mem_key_t) & paddr;
    mem_cache_remove(vmi->memory_cache, key);
}

void
memory_cache_destroy(
    vmi_instance_t vmi)
{
    dbprint(VMI_DEBUG_MEMCACHE, "->-> now in %s\n", __FUNCTION__);

    uint32_t tmp = vmi->memory_cache_size_max;

    dbprint(VMI_DEBUG_MEMCACHE, "%s: set mem cache size max to 0\n", __FUNCTION__);
    vmi->memory_cache_size_max = 0;
    dbprint(VMI_DEBUG_MEMCACHE, "%s: clean cache\n", __FUNCTION__);
    clean_cache(vmi);
    dbprint(VMI_DEBUG_MEMCACHE, "%s: set back max cache size to %d\n", __FUNCTION__, tmp);
    vmi->memory_cache_size_max = tmp;

	//////////////////////////////
	// new libvmi-tkl:

    //  vmi->memory_cache_size_max = 0;

    // if (vmi->memory_cache_lru) {
    //     g_queue_foreach(vmi->memory_cache_lru, (GFunc)g_free, NULL);
    //     g_queue_free(vmi->memory_cache_lru);
    //     vmi->memory_cache_lru = NULL;
    // }

    // if (vmi->memory_cache) {
    //     g_hash_table_destroy(vmi->memory_cache);
    //     vmi->memory_cache = NULL;
    // }

    // vmi->memory_cache_age = 0;
    // vmi->memory_cache_size_max = 0;
    vmi->get_data_callback = NULL;
    vmi->release_data_callback = NULL;
	
}
