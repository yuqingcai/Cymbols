#ifndef __CEE_TEXT_AUTOCOMPLETE_H__
#define __CEE_TEXT_AUTOCOMPLETE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"

typedef struct _CEETextAutoComplete* CEETextAutoCompleteRef;

CEETextAutoCompleteRef cee_text_autocomplete_create(const cee_char* type);
void cee_text_autocomplete_free(CEETextAutoCompleteRef data);
cee_uchar* cee_text_autocomplete_run(const cee_uchar* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TEXT_AUTOCOMPLETE_H__ */
