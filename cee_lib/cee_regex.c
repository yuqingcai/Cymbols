#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cee_regex.h"
#include "cee_mem.h"
#include "cee_datetime.h"

#define OVECCOUNT 30

CEEList* cee_regex_search(const cee_char* string,
                          const cee_char* pattern,
                          cee_boolean repeat,
                          cee_int options,
                          cee_long timeout_ms,
                          cee_boolean* timeout)
{
    cee_ulong t0 = 0;
    cee_ulong t1 = 0;
    pcre *re = NULL;
    const char *err_msg;
    cee_int err;
    cee_int ovector[OVECCOUNT];
    cee_int ret;
    cee_ulong length = 0;
    CEEList* ranges = NULL;
    CEERange* range = NULL;
    cee_int _options = options;
    
    if (!string)
        return NULL;
    
    length = strlen((char*)string);
    
    if (!_options)
        _options = 
                PCRE_NEWLINE_ANY | 
                PCRE_UTF8 | 
                PCRE_DOTALL
                ;
        
    if (!pattern || !length || !strlen(pattern))
        return NULL;
    
    memset(ovector, 0, sizeof(int)*OVECCOUNT);
    
    re = pcre_compile((char*)pattern,
                      _options,
                      &err_msg,
                      &err,
                      NULL);
    
    t0 = cee_timestamp_ms();
    
    do {
        ret = pcre_exec(re,
                        NULL,
                        (char*)string,
                        (int)length,
                        ovector[1],
                        0,
                        ovector,
                        OVECCOUNT);
        if (ret < 0)
            break;
        
        for (int i = 0; i < ret; i ++) {
            range = cee_range_create(ovector[i*2], 
                                     ovector[i*2+1] - ovector[i*2]);
            ranges = cee_list_prepend(ranges, range);
        }
        
        if (!(ovector[1] - ovector[0]))
            ovector[1] ++;
        
        t1 = cee_timestamp_ms();
        
        if (timeout_ms && (t1 - t0 >= timeout_ms)) {
            if (timeout)
                *timeout = TRUE;
            break;
        }
        
    } while (repeat);
    
    if (re)
        pcre_free(re);
    
    ranges = cee_list_reverse(ranges);
    
    return ranges;
}
