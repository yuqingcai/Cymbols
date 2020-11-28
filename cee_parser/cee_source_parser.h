#ifndef __CEE_SOURCE_PARSER_H__
#define __CEE_SOURCE_PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"
#include "cee_symbol.h"
#include "cee_reference.h"
#include "cee_backend.h"

typedef enum _CEESourceFregmentType {
    kCEESourceFregmentTypeRoot                      = 0 ,
    kCEESourceFregmentTypeSourceList                    ,
    kCEESourceFregmentTypeComment                       ,
    kCEESourceFregmentTypePrepDirective                 ,
    kCEESourceFregmentTypePrepDirectiveCondition        ,
    kCEESourceFregmentTypePrepDirectiveBranch           ,
    kCEESourceFregmentTypeStatement                     ,
    kCEESourceFregmentTypeRoundBracketList              ,       /** ( ) */
    kCEESourceFregmentTypeSquareBracketList             ,       /** [ ] */
    kCEESourceFregmentTypeCurlyBracketList              ,       /** { } */
    kCEESourceFregmentTypeStatementBlock                ,
    kCEESourceFregmentTypeAssignmentBlock               ,
    kCEESourceFregmentTypeTemplateDeclaration           ,
    kCEESourceFregmentTypeFunctionDefinition            ,
    kCEESourceFregmentTypeFunctionDeclaration           ,
    kCEESourceFregmentTypeNamespaceDefinition           ,
    kCEESourceFregmentTypeClassDefinition               ,
    kCEESourceFregmentTypeUnionDefinition               ,
    kCEESourceFregmentTypeEnumDefinition                ,
    kCEESourceFregmentTypeDeclaration                   ,
    kCEESourceFregmentTypeEnumurators                   ,
    kCEESourceFregmentTypeInterfaceDefinition           ,
    kCEESourceFregmentTypeInterfaceDeclaration          ,
    kCEESourceFregmentTypeImplementationDefinition      ,
    kCEESourceFregmentTypeProtocolDeclaration           ,
    kCEESourceFregmentTypeVariableBlock                 ,
    kCEESourceFregmentTypeLabelDeclaration              ,
    kCEESourceFregmentTypeIdentifierBlock               ,
    kCEESourceFregmentTypeXMLTagStart                   ,
    kCEESourceFregmentTypeXMLTagEnd                     ,
    kCEESourceFregmentTypeXMLTagList                    ,
    kCEESourceFregmentTypeCSSBlock                      ,
    /** 
     * ... 
     *
     */
    CEESourceFregmentTypeMax
} CEESourceFregmentType;


typedef enum _CEESourceFregmentIndex {
    kCEESourceFregmentIndexComment = 0,
    kCEESourceFregmentIndexPrepDirective,
    kCEESourceFregmentIndexStatement,
    kCEESourceFregmentIndexMax,
} CEESourceFregmentIndex;

typedef struct _CEESourceFregment {
    struct _CEESourceFregment* parent;
    CEEList* children;
    cee_char type[CEESourceFregmentTypeMax];
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEEList* tokens_ref;
    CEEList* tokens_ref_last;
    CEEList* symbols;
    CEEList* node;
    cee_char* filetype;
} CEESourceFregment;

#define SOURCE_FREGMENT_TOKENS_FIRST(fregment) ((CEESourceFregment*)fregment)->tokens_ref
#define SOURCE_FREGMENT_TOKENS_LAST(fregment) ((CEESourceFregment*)fregment)->tokens_ref_last

#define SOURCE_FREGMENT_TOKEN_PUSH(fregment, token, log_fregment_in_token) {\
    if (log_fregment_in_token)\
        token->fregment_ref = ((CEESourceFregment*)fregment);\
    ((CEESourceFregment*)fregment)->tokens_ref_last = cee_list_prepend(((CEESourceFregment*)fregment)->tokens_ref_last, (token));\
    if (!((CEESourceFregment*)fregment)->tokens_ref)\
        ((CEESourceFregment*)fregment)->tokens_ref = ((CEESourceFregment*)fregment)->tokens_ref_last;\
}

#define SOURCE_FREGMENT_TOKEN_NODE_REMOVE(fregment, p) {\
    if (((CEESourceFregment*)fregment)->tokens_ref_last) {\
        if (p == ((CEESourceFregment*)fregment)->tokens_ref)\
            ((CEESourceFregment*)fregment)->tokens_ref = ((CEESourceFregment*)fregment)->tokens_ref->prev;\
        ((CEESourceFregment*)fregment)->tokens_ref_last = cee_list_remove_link(((CEESourceFregment*)fregment)->tokens_ref_last, p);\
        if (!((CEESourceFregment*)fregment)->tokens_ref_last)\
            ((CEESourceFregment*)fregment)->tokens_ref = NULL;\
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
    cee_boolean (*token_type_matcher)(CEEToken*,
                                      CEETokenType);
    cee_boolean (*symbol_parse)(CEESourceParserRef,
                                const cee_char*,
                                const cee_char*,
                                CEESourceFregment**,
                                CEESourceFregment**,
                                CEESourceFregment**,
                                CEEList**,
                                CEESourceTokenMap**);
    
    cee_boolean (*reference_parse)(CEESourceParserRef,
                                   const cee_char*,
                                   const cee_char*,
                                   CEESourceTokenMap*,
                                   CEESourceFregment*,
                                   CEESourceFregment*,
                                   CEERange,
                                   CEEList**);
} CEESourceParser;

