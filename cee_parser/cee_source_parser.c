#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "cee_source_parser.h"
#include "cee_c_parser.h"


static void source_fregment_tokens_expand(CEESourceFregment* fregment,
                                          CEEList** tokens);
static void source_fregment_round_bracket_list_expand(CEESourceFregment* fregment,
                                                      CEEList** tokens);
static void source_fregment_symbols_dump(CEESourceFregment* fregment,
                                         CEETree* tree);
static CEESourceFregment* reference_source_fregment_get(CEESourceReference* reference);

/**
 * parser
 */
CEESourceParserRef cee_parser_create(const cee_char* identifier)
{   
    CEESourceParserRef parser = cee_malloc0(sizeof(CEESourceParser));
    parser->identifier = cee_strdup(identifier);
    return parser;
}

void cee_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->identifier)
        cee_free(parser->identifier);
    cee_free(data);
}

cee_boolean cee_source_symbol_parse(CEESourceParserRef parser_ref,
                                    const cee_uchar* filepath,
                                    const cee_uchar* subject,
                                    CEESourceFregment** statement,
                                    CEESourceFregment** prep_directive,
                                    CEESourceFregment** comment,
                                    CEEList** tokens_ref,
                                    CEESourceTokenMap** source_token_map)
{
    if (!parser_ref->symbol_parse)
        return FALSE;
    
    return parser_ref->symbol_parse(parser_ref, 
                                    filepath, 
                                    subject, 
                                    statement, 
                                    prep_directive, 
                                    comment, 
                                    tokens_ref, 
                                    source_token_map);
}

cee_boolean cee_source_reference_parse(CEESourceParserRef parser_ref,
                                       const cee_uchar* filepath,
                                       const cee_uchar* subject,
                                       CEESourceTokenMap* source_token_map,
                                       CEESourceFregment* statement,
                                       CEESourceFregment* prep_directive,
                                       cee_pointer database,
                                       CEESymbolCacheRef cache,
                                       CEERange range,
                                       CEEList** references)
{
    if (!parser_ref->reference_parse)
        return FALSE;
    
    return parser_ref->reference_parse(parser_ref,
                                       filepath, 
                                       subject,
                                       source_token_map,
                                       statement,
                                       prep_directive,
                                       database,
                                       cache,
                                       range,
                                       references);
}

cee_boolean cee_source_tags_create(CEESourceParserRef parser_ref,
                                   const cee_uchar* filepath,
                                   const cee_uchar* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFregment* statement,
                                   CEESourceFregment* prep_directive,
                                   cee_pointer database,
                                   CEESymbolCacheRef cache,
                                   CEERange range,
                                   CEEList* references,
                                   CEEList** tags)
{
    if (!parser_ref->tags_create)
        return FALSE;
    
    return parser_ref->tags_create(parser_ref, 
                                   filepath, 
                                   subject, 
                                   source_token_map, 
                                   statement, 
                                   prep_directive, 
                                   database, 
                                   cache, 
                                   range,
                                   references,
                                   tags);
}
CEESourceFregment* cee_source_fregment_create(CEESourceFregmentType type,
                                              const cee_uchar* filepath,
                                              const cee_uchar* subject)
{
    CEESourceFregment* fregment = cee_malloc0(sizeof(CEESourceFregment));
    fregment->type |= type;
    fregment->filepath_ref = filepath;
    fregment->subject_ref = subject;
    return fregment;
}

void cee_source_fregment_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEESourceFregment* fregment = (CEESourceFregment*)data;
    
    cee_list_free(fregment->tokens_last);
    cee_list_free(fregment->symbols);
    cee_list_free_full(fregment->children, cee_source_fregment_free);
    
    free(fregment);
}

void cee_source_fregment_free_full(cee_pointer data)
{
    if (!data)
        return;
    
    CEESourceFregment* fregment = (CEESourceFregment*)data;
    
    cee_list_free_full(fregment->tokens_last, cee_token_free);
    cee_list_free_full(fregment->symbols, cee_symbol_free);
    cee_list_free_full(fregment->children, cee_source_fregment_free_full);
    
    free(fregment);
}

