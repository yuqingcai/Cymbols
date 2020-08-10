#ifndef __CEE_ERROR_H__
#define __CEE_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_types.h"
    
void cee_error(const cee_char* format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_ERROR_H__ */
