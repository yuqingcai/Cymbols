#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "cee_lib.h"
#include "cee_token.h"

cee_int cee_token_count = 0;

void cee_token_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEEToken* token = (CEEToken*)data;
    cee_free(token);
    
    cee_token_count --;
}

CEEToken* cee_token_create(CEETokenID identifier,
                           cee_long offset,
                           cee_ulong length,
                           cee_ulong line_no)
{
    CEEToken* token = cee_malloc0(sizeof (CEEToken));
    token->identifier = identifier;
    token->offset = offset;
    token->length = length;
    token->line_no = line_no;
    
    cee_token_count ++;
    
    return token;
}

cee_int cee_token_count_get(void)
{
    return cee_token_count;
}

static cee_int token_location_compare(const cee_pointer a,
                                      const cee_pointer b)
{
    CEEToken* token0 = (CEEToken*)a;
    CEEToken* token1 = (CEEToken*)b;
    
    if (token0->offset < token1->offset)
        return -1;
    else if (token0->offset > token1->offset)
        return 1;
    return 0;
}

cee_char* cee_string_from_token(const cee_char* subject,
                                CEEToken* token)
{
    if (!subject || !token)
        return NULL;
    
    cee_char* str = cee_strndup(&subject[token->offset],
                                token->length);
    return str;
}

void cee_string_from_token_print(const cee_char* subject,
                                 CEEToken* token)
{
    cee_char* str = cee_string_from_token(subject, token);
    if (!str)
        return;
    
    fprintf(stdout, "%s", str);
    cee_free(str);
}

cee_char* cee_string_from_tokens(const cee_char* subject,
                                 CEEList* p,
                                 CEETokenStringOption option)
{
    if (!subject || !p)
        return NULL;
    
    cee_char* dump = NULL;
    CEEToken* token = NULL;
    cee_char* str = NULL;
    
    while (p) {
        token = p->data;
        
        if (option & kCEETokenStringOptionCompact) {
            if (cee_token_id_is_newline(token->identifier) ||
                 cee_token_id_is_whitespace(token->identifier))
                goto skip;
        }

        str = cee_string_from_token(subject, token);
        if (str) {
            cee_strconcat0(&dump, str, NULL);
            cee_free(str);
        }

skip:
        p = TOKEN_NEXT(p);
    }
    return dump;
}

void cee_string_from_tokens_print(const cee_char* subject,
                                  CEEList* p,
                                  CEETokenStringOption option)
{
    cee_char* str = cee_string_from_tokens(subject, p, option);
    if (!str)
        return;
    fprintf(stdout, "%s", str);
    cee_free(str);
}

cee_char* cee_string_from_token_slice(const cee_char* subject,
                                      CEEList* p,
                                      CEEList* q,
                                      CEETokenStringOption option)
{
    if (!subject || !p)
        return NULL;
    
    cee_char* dump = NULL;
    CEEToken* token = NULL;
    cee_char* str = NULL;
    
    while (p) {
        token = p->data;
        
        if (option & kCEETokenStringOptionCompact) {
            if (cee_token_id_is_newline(token->identifier) ||
                 cee_token_id_is_whitespace(token->identifier))
                goto skip;
        }
        
        str = cee_string_from_token(subject, token);
        if (str) {
            cee_strconcat0(&dump, str, NULL);
            cee_free(str);
        }
skip:
        if (q && p == q)
            break;
        
        p = TOKEN_NEXT(p);
    }
    return dump;
}

void cee_string_from_token_slice_print(const cee_char* subject,
                                       CEEList* p,
                                       CEEList* q,
                                       CEETokenStringOption option)
{
    cee_char* str = cee_string_from_token_slice(subject, p, q, option);
    if (!str)
        return;
    fprintf(stdout, "%s", str);
    cee_free(str);
}

void cee_string_concat_with_token(cee_char** str,
                                  const cee_char* subject,
                                  CEEToken* token)
{
    cee_char* token_str = cee_string_from_token(subject, token);
    if (token_str) {
        cee_strconcat0(str, token_str, NULL);
        cee_free(token_str);
    }
}