void cee_source_fregment_remove(CEESourceFregment* fregment)
{
    CEEList* p = fregment->parent->children;
    while (p) {
        if (p->data == fregment)
            break;
        p = p->next;
    }
    
    fregment->parent->children = 
        cee_list_remove_link(fregment->parent->children, p);
    cee_source_fregment_free(fregment);
}



void cee_source_fregment_descriptors_free(CEEList* descriptors)
{
    cee_list_free_full(descriptors, cee_free);
}

/**
 * source fregment
 */
CEESourceFregment* cee_source_fregment_append(CEESourceFregment* sibling,
                                              CEESourceFregmentType type,
                                              const cee_uchar* filepath,
                                              const cee_uchar* subject)
{
    if (!sibling || !sibling->parent)
        return NULL;
    
    CEESourceFregment* attached = cee_source_fregment_create(type,
                                                             filepath,
                                                             subject);
    attached->parent = sibling->parent;
    SOURCE_FREGMENT_CHILD_APPEND(sibling->parent, attached);
    return attached;
}

CEESourceFregment* cee_source_fregment_sub_attach(CEESourceFregment* main,
                                                  CEESourceFregmentType type,
                                                  const cee_uchar* filepath,
                                                  const cee_uchar* subject)
{
    if (!main)
        return NULL;
    
    CEESourceFregment* attached = cee_source_fregment_create(type,
                                                             filepath,
                                                             subject);
    attached->parent = main;
    SOURCE_FREGMENT_CHILD_APPEND(main, attached);
    return attached;
}

CEESourceFregment* cee_source_fregment_push(CEESourceFregment* main,
                                            CEESourceFregmentType type,
                                            const cee_uchar* filepath,
                                            const cee_uchar* subject)
{
    if (!main)
        return NULL;
    
    CEESourceFregment* attached = cee_source_fregment_sub_attach(main,
                                                                 type,
                                                                 filepath,
                                                                 subject);
    attached = cee_source_fregment_sub_attach(attached,
                                              kCEESourceFregmentTypeStatement,
                                              filepath,
                                              subject);
    return attached;
}

CEESourceFregment* cee_source_fregment_pop(CEESourceFregment* fregment)
{
    if (!fregment)
        return NULL;
    
    CEESourceFregment* parent = NULL;
    
    parent = fregment->parent;
    if (!parent)
        return NULL;
    
    parent = parent->parent;
    if (!parent)
        return NULL;
    
    return parent;
}

void cee_source_fregment_type_set(CEESourceFregment* fregment,
                                  CEESourceFregmentType type)
{
    if (!fregment)
        return;
    
    fregment->type |= type;
}

void cee_source_fregment_type_set_exclusive(CEESourceFregment* fregment,
                                            CEESourceFregmentType type)
{
    if (!fregment)
        return;
    
    fregment->type = type;
}

void cee_source_fregment_type_clear(CEESourceFregment* fregment,
                                    CEESourceFregmentType type)
{
    if (!fregment)
        return;
    
    fregment->type &= ~type;
}

cee_boolean cee_source_fregment_type_is(CEESourceFregment* fregment,
                                        CEESourceFregmentType type)
{
    return (fregment->type & type) != 0;
}

CEEList* cee_source_fregment_token_find(CEESourceFregment* fregment,
                                        CEETokenID* token_ids)
{
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        CEEToken* token = p->data;
        
        CEETokenID* q = token_ids;
        while (*q != -1) {
            if (token->identifier == *q)
                return p;
            q ++;
        }
        p = TOKEN_NEXT(p);
    }
    return NULL;
}

CEEList* cee_source_fregment_tokens_expand(CEESourceFregment* fregment)
{
    CEEList* tokens = NULL;
    source_fregment_tokens_expand(fregment, &tokens);
    return TOKEN_FIRST(tokens);
}

