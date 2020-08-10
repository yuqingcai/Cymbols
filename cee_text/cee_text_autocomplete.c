#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cee_text_autocomplete.h"

typedef struct _CEETextAutoComplete {
    
} CEETextAutoComplete;


CEETextAutoCompleteRef cee_text_autocomplete_create(const cee_char* type)
{
    CEETextAutoComplete* autocomplete = cee_malloc0(sizeof(CEETextAutoComplete));
    return autocomplete;
}

void cee_text_autocomplete_free(CEETextAutoCompleteRef data)
{
    if (!data)
        return;
    
    CEETextAutoComplete* autocomplete = (CEETextAutoComplete*)data;
    
    cee_free(autocomplete);
}

cee_uchar* cee_text_autocomplete_run(const cee_uchar* str)
{
    cee_uchar* complete = NULL;
    return complete;
}
