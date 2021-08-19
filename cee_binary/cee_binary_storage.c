#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "cee_binary_storage.h"

typedef struct _CEEBinaryStorage {
    cee_uchar* buffer;
    cee_ulong size;
} CEEBinaryStorage;

CEEBinaryStorageRef cee_binary_storage_create(const cee_uchar* content,
                                              cee_ulong length)
{
    CEEBinaryStorage* storage = (CEEBinaryStorage*)cee_malloc0(sizeof(CEEBinaryStorage));
    cee_binary_storage_buffer_set(storage, content, length);
    return storage;
}

void cee_binary_storage_free(CEEBinaryStorageRef data)
{
    if (!data)
        return;
    
    CEEBinaryStorage* storage = (CEEBinaryStorage*)data;
    
    if (storage->buffer)
        cee_free(storage->buffer);
    
    cee_free(storage);
}

void cee_binary_storage_buffer_set(CEEBinaryStorageRef storage,
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

const cee_uchar* cee_binary_storage_buffer_get(CEEBinaryStorageRef storage)
{
    return storage->buffer;
}

cee_ulong cee_binary_storage_size_get(CEEBinaryStorageRef storage)
{
    return storage->size;
}


cee_long cee_binary_storage_buffer_offset_by_paragraph_index(CEEBinaryStorageRef storage,
                                                             cee_int stride,
                                                             cee_long index)
{
    if (!storage || !storage->buffer || !storage->size)
        return 0;
    cee_long current = stride * index;
    return current;
}

void cee_binary_storage_buffer_byte_next(CEEBinaryStorageRef storage,
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