static void source_fregment_tokens_expand(CEESourceFregment* fregment,
                                          CEEList** tokens)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* current = NULL;
    CEESourceFregment* child = NULL;
    cee_int parameters = 0;
    cee_int subscripts = 0;
    cee_int i = 0;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        TOKEN_APPEND(*tokens, p->data);
        q = NULL;
        
        if (token->identifier == '(') {
            i = 0;
            q = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
            while (q) {
                current = (CEESourceFregment*)q->data;
                if (cee_source_fregment_type_is(current, kCEESourceFregmentTypeRoundBracketList)) {
                    if (i == parameters)
                        break;
                    i ++;
                }
                q = SOURCE_FREGMENT_NEXT(q);
            }
            
            parameters ++;
        }
        else if (token->identifier == '[') {
            i = 0;
            q = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
            while (q) {
                current = (CEESourceFregment*)q->data;
                if (cee_source_fregment_type_is(current, kCEESourceFregmentTypeSquareBracketList)) {
                    if (i == subscripts)
                        break;
                    i ++;
                }
                q = SOURCE_FREGMENT_NEXT(q);
            }
            
            subscripts ++;
        }
        
        if (q) {
            child = q->data;
            q = SOURCE_FREGMENT_CHILDREN_FIRST(child);
            while (q) {
                child = q->data;
                source_fregment_tokens_expand(child, tokens);
                q = SOURCE_FREGMENT_NEXT(q);
            }
        }
        
        p = TOKEN_NEXT(p);
    }
}

CEEList* cee_source_fregment_round_bracket_list_expand(CEESourceFregment* fregment)
{
    CEEList* tokens = NULL;
    source_fregment_round_bracket_list_expand(fregment, &tokens);
    return tokens;
}

static void source_fregment_round_bracket_list_expand(CEESourceFregment* fregment,
                                                      CEEList** tokens)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* current = NULL;
    CEESourceFregment* child = NULL;
    cee_int parameters = 0;
    cee_int i = 0;

    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        TOKEN_APPEND(*tokens, p->data);
        q = NULL;
        
        if (token->identifier == '(') {
            i = 0;
            q = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
            while (q) {
                current = (CEESourceFregment*)q->data;
                if (cee_source_fregment_type_is(current, kCEESourceFregmentTypeRoundBracketList)) {
                    if (i == parameters)
                        break;
                    i ++;
                }
                q = SOURCE_FREGMENT_NEXT(q);
            }
            
            parameters ++;
        }
        
        if (q) {
            child = q->data;
            q = SOURCE_FREGMENT_CHILDREN_FIRST(child);
            while (q) {
                child = q->data;
                source_fregment_round_bracket_list_expand(child, tokens);
                q = SOURCE_FREGMENT_NEXT(q);
            }
        }
        
        p = TOKEN_NEXT(p);
    }
}

void cee_source_fregment_tokens_remove_from(CEESourceFregment* fregment,
                                            CEEList* p)
{
    CEEList* prev = NULL;
    CEEList* q = NULL;
    
    q = fregment->tokens_last;
    if (fregment->tokens == p) {
        fregment->tokens_last = fregment->tokens = NULL;
    }
    else {
        prev = TOKEN_PREV(p);
        if (prev)
            TOKEN_NEXT_SET(prev, NULL);
        TOKEN_PREV_SET(p, NULL);
        fregment->tokens_last = prev;
    }
    cee_list_free_full(q, cee_token_free);
}

CEETree* cee_source_fregment_symbol_tree_create(CEESourceFregment* fregment)
{
    if (!cee_source_fregment_type_is(fregment, kCEESourceFregmentTypeRoot))
        return NULL;
    
    CEETree* tree = cee_tree_create();
    source_fregment_symbols_dump(fregment, tree);
    return tree;
}

