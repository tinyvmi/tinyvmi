
#include "slice.h"

/////////////////////////////////////////////////////
//
// implementations
//
////////////////////////////////////////////////////

/**
 * allocate a block of memory
 * 
 * as a replacement of g_slice_new
 * 
 */

gpointer
tiny_slice_new (gsize mem_size)
{
  gpointer mem = malloc (mem_size);
  return mem;
}

/** 
 * allocate a block of memory and init to 0
 * 
 * as a replacement of g_slice_new0
 * 
 * */
gpointer
tiny_slice_new0 (gsize mem_size)
{
  gpointer mem = tiny_slice_new (mem_size);
  if (mem)
    memset (mem, 0, mem_size);
  return mem;
}

/** 
 * free a block of mem, allocated by tiny_slice_new or tiny_silce_new0 
 * 
 * as a replacement of g_slice_free
 * 
 * */
void tiny_slice_free (gsize mem_size, gpointer mem){

    if(mem)
        free (mem);
}





