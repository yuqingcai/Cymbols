#ifndef __CEE_BACKEND_H__
#define __CEE_BACKEND_H__

#include <sqlite3.h>
#include "cee_lib.h"
#include "cee_symbol.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

cee_pointer cee_database_create(const cee_char* path);
cee_pointer cee_database_open(const cee_char* path);
CEEList* cee_database_session_descriptors_get(cee_pointer db);
cee_boolean cee_database_session_descriptors_remove(cee_pointer db);
cee_boolean cee_database_session_descriptor_append(cee_pointer db,
                                                   const cee_char* descriptor);
cee_boolean cee_database_filepaths_append(cee_pointer db,
                                          CEEList* filepaths);
CEEList* cee_database_filepaths_get(cee_pointer db,
                                    const cee_char* condition);
cee_boolean cee_database_filepaths_remove(cee_pointer db,
                                          CEEList* filepaths);
void cee_database_symbols_clean(cee_pointer db);
cee_boolean cee_database_symbols_write(cee_pointer db,
                                       CEEList* symbols);

CEEList* cee_database_symbols_search_by_descriptor(cee_pointer db,
                                                   const cee_char* descriptor);
CEEList* cee_database_symbols_search_by_parent(cee_pointer db,
                                               const cee_char* parent);
CEEList* cee_database_symbols_search_by_type(cee_pointer db,
                                             const cee_char* type);
CEEList* cee_database_symbols_search_by_filepath(cee_pointer db,
                                                 const cee_char* filepath);
cee_boolean cee_database_symbols_delete_by_filepath(cee_pointer db,
                                                    const cee_char* filepath);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BACKEND_H__ */
