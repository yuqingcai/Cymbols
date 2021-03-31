#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "cee_source_parser.h"

typedef struct _Attribute {
    cee_char* name;
    cee_char* value;
} Attribute;

static Attribute* attribute_create(const cee_char* name,
                                   const cee_char* value);
static void attribute_free(cee_pointer attribute);

static void symbol_tag_map_init(void);
static void reference_tag_map_init(void);
static void symbol_reference_map_init(void);
static void source_fregment_tokens_expand(CEESourceFregment* fregment,
                                          CEEList** tokens);
static void source_fregment_round_bracket_list_expand(CEESourceFregment* fregment,
                                                      CEEList** tokens);
static void source_fregment_symbols_dump(CEESourceFregment* fregment,
                                         CEETree* tree);
static void source_fregment_tree_symbols_search_recursive(CEESourceFregment* fregment,
                                                          CEECompareFunc matcher,
                                                          cee_pointer user_data,
                                                          CEEList** searched);
static CEEList* source_symbols_concat(CEEList* symbols0,
                                      CEEList* symbols1);
static CEEList* reference_tags_create(CEESourceSymbolReference* reference,
                                      CEESourceFregment* prep_directive,
                                      CEESourceFregment* statement,
                                      cee_pointer database,
                                      CEEList** caches);
CEESourceSymbol* symbol_referenced_search_in_cached(CEESourceSymbolReference* reference,
                                                    CEEList* caches);
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
static void source_fregment_tree_string_print(CEESourceFregment* fregment,
                                              cee_int indent);
static void source_fregment_symbols_parent_parse(CEESourceFregment* fregment);
static cee_char* global_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_char* class_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_char* union_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_char* namespace_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_char* implementation_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_char* interface_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_char* protocol_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_char* enumerator_block_scope_name_from_source_fregment(CEESourceFregment* fregment);
static cee_boolean is_anonymous_namespace_name(CEESourceFregment* fregment,
                                               const cee_char* name);
static cee_boolean is_anonymous_class_name(CEESourceFregment* fregment,
                                                 const cee_char* name);
static cee_boolean is_anonymous_union_name(CEESourceFregment* fregment,
                                                 const cee_char* name);
static cee_int symbol_relative_location_compare(const cee_pointer a,
                                                const cee_pointer b,
                                                cee_pointer user_data);

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
    
    symbol_tag_map[kCEESourceSymbolTypeUnknow]                              = kCEETagTypeIgnore;
    symbol_tag_map[kCEESourceSymbolTypePrepDirectiveDefine]                 = kCEETagTypePrepDirectiveDefine;
    symbol_tag_map[kCEESourceSymbolTypePrepDirectiveIncludePath]            = kCEETagTypePrepFilename;
    symbol_tag_map[kCEESourceSymbolTypePrepDirectiveParameter]              = kCEETagTypePrepDirectiveDefineParameter;
    symbol_tag_map[kCEESourceSymbolTypeFunctionDeclaration]                 = kCEETagTypeFunctionDeclaration;
    symbol_tag_map[kCEESourceSymbolTypeFunctionDefinition]                  = kCEETagTypeFunctionDefinition;
    symbol_tag_map[kCEESourceSymbolTypeVariableDeclaration]                 = kCEETagTypeVariable;
    symbol_tag_map[kCEESourceSymbolTypeCustomTypeDeclaration]               = kCEETagTypeTypeDeclaration;
    symbol_tag_map[kCEESourceSymbolTypePropertyDeclaration]                 = kCEETagTypeProperty;
    symbol_tag_map[kCEESourceSymbolTypeMessageDeclaration]                  = kCEETagTypeMessageDeclaration;
    symbol_tag_map[kCEESourceSymbolTypeMessageDefinition]                   = kCEETagTypeMessageDefinition;
    symbol_tag_map[kCEESourceSymbolTypeFunctionParameter]                   = kCEETagTypeFunctionParameter;
    symbol_tag_map[kCEESourceSymbolTypeMessageParameter]                    = kCEETagTypeMessageParameter;
    symbol_tag_map[kCEESourceSymbolTypeTypeDefine]                          = kCEETagTypeTypeDefine;
    symbol_tag_map[kCEESourceSymbolTypeClassDefinition]                     = kCEETagTypeClassDefinition;
    symbol_tag_map[kCEESourceSymbolTypeStructDefinition]                    = kCEETagTypeClassDefinition;
    symbol_tag_map[kCEESourceSymbolTypeEnumDefinition]                      = kCEETagTypeEnumDefinition;
    symbol_tag_map[kCEESourceSymbolTypeUnionDefinition]                     = kCEETagTypeUnionDefinition;
    symbol_tag_map[kCEESourceSymbolTypeEnumerator]                          = kCEETagTypeEnumerator;
    symbol_tag_map[kCEESourceSymbolTypeInterfaceDeclaration]                = kCEETagTypeInterface;
    symbol_tag_map[kCEESourceSymbolTypeInterfaceDefinition]                 = kCEETagTypeInterface;
    symbol_tag_map[kCEESourceSymbolTypeImplementationDefinition]            = kCEETagTypeImplementation;
    symbol_tag_map[kCEESourceSymbolTypeProtocolDeclaration]                 = kCEETagTypeProtocol;
    symbol_tag_map[kCEESourceSymbolTypeNamespaceDefinition]                 = kCEETagTypeNamespaceDefinition;
    symbol_tag_map[kCEESourceSymbolTypeLabel]                               = kCEETagTypeVariable;
    symbol_tag_map[kCEESourceSymbolTypeXMLTagStart]                         = kCEETagTypeXMLTag;
    symbol_tag_map[kCEESourceSymbolTypeXMLTagEnd]                           = kCEETagTypeXMLTag;
    symbol_tag_map[kCEESourceSymbolTypeXMLTagName]                          = kCEETagTypeXMLTag;
    symbol_tag_map[kCEESourceSymbolTypeXMLTagAttribute]                     = kCEETagTypeXMLAttribute;
    symbol_tag_map[kCEESourceSymbolTypeCSSSelector]                         = kCEETagTypeCSSSelector;
}

