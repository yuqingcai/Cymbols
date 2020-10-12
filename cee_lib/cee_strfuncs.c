#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>

#include "cee_mem.h"
#include "cee_error.h"
#include "cee_list.h"
#include "cee_strfuncs.h"
#include "cee_range.h"

cee_char* cee_strdup(const cee_char *string)
{
    cee_char* new_string;
    cee_size length;
    
    if (!string)
        return NULL;
    
    length = strlen(string) + 1;
    new_string = cee_malloc0(sizeof(cee_char)*length);
    memcpy(new_string, string, length);
    return new_string;
}

cee_pointer cee_memdup(const cee_pointer mem,
                       cee_uint nb_byte)
{
    cee_pointer new_mem;
    
    if (mem && nb_byte != 0) {
        new_mem = cee_malloc(nb_byte);
        memcpy(new_mem, mem, nb_byte);
    }
    else
        new_mem = NULL;
    
    return new_mem;
}

cee_char* cee_strndup(const cee_char *string,
                      cee_size n)
{
    if (!string)
        return NULL;
    
    cee_char *new_string = cee_malloc(sizeof(cee_char)*(n+1));
    if (!new_string)
        return NULL;
    
    strncpy(new_string, string, n);
    new_string[n] = '\0';
    
    return new_string;
}

cee_char* cee_strnfill(cee_size n,
                       cee_char fill_char)
{
    cee_char *string;
    
    string = cee_malloc(sizeof(cee_char)*(n+1));
    memset(string, (cee_uchar)fill_char, n);
    string[n] = '\0';
    
    return string;
}

cee_char* cee_stpcpy(cee_char *dest,
                     const cee_char *src)
{
    cee_char *d = dest;
    const cee_char *s = src;
    
    if (!dest)
        return NULL;
    if (!src)
        return NULL;
    
    do
        *d++ = *s;
    while (*s++ != '\0');
    
    return d - 1;
}

cee_char* cee_strconcat(const cee_char *string1, ...)
{
    cee_size l;
    va_list args;
    cee_char* s;
    cee_char* concat;
    cee_char* ptr;
    
    if (!string1)
        return NULL;
    
    l = 1 + strlen (string1);
    va_start(args, string1);
    s = va_arg(args, cee_char*);
    while (s) {
        l += strlen(s);
        s = va_arg(args, cee_char*);
    }
    va_end (args);
    
    concat = cee_malloc(sizeof(cee_char)*l);
    ptr = concat;
    
    ptr = cee_stpcpy(ptr, string1);
    va_start(args, string1);
    s = va_arg(args, cee_char*);
    while (s) {
        ptr = cee_stpcpy(ptr, s);
        s = va_arg(args, cee_char*);
    }
    va_end (args);
    
    return concat;
}

void cee_strconcat0(cee_char** string0, ...)
{
    if (!string0)
        return;
    
    cee_char* append;
    cee_char* ptr;
    cee_char* s;
    va_list args;
    cee_size l;
    
    l = 0;
    va_start(args, string0);
    s = va_arg(args, cee_char*);
    while (s) {
        l += strlen(s);
        s = va_arg(args, cee_char*);
    }
    va_end (args);
    l += 1; /* '\0' terminator */
    
    
    append = cee_malloc(sizeof(cee_char)*l);
    ptr = append;
    
    va_start(args, string0);
    s = va_arg(args, cee_char*);
    while (s) {
        ptr = cee_stpcpy(ptr, s);
        s = va_arg(args, cee_char*);
    }
    va_end (args);
    
    
    if (!*string0)
        *string0 = append;
    else {
        cee_char* string = cee_strconcat(*string0, append, NULL);
        cee_free(append);
        cee_free(*string0);
        *string0 = string;
    }
}

