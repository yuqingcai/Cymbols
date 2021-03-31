#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include "cee_symbol.h"

static cee_boolean symbol_is_asm_label(CEESourceSymbol* symbol);
static void source_symbol_tree_dump_to_wrappers(CEETree* tree, 
                                                int n, 
                                                CEEList** list);
cee_int cee_source_symbol_count = 0;

CEESourceSymbol* cee_source_symbol_create(CEESourceSymbolType type,
                                          const cee_char* name,
                                          const cee_char* alias,
                                          const cee_char* parent,
                                          const cee_char* derives,
                                          const cee_char* proto_descriptor,
                                          const cee_char* language,
                                          const cee_char* file_path,
                                          cee_int line_no,
                                          CEEList* ranges,
                                          CEERange fregment_range)
{
    CEESourceSymbol* symbol = cee_malloc0(sizeof(CEESourceSymbol));
    
    symbol->type = type;
    
    if (name)
        symbol->name = cee_strdup(name);
    
    if (alias)
        symbol->alias = cee_strdup(alias);
    
    if (parent)
        symbol->parent = cee_strdup(parent);
    
    if (derives)
        symbol->derives = cee_strdup(derives);
    
    if (proto_descriptor)
        symbol->proto_descriptor = cee_strdup(proto_descriptor);
    
    if (language)
        symbol->language = cee_strdup(language);
    
    if (file_path)
        symbol->file_path = cee_strdup(file_path);
    
    symbol->line_no = line_no;
    symbol->ranges = ranges;
    symbol->fregment_range = fregment_range;
        
    cee_source_symbol_count ++;
    
    return symbol;
}

void cee_source_symbol_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEESourceSymbol* symbol = (CEESourceSymbol*)data;    
    
    if (symbol->name)
        cee_free(symbol->name);
    
    if (symbol->alias)
        cee_free(symbol->alias);
    
    if (symbol->language)
        cee_free(symbol->language);
        
    if (symbol->file_path)
        cee_free(symbol->file_path);
    
    if (symbol->parent)
        cee_free(symbol->parent);
    
    if (symbol->derives)
        cee_free(symbol->derives);
    
    if (symbol->proto_descriptor)
        cee_free(symbol->proto_descriptor);
    
    if (symbol->ranges)
        cee_list_free_full(symbol->ranges, cee_range_free);
        
    cee_free(symbol);
    
    cee_source_symbol_count --;
}

cee_int cee_source_symbol_count_get(void)
{
    return cee_source_symbol_count;
}

CEESourceSymbol* cee_source_symbol_copy(CEESourceSymbol* symbol)
{
    if (!symbol)
        return NULL;
    
    CEESourceSymbol* copy = cee_malloc0(sizeof(CEESourceSymbol));
    copy->type = symbol->type;
    copy->name = cee_strdup(symbol->name);
    copy->alias = cee_strdup(symbol->alias);
    copy->language = cee_strdup(symbol->language);
    copy->file_path = cee_strdup(symbol->file_path);
    copy->parent = cee_strdup(symbol->parent);
    copy->derives = cee_strdup(symbol->derives);
    copy->line_no = symbol->line_no;
    
    if (symbol->ranges)
        copy->ranges = cee_list_copy_deep(symbol->ranges, cee_range_list_copy, NULL);
    
    copy->fregment_range = symbol->fregment_range;
    copy->proto_descriptor = cee_strdup(symbol->proto_descriptor);
    
    return copy;
}

cee_pointer cee_source_symbol_copy_func(const cee_pointer src,
                                        cee_pointer data)
{
    return cee_source_symbol_copy((CEESourceSymbol*)src);
}

CEESourceSymbol* cee_source_symbol_create_from_token_slice(const cee_char* file_path,
                                                           const cee_char* subject,
                                                           CEEList* begin,
                                                           CEEList* end,
                                                           CEESourceSymbolType type,
                                                           const cee_char* language,
                                                           CEETokenStringOption option)
{
    CEEToken* token = begin->data;
    CEESourceSymbol* symbol = cee_source_symbol_create(type,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       0,
                                                       NULL,
                                                       cee_range_make(0, 0));
    symbol->file_path = cee_strdup(file_path);
    symbol->language = cee_strdup(language);
    symbol->name = cee_string_from_token_slice(subject,
                                               begin,
                                               end,
                                               option);
    symbol->line_no = token->line_no;
    symbol->ranges = cee_ranges_from_token_slice(begin,
                                                 end,
                                                 kCEERangeListTypeContinue);
    symbol->fregment_range = cee_range_consistent_from_discrete(symbol->ranges);
    return symbol;
}

