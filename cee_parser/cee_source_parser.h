#ifndef __CEE_SOURCE_PARSER_H__
#define __CEE_SOURCE_PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"
#include "cee_symbol.h"

typedef enum _CEESourceFregmentType {
    kCEESourceFregmentTypeRoot                      = 1 << 1,
    kCEESourceFregmentTypeSourceList                = 1 << 2,
    kCEESourceFregmentTypeComment                   = 1 << 3,
    kCEESourceFregmentTypePrepDirective             = 1 << 4,
    kCEESourceFregmentTypeStatement                 = 1 << 5,
    kCEESourceFregmentTypeRoundBracketList          = 1 << 6,       /** ( ) */
    kCEESourceFregmentTypeSquareBracketList         = 1 << 7,       /** [ ] */
    kCEESourceFregmentTypeCurlyBracketList          = 1 << 8,       /** { } */
    kCEESourceFregmentTypeStatementBlock            = 1 << 9,
    kCEESourceFregmentTypeAssignmentBlock           = 1 << 10,
    kCEESourceFregmentTypeTemplateDeclaration       = 1 << 11,
    kCEESourceFregmentTypeFunctionDefinition        = 1 << 12,
    kCEESourceFregmentTypeNamespaceDefinition       = 1 << 13,
    kCEESourceFregmentTypeClassDefinition           = 1 << 14,
    kCEESourceFregmentTypeUnionDefinition           = 1 << 15,
    kCEESourceFregmentTypeEnumDefinition            = 1 << 16,
    kCEESourceFregmentTypeDeclaration               = 1 << 17,
    kCEESourceFregmentTypeEnumurators               = 1 << 18,
    kCEESourceFregmentTypeInterfaceDeclaration      = 1 << 19,
    kCEESourceFregmentTypeImplementationDefinition  = 1 << 20,
    kCEESourceFregmentTypeProtocolDeclaration       = 1 << 21,
    kCEESourceFregmentTypeVariableBlock             = 1 << 22,
    kCEESourceFregmentTypeLabelDeclaration          = 1 << 23,
    kCEESourceFregmentTypeIdentifierBlock           = 1 << 24,
} CEESourceFregmentType;

typedef struct _CEESourceFregment {
    struct _CEESourceFregment* parent;
    CEEList* children;
    CEESourceFregmentType type;
    const cee_uchar* filepath_ref;
    const cee_uchar* subject_ref;
    CEEList* tokens;
    CEEList* tokens_last;
    CEEList* symbols;
    CEEList* node;
} CEESourceFregment;

typedef struct _CEESourceTokenMap {
    cee_pointer* map;
    cee_ulong length;
} CEESourceTokenMap;

#define SOURCE_FREGMENT_TOKENS_FIRST(fregment) ((CEESourceFregment*)fregment)->tokens
#define SOURCE_FREGMENT_TOKENS_LAST(fregment) ((CEESourceFregment*)fregment)->tokens_last

#define SOURCE_FREGMENT_TOKEN_PUSH(fregment, token, log_fregment_in_token) {\
    if (log_fregment_in_token)\
        token->fregment_ref = ((CEESourceFregment*)fregment);\
    ((CEESourceFregment*)fregment)->tokens_last = cee_list_prepend(((CEESourceFregment*)fregment)->tokens_last, (token));\
    if (!((CEESourceFregment*)fregment)->tokens)\
        ((CEESourceFregment*)fregment)->tokens = ((CEESourceFregment*)fregment)->tokens_last;\
}

#define SOURCE_FREGMENT_TOKEN_NODE_REMOVE(fregment, p) {\
    if (((CEESourceFregment*)fregment)->tokens_last) {\
        if (p == ((CEESourceFregment*)fregment)->tokens)\
            ((CEESourceFregment*)fregment)->tokens = ((CEESourceFregment*)fregment)->tokens->prev;\
        ((CEESourceFregment*)fregment)->tokens_last = cee_list_remove_link(((CEESourceFregment*)fregment)->tokens_last, p);\
        if (!((CEESourceFregment*)fregment)->tokens_last)\
            ((CEESourceFregment*)fregment)->tokens = NULL;\
    }\
}

#define SOURCE_FREGMENT_CHILD_APPEND(main, child) {\
    ((CEESourceFregment*)main)->children = cee_list_prepend(((CEESourceFregment*)main)->children, (child));\
    ((CEESourceFregment*)child)->node = ((CEESourceFregment*)main)->children;\
}

