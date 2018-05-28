
#ifndef TINY_SLICE_H
#define TINY_SLICE_H

#include "types.h"



/**
 * allocate a block of memory
 * 
 * as a replacement of g_slice_new
 * 
 */
gpointer
tiny_slice_new (gsize mem_size);

/**
 * allocate a block of memory and init to 0
 * 
 * as a replacement of g_slice_new0
 * 
 * */
gpointer
tiny_slice_new0 (gsize mem_size);

/**
 * free a block of mem, allocated by tiny_slice_new or tiny_silce_new0 
 * 
 * as a replacement of g_slice_free
 * 
 * */
void tiny_slice_free (gsize mem_size, gpointer mem);


#endif //TINY_SLICE_H