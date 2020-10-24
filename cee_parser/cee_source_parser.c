#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "cee_source_parser.h"

static void symbol_tag_map_init(void);
static void reference_tag_map_init(void);
static void symbol_reference_map_init(void);
static void source_fregment_tokens_expand(CEESourceFregment* fregment,
                                          CEEList** tokens);
static void source_fregment_round_bracket_list_expand(CEESourceFregment* fregment,
                                                      CEEList** tokens);
static void source_fregment_symbols_dump(CEESourceFregment* fregment,
                                         CEETree* tree);
static CEEList* local_symbols_search_by_reference(CEESourceSymbolReference* reference,
                                                  CEESourceFregment* statement,
                                                  CEESourceFregment* prep_directive,
                                                  CEESourceFregmentType searchable_child_fregment_type);
static CEESourceFregment* source_fregment_from_reference_get(CEESourceSymbolReference* reference);
static void source_fregment_tree_symbols_search_recursive(CEESourceFregment* fregment,
                                                          CEESymbolMatcher matcher,
                                                          cee_pointer user_data,
                                                          CEEList** searched);
static CEEList* reference_tags_create(CEESourceSymbolReference* reference,
                                      CEESourceFregment* prep_directive,
                                      CEESourceFregment* statement,
                                      cee_pointer database);
static CEEList* comment_tags_create(CEESourceParserRef parser_ref,
                                    CEESourceFregment* fregment,
                                    CEERange range);
static CEEList* syntax_tags_create(CEESourceParserRef parser_ref,
                                   CEESourceFregment* fregment,
                                   CEERange range);
static cee_boolean syntax_tags_create_recursive(CEESourceParserRef parser_ref,
                                                CEESourceFregment* fregment,
                                                CEERange range,
                                                CEEList** tags);
static CEEList* language_private_tags_create(CEESourceParserRef parser_ref,
                                             CEESourceFregment* fregment);

static CEETagType symbol_tag_map[kCEESourceSymbolTypeMax];
static CEETagType reference_tag_map[kCEESourceReferenceTypeMax];
static CEESourceReferenceType symbol_reference_map[kCEESourceSymbolTypeMax];

void cee_parsers_init(void) {
    symbol_tag_map_init();
    reference_tag_map_init();
    symbol_reference_map_init();
}

static void symbol_tag_map_init(void)
{
    for (cee_int i = 0; i < kCEESourceSymbolTypeMax; i ++)
        symbol_tag_map[i] = kCEETagTypeIgnore;
    
    symbol_tag_map[kCEESourceSymbolTypeUnknow]                            = kCEETagTypeIgnore;
    symbol_tag_map[kCEESourceSymbolTypePrepDirectiveDefine]               = kCEETagTypePrepDirectiveDefine;
    symbol_tag_map[kCEESourceSymbolTypePrepDirectiveIncludePath]          = kCEETagTypePrepFilename;
    symbol_tag_map[kCEESourceSymbolTypePrepDirectiveParameter]            = kCEETagTypePrepDirectiveDefineParameter;
    symbol_tag_map[kCEESourceSymbolTypeFunctionDeclaration]               = kCEETagTypeFunctionDeclaration;
    symbol_tag_map[kCEESourceSymbolTypeFunctionDefinition]                = kCEETagTypeFunctionDefinition;
    symbol_tag_map[kCEESourceSymbolTypeVariableDeclaration]               = kCEETagTypeVariable;
    symbol_tag_map[kCEESourceSymbolTypeCustomTypeDeclaration]             = kCEETagTypeTypeDeclaration;
    symbol_tag_map[kCEESourceSymbolTypeProperty]                          = kCEETagTypeProperty;
    symbol_tag_map[kCEESourceSymbolTypeMessageDeclaration]                = kCEETagTypeMessageDeclaration;
    symbol_tag_map[kCEESourceSymbolTypeMessageDefinition]                 = kCEETagTypeMessageDefinition;
    symbol_tag_map[kCEESourceSymbolTypeFunctionParameter]                 = kCEETagTypeFunctionParameter;
    symbol_tag_map[kCEESourceSymbolTypeMessageParameter]                  = kCEETagTypeMessageParameter;
    symbol_tag_map[kCEESourceSymbolTypeTypeDefine]                        = kCEETagTypeTypeDefine;
    symbol_tag_map[kCEESourceSymbolTypeClassDefinition]                   = kCEETagTypeClassDefinition;
    symbol_tag_map[kCEESourceSymbolTypeEnumDefinition]                    = kCEETagTypeEnumDefinition;
    symbol_tag_map[kCEESourceSymbolTypeUnionDefinition]                   = kCEETagTypeUnionDefinition;
    symbol_tag_map[kCEESourceSymbolTypeEnumerator]                        = kCEETagTypeEnumerator;
    symbol_tag_map[kCEESourceSymbolTypeInterfaceDeclaration]              = kCEETagTypeInterface;
    symbol_tag_map[kCEESourceSymbolTypeImplementationDefinition]          = kCEETagTypeImplementation;
    symbol_tag_map[kCEESourceSymbolTypeProtocolDeclaration]               = kCEETagTypeProtocol;
    symbol_tag_map[kCEESourceSymbolTypeNamespaceDefinition]               = kCEETagTypeNamespaceDefinition;
    symbol_tag_map[kCEESourceSymbolTypeLabel]                             = kCEETagTypeVariable;
}

