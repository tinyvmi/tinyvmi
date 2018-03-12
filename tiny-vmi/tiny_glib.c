
#include <tiny_glib.h>


GSList * g_slist_prepend(GSList * list, gpointer key){

	// printf("-in %s\n",__FUNCTION__);
	
	GSList* new_node=(GSList*) malloc(sizeof(GSList));
	
	//printf("-- allocate tiny_list_node, in %s\n",__FUNCTION__);
	
	new_node->data=key;
	
	//printf("-- set node data=0x%lx, in %s\n",*key,__FUNCTION__);
	
	new_node->next=list;
	
	//printf("-- set head to next node, in %s\n",__FUNCTION__);
	
	// if(list->head==NULL){
	// //	printf("-- set tail and head to new_node, in %s\n",__FUNCTION__);
	// 	list->tail=new_node;
	// 	list->head=new_node;
	// }else{
	// //	printf("-- set head, in %s\n",__FUNCTION__);
	// 	list->head=new_node;
	// }
	// list->size++;
	
	return new_node;
}
