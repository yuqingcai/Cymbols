#ifndef __CEE_SOURCE_CONTEXT_H__
#define __CEE_SOURCE_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_source_parser.h"

typedef struct _CEESourceContext {
    cee_char* file_path;
    cee_char* name;
    CEERange range;
    CEEList* symbols;
} CEESourceContext;

CEESourceContext* cee_source_symbol_context_create(const cee_char* file_path,
                                                   CEESourceSymbol* symbol,
                                                   CEESourceFragment* prep_directive,
                                                   CEESourceFragment* statement,
                                                   cee_pointer database);
CEESourceContext* cee_source_reference_context_create(const cee_char* file_path,
                                                      CEESourceSymbolReference* reference,
                                                      CEESourceFragment* prep_directive,
                                                      CEESourceFragment* statement,
                                                      cee_pointer database);
void cee_source_context_free(CEESourceContext* context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SOURCE_CONTEXT_H__ */
