#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include "cee_datetime.h"
#include "cee_symbol.h"
#include "cee_symbol_cache.h"

static CEEList* symbols_search(sqlite3* database,
                               const cee_char* condition)
{
    if (!database || !condition)
        return NULL;
    
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    char* sql = NULL;
    char* text = NULL;
    int length = 0;
    sqlite3_stmt* stmt;
    
    cee_strconcat0(&sql, 
                   "SELECT type, descriptor, parent, derives, protos, language, filepath, locations fregment_range FROM cee_source_symbols WHERE ",
                   condition, 
                   ";",
                   NULL);
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;
    
    do {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            break;
        
        symbol = cee_symbol_create(kCEESourceSymbolTypeUnknow,
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
                
        /** descriptor */
        text = (char*)sqlite3_column_text(stmt, 1);
        length = sqlite3_column_bytes(stmt, 1);
        symbol->descriptor = cee_strndup(text, length);
        
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

void cee_symbols_clean(cee_pointer db)
{
    sqlite3* database = (sqlite3*)db;
    if (!database)
        return;
        
    char *message = NULL;
    char* sql = "DELETE FROM cee_source_symbols;";
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
    }
}

cee_boolean cee_symbols_write(cee_pointer db,
                              CEEList* symbols)
{
    sqlite3* database = (sqlite3*)db;
    if (!database)
        return FALSE;
    
    sqlite3_stmt* stmt = NULL;
    char *message = NULL;
    char* sql =
    "INSERT INTO cee_source_symbols (type, descriptor, parent, derives, protos, language, filepath, locations, fregment_range) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    int ret = SQLITE_OK;
    
    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "error");
        return FALSE;
    }
    
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, &message);
    
    CEEList* p = symbols;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        
        sqlite3_bind_int (stmt, 1, symbol->type);
        sqlite3_bind_text(stmt, 2, symbol->descriptor, -1, SQLITE_STATIC);
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
    
    sqlite3_exec(database, "END TRANSACTION", NULL, NULL, &message);
    sqlite3_finalize(stmt);
    return TRUE;
}

CEEList* cee_symbols_search_by_descriptor(cee_pointer db,
                                          const cee_char* descriptor,
                                          CEESymbolCacheRef cache)
{
    sqlite3* database = (sqlite3*)db;
    if (!database || !descriptor)
        return NULL;
    
    CEEList* symbols = NULL;
    char* condition = NULL;
    
    //cee_ulong m0 = 0;
    //cee_ulong m1 = 0; 
    
    if (cache) {
        //m0 = cee_timestamp_ms();
        symbols = cee_symbol_cache_search_by_descriptor(cache, descriptor);
        //m1 = cee_timestamp_ms();
        if (symbols) {
            //fprintf(stdout, "hit cache: %s cost: %lu ms\n", descriptor, m1 - m0);
            return symbols;
        }
    }
        
    //m0 = cee_timestamp_ms();
    cee_strconcat0(&condition, "descriptor=", "'", descriptor, "'", NULL);
    symbols = symbols_search(database, condition);
    //m1 = cee_timestamp_ms();
    //fprintf(stdout, "found: %s cost: %lu ms\n", descriptor, m1 - m0);
    
    cee_free(condition);
        
    if (symbols)
        cee_symbol_cache_append(cache, descriptor, symbols);
        
    return symbols;
}

CEEList* cee_symbols_search_by_parent(cee_pointer db,
                                      const cee_char* parent)
{
    sqlite3* database = (sqlite3*)db;
    if (!database || !parent)
        return NULL;
    
    CEEList* symbols = NULL;
    char* condition = NULL;
    cee_strconcat0(&condition, "parent=", "'", parent, "'", NULL);
    symbols = symbols_search(database, condition);
    cee_free(condition);
    return symbols;
}

CEEList* cee_symbols_search_by_type(cee_pointer db,
                                    const cee_char* type)
{
    sqlite3* database = (sqlite3*)db;
    if (!database || !type)
        return NULL;
    
    CEEList* symbols = NULL;
    char* condition = NULL;
    cee_strconcat0(&condition, "type=", "'", type, "'", NULL);
    symbols = symbols_search(database, condition);
    cee_free(condition);
    return symbols;
}

CEEList* cee_symbols_search_by_filepath(cee_pointer db,
                                        const cee_char* filepath)
{
    sqlite3* database = (sqlite3*)db;
    if (!database || !filepath)
        return NULL;
    
    CEEList* symbols = NULL;
    char* condition = NULL;
    cee_strconcat0(&condition, "filepath=", "'", filepath, "'", NULL);
    symbols = symbols_search(database, condition);
    cee_free(condition);
    return symbols;
}

