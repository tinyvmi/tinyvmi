/* 
 *
 */

// Three kinds of cache:
//  1) PID --> DTB
//  2) Symbol --> Virtual address
//  3) Virtual address --> physical address

#include "tiny_private.h"

int key_128_equals(key_128_t key1, key_128_t key2){
    const key_128_t cache_key1 = (const key_128_t) key1;
    const key_128_t cache_key2 = (const key_128_t) key2;
    return cache_key1->low == cache_key2->low && cache_key1->high == cache_key2->high;
}

/*
 * Initialize an already allocated key with the given values.
 * This is for performance!
 */
void key_128_init(vmi_instance_t vmi, key_128_t key, uint64_t low, uint64_t high)
{
    low = (low & ~(vmi->page_size - 1));
    key->low = low;
    key->high = high;
}

key_128_t key_128_build (vmi_instance_t vmi, uint64_t low, uint64_t high)
{
    key_128_t key = (key_128_t) malloc(sizeof(struct key_128));
    key_128_init(vmi, key, low, high);
    return key;
}


//
// Virtual address --> Physical address cache implementation
static int get_key_index(v2p_cache_t v2p_c, key_128_t key){
	int i,found=0;
	//printf("---searching the key:<va,dtb>=<0x%lx,0x%lx>, in %s\n",key->low,key->high,__FUNCTION__);
	if(v2p_c->count<=0){
	//printf("---nothing in v2p cache, in %s\n",__FUNCTION__);
		return -1;
	}
	for(i=0;i<v2p_c->count;i++){
	//printf("---searching the key:%d<va,dtb>=<0x%lx,0x%lx>, in %s\n",i,v2p_c->cache_lines[i]->key->low,v2p_c->cache_lines[i]->key->high,__FUNCTION__);
		if(key_128_equals(v2p_c->cache_lines[i]->key,key)){
			found=1;
			return i;
		}
	}
	if(!found) return -1;
}

status_t tiny_cache_insert(v2p_cache_t v2p_c, key_128_t key,v2p_cache_entry_t entry){
	 if(v2p_c->count<MAX_V2P_CACHE){
	 
		v2p_c->cache_lines[v2p_c->count]=(v2p_cache_line_t)malloc(sizeof(struct v2p_cache_line));
		
		v2p_c->cache_lines[v2p_c->count]->key=key;
		v2p_c->cache_lines[v2p_c->count]->entry=entry;
		
		v2p_c->count++;
		printf("--V2P cache set 0x%.16"PRIx64" -- 0x%.16"PRIx64" (0x%.16"PRIx64"/0x%.16"PRIx64"),in %s\n", key->high,
            entry->pa, key->high, key->low,__FUNCTION__);
		return VMI_SUCCESS;
	}else{
		printf("ERROR: v2p cache out of bound(%d). in %s\n",v2p_c->count,__FUNCTION__);
		return VMI_FAILURE;
	}
}
status_t tiny_cache_remove(v2p_cache_t v2p_c,key_128_t key){
	int key_i,i;
	key_i=get_key_index(v2p_c,key);
	if(key_i>=0){
		free(v2p_c->cache_lines[key_i]->key);
		free(v2p_c->cache_lines[key_i]->entry);
		free(v2p_c->cache_lines[key_i]);
		for(i=key_i;i<v2p_c->count;i++){
			v2p_c->cache_lines[i]=v2p_c->cache_lines[i+1];
		}
		v2p_c->count--;
		return VMI_SUCCESS;
	}else{
		return VMI_FAILURE;
	}
}
void tiny_cache_remove_all(v2p_cache_t v2p_c){
	int i;
	for(i=0;i<v2p_c->count;i++){
		free(v2p_c->cache_lines[i]->key);
		free(v2p_c->cache_lines[i]->entry);
		free(v2p_c->cache_lines[i]);
	}
	v2p_c->count=0;
	//free(v2p_c);
	return;
}
void tiny_cache_destroy(v2p_cache_t v2p_c){
	int i;
	for(i=0;i<v2p_c->count;i++){
		free(v2p_c->cache_lines[i]->key);
		free(v2p_c->cache_lines[i]->entry);
		free(v2p_c->cache_lines[i]);
	}
	free(v2p_c->cache_lines);
	free(v2p_c);
	return;
}