cee_boolean cee_string_regular_line_format(cee_char** descriptor)
{
    if (!descriptor || !*descriptor)
        return FALSE;
    
    cee_char* p = *descriptor;
    cee_size len = strlen(*descriptor);
    cee_size num = 0;
    
    while (*p) {
        if (*p == '\r' ||
            *p == '\n' ||
            *p == '\v' ||
            *p == '\t')
            *p = ' ';
        p ++;
    }
    
    p = *descriptor;
    while (*p) {
        if (*p == ' ' && *(p + 1) == ' ') {
            cee_char* q = p + 1;
            while (*q == ' ')
                q ++;
            
            num = *descriptor + len - q + 1; /** plus '\0' */
            memmove(p + 1, q, num);
        }
        p ++;
    }
    
    *descriptor = cee_strtrim(*descriptor, " ");
    
    return TRUE;
}

//cee_char* cee_formated_string_from_token_slice(const cee_char* subject,
//                                               CEEList* p,
//                                               CEEList* q)
//{
//    CEEToken* token = NULL;
//    cee_char* str = NULL;
//    cee_char* formated = NULL;
//    cee_boolean whitespace = FALSE;
//    cee_boolean concat = TRUE;
//    
//    while (p) {
//        token = p->data;
//        
//        if (cee_token_id_is_whitespace(token->identifier) ||
//            cee_token_id_is_newline(token->identifier)) {
//            if (!whitespace) {
//                concat = TRUE;
//                whitespace = TRUE;
//            }
//            else {
//                concat = FALSE;
//            }
//        }
//        else {
//            whitespace = FALSE;
//            concat = TRUE;
//        }
//        
//        if (concat) {
//            if (whitespace) {
//                cee_strconcat0(&formated, " ", NULL);
//            }
//            else {
//                str = cee_strndup(&subject[token->offset],
//                                  token->length);
//                if (str) {
//                    cee_strconcat0(&formated, str, NULL);
//                    cee_free(str);
//                }
//            }
//        }
//        
//        if (q && p == q)
//            break;
//        
//        p = TOKEN_NEXT(p);
//    }
//    
//    return formated;
//}

//cee_char* cee_formated_string_from_token_slice_append(const cee_char* subject,
//                                                      CEEList* p,
//                                                      CEEList* q,
//                                                      const cee_char* append)
//{
//    cee_char* temp = NULL;
//    cee_char* string = NULL;
//    
//    temp = cee_formated_string_from_token_slice(subject, p, q);
//    if (temp) {
//        string = cee_strconcat(temp, append, NULL);
//        cee_free(temp);
//    }
//    
//    return string;
//}

//cee_char* cee_formated_string_from_token_slice_prepend(const cee_char* subject,
//                                                       CEEList* p,
//                                                       CEEList* q,
//                                                       const cee_char* prepend)
//{
//    cee_char* temp = NULL;
//    cee_char* string = NULL;
//    
//    temp = cee_formated_string_from_token_slice(subject, p, q);
//    if (temp) {
//        string = cee_strconcat(prepend, temp, NULL);
//        cee_free(temp);
//    }
//    
//    return string;
//}

CEEList* cee_range_from_token(CEEToken* token)
{
    CEEList* ranges = NULL;
    CEERange* range = cee_range_create(token->offset, token->length);
    ranges = cee_list_prepend(ranges, range);
    return ranges;
}

