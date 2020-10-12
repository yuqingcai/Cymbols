#include <stdlib.h>
#include <stdio.h>
#include "cee_backend.h"

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
    if (!existed)
        return FALSE;
    
    
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
    "   name           TEXT                                            ,"  \
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
    "CREATE INDEX IF NOT EXISTS cee_source_symbols_name_index on cee_source_symbols (name);";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
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

CEEList* cee_database_session_descriptors_get(cee_pointer db)
{
    if (!db)
        return NULL;
    
    CEEList* descriptors = NULL;
    cee_char* descriptor = NULL;
    char* str = NULL;
    cee_ulong length = 0;
    const char* sql = 
    "SELECT descriptor FROM cee_project_sessions;";
    
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
    
    return descriptors;
}

cee_boolean cee_database_session_descriptors_remove(cee_pointer db)
{
    if (!db)
        return FALSE;
    
    char *message = NULL;
    char* sql = "DELETE FROM cee_project_sessions;";
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
    
    cee_boolean ret = TRUE;
    sqlite3_stmt* stmt = NULL;
    char* sql =
    "INSERT INTO cee_project_sessions (descriptor) VALUES (?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
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
    char *message = NULL;
    char* sql =
    "INSERT INTO cee_project_file_paths (name, path) VALUES (?, ?);";
    
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

cee_boolean cee_database_filepaths_remove(cee_pointer db,
                                          CEEList* filepaths)
{
    if (!db || !filepaths)
        return FALSE;
    
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    char *message = NULL;
    char* sql =
    "DELETE FROM cee_project_file_paths WHERE path=?;";
    
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

void cee_database_symbols_clean(cee_pointer db)
{
    if (!db)
        return;
        
    char *message = NULL;
    char* sql = "DELETE FROM cee_source_symbols;";
    if (sqlite3_exec(db, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
    }
}

void cee_database_symbols_from_source_clean(cee_pointer db,
                                            const cee_char* filepath)
{
    if (!db || !filepath)
        return;
    
    char sql[4096];
    char *message = NULL;
    sprintf(sql, "DELETE FROM cee_source_symbols WHERE filepath=\"%s\";", filepath);
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
    char *message = NULL;
    char* sql =
    "INSERT INTO cee_source_symbols (type, name, parent, derives, protos, language, filepath, locations, fregment_range) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
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
        sqlite3_bind_text(stmt, 3, symbol->parent, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, symbol->derives, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, symbol->protos, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, symbol->language, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, symbol->filepath, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, symbol->locations, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 9, symbol->fregment_range, -1, SQLITE_STATIC);
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
    
    char* condition = NULL;
    cee_strconcat0(&condition, "name=", "'", name, "'", NULL);
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
    char* condition = NULL;
    cee_strconcat0(&condition, "parent=", "'", parent, "'", NULL);
    symbols = symbols_search(db, condition);
    cee_free(condition);
    return symbols;
}

CEEList* cee_database_symbols_search_by_type(cee_pointer db,
                                             const cee_char* type)
{
    if (!db || !type)
        return NULL;
    
    CEEList* symbols = NULL;
    char* condition = NULL;
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
    char* condition = NULL;
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
    char* sql = NULL;
    char* text = NULL;
    int length = 0;
    sqlite3_stmt* stmt;
    
    cee_strconcat0(&sql, 
                   "SELECT type, name, parent, derives, protos, language, filepath, locations fregment_range FROM cee_source_symbols WHERE ",
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
                                          NULL);
        
        /** type */
        symbol->type = sqlite3_column_int(stmt, 0);
                
        /** name */
        text = (char*)sqlite3_column_text(stmt, 1);
        length = sqlite3_column_bytes(stmt, 1);
        symbol->name = cee_strndup(text, length);
        
        /** parent */
        text = (char*)sqlite3_column_text(stmt, 2);
        length = sqlite3_column_bytes(stmt, 2);
        symbol->parent = cee_strndup(text, length);
        
        /** derives */
        text = (char*)sqlite3_column_text(stmt, 3);
        length = sqlite3_column_bytes(stmt, 3);
        symbol->derives = cee_strndup(text, length);
        
        /** protos */
        text = (char*)sqlite3_column_text(stmt, 4);
        length = sqlite3_column_bytes(stmt, 4);
        symbol->protos = cee_strndup(text, length);
        
        /** language */
        text = (char*)sqlite3_column_text(stmt, 5);
        length = sqlite3_column_bytes(stmt, 5);
        symbol->language = cee_strndup(text, length);
        
        /** filepath */
        text = (char*)sqlite3_column_text(stmt, 6);
        length = sqlite3_column_bytes(stmt, 6);
        symbol->filepath = cee_strndup(text, length);
        
        /** locations */
        text = (char*)sqlite3_column_text(stmt, 7);
        length = sqlite3_column_bytes(stmt, 7);
        symbol->locations = cee_strndup(text, length);
        
        /** fregment_range */
        text = (char*)sqlite3_column_text(stmt, 8);
        length = sqlite3_column_bytes(stmt, 8);
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
    
    char *message = NULL;
    char* sql = NULL;
    cee_strconcat0(&sql, 
                   "DELETE FROM cee_source_symbols WHERE filepath=",
                   filepath,
                   ";",
                   NULL);
    
    if (sqlite3_exec(db, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    return TRUE;
}

cee_boolean cee_database_security_bookmark_append(cee_pointer db,
                                                  const cee_char* filepath,
                                                  const cee_char* content)
{
    if (!db)
        return FALSE;
    
    cee_boolean ret = TRUE;
    sqlite3_stmt* stmt = NULL;
    char* sql =
    "INSERT INTO cee_security_bookmarks (filepath, content) VALUES (?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_bind_text(stmt, 1, filepath, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, content, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE)
        ret = FALSE;
    
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    
    return ret;
}

cee_boolean cee_database_security_bookmark_remove(cee_pointer db,
                                                  const cee_char* filepath)
{
    if (!db || !filepath)
        return FALSE;
    
    cee_boolean ret = TRUE;
    sqlite3_stmt* stmt = NULL;
    char* sql =
    "DELETE FROM cee_security_bookmarks WHERE filepath=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_bind_text(stmt, 1, filepath, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
        ret = FALSE;
    
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    
    return ret;
}

cee_char* cee_database_security_bookmark_content_get(cee_pointer db,
                                                     const cee_char* filepath)
{
    if (!db || !filepath)
        return NULL;
    
    char sql[4096];
    char* str = NULL;
    cee_ulong length = 0;
    sqlite3_stmt* stmt = NULL;
    sprintf(sql, "SELECT content FROM cee_security_bookmarks WHERE filepath=\"%s\";", filepath);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;

    if (sqlite3_step(stmt) != SQLITE_ROW)
        return NULL;
    
    str = (char*)sqlite3_column_text(stmt, 0);
    length = sqlite3_column_bytes(stmt, 0);
    if (!str)
        return NULL;
    
    return cee_strndup(str, length);
}


cee_boolean cee_database_filepaths_user_selected_append(cee_pointer db,
                                                        CEEList* filepaths)
{
    if (!db)
        return FALSE;
    
    CEEList* validateds = validate_filepaths_user_selected(db, filepaths);
    if (!validateds)
        return FALSE;
    
    cee_char* filepath = NULL;
    CEEList* p = NULL;
    sqlite3_stmt* stmt = NULL;
    char *message = NULL;
    char* sql =
    "INSERT INTO cee_project_file_paths_user_selected (filepath) VALUES (?);";
    
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
    char sql[4096];
    char* str = NULL;
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
