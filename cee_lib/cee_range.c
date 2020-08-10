#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include "cee_range.h"
#include "cee_mem.h"
#include "cee_regex.h"
#include "cee_strfuncs.h"

CEERange* cee_range_create(cee_long location,
                           cee_ulong length)
{
    CEERange* range = (CEERange*)cee_malloc0(sizeof(CEERange));
    if (!range)
        return NULL;
    
    range->location = location;
    range->length = length;
        
    return range;
}

CEERange cee_range_make(cee_long location,
                        cee_ulong length)
{
    CEERange range;
    range.location = location;
    range.length = length;
    return range;
}

void cee_range_free(void* range)
{
    if (!range)
        return;
    
    cee_free(range);
}

CEEList* cee_ranges_from_string(const cee_char* str)
{
    const cee_char* pattern = "range\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*\\)";
    CEEList* matches = NULL;
    CEERange* match = NULL;
    cee_int nb_match = 0;
    cee_char* str_location = NULL;
    cee_char* str_length = NULL;
    CEERange* range = NULL;
    CEEList* ranges = NULL;
    
    if (!str)
        goto exit;
    
    matches = cee_regex_search(str, pattern, TRUE, 0, 0, NULL);
    nb_match = cee_list_length(matches);
    if (!nb_match || nb_match % 3)
        goto exit;
    
    for (cee_int i = 0; i < nb_match / 3; i ++) {
        match = cee_list_nth_data(matches, (i*3) + 1);
        str_location = cee_strndup(&str[match->location], match->length);
        match = cee_list_nth_data(matches, (i*3) + 2);
        str_length = cee_strndup(&str[match->location], match->length);
        
        if (str_location && str_length) {
            range = cee_range_create(atol(str_location), atoll(str_length));
            ranges = cee_list_prepend(ranges, range);
        }
        
        if (str_location)
            cee_free(str_location);
        
        if (str_length)
            cee_free(str_length);
    }
    
    ranges = cee_list_reverse(ranges);
    
exit:
    cee_list_free_full(matches, cee_range_free);
    return ranges;
}

cee_char* cee_string_from_range(CEERange* range)
{
    cee_char* str_range = NULL;
    cee_char* str_location = NULL;
    cee_char* str_length = NULL;
    cee_int length = 0;
    
    if (!range)
        return NULL;
    
    length = snprintf(NULL, 0, "%ld", range->location);
    str_location = cee_malloc0(length + 1);
    if (str_location)
        snprintf(str_location, length + 1, "%ld", range->location);
    
    length = snprintf(NULL, 0, "%lu", range->length);
    str_length = cee_malloc0(length + 1);
    if (str_length)
        snprintf(str_length, length+1, "%ld", range->length);
    
    if (str_location && str_length)
        cee_strconcat0(&str_range,
                       "range",
                       "(",
                       str_location,
                       ",",
                       str_length,
                       ")",
                       NULL);
    
    if (str_location)
        cee_free(str_location);
    if (str_length)
        cee_free(str_length);
    
    return str_range;
}

cee_char* cee_string_from_ranges(CEEList* ranges)
{
    cee_char* str_ranges = NULL;
    cee_char* str_range = NULL;
    CEEList* p = NULL;
    CEERange* range = NULL;
    
    p = ranges;
    while (p) {
        range = p->data;
        str_range = cee_string_from_range(range);
        
        cee_strconcat0(&str_ranges, str_range, NULL);
        
        if (p->next)
            cee_strconcat0(&str_ranges, ",", NULL);
        
        if (str_range)
            cee_free(str_range);
        
        p = p->next;
    }
    
    return str_ranges;
}

cee_boolean cee_location_in_range(cee_long location,
                                  CEERange range)
{
    return (location >= range.location) && 
            (location < range.location + range.length);
}

cee_boolean cee_location_overflow_range(cee_long location,
                                        CEERange range)
{
    return location > range.location + range.length - 1;
}

cee_long cee_range_tail(CEERange range)
{
    return range.location + range.length - 1;
}

cee_boolean cee_ranges_intersect(CEERange range0,
                                 CEERange range1)
{
    return cee_location_in_range(range0.location, range1) || 
            cee_location_in_range(cee_range_tail(range0), range1) ||
            (range0.location < range1.location && cee_range_tail(range0) > cee_range_tail(range1));
}

CEERange cee_range_consistent_from_discrete(CEEList* ranges)
{
    CEERange* range0 = cee_list_first(ranges)->data;
    CEERange* rangeN = cee_list_last(ranges)->data;
    CEERange range = cee_range_make(-1, 0);
    if (range0 && rangeN)    
        range = cee_range_make(range0->location, 
                               rangeN->location + rangeN->length - range0->location);
    return range;
}