#define SOURCE_FREGMENT_CHILDREN_FIRST(fregment) cee_list_last(((CEESourceFregment*)fregment)->children)
#define SOURCE_FREGMENT_CHILDREN_LAST(fregment) cee_list_first(((CEESourceFregment*)fregment)->children)
#define SOURCE_FREGMENT_NEXT(p) (p)->prev
#define SOURCE_FREGMENT_PREV(p) (p)->next

#define SOURCE_FREGMENT_NODE_HAS_NEXT(node) ((node)->prev != NULL)
#define SOURCE_FREGMENT_NODE_NEXT(node) ((node)->prev)

typedef struct _CEESourceParser* CEESourceParserRef;

typedef struct _CEESourceParser {
    cee_char* identifier;
    cee_pointer imp;
    cee_boolean (*symbol_parse)(CEESourceParserRef,
                                const cee_uchar*,
                                const cee_uchar*,
                                CEESourceFregment**,
                                CEESourceFregment**,
                                CEESourceFregment**,
                                CEEList**,
                                CEESourceTokenMap**);
    
    cee_boolean (*reference_parse)(CEESourceParserRef,
                                   const cee_uchar*,
                                   const cee_uchar*,
                                   CEESourceTokenMap*,
                                   CEESourceFregment*,
                                   CEESourceFregment*,
                                   cee_pointer,
                                   CEESymbolCacheRef,
                                   CEERange,
                                   CEEList**);
        
    cee_boolean (*tags_create)(CEESourceParserRef,
                               const cee_uchar*,
                               const cee_uchar*,
                               CEESourceTokenMap*,
                               CEESourceFregment*,
                               CEESourceFregment*,
                               cee_pointer,
                               CEESymbolCacheRef,
                               CEERange,
                               CEEList*,
                               CEEList**);
} CEESourceParser;

CEESourceParserRef cee_parser_create(const cee_char* identifier);
void cee_parser_free(cee_pointer data);
cee_boolean cee_source_symbol_parse(CEESourceParserRef parser_ref,
                                    const cee_uchar* filepath,
                                    const cee_uchar* subject,
                                    CEESourceFregment** statement,
                                    CEESourceFregment** prep_directive,
                                    CEESourceFregment** comment,
                                    CEEList** tokens_ref,
                                    CEESourceTokenMap** source_token_map);
cee_boolean cee_source_reference_parse(CEESourceParserRef parser_ref,
                                       const cee_uchar* filepath,
                                       const cee_uchar* subject,
                                       CEESourceTokenMap* source_token_map,
                                       CEESourceFregment* statement,
                                       CEESourceFregment* prep_directive,
                                       cee_pointer database,
                                       CEESymbolCacheRef cache,
                                       CEERange range,
                                       CEEList** references);
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
                                   CEEList** tags);
CEESourceFregment* cee_source_fregment_create(CEESourceFregmentType type,
                                              const cee_uchar* filepath,
                                              const cee_uchar* subject);
void cee_source_fregment_free(cee_pointer data);
void cee_source_fregment_free_full(cee_pointer data);
void cee_source_fregment_remove(CEESourceFregment* fregment);
void cee_source_fregment_descriptors_free(CEEList* descriptors);
CEESourceFregment* cee_source_fregment_append(CEESourceFregment* sibling,
                                              CEESourceFregmentType type,
                                              const cee_uchar* filepath,
                                              const cee_uchar* subject);
CEESourceFregment* cee_source_fregment_sub_attach(CEESourceFregment* main,
                                                  CEESourceFregmentType type,
                                                  const cee_uchar* filepath,
                                                  const cee_uchar* subject);
CEESourceFregment* cee_source_fregment_push(CEESourceFregment* main,
                                            CEESourceFregmentType type,
                                            const cee_uchar* filepath,
                                            const cee_uchar* subject);
CEESourceFregment* cee_source_fregment_pop(CEESourceFregment* fregment);
void cee_source_fregment_type_set(CEESourceFregment* fregment,
                                  CEESourceFregmentType type);
void cee_source_fregment_type_set_exclusive(CEESourceFregment* fregment,
                                            CEESourceFregmentType type);
void cee_source_fregment_type_clear(CEESourceFregment* fregment,
                                    CEESourceFregmentType type);
cee_boolean cee_source_fregment_type_is(CEESourceFregment* fregment,
                                        CEESourceFregmentType type);
CEEList* cee_source_fregment_token_find(CEESourceFregment* fregment,
                                        CEETokenID* token_ids);
