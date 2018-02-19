
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

static unsigned int
g_int64_hash(
    void * v)
{
    return (unsigned int) * (const int64_t *) v;
}

static int
g_int64_equal(
    void* v1,
    void* v2)
{
    return *((const int64_t *) v1) == *((const int64_t *) v2);
}

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
		return VMI_SUCCESS;
	}else{
		printf("ERROR: mem cache out of bound(%d). in %s\n",mem_c->count,__FUNCTION__);
		return VMI_FAILURE;
	}
}


//list for most recent pages
tiny_list_node_t tiny_list_last(tiny_list_t list){
 return list->tail;
}

status_t tiny_list_foreach(tiny_list_t list,void * remove_entry, mem_cache_t memory_cache){

}
status_t tiny_list_free(tiny_list_t list){

	tiny_list_node_t node=list->head;
	tiny_list_node_t next=list->head;
	list->head=NULL;
	
	while(node){
		next=node->next;
		free(node->data);
		free(node);
		node=next;
	}
	return VMI_SUCCESS;
}

tiny_list_node_t tiny_list_find_custom(tiny_list_t list,
                mem_key_t key){
				
	tiny_list_node_t node=list->head;	
	while(node){
		if((*(node->data))==(*key)){
			return node;
		}
		node=node->next;
	}
	return NULL;
}

tiny_list_t tiny_list_remove_link(tiny_list_t lru, tiny_list_node_t last){

	return tiny_list_remove(lru,last->data);
}

tiny_list_t tiny_list_remove(tiny_list_t list,mem_key_t key){

	tiny_list_node_t node=list->head;
	tiny_list_node_t pre=list->head;
	if((*node->data)==(*key)){
		list->head=node->next;
		free(node->data);
		free(node);
		return list;
	}
	while(node){
		if((*(node->data))==(*key)){
			pre->next=node->next;
			free(node->data);
			free(node);
			if(pre->next==NULL){
				list->tail=pre;
			}
			return list;
		}
		pre=node;
		node=node->next;
	}
	return list;
}

tiny_list_t tiny_list_prepend(tiny_list_t list,mem_key_t key){

	//printf("-in %s\n",__FUNCTION__);
	
	tiny_list_node_t new_node=(tiny_list_node_t) malloc(sizeof(struct tiny_list_node));
	
	//printf("-- allocate tiny_list_node, in %s\n",__FUNCTION__);
	
	new_node->data=key;
	
	//printf("-- set node data=0x%lx, in %s\n",*key,__FUNCTION__);
	
	new_node->next=list->head;
	
	//printf("-- set head to next node, in %s\n",__FUNCTION__);
	
	if(list->head==NULL){
	//	printf("-- set tail and head to new_node, in %s\n",__FUNCTION__);
		list->tail=new_node;
		list->head=new_node;
	}else{
	//	printf("-- set head, in %s\n",__FUNCTION__);
		list->head=new_node;
	}
	list->size++;
	
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
    while (vmi->memory_cache_size > vmi->memory_cache_size_max / 2) {
        mem_key_t key = NULL;
        struct tiny_list_node *last = tiny_list_last(vmi->memory_cache_lru);

        key = last->data;
        list = tiny_list_prepend(list, key);
        vmi->memory_cache_lru =
            tiny_list_remove_link(vmi->memory_cache_lru, last);
        vmi->memory_cache_size--;
    }
    tiny_list_foreach(list, remove_entry, vmi->memory_cache);
   
    printf("--MEMORY cache cleanup round complete (cache size = %u)\n",
            mem_cache_size(vmi->memory_cache));*/
	tiny_list_free(vmi->memory_cache_lru);
	mem_cache_t mem_c=vmi->memory_cache;
	for(i=0;i<mem_c->count;i++){
		printf("---searching the key:%d<paddr>=<0x%lx>, in %s\n",i,mem_c->cache_lines[i]->key,__FUNCTION__);
		free(mem_c->cache_lines[i]->key);
		
		mem_cache_entry_free(mem_c->cache_lines[i]->entry);
		
		free(mem_c->cache_lines[i]->entry);
		free(mem_c->cache_lines[i]);
		
	}
			
    printf("--MEMORY cache cleanup round complete, in %s \n",__FUNCTION__);

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

    mem_cache_entry_t entry =
        (mem_cache_entry_t)
        malloc(sizeof(struct mem_cache_entry));

    entry->paddr = paddr;
    entry->length = length;
    entry->last_updated = time(NULL);
    entry->last_used = entry->last_updated;
    entry->data = get_memory_data(vmi, paddr, length);

    if (vmi->memory_cache_size >= vmi->memory_cache_size_max) {
        clean_cache(vmi);
    }

    return entry;
}



void
memory_cache_init(
    vmi_instance_t vmi,unsigned long age_limit)
{
	printf("\nin %s\n",__FUNCTION__);
    vmi->memory_cache = (mem_cache_t) malloc(sizeof(struct mem_cache_array));
	
	mem_cache_line_t mem_cache_lines=(mem_cache_line_t *)malloc(MAX_MEM_CACHE*sizeof(mem_cache_line_t));
	
	vmi->memory_cache->cache_lines=mem_cache_lines;
	vmi->memory_cache->count=0;
	
    vmi->memory_cache_lru = (tiny_list_t)malloc(sizeof(struct tiny_list));
	vmi->memory_cache_lru->head=NULL;
	vmi->memory_cache_lru->tail=NULL;
	//if(vmi->memory_cache_lru->head==NULL)	printf("head=tail=NULL\n");
	//else printf("error, head not NULL\n");
    vmi->memory_cache_age = age_limit;
    vmi->memory_cache_size = 0;
    vmi->memory_cache_size_max = MAX_PAGE_CACHE_SIZE;
	
}




void *
memory_cache_insert(
    vmi_instance_t vmi,
    addr_t paddr)
{
    mem_cache_entry_t entry = NULL;
    addr_t paddr_aligned = paddr & ~(((addr_t) vmi->page_size) - 1);

    if (paddr != paddr_aligned) {
        printf("Memory cache request for non-aligned page\n");
        return NULL;
    }
	//printf("now in %s\n",__FUNCTION__);
    mem_key_t key = &paddr;
    if ((entry = mem_cache_lookup(vmi->memory_cache, key)) != NULL) {
        printf("--MEMORY cache hit 0x%"PRIx64"\n", paddr);
        return validate_and_return_data(vmi, entry);
    }
    else {
        printf("--MEMORY cache set 0x%"PRIx64"\n", paddr);

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
        vmi->memory_cache_size++;

        return entry->data;
    }
}

void
memory_cache_destroy(
    vmi_instance_t vmi)
{
    uint32_t tmp = vmi->memory_cache_size_max;

    vmi->memory_cache_size_max = 0;
    clean_cache(vmi);
    vmi->memory_cache_size_max = tmp;
}
