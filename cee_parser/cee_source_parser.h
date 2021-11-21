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

typedef enum _CEESourceFragmentType {
    kCEESourceFragmentTypeRoot                      = 0 ,
    kCEESourceFragmentTypeSourceList                    ,
    kCEESourceFragmentTypeComment                       ,
    kCEESourceFragmentTypePrepDirective                 ,
    kCEESourceFragmentTypePrepDirectiveCondition        ,
    kCEESourceFragmentTypePrepDirectiveBranch           ,
    kCEESourceFragmentTypeStatement                     ,
    kCEESourceFragmentTypeRoundBracketList              ,       /** ( ) */
    kCEESourceFragmentTypeSquareBracketList             ,       /** [ ] */
    kCEESourceFragmentTypeCurlyBracketList              ,       /** { } */
    kCEESourceFragmentTypeStatementBlock                ,
    kCEESourceFragmentTypeAssignmentBlock               ,
    kCEESourceFragmentTypeTemplateDeclaration           ,
    kCEESourceFragmentTypeFunctionDefinition            ,
    kCEESourceFragmentTypeFunctionDeclaration           ,
    kCEESourceFragmentTypeNamespaceDefinition           ,
    kCEESourceFragmentTypeClassDefinition               ,
    kCEESourceFragmentTypeStructDefinition              ,
    kCEESourceFragmentTypeUnionDefinition               ,
    kCEESourceFragmentTypeEnumDefinition                ,
    kCEESourceFragmentTypeDeclaration                   ,
    kCEESourceFragmentTypeEnumerators                   ,
    kCEESourceFragmentTypeObjectDefinition              ,
    kCEESourceFragmentTypeEnumeratorBlock               ,
    kCEESourceFragmentTypeInterfaceDefinition           ,
    kCEESourceFragmentTypeInterfaceDeclaration          ,
    kCEESourceFragmentTypeImplementationDefinition      ,
    kCEESourceFragmentTypeProtocolDeclaration           ,
    kCEESourceFragmentTypeVariableBlock                 ,
    kCEESourceFragmentTypeLabelDeclaration              ,
    kCEESourceFragmentTypeIdentifierBlock               ,
    kCEESourceFragmentTypeXMLTagStart                   ,
    kCEESourceFragmentTypeXMLTagEnd                     ,
    kCEESourceFragmentTypeXMLTagList                    ,
    kCEESourceFragmentTypeCSSBlock                      ,
    /** 
     * ... 
     *
     */
    CEESourceFragmentTypeMax
} CEESourceFragmentType;


typedef enum _CEESourceFragmentIndex {
    kCEESourceFragmentIndexComment = 0,
    kCEESourceFragmentIndexPrepDirective,
    kCEESourceFragmentIndexStatement,
    kCEESourceFragmentIndexMax,
} CEESourceFragmentIndex;

typedef struct _CEESourceFragment {
    struct _CEESourceFragment* parent;
    CEEList* children;
    cee_char type[CEESourceFragmentTypeMax];
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEEList* tokens_ref;
    CEEList* tokens_ref_last;
    CEEList* symbols;
    CEEList* node_ref;
    cee_char* filetype;
    CEEList* attributes;
} CEESourceFragment;

#define SOURCE_FREGMENT_TOKENS_FIRST(fragment) ((CEESourceFragment*)fragment)->tokens_ref
#define SOURCE_FREGMENT_TOKENS_LAST(fragment) ((CEESourceFragment*)fragment)->tokens_ref_last

#define SOURCE_FREGMENT_TOKEN_PUSH(fragment, token, log_fragment_in_token) {\
    if (log_fragment_in_token)\
        token->fragment_ref = ((CEESourceFragment*)fragment);\
    ((CEESourceFragment*)fragment)->tokens_ref_last = cee_list_prepend(((CEESourceFragment*)fragment)->tokens_ref_last, (token));\
    if (!((CEESourceFragment*)fragment)->tokens_ref)\
        ((CEESourceFragment*)fragment)->tokens_ref = ((CEESourceFragment*)fragment)->tokens_ref_last;\
}

#define SOURCE_FREGMENT_TOKEN_POP(fragment) {\
    CEEToken* pop = NULL;\
    if (fragment->tokens_ref_last) {\
        CEEList* p = fragment->tokens_ref_last;\
        fragment->tokens_ref_last = cee_list_remove_link(fragment->tokens_ref_last, p);\
        if (!fragment->tokens_ref_last)\
            fragment->tokens_ref = NULL;\
        pop = p->data;\
        pop->fragment_ref = NULL;\
        cee_list_free(p);\
    }\
    return pop;\
}