static void reference_tag_map_init(void)
{
    for (cee_int i = 0; i < kCEESourceReferenceTypeMax; i ++)
        reference_tag_map[i] = kCEETagTypeIgnore;
    
    reference_tag_map[kCEESourceReferenceTypeUnknow]                      = kCEETagTypeIgnore;
    reference_tag_map[kCEESourceReferenceTypeReplacement]                 = kCEETagTypePrepDirectiveReference;
    reference_tag_map[kCEESourceReferenceTypeFunction]                    = kCEETagTypeFunctionReference;
    reference_tag_map[kCEESourceReferenceTypeCustomTypeDeclaration]       = kCEETagTypeTypeReference;
    reference_tag_map[kCEESourceReferenceTypeVariable]                    = kCEETagTypeVariableReference;
    reference_tag_map[kCEESourceReferenceTypeMember]                      = kCEETagTypeMemberReference;
    reference_tag_map[kCEESourceReferenceTypeTypeDefine]                  = kCEETagTypeTypeDefineReference;
    reference_tag_map[kCEESourceReferenceTypeEnumerator]                  = kCEETagTypeEnumeratorReference;
    reference_tag_map[kCEESourceReferenceTypeNamespace]                   = kCEETagTypeNamespaceReference;
    reference_tag_map[kCEESourceReferenceTypeLabel]                       = kCEETagTypeLabelReference;
}

static void symbol_reference_map_init(void)
{
    for (cee_int i = 0; i < kCEESourceSymbolTypeMax; i ++)
        symbol_reference_map[i] = kCEESourceReferenceTypeUnknow;
    
    symbol_reference_map[kCEESourceSymbolTypePrepDirectiveDefine]         = kCEESourceReferenceTypeReplacement;
    symbol_reference_map[kCEESourceSymbolTypePrepDirectiveParameter]      = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeFunctionDeclaration]         = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeFunctionDefinition]          = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeLabel]                       = kCEESourceReferenceTypeLabel;
    symbol_reference_map[kCEESourceSymbolTypeVariableDeclaration]         = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeCustomTypeDeclaration]       = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeProperty]                    = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeMessageDeclaration]          = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeMessageDefinition]           = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeFunctionParameter]           = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeMessageParameter]            = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeTypeDefine]                  = kCEESourceReferenceTypeTypeDefine;
    symbol_reference_map[kCEESourceSymbolTypeClassDefinition]             = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeEnumDefinition]              = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeUnionDefinition]             = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeEnumerator]                  = kCEESourceReferenceTypeEnumerator;
    symbol_reference_map[kCEESourceSymbolTypeNamespaceDefinition]         = kCEESourceReferenceTypeNamespace;
    symbol_reference_map[kCEESourceSymbolTypeInterfaceDeclaration]        = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeImplementationDefinition]    = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeProtocolDeclaration]         = kCEESourceReferenceTypeFunction;
}

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
                                    CEESourceFregment** prep_directive,
                                    CEESourceFregment** statement,
                                    CEESourceFregment** comment,
                                    CEEList** tokens_ref,
                                    CEESourceTokenMap** source_token_map)
{
    if (!parser_ref || !parser_ref->symbol_parse)
        return FALSE;
    
    return parser_ref->symbol_parse(parser_ref, 
                                    filepath, 
                                    subject, 
                                    prep_directive, 
                                    statement, 
                                    comment, 
                                    tokens_ref, 
                                    source_token_map);
}