static void source_fregment_symbols_dump(CEESourceFregment* fregment,
                                         CEETree* tree)
{
    CEESourceSymbol* symbol = NULL;
    CEESourceFregment* child = NULL;
    CEESourceFregment* grandson = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* s = NULL;
    CEETree* leaf = NULL;
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        
        child = p->data;
        
        if (cee_source_fregment_type_is(child, kCEESourceFregmentTypeSourceList) ||
            cee_source_fregment_type_is(child, kCEESourceFregmentTypeRoundBracketList) ||
            cee_source_fregment_type_is(child, kCEESourceFregmentTypeSquareBracketList) ||
            cee_source_fregment_type_is(child, kCEESourceFregmentTypeCurlyBracketList)) {
        
            q =  SOURCE_FREGMENT_CHILDREN_FIRST(child);
            while (q) {
                grandson = q->data;
                
                if (cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeFunctionDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeEnumurators) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypePrepDirective)) {
                    
                    s = grandson->symbols;
                    while (s) {
                        
                        symbol = s->data;
                        
                        if (symbol->type == kCEESourceSymbolTypeFunctionDefinition ||
                            symbol->type == kCEESourceSymbolTypeMessageDefinition ||
                            symbol->type == kCEESourceSymbolTypeFunctionDeclaration ||
                            symbol->type == kCEESourceSymbolTypeVariableDeclaration ||
                            symbol->type == kCEESourceSymbolTypeCustomTypeDeclaration ||
                            symbol->type == kCEESourceSymbolTypeProperty ||
                            symbol->type == kCEESourceSymbolTypeMessageDeclaration ||
                            symbol->type == kCEESourceSymbolTypeTypedef ||
                            symbol->type == kCEESourceSymbolTypeEnumerator ||
                            symbol->type == kCEESourceSymbolTypePrepDirectiveDefine) {
                            
                            leaf = cee_tree_create();
                            leaf->data = symbol;
                            tree->children = cee_list_prepend(tree->children, leaf);
                            
                        }
                        s = s->next;
                    }
                }
                
                if (cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeTemplateDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeNamespaceDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeClassDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeEnumDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeUnionDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeInterfaceDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeImplementationDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeProtocolDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeVariableBlock) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeIdentifierBlock) ) {
                    
                    s = grandson->symbols;
                    while (s) {
                        symbol = s->data;
                        
                        if (symbol->type == kCEESourceSymbolTypeTemplateDeclaration ||
                            symbol->type == kCEESourceSymbolTypeNamespaceDefinition ||
                            symbol->type == kCEESourceSymbolTypeClassDefinition ||
                            symbol->type == kCEESourceSymbolTypeEnumDefinition ||
                            symbol->type == kCEESourceSymbolTypeUnionDefinition ||
                            symbol->type == kCEESourceSymbolTypeInterfaceDeclaration ||
                            symbol->type == kCEESourceSymbolTypeImplementationDefinition ||
                            symbol->type == kCEESourceSymbolTypeProtocolDeclaration ||
                            symbol->type == kCEESourceSymbolTypeExternBlock) {
                            
                            leaf = cee_tree_create();
                            leaf->data = symbol;
                            tree->children = cee_list_prepend(tree->children, leaf);
                                                        
                            break;
                        }
                        s = s->next;
                    }
                    
                    if (!leaf) {
                        leaf = cee_tree_create();
                        tree->children = cee_list_prepend(tree->children, leaf);
                    }
                    
                    source_fregment_symbols_dump(grandson, leaf);
                }
                q = SOURCE_FREGMENT_NEXT(q);
            }
        }
        
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

CEEList* cee_source_fregment_search(CEEBST* fregments_bst,
                                    cee_long token_offset)
{
    CEEBST* p = fregments_bst;
    while (p) {
        CEEList* node = p->data;
        CEESourceFregment* fregment = node->data;
        CEEToken* token = SOURCE_FREGMENT_TOKENS_FIRST(fregment)->data;
        if (token_offset < token->offset) {
            p = p->left;
        }
        else if (token_offset >= token->offset) {
            return node;
        }
    }
    return NULL;
}

CEEList* cee_source_fregment_symbol_tags_create(CEESourceFregment* fregment,
                                                CEETagType tag_type_map[])
{
    CEEList* tags = NULL;
    CEETag* tag = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEETagType tag_type = kCEETagTypeIgnore;
    CEEList* locations = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        if (symbol) {
            locations = cee_ranges_from_string(symbol->locations);
            q = locations;
            while (q) {
                tag_type = tag_type_map[symbol->type];
                if (tag_type != kCEETagTypeIgnore) {
                    tag = cee_tag_create(tag_type_map[symbol->type],
                                         *((CEERange*)q->data));
                    tags = cee_list_prepend(tags, tag);
                }
                q = q->next;
            }
            if (locations)
                cee_list_free_full(locations, cee_range_free);
        }
        p = p->next;
    }
    return tags;
}

