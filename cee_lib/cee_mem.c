#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "cee_mem.h"
#include "cee_error.h"

cee_pointer cee_malloc(cee_size nb_byte)
{
    cee_pointer bytes = malloc(nb_byte);
    if (bytes)
        return bytes;
    
    cee_error("cee_malloc: failed to allocate %d bytes", nb_byte);
    
    return NULL;
}

cee_pointer cee_malloc0(cee_size nb_byte)
{
     cee_pointer bytes = calloc(1, nb_byte);
    if (bytes)
        return bytes;
    
    cee_error("cee_malloc0: failed to allocate %d bytes", nb_byte);
    
    return NULL;
}

cee_pointer cee_realloc(cee_pointer ptr, cee_size nb_byte)
{
    cee_pointer bytes = realloc(ptr, nb_byte);
    if (bytes)
        return bytes;
    
    cee_error("cee_realloc: failed to reallocate %d bytes", nb_byte);
    
    return NULL;
}

void cee_free(cee_pointer data)
{
    if (!data)
        return;
    
     free(data);
}
