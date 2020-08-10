#ifndef __CEE_TEXT_MODIFY_H__
#define __CEE_TEXT_MODIFY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"

typedef struct _CEETextModify* CEETextModifyRef;

typedef struct _CEETextModifyLogger* CEETextModifyLoggerRef;

void cee_text_modify_prepend(CEETextModifyLoggerRef logger,
                             CEETextModifyRef modify);
CEETextModifyRef cee_text_modify_backward(CEETextModifyLoggerRef logger);
CEETextModifyRef cee_text_modify_forward(CEETextModifyLoggerRef logger);
void cee_text_modify_logger_free(CEETextModifyLoggerRef logger);
void cee_text_modify_logger_clear(CEETextModifyLoggerRef logger);
CEETextModifyLoggerRef cee_text_modify_logger_create(void (*modify_free)(cee_pointer));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TEXT_MODIFY_H__ */