cee_char* cee_strjoin(const cee_char* separator, ...)
{
    cee_char *string, *s;
    va_list args;
    cee_size len;
    cee_size separator_len;
    cee_char* ptr;
    
    if (separator == NULL)
        separator = "";
    
    separator_len = strlen(separator);
    
    va_start(args, separator);
    
    s = va_arg(args, cee_char*);
    
    if (s) {
        len = 1 + strlen(s);
        
        s = va_arg(args, cee_char*);
        while (s) {
            len += separator_len + strlen(s);
            s = va_arg(args, cee_char*);
        }
        va_end(args);
        
        string = cee_malloc0(sizeof(cee_char)*len);
        
        va_start(args, separator);
        
        s = va_arg(args, cee_char*);
        ptr = cee_stpcpy(string, s);
        
        s = va_arg(args, cee_char*);
        while (s) {
            ptr = cee_stpcpy(ptr, separator);
            ptr = cee_stpcpy (ptr, s);
            s = va_arg (args, cee_char*);
        }
    }
    else
        string = cee_strdup ("");
    
    va_end (args);
    
    return string;
}

cee_boolean cee_str_has_suffix(const cee_char *string,
                               const cee_char *suffix)
{
    cee_size string_length;
    cee_size suffix_length;
    
    if (!string)
        return FALSE;
    
    if (!suffix)
        return FALSE;
    
    string_length = strlen(string);
    suffix_length = strlen(suffix);
    
    if (string_length < suffix_length)
        return FALSE;
    
    return strcmp(string + string_length - suffix_length, suffix) == 0;
}

cee_boolean cee_str_has_prefix(const cee_char *string,
                               const cee_char *prefix)
{
    if (!string)
        return FALSE;
    if (!prefix)
        return FALSE;
    
    return strncmp(string, prefix, strlen(prefix)) == 0;
}

cee_boolean cee_ascii_isspace(cee_char ch)
{
    return (ch == ' ' ||
            ch == '\t' ||
            ch == '\n' ||
            ch == '\f' ||
            ch == '\v' ||
            ch == '\r');
}

cee_char* cee_strchug(cee_char *string)
{
    cee_uchar* start;
    
    if (!string)
        return NULL;
    
    for (start = (cee_uchar*)string; *start && cee_ascii_isspace(*start); start++);
    
    memmove(string, start, strlen((cee_char*)start)+1);
    
    return string;
}

cee_char* cee_strchomp(cee_char *string)
{
    cee_size len;
    
    if (!string)
        return NULL;
    
    len = strlen(string);
    while (len--) {
        if (cee_ascii_isspace((cee_uchar)string[len]))
            string[len] = '\0';
        else
            break;
    }
    
    return string;
}

static cee_boolean character_in_set(cee_char ch,
                        const cee_char* characters)
{
    const cee_char* p = characters;
    while (*p) {
        if (*p == ch)
            return TRUE;
        p ++;
    }
    return FALSE;
}

cee_char* cee_strtrim(cee_char* string,
                      const cee_char* characters)
{
    cee_uchar* start;
    cee_size len;
    
    if (!string)
        return NULL;
    
    if (!characters)
        return string;
    
    for (start = (cee_uchar*)string; *start && character_in_set(*start, characters); start++);
    
    memmove(string, start, strlen((cee_char*)start)+1);
    
    len = strlen(string);
    while (len--) {
        if (character_in_set((cee_uchar)string[len], characters))
            string[len] = '\0';
        else
            break;
    }
    
    return string;
}