CEESourceSymbol* cee_source_symbol_create_from_tokens(const cee_char* file_path,
                                                      const cee_char* subject,
                                                      CEEList* tokens,
                                                      CEESourceSymbolType type,
                                                      const cee_char* language,
                                                      CEETokenStringOption option)
{
    CEEToken* token = tokens->data;
    CEESourceSymbol* symbol = cee_source_symbol_create(type,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       0,
                                                       NULL,
                                                       cee_range_make(0, 0));
    symbol->file_path = cee_strdup(file_path);
    symbol->language = cee_strdup(language);
    symbol->name = cee_string_from_tokens(subject,
                                          TOKEN_FIRST(tokens),
                                          option);
    symbol->line_no = token->line_no;
    symbol->ranges = cee_ranges_from_tokens(TOKEN_FIRST(tokens), kCEERangeListTypeSeparate);
    symbol->fregment_range = cee_range_consistent_from_discrete(symbol->ranges);
    return symbol;
}

void cee_source_symbol_name_format(cee_char* name)
{
    cee_size len = strlen(name);
    cee_char* p = name;
    cee_size num = 0;
    while (*p) {
        if (*p == ' ' && *(p + 1) == ' ') {
            cee_char* q = p + 1;
            while (*q == ' ')
                q ++;
            
            num = name + len - q + 1; /** plus '\0' */
            memmove(p + 1, q, num);
        }
        p ++;
    }
}

void cee_source_symbol_print(CEESourceSymbol* symbol)
{
    if (!symbol) {
        fprintf(stdout, "symbol dump error: symbol is NULL\n");
        return ;
    }
    
    const cee_char* name = "?";
    const cee_char* protos = "?";
    const cee_char* derives = "?";
    const cee_char* filepath = "?";
    const cee_char* parent = "?";
    const cee_char* alias = "?";
    cee_char* ranges = NULL;
    cee_char* fregment_range = NULL;
    cee_int line_no = symbol->line_no;
    
    CEESourceSymbolType type = symbol->type;
    
    if (symbol->name)
        name = symbol->name;
    
    if (symbol->alias)
        alias = symbol->alias;
    
    if (symbol->proto_descriptor)
        protos = symbol->proto_descriptor;
    
    if (symbol->derives)
        derives = symbol->derives;
    
    if (symbol->file_path)
        filepath = symbol->file_path;
    
    if (symbol->ranges)
        ranges = cee_string_from_ranges(symbol->ranges);
    else
        ranges = cee_strdup("?");
    
    fregment_range = cee_string_from_range(&symbol->fregment_range);
    
    if (symbol->parent)
        parent = symbol->parent;
    
    fprintf(stdout,
            "%s %s %d %s %d %s %s %s %s %s\n",
            name,
            alias,
            type,
            protos,
            line_no,
            ranges,
            derives,
            filepath,
            fregment_range,
            parent);
    
    if (ranges)
        cee_free(ranges);
    
    if (fregment_range)
        cee_free(fregment_range);
}

void cee_source_symbols_print(CEEList* symbols)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    p = symbols;
    while (p) {
        symbol = p->data;
        cee_source_symbol_print(symbol);
        p = p->next;
    }
}

cee_boolean cee_source_symbol_is_block_type(CEESourceSymbol* symbol)
{
    return (symbol->type == kCEESourceSymbolTypeFunctionDefinition ||
            symbol->type == kCEESourceSymbolTypeMessageDefinition ||
            symbol->type == kCEESourceSymbolTypeClassDefinition ||
            symbol->type == kCEESourceSymbolTypeStructDefinition ||
            symbol->type == kCEESourceSymbolTypeEnumDefinition ||
            symbol->type == kCEESourceSymbolTypeUnionDefinition ||
            symbol->type == kCEESourceSymbolTypeNamespaceDefinition ||
            symbol->type == kCEESourceSymbolTypeInterfaceDeclaration ||
            symbol->type == kCEESourceSymbolTypeInterfaceDefinition ||
            symbol->type == kCEESourceSymbolTypeImplementationDefinition ||
            symbol->type == kCEESourceSymbolTypeProtocolDeclaration ||
            symbol->type == kCEESourceSymbolTypeExternBlock);
}

cee_int cee_source_symbol_location_compare(const cee_pointer a,
                                           const cee_pointer b)
{
    cee_int ret = 0;
    CEESourceSymbol* symbol0 = (CEESourceSymbol*)a;
    CEESourceSymbol* symbol1 = (CEESourceSymbol*)b;
    CEERange* range0 = NULL;
    CEERange* range1 = NULL;
    
    if (!symbol0->ranges || !symbol1->ranges)
        goto exit;
        
    range0 = cee_list_first(symbol0->ranges)->data;
    range1 = cee_list_first(symbol1->ranges)->data;
    
    if (!range0 || !range1)
        goto exit;
    
    ret = cee_range_compare(range0, range1);

exit:
    return ret;
}