cee_boolean cee_symbols_delete_by_filepath(cee_pointer db,
                                           const cee_char* filepath)
{
    sqlite3* database = (sqlite3*)db;
    
    if (!database || !filepath)
        return FALSE;
    
    char *message = NULL;
    char* sql = NULL;
    cee_strconcat0(&sql, 
                   "DELETE FROM cee_source_symbols WHERE filepath=",
                   filepath,
                   ";",
                   NULL);
    
    if (sqlite3_exec(database, sql, NULL, NULL, &message) != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", message);
        sqlite3_free(message);
        return FALSE;
    }
    
    return TRUE;
}

void cee_symbol_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEESourceSymbol* symbol = (CEESourceSymbol*)data;
    
    if (symbol->descriptor)
        cee_free(symbol->descriptor);
    
    if (symbol->parent)
        cee_free(symbol->parent);
    
    if (symbol->derives)
        cee_free(symbol->derives);
    
    if (symbol->protos)
        cee_free(symbol->protos);
    
    if (symbol->filepath)
        cee_free(symbol->filepath);
    
    if (symbol->language)
        cee_free(symbol->language);
    
    if (symbol->locations)
        cee_free(symbol->locations);
    
    if (symbol->fregment_range)
        cee_free(symbol->fregment_range);
    
    cee_free(symbol);
}

CEESourceSymbol* cee_symbol_create(CEESourceSymbolType type,
                                   const cee_char* descriptor,
                                   const cee_char* parent,
                                   const cee_char* derives,
                                   const cee_char* protos,
                                   const cee_char* language,
                                   const cee_char* filepath,
                                   const cee_char* locations,
                                   const cee_char* fregment_range)
{
    CEESourceSymbol* symbol = cee_malloc0(sizeof(CEESourceSymbol));
    
    symbol->type = type;
    
    if (descriptor)
        symbol->descriptor = cee_strdup(descriptor);
    
    if (parent)
        symbol->parent = cee_strdup(parent);
    
    if (derives)
        symbol->derives = cee_strdup(derives);
    
    if (protos)
        symbol->protos = cee_strdup(protos);
    
    if (language)
        symbol->language = cee_strdup(language);
    
    if (filepath)
        symbol->filepath = cee_strdup(filepath);
    
    if (locations)
        symbol->locations = cee_strdup(locations);
    
    if (fregment_range)
        symbol->fregment_range = cee_strdup(fregment_range);
    
    return symbol;
}

CEESourceSymbol* cee_symbol_copy(CEESourceSymbol* symbol)
{
    CEESourceSymbol* copy = cee_malloc0(sizeof(CEESourceSymbol));
    
    copy->type = symbol->type;
    copy->descriptor = cee_strdup(symbol->descriptor);
    copy->language = cee_strdup(symbol->language);
    copy->filepath = cee_strdup(symbol->filepath);
    copy->parent = cee_strdup(symbol->parent);
    copy->derives = cee_strdup(symbol->derives);
    copy->protos = cee_strdup(symbol->protos);
    copy->locations = cee_strdup(symbol->locations);
    copy->fregment_range = cee_strdup(symbol->fregment_range);
    
    return copy;
}

void cee_symbol_dump(CEESourceSymbol* symbol)
{
    if (!symbol) {
        fprintf(stdout, "symbol dump error: symbol is NULL\n");
        return ;
    }
    
    const cee_char* descriptor = "?";
    const cee_char* protos = "?";
    const cee_char* derives = "?";
    const cee_char* filepath = "?";
    const cee_char* locations = "?";
    const cee_char* fregment_range = "?";
    
    if (symbol->descriptor)
        descriptor = symbol->descriptor;
    
    if (symbol->protos)
        protos = symbol->protos;
    
    if (symbol->derives)
        derives = symbol->derives;
    
    if (symbol->locations)
        locations = symbol->locations;
    
    if (symbol->filepath)
        filepath = symbol->filepath;
    
    if (symbol->fregment_range)
        filepath = symbol->fregment_range;
    
    fprintf(stdout, "%s %s %s %s %s %s\n",
            descriptor,
            protos,
            locations,
            derives, 
            filepath,
            fregment_range);
}

void cee_symbols_dump(CEEList* symbols)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    p = symbols;
    while (p) {
        symbol = p->data;
        cee_symbol_dump(symbol);
        p = p->next;
    }
}

CEESourceSymbolType cee_symbol_database_search(cee_pointer database, 
                                               const cee_uchar* subject, 
                                               cee_long offset, 
                                               cee_ulong length)
{
    return kCEESourceSymbolTypeUnknow;
}

