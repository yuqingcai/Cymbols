#ifndef __CEE_MEM_H__
#define __CEE_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_types.h"
    
cee_pointer cee_malloc(cee_size nb_byte);
cee_pointer cee_malloc0(cee_size nb_byte);
cee_pointer cee_realloc(cee_pointer ptr,
                        cee_size nb_byte);
void cee_free(cee_pointer data);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_MEM_H__ */