CEESourceSymbol* cee_source_symbol_tree_search(CEETree* tree,
                                               CEECompareFunc matcher,
                                               cee_pointer user_data)
{
    if (!tree)
        return NULL;
    
    CEEList* p = NULL; 
    CEESourceSymbol* symbol = tree->data;
    if (symbol) {
        if (!matcher(symbol, user_data))
            return symbol;
    }
    
    if (tree->children) {
        p = CEE_TREE_CHILDREN_FIRST(tree);
        while (p) {
            tree = p->data;
            symbol = cee_source_symbol_tree_search(tree, 
                                                   matcher,
                                                   user_data);
            if (symbol)
                return symbol;
            
            p = CEE_TREE_NODE_NEXT(p);
        }
    }
    return NULL;
}

cee_int cee_source_symbol_matcher_by_buffer_offset(cee_pointer data,
                                                   cee_pointer user_data)
{
    if (!data || !user_data)
        return -1;
    
    CEESourceSymbol* symbol = (CEESourceSymbol*)data;
    cee_long offset = CEE_POINTER_TO_INT(user_data);
    CEERange range;
    if (symbol->ranges) {
        range = cee_range_consistent_from_discrete(symbol->ranges);
        if (cee_location_in_range(offset, range) ||
            cee_location_followed_range(offset, range))
            return 0;
    }
    return -1;
}

cee_int cee_source_symbol_matcher_by_name(cee_pointer data,
                                          cee_pointer user_data)
{
    if (!data || !user_data)
        return -1;
    
    CEESourceSymbol* symbol = (CEESourceSymbol*)data;
    cee_char* name = (cee_char*)user_data;
    if ((symbol->name && !strcmp(symbol->name, name)) ||
        (symbol->alias && !strcmp(symbol->alias, name)))
        return 0;
    
    return -1;
}

cee_boolean cee_source_symbols_are_equal(CEESourceSymbol* symbol0,
                                         CEESourceSymbol* symbol1)
{
    CEERange range0 = cee_range_consistent_from_discrete(symbol0->ranges);
    CEERange range1 = cee_range_consistent_from_discrete(symbol1->ranges);
    
    if (!strcmp(symbol0->file_path, symbol1->file_path) &&
        (range0.location == range1.location && range0.length == range1.length))
        return TRUE;
    
    return FALSE;
}

cee_boolean cee_source_symbol_is_definition(CEESourceSymbol* symbol)
{
    return (symbol->type == kCEESourceSymbolTypePrepDirectiveDefine || 
            symbol->type == kCEESourceSymbolTypeFunctionDefinition || 
            symbol->type == kCEESourceSymbolTypePropertyDeclaration ||
            symbol->type == kCEESourceSymbolTypeVariableDeclaration || 
            symbol->type == kCEESourceSymbolTypeMessageDefinition || 
            symbol->type == kCEESourceSymbolTypeTypeDefine || 
            symbol->type == kCEESourceSymbolTypeClassDefinition ||
            symbol->type == kCEESourceSymbolTypeStructDefinition ||
            symbol->type == kCEESourceSymbolTypeEnumDefinition || 
            symbol->type == kCEESourceSymbolTypeUnionDefinition || 
            symbol->type == kCEESourceSymbolTypeNamespaceDefinition || 
            symbol->type == kCEESourceSymbolTypeImplementationDefinition || 
            symbol->type == kCEESourceSymbolTypeProtocolDeclaration ||
            symbol_is_asm_label(symbol));
}

static cee_boolean symbol_is_asm_label(CEESourceSymbol* symbol)
{
    if (symbol->language &&
        (cee_strcmp(symbol->language, "gnu_asm", FALSE) ||
         cee_strcmp(symbol->language, "arm_asm", FALSE)))
    {
        if (symbol->type == kCEESourceSymbolTypeLabel)
            return TRUE;
    }
    return FALSE;
}

CEESourceSymbolWrapper* cee_source_symbol_wrapper_create(CEESourceSymbol* symbol,
                                                         cee_uint level)
{
    CEESourceSymbolWrapper* wrapper = cee_malloc0(sizeof(CEESourceSymbolWrapper));
    wrapper->symbol_ref = symbol;
    wrapper->level = level;
    return wrapper;
}

void cee_source_symbol_wrapper_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEESourceSymbolWrapper* wrapper = data;
    cee_free(wrapper);
}

void cee_source_symbol_tree_dump_to_wrappers(CEETree* tree, 
                                             CEEList** list)
{
    source_symbol_tree_dump_to_wrappers(tree, 0, list);
}

static void source_symbol_tree_dump_to_wrappers(CEETree* tree, 
                                                int n, 
                                                CEEList** list)