CEESourceSymbol* cee_symbol_from_token_slice(const cee_uchar* filepath,
                                             const cee_uchar* subject,
                                             CEEList* begin,
                                             CEEList* end,
                                             CEESourceSymbolType type,
                                             const cee_char* language)
{
    CEESourceSymbol* symbol = cee_malloc0(sizeof(CEESourceSymbol));
    symbol->filepath = cee_strdup((cee_char*)filepath);
    symbol->type = type;
    symbol->language = cee_strdup(language);
    symbol->descriptor = cee_string_from_token_slice(subject, 
                                                     begin, 
                                                     end, 
                                                     kCEETokenStringOptionCompact);
    symbol->locations = cee_ranges_string_from_token_slice(begin,
                                                           end,
                                                           kCEERangeListTypeContinue);
    return symbol;
}

CEESourceSymbol* cee_symbol_from_tokens(const cee_uchar* filepath,
                                        const cee_uchar* subject,
                                        CEEList* tokens,
                                        CEESourceSymbolType type,
                                        const cee_char* language)
{
    CEESourceSymbol* symbol = cee_malloc0(sizeof(CEESourceSymbol));
    symbol->filepath = cee_strdup((cee_char*)filepath);
    symbol->type = type;
    symbol->language = cee_strdup(language);
    symbol->descriptor = cee_string_from_tokens(subject, 
                                                TOKEN_FIRST(tokens), 
                                                kCEETokenStringOptionCompact);
    symbol->locations = cee_ranges_string_from_tokens(TOKEN_FIRST(tokens),
                                                      kCEERangeListTypeSeparate);
    return symbol;
    
}

cee_int cee_symbol_location_compare(const cee_pointer a,
                                    const cee_pointer b)
{
    cee_int ret = 0;
    CEESourceSymbol* symbol0 = (CEESourceSymbol*)a;
    CEESourceSymbol* symbol1 = (CEESourceSymbol*)b;
    CEEList* ranges0 = NULL;
    CEEList* ranges1 = NULL;
    CEERange* range0 = NULL;
    CEERange* range1 = NULL;
    
    if (!symbol0->locations || !symbol1->locations)
        goto exit;
    
    ranges0 = cee_ranges_from_string(symbol0->locations);
    ranges1 = cee_ranges_from_string(symbol1->locations);
    
    if (!ranges0 || !ranges1)
        goto exit;
    
    range0 = ranges0->data;
    range1 = ranges1->data;
    
    if (range0->location < range1->location)
        ret = -1;
    else if (range0->location > range1->location)
        ret = 1;
    else
        ret = 0;

exit:
    
    if (ranges0)
        cee_list_free_full(ranges0, cee_range_free);
    
    if (ranges1)
        cee_list_free_full(ranges1, cee_range_free);
    
    return ret;
}

CEESourceReference* cee_reference_create(const cee_uchar* subject,
                                         CEEList* tokens,
                                         CEESourceReferenceType type)
{
    CEESourceReference* reference = cee_malloc0(sizeof(CEESourceReference));
    reference->type = type;
    reference->tokens_ref = tokens;
    reference->descriptor = cee_string_from_tokens(subject,
                                                   TOKEN_FIRST(tokens),
                                                   kCEETokenStringOptionCompact);
    reference->locations = cee_ranges_from_tokens(TOKEN_FIRST(tokens), 
                                                  kCEERangeListTypeSeparate);
    return reference;
}

void cee_reference_free(cee_pointer data)
{
    if (!data)
        return;
    CEESourceReference* reference = (CEESourceReference*)data;
    
    if (reference->tokens_ref)
        cee_list_free(reference->tokens_ref);
    
    if (reference->descriptor)
        cee_free(reference->descriptor);
    
    if (reference->locations)
        cee_list_free_full(reference->locations, cee_range_free);
    
    cee_free(reference);
}


cee_boolean cee_symbol_is_block_type(CEESourceSymbol* symbol)
{
    return (symbol->type == kCEESourceSymbolTypeFunctionDefinition ||
            symbol->type == kCEESourceSymbolTypeMessageDefinition ||
            symbol->type == kCEESourceSymbolTypeClassDefinition ||
            symbol->type == kCEESourceSymbolTypeEnumDefinition ||
            symbol->type == kCEESourceSymbolTypeUnionDefinition ||
            symbol->type == kCEESourceSymbolTypeNamespaceDefinition ||
            symbol->type == kCEESourceSymbolTypeInterfaceDeclaration ||
            symbol->type == kCEESourceSymbolTypeImplementationDefinition ||
            symbol->type == kCEESourceSymbolTypeProtocolDeclaration ||
            symbol->type == kCEESourceSymbolTypeExternBlock);
}