cee_char** cee_strsplit(const cee_char *string,
                        const cee_char *delimiter,
                        cee_int max_tokens)
{
    CEEList *string_list = NULL, *list;
    cee_char **str_array, *s;
    cee_uint n = 0;
    const cee_char *remainder;
    
    
    if (!string)
        return NULL;
    
    if (!delimiter)
        return NULL;
    
    if (delimiter[0] == '\0')
        return NULL;
    
    if (max_tokens < 1)
        max_tokens = INT_MAX;
    
    remainder = string;
    s = strstr(remainder, delimiter);
    if (s) {
        cee_size delimiter_len = strlen(delimiter);
        
        while (--max_tokens && s) {
            cee_size len;
            
            len = s - remainder;
            string_list =
                cee_list_prepend(string_list, cee_strndup(remainder, len));
            n++;
            remainder = s + delimiter_len;
            s = strstr(remainder, delimiter);
        }
    }
    
    if (*string) {
        n++;
        string_list = cee_list_prepend(string_list, cee_strdup(remainder));
    }
    
    str_array = cee_malloc0(sizeof(cee_char*)*(n+1));
    
    str_array[n--] = NULL;
    for (list = string_list; list; list = list->next)
        str_array[n--] = list->data;
    
    cee_list_free(string_list);
    
    return str_array;
}

void cee_strfreev(cee_char **str_array)
{
    if (str_array) {
        int i;
        for (i = 0; str_array[i] != NULL; i++)
            cee_free(str_array[i]);
        
        cee_free(str_array);
    }
}

cee_boolean cee_strcontain(const cee_char* string,
                           const cee_char* substring,
                           cee_boolean case_insensitive)
{
    /**
     * TODO : complete implementation
     */
    if (strstr(string, substring))
        return TRUE;
    
    return FALSE;
}

#define IS_SPACE(c)             ((c) == ' ' || (c) == '\f' || (c) == '\n' || \
                                 (c) == '\r' || (c) == '\t' || (c) == '\v')

#define IS_UPPER_LETTER(c)      ((c) >= 'A' && (c) <= 'Z')
#define IS_LOWER_LETTER(c)      ((c) >= 'a' && (c) <= 'z')
#define IS_LOW_DASH(c)          ((c) == '_')
#define IS_LETTER(c)            (IS_UPPER_LETTER (c) || IS_LOWER_LETTER (c) || IS_LOW_DASH(c))
#define TO_UPPER_LETTER(c)      (IS_LOWER_LETTER(c) ? (c) - 'a' + 'A' : (c))
#define TO_LOWER_LETTER(c)      (IS_UPPER_LETTER(c) ? (c) - 'A' + 'a' : (c))

static cee_int strcmp_insensitive(const cee_char* string1,
                                  const cee_char* string2)
{
    cee_int c1 = 0, c2 = 0;
    
    while (*string1 && *string2) {
        c1 = (cee_int)(cee_char)TO_UPPER_LETTER(*string1);
        c2 = (cee_int)(cee_char)TO_UPPER_LETTER(*string2);
        if (c1 != c2)
            return (c1 - c2);
        string1 ++;
        string2 ++;
    }
    
    c1 = (cee_int)(cee_char)TO_UPPER_LETTER(*string1);
    c2 = (cee_int)(cee_char)TO_UPPER_LETTER(*string2);
    
    if (!*string1)
        return -c2;
    
    if (!*string2)
        return c1;
    
    return 0;
}

static cee_int strcmp_sensitive(const cee_char *string1,
                                const cee_char *string2)
{
    cee_int c1 = 0, c2 = 0;
    
    while (*string1 && *string2) {
        c1 = (cee_int)(cee_char)*string1;
        c2 = (cee_int)(cee_char)*string2;
        if (c1 != c2)
            return (c1 - c2);
        string1 ++;
        string2 ++;
    }
    
    c1 = (cee_int)(cee_char)*string1;
    c2 = (cee_int)(cee_char)*string2;
    
    if (!*string1)
        return -c2;
    
    if (!*string2)
        return c1;
    
    return 0;
}

cee_int cee_strcmp(const cee_char *string1,
                   const cee_char *string2,
                   cee_boolean sensitive)
{
    if (sensitive)
        return strcmp_sensitive(string1, string2);
    
    return strcmp_insensitive(string1, string2);
}