cee_boolean cee_source_reference_parse(CEESourceParserRef parser_ref,
                                       const cee_uchar* filepath,
                                       const cee_uchar* subject,
                                       CEESourceTokenMap* source_token_map,
                                       CEESourceFregment* prep_directive,
                                       CEESourceFregment* statement,
                                       CEERange range,
                                       CEEList** references)
{
    if (!parser_ref || !parser_ref->reference_parse)
        return FALSE;
    
    return parser_ref->reference_parse(parser_ref,
                                       filepath, 
                                       subject,
                                       source_token_map,
                                       prep_directive,
                                       statement,
                                       range,
                                       references);
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
    cee_list_free_full(fregment->symbols, cee_source_symbol_free);
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
    if (!fregment)
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
                            symbol->type == kCEESourceSymbolTypeTypeDefine ||
                            symbol->type == kCEESourceSymbolTypeEnumerator ||
                            symbol->type == kCEESourceSymbolTypePrepDirectiveDefine ||
                            symbol->type == kCEESourceSymbolTypePrepDirectiveInclude) {
                            
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

void cee_source_fregment_symbol_tree_dump_to_list(CEETree* tree,
                                                  CEEList** list)
{
    if (!tree)
        return;
    
    CEEList* p = NULL;    
    CEESourceSymbol* symbol = tree->data;
    if (symbol)
        *list = cee_list_prepend(*list, symbol);
    
    if (tree->children) {
        p = CEE_TREE_CHILDREN_FIRST(tree);
        while (p) {
            tree = p->data;
            cee_source_fregment_symbol_tree_dump_to_list(tree, list);
            p = CEE_TREE_NODE_NEXT(p);
        }
    }
}

CEEList* cee_source_fregment_symbol_tags_create(CEESourceFregment* fregment)
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
                tag_type = symbol_tag_map[symbol->type];
                if (tag_type != kCEETagTypeIgnore) {
                    tag = cee_tag_create(tag_type, *((CEERange*)q->data));
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

CEESourceFregment* cee_source_fregment_parent_get(CEESourceFregment* fregment)
{
    return fregment->parent;
}

CEESourceFregment* cee_source_fregment_grandfather_get(CEESourceFregment* fregment)
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
            if (cee_source_symbol_is_block_type(symbol))
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
    while (cee_source_fregment_grandfather_get(fregment) && 
           !cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeRoot))
        fregment = cee_source_fregment_grandfather_get(fregment);
    return fregment;
}

void cee_source_fregment_indexes_in_range(CEESourceTokenMap* token_map,
                                          CEERange range,
                                          CEESourceFregment** indexes)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* fregment = NULL;
    
    if (!token_map || range.length == 0)
        return;
    
    if (range.location >= token_map->length || 
        range.location + range.length - 1 >= token_map->length)
        assert(FALSE);
    
    p = (CEEList*)token_map->map[range.location];
    while (p) {
        token = p->data;
        
        if (token->offset >= range.location + range.length)
            break;
        
        fregment = (CEESourceFregment*)token->fregment_ref;
        if (fregment) {
            
            fregment = cee_source_fregment_sublevel_backtrack(fregment);
            
            if (fregment->type & kCEESourceFregmentTypeComment) {
                if (!indexes[kCEESourceFregmentIndexComment])
                    indexes[kCEESourceFregmentIndexComment] = fregment;
            }
            else if (fregment->type & kCEESourceFregmentTypePrepDirective) {
                if (!indexes[kCEESourceFregmentIndexPrepDirective])
                    indexes[kCEESourceFregmentIndexPrepDirective] = fregment;
            }
            else if (!(fregment->type & kCEESourceFregmentTypeRoot)){
                if (!indexes[kCEESourceFregmentIndexStatement])
                    indexes[kCEESourceFregmentIndexStatement] = fregment;
            }
        }
        
        if (indexes[kCEESourceFregmentIndexComment] && 
            indexes[kCEESourceFregmentIndexPrepDirective] && 
            indexes[kCEESourceFregmentIndexStatement])
            break;
        
        p = TOKEN_NEXT(p);
    }
}

CEEList* cee_source_fregment_symbols_search_by_type(CEESourceFregment* fregment,
                                                    CEESourceSymbolType type)
{
    CEEList* symbols = NULL;
    CEEList* p = fregment->symbols;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        if (symbol->type == type)
            symbols = cee_list_prepend(symbols, symbol);
        
        p = p->next;
    }
    return symbols;
}