cee_boolean cee_source_fregment_parent_type_is(CEESourceFregment* fregment,
                                               CEESourceFregmentType type)
{
    CEESourceFregment* parent = fregment->parent;
    if (parent && (parent->type & type))
        return TRUE;
    return FALSE;
}

cee_boolean cee_source_fregment_grandfather_type_is(CEESourceFregment* fregment,
                                                    CEESourceFregmentType type)
{
    CEESourceFregment* parent = fregment->parent;
    if (parent) {
        parent = parent->parent;
        if (parent && (parent->type & type))
            return TRUE;
    }
    
    return FALSE;
}

CEESourceFregment* cee_source_fregment_parent(CEESourceFregment* fregment)
{
    return fregment->parent;
}

CEESourceFregment* cee_source_fregment_grandfather(CEESourceFregment* fregment)
{
    if (fregment->parent)
        return fregment->parent->parent;
    
    return NULL;
}

void cee_source_fregment_string_print(CEESourceFregment* fregment)
{
    if (!fregment->subject_ref)
        return;
    
    cee_char* str = cee_string_from_tokens(fregment->subject_ref,
                                           SOURCE_FREGMENT_TOKENS_FIRST(fregment), 
                                           kCEETokenStringOptionDefault);
    if (str) {
        fprintf(stdout, "%s\n", str);
        cee_free(str);
    }
}

CEERange cee_source_fregment_content_range(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* first = NULL;
    CEEToken* last = NULL;
    CEERange range = cee_range_make(-1, 0);
    
    p = cee_token_not_whitespace_newline_first(SOURCE_FREGMENT_TOKENS_FIRST(fregment));
    q = cee_token_not_whitespace_newline_last(SOURCE_FREGMENT_TOKENS_FIRST(fregment));
    
    if (p)
        first = p->data;
    
    if (q)
        last = q->data;
    
    if (first && last)
        range = cee_range_make(first->offset, 
                               last->offset + last->length - first->offset);
    
    return range;
}

void cee_source_fregment_symbols_fregment_range_mark(CEESourceFregment* fregment)
{
    CEERange range = cee_source_fregment_content_range(fregment);
    cee_char* str = cee_string_from_range(&range);
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    if (str) {
        p = fregment->symbols;
        while (p) {
            symbol = p->data;
            if (cee_symbol_is_block_type(symbol))
                symbol->fregment_range = cee_strdup(str);
            p = p->next;
        }
        cee_free(str);
    }
}

/**
 *  a leaf token identifier is '(', '[' or '{', leaf token trigger fregment push
 */
CEESourceFregment* cee_source_fregment_child_index_by_leaf(CEESourceFregment* fregment,
                                                           CEEToken* leaf_token)
{
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    CEEList* q = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    
    while (p) {
        
        if (cee_token_is_identifier(p, '(') || 
            cee_token_is_identifier(p, '[') || 
            cee_token_is_identifier(p, '{')) {
            
            if (p->data == leaf_token && q)
                return q->data;
            
            if (q)
                q = SOURCE_FREGMENT_NEXT(q);
        }
        
        p = TOKEN_NEXT(p);
    }
    
    return NULL;
}

/**
 * ignore all whtespace and new line
 */
cee_boolean cee_source_fregment_tokens_pattern_match(CEESourceFregment* fregment,
                                                     CEEList* from, 
                                                     ...)
{
#define TOKENS_MAX 32
    va_list args;
    cee_int n = 0;
    cee_int i = 0;
    CEEList* p = NULL;
    CEETokenID token_id[TOKENS_MAX];
        
    memset(token_id, 0, TOKENS_MAX * sizeof(CEETokenID));
    
    va_start(args, from);
    while (TRUE) {
        token_id[n] = va_arg(args, CEETokenID);
        if (!token_id[n])
            break;
        n ++;
        assert(n < TOKENS_MAX);
    }
    va_end(args);
    
    p = from;
    while (p) {
        if (!cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE) &&
            !cee_token_is_identifier(p, kCEETokenID_NEW_LINE)) {
            if (cee_token_is_identifier(p, token_id[i])) {
                i ++;
                if (i == n)
                    return TRUE;
            }
            else
                break;
        }
        p = TOKEN_NEXT(p);
    }
    
    return FALSE;
}