cee_char* cee_str_replace(cee_char* string,
                          cee_long location,
                          cee_ulong length,
                          const cee_char* replacement)
{
    if (!string)
        return NULL;
    
    if (!replacement)
        return string;
    
    cee_ulong origin_length = strlen((const cee_char*)string);
    cee_ulong replacement_length = strlen((const cee_char*)replacement);
    cee_ulong new_length = origin_length - length + replacement_length;
    
    if (!replacement_length && !length)
        return string;
    
    if (!replacement_length && length) { /** delete */
        cee_long replace_head = location;
        cee_long replace_tail = location + length - 1;
        cee_long move_size = origin_length - (replace_tail+1);
        
        if (replace_head != 0 && replace_tail == origin_length - 1)
            string[replace_head] = '\0';
        else if (replace_head == 0 && replace_tail == origin_length - 1)
            string[0] = '\0';
        else {
            memmove(&string[replace_head], &string[replace_tail+1], move_size);
            
            string[origin_length - length] = '\0';
        }
    }
    else { /** replace */
        if (new_length > origin_length)
            string = (cee_char*)cee_realloc((cee_pointer)string, new_length + 1);
        
        cee_long replace_tail = location + length - 1;
        cee_long move_size = origin_length - (replace_tail + 1);
        cee_long replacement_head = location;
        cee_long replacement_tail = location + replacement_length - 1;
        
        memmove(&string[replacement_tail+1], &string[replace_tail+1], move_size);
        memcpy(&string[replacement_head], replacement, replacement_length);
        
        string[new_length] = '\0';
    }
    
    return string;
}

cee_char* cee_str_replace_occurrence(cee_char* string,
                                     const cee_char* occurrence,
                                     const cee_char* replacement)
{
    CEEList* locations = NULL;
    CEEList* p = NULL;
    cee_long i = 0;
    cee_long j = 0;
    cee_long k = -1;
    cee_ulong location = 0;
    cee_size length_copy = 0;
    cee_size length_occurrence = 0;
    cee_size length_replacement = 0;
    cee_size new_length = 0;
    cee_char* new_string = NULL;
    
    if (!string)
        return NULL;
    
    while (string[i]) {
        
        if (string[i] == occurrence[j]) {
            k = i;
            while (occurrence[j] && string[i]) {
                if (occurrence[j] == string[i]) {
                    i ++;
                    j ++;
                }
                else {
                    i = k + 1;
                    break;
                }
            }
            
            if (!occurrence[j])
                locations = cee_list_append(locations, CEE_LONG_TO_POINTER(k));
            
            k = -1;
            j = 0;
        }
        else {
            i ++;
        }
    }
    
    if (!cee_list_length(locations))
        return cee_strdup(string);
    
    location = 0;
    length_copy = 0;
    length_occurrence = strlen(occurrence);
    length_replacement = strlen(replacement);
    new_length = strlen(string) + ((length_replacement - length_occurrence) * cee_list_length(locations));
    new_string = (cee_char*)cee_malloc(new_length + 1);
    new_string[new_length] = '\0';
    
    i = 0;
    j = 0;
    p = locations;
    while (p) {
        location = CEE_POINTER_TO_LONG(p->data);
        length_copy = location - i;
        
        memcpy(&new_string[j], &string[i], length_copy);
        memcpy(&new_string[j+length_copy], replacement, length_replacement);
        
        i = location + length_occurrence;
        j += length_copy + length_replacement;
        p = p->next;
    }
    if (i < strlen(string))
        memcpy(&new_string[j], &string[i], strlen(string) - i);
    
    return new_string;
}

static cee_boolean character_compare(cee_char a,
                                     cee_char b,
                                     cee_boolean sensitive)
{
    cee_boolean ret = FALSE;
    if (!sensitive) {
        if (IS_LETTER(a) && IS_LETTER(b)) {
            a = TO_UPPER_LETTER(a);
            b = TO_UPPER_LETTER(b);
            if (a == b)
                ret = TRUE;
            else
                ret = FALSE;
        }
        else {
            if (a == b)
                ret = TRUE;
            else 
                ret = FALSE;
        }
    }
    else {
        if (a == b)
            ret = TRUE;
        else 
            ret = FALSE;
    }
    
    return ret;
}