CEEList* cee_source_fregment_symbols_search_by_name(CEESourceFregment* fregment,
                                                    const cee_char* name)
{
    if (!fregment)
        return NULL;
    
    CEEList* symbols = NULL;
    CEEList* p = fregment->symbols;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        if (symbol->name && !strcmp(symbol->name, name))
            symbols = cee_list_prepend(symbols, symbol);
        
        p = p->next;
    }
    return symbols;
}

CEEList* cee_source_fregment_symbols_in_children_search_by_name(CEESourceFregment* fregment,
                                                                const cee_char* name)
{
    if (!fregment)
        return NULL;
    
    CEEList* symbols = NULL;
    CEEList* p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        CEESourceFregment* child = p->data;
        CEEList* q = cee_source_fregment_symbols_search_by_name(child, name);
        symbols = cee_list_concat(symbols, q);
        p = SOURCE_FREGMENT_NEXT(p);
    }
    return symbols;
}

CEEList* cee_source_fregment_tree_symbols_search(CEESourceFregment* fregment,
                                                 CEESymbolMatcher matcher,
                                                 cee_pointer user_data)
{
    /*
     *  Preprocess Directive Source Fregment Tree:
     *
     *                  kCEESourceFregmentTypeRoot
     *                  |
     *                  +-kCEESourceFregmentTypeSourceList
     *                    |
     *                    +-kCEESourceFregmentTypePrepDirective
     *                    +-kCEESourceFregmentTypePrepDirective
     *                    |
     *                    +...
     *                    |
     *                    +-kCEESourceFregmentTypePrepDirective
     *
     *
     *
     *  Statement Source Fregment Tree:
     *
     *                  kCEESourceFregmentTypeRoot
     *                  |
     *                  +-kCEESourceFregmentTypeSourceList
     *                    |
     *                    +-kCEESourceFregmentTypeFunctionDefinition
     *                    | |
     *                    | +-kCEESourceFregmentTypeRoundBracketList
     *                    | | |
     *                    | | +-kCEESourceFregmentTypeStatement
     *                    | | +-kCEESourceFregmentTypeStatement
     *                    | |
     *                    | +-kCEESourceFregmentTypeCurlyBracketList
     *                    |   |
     *                    |   +-kCEESourceFregmentTypeStatement
     *                    |   +-kCEESourceFregmentTypeStatement
     *                    |   +-kCEESourceFregmentTypeStatement
     *                    |
     *                    |
     *                    +-kCEESourceFregmentTypeStatement
     *                    +-kCEESourceFregmentTypeStatement
     *                    |
     *                    +...
     *                    |
     *                    +-kCEESourceFregmentTypeFunctionDeclaration
     *                      |
     *                      +-kCEESourceFregmentTypeRoundBracketList
     *                        |
     *                        +-kCEESourceFregmentTypeStatement
     *                        +-kCEESourceFregmentTypeStatement
     *
     */
    CEEList* symbols = NULL;
    source_fregment_tree_symbols_search_recursive(fregment,
                                                  matcher,
                                                  user_data,
                                                  &symbols);
    return symbols;
}
 
static void source_fregment_tree_symbols_search_recursive(CEESourceFregment* fregment,
                                                          CEESymbolMatcher matcher,
                                                          cee_pointer user_data,
                                                          CEEList** searched)
{
    if (!fregment)
        return;
    /*
     * cee_string_from_tokens_print(fregment->subject_ref,
     *                            fregment->tokens,
     *                            kCEETokenStringOptionDefault);
     */
    CEEList* p = fregment->symbols;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        if (matcher(symbol, user_data))
            *searched = cee_list_prepend(*searched, symbol);
        p = p->next;
    }
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        fregment = p->data;
        source_fregment_tree_symbols_search_recursive(fregment, 
                                                      matcher, 
                                                      user_data, 
                                                      searched);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

