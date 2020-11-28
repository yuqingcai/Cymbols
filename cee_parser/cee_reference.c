#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include "cee_reference.h"

CEESourceSymbolReference* cee_source_symbol_reference_create(const cee_char* filepath,
                                                             const cee_char* subject,
                                                             CEEList* tokens,
                                                             CEESourceReferenceType type)
{
    CEESourceSymbolReference* reference = cee_malloc0(sizeof(CEESourceSymbolReference));
    reference->filepath = cee_strdup(filepath);
    reference->type = type;
    reference->tokens_ref = tokens;
    reference->name = cee_string_from_tokens(subject,
                                             TOKEN_FIRST(tokens),
                                             kCEETokenStringOptionCompact);
    reference->locations = cee_ranges_string_from_tokens(TOKEN_FIRST(tokens), 
                                                         kCEERangeListTypeSeparate);
    return reference;
}

void cee_source_symbol_reference_free(cee_pointer data)
{
    if (!data)
        return;
    CEESourceSymbolReference* reference = (CEESourceSymbolReference*)data;
    
    if (reference->filepath)
        cee_free(reference->filepath);
    
    if (reference->tokens_ref)
        cee_list_free(reference->tokens_ref);
    
    if (reference->name)
        cee_free(reference->name);
    
    if (reference->locations)
        cee_free(reference->locations);
    
    cee_free(reference);
}


void cee_source_symbol_reference_print(CEESourceSymbolReference* reference)
{
    if (!reference) {
        fprintf(stdout, "reference dump error: reference is NULL\n");
        return ;
    }
    
    const cee_char* filepath = "?";
    const cee_char* name = "?";
    const cee_char* locations = "?";
    
    if (reference->filepath)
        filepath = reference->filepath;
    
    if (reference->name)
        name = reference->name;
    
    if (reference->locations)
        locations = reference->locations;
    
    fprintf(stdout, "%s %s %s\n",
            name,
            locations,
            filepath);
}

void cee_source_symbol_references_print(CEEList* references)
{
    CEEList* p = NULL;
    CEESourceSymbolReference* reference = NULL;
    p = references;
    while (p) {
        reference = p->data;
        cee_source_symbol_reference_print(reference);
        p = p->next;
    }
}