static cee_boolean subject_match_word(const cee_char* string,
                                      cee_long subject_location,
                                      cee_ulong subject_length)
{
    cee_ulong string_length = strlen(string);
    cee_boolean ret = FALSE;
    cee_long i = subject_location;
    cee_long j = i + subject_length - 1;
    
    if (i == 0 && j == string_length - 1) {
        ret = TRUE;
    }
    else if (i == 0 && j != string_length - 1) {
        if (IS_SPACE(string[j+1]))
            ret = TRUE;
    }
    else if (i != 0 && j == string_length - 1) {
        if (IS_SPACE(string[i-1]))
            ret = TRUE;
    }
    else {
        if ((IS_SPACE(string[i-1]) && IS_SPACE(string[j+1])) ||
            (!IS_LETTER(string[i-1]) && !IS_LETTER(string[j+1])))
            ret = TRUE;
    }
    return ret;
}


static cee_boolean subject_is_prefixed(const cee_char* string,
                                       cee_long subject_location,
                                       cee_ulong subject_length)
{
    cee_boolean ret = FALSE;
    cee_long i = subject_location;
    
    if (i == 0) {
        ret = TRUE;
    }
    else {
        if (IS_SPACE(string[i-1]))
            ret = TRUE;
    }
    return ret;
}

static cee_boolean subject_is_sufixed(const cee_char* string,
                                      cee_long subject_location,
                                      cee_ulong subject_length)
{
    cee_ulong string_length = strlen(string);
    cee_boolean ret = FALSE;
    cee_long i = subject_location;
    cee_long j = i + subject_length - 1;
    
    if (j == string_length - 1) {
        ret = TRUE;
    }
    else {
        if (IS_SPACE(string[j+1]))
            ret = TRUE;
    }
    
    return ret;
}

static cee_boolean match_validated_in_mode(CEEStringSearchMode mode,
                                           const cee_char* string,
                                           cee_long location,
                                           cee_ulong length)
{
    cee_boolean ret = FALSE;
    
    switch (mode) {
        case kCEEStringSearchModeMatchWord:
            if (subject_match_word(string, location, length))
                ret = TRUE;
            break;
        case kCEEStringSearchModeContains:
            ret = TRUE;
            break;
        case kCEEStringSearchModeStartWith:
            if (subject_is_prefixed(string, location, length))
                ret = TRUE;
            break;
        case kCEEStringSearchModeEndWith:
            if (subject_is_sufixed(string, location, length))
                ret = TRUE;
            break;
        default:
            ret = TRUE;
            break;
    }
    return ret;
}

CEEList* cee_str_search(const cee_char* string,
                        const cee_char* target,
                        cee_boolean sensitive,
                        CEEStringSearchMode mode)
{
    if (!string || !target)
        return NULL;
    
    CEEList* ranges = NULL;
    CEERange* range = NULL;
    cee_ulong string_length = strlen(string);
    cee_ulong target_length = strlen(target);
    cee_long i = 0;
    cee_boolean found = FALSE;
            
    if (!string || !target || 
        !string_length || 
        !target_length ||
        string_length < target_length)
        return NULL;
    
    while (TRUE) {
        found = FALSE;
        if (character_compare(string[i], target[0], sensitive)) {
            found = TRUE;
            
            for (cee_long k = 0; k < target_length; k ++) {
                if (!character_compare(string[i+k], target[k], sensitive)) {
                    found = FALSE;
                    break;
                }
            }
        }
        
        if (string_length - i < target_length)
            break;
        
        if (found) {
            if (match_validated_in_mode(mode, string, i, target_length)) {
                range = cee_range_create(i, target_length);
                ranges = cee_list_prepend(ranges, range);
            }
            
            i += target_length;
        }
        else {
            i ++;
        }
    }
    
    ranges = cee_list_reverse(ranges);
    
    return ranges;
}
