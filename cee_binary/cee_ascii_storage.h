#ifndef __CEE_ASCII_STORAGE_H__
#define __CEE_ASCII_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"

typedef struct _CEEASCIIStorage* CEEASCIIStorageRef;

CEEASCIIStorageRef cee_ascii_storage_create(const cee_uchar* content,
                                            cee_ulong length);
const cee_uchar* cee_ascii_storage_buffer_get(CEEASCIIStorageRef storage);
cee_ulong cee_ascii_storage_size_get(CEEASCIIStorageRef storage);
void cee_ascii_storage_free(CEEASCIIStorageRef data);
void cee_ascii_storage_buffer_set(CEEASCIIStorageRef storage,
                                  const cee_uchar* content,
                                  cee_ulong length);
cee_long cee_ascii_storage_buffer_offset_by_paragraph_index(CEEASCIIStorageRef storage,
                                                            cee_int stride,
                                                            cee_long index);
void cee_ascii_storage_buffer_byte_next(CEEASCIIStorageRef storage,
                                        cee_long current,
                                        cee_long *next,
                                        cee_uchar* byte);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_ASCII_STORAGE_H__ */