CEEList* cee_symbols_search_by_reference(CEESourceSymbolReference* reference,
                                         CEESourceFregment* prep_directive,
                                         CEESourceFregment* statement,
                                         cee_pointer database,
                                         CEESourceReferenceSearchOption options)
{
    CEEList* global = NULL;
    CEEList* symbols = NULL;
    CEEList* duplicated = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    
    if (options & kCEESourceReferenceSearchOptionLocal) {
        p = local_symbols_search_by_reference(reference,
                                              prep_directive,
                                              statement,
                                              kCEESourceFregmentTypeRoundBracketList|
                                              kCEESourceFregmentTypeCurlyBracketList);
        symbols = cee_list_concat(symbols, p);
    }
    
    if (options & kCEESourceReferenceSearchOptionGlobal) {
        p = cee_database_symbols_search_by_name(database, reference->name);
        global = cee_list_concat(global, p);
    }
    /**
     *  Remove duplicated symbols.
     *  global contain local symbols.
     */
    p = symbols;
    while (p) {
        CEESourceSymbol* symbol0 = p->data;
        q = global;
        while (q) {
            CEESourceSymbol* symbol1 = q->data;
            if (cee_source_symbols_are_equal(symbol0, symbol1))
                duplicated = cee_list_prepend(duplicated, q);
            q = q->next;
        }
        p = p->next;
    }
    
    p = duplicated;
    while (p) {
        CEEList* q = p->data;
        global = cee_list_remove_link(global, q);
        cee_list_free_full(q, cee_source_symbol_free);
        p = p->next;
    }
    cee_list_free(duplicated);
    
    symbols = cee_list_concat(symbols, global);
    
    return symbols;
}

static CEEList* local_symbols_search_by_reference(CEESourceSymbolReference* reference,
                                                  CEESourceFregment* prep_directive,
                                                  CEESourceFregment* statement,
                                                  CEESourceFregmentType searchable_child_fregment_type)
{
    CEESourceFregment* fregment = NULL;
    CEESourceFregment* current = NULL;
    const cee_char* name = NULL;
    CEEList* symbols = NULL;
    CEEList* copied = NULL;
    CEEList* p = NULL;
    
    name = reference->name;
    if (!name)
        return NULL;
        
    current = source_fregment_from_reference_get(reference);
    if (!current)
        return NULL;
        
    /** search current */
    symbols = cee_source_fregment_symbols_search_by_name(current, name);
    if (symbols) 
        goto found;
    
    /** search current child fregment (round bracket list) */
    p = SOURCE_FREGMENT_CHILDREN_FIRST(current);
    while (p) {
                
        if (((CEESourceFregment*)p->data)->type & searchable_child_fregment_type) {
            symbols = cee_source_fregment_symbols_in_children_search_by_name(p->data, name);
            if (symbols) 
                goto found;
        }
        p = SOURCE_FREGMENT_NEXT(p);
    }
    
    /** search current siblings */
    if (current->node) {
        p = SOURCE_FREGMENT_PREV(current->node);
        while (p) {
                        
            symbols = cee_source_fregment_symbols_search_by_name(p->data, name);
            if (symbols) 
                goto found;
            p = SOURCE_FREGMENT_PREV(p);
        }
    }
    
    if (!cee_source_fregment_grandfather_get(current))
        goto search_prep_directive;
    
    fregment = current;
    while (TRUE) {
        fregment = cee_source_fregment_grandfather_get(fregment);
        if (!fregment)
            break;
                
        /** search grandfather */
        symbols = cee_source_fregment_symbols_search_by_name(fregment, name);
        if (symbols)
            goto found;
        
        /** search grandfather's children(except current parent) */
        p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
        while (p) {
            if (p->data == cee_source_fregment_parent_get(current))
                break;
                        
            if (((CEESourceFregment*)p->data)->type & searchable_child_fregment_type) {
                symbols = cee_source_fregment_symbols_in_children_search_by_name(p->data, name);
                if (symbols) 
                    goto found;
            }
            
            p = SOURCE_FREGMENT_NEXT(p);
        }
        
        /** search grandfather siblings */
        if (fregment->node) {
            p = SOURCE_FREGMENT_PREV(fregment->node);
            while (p) {
                                
                symbols = cee_source_fregment_symbols_search_by_name(p->data, name);
                if (symbols)
                    goto found;
                
                p = SOURCE_FREGMENT_PREV(p);
            }
        }
    }
    
search_prep_directive:
    symbols = cee_source_fregment_tree_symbols_search(prep_directive,
                                                      cee_source_symbol_matcher_by_name,
                                                      (cee_pointer)name);
    if (symbols)
        goto found;
    
    return NULL;
    
found:
    p = symbols;
    while (p) {
        copied = cee_list_prepend(copied, cee_source_symbol_copy(p->data));
        p = p->next;
    }
    
    return copied;
}

