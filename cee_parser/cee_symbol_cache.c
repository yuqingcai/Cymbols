#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_symbol_cache.h"
#include "cee_symbol.h"

typedef struct _CEESymbolCacheItem {
    cee_char* descriptor;
    CEEList* symbols;
} CEESymbolCacheItem;

typedef struct _CEESymbolCache {
    CEEList* items;
} CEESymbolCache;

static CEESymbolCacheItem* cache_item_create(void);
static void cache_item_free(cee_pointer data);


CEESymbolCacheRef cee_symbol_cache_create(void)
{
    return cee_malloc0(sizeof(CEESymbolCache));
}

void cee_symbol_cache_free(cee_pointer data)
{
    CEESymbolCache* cache = (CEESymbolCache*)data;
    cee_list_free_full(cache->items, cache_item_free);
    cee_free(cache);
}

static CEESymbolCacheItem* cache_item_create(void)
{
    return cee_malloc0(sizeof(CEESymbolCacheItem));
}

static void cache_item_free(cee_pointer data)
{
    CEESymbolCacheItem* item = (CEESymbolCacheItem*)data;
    if (item->descriptor)
        cee_free(item->descriptor);
    
    if (item->symbols)
        cee_list_free_full(item->symbols, cee_symbol_free);
    
    cee_free(item);
}

static CEEList* cache_item_symbols_copy(CEESymbolCacheItem* item)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* q = NULL;
    p = item->symbols;
    while (p) {
        symbol = p->data;
        q = cee_list_prepend(q, cee_symbol_copy(symbol));
        p = p->next;
    }
    return q;
}

CEEList* cee_symbol_cache_search_by_descriptor(CEESymbolCacheRef cache, 
                                               const cee_char* descirptor)
{
    CEEList* p = NULL;
    CEESymbolCacheItem* item = NULL;
    
    p = cache->items;
    while (p) {
        item = p->data;
        
        if (!strcmp(item->descriptor, descirptor)) 
            return cache_item_symbols_copy(item);
        
        p = p->next;
    }
    
    return NULL;
}

void cee_symbol_cache_append(CEESymbolCacheRef cache,
                             const cee_char* descriptor,
                             CEEList* symbols)
{
    CEESymbolCacheItem* item = NULL;
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    
    item = cache_item_create();
    item->descriptor = cee_strdup(descriptor);
    p = symbols;
    while (p) {
        symbol = p->data;
        item->symbols = cee_list_prepend(item->symbols, cee_symbol_copy(symbol));
        p = p->next;
    }
    
    cache->items = cee_list_prepend(cache->items, item);
    
}