/**
* ignore all whtespace and new line
*/
CEEList* cee_source_fregment_tokens_break(CEESourceFregment* fregment,
                                          CEEList* from, 
                                          CEERange range,
                                          CEEList** sub)
{
    if (sub)
        *sub = NULL;
    
    if (!from)
        return NULL;
    
    cee_long offset = range.location;
    cee_ulong length = range.length;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* begin = NULL;
    CEEList* end = NULL;
    CEEList* next = NULL;
    cee_long i = 0;
    
    i = 0;
    p = from;
    while (p) {
        if (!cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE) &&
            !cee_token_is_identifier(p, kCEETokenID_NEW_LINE)) {
            
            if (i == offset) {
                begin = p;
                break;
            }
            
            i ++;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (!begin)
        return NULL;
    
    i = 0;
    p = begin;
    while (p) {
        
        if (!cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE) &&
            !cee_token_is_identifier(p, kCEETokenID_NEW_LINE)) {
            
            if (i == length - 1) {
                end = p;
                break;
            }

            i ++;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (!end)
        return NULL;
    
    next = TOKEN_NEXT(end);
    
    p = begin;
    while (p) {
        if (!cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE) &&
            !cee_token_is_identifier(p, kCEETokenID_NEW_LINE)) {
            q = TOKEN_NEXT(p);
            SOURCE_FREGMENT_TOKEN_NODE_REMOVE(fregment, p);
            *sub = cee_list_concat(p, *sub);
            
            if (p == end)
                break;
            
            p = q;
        }
        else
            p = TOKEN_NEXT(p);
    }
    
    return next;
}

CEESourceFregment* cee_source_fregment_sublevel_backtrack(CEESourceFregment* fregment)
{
    while (cee_source_fregment_grandfather(fregment) && 
           !cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeRoot))
        fregment = cee_source_fregment_grandfather(fregment);
    return fregment;
}


void cee_source_fregment_indexes_in_range(const cee_uchar* filepath,
                                          const cee_uchar* subject,
                                          CEESourceTokenMap* source_token_map,
                                          CEERange range,
                                          CEESourceFregment** indexes)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* fregment = NULL;
    
    if (range.location >= source_token_map->length ||
        range.location + range.length - 1 >= source_token_map->length)
        assert(FALSE);
    
    p = (CEEList*)source_token_map->map[range.location];
    while (p) {
        token = p->data;
        
        if (token->offset >= range.location + range.length)
            break;
        
        fregment = (CEESourceFregment*)token->fregment_ref;
        if (fregment) {
            
            fregment = cee_source_fregment_sublevel_backtrack(fregment);
            
            if (fregment->type & kCEESourceFregmentTypeComment) {
                if (!indexes[CEE_SOURCE_COMMENT_FREGMENT_INDEX])
                    indexes[CEE_SOURCE_COMMENT_FREGMENT_INDEX] = fregment;
            }
            else if (fregment->type & kCEESourceFregmentTypePrepDirective) {
                if (!indexes[CEE_SOURCE_PREP_DIRECTIVE_FREGMENT_INDEX])
                    indexes[CEE_SOURCE_PREP_DIRECTIVE_FREGMENT_INDEX] = fregment;
            }
            else if (!(fregment->type & kCEESourceFregmentTypeRoot)){
                if (!indexes[CEE_SOURCE_STATEMENT_FREGMENT_INDEX])
                    indexes[CEE_SOURCE_STATEMENT_FREGMENT_INDEX] = fregment;
            }
        }
        
        if (indexes[CEE_SOURCE_COMMENT_FREGMENT_INDEX] && 
            indexes[CEE_SOURCE_PREP_DIRECTIVE_FREGMENT_INDEX] && 
            indexes[CEE_SOURCE_STATEMENT_FREGMENT_INDEX])
            break;
        
        p = TOKEN_NEXT(p);
    }
    
}