static CEESourceFregment* source_fregment_from_reference_get(CEESourceSymbolReference* reference)
{
    if (!reference->tokens_ref)
        return NULL;
    
    CEEList* tokens_ref = reference->tokens_ref;
    CEEToken* token = TOKEN_FIRST(tokens_ref)->data;
    CEESourceFregment* fregment = token->fregment_ref;
    return fregment;
}

CEEList* cee_source_tags_create(CEESourceParserRef parser_ref,
                                CEESourceTokenMap* token_map,
                                CEESourceFregment* prep_directive,
                                CEESourceFregment* statement,
                                cee_pointer database,
                                CEERange range,
                                CEEList* references)
{
    CEEList* tags = NULL;
    CEEList* p = NULL;
    CEESourceFregment* indexes[kCEESourceFregmentIndexMax];
    memset(indexes, 0, sizeof(CEESourceFregment*)*kCEESourceFregmentIndexMax);
    cee_source_fregment_indexes_in_range(token_map, range, indexes);
    
    /** comment tags */
    if (indexes[kCEESourceFregmentIndexComment])
        tags = comment_tags_create(parser_ref, 
                                   indexes[kCEESourceFregmentIndexComment], 
                                   range);
    /** syntax tags */
    for (cee_int i = kCEESourceFregmentIndexPrepDirective; 
         i < kCEESourceFregmentIndexMax; 
         i ++) {
        
        if (!indexes[i])
            continue;
        
        tags = cee_list_concat(tags, syntax_tags_create(parser_ref, 
                                                        indexes[i], 
                                                        range));
    }
    
    /** reference tags */
    p = references;
    while (p) {
        tags = cee_list_concat(tags, reference_tags_create(p->data, 
                                                           prep_directive, 
                                                           statement, 
                                                           database));
        p = p->next;
    }
    
    tags = cee_list_sort(tags, cee_tag_compare);
    return tags;
}

static CEEList* comment_tags_create(CEESourceParserRef parser_ref,
                                    CEESourceFregment* fregment,
                                    CEERange range)
{
    CEEList* tags = NULL;
    CEEList* p = fregment->node;
    while (p) {
        fregment = p->data;
        if (cee_source_fregment_over_range(fregment, range))
            break;
        tags = cee_list_concat(tags, language_private_tags_create(parser_ref, fregment));
        p = SOURCE_FREGMENT_NEXT(p);
    }
    return tags;
}

cee_boolean cee_source_fregment_over_range(CEESourceFregment* fregment,
                                           CEERange range)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    if (!p)
        return FALSE;
    
    token = p->data;
    if (token->offset >= range.location + range.length)
        return TRUE;
    
    return FALSE;
}

static CEEList* syntax_tags_create(CEESourceParserRef parser_ref,
                                   CEESourceFregment* fregment,
                                   CEERange range)
{
    CEEList* tags = NULL;
    CEEList* p = NULL;
    p = fregment->node;
    while (p) {
        fregment = p->data;
        if (!syntax_tags_create_recursive(parser_ref, fregment, range, &tags))
            break;
        p = SOURCE_FREGMENT_NEXT(p);
    }
    return tags;
}

static cee_boolean syntax_tags_create_recursive(CEESourceParserRef parser_ref,
                                                         CEESourceFregment* fregment,
                                                         CEERange range,
                                                         CEEList** tags)
{
    if (cee_source_fregment_over_range(fregment, range))
        return FALSE;
    
    CEEList* p = NULL;
    
    *tags = cee_list_concat(*tags, cee_source_fregment_symbol_tags_create(fregment));
    *tags = cee_list_concat(*tags, language_private_tags_create(parser_ref, fregment));
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        if (!syntax_tags_create_recursive(parser_ref, p->data, range, tags))
            return FALSE;
            
        p = SOURCE_FREGMENT_NEXT(p);
    }
    return TRUE;
}