v2p_cache_entry_t tiny_cache_lookup(v2p_cache_t v2p_c,key_128_t key){
	int key_i;
	//printf("---searching the key:<va,dtb>=<0x%lx,0x%lx>in %s\n",key->low,key->high,__FUNCTION__);
	key_i=get_key_index(v2p_c,key);
	if(key_i<0) return NULL;
	return v2p_c->cache_lines[key_i]->entry;
}

void
v2p_cache_init(
    vmi_instance_t vmi)
{
	//printf("%s:size of v2p_cache array:%d",__FUNCTION__,MAX_V2P_CACHE*sizeof(v2p_cache_line_t));
	
	v2p_cache_t v2p_cache=(v2p_cache_t)malloc(sizeof(struct v2p_cache_array));
	
	v2p_cache_line_t v2p_cache_lines=(v2p_cache_line_t *)malloc(MAX_V2P_CACHE*sizeof(v2p_cache_line_t));
	
	v2p_cache->cache_lines=v2p_cache_lines;
	
	v2p_cache->count=0;
    
	vmi->v2p_cache = v2p_cache;
	
	
}

void
v2p_cache_destroy(
    vmi_instance_t vmi)
{
    tiny_cache_destroy(vmi->v2p_cache);
}

status_t
v2p_cache_get(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb,
    addr_t *pa)
{
	//printf("---in %s\n",__FUNCTION__);
    v2p_cache_entry_t entry = NULL;
    struct key_128 local_key;
    key_128_t key = &local_key;

	//printf("---now init the key:<va,dtb>=<0x%lx,0x%lx>in %s\n",(uint64_t)va, (uint64_t)dtb,__FUNCTION__);
    key_128_init(vmi, key, (uint64_t)va, (uint64_t)dtb);

	//printf("---now look up the key:<va,dtb>=<0x%lx,0x%lx>in %s\n",(uint64_t)va, (uint64_t)dtb,__FUNCTION__);
    if ((entry = tiny_cache_lookup(vmi->v2p_cache, key)) != NULL) {

        entry->last_used = time(NULL);
        *pa = entry->pa | ((vmi->page_size - 1) & va);
        printf("--V2P cache hit 0x%.16"PRIx64" -- 0x%.16"PRIx64" (0x%.16"PRIx64"/0x%.16"PRIx64"),in %s\n",
                va, *pa, key->high, key->low,__FUNCTION__);
        return VMI_SUCCESS;
    }

    return VMI_FAILURE;
}

v2p_cache_entry_t v2p_cache_entry_create (vmi_instance_t vmi, addr_t pa)
{
    v2p_cache_entry_t entry = (v2p_cache_entry_t) malloc(sizeof(struct v2p_cache_entry));
    pa &= ~((addr_t)vmi->page_size - 1);
    entry->pa = pa;
    entry->last_used = time(NULL);
    return entry;
}

void
v2p_cache_set(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb,
    addr_t pa)
{
    if (!va || !dtb || !pa) {
        return;
    }
    key_128_t key = key_128_build(vmi, (uint64_t)va, (uint64_t)dtb);
    v2p_cache_entry_t entry = v2p_cache_entry_create(vmi, pa);
  	if(VMI_SUCCESS==tiny_cache_insert(vmi->v2p_cache, key, entry)){
		printf("--V2P cache set 0x%.16"PRIx64" -- 0x%.16"PRIx64" (0x%.16"PRIx64"/0x%.16"PRIx64"),in %s\n", va,
            pa, key->high, key->low,__FUNCTION__);
	}else{
		printf("%s:cache set FAILURE\n",__FUNCTION__);
	}
	
}

status_t
v2p_cache_del(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb)
{
    struct key_128 local_key;
    key_128_t key = &local_key;
    key_128_init(vmi, key, (uint64_t)va, (uint64_t)dtb);
    printf("--V2P cache del 0x%.16"PRIx64" (0x%.16"PRIx64"/0x%.16"PRIx64")\n", va,
            key->high, key->low);

    // key collision doesn't really matter here because worst case
    // scenario we incur an small performance hit

    return tiny_cache_remove(vmi->v2p_cache, key);
	
}

void
v2p_cache_flush(
    vmi_instance_t vmi)
{
    tiny_cache_remove_all(vmi->v2p_cache);
    printf("--V2P cache flushed,in %s\n",__FUNCTION__);
}


// Below are wrapper functions for external API access to the cache

void
vmi_v2pcache_add(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb,
    addr_t pa)
{
    return v2p_cache_set(vmi, va, dtb, pa);
}

void
vmi_v2pcache_flush(
    vmi_instance_t vmi)
{
    return v2p_cache_flush(vmi);
}
