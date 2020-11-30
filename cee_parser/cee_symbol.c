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
                                          const cee_char* proto,
                                          const cee_char* language,
                                          const cee_char* filepath,
                                          const cee_char* locations,
                                          const cee_char* fregment_range)
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
    
    if (proto)
        symbol->proto = cee_strdup(proto);
    
    if (language)
        symbol->language = cee_strdup(language);
    
    if (filepath)
        symbol->filepath = cee_strdup(filepath);
    
    if (locations)
        symbol->locations = cee_strdup(locations);
    
    if (fregment_range)
        symbol->fregment_range = cee_strdup(fregment_range);
    
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
        
    if (symbol->filepath)
        cee_free(symbol->filepath);
    
    if (symbol->parent)
        cee_free(symbol->parent);
    
    if (symbol->derives)
        cee_free(symbol->derives);
    
    if (symbol->proto)
        cee_free(symbol->proto);
    
    if (symbol->locations)
        cee_free(symbol->locations);
    
    if (symbol->fregment_range)
        cee_free(symbol->fregment_range);
    
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
    CEESourceSymbol* copy = cee_source_symbol_create(symbol->type,
                                                     symbol->name,
                                                     symbol->alias,
                                                     symbol->parent,
                                                     symbol->derives,
                                                     symbol->proto,
                                                     symbol->language,
                                                     symbol->filepath,
                                                     symbol->locations,
                                                     symbol->fregment_range);
    return copy;
}

CEESourceSymbol* cee_source_symbol_create_from_token_slice(const cee_char* filepath,
                                                           const cee_char* subject,
                                                           CEEList* begin,
                                                           CEEList* end,
                                                           CEESourceSymbolType type,
                                                           const cee_char* language)
{
    CEESourceSymbol* symbol = cee_source_symbol_create(kCEESourceSymbolTypeUnknow,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL);
    symbol->filepath = cee_strdup((cee_char*)filepath);
    symbol->type = type;
    symbol->language = cee_strdup(language);
    symbol->name = cee_string_from_token_slice(subject,
                                               begin,
                                               end,
                                               kCEETokenStringOptionCompact);
    symbol->locations = cee_ranges_string_from_token_slice(begin,
                                                           end,
                                                           kCEERangeListTypeContinue);
    symbol->fregment_range = cee_strdup(symbol->locations);
    return symbol;
}

CEESourceSymbol* cee_source_symbol_create_from_tokens(const cee_char* filepath,
                                                      const cee_char* subject,
                                                      CEEList* tokens,
                                                      CEESourceSymbolType type,
                                                      const cee_char* language)
{
    CEESourceSymbol* symbol = cee_source_symbol_create(kCEESourceSymbolTypeUnknow,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL);
    symbol->filepath = cee_strdup((cee_char*)filepath);
    symbol->type = type;
    symbol->language = cee_strdup(language);
    symbol->name = cee_string_from_tokens(subject,
                                          TOKEN_FIRST(tokens),
                                          kCEETokenStringOptionCompact);
    symbol->locations = cee_ranges_string_from_tokens(TOKEN_FIRST(tokens),
                                                      kCEERangeListTypeSeparate);
    symbol->fregment_range = cee_ranges_string_from_tokens(TOKEN_FIRST(tokens), 
                                                           kCEERangeListTypeContinue);
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
    const cee_char* locations = "?";
    const cee_char* fregment_range = "?";
    CEESourceSymbolType type = symbol->type;
    
    if (symbol->name)
        name = symbol->name;
    
    if (symbol->proto)
        protos = symbol->proto;
    
    if (symbol->derives)
        derives = symbol->derives;
    
    if (symbol->locations)
        locations = symbol->locations;
    
    if (symbol->filepath)
        filepath = symbol->filepath;
    
    if (symbol->fregment_range)
        fregment_range = symbol->fregment_range;
    
    fprintf(stdout, "%s %d %s %s %s %s %s\n",
            name,
            type,
            protos,
            locations,
            derives, 
            filepath,
            fregment_range);
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
    
    ret = cee_range_compare(range0, range1);
    
exit:
    
    if (ranges0)
        cee_list_free_full(ranges0, cee_range_free);
    
    if (ranges1)
        cee_list_free_full(ranges1, cee_range_free);
    
    return ret;
}

CEESourceSymbol* cee_source_symbol_tree_search(CEETree* tree,
                                               CEESymbolMatcher matcher,
                                               cee_pointer user_data)
{
    if (!tree)
        return NULL;
    
    CEEList* p = NULL; 
    CEESourceSymbol* symbol = tree->data;
    if (symbol) {
        if (matcher(symbol, user_data))
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

cee_boolean cee_source_symbol_matcher_by_buffer_offset(cee_pointer data,
                                                       cee_pointer user_data)
{
    if (!data || !user_data)
        return FALSE;
    
    CEESourceSymbol* symbol = (CEESourceSymbol*)data;
    cee_long offset = CEE_POINTER_TO_INT(user_data);
    CEEList* ranges = cee_ranges_from_string(symbol->locations);
    CEERange range;
    if (ranges) {
        range = cee_range_consistent_from_discrete(ranges);
        cee_list_free_full(ranges, cee_range_free);
        if (cee_location_in_range(offset, range))
            return TRUE;
    }
    return FALSE;
}

cee_boolean cee_source_symbol_matcher_by_name(cee_pointer data,
                                              cee_pointer user_data)
{
    if (!data || !user_data)
        return FALSE;
    
    CEESourceSymbol* symbol = (CEESourceSymbol*)data;
    cee_char* name = (cee_char*)user_data;
    if (symbol->name && !strcmp(symbol->name, name))
        return TRUE;
    
    return FALSE;
}

cee_boolean cee_source_symbols_are_equal(CEESourceSymbol* symbol0,
                                         CEESourceSymbol* symbol1)
{
    if (!strcmp(symbol0->filepath, symbol1->filepath) &&
        !strcmp(symbol0->locations, symbol1->locations)) 
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