static CEEList* language_private_tags_create(CEESourceParserRef parser_ref,
                                             CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEETag* tag = NULL;
    CEEList* tags = NULL;
    CEETagType tag_type = kCEETagTypeIgnore;
    CEERange range;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (!(token->state & kCEETokenStateSymbolOccupied)) {
        
            tag_type = kCEETagTypeIgnore;
            
            if (parser_ref->token_type_matcher) {
                if (parser_ref->token_type_matcher(token, kCEETokenIdentifierTypeKeyword))
                    tag_type = kCEETagTypeKeyword;
                else if (parser_ref->token_type_matcher(token, kCEETokenIdentifierTypePrepDirective))
                    tag_type = kCEETagTypePrepDirective;
                else if (parser_ref->token_type_matcher(token, kCEETokenIdentifierTypePunctuation))
                    tag_type = kCEETagTypePunctuation;
            }
            
            if (tag_type == kCEETagTypeIgnore) {
                if (token->identifier == kCEETokenID_CONSTANT)
                    tag_type = kCEETagTypeConstant;
                else if (token->identifier == kCEETokenID_C_COMMENT)
                    tag_type = kCEETagTypeComment;
                else if (token->identifier == kCEETokenID_CPP_COMMENT)
                    tag_type = kCEETagTypeComment;
                else if (token->identifier == kCEETokenID_LITERAL)
                    tag_type = kCEETagTypeLiteral;
                else if (token->identifier == kCEETokenID_CHARACTER)
                    tag_type = kCEETagTypeCharacter;
            }
            
            if (tag_type != kCEETagTypeIgnore) {
                range = cee_range_make(token->offset, token->length);
                tag = cee_tag_create(tag_type, range);
                if (tag)
                    tags = cee_list_prepend(tags, tag);
            }
        }
        p = TOKEN_NEXT(p);
    }
    return tags;
}

static CEEList* reference_tags_create(CEESourceSymbolReference* reference,
                                      CEESourceFregment* prep_directive,
                                      CEESourceFregment* statement,
                                      cee_pointer database)
{
    CEEList* tags = NULL;
    CEERange* range = NULL;
    CEETag* tag = NULL;
    CEEList* p = NULL;
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    CEESourceReferenceSearchOption options = kCEESourceReferenceSearchOptionLocal | 
        kCEESourceReferenceSearchOptionGlobal;
    
    if (reference->type == kCEESourceReferenceTypeUnknow) {
        symbols = cee_symbols_search_by_reference(reference,
                                                  prep_directive,
                                                  statement,
                                                  database, 
                                                  options);
        if (!symbols)
            goto exit;
        
        symbol = cee_list_nth_data(symbols, 0);
        reference->type = symbol_reference_map[symbol->type];
    }
    
    CEEList* ranges = cee_ranges_from_string(reference->locations);
    p = ranges;
    while (p) {
        range = p->data;          
        tag = cee_tag_create(reference_tag_map[reference->type], *range);
        tags = cee_list_prepend(tags, tag);
        p = p->next;
    }
    cee_list_free_full(ranges, cee_range_free);
    cee_list_free_full(symbols, cee_source_symbol_free);

exit:
    return tags;
}

CEETokenCluster* cee_token_cluster_search_by_buffer_offset(CEEList* references, 
                                                           CEESourceFregment* prep_directive, 
                                                           CEESourceFregment* statement, 
                                                           cee_long offset)
{
    CEETokenCluster* cluster = NULL;
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    CEESourceSymbolReference* reference = NULL;
    CEEList* ranges = NULL;
    CEEList* p = references;
        
    while (p) {
        reference = p->data;
        ranges = cee_ranges_from_string(reference->locations);
        if (ranges) {
            if (cee_location_in_ranges(offset, ranges))
                cluster = cee_token_cluster_create(kCEETokenClusterTypeReference, 
                                                   reference);
            cee_list_free_full(ranges, cee_range_free);
            if (cluster)
                goto exit;
        }
        p = p->next;
    }
    
    symbols = cee_source_fregment_tree_symbols_search(prep_directive,
                                                      cee_source_symbol_matcher_by_buffer_offset,
                                                      CEE_INT_TO_POINTER(offset));
    if (symbols) {
        symbol = cee_list_nth_data(symbols, 0);
        cluster = cee_token_cluster_create(kCEETokenClusterTypeSymbol, 
                                           symbol);
        cee_list_free(symbols);
        goto exit;
    }
    
    symbols = cee_source_fregment_tree_symbols_search(statement,
                                                      cee_source_symbol_matcher_by_buffer_offset,
                                                      CEE_INT_TO_POINTER(offset));
    if (symbols) {
        symbol = cee_list_nth_data(symbols, 0);
        cluster = cee_token_cluster_create(kCEETokenClusterTypeSymbol, 
                                           symbol);
        cee_list_free(symbols);
        goto exit;
    }
    
exit:
    return cluster;
}
