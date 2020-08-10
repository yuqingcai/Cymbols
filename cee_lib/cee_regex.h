#ifndef __CEE_REGEX_H__
#define __CEE_REGEX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>
#include "cee_types.h"
#include "cee_list.h"
#include "cee_range.h"
CEEList* cee_regex_search(const cee_char* string,
                          const cee_char* pattern,
                          cee_boolean repeat,
                          cee_int options,
                          cee_long timeout_ms,
                          cee_boolean* timeout);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_RANGE_H__ */