CEEList* cee_source_fregment_tokens_expand(CEESourceFregment* fregment);
CEEList* cee_source_fregment_round_bracket_list_expand(CEESourceFregment* fregment);
void cee_source_fregment_tokens_remove_from(CEESourceFregment* fregment,
                                            CEEList* p);
void cee_parser_tags_request_range_set(CEESourceParser* parser,
                                       CEERange range);
CEETree* cee_source_fregment_symbol_tree_create(CEESourceFregment* fregment);
CEEList* cee_source_fregment_search(CEEBST* fregments_bst,
                                    cee_long token_offset);
CEEList* cee_source_fregment_symbol_tags_create(CEESourceFregment* fregment,
                                                CEETagType tag_type_map[]);
cee_boolean cee_source_fregment_parent_type_is(CEESourceFregment* fregment,
                                               CEESourceFregmentType type);
cee_boolean cee_source_fregment_grandfather_type_is(CEESourceFregment* fregment,
                                                    CEESourceFregmentType type);
CEESourceFregment* cee_source_fregment_parent(CEESourceFregment* fregment);
CEESourceFregment* cee_source_fregment_grandfather(CEESourceFregment* fregment);
void cee_source_fregment_string_print(CEESourceFregment* fregment);
CEERange cee_source_fregment_content_range(CEESourceFregment* fregment);
void cee_source_fregment_symbols_fregment_range_mark(CEESourceFregment* fregment);
CEESourceFregment* cee_source_fregment_child_index_by_leaf(CEESourceFregment* fregment,
                                                           CEEToken* leaf_token);
cee_boolean cee_source_fregment_tokens_pattern_match(CEESourceFregment* fregment,
                                                     CEEList* from, 
                                                     ...);
CEEList* cee_source_fregment_tokens_break(CEESourceFregment* fregment,
                                          CEEList* from, 
                                          CEERange range,
                                          CEEList** sub);
CEESourceFregment* cee_source_fregment_sublevel_backtrack(CEESourceFregment* fregment);
void cee_source_fregment_indexes_in_range(const cee_uchar* filepath,
                                          const cee_uchar* subject,
                                          CEESourceTokenMap* source_token_map,
                                          CEERange range,
                                          CEESourceFregment** indexes);
CEEList* cee_references_tags_create(CEESourceReference* reference,
                                    struct _CEESourceFregment* statement,
                                    struct _CEESourceFregment* prep_directive,
                                    CEESourceReferenceType* symbol_reference_map,
                                    CEETagType* reference_tag_map,
                                    cee_pointer database,
                                    CEESymbolCacheRef cache);

CEEList* cee_symbol_search_by_reference(CEESourceReference* reference,
                                        struct _CEESourceFregment* statement,
                                        struct _CEESourceFregment* prep_directive,
                                        cee_pointer database,
                                        CEESymbolCacheRef cache,
                                        CEESourceReferenceSearchOption options);

CEEList* cee_symbol_search_by_reference_scope(CEESourceReference* reference,
                                              CEESourceFregment* statement,
                                              CEESourceFregment* prep_directive);
CEEList* cee_symbol_search_by_reference_global(CEESourceReference* reference,
                                               cee_pointer database,
                                               CEESymbolCacheRef cache);

CEESourceSymbol* cee_symbol_search_in_prep_directive_by_descriptor(CEESourceFregment* fregment,
                                                                   const cee_char* descriptor);
CEESourceSymbol* cee_symbol_search_in_fregment_by_descriptor(struct _CEESourceFregment* fregment,
                                                             const cee_char* descriptor);
CEESourceSymbol* cee_symbol_search_in_child_fregment_by_descriptor(CEESourceFregment* fregment,
                                                                   const cee_char* descriptor);
CEESourceSymbol* cee_symbol_search_in_fregment_by_type(CEESourceFregment* fregment,
                                                       CEESourceSymbolType type);

CEESourceTokenMap* cee_source_token_map_create(const cee_uchar* subject);
void cee_source_token_map_free(CEESourceTokenMap* token_map);
void cee_source_token_map(CEESourceTokenMap* token_map,
                          CEEList* p);

#define CEE_SOURCE_FREGMENT_TYPES_MAX              3
#define CEE_SOURCE_COMMENT_FREGMENT_INDEX          0
#define CEE_SOURCE_PREP_DIRECTIVE_FREGMENT_INDEX   1
#define CEE_SOURCE_STATEMENT_FREGMENT_INDEX        2

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SOURCE_ANALYSER_H__ */
