#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_source_context.h"

CEESourceContext* cee_source_symbol_context_create(const cee_char* file_path,
                                                   CEESourceSymbol* symbol,
                                                   CEESourceFregment* prep_directive,
                                                   CEESourceFregment* statement,
                                                   cee_pointer database)
{
    CEESourceContext* context = cee_malloc0(sizeof(CEESourceContext));
    context->file_path = cee_strdup(file_path);
    context->name = cee_strdup(symbol->name);
    context->range = cee_range_consistent_from_discrete(symbol->ranges);
    context->symbols = cee_source_symbols_search_by_symbol(symbol,
                                                           prep_directive,
                                                           statement,
                                                           database);
    return context;
}

CEESourceContext* cee_source_reference_context_create(const cee_char* file_path,
                                                      CEESourceSymbolReference* reference,
                                                      CEESourceFregment* prep_directive,
                                                      CEESourceFregment* statement,
                                                      cee_pointer database)
{
    CEESourceContext* context = cee_malloc0(sizeof(CEESourceContext));
    context->file_path = cee_strdup(file_path);
    context->name = cee_strdup(reference->name);
    context->range = cee_range_consistent_from_discrete(reference->ranges);
    context->symbols = cee_source_symbols_search_by_reference(reference,
                                                              prep_directive,
                                                              statement,
                                                              database);
    return context;
}

void cee_source_context_free(CEESourceContext* context)
{
    if (context->file_path)
        cee_free(context->file_path);
    
    if (context->name)
        cee_free(context->name);
    
    if (context->symbols)
        cee_list_free_full(context->symbols, cee_source_symbol_free);
    
    cee_free(context);
}

