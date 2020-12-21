#ifndef __CEE_STRFUNCS_H__
#define __CEE_STRFUNCS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_types.h"
    
cee_char* cee_strdup(const cee_char *string);
cee_pointer cee_memdup(const cee_pointer mem,
                       cee_uint nb_byte);
cee_char* cee_strndup(const cee_char *string,
                      cee_size n);
cee_char* cee_strnfill(cee_size n,
                       cee_char fill_char);
cee_char * cee_stpcpy(cee_char *dest,
                      const cee_char *src);
cee_char* cee_strconcat(const cee_char *str, ...);
void cee_strconcat0(cee_char** str, ...);
cee_char* cee_strjoin(const cee_char* separator, ...);
cee_boolean cee_str_has_suffix(const cee_char *str,
                               const cee_char *suffix);
cee_boolean cee_str_has_prefix(const cee_char *string,
                               const cee_char *prefix);
cee_boolean cee_ascii_isspace(cee_char ch);
cee_char* cee_strchug(cee_char *string);
cee_char* cee_strchomp(cee_char *string);
cee_char* cee_strtrim(cee_char* string,
                      const cee_char* characters);
cee_char** cee_strsplit(const cee_char *string,
                        const cee_char *delimiter,
                        cee_int max_tokens);
void cee_strfreev(cee_char **str_array);
cee_boolean cee_strcontain(const cee_char* string,
                           const cee_char* substring,
                           cee_boolean case_insensitive);
cee_int cee_strcmp(const cee_char *string1,
                   const cee_char *string2,
                   cee_boolean sensitive);
cee_char* cee_str_replace(cee_char* string,
                          cee_long location,
                          cee_ulong length,
                          const cee_char* replacement);
cee_char* cee_str_replace_occurrence(cee_char* string,
                                     const cee_char* occurrence,
                                     const cee_char* replacement);
typedef enum _CEEStringSearchMatch {
    kCEEStringSearchModeContains,
    kCEEStringSearchModeMatchWord,
    kCEEStringSearchModeStartWith,
    kCEEStringSearchModeEndWith
} CEEStringSearchMode;

CEEList* cee_str_search(const cee_char* string,
                        const cee_char* subject,
                        cee_boolean sensitive,
                        CEEStringSearchMode mode);

#define cee_strstrip(str) cee_strchomp(cee_strchug(str))
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_STRFUNCS_H__ */