{
    if (!tree)
        return;
    
    CEEList* p = NULL;    
    CEESourceSymbol* symbol = tree->data;
    if (symbol) {
        CEESourceSymbolWrapper* wrapper = cee_source_symbol_wrapper_create(symbol, n);
        *list = cee_list_prepend(*list, wrapper);
    }
    
    n ++;
    if (tree->children) {
        p = CEE_TREE_CHILDREN_FIRST(tree);
        while (p) {
            tree = p->data;
            source_symbol_tree_dump_to_wrappers(tree, n, list);
            p = CEE_TREE_NODE_NEXT(p);
        }
    }
}

cee_int cee_source_symbol_wrapper_location_compare(const cee_pointer a,
                                                   const cee_pointer b)

{
    CEESourceSymbolWrapper* wrapper0 = (CEESourceSymbolWrapper*)a;
    CEESourceSymbolWrapper* wrapper1 = (CEESourceSymbolWrapper*)b;
    CEESourceSymbol* symbol0 = wrapper0->symbol_ref;
    CEESourceSymbol* symbol1 = wrapper1->symbol_ref;
    return cee_source_symbol_location_compare(symbol0, symbol1);
}


cee_pointer wrapper_copy(const cee_pointer src,
                         cee_pointer data)
{
    CEESourceSymbolWrapper* wrapper = src;
    return cee_source_symbol_wrapper_create(wrapper->symbol_ref, 
                                            wrapper->level);
}

CEEList* cee_source_symbol_wrappers_copy_with_condition(CEEList* wrappers,
                                                        const cee_char* conditoin)
{
    CEEList* filtered = NULL;
    CEEList* p = NULL;
    
    if (!conditoin || !strcmp(conditoin, "")) {
        filtered = cee_list_copy_deep(wrappers, wrapper_copy, NULL);
    }
    else {
        p = wrappers;
        while (p) {
            CEESourceSymbolWrapper* wrapper = p->data;
            CEESourceSymbol* symbol = wrapper->symbol_ref;
            if (strstr(symbol->name, conditoin)) {
                /** level 0 is always tree root */
                CEESourceSymbolWrapper* copy = cee_source_symbol_wrapper_create(symbol, 1);
                filtered = cee_list_prepend(filtered, copy);
            }
            p = p->next;
        }
        filtered = cee_list_reverse(filtered);
    }
    return filtered;
}

CEESourceSymbolCache* cee_source_symbol_cache_create(CEESourceSymbol* symbol,
                                                     cee_long referenced_location)
{
    CEESourceSymbolCache* cache = (CEESourceSymbolCache*)cee_malloc0(sizeof(CEESourceSymbolCache));
    cache->symbol = symbol;
    if (referenced_location != -1)
        cache->referened_locations =
            cee_list_prepend(cache->referened_locations, CEE_LONG_TO_POINTER(referenced_location));
    return cache;
}

void cee_source_symbol_cache_free(cee_pointer data)
{
    CEESourceSymbolCache* cache = (CEESourceSymbolCache*)data;
    if (cache->symbol)
        cee_source_symbol_free(cache->symbol);
    if (cache->referened_locations)
        cee_list_free(cache->referened_locations);
    
    cee_free(cache);
}

CEESourceSymbolCache* cee_source_symbol_cached(CEEList* caches,
                                               CEESourceSymbol* symbol)
{
    CEEList* p = caches;
    while (p) {
        CEESourceSymbolCache* cache = p->data;
        CEESourceSymbol* cached_symbol = cache->symbol;
        
        if (cached_symbol->name && symbol->name && !strcmp(cached_symbol->name, symbol->name) &&
            cached_symbol->file_path && symbol->file_path && !strcmp(cached_symbol->file_path, symbol->file_path)) {
            CEERange range0 = cee_range_consistent_from_discrete(cached_symbol->ranges);
            CEERange range1 = cee_range_consistent_from_discrete(symbol->ranges);
            if (range0.location == range1.location)
                return cache;
        }
        
        p = p->next;
    }
    return NULL;
}

void cee_source_symbol_cache_location(CEESourceSymbolCache* cache,
                                      cee_long referenced_location)
{
    if (!cache)
        return ;
    cache->referened_locations = cee_list_prepend(cache->referened_locations,
                                                  CEE_LONG_TO_POINTER(referenced_location));
}


cee_boolean cee_source_symbol_cache_name_hit(CEESourceSymbolCache* cache,
                                             const cee_char* name)
{
    if (!cache)
        return FALSE;
    
    CEESourceSymbol* symbol = cache->symbol;
    
    if (symbol->name && name && !strcmp(symbol->name, name))
        return TRUE;
    
    return FALSE;
}


cee_boolean cee_source_symbol_cache_location_hit(CEESourceSymbolCache* cache,
                                                 cee_long location)
{
    if (!cache)
        return FALSE;
    
    CEEList* p = cache->referened_locations;
    while (p) {
        if (CEE_POINTER_TO_LONG(p->data) == location)
            return TRUE;
        p = p->next;
    }
    
    return FALSE;
}
