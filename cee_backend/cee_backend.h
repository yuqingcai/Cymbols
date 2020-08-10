#ifndef __CEE_BACKEND_H__
#define __CEE_BACKEND_H__

#include <sqlite3.h>
#include "cee_lib.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

cee_pointer cee_database_create(const cee_char* path);
cee_pointer cee_database_open(const cee_char* path);
CEEList* cee_database_session_descriptors_get(cee_pointer database);
cee_boolean cee_database_session_descriptors_remove(cee_pointer database);
cee_boolean cee_database_session_descriptor_append(cee_pointer database,
                                                   const cee_char* descriptor);
cee_boolean cee_database_filepaths_append(cee_pointer database,
                                          CEEList* filepaths);
CEEList* cee_database_filepaths_get(cee_pointer database,
                                    const cee_char* condition);
cee_boolean cee_database_filepaths_remove(cee_pointer database,
                                          CEEList* filepaths);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BACKEND_H__ */