static void reference_tag_map_init(void)
{
    for (cee_int i = 0; i < kCEESourceReferenceTypeMax; i ++)
        reference_tag_map[i] = kCEETagTypeIgnore;
    
    reference_tag_map[kCEESourceReferenceTypeUnknow]                        = kCEETagTypeIgnore;
    reference_tag_map[kCEESourceReferenceTypeReplacement]                   = kCEETagTypePrepDirectiveReference;
    reference_tag_map[kCEESourceReferenceTypeFunction]                      = kCEETagTypeFunctionReference;
    reference_tag_map[kCEESourceReferenceTypeCustomTypeDeclaration]         = kCEETagTypeTypeReference;
    reference_tag_map[kCEESourceReferenceTypeVariable]                      = kCEETagTypeVariableReference;
    reference_tag_map[kCEESourceReferenceTypeMember]                        = kCEETagTypeMemberReference;
    reference_tag_map[kCEESourceReferenceTypeTypeDefine]                    = kCEETagTypeTypeDefineReference;
    reference_tag_map[kCEESourceReferenceTypeEnumerator]                    = kCEETagTypeEnumeratorReference;
    reference_tag_map[kCEESourceReferenceTypeNamespace]                     = kCEETagTypeNamespaceReference;
    reference_tag_map[kCEESourceReferenceTypeLabel]                         = kCEETagTypeLabelReference;
}

static void symbol_reference_map_init(void)
{
    for (cee_int i = 0; i < kCEESourceSymbolTypeMax; i ++)
        symbol_reference_map[i] = kCEESourceReferenceTypeUnknow;
    
    symbol_reference_map[kCEESourceSymbolTypePrepDirectiveDefine]           = kCEESourceReferenceTypeReplacement;
    symbol_reference_map[kCEESourceSymbolTypePrepDirectiveParameter]        = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeFunctionDeclaration]           = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeFunctionDefinition]            = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeLabel]                         = kCEESourceReferenceTypeLabel;
    symbol_reference_map[kCEESourceSymbolTypeVariableDeclaration]           = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeCustomTypeDeclaration]         = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypePropertyDeclaration]           = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeMessageDeclaration]            = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeMessageDefinition]             = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeFunctionParameter]             = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeMessageParameter]              = kCEESourceReferenceTypeVariable;
    symbol_reference_map[kCEESourceSymbolTypeTypeDefine]                    = kCEESourceReferenceTypeTypeDefine;
    symbol_reference_map[kCEESourceSymbolTypeClassDefinition]               = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeStructDefinition]              = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeEnumDefinition]                = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeUnionDefinition]               = kCEESourceReferenceTypeCustomTypeDeclaration;
    symbol_reference_map[kCEESourceSymbolTypeEnumerator]                    = kCEESourceReferenceTypeEnumerator;
    symbol_reference_map[kCEESourceSymbolTypeNamespaceDefinition]           = kCEESourceReferenceTypeNamespace;
    symbol_reference_map[kCEESourceSymbolTypeInterfaceDeclaration]          = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeInterfaceDefinition]           = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeImplementationDefinition]      = kCEESourceReferenceTypeFunction;
    symbol_reference_map[kCEESourceSymbolTypeProtocolDeclaration]           = kCEESourceReferenceTypeFunction;
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
                                    const cee_char* filepath,
                                    const cee_char* subject,
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
                                       const cee_char* filepath,
                                       const cee_char* subject,
                                       CEESourceTokenMap* source_token_map,
                                       CEESourceFregment* prep_directive,
                                       CEESourceFregment* statement,
                                       CEERange range,
                                       CEEList** references)
{
    if (!parser_ref || !parser_ref->reference_parse)
        return FALSE;
    
    cee_boolean ret = parser_ref->reference_parse(parser_ref,
                                                  filepath,
                                                  subject,
                                                  source_token_map,
                                                  prep_directive,
                                                  statement,
                                                  range,
                                                  references);
    
    if (*references)
        *references = cee_list_sort(*references,
                                    cee_source_symbol_reference_location_compare);
    
    return ret;
}

int cee_source_fregment_count = 0;
CEESourceFregment* cee_source_fregment_create(CEESourceFregmentType type,
                                              const cee_char* filepath,
                                              const cee_char* subject,
                                              const cee_char* filetype)
{
    CEESourceFregment* fregment = cee_malloc0(sizeof(CEESourceFregment));
    fregment->type[type] = 1;
    fregment->filepath_ref = filepath;
    fregment->subject_ref = subject;
    fregment->filetype = cee_strdup(filetype);
    
    cee_source_fregment_count ++;
    
    return fregment;
}

void cee_source_fregment_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEESourceFregment* fregment = (CEESourceFregment*)data;
    
    cee_list_free(fregment->tokens_ref_last);
    cee_list_free(fregment->symbols);
    cee_list_free_full(fregment->children, cee_source_fregment_free);
    
    if (fregment->filetype)
        cee_free(fregment->filetype);
    
    cee_list_free_full(fregment->attributes, attribute_free);
    
    free(fregment);
    
    cee_source_fregment_count --;
}