#define SOURCE_FREGMENT_TOKEN_NODE_REMOVE(fragment, p) {\
    if (((CEESourceFragment*)fragment)->tokens_ref_last) {\
        if (p == ((CEESourceFragment*)fragment)->tokens_ref)\
            ((CEESourceFragment*)fragment)->tokens_ref = ((CEESourceFragment*)fragment)->tokens_ref->prev;\
        ((CEESourceFragment*)fragment)->tokens_ref_last = cee_list_remove_link(((CEESourceFragment*)fragment)->tokens_ref_last, p);\
        if (!((CEESourceFragment*)fragment)->tokens_ref_last)\
            ((CEESourceFragment*)fragment)->tokens_ref = NULL;\
    }\
}

#define SOURCE_FREGMENT_CHILD_APPEND(main, child) {\
    ((CEESourceFragment*)main)->children = cee_list_prepend(((CEESourceFragment*)main)->children, (child));\
    ((CEESourceFragment*)child)->node_ref = ((CEESourceFragment*)main)->children;\
}

#define SOURCE_FREGMENT_CHILDREN_FIRST(fragment) cee_list_last(((CEESourceFragment*)fragment)->children)
#define SOURCE_FREGMENT_CHILDREN_LAST(fragment) cee_list_first(((CEESourceFragment*)fragment)->children)
#define SOURCE_FREGMENT_NEXT(p) (p)->prev
#define SOURCE_FREGMENT_PREV(p) (p)->next
#define SOURCE_FREGMENT_HAS_NEXT(p) ((p)->prev != NULL)


typedef struct _CEESourceParser* CEESourceParserRef;

typedef struct _CEESourceParser {
    cee_char* identifier;
    cee_pointer imp;
    cee_boolean (*token_type_matcher)(CEEToken*,
                                      CEETokenType);
    cee_boolean (*symbol_parse)(CEESourceParserRef,
                                const cee_char*,
                                const cee_char*,
                                CEESourceFragment**,
                                CEESourceFragment**,
                                CEESourceFragment**,
                                CEEList**,
                                CEESourceTokenMap**);
    
    cee_boolean (*reference_parse)(CEESourceParserRef,
                                   const cee_char*,
                                   const cee_char*,
                                   CEESourceTokenMap*,
                                   CEESourceFragment*,
                                   CEESourceFragment*,
                                   CEERange,
                                   CEEList**);
    
    cee_boolean (*symbol_search_in_scope)(CEESourceParserRef,
                                          CEESourceSymbolReference*,
                                          CEESourceFragment*,
                                          CEESourceFragment*,
                                          CEESourceSymbol**);
} CEESourceParser;

void cee_parsers_init(void);
CEESourceParserRef cee_parser_create(const cee_char* identifier);
void cee_parser_free(cee_pointer data);
cee_boolean cee_source_symbol_parse(CEESourceParserRef parser_ref,
                                    const cee_char* filepath,
                                    const cee_char* subject,
                                    CEESourceFragment** prep_directive,
                                    CEESourceFragment** statement,
                                    CEESourceFragment** comment,
                                    CEEList** tokens_ref,
                                    CEESourceTokenMap** source_token_map);
cee_boolean cee_source_reference_parse(CEESourceParserRef parser_ref,
                                       const cee_char* filepath,
                                       const cee_char* subject,
                                       CEESourceTokenMap* source_token_map,
                                       CEESourceFragment* prep_directive,
                                       CEESourceFragment* statement,
                                       CEERange range,
                                       CEEList** references);
CEESourceFragment* cee_source_fragment_create(CEESourceFragmentType type,
                                              const cee_char* filepath,
                                              const cee_char* subject,
                                              const cee_char* filetype);
cee_int cee_source_fragment_count_get(void);
void cee_source_fragment_free(cee_pointer data);
void cee_source_fragment_attribute_set(CEESourceFragment* fragment,
                                       const cee_char* name,
                                       const cee_char* value);
const cee_char* cee_source_fragment_attribute_get(CEESourceFragment* fragment,
                                                  const cee_char* name);
void cee_source_fragment_free_full(cee_pointer data);
CEESourceFragment* cee_source_fragment_attach(CEESourceFragment* sibling,
                                              CEESourceFragmentType type,
                                              const cee_char* filepath,
                                              const cee_char* subject,
                                              const cee_char* filetype);
CEESourceFragment* cee_source_fragment_sub_attach(CEESourceFragment* main,
                                                  CEESourceFragmentType type,
                                                  const cee_char* filepath,
                                                  const cee_char* subject,
                                                  const cee_char* filetype);
CEESourceFragment* cee_source_fragment_push(CEESourceFragment* main,
                                            CEESourceFragmentType type,
                                            const cee_char* filepath,
                                            const cee_char* subject,
                                            const cee_char* filetype);
CEESourceFragment* cee_source_fragment_pop(CEESourceFragment* fragment);
void cee_source_fragment_type_set(CEESourceFragment* fragment,
                                  CEESourceFragmentType type);