CEEList* cee_references_tags_create(CEESourceReference* reference,
                                    CEESourceFregment* statement,
                                    CEESourceFregment* prep_directive,
                                    CEESourceReferenceType* symbol_reference_map,
                                    CEETagType* reference_tag_map,
                                    cee_pointer database,
                                    CEESymbolCacheRef cache)
{
    CEEList* tags = NULL;
    CEERange* range = NULL;
    CEETag* tag = NULL;
    CEEList* p = NULL;
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    
    if (reference->type == kCEESourceReferenceTypeUnknow) {
        symbols = cee_symbol_search_by_reference(reference,
                                                 statement,
                                                 prep_directive,
                                                 database, 
                                                 cache,
                                                 kCEESourceReferenceSearchOptionLocal);
        if (!symbols) {
            symbols = cee_symbol_search_by_reference(reference, 
                                                     statement,
                                                     prep_directive,
                                                     database, 
                                                     cache,
                                                     kCEESourceReferenceSearchOptionGlobal);
        }
        
        if (!symbols)
            goto exit;
        
        symbol = cee_list_nth_data(symbols, 0);
        reference->type = symbol_reference_map[symbol->type];
    }
    
    /** create tags */
    p = reference->locations;
    while (p) {
        range = p->data;          
        tag = cee_tag_create(reference_tag_map[reference->type], *range);
        tags = cee_list_prepend(tags, tag);
        p = p->next;
    }
    
    cee_list_free_full(symbols, cee_symbol_free);

exit:
    return tags;
}

CEEList* cee_symbol_search_by_reference(CEESourceReference* reference,
                                        CEESourceFregment* statement,
                                        CEESourceFregment* prep_directive,
                                        cee_pointer database,
                                        CEESymbolCacheRef cache,
                                        CEESourceReferenceSearchOption options)
{
    if (options & kCEESourceReferenceSearchOptionLocal) 
        return cee_symbol_search_by_reference_scope(reference, statement, prep_directive);
    else if (options & kCEESourceReferenceSearchOptionGlobal)
        return cee_symbol_search_by_reference_global(reference, database, cache);
    
    return NULL;
}

static CEESourceFregment* reference_source_fregment_get(CEESourceReference* reference)
{
    CEEList* tokens_ref = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* fregment = NULL;
    
    tokens_ref = reference->tokens_ref;
    if (!tokens_ref)
        return NULL;
    
    token = TOKEN_FIRST(tokens_ref)->data;
    fregment = token->fregment_ref;
    return fregment;
}