void cee_parsers_init(void);
CEESourceParserRef cee_parser_create(const cee_char* identifier);
void cee_parser_free(cee_pointer data);
cee_boolean cee_source_symbol_parse(CEESourceParserRef parser_ref,
                                    const cee_char* filepath,
                                    const cee_char* subject,
                                    CEESourceFregment** prep_directive,
                                    CEESourceFregment** statement,
                                    CEESourceFregment** comment,
                                    CEEList** tokens_ref,
                                    CEESourceTokenMap** source_token_map);
cee_boolean cee_source_reference_parse(CEESourceParserRef parser_ref,
                                       const cee_char* filepath,
                                       const cee_char* subject,
                                       CEESourceTokenMap* source_token_map,
                                       CEESourceFregment* prep_directive,
                                       CEESourceFregment* statement,
                                       CEERange range,
                                       CEEList** references);

CEESourceFregment* cee_source_fregment_create(CEESourceFregmentType type,
                                              const cee_char* filepath,
                                              const cee_char* subject,
                                              const cee_char* filetype);
cee_int cee_source_fregment_count_get(void);
void cee_source_fregment_free(cee_pointer data);
void cee_source_fregment_free_full(cee_pointer data);
void cee_source_fregment_remove(CEESourceFregment* fregment);
void cee_source_fregment_descriptors_free(CEEList* descriptors);
CEESourceFregment* cee_source_fregment_attach(CEESourceFregment* sibling,
                                              CEESourceFregmentType type,
                                              const cee_char* filepath,
                                              const cee_char* subject,
                                              const cee_char* filetype);
CEESourceFregment* cee_source_fregment_sub_attach(CEESourceFregment* main,
                                                  CEESourceFregmentType type,
                                                  const cee_char* filepath,
                                                  const cee_char* subject,
                                                  const cee_char* filetype);
CEESourceFregment* cee_source_fregment_push(CEESourceFregment* main,
                                            CEESourceFregmentType type,
                                            const cee_char* filepath,
                                            const cee_char* subject,
                                            const cee_char* filetype);
CEESourceFregment* cee_source_fregment_pop(CEESourceFregment* fregment);
void cee_source_fregment_type_set(CEESourceFregment* fregment,
                                  CEESourceFregmentType type);
void cee_source_fregment_type_set_exclusive(CEESourceFregment* fregment,
                                            CEESourceFregmentType type);
void cee_source_fregment_type_clear(CEESourceFregment* fregment,
                                    CEESourceFregmentType type);
cee_boolean cee_source_fregment_type_is_one_of(CEESourceFregment* fregment,
                                               CEESourceFregmentType* types);
cee_boolean cee_source_fregment_type_is(CEESourceFregment* fregment,
                                        CEESourceFregmentType type);
cee_boolean cee_source_fregment_parent_type_is(CEESourceFregment* fregment,
                                               CEESourceFregmentType type);
cee_boolean cee_source_fregment_grandfather_type_is(CEESourceFregment* fregment,
                                                    CEESourceFregmentType type);
CEEList* cee_source_fregment_token_find(CEESourceFregment* fregment,
                                        CEETokenID* token_ids);
CEEList* cee_source_fregment_tokens_expand(CEESourceFregment* fregment);
CEEList* cee_source_fregment_round_bracket_list_expand(CEESourceFregment* fregment);
void cee_source_fregment_tokens_remove_from(CEESourceFregment* fregment,
                                            CEEList* p);
CEETree* cee_source_fregment_symbol_tree_create(CEESourceFregment* fregment);
void cee_source_fregment_symbol_tree_dump_to_list(CEETree* tree,
                                                  CEEList** list);
CEEList* cee_source_fregment_symbol_tags_create(CEESourceFregment* fregment);
CEESourceFregment* cee_source_fregment_parent_get(CEESourceFregment* fregment);
CEESourceFregment* cee_source_fregment_grandfather_get(CEESourceFregment* fregment);
void cee_source_fregment_string_print(CEESourceFregment* fregment);
void cee_source_fregment_tree_string_print(CEESourceFregment* fregment);
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
cee_boolean cee_source_fregment_over_range(CEESourceFregment* fregment,
                                           CEERange range);
CEESourceFregment* cee_source_fregment_sublevel_backtrack(CEESourceFregment* fregment);
void cee_source_fregment_indexes_in_range(CEESourceTokenMap* token_map,
                                          CEERange range,
                                          CEESourceFregment** indexes);
CEEList* cee_source_fregment_symbols_search_by_type(CEESourceFregment* fregment,
                                                    CEESourceSymbolType type);
CEEList* cee_source_fregment_symbols_search_by_name(CEESourceFregment* fregment,
                                                    const cee_char* name);
CEEList* cee_source_fregment_symbols_in_children_search_by_name(CEESourceFregment* fregment,
                                                                const cee_char* name);
CEEList* cee_source_fregment_tree_symbols_search(CEESourceFregment* fregment,
                                                 CEESymbolMatcher matcher,
                                                 cee_pointer user_data);
CEETokenCluster* cee_token_cluster_search_by_buffer_offset(CEEList* references,
                                                           CEESourceFregment* prep_directive,
                                                           CEESourceFregment* statement,
                                                           cee_long offset);
CEEList* cee_symbols_search_by_reference(CEESourceSymbolReference* reference,
                                         CEESourceFregment* prep_directive,
                                         CEESourceFregment* statement,
                                         cee_pointer database,
                                         CEESourceReferenceSearchOption options);
CEEList* cee_source_tags_create(CEESourceParserRef parser_ref,
                                CEESourceTokenMap* token_map,
                                CEESourceFregment* prep_directive,
                                CEESourceFregment* statement,
                                cee_pointer database,
                                CEERange range,
                                CEEList* references);
void cee_source_fregment_tree_symbols_parent_parse(CEESourceFregment* fregment);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SOURCE_PARSER_H__ */
