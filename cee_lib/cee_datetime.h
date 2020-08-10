#ifndef __CEE_DATETIME_H__
#define __CEE_DATETIME_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_types.h"
#include <sys/time.h>

cee_long cee_timestamp_ms(void);
cee_char* cee_time_to_iso8601(time_t t);
time_t cee_time_from_iso8601(const cee_char* str);
time_t cee_time_current(void);
cee_int cee_time_compare(time_t t0, time_t t1);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_DATETIME_H__ */
