#ifndef __CEE_SYMBOL_CACHE_H__
#define __CEE_SYMBOL_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"

typedef struct _CEESymbolCache* CEESymbolCacheRef;

CEESymbolCacheRef cee_symbol_cache_create(void);
void cee_symbol_cache_free(cee_pointer data);
CEEList* cee_symbol_cache_search_by_descriptor(CEESymbolCacheRef cache, 
                                               const cee_char* descirptor);
void cee_symbol_cache_append(CEESymbolCacheRef cache, 
                             const cee_char* descriptor,
                             CEEList* symbols);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SYMBOL_CACHE_H__ */
