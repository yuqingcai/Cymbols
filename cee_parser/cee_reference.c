#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include "cee_reference.h"

CEESourceSymbolReference* cee_source_symbol_reference_create(cee_pointer parser_ref,
                                                             const cee_char* file_path,
                                                             const cee_char* subject,
                                                             CEEList* tokens,
                                                             CEESourceReferenceType type)
{
    CEESourceSymbolReference* reference = cee_malloc0(sizeof(CEESourceSymbolReference));
    reference->file_path = cee_strdup(file_path);
    reference->type = type;
    reference->tokens_ref = tokens;
    reference->name = cee_string_from_tokens(subject,
                                             TOKEN_FIRST(tokens),
                                             kCEETokenStringOptionCompact);
    reference->ranges = cee_ranges_from_tokens(TOKEN_FIRST(tokens), kCEERangeListTypeSeparate);
    reference->parser_ref = parser_ref;
    
    if (tokens) {
        CEEToken* first_token = TOKEN_FIRST(tokens)->data;
        reference->line_no = first_token->line_no;
    }
    return reference;
}

void cee_source_symbol_reference_free(cee_pointer data)
{
    if (!data)
        return;
    CEESourceSymbolReference* reference = (CEESourceSymbolReference*)data;
    
    if (reference->file_path)
        cee_free(reference->file_path);
    
    if (reference->tokens_ref)
        cee_list_free(reference->tokens_ref);
    
    if (reference->name)
        cee_free(reference->name);
    
    if (reference->ranges)
        cee_list_free_full(reference->ranges, cee_range_free);
    
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
    cee_char* ranges = NULL;
    
    if (reference->file_path)
        filepath = reference->file_path;
    
    if (reference->name)
        name = reference->name;
    
    if (reference->ranges)
        ranges = cee_string_from_ranges(reference->ranges);
    else
        ranges = cee_strdup("?");
    
    fprintf(stdout, "%s %s %s\n",
            name,
            ranges,
            filepath);
    
    if (ranges)
        cee_free(ranges);
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

cee_int cee_source_symbol_reference_location_compare(const cee_pointer a,
                                                     const cee_pointer b)

{
    cee_int ret = 0;
    CEESourceSymbolReference* reference0 = (CEESourceSymbolReference*)a;
    CEESourceSymbolReference* reference1 = (CEESourceSymbolReference*)b;
    CEERange* range0 = NULL;
    CEERange* range1 = NULL;
    
    if (!reference0->ranges || !reference1->ranges)
        goto exit;
    
    range0 = cee_list_first(reference0->ranges)->data;
    range1 = cee_list_first(reference1->ranges)->data;
    
    if (!range0 || !range1)
        goto exit;
    
    ret = cee_range_compare(range0, range1);
    
exit:
    return ret;
}