void cee_source_fregment_free_full(cee_pointer data)
{
    if (!data)
        return;
    
    CEESourceFregment* fregment = (CEESourceFregment*)data;
    
    cee_list_free(fregment->tokens_ref_last);
    cee_list_free_full(fregment->symbols, cee_source_symbol_free);
    cee_list_free_full(fregment->children, cee_source_fregment_free_full);
    
    if (fregment->filetype)
        cee_free(fregment->filetype);
    
    cee_list_free_full(fregment->attributes, attribute_free);
    
    fregment->symbols = NULL;
    fregment->children = NULL;
    
    free(fregment);
    
    cee_source_fregment_count --;
}

void cee_source_fregment_attribute_set(CEESourceFregment* fregment,
                                       const cee_char* name,
                                       const cee_char* value)
{
    if (!fregment)
        return;
    
    CEEList* p = fregment->attributes;
    cee_boolean found = FALSE;
    Attribute* attribute = NULL;
    while (p) {
        attribute = p->data;
        if (!cee_strcmp(attribute->name, name, FALSE)) {
            found = TRUE;
            break;
        }
        p = p->next;
    }
    
    if (found) {
        if (attribute->value)
            cee_free(attribute->value);
        
        attribute->value = cee_strdup(value);
    }
    else {
        attribute = attribute_create(name, value);
        fregment->attributes = cee_list_prepend(fregment->attributes, attribute);
    }
}

const cee_char* cee_source_fregment_attribute_get(CEESourceFregment* fregment,
                                                  const cee_char* name)
{
    if (!fregment)
        return NULL;
    
    CEEList* p = fregment->attributes;
    Attribute* attribute = NULL;
    while (p) {
        attribute = p->data;
        if (!cee_strcmp(attribute->name, name, FALSE))
            return attribute->value;
        p = p->next;
    }
    return NULL;
}

static Attribute* attribute_create(const cee_char* name,
                                   const cee_char* value)
{
    Attribute* attribute = cee_malloc0(sizeof(Attribute));
    attribute->name = cee_strdup(name);
    attribute->value = cee_strdup(value);
    return attribute;
}

static void attribute_free(cee_pointer data)
{
    if (!data)
        return;
    
    Attribute* attribute = (Attribute*)data;
    
    if (attribute->name)
        cee_free(attribute->name);
    
    if (attribute->value)
        cee_free(attribute->value);
    
    cee_free(attribute);
}

cee_int cee_source_fregment_count_get(void)
{
    return cee_source_fregment_count;
}

/**
 * source fregment
 */
CEESourceFregment* cee_source_fregment_attach(CEESourceFregment* sibling,
                                              CEESourceFregmentType type,
                                              const cee_char* filepath,
                                              const cee_char* subject,
                                              const cee_char* filetype)
{
    if (!sibling || !sibling->parent)
        return NULL;
    
    CEESourceFregment* attached = cee_source_fregment_create(type,
                                                             filepath,
                                                             subject,
                                                             filetype);
    attached->parent = sibling->parent;
    SOURCE_FREGMENT_CHILD_APPEND(sibling->parent, attached);
    return attached;
}

CEESourceFregment* cee_source_fregment_sub_attach(CEESourceFregment* main,
                                                  CEESourceFregmentType type,
                                                  const cee_char* filepath,
                                                  const cee_char* subject,
                                                  const cee_char* filetype)
{
    if (!main)
        return NULL;
    
    CEESourceFregment* attached = cee_source_fregment_create(type,
                                                             filepath,
                                                             subject,
                                                             filetype);
    attached->parent = main;
    SOURCE_FREGMENT_CHILD_APPEND(main, attached);
    return attached;
}

CEESourceFregment* cee_source_fregment_push(CEESourceFregment* main,
                                            CEESourceFregmentType type,
                                            const cee_char* filepath,
                                            const cee_char* subject,
                                            const cee_char* filetype)
{
    if (!main)
        return NULL;
    
    CEESourceFregment* attached = cee_source_fregment_sub_attach(main,
                                                                 type,
                                                                 filepath,
                                                                 subject,
                                                                 filetype);
    attached = cee_source_fregment_sub_attach(attached,
                                              kCEESourceFregmentTypeStatement,
                                              filepath,
                                              subject,
                                              filetype);
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
    
    fregment->type[type] = 1;
}

void cee_source_fregment_type_set_exclusive(CEESourceFregment* fregment,
                                            CEESourceFregmentType type)
{
    if (!fregment)
        return;
       
    memset(fregment->type, 0, sizeof(fregment->type));
    fregment->type[type] = 1;
}

void cee_source_fregment_type_clear(CEESourceFregment* fregment,
                                    CEESourceFregmentType type)
{
    if (!fregment)
        return;
    
    fregment->type[type] = 0;
}

cee_boolean cee_source_fregment_type_is_one_of(CEESourceFregment* fregment,
                                               CEESourceFregmentType* types)
{
    CEESourceFregmentType* p = types;
    int i = 0;
    
    while (TRUE) {
        if (p[i] == -1)
            break;
        else if (fregment->type[p[i]] != 0)
            return TRUE;
        i ++;
    }
    return FALSE;
}

cee_boolean cee_source_fregment_type_is(CEESourceFregment* fregment,
                                        CEESourceFregmentType type)
{
    return fregment->type[type] != 0;
}