CEEList* cee_ranges_from_tokens(CEEList* tokens,
                                CEERangeListType type)
{
    CEEList* ranges = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    CEERange* range = NULL;
    cee_long offset = -1;
    cee_ulong length = 0;
    
    if (!tokens)
        return NULL;
    
    q = tokens;
    while (q) {
        p = cee_list_prepend(p, q->data);
        q = TOKEN_NEXT(q);
    }
    
    p = cee_list_sort(p, token_location_compare);
    q = p;
    
    if (type == kCEERangeListTypeSeparate) {
        while (p) {
            token = p->data;
            range = cee_range_create(token->offset, token->length);
            ranges = cee_list_prepend(ranges, range);
            p = p->next;
        }
        ranges = cee_list_reverse(ranges);
    }
    else if (type == kCEERangeListTypeContinue) {
        token = cee_list_first(p)->data;
        offset = token->offset;
        
        token = cee_list_last(p)->data;
        length = token->offset + token->length - offset;
        
        range = cee_range_create(offset, length);
        ranges = cee_list_prepend(ranges, range);
    }
    
    cee_list_free(q);
    
    return ranges;
}

CEEList* cee_ranges_from_token_slice(CEEList* p,
                                     CEEList* q,
                                     CEERangeListType type)
{
    CEEList* ranges = NULL;
    CEEList* tokens = NULL;
    while (p) {
        tokens = cee_list_prepend(tokens, p->data);
        
        if (q && p == q)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    ranges = cee_ranges_from_tokens(TOKEN_FIRST(tokens), type);
    cee_list_free(tokens);
    return ranges;
}

cee_char* cee_range_string_from_token(CEEToken* token)
{
    if (!token)
        return NULL;
    CEERange range = cee_range_make(token->offset, token->length);
    return cee_string_from_range(&range);
}

cee_char* cee_ranges_string_from_tokens(CEEList* tokens,
                                        CEERangeListType type)
{
    CEEList* ranges = NULL;
    cee_char* str = NULL;
    
    if (!tokens)
        return NULL;
    
    ranges = cee_ranges_from_tokens(tokens, type);
    str = cee_string_from_ranges(ranges);
    if (ranges)
        cee_list_free_full(ranges, cee_range_free);
    
    return str;
}

cee_char* cee_ranges_string_from_token_slice(CEEList* p,
                                             CEEList* q,
                                             CEERangeListType type)
{
    cee_char* str = NULL;
    CEEList* ranges = cee_ranges_from_token_slice(p, q, type);
    str = cee_string_from_ranges(ranges);
    if (ranges)
        cee_list_free_full(ranges, cee_range_free);
    return str;    
}

cee_boolean cee_token_id_is_whitespace(CEETokenID identifier)
{
    return identifier == kCEETokenID_WHITE_SPACE;
}

cee_boolean cee_token_id_is_newline(CEETokenID identifier)
{
    return identifier == kCEETokenID_NEW_LINE;
}

cee_boolean cee_token_is_identifier(CEEList* p,
                                    CEETokenID identifier)
{
    if (!p || !p->data)
        return FALSE;
    
    CEEToken* token = p->data;
    return token->identifier == identifier;
}

cee_boolean cee_token_is_ignore(CEEList* p)
{
    if (!p || !p->data)
        return FALSE;
    
    CEEToken* token = p->data;
    return token->state & kCEETokenStateIgnore;
}

CEEList* cee_token_not_whitespace_newline_before(CEEList* p)
{
    if (!p)
        return NULL;
    
    CEEToken* token = NULL;
    p = TOKEN_PREV(p);
    
    while (p) {
        token = p->data;
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            break;
        }
        p = TOKEN_PREV(p);
    }
    
    return p;
}

CEEList* cee_token_not_whitespace_newline_after(CEEList* p)
{
    if (!p)
        return NULL;
    
    CEEToken* token = NULL;
    
    p = TOKEN_NEXT(p);
    while (p) {
        token = p->data;
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            break;
        }
        p = TOKEN_NEXT(p);
    }
    
    return p;
}

CEEList* cee_token_not_whitespace_newline_first(CEEList* p)
{
    if (!p)
        return NULL;
    
    CEEToken* token = NULL;
    p = TOKEN_FIRST(p);
    
    while (p) {
        token = p->data;
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            break;
        }
        p = TOKEN_NEXT(p);
    }
    
    return p;
}

CEEList* cee_token_not_whitespace_newline_last(CEEList* p)
{
    if (!p)
        return NULL;
    
    CEEToken* token = NULL;
    p = TOKEN_LAST(p);
    
    while (p) {
        token = p->data;
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            break;
        }
        p = TOKEN_PREV(p);
    }
    
    return p;
}

