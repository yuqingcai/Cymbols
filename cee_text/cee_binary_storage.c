#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "cee_binary_storage.h"

typedef struct _CEEBinaryStorage {
} CEEBinaryStorage;

CEEBinaryStorageRef cee_binary_storage_create(const cee_uchar* content,
                                              cee_ulong length)
{
    CEEBinaryStorage* storage = (CEEBinaryStorage*)cee_malloc0(sizeof(CEEBinaryStorage));
    return storage;
}

void cee_binary_storage_free(CEEBinaryStorageRef data)
{
    if (!data)
        return;
    
    CEEBinaryStorage* storage = (CEEBinaryStorage*)data;
    
    cee_free(storage);
}

void cee_binary_storage_buffer_set(CEEBinaryStorageRef storage,
                                   const cee_uchar* content,
                                   cee_ulong length)
{
}

const cee_uchar* cee_binary_storage_buffer_get(CEEBinaryStorageRef storage)
{
    return NULL;
}

cee_ulong cee_binary_storage_size_get(CEEBinaryStorageRef storage)
{
    return 0;
}