cee_boolean cee_source_fregment_parent_type_is(CEESourceFregment* fregment,
                                               CEESourceFregmentType type)
{
    CEESourceFregment* parent = fregment->parent;
    if (parent && cee_source_fregment_type_is(parent, type))
        return TRUE;
    return FALSE;
}

cee_boolean cee_source_fregment_grandfather_type_is(CEESourceFregment* fregment,
                                                    CEESourceFregmentType type)
{
    CEESourceFregment* parent = fregment->parent;
    if (parent) {
        parent = parent->parent;
        if (parent && cee_source_fregment_type_is(parent, type))
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
    return tokens;
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
    
    q = fregment->tokens_ref_last;
    if (fregment->tokens_ref == p) {
        fregment->tokens_ref_last = fregment->tokens_ref = NULL;
    }
    else {
        prev = TOKEN_PREV(p);
        if (prev)
            TOKEN_NEXT_SET(prev, NULL);
        TOKEN_PREV_SET(p, NULL);
        fregment->tokens_ref_last = prev;
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
            cee_source_fregment_type_is(child, kCEESourceFregmentTypeCurlyBracketList) ||
            cee_source_fregment_type_is(child, kCEESourceFregmentTypeXMLTagList)) {
        
            q =  SOURCE_FREGMENT_CHILDREN_FIRST(child);
            while (q) {
                grandson = q->data;
                
                if (cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeFunctionDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeFunctionDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeEnumerators) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypePrepDirective)  ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeXMLTagStart) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeLabelDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeEnumeratorBlock)) {
                    
                    s = grandson->symbols;
                    while (s) {
                        
                        symbol = s->data;
                        
                        if (symbol->type == kCEESourceSymbolTypeFunctionDefinition ||
                            symbol->type == kCEESourceSymbolTypeMessageDefinition ||
                            symbol->type == kCEESourceSymbolTypeFunctionDeclaration ||
                            symbol->type == kCEESourceSymbolTypeOperatorOverloadDefinition ||
                            symbol->type == kCEESourceSymbolTypeOperatorOverloadDeclaration ||
                            symbol->type == kCEESourceSymbolTypeVariableDeclaration ||
                            symbol->type == kCEESourceSymbolTypeCustomTypeDeclaration ||
                            symbol->type == kCEESourceSymbolTypePropertyDeclaration ||
                            symbol->type == kCEESourceSymbolTypeMessageDeclaration ||
                            symbol->type == kCEESourceSymbolTypeTypeDefine ||
                            symbol->type == kCEESourceSymbolTypeEnumerator ||
                            symbol->type == kCEESourceSymbolTypePrepDirectiveDefine ||
                            symbol->type == kCEESourceSymbolTypePrepDirectiveInclude ||
                            symbol->type == kCEESourceSymbolTypeXMLTagName ||
                            symbol->type == kCEESourceSymbolTypeImport ||
                            symbol->type == kCEESourceSymbolTypePackage ||
                            symbol->type == kCEESourceSymbolTypeUsingDeclaration ||
                            symbol->type == kCEESourceSymbolTypeLabel) {
                            
                            leaf = cee_tree_create();
                            leaf->data = symbol;
                            tree->children = cee_list_prepend(tree->children, leaf);
                            
                        }
                        s = s->next;
                    }
                }
                
                /** recursive dump */
                if (cee_source_fregment_type_is(grandson, kCEESourceFregmentTypePrepDirectiveCondition) ||
                    //cee_source_fregment_type_is(grandson, kCEESourceFregmentTypePrepDirectiveBranch) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeTemplateDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeNamespaceDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeClassDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeStructDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeEnumDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeEnumeratorBlock) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeUnionDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeInterfaceDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeInterfaceDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeImplementationDefinition) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeProtocolDeclaration) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeVariableBlock) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeIdentifierBlock) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeXMLTagStart) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeXMLTagStart) ||
                    cee_source_fregment_type_is(grandson, kCEESourceFregmentTypeCSSBlock)) {
                                        
                    s = grandson->symbols;
                    while (s) {
                        symbol = s->data;
                        
                        if (symbol->type == kCEESourceSymbolTypeTemplateDeclaration ||
                            symbol->type == kCEESourceSymbolTypeNamespaceDefinition ||
                            symbol->type == kCEESourceSymbolTypeClassDefinition ||
                            symbol->type == kCEESourceSymbolTypeStructDefinition ||
                            symbol->type == kCEESourceSymbolTypeEnumDefinition ||
                            symbol->type == kCEESourceSymbolTypeUnionDefinition ||
                            symbol->type == kCEESourceSymbolTypeInterfaceDeclaration ||
                            symbol->type == kCEESourceSymbolTypeInterfaceDefinition ||
                            symbol->type == kCEESourceSymbolTypeImplementationDefinition ||
                            symbol->type == kCEESourceSymbolTypeProtocolDeclaration ||
                            symbol->type == kCEESourceSymbolTypeExternBlock ||
                            symbol->type == kCEESourceSymbolTypeCSSSelector) {
                            
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

CEEList* cee_source_fregment_symbol_list_type_filter(CEEList* list,
                                                     CEESourceSymbolType* types)
{
    CEEList* p = NULL;
    CEEList* r = NULL;
    CEEList* s = NULL;
    CEESourceSymbolType* q = NULL;
    CEESourceSymbol* symbol = NULL;
    
    p = list;
    while (p) {
        symbol = p->data;
        q = types;
        while (*q) {
            if (symbol->type == *q) {
                r = cee_list_prepend(r, p);
                break;
            }
            q ++;
        }
        p = p->next;
    }
    
    if (r) {
        p = r;
        while (p) {
            s = p->data;
            list = cee_list_remove_link(list, s);
            cee_list_free(s);
            p = p->next;
        }
        cee_list_free(r);
    }
    return list;
}

CEEList* cee_source_fregment_symbol_tags_create(CEESourceFregment* fregment)
{
    CEEList* tags = NULL;
    CEETag* tag = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEETagType tag_type = kCEETagTypeIgnore;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        if (symbol) {
            q = symbol->ranges;
            while (q) {
                tag_type = symbol_tag_map[symbol->type];
                if (tag_type != kCEETagTypeIgnore) {
                    tag = cee_tag_create(tag_type, *((CEERange*)q->data));
                    tags = cee_list_prepend(tags, tag);
                }
                q = q->next;
            }
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
    if (!fregment || !fregment->subject_ref)
        return;
    
    cee_char* str = cee_string_from_tokens(fregment->subject_ref,
                                           SOURCE_FREGMENT_TOKENS_FIRST(fregment), 
                                           kCEETokenStringOptionDefault);
    if (str) {
        fprintf(stdout, "%s", str);
        cee_free(str);
    }
    fprintf(stdout, "\n");
}

void cee_source_fregment_tree_string_print(CEESourceFregment* fregment)
{
    int i = 0;
    source_fregment_tree_string_print(fregment, i);
}

static void source_fregment_tree_string_print(CEESourceFregment* fregment,
                                              cee_int indent)
{
    for (int i = 0; i < indent; i ++)
        fprintf(stdout, " ");
    
    cee_source_fregment_string_print(fregment);
    
    indent ++;
    
    CEEList* p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        CEESourceFregment* child = p->data;
        source_fregment_tree_string_print(child, indent);
        p = SOURCE_FREGMENT_NEXT(p);
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
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        if (cee_source_symbol_is_block_type(symbol))
            symbol->fregment_range = range;
        p = p->next;
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
            !cee_token_is_identifier(p, kCEETokenID_NEW_LINE) &&
            !cee_token_is_ignore(p)) {
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

CEESourceFregment* cee_source_fregment_level_backtrack(CEESourceFregment* fregment)
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
            if (cee_source_fregment_type_is(fregment, kCEESourceFregmentTypeComment)) {
                if (!indexes[kCEESourceFregmentIndexComment])
                    indexes[kCEESourceFregmentIndexComment] = fregment;
            }
            else if (cee_source_fregment_type_is(fregment, kCEESourceFregmentTypePrepDirective)) {
                if (!indexes[kCEESourceFregmentIndexPrepDirective])
                    indexes[kCEESourceFregmentIndexPrepDirective] = fregment;
            }
            else {
                fregment = cee_source_fregment_level_backtrack(fregment);
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
        if ((symbol->name && !strcmp(symbol->name, name)) ||
            (symbol->alias && !strcmp(symbol->alias, name)))
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

CEEList*   cee_source_fregment_tree_symbols_search(CEESourceFregment* fregment,
                                                 CEECompareFunc matcher,
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
                                                          CEECompareFunc matcher,
                                                          cee_pointer user_data,
                                                          CEEList** searched)
{
    if (!fregment)
        return;
    
    CEEList* p = fregment->symbols;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        if (!matcher(symbol, user_data))
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

CEEList* cee_source_tags_create(CEESourceParserRef parser_ref,
                                CEESourceTokenMap* token_map,
                                CEESourceFregment* prep_directive,
                                CEESourceFregment* statement,
                                cee_pointer database,
                                CEERange range,
                                CEEList* references,
                                CEEList** cache)
{
    //cee_ulong m0 = 0;
    //cee_ulong m1 = 0;
    
    CEEList* tags = NULL;
    CEEList* p = NULL;
    CEESourceFregment* indexes[kCEESourceFregmentIndexMax];
    memset(indexes, 0, sizeof(CEESourceFregment*)*kCEESourceFregmentIndexMax);
    cee_source_fregment_indexes_in_range(token_map, range, indexes);
    
    //m0 = cee_timestamp_ms();
    /** comment tags */
    if (indexes[kCEESourceFregmentIndexComment])
        tags = comment_tags_create(parser_ref, 
                                   indexes[kCEESourceFregmentIndexComment], 
                                   range);
    //m1 = cee_timestamp_ms();
    //fprintf(stdout, "comment tags in range cost: %lu ms\n", m1 - m0);
        
    //m0 = cee_timestamp_ms();
    /** syntax tags */
    for (cee_int i = kCEESourceFregmentIndexPrepDirective; i < kCEESourceFregmentIndexMax; i ++) {
        if (!indexes[i])
            continue;
        
        tags = cee_list_concat(tags, syntax_tags_create(parser_ref, indexes[i], range));
    }
    //m1 = cee_timestamp_ms();
    //fprintf(stdout, "syntax tags in range cost: %lu ms\n", m1 - m0);
    
    //m0 = cee_timestamp_ms();
    /** reference tags */
    p = references;
    while (p) {
        tags = cee_list_concat(tags, reference_tags_create(p->data,
                                                           prep_directive,
                                                           statement,
                                                           database,
                                                           cache));
        p = p->next;
    }
    //m1 = cee_timestamp_ms();
    //fprintf(stdout, "reference tags in range cost: %lu ms\n", m1 - m0);
    
    tags = cee_list_sort(tags, cee_tag_compare);
    return tags;
}

static CEEList* comment_tags_create(CEESourceParserRef parser_ref,
                                    CEESourceFregment* fregment,
                                    CEERange range)
{
    CEEList* tags = NULL;
    CEEList* p = fregment->node_ref;
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
    p = fregment->node_ref;
    while (p) {
        fregment = p->data;
        if (!syntax_tags_create_recursive(parser_ref, fregment, range, &tags))
            break;
        p = SOURCE_FREGMENT_NEXT(p);
    }
        
    while (fregment && !cee_source_fregment_over_range(fregment, range)) {
        fregment = cee_source_fregment_grandfather_get(fregment);
        if (!fregment || !fregment->node_ref ||
            !SOURCE_FREGMENT_HAS_NEXT(fregment->node_ref))
            break;
        
        p = SOURCE_FREGMENT_NEXT(fregment->node_ref);
        while (p) {
            fregment = p->data;
            if (!syntax_tags_create_recursive(parser_ref, fregment, range, &tags))
                break;
            p = SOURCE_FREGMENT_NEXT(p);
        }
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
                if (parser_ref->token_type_matcher(token, kCEETokenTypeKeyword))
                    tag_type = kCEETagTypeKeyword;
                else if (parser_ref->token_type_matcher(token, kCEETokenTypePrepDirective))
                    tag_type = kCEETagTypePrepDirective;
                else if (parser_ref->token_type_matcher(token, kCEETokenTypePunctuation))
                    tag_type = kCEETagTypePunctuation;
                else if (parser_ref->token_type_matcher(token, kCEETokenTypeASMDirective))
                    tag_type = kCEETagTypeASMDirective;
            }
            
            if (tag_type == kCEETagTypeIgnore) {
                if (token->identifier == kCEETokenID_CONSTANT)
                    tag_type = kCEETagTypeConstant;
                else if (token->identifier == kCEETokenID_LINE_COMMENT)
                    tag_type = kCEETagTypeComment;
                else if (token->identifier == kCEETokenID_LINES_COMMENT)
                    tag_type = kCEETagTypeComment;
                else if (token->identifier == kCEETokenID_HTML_COMMENT)
                    tag_type = kCEETagTypeComment;
                else if (token->identifier == kCEETokenID_LITERAL)
                    tag_type = kCEETagTypeLiteral;
                else if (token->identifier == kCEETokenID_MULTI_LITERAL)
                    tag_type = kCEETagTypeLiteral;
                else if (token->identifier == kCEETokenID_CHARACTER)
                    tag_type = kCEETagTypeCharacter;
                else if (token->identifier == kCEETokenID_CSS_PROPERTY)
                    tag_type = kCEETagTypeCSSProperty;
                else if (token->identifier == kCEETokenID_CSS_FUNCTION)
                    tag_type = kCEETagTypeFunctionReference;
                else if (token->identifier == kCEETokenID_CSS_FUNCTION)
                    tag_type = kCEETagTypeFunctionReference;
                else if (token->identifier == kCEETokenID_ANNOTATION)
                    tag_type = kCEETagTypeAnnotation;
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
                                      cee_pointer database,
                                      CEEList** caches)
{
    CEEList* tags = NULL;
    CEERange* range = NULL;
    CEETag* tag = NULL;
    CEEList* p = NULL;
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    CEESourceSymbolCache* cache = NULL;
    CEERange reference_range =
        cee_range_consistent_from_discrete(reference->ranges);
    
    symbol = symbol_referenced_search_in_cached(reference, *caches);
    if (!symbol) {
        if (reference->type == kCEESourceReferenceTypeUnknow) {
            symbols = cee_source_symbols_search_by_reference(reference,
                                                             prep_directive,
                                                             statement,
                                                             database);
            if (!symbols)
                goto exit;
            
            symbol = cee_source_symbol_copy(cee_list_nth_data(symbols, 0));
            cee_list_free_full(symbols, cee_source_symbol_free);
            
            cache = cee_source_symbol_cached(*caches, symbol);
            if (cache) {
                cee_source_symbol_cache_location(cache, reference_range.location);
                cee_source_symbol_free(symbol);
            }
            else {
                cache = cee_source_symbol_cache_create(symbol, reference_range.location);
                *caches = cee_list_prepend(*caches, cache);
            }
            symbol = cache->symbol;
        }
    }
    
    if (symbol) {
        reference->type = symbol_reference_map[symbol->type];
        p = reference->ranges;
        while (p) {
            range = p->data;
            tag = cee_tag_create(reference_tag_map[reference->type], *range);
            tags = cee_list_prepend(tags, tag);
            p = p->next;
        }
    }
        
exit:
    return tags;
}

CEESourceSymbol* symbol_referenced_search_in_cached(CEESourceSymbolReference* reference,
                                                    CEEList* caches)
{
    CEEList* p = caches;
    const cee_char* name = reference->name;
    cee_long location = cee_range_consistent_from_discrete(reference->ranges).location;
    
    while (p) {
        CEESourceSymbolCache* cache = p->data;
        if (cee_source_symbol_cache_name_hit(cache, name) &&
            cee_source_symbol_cache_location_hit(cache, location))
            return cache->symbol;
        p = p->next;
    }
    return NULL;
}

CEEList* cee_source_symbols_search_by_symbol(CEESourceSymbol* symbol,
                                             CEESourceFregment* prep_directive,
                                             CEESourceFregment* statement,
                                             cee_pointer database)
{
    if (!symbol)
        return NULL;
    
    CEEList* symbols0 = NULL;
    CEEList* symbols1 = NULL;
    CEEList* local =  NULL;
    CEEList* global = NULL;
    CEERange range = cee_range_consistent_from_discrete(symbol->ranges);
    
    symbols0 = cee_source_fregment_tree_symbols_search(prep_directive,
                                                       cee_source_symbol_matcher_by_name,
                                                       (cee_pointer)symbol->name);
    symbols1 = cee_source_fregment_tree_symbols_search(statement,
                                                       cee_source_symbol_matcher_by_name,
                                                       (cee_pointer)symbol->name);
    if (symbols0 && symbols1)
        symbols0 = cee_list_concat(symbols0, symbols1);
    else if (symbols1)
        symbols0 = symbols1;
    
    local = cee_list_copy_deep(symbols0, cee_source_symbol_copy_func, NULL);
    cee_list_free(symbols0);
    
    if (local) {
        local = cee_list_sort(local, cee_source_symbol_location_compare);
        CEEList* p = cee_list_find_custom(local,
                                          CEE_LONG_TO_POINTER(range.location),
                                          cee_source_symbol_matcher_by_buffer_offset);
        if (p) {
            local = cee_list_remove_link(local, p);
            local = cee_list_concat(p, local);
        }
    }
    
    global = cee_database_symbols_search_by_name(database, symbol->name);
    if (!global)
        global = cee_database_symbols_search_by_alias(database, symbol->name);
    
    return source_symbols_concat(local, global);
}

CEEList* cee_source_symbols_search_by_reference(CEESourceSymbolReference* reference,
                                                CEESourceFregment* prep_directive,
                                                CEESourceFregment* statement,
                                                cee_pointer database)
{
    CEEList* local = NULL;
    CEEList* global = NULL;
    CEESourceSymbol* symbol_in_scope = NULL;
    CEESourceParserRef parser_ref = reference->parser_ref;
    if (parser_ref->symbol_search_in_scope)
        parser_ref->symbol_search_in_scope(reference->parser_ref,
                                           reference,
                                           prep_directive,
                                           statement,
                                           &symbol_in_scope);
    if (symbol_in_scope)
        local = cee_list_prepend(local, symbol_in_scope);
    
    global = cee_database_symbols_search_by_name(database, reference->name);
    if (!global)
        global = cee_database_symbols_search_by_alias(database, reference->name);
    
    return source_symbols_concat(local, global);
}

CEESourceFregment* cee_source_fregment_from_reference_get(CEESourceSymbolReference* reference)
{
    if (!reference->tokens_ref)
        return NULL;
    
    CEEList* tokens_ref = reference->tokens_ref;
    CEEToken* token = TOKEN_FIRST(tokens_ref)->data;
    CEESourceFregment* fregment = token->fregment_ref;
    return fregment;
}

/**
 * remove symbols(appeared in symbols0 and symbols1) from symbols1
 * then join symbols0 and symbol1 together
 */
static CEEList* source_symbols_concat(CEEList* symbols0,
                                      CEEList* symbols1)
{
    CEEList* validated = NULL;
    CEEList* duplicated = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    
    if (symbols0 && symbols1) {
        
        p = symbols0;
        while (p) {
            CEESourceSymbol* symbol0 = p->data;
            
            q = symbols1;
            while (q) {
                CEESourceSymbol* symbol1 = q->data;
                if (cee_source_symbols_are_equal(symbol0, symbol1) &&
                    !cee_list_find(duplicated, q))
                    duplicated = cee_list_prepend(duplicated, q);
                
                q = q->next;
            }
            
            p = p->next;
        }
        
        p = duplicated;
        while (p) {
            CEEList* q = p->data;
            symbols1 = cee_list_remove_link(symbols1, q);
            cee_list_free_full(q, cee_source_symbol_free);
            if (!symbols1)
                break;
            p = p->next;
        }
        cee_list_free(duplicated);
        
        if (symbols0 && symbols1)
            validated = cee_list_concat(symbols0, symbols1);
        else if (symbols0)
            validated = symbols0;
    }
    else if (symbols0)
        validated = symbols0;
    else if (symbols1)
        validated = symbols1;
    
    return validated;
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
    CEEList* p = references;
    
    while (p) {
        reference = p->data;
        if (cee_location_in_ranges(offset, reference->ranges) ||
            cee_location_followed_ranges(offset, reference->ranges))
            cluster = cee_token_cluster_create(kCEETokenClusterTypeReference, reference);
        if (cluster)
            goto exit;
        p = p->next;
    }
    
    symbols = cee_source_fregment_tree_symbols_search(prep_directive,
                                                      cee_source_symbol_matcher_by_buffer_offset,
                                                      CEE_INT_TO_POINTER(offset));
    if (symbols) {
        symbol = cee_list_nth_data(symbols, 0);
        cluster = cee_token_cluster_create(kCEETokenClusterTypeSymbol, symbol);
        cee_list_free(symbols);
        goto exit;
    }
    
    symbols = cee_source_fregment_tree_symbols_search(statement,
                                                      cee_source_symbol_matcher_by_buffer_offset,
                                                      CEE_INT_TO_POINTER(offset));
    if (symbols) {
        symbol = cee_list_nth_data(symbols, 0);
        cluster = cee_token_cluster_create(kCEETokenClusterTypeSymbol, symbol);
        cee_list_free(symbols);
        goto exit;
    }
    
exit:
    return cluster;
}

void cee_source_fregment_tree_symbols_parent_parse(CEESourceFregment* fregment)
{
    if (!fregment)
        return;
    
    source_fregment_symbols_parent_parse(fregment);
    
    CEEList* p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        fregment = p->data;
        cee_source_fregment_tree_symbols_parent_parse(fregment);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

static void source_fregment_symbols_parent_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    CEESourceFregment *grandfather = NULL;
    cee_char* parent = NULL;
    
    grandfather = cee_source_fregment_grandfather_get(fregment);
    if (!grandfather)
        return;
    
    if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeTemplateDeclaration) || 
        cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeVariableBlock) || 
        cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeIdentifierBlock)) {
        
        while (1) {
            grandfather = cee_source_fregment_grandfather_get(grandfather);
            if (!grandfather)
                break;
            
            if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeTemplateDeclaration) || 
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeVariableBlock) || 
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeIdentifierBlock))
                continue;
            
            break;
        }
    }
    
    if (!grandfather)
        return;
    
    if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeRoot))
        parent = global_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition))
        parent = class_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeStructDefinition))
        parent = class_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeUnionDefinition))
        parent = union_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeNamespaceDefinition))
        parent = namespace_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeImplementationDefinition))
        parent = implementation_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDeclaration))
        parent = interface_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDefinition))
        parent = interface_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeProtocolDeclaration))
        parent = protocol_scope_name_from_source_fregment(grandfather);
    else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumeratorBlock))
        parent = enumerator_block_scope_name_from_source_fregment(grandfather);
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeFunctionDeclaration || 
            symbol->type == kCEESourceSymbolTypeFunctionDefinition || 
            symbol->type == kCEESourceSymbolTypeVariableDeclaration || 
            symbol->type == kCEESourceSymbolTypeCustomTypeDeclaration ||
            symbol->type == kCEESourceSymbolTypePropertyDeclaration ||
            symbol->type == kCEESourceSymbolTypeOperatorOverloadDeclaration||
            symbol->type == kCEESourceSymbolTypeOperatorOverloadDefinition || 
            symbol->type == kCEESourceSymbolTypeMessageDeclaration || 
            symbol->type == kCEESourceSymbolTypeMessageDefinition || 
            symbol->type == kCEESourceSymbolTypeTypeDefine || 
            symbol->type == kCEESourceSymbolTypeClassDefinition ||
            symbol->type == kCEESourceSymbolTypeStructDefinition ||
            symbol->type == kCEESourceSymbolTypeEnumDefinition || 
            symbol->type == kCEESourceSymbolTypeUnionDefinition || 
            symbol->type == kCEESourceSymbolTypeNamespaceDefinition || 
            symbol->type == kCEESourceSymbolTypeInterfaceDeclaration ||
            symbol->type == kCEESourceSymbolTypeInterfaceDefinition ||
            symbol->type == kCEESourceSymbolTypeImplementationDefinition ||
            symbol->type == kCEESourceSymbolTypeProtocolDeclaration) {
            symbol->parent = cee_strdup(parent);
        }
        
        p = p->next;
    }
    
    if (parent)
        cee_free(parent);
    
}