cee_boolean cee_token_rear_is_round_brackets(CEEList* p)
{
    CEEToken* token = NULL;
    
    if (!p)
        return FALSE;
    
    p = cee_token_not_whitespace_newline_after(p);
    if (!p)
        return FALSE;
    
    BRACKET_SIGN_DECLARATION();
    
    CLEAR_BRACKETS();
    
    token = p->data;
    if (token->identifier == '(') {
        SIGN_BRACKET(token->identifier);
        p = TOKEN_NEXT(p);
        while (p) {
            token = p->data;
            SIGN_BRACKET(token->identifier);
            if (token->identifier == ')' && BRACKETS_IS_CLEAN())
                return TRUE;
            p = TOKEN_NEXT(p);
        }
    }
    return FALSE;
}

CEEList* cee_token_skip_round_brackets(CEEList* p,
                                       cee_int n)
{
    CEEToken* token = NULL;
    cee_int count = 0;
    BRACKET_SIGN_DECLARATION();
    
    CLEAR_BRACKETS();
    
    while (p) {
        token = p->data;
        
        SIGN_BRACKET(token->identifier);
        
        if (token->identifier == ')' &&
            BRACKETS_IS_CLEAN()) {
            count ++;
            if (count == n)
                return TOKEN_NEXT(p);
        }
        
        p = TOKEN_NEXT(p);
    }
    return p;
}

CEEList* cee_token_c_parameter_list_slice(CEEList* from,
                                          CEEList* to,
                                          CEETokenID separator)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* cluster = NULL;
    CEEList* clusters = NULL;
    
    BRACKET_SIGN_DECLARATION();
    CLEAR_BRACKETS();
    
    p = TOKEN_NEXT(from);
    while (TRUE) {
        
        while (p && p != to) {
            token = p->data;
            
            SIGN_BRACKET(token->identifier);
            
            if (token->identifier == separator && BRACKETS_IS_CLEAN()) {
                p = TOKEN_NEXT(p);
                break;
            }
            else {
                TOKEN_APPEND(cluster, token);
            }
            
            p = TOKEN_NEXT(p);
        }
        
        clusters = cee_list_prepend(clusters, cluster);
        cluster = NULL;
        
        if (p == to)
            break;
    }
    
    clusters = cee_list_reverse(clusters);
    
    return clusters;
}

static CEEList* _token_until_identifier(CEEList* p,
                                        CEETokenID identifier)
{
    CEEToken* token = NULL;
    while (p) {
        token = p->data;
        
        if (token->identifier == identifier)
            return p;
        
        p = TOKEN_NEXT(p);
    }
    return NULL;
}


static CEEList* _token_until_identifier_brackets_clear(CEEList* p,
                                                       CEETokenID identifier)
{
    BRACKET_SIGN_DECLARATION();
    
    CEEToken* token = NULL;
    while (p) {
        token = p->data;
        
        SIGN_BRACKET(token->identifier);
        
        if (token->identifier == identifier &&
            BRACKETS_IS_CLEAN())
            return p;
        
        p = TOKEN_NEXT(p);
    }
    return NULL;
}

CEEList* cee_token_until_identifier(CEEList* p,
                                    CEETokenID identifier,
                                    cee_boolean brackets_clear)
{
    if (brackets_clear)
        return _token_until_identifier_brackets_clear(p, identifier);
    return _token_until_identifier(p, identifier);
}

cee_boolean cee_token_is_round_bracket_pair(CEEList* p)
{
    CEEToken* token = NULL;
    CEETokenID identifier = kCEETokenID_UNKNOW;
    cee_int count = 0;
    
    if (!p)
        return FALSE;
    
    token = p->data;
    if (token->identifier != '(')
        return FALSE;
    
    BRACKET_SIGN_DECLARATION();
    
    CLEAR_BRACKETS();
    while (p) {
        token = p->data;
        identifier = token->identifier;
        
        SIGN_BRACKET(identifier);
        
        if (BRACKETS_IS_CLEAN()) {
            if (identifier == ')') {
                count ++;
                if (count == 2)
                    break;
            }
            else if (identifier == ',' || identifier == ';')
                break;
        }
        p = TOKEN_NEXT(p);
    }
    
    return count == 2;
}