CEEList* cee_symbol_search_by_reference_scope(CEESourceReference* reference,
                                              CEESourceFregment* statement,
                                              CEESourceFregment* prep_directive)
{
    CEESourceFregment* fregment = NULL;
    CEESourceFregment* current = NULL;
    const cee_char* descriptor = NULL;
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* p = NULL;
    
    current = reference_source_fregment_get(reference);
    if (!current)
        return NULL;
    
    descriptor = reference->descriptor;
    if (!descriptor)
        return NULL;
    
    /** search current */
    symbol = cee_symbol_search_in_fregment_by_descriptor(current, descriptor);
    if (symbol) 
        goto found_symbol;
    
    /** search current sub fregment (round bracket list) */
    p = SOURCE_FREGMENT_CHILDREN_FIRST(current);
    while (p) {
        if (((CEESourceFregment*)p->data)->type & kCEESourceFregmentTypeRoundBracketList) {
            symbol = cee_symbol_search_in_child_fregment_by_descriptor(p->data, descriptor);
            if (symbol) 
                goto found_symbol;
        }
        p = SOURCE_FREGMENT_NEXT(p);
    }
    
    /** search current siblings */
    if (current->node) {
        p = SOURCE_FREGMENT_PREV(current->node);
        while (p) {     
            symbol = cee_symbol_search_in_fregment_by_descriptor(p->data, descriptor);
            if (symbol) 
                goto found_symbol;
            p = SOURCE_FREGMENT_PREV(p);
        }
    }
    
    if (!cee_source_fregment_grandfather(current))
        goto search_prep_directive;
    
    fregment = current;
    while (TRUE) {
        fregment = cee_source_fregment_grandfather(fregment);
        if (!fregment)
            break;
        
        /** search grandfather */
        symbol = cee_symbol_search_in_fregment_by_descriptor(fregment, descriptor);
        if (symbol)
            goto found_symbol;
        
        /** search grandfather's children(except current parent) */
        p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
        while (p) {
            if (p->data == cee_source_fregment_parent(current))
                break;
            
            if (((CEESourceFregment*)p->data)->type & kCEESourceFregmentTypeRoundBracketList) {
                symbol = cee_symbol_search_in_child_fregment_by_descriptor(p->data, descriptor);
                if (symbol) 
                    goto found_symbol;
            }
            
            p = SOURCE_FREGMENT_NEXT(p);
        }
        
        /** search grandfather siblings */
        if (fregment->node) {
            p = SOURCE_FREGMENT_PREV(fregment->node);
            while (p) {
                symbol = cee_symbol_search_in_fregment_by_descriptor(p->data, descriptor);
                if (symbol)
                    goto found_symbol;
                
                p = SOURCE_FREGMENT_PREV(p);
            }
        }
    }
    
search_prep_directive:
    symbol = cee_symbol_search_in_prep_directive_by_descriptor(prep_directive, descriptor);
    if (symbol)
        goto found_symbol;
    
    return NULL;
    
found_symbol:
    symbols = cee_list_prepend(symbols, cee_symbol_copy(symbol));
    return symbols;
}

CEEList* cee_symbol_search_by_reference_global(CEESourceReference* reference,
                                               cee_pointer database,
                                               CEESymbolCacheRef cache)
{    
    return cee_symbols_search_by_descriptor(database, reference->descriptor, cache);
}

CEESourceSymbol* cee_symbol_search_in_prep_directive_by_descriptor(CEESourceFregment* fregment,
                                                                   const cee_char* descriptor)
{
    CEESourceSymbol* symbol = NULL;
    CEEList* p = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypePrepDirectiveDefine &&
            !strcmp(symbol->descriptor, descriptor))
            return symbol;
        
        p = p->next;
    }
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        fregment = p->data;
        symbol = cee_symbol_search_in_prep_directive_by_descriptor(fregment, descriptor);
        if (symbol)
            return symbol;
        p = SOURCE_FREGMENT_NEXT(p);
    }
    return NULL;
}

CEESourceSymbol* cee_symbol_search_in_fregment_by_descriptor(CEESourceFregment* fregment,
                                                             const cee_char* descriptor)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->descriptor &&
            !strcmp(symbol->descriptor, descriptor))
            return symbol;
        
        p = p->next;
    }
    return NULL;
}

CEESourceSymbol* cee_symbol_search_in_child_fregment_by_descriptor(CEESourceFregment* fregment,
                                                                   const cee_char* descriptor)
{
    CEESourceSymbol* symbol = NULL;
    CEEList* p = NULL;
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        symbol = cee_symbol_search_in_fregment_by_descriptor(p->data, descriptor);
        if (symbol) 
            return symbol;
        
        p = SOURCE_FREGMENT_NEXT(p);
    }
    return NULL;
}

CEESourceSymbol* cee_symbol_search_in_fregment_by_type(CEESourceFregment* fregment,
                                                       CEESourceSymbolType type)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == type)
            return symbol;
        
        p = p->next;
    }
    return NULL;
}

CEESourceTokenMap* cee_source_token_map_create(const cee_uchar* subject)
{
    CEESourceTokenMap* token_map = cee_malloc0(sizeof(CEESourceTokenMap));
    token_map->length = strlen((const cee_char*)subject) + 1; /** length contain NULL terminater */
    token_map->map = (cee_pointer*)cee_malloc0(sizeof(cee_pointer) * token_map->length);
    return token_map;
}

void cee_source_token_map_free(CEESourceTokenMap* token_map)
{
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

