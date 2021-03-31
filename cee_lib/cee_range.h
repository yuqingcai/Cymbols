#ifndef __CEE_RANGE_H__
#define __CEE_RANGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cee_types.h"
#include "cee_list.h"

typedef struct _CEERange {
    cee_long location;
    cee_ulong length;
} CEERange;

CEERange* cee_range_create(cee_long location,
                           cee_ulong length);
CEERange cee_range_make(cee_long location,
                        cee_ulong length);
void cee_range_free(void* range);
CEEList* cee_ranges_from_string(const cee_char* str);
CEERange cee_range_from_string(const cee_char* str);
cee_char* cee_string_from_range(CEERange* range);
cee_char* cee_string_from_ranges(CEEList* ranges);
cee_boolean cee_location_in_range(cee_long location,
                                  CEERange range);
cee_boolean cee_location_followed_range(cee_long location,
                                        CEERange range);
cee_boolean cee_location_in_ranges(cee_long location,
                                   CEEList* ranges);
cee_boolean cee_location_followed_ranges(cee_long location,
                                         CEEList* ranges);
cee_long cee_range_tail(CEERange range);
cee_boolean cee_location_overflow_range(cee_long location,
                                        CEERange range);
cee_boolean cee_ranges_intersect(CEERange range0,
                                 CEERange range1);
CEERange cee_range_consistent_from_discrete(CEEList* ranges);
cee_int cee_range_compare(CEERange* range0,
                          CEERange* range1);
cee_int cee_range_string_compare(const cee_char* str0,
                                 const cee_char* str1);
cee_pointer cee_range_list_copy(const cee_pointer src,
                                cee_pointer data);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_RANGE_H__ */
