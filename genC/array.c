/*
 * $Id$
 * 
 * some dynamic char * array management.
 */

#include <stdlib.h>
#include "genC.h"

#define GEN_ARRAY_SIZE_INCREMENT (50)

struct _gen_array_chunk_t {
    int size;
    int nitems;
    char ** array;
};

gen_array_t
gen_array_make(int size)
{
    gen_array_t a;
    int i;
    if (size<=0) size= GEN_ARRAY_SIZE_INCREMENT; /* default size */
    a = (gen_array_t) malloc(sizeof(struct _gen_array_chunk_t));
    message_assert("array ok", a);
    a->size = size;
    a->nitems = 0; /* number of items stored */
    a->array = (char**) malloc(sizeof(char*)*(a->size));
    message_assert("malloc ok", a->array);
    for (i=0; i<size; i++) a->array[i] = (char*) NULL;
    return a;
}

static void
gen_array_resize(gen_array_t a)
{
    int nsize = GEN_ARRAY_SIZE_INCREMENT, i;
    a->array = (char**) realloc(a->array, sizeof(char*)*nsize);
    message_assert("realloc ok", a->array);
    for (i=a->size; i<nsize; i++) a->array[i] = (char*) NULL;
    a->size = nsize;
}

void
gen_array_free(gen_array_t a)
{
    free(a->array);
    free(a);
}

void 
gen_array_full_free(gen_array_t a)
{
    int i;
    for (i=0; i<a->size; i++)
	if (a->array[i]) free(a->array[i]);
    gen_array_free(a);
}

void
gen_array_addto(gen_array_t a, int i, char * what)
{
    if (i==a->size) gen_array_resize(a);
    message_assert("valid index", 0<=i && i<a->size);
    if (a->array[i]) a->nitems--;
    a->array[i] = what;
    if (a->array[i]) a->nitems++;
}

void 
gen_array_append(gen_array_t a, char * what)
{
    gen_array_addto(a, a->nitems, what);
}

void
gen_array_dupaddto(gen_array_t a, int i, char * what)
{
    gen_array_addto(a, i, strdup(what));
}

void
gen_array_dupappend(gen_array_t a, char * what)
{
    gen_array_append(a, strdup(what));
}

/* Observers...
 */
char **
gen_array_pointer(gen_array_t a)
{
    return a->array;
}

int 
gen_array_nitems(gen_array_t a)
{
    return a->nitems;
}

int 
gen_array_size(gen_array_t a)
{
    return a->size;
}

char *
gen_array_item(gen_array_t a, int i)
{
    message_assert("valid index", 0<=i && i<a->size);
    return a->array[i];
}

/* Sort: assumes that the items are the first ones.
 */
static int 
gen_array_cmp(const void * a1, const void * a2)
{
    return strcmp(* (char **) a1, * (char **) a2);
}

void
gen_array_sort(gen_array_t a)
{
   qsort(a->array, a->nitems, sizeof(char *), gen_array_cmp);
}
