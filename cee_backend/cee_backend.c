#include <stdlib.h>
#include <stdio.h>
#include "cee_backend.h"

#define SQL_MAX_LENGTH  4096

static cee_boolean tables_existed(sqlite3* db);
static cee_boolean tables_create(sqlite3* db);
static CEEList* symbols_search(sqlite3* db,
                               const cee_char* condition);

cee_pointer cee_database_create(const cee_char* path)
{
    int error = 0;
    sqlite3* database = NULL;
    cee_char* backend_path = cee_path_dirname(path);    
    if (cee_dir_make_p(backend_path)) 
        goto exit;
    
    error = sqlite3_open(path, &database);
    if (error) {
        fprintf(stdout, "Error: %s\n", sqlite3_errmsg(database));
        goto exit;
    }
    
    if (!tables_create(database)) {
        sqlite3_close(database);
        database = NULL;
        goto exit;
    }
        
exit:
    if (backend_path)
        cee_free(backend_path);
    
    return database;
}

cee_pointer cee_database_open(const cee_char* path)
{
    sqlite3* database = NULL;
    int error = 0;
    
    error = sqlite3_open(path, &database);
    if (error) {
        fprintf(stdout, "Error: %s\n", sqlite3_errmsg(database));
        return NULL;
    }
    
    if (!tables_existed(database)) {
        if (database)
            sqlite3_close(database);
        return NULL;
    }
    
    return database;
}

void cee_database_close(cee_pointer db)
{
    if (!db)
        return;
    
    sqlite3_close(db);
}

static cee_boolean tables_existed(sqlite3* database)
{
    cee_boolean existed = FALSE;
    sqlite3_stmt* stmt = NULL;
    const cee_char* sql = NULL;
    
    /** cee_application_info */
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_application_info'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_application_info table not existed!");
        return FALSE;
    }
    
    /** cee_project_sessions table*/
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_project_sessions'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_project_sessions table not existed!");
        return FALSE;
    }
    
    /** cee_file_reference_roots table*/
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_file_reference_roots'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return existed;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_file_reference_roots table not existed!");
        return FALSE;
    }
    
    /** cee_file_references table*/
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_file_references'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_file_references table not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_file_name_indexes'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_file_name_indexes index not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_file_path_indexes'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_file_path_indexes index not existed!");
        return FALSE;
    }
    
    /** cee_source_symbols table*/
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_source_symbols'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return existed;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_source_symbols table not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_source_symbols_name_indexes'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_source_symbols_name_indexes index not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_source_symbols_file_path_indexes'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_source_symbols_file_path_indexes index not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_source_symbols_alias_indexes'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_source_symbols_alias_indexes index not existed!");
        return FALSE;
    }
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_source_symbols_parent_indexes'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return FALSE;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_source_symbols_parent_indexes index not existed!");
        return FALSE;
    }
    
    /** cee_security_bookmarks table*/
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_security_bookmarks'";
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return existed;
    }
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        existed = TRUE;
    } while (1);
    sqlite3_finalize(stmt);
    if (!existed) {
        fprintf(stdout, "ERROR: cee_security_bookmarks table not existed!");
        return FALSE;
    }
    
    return TRUE;
}

