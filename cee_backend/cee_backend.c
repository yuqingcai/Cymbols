#include <stdlib.h>
#include <stdio.h>
#include "cee_backend.h"

static cee_boolean tables_existed(sqlite3* database);
static cee_boolean tables_create(sqlite3* database);
static CEEList* validate_filepaths(cee_pointer database,
                                   CEEList* filepaths);
static cee_pointer filepath_list_copy(const cee_pointer src,
                               cee_pointer data);
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


static cee_boolean tables_existed(sqlite3* database)
{
    cee_boolean existed = FALSE;
    sqlite3_stmt* stmt = NULL;
    const char* sql = NULL;
    
    sql =
    "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_project_file_paths'";
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
    
    if (!existed)
        return FALSE;
    
    existed = FALSE;
    sql =
    "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_project_sessions'";
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
    
    if (!existed)
        return FALSE;
    
    existed = FALSE;
    sql =
    "SELECT name FROM sqlite_master WHERE type='index' AND name='project_file_paths_name_index'";
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
    
    if (!existed)
        return FALSE;
    
    return TRUE;
}

static cee_boolean tables_create(sqlite3* database)
{
    char *message = NULL;
    const char* sql = NULL;
    
    sql =
    "CREATE TABLE IF NOT EXISTS cee_project_file_paths ("                  \
    "   id             INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   name           TEXT                                   NOT NULL ,"  \
    "   path           TEXT                                   NOT NULL "   \
    ");";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS project_file_paths_name_index on cee_project_file_paths (name);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE TABLE IF NOT EXISTS cee_project_sessions ("                         \
    "   id                 INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   descriptor         TEXT                                            "   \
    ");";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    
    sql =
    "CREATE TABLE IF NOT EXISTS cee_source_symbols ("                  \
    "   id             INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   type           INTEGER                                         ,"  \
    "   descriptor     TEXT                                            ,"  \
    "   parent         TEXT                                            ,"  \
    "   derives        TEXT                                            ,"  \
    "   protos         TEXT                                            ,"  \
    "   language       TEXT                                            ,"  \
    "   filepath       TEXT                                            ,"  \
    "   locations      TEXT                                            ,"  \
    "   fregment_range TEXT                                             "  \
    ");";
    
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_descriptor_index on cee_source_symbols (descriptor);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    return TRUE;
}

CEEList* cee_database_session_descriptors_get(cee_pointer database)
{
    if (!database)
        return NULL;
    
    CEEList* descriptors = NULL;
    cee_char* descriptor = NULL;
    char* str = NULL;
    cee_ulong length = 0;
    const char* sql =
    "SELECT descriptor FROM cee_project_sessions;";
    
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
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
    
    return descriptors;
}

cee_boolean cee_database_session_descriptors_remove(cee_pointer database)
{
    if (!database)
        return FALSE;
    
    char *message = NULL;
    char* sql = "DELETE FROM cee_project_sessions;";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    return TRUE;
}

cee_boolean cee_database_session_descriptor_append(cee_pointer database,
                                                   const cee_char* descriptor)
{
    if (!database || !descriptor)
        return FALSE;
    
    cee_boolean ret = TRUE;
    sqlite3_stmt* stmt = NULL;
    char* sql =
    "INSERT INTO cee_project_sessions (descriptor) VALUES (?);";
    
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_bind_text(stmt, 1, descriptor, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE)
        ret = FALSE;
    
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_filepaths_append(cee_pointer database,
                                          CEEList* filepaths)
{
    if (!database)
        return FALSE;
    
    CEEList* validateds = validate_filepaths(database, filepaths);
    if (!validateds)
        return FALSE;
    
    cee_char* filepath = NULL;
    cee_char* filename = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    char *message = NULL;
    char* sql =
    "INSERT INTO cee_project_file_paths (name, path) VALUES (?, ?);";
    
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, &message);
    p = validateds;
    while (p) {
        filepath = p->data;
        filename = cee_path_basename(filepath);
        if (filename) {
            sqlite3_bind_text(stmt, 1, filename, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, filepath, -1, SQLITE_STATIC);
            
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                cee_free(filename);
                break;
            }
            
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
            cee_free(filename);
        }
        p = p->next;
    }
    
    sqlite3_exec(database, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    cee_list_free_full(validateds, cee_free);
    
    return TRUE;
}

static CEEList* validate_filepaths(cee_pointer database,
                                   CEEList* filepaths)
{
    CEEList* existeds = NULL;
    CEEList* validateds = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    
    existeds = cee_database_filepaths_get(database, NULL);
    if (!existeds)
        return cee_list_copy_deep(filepaths, 
                                  filepath_list_copy, 
                                  NULL);
    
    p = filepaths;
    while (p) {
        cee_boolean found = FALSE;
        q = existeds;
        while (q) {
            if (!cee_strcmp(p->data, q->data, TRUE)) {
                found = TRUE;
                break;
            }
            q = q->next;
        }
        if (!found)
            validateds = cee_list_prepend(validateds, cee_strdup(p->data));
        p = p->next;
    }
    
    return validateds;
}

static cee_pointer filepath_list_copy(const cee_pointer src,
                                      cee_pointer data)
{
    return cee_strdup(src);
}

CEEList* cee_database_filepaths_get(cee_pointer database,
                                    const cee_char* condition)
{
    if (!database)
        return NULL;
    
    CEEList* filepaths = NULL;
    char sql[4096];
    char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    if (condition)
        sprintf(sql, 
                "SELECT path, name FROM cee_project_file_paths WHERE name LIKE \"%%%s%%\" ORDER BY name ASC;",
                condition);
    else
        sprintf(sql, "SELECT path, name FROM cee_project_file_paths ORDER BY name ASC;");
            
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        str = (char*)sqlite3_column_text(stmt, 0);
        length = sqlite3_column_bytes(stmt, 0);
        filepaths = cee_list_prepend(filepaths, cee_strndup(str, length));
        
    } while (1);
    sqlite3_finalize(stmt);
    
    filepaths = cee_list_reverse(filepaths);
    
    return filepaths;
}

cee_boolean cee_database_filepaths_remove(cee_pointer database,
                                          CEEList* filepaths)
{
    if (!database || !filepaths)
        return FALSE;
    
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    char *message = NULL;
    char* sql =
    "DELETE FROM cee_project_file_paths WHERE path=?;";
    
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, &message);
    
    p = filepaths;
    while (p) {
        sqlite3_bind_text(stmt, 1, p->data, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE)
            break;
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        
        p = p->next;
    }
    
    
    sqlite3_exec(database, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    return TRUE;
}
