#include <stdlib.h>
#include <stdio.h>
#include "cee_backend.h"

#define SQL_MAX_LENGTH  4096

static cee_boolean tables_existed(sqlite3* db);
static cee_boolean tables_create(sqlite3* db);
static CEEList* validate_filepaths(cee_pointer db,
                                   CEEList* filepaths);
static CEEList* validate_filepaths_user_selected(cee_pointer db,
                                                 CEEList* filepaths);
static cee_pointer filepath_list_copy(const cee_pointer src,
                               cee_pointer data);
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
    
    /** cee_project_file_paths table*/
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_project_file_paths'";
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
        fprintf(stdout, "ERROR: cee_project_file_paths table not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='project_file_name_index'";
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
        fprintf(stdout, "ERROR: project_file_name_index index not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='project_file_path_index'";
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
        fprintf(stdout, "ERROR: project_file_path_index index not existed!");
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
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_source_symbols_name_index'";
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
        fprintf(stdout, "ERROR: cee_source_symbols_name_index index not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_source_symbols_filepath_index'";
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
        fprintf(stdout, "ERROR: cee_source_symbols_filepath_index index not existed!");
        return FALSE;
    }
    
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='index' AND name='cee_source_symbols_parent_index'";
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
        fprintf(stdout, "ERROR: cee_source_symbols_parent_index index not existed!");
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
    
    /** cee_project_file_paths_user_selected table*/
    existed = FALSE;
    sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='cee_project_file_paths_user_selected'";
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
        fprintf(stdout, "ERROR: cee_project_file_paths_user_selected table not existed!");
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
    
    /** cee_project_file_paths table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_project_file_paths ("                      \
    "   id                  INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ," \
    "   name                TEXT                                   NOT NULL ," \
    "   filepath            TEXT                                   NOT NULL ," \
    "   symbols_count       INTEGER                                NOT NULL ," \
    "   last_parsed_time    TEXT                                             " \
    ");";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS project_file_name_index on cee_project_file_paths (name);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS project_file_path_index on cee_project_file_paths (filepath);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    
    /** cee_source_symbols table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_source_symbols ("                  \
    "   id             INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   type           INTEGER                                         ,"  \
    "   name           TEXT                                            ,"  \
    "   alias          TEXT                                            ,"  \
    "   parent         TEXT                                            ,"  \
    "   derives        TEXT                                            ,"  \
    "   proto          TEXT                                            ,"  \
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
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_name_index on cee_source_symbols (name);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_alias_index on cee_source_symbols (alias);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_filepath_index on cee_source_symbols (filepath);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    sql =
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_parent_index on cee_source_symbols (parent);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    /** cee_security_bookmarks table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_security_bookmarks ("                  \
    "   id             INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   filepath       TEXT UNIQUE                            NOT NULL ,"  \
    "   content        TEXT                                   NOT NULL  "  \
    ");";
    
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    /** cee_project_file_paths_user_selected table*/
    sql =
    "CREATE TABLE IF NOT EXISTS cee_project_file_paths_user_selected ("    \
    "   id             INTEGER     PRIMARY KEY AUTOINCREMENT  NOT NULL ,"  \
    "   filepath       TEXT UNIQUE                            NOT NULL "   \
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

cee_boolean cee_database_filepaths_append(cee_pointer db,
                                          CEEList* filepaths)
{
    if (!db)
        return FALSE;
    
    CEEList* validateds = validate_filepaths(db, filepaths);
    if (!validateds)
        return FALSE;
    
    cee_char* filepath = NULL;
    cee_char* filename = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "INSERT INTO cee_project_file_paths (name, filepath, symbols_count) VALUES (?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    p = validateds;
    while (p) {
        filepath = p->data;
        filename = cee_path_basename(filepath);
        if (filename) {
            sqlite3_bind_text(stmt, 1, filename, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, filepath, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, 0);
            
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
    
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    cee_list_free_full(validateds, cee_free);
    
    return TRUE;
}

static CEEList* validate_filepaths(cee_pointer db,
                                   CEEList* filepaths)
{
    CEEList* existeds = NULL;
    CEEList* validateds = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    
    existeds = cee_database_filepaths_get(db, NULL);
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

static CEEList* validate_filepaths_user_selected(cee_pointer db,
                                                 CEEList* filepaths)
{
    CEEList* existeds = NULL;
    CEEList* validateds = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    
    existeds = cee_database_filepaths_user_selected_get(db);
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

CEEList* cee_database_filepaths_get(cee_pointer db,
                                    const cee_char* condition)
{
    if (!db)
        return NULL;
    
    CEEList* filepaths = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    if (condition)
        sprintf(sql, 
                "SELECT filepath, name FROM cee_project_file_paths WHERE name LIKE \"%%%s%%\" ORDER BY name ASC;",
                condition);
    else
        sprintf(sql, "SELECT filepath, name FROM cee_project_file_paths ORDER BY name ASC;");
            
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
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

cee_boolean cee_database_filepath_last_parsed_time_set(cee_pointer db,
                                                       const cee_char* filepath,
                                                       const cee_char* time_str)
{
    if (!db || !filepath)
        return FALSE;
    
    cee_boolean ret = FALSE;
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    
    if (!time_str)
        sprintf(sql, "UPDATE cee_project_file_paths set last_parsed_time=NULL WHERE filepath=\"%s\";",
                filepath);
    else
        sprintf(sql, "UPDATE cee_project_file_paths set last_parsed_time=\"%s\" WHERE filepath=\"%s\";",
                time_str,
                filepath);
    
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

cee_char* cee_database_filepath_last_parsed_time_get(cee_pointer db,
                                                     const cee_char* filepath)
{
    if (!db || !filepath)
        return NULL;
        
    cee_char* time_str = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    sprintf(sql, "SELECT last_parsed_time FROM cee_project_file_paths WHERE filepath=\"%s\";", filepath);
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

cee_boolean cee_database_filepath_symbols_count_set(cee_pointer db,
                                                    const cee_char* filepath,
                                                    cee_int symbols_count)
{
    if (!db || !filepath)
        return FALSE;
    
    cee_boolean ret = FALSE;
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "UPDATE cee_project_file_paths set symbols_count=%d WHERE filepath=\"%s\";", 
            symbols_count,
            filepath);
    
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

cee_int cee_database_filepath_symbols_count_get(cee_pointer db,
                                                const cee_char* filepath)
{
    if (!db || !filepath)
        return -1;
        
    cee_char sql[SQL_MAX_LENGTH];
    sqlite3_stmt* stmt = NULL;
    cee_int symbols_count = -1;
    sprintf(sql, "SELECT symbols_count FROM cee_project_file_paths WHERE filepath=\"%s\";", filepath);
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

cee_boolean cee_database_filepaths_remove(cee_pointer db,
                                          CEEList* filepaths)
{
    if (!db || !filepaths)
        return FALSE;
    
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "DELETE FROM cee_project_file_paths WHERE filepath=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    
    p = filepaths;
    while (p) {
        sqlite3_bind_text(stmt, 1, p->data, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE)
            break;
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        
        p = p->next;
    }
    
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    return TRUE;
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
    sql = "UPDATE cee_project_file_paths set last_parsed_time=NULL;";
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
                        "type, name, alias, parent, derives, proto, language, filepath, locations, fregment_range" \
                    ")" \
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    int ret = SQLITE_OK;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    
    CEEList* p = symbols;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        
        sqlite3_bind_int (stmt, 1, symbol->type);
        sqlite3_bind_text(stmt, 2, symbol->name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, symbol->alias, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, symbol->parent, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, symbol->derives, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, symbol->proto, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, symbol->language, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, symbol->filepath, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 9, symbol->locations, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 10, symbol->fregment_range, -1, SQLITE_STATIC);
        ret = sqlite3_step(stmt);
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        
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

CEEList* cee_database_symbols_search_by_filepath(cee_pointer db,
                                                 const cee_char* filepath)
{
    if (!db || !filepath)
        return NULL;
    
    CEEList* symbols = NULL;
    cee_char* condition = NULL;
    cee_strconcat0(&condition, "filepath=", "'", filepath, "'", NULL);
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
    
    cee_strconcat0(&sql, 
                   "SELECT type, name, alias, parent, derives, proto, language, filepath, locations fregment_range FROM cee_source_symbols WHERE ",
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
                                          NULL,
                                          NULL);
        
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
        
        /** proto */
        text = (char*)sqlite3_column_text(stmt, 5);
        length = sqlite3_column_bytes(stmt, 5);
        symbol->proto = cee_strndup(text, length);
        
        /** language */
        text = (char*)sqlite3_column_text(stmt, 6);
        length = sqlite3_column_bytes(stmt, 6);
        symbol->language = cee_strndup(text, length);
        
        /** filepath */
        text = (char*)sqlite3_column_text(stmt, 7);
        length = sqlite3_column_bytes(stmt, 7);
        symbol->filepath = cee_strndup(text, length);
        
        /** locations */
        text = (char*)sqlite3_column_text(stmt, 8);
        length = sqlite3_column_bytes(stmt, 8);
        symbol->locations = cee_strndup(text, length);
        
        /** fregment_range */
        text = (char*)sqlite3_column_text(stmt, 9);
        length = sqlite3_column_bytes(stmt, 9);
        symbol->fregment_range = cee_strndup(text, length);
        
        symbols = cee_list_prepend(symbols, symbol);
        
    } while (1);
    
    sqlite3_finalize(stmt);
    
    cee_free(sql);
    
    return symbols;
}

cee_boolean cee_database_symbols_delete_by_filepath(cee_pointer db,
                                                    const cee_char* filepath)
{    
    if (!db || !filepath)
        return FALSE;
    
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* message = NULL;
    sprintf(sql, "DELETE FROM cee_source_symbols WHERE filepath=\"%s\";", filepath);
    if (sqlite3_exec(db, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
    }
    cee_database_filepath_last_parsed_time_set(db, filepath, NULL);
    cee_database_filepath_symbols_count_set(db, filepath, 0);
    return TRUE;
}

cee_boolean cee_database_security_bookmark_append(cee_pointer db,
                                                  const cee_char* filepath,
                                                  const cee_char* content)
{
    if (!db || !filepath || !content)
        return FALSE;
    
    cee_boolean ret = FALSE;
    sqlite3_stmt* stmt = NULL;
    cee_char* sql = "INSERT INTO cee_security_bookmarks (filepath, content) VALUES (?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        goto exit;
    }
    
    sqlite3_bind_text(stmt, 1, filepath, -1, SQLITE_STATIC);
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
                                                  const cee_char* filepath)
{
    if (!db || !filepath)
        return FALSE;
    
    cee_boolean ret = FALSE;
    sqlite3_stmt* stmt = NULL;
    cee_char* sql = "DELETE FROM cee_security_bookmarks WHERE filepath=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        goto exit;
    }
    
    sqlite3_bind_text(stmt, 1, filepath, -1, SQLITE_STATIC);
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
                                                     const cee_char* filepath)
{
    if (!db || !filepath)
        return NULL;
    
    cee_char* content = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "SELECT content FROM cee_security_bookmarks WHERE filepath=\"%s\";", filepath);
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


cee_boolean cee_database_filepaths_user_selected_append(cee_pointer db,
                                                        CEEList* filepaths)
{
    if (!db || !filepaths)
        return FALSE;
    
    CEEList* validateds = validate_filepaths_user_selected(db, filepaths);
    if (!validateds)
        return FALSE;
    
    cee_char* filepath = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    cee_char* message = NULL;
    cee_char* sql = "INSERT INTO cee_project_file_paths_user_selected (filepath) VALUES (?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &message);
    p = validateds;
    while (p) {
        filepath = p->data;
        if (filepath) {
            sqlite3_bind_text(stmt, 1, filepath, -1, SQLITE_STATIC);
            
            if (sqlite3_step(stmt) != SQLITE_DONE)
                break;
            
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
        p = p->next;
    }
    
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    
    cee_list_free_full(validateds, cee_free);
    
    return TRUE;
}

CEEList* cee_database_filepaths_user_selected_get(cee_pointer db)
{
    
    if (!db)
        return NULL;
    
    CEEList* filepaths = NULL;
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "SELECT filepath FROM cee_project_file_paths_user_selected;");
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
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

CEEProjectFilePathEntryInfo* cee_project_file_path_entry_info_create()
{
    return (CEEProjectFilePathEntryInfo*)cee_malloc0(sizeof(CEEProjectFilePathEntryInfo));
}

void cee_project_file_path_entry_info_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEEProjectFilePathEntryInfo* info = (CEEProjectFilePathEntryInfo*)data;
    
    if (info->file_path)
        cee_free(info->file_path);
    
    if (info->last_parsed_time)
        cee_free(info->last_parsed_time);
    
    cee_free(info);
}

CEEList* cee_database_filepath_entry_infos_get(cee_pointer db)
{
    
    if (!db)
        return NULL;
    
    CEEList* infos = NULL;
    
    cee_char sql[SQL_MAX_LENGTH];
    cee_char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    
    sprintf(sql, "SELECT filepath, name, last_parsed_time, symbols_count FROM cee_project_file_paths ORDER BY name ASC;");
            
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        CEEProjectFilePathEntryInfo* info = cee_project_file_path_entry_info_create();
        
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
