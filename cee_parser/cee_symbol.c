#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include "cee_symbol.h"

CEESourceSymbol* cee_source_symbol_create(CEESourceSymbolType type,
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

void cee_source_symbol_free(cee_pointer data)
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

CEESourceSymbol* cee_source_symbol_copy(CEESourceSymbol* symbol)
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

CEESourceSymbol* cee_source_symbol_create_from_token_slice(const cee_uchar* filepath,
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

CEESourceSymbol* cee_source_symbol_create_from_tokens(const cee_uchar* filepath,
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

void cee_source_symbol_print(CEESourceSymbol* symbol)
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
            symbol->type == kCEESourceSymbolTypeEnumDefinition ||
            symbol->type == kCEESourceSymbolTypeUnionDefinition ||
            symbol->type == kCEESourceSymbolTypeNamespaceDefinition ||
            symbol->type == kCEESourceSymbolTypeInterfaceDeclaration ||
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

cee_boolean cee_source_symbol_matcher_by_descriptor(cee_pointer data,
                                                    cee_pointer user_data)
{
    if (!data || !user_data)
        return FALSE;
    
    CEESourceSymbol* symbol = (CEESourceSymbol*)data;
    cee_char* descriptor = (cee_char*)user_data;
    if (symbol->descriptor && !strcmp(symbol->descriptor, descriptor))
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
            symbol->type == kCEESourceSymbolTypeProperty || 
            symbol->type == kCEESourceSymbolTypeVariableDeclaration || 
            symbol->type == kCEESourceSymbolTypeMessageDefinition || 
            symbol->type == kCEESourceSymbolTypeTypeDefine || 
            symbol->type == kCEESourceSymbolTypeClassDefinition || 
            symbol->type == kCEESourceSymbolTypeEnumDefinition || 
            symbol->type == kCEESourceSymbolTypeUnionDefinition || 
            symbol->type == kCEESourceSymbolTypeNamespaceDefinition || 
            symbol->type == kCEESourceSymbolTypeImplementationDefinition || 
            symbol->type == kCEESourceSymbolTypeProtocolDeclaration);
}
