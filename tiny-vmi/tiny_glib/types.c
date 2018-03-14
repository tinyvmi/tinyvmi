#include "types.h"



// /* implementations */


gpointer tiny_malloc0 (gsize n_bytes){
    gpointer mem = g_malloc(n_bytes);
    if (mem)
        memset (mem, 0, n_bytes);
    return mem;
}


