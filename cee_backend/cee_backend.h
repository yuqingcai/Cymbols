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
void cee_database_close(cee_pointer db);
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
cee_boolean cee_database_filepaths_user_selected_append(cee_pointer db,
                                                        CEEList* filepaths);
CEEList* cee_database_filepaths_user_selected_get(cee_pointer db);
void cee_database_symbols_clean(cee_pointer db);
void cee_database_symbols_from_source_clean(cee_pointer db,
                                            const cee_char* filepath);
cee_boolean cee_database_symbols_write(cee_pointer db,
                                       CEEList* symbols);
CEEList* cee_database_symbols_search_by_name(cee_pointer db,
                                             const cee_char* name);
CEEList* cee_database_symbols_search_by_parent(cee_pointer db,
                                               const cee_char* parent);
CEEList* cee_database_symbols_search_by_type(cee_pointer db,
                                             const cee_char* type);
CEEList* cee_database_symbols_search_by_filepath(cee_pointer db,
                                                 const cee_char* filepath);
cee_boolean cee_database_symbols_delete_by_filepath(cee_pointer db,
                                                    const cee_char* filepath);
cee_boolean cee_database_security_bookmark_append(cee_pointer db,
                                                  const cee_char* filepath,
                                                  const cee_char* content);
cee_boolean cee_database_security_bookmark_remove(cee_pointer db,
                                                  const cee_char* filepath);
cee_char* cee_database_security_bookmark_content_get(cee_pointer db,
                                                     const cee_char* filepath);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BACKEND_H__ */