static cee_boolean tables_create(sqlite3* database)
{
    cee_char* message = NULL;
    const cee_char* sql = NULL;
    
    /** cee_application_info */
    sql =
    "CREATE TABLE IF NOT EXISTS cee_application_info ("                        \
    "   id                 INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   descriptor         TEXT                                            "   \
    ");";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    /** cee_project_sessions table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_project_sessions ("                        \
    "   id                 INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   descriptor         TEXT                                            "   \
    ");";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    /** cee_file_reference_roots table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_file_reference_roots ("    \
    "   id             INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   file_path      TEXT UNIQUE                            NOT NULL "   \
    ");";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    /** cee_file_references table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_file_references ("                      \
    "   id                  INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ," \
    "   name                TEXT                                   NOT NULL ," \
    "   file_path           TEXT                                   NOT NULL ," \
    "   symbols_count       INTEGER                                NOT NULL ," \
    "   last_parsed_time    TEXT                                             " \
    ");";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_file_name_indexes on cee_file_references (name);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_file_path_indexes on cee_file_references (file_path);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    /** cee_source_symbols table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_source_symbols ("                  \
    "   id                  INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   type                INTEGER                                         ,"  \
    "   name                TEXT                                            ,"  \
    "   alias               TEXT                                            ,"  \
    "   parent              TEXT                                            ,"  \
    "   derives             TEXT                                            ,"  \
    "   proto_descriptor    TEXT                                            ,"  \
    "   language            TEXT                                            ,"  \
    "   file_path           TEXT                                            ,"  \
    "   line_no             INTEGER                                         ,"  \
    "   ranges              TEXT                                            ,"  \
    "   fragment_range      TEXT                                             "  \
    ");";
    
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_name_indexes on cee_source_symbols (name);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_file_path_indexes on cee_source_symbols (file_path);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_alias_indexes on cee_source_symbols (alias);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_parent_indexes on cee_source_symbols (parent);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    /** cee_security_bookmarks table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_security_bookmarks ("                  \
    "   id             INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   file_path      TEXT UNIQUE                            NOT NULL ,"  \
    "   content        TEXT                                   NOT NULL  "  \
    ");";
    
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    return TRUE;
}

cee_boolean cee_database_application_info_set(sqlite3* db,
                                              const cee_char* descriptor)
{
    if (!db || !descriptor)
        return FALSE;
    
    cee_boolean ret = FALSE;
    sqlite3_stmt* stmt = NULL;
    cee_char* sql = "INSERT INTO cee_application_info (descriptor) VALUES (?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        goto exit;
    }
    
    sqlite3_bind_text(stmt, 1, descriptor, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE)
        ret = FALSE;
    
    ret = TRUE;
    
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    
exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_char* cee_database_application_info_get(sqlite3* db)
{
    if (!db )
        return NULL;
    
    cee_char* info = NULL;
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    cee_char* sql = "SELECT descriptor FROM cee_application_info;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;

    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;
    
    str = (char*)sqlite3_column_text(stmt, 0);
    if (!str)
        goto exit;
    
    length = sqlite3_column_bytes(stmt, 0);
    info = cee_strndup(str, length);
    
exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return info;
}

CEEList* cee_database_session_descriptors_get(cee_pointer db)
{
    if (!db)
        return NULL;
    
    CEEList* descriptors = NULL;
    cee_char* descriptor = NULL;
    cee_char* str = NULL;
    cee_ulong length = 0;
    const cee_char* sql = "SELECT descriptor FROM cee_project_sessions;";
    
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "SQL Error");
        return NULL;
    }
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        str = (char*)sqlite3_column_text(stmt, 0);
        length = sqlite3_column_bytes(stmt, 0);
        
        if (!str)
            continue;
        
        descriptor = cee_strndup(str, length);
        
        if (descriptor)
            descriptors = cee_list_append(descriptors, descriptor);
        
    } while (1);
    
    if (stmt)
        sqlite3_finalize(stmt);
    
    return descriptors;
}

cee_boolean cee_database_session_descriptors_remove(cee_pointer db)
{
    if (!db)
        return FALSE;
    
    cee_char* message = NULL;
    cee_char* sql = "DELETE FROM cee_project_sessions;";
    if (sqlite3_exec(db, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    return TRUE;
}

cee_boolean cee_database_session_descriptor_append(cee_pointer db,
                                                   const cee_char* descriptor)
{
    if (!db || !descriptor)
        return FALSE;
    
    cee_boolean ret = FALSE;
    sqlite3_stmt* stmt = NULL;
    cee_char* sql = "INSERT INTO cee_project_sessions (descriptor) VALUES (?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        goto exit;
    }
    
    sqlite3_bind_text(stmt, 1, descriptor, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE)
        goto exit;
    
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    ret = TRUE;

exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_file_reference_roots_append(cee_pointer db,
                                                      CEEList* file_paths)
{
    cee_boolean ret = FALSE;
    
    if (!db || !file_paths)
        return ret;
    
    cee_char* file_path = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "INSERT INTO cee_file_reference_roots (file_path) VALUES (?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return ret;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    p = file_paths;
    while (p) {
        file_path = p->data;
        if (!cee_database_file_reference_root_is_existed(db, file_path)) {
            sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);
            
            if (sqlite3_step(stmt) != SQLITE_DONE)
                goto exit;
            
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
        p = p->next;
    }
    ret = TRUE;
    
exit:
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_file_reference_root_remove(cee_pointer db,
                                                    const cee_char* file_path)
{
    cee_boolean ret = FALSE;
    
    if (!db || !file_path)
        return ret;
    
    cee_char* sql = "DELETE FROM cee_file_reference_roots WHERE file_path=?;";
    
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return ret;
    }
    
    if (cee_database_file_reference_root_is_existed(db, file_path)) {
        sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_DONE)
            ret = TRUE;
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_file_reference_roots_remove(cee_pointer db,
                                                     CEEList* file_paths)
{
    cee_boolean ret = FALSE;
    
    if (!db || !file_paths)
        return ret;
    
    cee_char* file_path = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "DELETE FROM cee_file_reference_roots WHERE file_path=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return ret;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    
    p = file_paths;
    while (p) {
        file_path = p->data;
        if (cee_database_file_reference_root_is_existed(db, file_path)) {
            sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);
            
            if (sqlite3_step(stmt) != SQLITE_DONE)
                goto exit;
            
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
        p = p->next;
    }
    ret = TRUE;
    
exit:
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    return ret;
}

CEEList* cee_database_file_reference_roots_get(cee_pointer db)
{
    
    if (!db)
        return NULL;
    
    CEEList* file_paths = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "SELECT file_path FROM cee_file_reference_roots;");
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        str = (char*)sqlite3_column_text(stmt, 0);
        length = sqlite3_column_bytes(stmt, 0);
        file_paths = cee_list_prepend(file_paths, cee_strndup(str, length));
        
    } while (1);
    sqlite3_finalize(stmt);
    
    file_paths = cee_list_reverse(file_paths);
    
    return file_paths;
}


cee_boolean cee_database_file_reference_root_is_existed(cee_pointer db,
                                                        const cee_char* file_path)
{
    
    if (!db)
        return FALSE;
    
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    cee_boolean ret = FALSE;
    
    sprintf(sql, "SELECT file_path FROM cee_file_reference_roots WHERE file_path=\"%s\";", file_path);
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;
    
    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;
    
    ret = TRUE;
    
exit:
    sqlite3_finalize(stmt);
    return ret;
}

cee_boolean cee_database_file_reference_append(cee_pointer db,
                                               const cee_char* file_path)
{
    cee_boolean ret = FALSE;
    
    if (!db || !file_path)
        return ret;
    
    cee_char* file_name = NULL;
    cee_char* sql = "INSERT INTO cee_file_references (name, file_path, symbols_count) VALUES (?, ?, ?);";
    
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return ret;
    }
    
    if (!cee_database_file_reference_is_existed(db, file_path)) {
        file_name = cee_path_basename(file_path);
        if (file_name) {
            sqlite3_bind_text(stmt, 1, file_name, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, file_path, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, 0);
            
            if (sqlite3_step(stmt) == SQLITE_DONE)
                ret = TRUE;
            
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
            cee_free(file_name);
        }
    }
    
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_file_references_append(cee_pointer db,
                                                CEEList* file_paths)
{
    cee_boolean ret = FALSE;
    
    if (!db || !file_paths)
        return ret;
    
    cee_char* file_path = NULL;
    cee_char* file_name = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "INSERT INTO cee_file_references (name, file_path, symbols_count) VALUES (?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return ret;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    p = file_paths;
    while (p) {
        file_path = p->data;
        if (!cee_database_file_reference_is_existed(db, file_path)) {
            file_name = cee_path_basename(file_path);
            if (file_name) {
                sqlite3_bind_text(stmt, 1, file_name, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, file_path, -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 3, 0);
                
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    cee_free(file_name);
                    goto exit;
                }
                
                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
                cee_free(file_name);
            }
        }
        p = p->next;
    }
    ret = TRUE;
    
exit:
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_file_reference_remove(cee_pointer db,
                                               const cee_char* file_path)
{
    cee_boolean ret = FALSE;
    
    if (!db || !file_path)
        return ret;
    
    cee_char* sql = "DELETE FROM cee_file_references WHERE file_path=?;";
    
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return ret;
    }
    
    if (cee_database_file_reference_is_existed(db, file_path)) {
        sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_DONE)
            ret = TRUE;
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_file_references_remove(cee_pointer db,
                                                CEEList* file_paths)
{
    cee_boolean ret = FALSE;
    
    if (!db || !file_paths)
        return ret;
    
    cee_char* file_path = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "DELETE FROM cee_file_references WHERE file_path=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return ret;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    p = file_paths;
    while (p) {
        file_path = p->data;
        if (cee_database_file_reference_is_existed(db, file_path)) {
            sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);
            
            if (sqlite3_step(stmt) != SQLITE_DONE)
                goto exit;
            
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
        p = p->next;
    }
    ret = TRUE;
    
exit:
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    return ret;
}

CEEList* cee_database_file_reference_paths_get(cee_pointer db,
                                               const cee_char* condition)
{
    if (!db)
        return NULL;
    
    CEEList* file_paths = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    if (condition)
        sprintf(sql, 
                "SELECT file_path, name FROM cee_file_references WHERE name LIKE \"%%%s%%\" ORDER BY name ASC;",
                condition);
    else
        sprintf(sql, "SELECT file_path, name FROM cee_file_references ORDER BY name ASC;");
            
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        str = (char*)sqlite3_column_text(stmt, 0);
        length = sqlite3_column_bytes(stmt, 0);
        file_paths = cee_list_prepend(file_paths, cee_strndup(str, length));
        
    } while (1);
    
    sqlite3_finalize(stmt);
    
    file_paths = cee_list_reverse(file_paths);
    
    return file_paths;
}

cee_boolean cee_database_file_reference_is_existed(cee_pointer db,
                                                   const cee_char* file_path)
{
    if (!db)
        return FALSE;
    
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    cee_boolean ret = FALSE;
    
    sprintf(sql, "SELECT file_path FROM cee_file_references WHERE file_path=\"%s\";", file_path);
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;
    
    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;
    
    ret = TRUE;
    
exit:
    sqlite3_finalize(stmt);
    return ret;
}

cee_boolean cee_database_file_reference_last_parsed_time_set(cee_pointer db,
                                                             const cee_char* file_path,
                                                             const cee_char* time_str)
{
    if (!db || !file_path)
        return FALSE;
    
    cee_boolean ret = FALSE;
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    
    if (!time_str)
        sprintf(sql, "UPDATE cee_file_references set last_parsed_time=NULL WHERE file_path=\"%s\";",
                file_path);
    else
        sprintf(sql, "UPDATE cee_file_references set last_parsed_time=\"%s\" WHERE file_path=\"%s\";",
                time_str, file_path);
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;
    
    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;
    
    ret = TRUE;
    
exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_char* cee_database_file_reference_last_parsed_time_get(cee_pointer db,
                                                           const cee_char* file_path)
{
    if (!db || !file_path)
        return NULL;
        
    cee_char* time_str = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    sprintf(sql, "SELECT last_parsed_time FROM cee_file_references WHERE file_path=\"%s\";", file_path);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;
    
    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;
    
    str = (char*)sqlite3_column_text(stmt, 0);
    if (!str)
        goto exit;
    
    length = sqlite3_column_bytes(stmt, 0);
    time_str = cee_strndup(str, length);
    
exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return time_str;
}

cee_boolean cee_database_file_reference_symbols_count_set(cee_pointer db,
                                                          const cee_char* file_path,
                                                          cee_int symbols_count)
{
    if (!db || !file_path)
        return FALSE;
    
    cee_boolean ret = FALSE;
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "UPDATE cee_file_references set symbols_count=%d WHERE file_path=\"%s\";",
            symbols_count,
            file_path);
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;
    
    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;
    
    ret = TRUE;

exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_int cee_database_file_reference_symbols_count_get(cee_pointer db,
                                                      const cee_char* file_path)
{
    if (!db || !file_path)
        return -1;
        
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    cee_int symbols_count = -1;
    sprintf(sql, "SELECT symbols_count FROM cee_file_references WHERE file_path=\"%s\";", file_path);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;
    
    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;

    symbols_count = sqlite3_column_int(stmt, 0);
    
exit:
    
    if (stmt)
        sqlite3_finalize(stmt);
    
    return symbols_count;
}

void cee_database_symbols_delete(cee_pointer db)
{
    if (!db)
        return;
        
    cee_char* message = NULL;
    cee_char* sql = "DELETE FROM cee_source_symbols;";
    if (sqlite3_exec(db, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
    }
    sql = "UPDATE cee_file_references set last_parsed_time=NULL;";
    if (sqlite3_exec(db, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
    }
}

cee_boolean cee_database_symbols_write(cee_pointer db,
                                       CEEList* symbols)
{
    if (!db)
        return FALSE;
    
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "INSERT INTO cee_source_symbols (" \
                        "type, name, alias, parent, derives, proto_descriptor, language, file_path, line_no, ranges, fragment_range" \
                    ")" \
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    int ret = SQLITE_OK;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    
    CEEList* p = symbols;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        cee_char* ranges_str = cee_string_from_ranges(symbol->ranges);
        cee_char* fragment_range_str = cee_string_from_range(&symbol->fragment_range);
        
        sqlite3_bind_int (stmt, 1, symbol->type);
        sqlite3_bind_text(stmt, 2, symbol->name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, symbol->alias, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, symbol->parent, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, symbol->derives, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, symbol->proto_descriptor, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, symbol->language, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, symbol->file_path, -1, SQLITE_STATIC);
        sqlite3_bind_int (stmt, 9, symbol->line_no);
        sqlite3_bind_text(stmt, 10, ranges_str, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 11, fragment_range_str, -1, SQLITE_STATIC);
        ret = sqlite3_step(stmt);
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        
        if (ranges_str)
            cee_free(ranges_str);
        
        if (fragment_range_str)
            cee_free(fragment_range_str);
        
        if (ret != SQLITE_DONE)
            break;
        
        p = p->next;
    }
    
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    return TRUE;
}
CEEList* cee_database_symbols_search_by_name(cee_pointer db,
                                             const cee_char* name)
{
    if (!db || !name)
        return NULL;
    
    cee_char* condition = NULL;
    cee_strconcat0(&condition, "name=", "'", name, "'", NULL);
    CEEList* symbols = symbols_search(db, condition);
    
    cee_free(condition);
    
    return symbols;
}

CEEList* cee_database_symbols_search_by_alias(cee_pointer db,
                                              const cee_char* alias)
{
    if (!db || !alias)
        return NULL;
    
    cee_char* condition = NULL;
    cee_strconcat0(&condition, "alias=", "'", alias, "'", NULL);
    CEEList* symbols = symbols_search(db, condition);
    
    cee_free(condition);
    
    return symbols;
}

CEEList* cee_database_symbols_search_by_parent(cee_pointer db,
                                               const cee_char* parent)
{
    if (!db || !parent)
        return NULL;
    
    CEEList* symbols = NULL;
    cee_char* condition = NULL;
    
    cee_strconcat0(&condition, "parent=", "'", parent, "'", NULL);
    symbols = symbols_search(db, condition);
    
    if (!symbols) {
        if (condition)
            cee_free(condition);
        condition = NULL;
        
        cee_strconcat0(&condition, "parent like ", "'%,", parent, ",%'", NULL);
        symbols = symbols_search(db, condition);
    }
    
    cee_free(condition);
    return symbols;
}

CEEList* cee_database_symbols_search_by_type(cee_pointer db,
                                             const cee_char* type)
{
    if (!db || !type)
        return NULL;
    
    CEEList* symbols = NULL;
    cee_char* condition = NULL;
    cee_strconcat0(&condition, "type=", "'", type, "'", NULL);
    symbols = symbols_search(db, condition);
    cee_free(condition);
    return symbols;
}

CEEList* cee_database_symbols_search_by_file_path(cee_pointer db,
                                                  const cee_char* file_path)
{
    if (!db || !file_path)
        return NULL;
    
    CEEList* symbols = NULL;
    cee_char* condition = NULL;
    cee_strconcat0(&condition, "file_path=", "'", file_path, "'", NULL);
    symbols = symbols_search(db, condition);
    cee_free(condition);
    return symbols;
}

static CEEList* symbols_search(sqlite3* db,
                               const cee_char* condition)
{
    if (!db || !condition)
        return NULL;
    
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    cee_char* sql = NULL;
    cee_char* text = NULL;
    int length = 0;
    sqlite3_stmt* stmt;
    cee_char* str = NULL;
    
    cee_strconcat0(&sql, 
                   "SELECT type, name, alias, parent, derives, proto_descriptor, language, file_path, line_no, ranges, fragment_range FROM cee_source_symbols WHERE ",
                   condition, 
                   ";",
                   NULL);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        symbol = cee_source_symbol_create(kCEESourceSymbolTypeUnknow,
                                          NULL,
                                          NULL,
                                          NULL, 
                                          NULL, 
                                          NULL, 
                                          NULL, 
                                          NULL,
                                          0,
                                          NULL,
                                          cee_range_make(-1, 0));
        
        /** type */
        symbol->type = sqlite3_column_int(stmt, 0);
        
        /** name */
        text = (char*)sqlite3_column_text(stmt, 1);
        length = sqlite3_column_bytes(stmt, 1);
        symbol->name = cee_strndup(text, length);
        
        /** alias */
        text = (char*)sqlite3_column_text(stmt, 2);
        length = sqlite3_column_bytes(stmt, 2);
        symbol->alias = cee_strndup(text, length);
        
        /** parent */
        text = (char*)sqlite3_column_text(stmt, 3);
        length = sqlite3_column_bytes(stmt, 3);
        symbol->parent = cee_strndup(text, length);
        
        /** derives */
        text = (char*)sqlite3_column_text(stmt, 4);
        length = sqlite3_column_bytes(stmt, 4);
        symbol->derives = cee_strndup(text, length);
        
        /** proto_descriptor */
        text = (char*)sqlite3_column_text(stmt, 5);
        length = sqlite3_column_bytes(stmt, 5);
        symbol->proto_descriptor = cee_strndup(text, length);
        
        /** language */
        text = (char*)sqlite3_column_text(stmt, 6);
        length = sqlite3_column_bytes(stmt, 6);
        symbol->language = cee_strndup(text, length);
        
        /** file_path */
        text = (char*)sqlite3_column_text(stmt, 7);
        length = sqlite3_column_bytes(stmt, 7);
        symbol->file_path = cee_strndup(text, length);
        
        /** line_no */
        symbol->line_no = sqlite3_column_int(stmt, 8);
        
        /** ranges */
        text = (char*)sqlite3_column_text(stmt, 9);
        length = sqlite3_column_bytes(stmt, 9);
        str = cee_strndup(text, length);
        if (str) {
            symbol->ranges = cee_ranges_from_string(str);
            cee_free(str);
        }
        
        /** fragment_range */
        text = (char*)sqlite3_column_text(stmt, 10);
        length = sqlite3_column_bytes(stmt, 10);
        str = cee_strndup(text, length);
        if (str) {
            symbol->fragment_range = cee_range_from_string(str);
            cee_free(str);
        }
        
        symbols = cee_list_prepend(symbols, symbol);
        
    } while (1);
    
    sqlite3_finalize(stmt);
    
    cee_free(sql);
        
    return symbols;
}

