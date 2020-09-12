#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include "cee_reference.h"

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
    reference->locations = cee_ranges_string_from_tokens(TOKEN_FIRST(tokens), 
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
        cee_free(reference->locations);
    
    cee_free(reference);
}


void cee_reference_print(CEESourceReference* reference)
{
    if (!reference) {
        fprintf(stdout, "reference dump error: reference is NULL\n");
        return ;
    }
    
    const cee_char* descriptor = "?";
    const cee_char* locations = "?";
    
    if (reference->descriptor)
        descriptor = reference->descriptor;
    
    if (reference->locations)
        locations = reference->locations;
    
    fprintf(stdout, "%s %s\n",
            descriptor,
            locations);
}

void cee_references_print(CEEList* references)
{
    CEEList* p = NULL;
    CEESourceReference* reference = NULL;
    p = references;
    while (p) {
        reference = p->data;
        cee_reference_print(reference);
        p = p->next;
    }
}