void cee_source_fragment_type_set_exclusive(CEESourceFragment* fragment,
                                            CEESourceFragmentType type);
void cee_source_fragment_type_clear(CEESourceFragment* fragment,
                                    CEESourceFragmentType type);
cee_boolean cee_source_fragment_type_is_one_of(CEESourceFragment* fragment,
                                               CEESourceFragmentType* types);
cee_boolean cee_source_fragment_type_is(CEESourceFragment* fragment,
                                        CEESourceFragmentType type);
cee_boolean cee_source_fragment_parent_type_is(CEESourceFragment* fragment,
                                               CEESourceFragmentType type);
cee_boolean cee_source_fragment_grandfather_type_is(CEESourceFragment* fragment,
                                                    CEESourceFragmentType type);
CEEList* cee_source_fragment_token_find(CEESourceFragment* fragment,
                                        CEETokenID* token_ids);
CEEList* cee_source_fragment_tokens_expand(CEESourceFragment* fragment);
CEEList* cee_source_fragment_round_bracket_list_expand(CEESourceFragment* fragment);
void cee_source_fragment_tokens_remove_from(CEESourceFragment* fragment,
                                            CEEList* p);
CEETree* cee_source_fragment_symbol_tree_create(CEESourceFragment* fragment);
void cee_source_fragment_symbol_tree_dump_to_list(CEETree* tree,
                                                  CEEList** list);
CEEList* cee_source_fragment_symbol_list_type_filter(CEEList* list,
                                                     CEESourceSymbolType* types);
CEEList* cee_source_fragment_symbol_tags_create(CEESourceFragment* fragment);
CEESourceFragment* cee_source_fragment_parent_get(CEESourceFragment* fragment);
CEESourceFragment* cee_source_fragment_grandfather_get(CEESourceFragment* fragment);
void cee_source_fragment_string_print(CEESourceFragment* fragment);
void cee_source_fragment_tree_string_print(CEESourceFragment* fragment);
CEERange cee_source_fragment_content_range(CEESourceFragment* fragment);
void cee_source_fragment_symbols_fragment_range_mark(CEESourceFragment* fragment);
CEESourceFragment* cee_source_fragment_child_index_by_leaf(CEESourceFragment* fragment,
                                                           CEEToken* leaf_token);
cee_boolean cee_source_fragment_tokens_pattern_match(CEESourceFragment* fragment,
                                                     CEEList* from, 
                                                     ...);
CEEList* cee_source_fragment_tokens_break(CEESourceFragment* fragment,
                                          CEEList* from, 
                                          CEERange range,
                                          CEEList** sub);
cee_boolean cee_source_fragment_over_range(CEESourceFragment* fragment,
                                           CEERange range);
CEEList* cee_source_symbols_search_by_symbol(CEESourceSymbol* symbol,
                                             CEESourceFragment* prep_directive,
                                             CEESourceFragment* statement,
                                             cee_pointer database);
CEEList* cee_source_symbols_search_by_reference(CEESourceSymbolReference* reference,
                                                CEESourceFragment* prep_directive,
                                                CEESourceFragment* statement,
                                                cee_pointer database);
CEESourceFragment* cee_source_fragment_level_backtrack(CEESourceFragment* fragment);
void cee_source_fragment_indexes_in_range(CEESourceTokenMap* token_map,
                                          CEERange range,
                                          CEESourceFragment** indexes);
CEEList* cee_source_fragment_symbols_search_by_type(CEESourceFragment* fragment,
                                                    CEESourceSymbolType type);
CEEList* cee_source_fragment_symbols_search_by_name(CEESourceFragment* fragment,
                                                    const cee_char* name);
CEEList* cee_source_fragment_symbols_in_children_search_by_name(CEESourceFragment* fragment,
                                                                const cee_char* name);
CEEList* cee_source_fragment_tree_symbols_search(CEESourceFragment* fragment,
                                                 CEECompareFunc matcher,
                                                 cee_pointer user_data);
CEETokenCluster* cee_token_cluster_search_by_buffer_offset(CEEList* references,
                                                           CEESourceFragment* prep_directive,
                                                           CEESourceFragment* statement,
                                                           cee_long offset);
CEEList* cee_source_tags_create(CEESourceParserRef parser_ref,
                                CEESourceTokenMap* token_map,
                                CEESourceFragment* prep_directive,
                                CEESourceFragment* statement,
                                cee_pointer database,
                                CEERange range,
                                CEEList* references,
                                CEEList** cache);
void cee_source_fragment_tree_symbols_parent_parse(CEESourceFragment* fragment);
CEESourceFragment* cee_source_fragment_from_reference_get(CEESourceSymbolReference* reference);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SOURCE_PARSER_H__ */
