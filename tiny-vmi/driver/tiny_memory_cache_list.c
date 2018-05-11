
#include "tiny_private.h"

#include "tiny_memory_cache.h"

#include "glib_compat.h"



//list for most recent pages

tiny_list_t create_new_list(int capacity){
    
    tiny_list_t list = (tiny_list_t)malloc(sizeof(struct tiny_list));
    
    DBG_LINE;

    list->nodes = (tiny_list_node_t *)malloc(capacity * sizeof(tiny_list_node_t));
    list->capacity = capacity;
    list->head = -1;
    list->tail = -1;
    list->size = 0;

    return list;

}

status_t tiny_list_free(tiny_list_t list){

    if (!list)
        return VMI_SUCCESS;
   
    DBG_LINE;
    
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

    free(list->nodes);
    free(list);

	return VMI_SUCCESS;
}

tiny_list_node_t tiny_list_last(tiny_list_t list){   

    DBG_LINE;
    
    if(list->tail == -1){
        errprint("tiny_list_last: ERROR, list has no element yet.");
    }
    return list->nodes[list->tail];
}

// status_t tiny_list_foreach(tiny_list_t list,void * remove_entry, mem_cache_t memory_cache){

// }

tiny_list_node_t tiny_list_find_custom(tiny_list_t list,
                mem_key_t key){
				
    int node_index = list->head;
	tiny_list_node_t node;	
   
    DBG_LINE;
    
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
 * node will not be freed
*/

void tiny_list_unlink(tiny_list_t list, tiny_list_node_t node){

    // remove it from list:

    // first, get its next node and pre node
    int next_node = node->next;
    int pre_node = node->previous;
   
    DBG_LINE;
    
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

    // free(node->data);
    // free(node);

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

    DBG_LINE;
    
	while(node_index != -1){

        node = list->nodes[node_index];

		if((*(node->data))==(*key)){
            // found the key
            tiny_list_unlink(list,node);

            free(node->data);
            free(node);

            return list;
		}
        // key not found, go to next
		node_index = node->next;
	}
	return list;
}

gpointer tiny_list_pop_last(tiny_list_t list){

    int old_tail = list->tail;

    if (old_tail == -1){
        errprint("ERROR: no tail node yet");
    }

    DBG_LINE;
    
    tiny_list_node_t old_last_node = list->nodes[old_tail];

    gpointer data = old_last_node->data;


    if(old_last_node->next != -1){
        dbprint(VMI_DEBUG_MEMCACHE, "%s\n", __FUNCTION__);
        errprint("ERROR: tail's next should be -1");
    }

    int new_tail = old_last_node->previous;

    //maintain the list struct with new tail and old tail
    // free(old_last_node->data);
    free(old_last_node);
    list->size --;

    list->tail = new_tail;
    if(new_tail != -1) {
        list->nodes[new_tail] -> next = -1;
    }
    
	return data;
    
}

tiny_list_t tiny_list_remove_last(tiny_list_t list){

    int old_tail = list->tail;

    DBG_LINE;
    
    if (old_tail == -1){
        errprint("ERROR: no tail node yet");
    }

    gpointer data = tiny_list_pop_last(list);

    free(data);

	return list;
}

tiny_list_t tiny_list_increase_capacity(tiny_list_t list, int factor){

    int i = 0;

    int old_capacity = list->capacity;
    int new_capacity = list->capacity * factor;

    DBG_LINE;
    
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


// put an existing node to head.
void tiny_list_push_head_link(tiny_list_t list, tiny_list_node_t new_node){
    
    DBG_LINE;
    
    if (list->size == list-> capacity){
        errprint("ERROR: list->size == list->capacity, reach max capacity? should not happend here\n");
    }

	list->size++;

    // now insert the node to head
	if(list->size == 1){ // newnode is now the only node on list.
		dbprint(VMI_DEBUG_MEMCACHE, "-- set tail and head to new_node, in %s\n",__FUNCTION__);
	    list->head = 0;
        list->tail = 0;

        new_node->next = -1;
        list->nodes[0] = new_node;

	}else {
        // we have free space, insert one node and set head to new node

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
    
}

tiny_list_t tiny_list_prepend(tiny_list_t list,mem_key_t key){

    DBG_START;
	
	tiny_list_node_t new_node=(tiny_list_node_t) malloc(sizeof(struct tiny_list_node));
	
	//dbprint(VMI_DEBUG_MEMCACHE, "-- allocate tiny_list_node, in %s\n",__FUNCTION__);
	
	new_node->data=key;
	
    // since it will be the head, the pre should be -1 (null)
    new_node->previous = -1; //important: check this for list head.

	//dbprint(VMI_DEBUG_MEMCACHE, "-- set node data=0x%lx, in %s\n",*key,__FUNCTION__);
	
	if (list->size >= list->capacity) {  
        // we have more nodes than the capacity of the queue
        // either:
        //      - replace tail with new node, set head to it
        // or:
        //      - create a bigger queue with 2X capacity
        
        list = tiny_list_remove_last(list);

        // TODO: fixme: increase capacity when necessary.
        // list = tiny_list_increase_capacity(list, 2);
    }

    tiny_list_push_head_link(list, new_node);

    dbprint(VMI_DEBUG_MEMCACHE, "-- in %s: prepend one element as head, new size: %d\n",__FUNCTION__, list->size);

	DBG_DONE;

	return list;
}
