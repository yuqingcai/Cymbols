#ifndef __CEE_BACKEND_H__
#define __CEE_BACKEND_H__

#include <sqlite3.h>
#include "cee_lib.h"
#include "cee_symbol.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _CEEFileReferenceInfo {
    cee_char* file_path;
    cee_char* last_parsed_time;
    cee_int symbol_count;
} CEEFileReferenceInfo;

CEEFileReferenceInfo* cee_file_reference_info_create(void);
void cee_file_reference_info_free(cee_pointer data);

cee_pointer cee_database_create(const cee_char* path);
cee_pointer cee_database_open(const cee_char* path);
void cee_database_close(cee_pointer db);
cee_boolean cee_database_application_info_set(sqlite3* db,
                                              const cee_char* descriptor);
cee_char* cee_database_application_info_get(sqlite3* db);
CEEList* cee_database_session_descriptors_get(cee_pointer db);
cee_boolean cee_database_session_descriptors_remove(cee_pointer db);
cee_boolean cee_database_session_descriptor_append(cee_pointer db,
                                                   const cee_char* descriptor);
cee_boolean cee_database_file_reference_roots_append(cee_pointer db,
                                                      CEEList* file_paths);
cee_boolean cee_database_file_reference_root_remove(cee_pointer db,
                                                    const cee_char* file_path);
cee_boolean cee_database_file_reference_roots_remove(cee_pointer db,
                                                     CEEList* file_paths);
CEEList* cee_database_file_reference_roots_get(cee_pointer db);
cee_boolean cee_database_file_reference_root_is_existed(cee_pointer db,
                                                        const cee_char* file_path);
cee_boolean cee_database_file_reference_append(cee_pointer db,
                                               const cee_char* file_path);
cee_boolean cee_database_file_references_append(cee_pointer db,
                                                CEEList* file_paths);
cee_boolean cee_database_file_reference_remove(cee_pointer db,
                                               const cee_char* file_path);
cee_boolean cee_database_file_references_remove(cee_pointer db,
                                                CEEList* file_paths);
CEEList* cee_database_file_reference_paths_get(cee_pointer db,
                                               const cee_char* condition);
cee_boolean cee_database_file_reference_is_existed(cee_pointer db,
                                                   const cee_char* file_path);
cee_boolean cee_database_file_reference_last_parsed_time_set(cee_pointer db,
                                                             const cee_char* file_path,
                                                             const cee_char* time_str);
cee_char* cee_database_file_reference_last_parsed_time_get(cee_pointer db,
                                                           const cee_char* file_path);
cee_boolean cee_database_file_reference_symbols_count_set(cee_pointer db,
                                                          const cee_char* file_path,
                                                          cee_int symbols_count);
cee_int cee_database_file_reference_symbols_count_get(cee_pointer db,
                                                      const cee_char* file_path);
void cee_database_symbols_delete(cee_pointer db);
cee_boolean cee_database_symbols_write(cee_pointer db,
                                        CEEList* symbols);
CEEList* cee_database_symbols_search_by_name(cee_pointer db,
                                             const cee_char* name);
CEEList* cee_database_symbols_search_by_alias(cee_pointer db,
                                              const cee_char* alias);
CEEList* cee_database_symbols_search_by_parent(cee_pointer db,
                                               const cee_char* parent);
CEEList* cee_database_symbols_search_by_type(cee_pointer db,
                                             const cee_char* type);
CEEList* cee_database_symbols_search_by_file_path(cee_pointer db,
                                                  const cee_char* file_path);
cee_boolean cee_database_symbols_delete_by_file_path(cee_pointer db,
                                                     const cee_char* file_path);
cee_boolean cee_database_security_bookmark_append(cee_pointer db,
                                                  const cee_char* file_path,
                                                  const cee_char* content);
cee_boolean cee_database_security_bookmark_remove(cee_pointer db,
                                                  const cee_char* file_path);
cee_char* cee_database_security_bookmark_content_get(cee_pointer db,
                                                     const cee_char* file_path);
CEEList* cee_database_file_reference_infos_get(cee_pointer db);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BACKEND_H__ */
