#ifndef __CEE_BINARY_STORAGE_H__
#define __CEE_BINARY_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"

typedef struct _CEEBinaryStorage* CEEBinaryStorageRef;

CEEBinaryStorageRef cee_binary_storage_create(const cee_uchar* content,
                                              cee_ulong length);

const cee_uchar* cee_binary_storage_buffer_get(CEEBinaryStorageRef storage);
cee_ulong cee_binary_storage_size_get(CEEBinaryStorageRef storage);
void cee_binary_storage_free(CEEBinaryStorageRef data);

void cee_binary_storage_buffer_set(CEEBinaryStorageRef storage,
                                   const cee_uchar* content,
                                   cee_ulong length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BINARY_STORAGE_H__ */