static cee_char* global_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    return cee_strdup("global");
}

static cee_char* class_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    cee_char* names = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeClassDefinition ||
            symbol->type == kCEESourceSymbolTypeStructDefinition ||
            symbol->type == kCEESourceSymbolTypeTypeDefine) {
            if (!is_anonymous_class_name(fregment, symbol->name)) {
                if (!names)
                    names = cee_strdup(",");
                cee_strconcat0(&names, symbol->name, ",", NULL);
            }

        }
        p = p->next;
    }
    
    return names;
}

static cee_char* union_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    cee_char* names = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeUnionDefinition ||
            symbol->type == kCEESourceSymbolTypeTypeDefine) {
            if (!is_anonymous_union_name(fregment, symbol->name)) {
                if (!names)
                    names = cee_strdup(",");
                cee_strconcat0(&names, symbol->name, ",", NULL);
            }
        }
        
        p = p->next;
    }
    
    return names;
}

static cee_char* namespace_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    cee_char* names = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeNamespaceDefinition) {
            if (!is_anonymous_namespace_name(fregment, symbol->name)) {
                if (!names)
                    names = cee_strdup(",");
                cee_strconcat0(&names, symbol->name, ",", NULL);
            }
        }
        p = p->next;
    }
    return names;
}

static cee_char* implementation_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeImplementationDefinition)
            return cee_strdup(symbol->name);
        
        p = p->next;
    }
    return NULL;
}

static cee_char* interface_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeInterfaceDeclaration ||
            symbol->type == kCEESourceSymbolTypeInterfaceDefinition)
            return cee_strdup(symbol->name);
        
        p = p->next;
    }
    return NULL;
}

static cee_char* protocol_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeProtocolDeclaration)
            return cee_strdup(symbol->name);
        
        p = p->next;
    }
    return NULL;
}

static cee_char* enumerator_block_scope_name_from_source_fregment(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEESourceSymbol* symbol = NULL;
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeEnumerator)
            return cee_strdup(symbol->name);
        
        p = p->next;
    }
    return NULL;
}

static cee_boolean is_anonymous_namespace_name(CEESourceFregment* fregment,
                                                const cee_char* name)
{
    return !cee_strcmp(name, "namespace", FALSE);
}

static cee_boolean is_anonymous_class_name(CEESourceFregment* fregment,
                                           const cee_char* name)
{
    return !cee_strcmp(name, "struct", FALSE) || 
            !cee_strcmp(name, "class", FALSE);
}

static cee_boolean is_anonymous_union_name(CEESourceFregment* fregment,
                                           const cee_char* name)
{
    return !cee_strcmp(name, "union", FALSE);
}
