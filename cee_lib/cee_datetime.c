#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include "cee_list.h"
#include "cee_mem.h"
#include "cee_datetime.h"
#include "cee_strfuncs.h"
#include "cee_regex.h"

cee_long cee_timestamp_ms()
{
    struct timeval te;
    gettimeofday(&te, NULL);
    cee_long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
    return milliseconds;
}

#define MAX_TIME_STRING 64
cee_char* cee_time_to_iso8601(time_t t)
{
    cee_char* str = (cee_char*)cee_malloc0(sizeof(cee_char)*MAX_TIME_STRING);
    struct tm* timeinfo = localtime(&t);
    sprintf(str, "%d-%d-%dT%d:%d:%dZ", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
            timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    return str;
}

time_t cee_time_from_iso8601(const cee_char* str)
{
    if (!str)
        return 0;
    
    const cee_char* pattern = "(\\d+)-(\\d+)-(\\d+)T(\\d+)\\:(\\d+)\\:(\\d+)Z";
    CEEList* matches = cee_regex_search(pattern, str, FALSE, 0, 0, NULL);
    if (!matches)
        return 0;

    CEERange* r1 = cee_list_nth(matches, 1)->data;
    CEERange* r2 = cee_list_nth(matches, 2)->data;
    CEERange* r3 = cee_list_nth(matches, 3)->data;
    CEERange* r4 = cee_list_nth(matches, 4)->data;
    CEERange* r5 = cee_list_nth(matches, 5)->data;
    CEERange* r6 = cee_list_nth(matches, 6)->data;
    
    cee_char* year = cee_strndup(str+r1->location, r1->length);
    cee_char* mon = cee_strndup(str+r2->location, r2->length);
    cee_char* mday = cee_strndup(str+r3->location, r3->length);
    cee_char* hour = cee_strndup(str+r4->location, r4->length);
    cee_char* min = cee_strndup(str+r5->location, r5->length);
    cee_char* sec = cee_strndup(str+r6->location, r6->length);
    
    struct tm timeinfo;
    memset(&timeinfo, 0, sizeof(timeinfo));
    timeinfo.tm_year = atoi(year) - 1900;
    timeinfo.tm_mon = atoi(mon) - 1;
    timeinfo.tm_mday = atoi(mday);
    timeinfo.tm_hour = atoi(hour);
    timeinfo.tm_min = atoi(min);
    timeinfo.tm_sec = atoi(sec);
    
    time_t t = mktime(&timeinfo);
    
    cee_free(year);
    cee_free(mon);
    cee_free(mday);
    cee_free(hour);
    cee_free(min);
    cee_free(sec);
    
    cee_list_free_full(matches, cee_range_free);
    
    return t;
}

time_t cee_time_current() {
    time_t t;
    time(&t);
    return t;
}

cee_int cee_time_compare(time_t t0, time_t t1)
{
    if (t0 == t1)
        return 0;
    
    if (t0 < t1)
        return -1;
    
    if (t0 > t1)
        return 1;
    
    return 0;
}
