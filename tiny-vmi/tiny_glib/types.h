#ifndef TINY_GLIB_TYPES_H
#define TINY_GLIB_TYPES_H


#include <stdbool.h> 


#define NULL 0
#define TRUE 1
#define FALSE 0

#define g_malloc malloc

#define g_free free 

typedef unsigned int    gsize;
typedef bool gboolean;

////////////////////////
// the following copied from glib
typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef int    gint;
// typedef gint   gboolean;

typedef unsigned char   guchar;
typedef unsigned short  gushort;
typedef unsigned long   gulong;
typedef unsigned int    guint;

typedef float   gfloat;
typedef double  gdouble;
typedef void* gpointer;
typedef const void *gconstpointer;

// gint32, guint32, gint64, guint64, etc.: 
// https://developer.gnome.org/glib/stable/glib-Basic-Types.html#gint64
typedef signed int gint32;
typedef unsigned int guint32;
typedef signed long gint64;  
typedef unsigned long guint64;

typedef gint            (*GCompareFunc)         (gconstpointer  a,
                                                 gconstpointer  b);
typedef gint            (*GCompareDataFunc)     (gconstpointer  a,
                                                 gconstpointer  b,
						 gpointer       user_data);
typedef gboolean        (*GEqualFunc)           (gconstpointer  a,
                                                 gconstpointer  b);
typedef void            (*GDestroyNotify)       (gpointer       data);
typedef void            (*GFunc)                (gpointer       data,
                                                 gpointer       user_data);
typedef guint           (*GHashFunc)            (gconstpointer  key);
typedef void            (*GHFunc)               (gpointer       key,
                                                 gpointer       value,
                                                 gpointer       user_data);

// glib/gnode.h
typedef gpointer	(*GCopyFunc)            (gconstpointer  src,
                                                 gpointer       data);
    

/**
 * GFreeFunc:
 * @data: a data pointer
 *
 * Declares a type of function which takes an arbitrary
 * data pointer argument and has no return value. It is
 * not currently used in GLib or GTK+.
 */
typedef void            (*GFreeFunc)            (gpointer       data);

/**
 * GTranslateFunc:
 * @str: the untranslated string
 * @data: user data specified when installing the function, e.g.
 *  in g_option_group_set_translate_func()
 * 
 * The type of functions which are used to translate user-visible
 * strings, for <option>--help</option> output.
 * 
 * Returns: a translation of the string for the current locale.
 *  The returned string is owned by GLib and must not be freed.
 */
typedef const gchar *   (*GTranslateFunc)       (const gchar   *str,
						 gpointer       data);


/* Define min and max constants for the fixed size numerical types */
#define G_MININT8	((gint8) -0x80)
#define G_MAXINT8	((gint8)  0x7f)
#define G_MAXUINT8	((guint8) 0xff)

#define G_MININT16	((gint16) -0x8000)
#define G_MAXINT16	((gint16)  0x7fff)
#define G_MAXUINT16	((guint16) 0xffff)

#define G_MININT32	((gint32) -0x80000000)
#define G_MAXINT32	((gint32)  0x7fffffff)
#define G_MAXUINT32	((guint32) 0xffffffff)

#define G_MININT64	((gint64) G_GINT64_CONSTANT(-0x8000000000000000))
#define G_MAXINT64	G_GINT64_CONSTANT(0x7fffffffffffffff)
#define G_MAXUINT64	G_GUINT64_CONSTANT(0xffffffffffffffff)

#define GPOINTER_TO_INT(p)	((gint) (p))
#define GPOINTER_TO_UINT(p)	((guint) (p))


#define GINT_TO_POINTER(i)	((gpointer) (i))
#define GUINT_TO_POINTER(u)	((gpointer) (u))


gpointer
g_malloc_n (gsize n_blocks,
	    gsize n_block_bytes);


#define _G_NEW(struct_type, n_structs, func) \
        ((struct_type *) g_##func##_n ((n_structs), sizeof (struct_type)))

#define g_new(struct_type, n_structs)			_G_NEW (struct_type, n_structs, malloc)

// GLIB_AVAILABLE_IN_ALL
gchar*	              g_strndup	       (const gchar *str,
					gsize        n) ;  


// #undef	MAX
// #define MAX(a, b)  (((a) > (b)) ? (a) : (b))


#define g_return_if_fail(expr) \
if(!expr){\
return;\
}

#define g_return_val_if_fail(expr,value) \
if(!expr){\
return value;\
}

// #define g_malloc0(size) \
// ({gpointer mem; \
// mem=g_malloc(size);\
// if(mem) memset(mem,0,size);\
// return mem;})

#define g_malloc0(size) tiny_malloc0(size)

gpointer tiny_malloc0 (gsize n_bytes);


#define g_warning(...)  printf ("[%s:%d]: %s", __FILE__, __LINE__, __VA_ARGS__)

#endif //TINY_GLIB_TYPES_H