cee_boolean cee_database_symbols_delete_by_file_path(cee_pointer db,
                                                     const cee_char* file_path)
{    
    if (!db || !file_path)
        return FALSE;
    
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* message = NULL;
    sprintf(sql, "DELETE FROM cee_source_symbols WHERE file_path=\"%s\";", file_path);
    if (sqlite3_exec(db, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
    }
    cee_database_file_reference_last_parsed_time_set(db, file_path, NULL);
    cee_database_file_reference_symbols_count_set(db, file_path, 0);
    return TRUE;
}

cee_boolean cee_database_security_bookmark_append(cee_pointer db,
                                                  const cee_char* file_path,
                                                  const cee_char* content)
{
    if (!db || !file_path || !content)
        return FALSE;
    
    cee_boolean ret = FALSE;
    sqlite3_stmt* stmt = NULL;
    cee_char* sql = "INSERT INTO cee_security_bookmarks (file_path, content) VALUES (?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        goto exit;
    }
    
    sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, content, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE)
        goto exit;
    
    ret = TRUE;
    
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    
exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_security_bookmark_remove(cee_pointer db,
                                                  const cee_char* file_path)
{
    if (!db || !file_path)
        return FALSE;
    
    cee_boolean ret = FALSE;
    sqlite3_stmt* stmt = NULL;
    cee_char* sql = "DELETE FROM cee_security_bookmarks WHERE file_path=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        goto exit;
    }
    
    sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
        goto exit;
    
    ret = TRUE;
    
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return ret;
}

