
/*
 * @file glib_compat.c
 * @brief Compatibility functions are defined here that enable use of LibVMI 
 *  with versions of glib older than 2.22
 *
 */


/* Pointers to these convenience functions are passed as parameters to many other
 *  functions related to GHashTable initialization and manipulation, so we cannot
 *  employ any other (more pleasant) tricks that rely upon the pre-processor.
 */