CEEList* cee_token_after_round_bracket_pair(CEEList* p)
{
    CEEToken* token = NULL;
    CEETokenID identifier = kCEETokenID_UNKNOW;
    cee_int count = 0;
    
    BRACKET_SIGN_DECLARATION();
    
    CLEAR_BRACKETS();
    while (p) {
        token = p->data;
        identifier = token->identifier;
        
        SIGN_BRACKET(identifier);
        
        if (BRACKETS_IS_CLEAN()) {
            if (identifier == ')') {
                count ++;
                if (count == 2)
                    return TOKEN_NEXT(p);
            }
        }
        
        p = TOKEN_NEXT(p);
    }
    return NULL;
}

CEEList* cee_token_identifier_first_equal(CEEList* p,
                                          CEETokenID token_id)
{
    while (p) {
        if (cee_token_is_identifier(p, token_id))
            return p;
        
        p = TOKEN_NEXT(p);
    }
    return NULL;
}


CEEList* cee_tokens_from_slice(CEEList* begin,
                               CEEList* end)
{
    CEEList* tokens = NULL;
    CEEList* p = begin;
    while (p) {
        TOKEN_APPEND(tokens, p->data);
        p = TOKEN_NEXT(p);
    }
    return tokens;
}

void cee_token_slice_state_mark(CEEList* begin,
                                CEEList* end,
                                CEETokenState state)
{
    CEEList* p = begin;
    CEEToken* token = NULL;
    
    while (p) {
        token = p->data;
        token->state |= state;
        
        if (end && p == end)
            break;
        
        p = TOKEN_NEXT(p);
    }
}

void cee_tokens_state_mark(CEEList* tokens,
                           CEETokenState state)
{
    
    CEEList* p = tokens;
    CEEToken* token = NULL;
    
    while (p) {
        token = p->data;
        token->state |= state;
        
        p = TOKEN_NEXT(p);
    }
}

CEEList* cee_token_is_identifier_before(CEEList* p,
                                        CEETokenID token_id)
{
    if (!p)
        return NULL;
    
    CEEToken* token = NULL;
    p = TOKEN_PREV(p);
    
    while (p) {
        token = p->data;
        if (cee_token_is_identifier(p, token_id))
            return p;
        
        p = TOKEN_PREV(p);
    }
    
    return NULL;
}

CEESourceTokenMap* cee_source_token_map_create(const cee_char* subject)
{
    if (!subject)
        return NULL;
    
    CEESourceTokenMap* token_map = cee_malloc0(sizeof(CEESourceTokenMap));
    token_map->length = strlen(subject) + 1; /** plus NULL terminater */
    token_map->map = (cee_pointer*)cee_malloc0(sizeof(cee_pointer) * token_map->length);
    return token_map;
}

void cee_source_token_map_free(CEESourceTokenMap* token_map)
{
    if (!token_map)
        return;
    
    if (token_map->map)
        cee_free(token_map->map);
    
    cee_free(token_map);
}

void cee_source_token_map(CEESourceTokenMap* token_map,
                          CEEList* p)
{
    CEEToken* token = p->data;
    for (int i = 0; i < token->length; i ++)
        token_map->map[token->offset + i] = p;
}

CEETokenCluster* cee_token_cluster_create(CEETokenClusterType type,
                                          cee_pointer content_ref)
{
    CEETokenCluster* cluster = cee_malloc0(sizeof(CEETokenCluster));
    cluster->type = type;
    cluster->content_ref = content_ref;
    return cluster;
}

void cee_token_cluster_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETokenCluster* cluster = (CEETokenCluster*)data;
    cee_free(cluster);
}
