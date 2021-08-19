#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "cee_ascii_storage.h"

typedef struct _CEEASCIIStorage {
    cee_uchar* buffer;
    cee_ulong size;
} CEEASCIIStorage;

CEEASCIIStorageRef cee_ascii_storage_create(const cee_uchar* content,
                                            cee_ulong length)
{
    CEEASCIIStorage* storage = (CEEASCIIStorage*)cee_malloc0(sizeof(CEEASCIIStorage));
    cee_ascii_storage_buffer_set(storage, content, length);
    return storage;
}

void cee_ascii_storage_free(CEEASCIIStorageRef data)
{
    if (!data)
        return;
    
    CEEASCIIStorage* storage = (CEEASCIIStorage*)data;
    
    if (storage->buffer)
        cee_free(storage->buffer);
    
    cee_free(storage);
}

void cee_ascii_storage_buffer_set(CEEASCIIStorageRef storage,
                                  const cee_uchar* content,
                                  cee_ulong length)
{
    if (storage->buffer)
        cee_free(storage->buffer);
    storage->buffer = NULL;
    storage->size = 0;
    
    if (!content || !length)
        return;
    
    storage->buffer = cee_malloc(sizeof(cee_uchar)*length);
    memcpy(storage->buffer, content, length);
    storage->size = length;
}

const cee_uchar* cee_ascii_storage_buffer_get(CEEASCIIStorageRef storage)
{
    return storage->buffer;
}

cee_ulong cee_ascii_storage_size_get(CEEASCIIStorageRef storage)
{
    return storage->size;
}


cee_long cee_ascii_storage_buffer_offset_by_paragraph_index(CEEASCIIStorageRef storage,
                                                            cee_int stride,
                                                            cee_long index)
{
    if (!storage || !storage->buffer || !storage->size)
        return 0;
    cee_long current = stride * index;
    return current;
}

void cee_ascii_storage_buffer_byte_next(CEEASCIIStorageRef storage,
                                        cee_long current,
                                        cee_long *next,
                                        cee_uchar* byte)
{
    if (!storage || !storage->buffer)
       return;
   
    if (current == storage->size) {
        *byte = 0;
        *next = -1;
    }
    else {
        *byte = storage->buffer[current];
        *next = current + 1;
    }
}
