#include "types.h"



// /* implementations */


gpointer tiny_malloc0 (gsize n_bytes){
    gpointer mem = g_malloc(n_bytes);
    if (mem)
        memset (mem, 0, n_bytes);
    return mem;
}


/**
 * g_malloc_n:
 * @n_blocks: the number of blocks to allocate
 * @n_block_bytes: the size of each block in bytes
 * 
 * This function is similar to g_malloc(), allocating (@n_blocks * @n_block_bytes) bytes,
 * but care is taken to detect possible overflow during multiplication.
 * 
 * Since: 2.24
 * Returns: a pointer to the allocated memory
 */
gpointer
g_malloc_n (gsize n_blocks,
	    gsize n_block_bytes)
{
//   if (SIZE_OVERFLOWS (n_blocks, n_block_bytes))
//     {
//       g_error ("%s: overflow allocating %"G_GSIZE_FORMAT"*%"G_GSIZE_FORMAT" bytes",
//                G_STRLOC, n_blocks, n_block_bytes);
//     }

  return g_malloc (n_blocks * n_block_bytes);
}



/**
 * g_strndup:
 * @str: the string to duplicate
 * @n: the maximum number of bytes to copy from @str
 *
 * Duplicates the first @n bytes of a string, returning a newly-allocated
 * buffer @n + 1 bytes long which will always be nul-terminated. If @str
 * is less than @n bytes long the buffer is padded with nuls. If @str is
 * %NULL it returns %NULL. The returned value should be freed when no longer
 * needed.
 *
 * To copy a number of characters from a UTF-8 encoded string,
 * use g_utf8_strncpy() instead.
 *
 * Returns: a newly-allocated buffer containing the first @n bytes
 *     of @str, nul-terminated
 */
gchar*
g_strndup (const gchar *str,
           gsize        n)
{
  gchar *new_str;

  if (str)
    {
      new_str = g_new (gchar, n + 1);
      strncpy (new_str, str, n);
      new_str[n] = '\0';
    }
  else
    new_str = NULL;

  return new_str;
}