cee_char* cee_database_security_bookmark_content_get(cee_pointer db,
                                                     const cee_char* file_path)
{
    if (!db || !file_path)
        return NULL;
    
    cee_char* content = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "SELECT content FROM cee_security_bookmarks WHERE file_path=\"%s\";", file_path);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto exit;

    if (sqlite3_step(stmt) != SQLITE_ROW)
        goto exit;
    
    str = (char*)sqlite3_column_text(stmt, 0);
    if (!str)
        goto exit;
    
    length = sqlite3_column_bytes(stmt, 0);
    content = cee_strndup(str, length);

exit:
    if (stmt)
        sqlite3_finalize(stmt);
    
    return content;
}

CEEFileReferenceInfo* cee_file_reference_info_create()
{
    return (CEEFileReferenceInfo*)cee_malloc0(sizeof(CEEFileReferenceInfo));
}

void cee_file_reference_info_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEEFileReferenceInfo* info = (CEEFileReferenceInfo*)data;
    
    if (info->file_path)
        cee_free(info->file_path);
    
    if (info->last_parsed_time)
        cee_free(info->last_parsed_time);
    
    cee_free(info);
}

CEEList* cee_database_file_reference_infos_get(cee_pointer db)
{
    
    if (!db)
        return NULL;
    
    CEEList* infos = NULL;
    
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "SELECT file_path, name, last_parsed_time, symbols_count FROM cee_file_references ORDER BY name ASC;");
            
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        CEEFileReferenceInfo* info = cee_file_reference_info_create();
        
        str = (char*)sqlite3_column_text(stmt, 0);
        length = sqlite3_column_bytes(stmt, 0);
        info->file_path = cee_strndup(str, length);
        
        str = (char*)sqlite3_column_text(stmt, 2);
        length = sqlite3_column_bytes(stmt, 2);
        info->last_parsed_time = cee_strndup(str, length);
        
        info->symbol_count = sqlite3_column_int(stmt, 3);
        
        infos = cee_list_prepend(infos, info);
        
    } while (1);
    sqlite3_finalize(stmt);
    
    infos = cee_list_reverse(infos);
    
    return infos;
}
