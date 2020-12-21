#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include "cee_text_modify.h"

typedef struct _CEETextModifyLogger {
    CEEList* modifies;
    CEEList* current;
    void (*modify_free)(cee_pointer);
} CEETextModifyLogger;

void cee_text_modify_prepend(CEETextModifyLoggerRef logger,
                             CEETextModifyRef modify)
{
    if (!logger->current->data) {
        logger->current->data = modify;
        logger->current = cee_list_prepend(logger->current, NULL);
        logger->modifies = logger->current;
    }
    else {
        assert(logger->current->prev);
        
        CEEList* prev = logger->current->prev;
        logger->current->prev = NULL;
        prev->next = NULL;
        cee_list_free_full(logger->modifies, logger->modify_free);
        
        if (logger->modify_free)
            logger->modify_free(logger->current->data);
        
        logger->current->data = modify;
        logger->current = cee_list_prepend(logger->current, NULL);
        logger->modifies = logger->current;
    }
}

CEETextModifyRef cee_text_modify_backward(CEETextModifyLoggerRef logger)
{
    if (!logger->current->next)
        return NULL;
    
    logger->current = logger->current->next;
    return logger->current->data;
}

CEETextModifyRef cee_text_modify_forward(CEETextModifyLoggerRef logger)
{
    if (!logger->current->data)
        return NULL;
    
    CEETextModifyRef modify = logger->current->data;
    
    if (logger->current->prev)
        logger->current = logger->current->prev;
    
    return modify;
}

void cee_text_modify_logger_clear(CEETextModifyLoggerRef logger)
{
    if (!logger)
        return;
    
    if (logger->modifies)
        cee_list_free_full(logger->modifies, logger->modify_free);
    
    logger->modifies = cee_list_create();
    logger->current = logger->modifies;
}

void cee_text_modify_logger_free(CEETextModifyLoggerRef logger)
{
    if (!logger)
        return;
    
    if (logger->modifies)
        cee_list_free_full(logger->modifies, logger->modify_free);
    
    cee_free(logger);
}

CEETextModifyLoggerRef cee_text_modify_logger_create(void (*modify_free)(cee_pointer))
{
    assert(modify_free);
    
    CEETextModifyLogger* logger = (CEETextModifyLogger*)cee_malloc0(sizeof(CEETextModifyLogger));
    logger->modify_free = modify_free;
    logger->modifies = cee_list_create();
    logger->current = logger->modifies;
    
    return logger;
}
