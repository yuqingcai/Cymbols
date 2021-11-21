#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_javascript.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef enum _JSFunctionDefinitionTranslateState {
    kJSFunctionDefinitionTranslateStateInit = 0,
    kJSFunctionDefinitionTranslateStateKeyword,
    kJSFunctionDefinitionTranslateStateIdentifier,
    kJSFunctionDefinitionTranslateStateParameterList,
    kJSFunctionDefinitionTranslateStateParameterListEnd,
    kJSFunctionDefinitionTranslateStateConfirm,
    kJSFunctionDefinitionTranslateStateError,
    kJSFunctionDefinitionTranslateStateMax,
} JSFunctionDefinitionTranslateState;

typedef enum _JSFunctionParametersDeclarationState {
    kJSFunctionParametersDeclarationTranslateStateInit = 0,
    kJSFunctionParametersDeclarationTranslateStateIdentifier,
    kJSFunctionParametersDeclarationTranslateStateConfirm,
    kJSFunctionParametersDeclarationTranslateStateError,
    kJSFunctionParametersDeclarationTranslateStateMax,
} JSFunctionParametersDeclarationState;

typedef enum _JSClassDefinitionTranslateState {
    kJSClassDefinitionTranslateStateInit = 0,
    kJSClassDefinitionTranslateStateObjectSpecifier,
    kJSClassDefinitionTranslateStateObjectIdentifier,
    kJSClassDefinitionTranslateStateConfirm,
    kJSClassDefinitionTranslateStateExtends,
    kJSClassDefinitionTranslateStateExtendedIdentifier,
    kJSClassDefinitionTranslateStateError,
    kJSClassDefinitionTranslateStateMax,
} JSClassDefinitionTranslateState;

typedef enum _JSMethodDefinitionTranslateState {
    kJSMethodDefinitionTranslateStateInit = 0,
    kJSMethodDefinitionTranslateStateIdentifier,
    kJSMethodDefinitionTranslateStateParameterList,
    kJSMethodDefinitionTranslateStateParameterListEnd,
    kJSMethodDefinitionTranslateStateColon,
    kJSMethodDefinitionTranslateStateFunction,
    kJSMethodDefinitionTranslateStateConfirm,
    kJSMethodDefinitionTranslateStateError,
    kJSMethodDefinitionTranslateStateMax,
} JSMethodDefinitionTranslateState;

typedef enum _JSDeclarationTranslateState {
    kJSDeclarationTranslateStateInit = 0,
    kJSDeclarationTranslateStateDeclarationSpecifier,
    kJSDeclarationTranslateStateIdentifier,
    kJSDeclarationTranslateStateConfirm,
    kJSDeclarationTranslateStateError,
    kJSDeclarationTranslateStateMax,
} JSDeclarationTranslateState;

typedef cee_boolean (*ParseTrap)(CEESourceFragment*,
                                 CEEList**);

typedef struct _JSParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFragment* statement_root;
    CEESourceFragment* statement_current;
    CEESourceFragment* comment_root;
    CEESourceFragment* comment_current;
    ParseTrap block_header_traps[CEETokenID_MAX];
} JSParser;

static cee_int js_function_definition_translate_table[kJSFunctionDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int js_function_parameters_declaration_translate_table[kJSFunctionParametersDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int js_class_definition_translate_table[kJSClassDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int js_method_definition_translate_table[kJSMethodDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int js_declaration_translate_table[kJSDeclarationTranslateStateMax][CEETokenID_MAX];

static CEETokenType js_token_type_map[CEETokenID_MAX];

static void js_block_header_trap_init(JSParser* parser);
static JSParser* parser_create(void);
static void parser_free(cee_pointer data);
static cee_boolean token_id_is_assignment(CEETokenID identifier);
static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier);
static cee_boolean token_id_is_punctuation(CEETokenID identifier);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type);
static cee_boolean js_object_property_parse(CEESourceFragment* fragment);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFragment** prep_directive,
                                CEESourceFragment** statement,
                                CEESourceFragment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static cee_boolean reference_parse(CEESourceParserRef parser_ref,
                                   const cee_char* filepath,
                                   const cee_char* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFragment* prep_directive,
                                   CEESourceFragment* statement,
                                   CEERange range,
                                   CEEList** references);
static cee_boolean references_in_source_fragment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
                                                       CEESourceFragment* fragment,
                                                       const cee_char* subject,
                                                       CEESourceFragment* prep_directive,
                                                       CEESourceFragment* statement,
                                                       CEERange range,
                                                       CEEList** references);
static CEESourceFragment* js_referernce_fragment_convert(CEESourceFragment* fragment,
                                                         const cee_char* subject);
static void js_reference_fragment_parse(CEESourceParserRef parser_ref,
                                        const cee_char* filepath,
                                        CEESourceFragment* fragment,
                                        const cee_char* subject,
                                        CEESourceFragment* prep_directive,
                                        CEESourceFragment* statement,
                                        CEEList** references);
static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFragment* prep_directive,
                                          CEESourceFragment* statement,
                                          CEESourceSymbol** symbol);
static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFragment* searching,
                                                         const cee_char* name);
static void js_token_type_map_init(void);
static void symbol_parse_init(JSParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
static void symbol_parse_clear(JSParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
/** comment */
static cee_boolean comment_token_push(JSParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fragment_reduce(JSParser* parser);
static cee_boolean comment_attach(JSParser* parser);
static void label_parse(JSParser* parser);
static cee_boolean label_reduce(JSParser* parser);
/** statement */
static void statement_parse(JSParser* parser);
static cee_boolean statement_reduce(JSParser* parser);
static cee_boolean statement_pop(JSParser* parser);
static cee_boolean statement_attach(JSParser* parser,
                                    CEESourceFragmentType type);
static cee_boolean statement_sub_attach(JSParser* parser,
                                        CEESourceFragmentType type);
static cee_boolean statement_token_push(JSParser* parser,
                                        CEEToken* push);
/** block */
static void block_header_parse(JSParser* parser);
static cee_boolean statement_block_parse(CEESourceFragment* fragment);
static void block_push(JSParser* parser);
static cee_boolean block_pop(JSParser* parser);
static void block_parse(JSParser* parser);
static cee_boolean block_reduce(JSParser* parser);
/** parameter list */
static void parameter_list_push(JSParser* parser);
static cee_boolean parameter_list_pop(JSParser* parser);
/** subscript */
static void subscript_push(JSParser* parser);
static cee_boolean subscript_pop(JSParser* parser);
void js_function_definition_translate_table_init(void);
static cee_boolean js_function_definition_trap(CEESourceFragment* fragment,
                                               CEEList** pp);
static void js_function_parameters_declaration_translate_table_init(void);
static cee_boolean js_function_parameters_parse(CEESourceFragment* fragment);
static CEESourceSymbol* js_function_parameter_identifier_create(CEESourceFragment* fragment,
                                                                CEEList* identifier);
static cee_char* js_function_proto_descriptor_create(CEESourceFragment* fragment,
                                                     CEEList* prefix,
                                                     CEEList* prefix_tail,
                                                     CEEList* identifier,
                                                     CEEList* parameter_list,
                                                     CEEList* parameter_list_end);
static cee_boolean js_class_definition_trap(CEESourceFragment* fragment,
                                            CEEList** pp);
static cee_char* js_class_proto_descriptor_create(CEESourceFragment* fragment,
                                                  CEESourceSymbol* definition,
                                                  const cee_char* derives_str);
static void js_class_definition_translate_table_init(void);
static void js_method_definition_translate_table_init(void);
static cee_boolean js_method_definition_parse(CEESourceFragment* fragment);
static cee_boolean js_object_definition_parse(CEESourceFragment* fragment);
static void js_declaration_translate_table_init(void);
static cee_boolean js_declaration_parse(CEESourceFragment* fragment);
static CEESourceSymbol* js_identifier_declaration_create(CEESourceFragment* fragment,
                                                         CEEList* identifier);
static CEEList* skip_js_declaration_interval(CEEList* p);

static void js_block_header_trap_init(JSParser* parser)
{
   
   for (int i = 0; i < kCEETokenID_END; i ++)
       parser->block_header_traps[i] = NULL;
   
   parser->block_header_traps[kCEETokenID_CLASS] = js_class_definition_trap;
   parser->block_header_traps[kCEETokenID_FUNCTION] = js_function_definition_trap;
}

CEESourceParserRef cee_js_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    parser->symbol_search_in_scope = symbol_search_in_scope;
    
    JSParser* js = parser_create();
    js->super = parser;
        
    js_block_header_trap_init(js);
    js_token_type_map_init();
    
    js_function_definition_translate_table_init();
    js_function_parameters_declaration_translate_table_init();
    js_class_definition_translate_table_init();
    js_method_definition_translate_table_init();
    js_declaration_translate_table_init();
    
    parser->imp = js;
	return parser;
}

void cee_js_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static JSParser* parser_create(void)
{
    return cee_malloc0(sizeof(JSParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    JSParser* parser = (JSParser*)data;
    cee_free(parser);
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type)
{
    return (js_token_type_map[token->identifier] & type) != 0;
}

static cee_boolean token_id_is_assignment(CEETokenID identifier)
{
    return (js_token_type_map[identifier] & kCEETokenTypeAssignment) != 0;
}

static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier)
{
    return (js_token_type_map[identifier] & kCEETokenTypeDeclarationSpecifier) != 0;
}

static cee_boolean token_id_is_punctuation(CEETokenID identifier)
{
    return (js_token_type_map[identifier] & kCEETokenTypePunctuation) != 0;
}

static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFragment** prep_directive,
                                CEESourceFragment** statement,
                                CEESourceFragment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map)
{
    JSParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    
    if (!subject)
        return FALSE;
    
    map = cee_source_token_map_create(subject);
    cee_lexer_js_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
    
    do {
        ret = cee_lexer_js_token_get(&token);
    
        TOKEN_APPEND(tokens, token);
        cee_source_token_map(map, tokens);
        
        if (token_is_comment(token)) {
            comment_token_push(parser, token);
            comment_fragment_reduce(parser);
            
            if (!ret)
                break;
            else
                continue;
        }
                
        if (token->identifier == '{') {
            statement_token_push(parser, token);
            block_header_parse(parser);
            block_push(parser);
        }
        else if (token->identifier == '}') {
            block_parse(parser);
            if (!block_pop(parser))
                break;
            
            statement_token_push(parser, token);
            
            if (!block_reduce(parser))
                break;
        }
        else if (token->identifier == '[') {
            statement_token_push(parser, token);
            subscript_push(parser);
        }
        else if (token->identifier == ']') {
            if (!subscript_pop(parser))
                break;
            
            statement_token_push(parser, token);
        }
        else if (token->identifier == '(') {
            statement_token_push(parser, token);
            parameter_list_push(parser);
        }
        else if (token->identifier == ')') {
            if (!parameter_list_pop(parser))
                break;
            
            statement_token_push(parser, token);
        }
        else if (token->identifier == ':') {
            statement_token_push(parser, token);
            label_parse(parser);
            label_reduce(parser);
        }
        else if (token->identifier == ';') {
            statement_token_push(parser, token);
            statement_parse(parser);
            statement_reduce(parser);
        }
        else if (token->identifier == ',') {
            statement_token_push(parser, token);
            if (cee_source_fragment_grandfather_type_is(parser->statement_current, kCEESourceFragmentTypeObjectDefinition)) {
                js_object_property_parse(parser->statement_current);
                statement_reduce(parser);
            }
        }
        else {
            statement_token_push(parser, token);
        }
        
        if (!ret)
            break;
    
    } while(1);
    
    cee_source_fragment_tree_symbols_parent_parse(parser->statement_root);
    
    *statement = parser->statement_root;
    *prep_directive = NULL;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_js_buffer_free();
    
    return TRUE;
}

static cee_boolean js_object_property_parse(CEESourceFragment* fragment)
{
    cee_string_from_tokens_print(fragment->subject_ref,
                                 SOURCE_FREGMENT_TOKENS_FIRST(fragment),
                                 kCEETokenStringOptionDefault);
    
    if (!cee_source_fragment_grandfather_type_is(fragment, kCEESourceFragmentTypeObjectDefinition))
        return FALSE;
    
    if (!fragment || !fragment->tokens_ref)
        return FALSE;
    
    return TRUE;
}

static cee_boolean reference_parse(CEESourceParserRef parser_ref,
                                   const cee_char* filepath,
                                   const cee_char* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFragment* prep_directive,
                                   CEESourceFragment* statement,
                                   CEERange range,
                                   CEEList** references)
{
    CEESourceFragment* fragment = NULL;
    CEEList* p = NULL;
    
    CEESourceFragment* indexes[kCEESourceFragmentIndexMax];
    memset(indexes, 0, sizeof(CEESourceFragment*)*kCEESourceFragmentIndexMax);
    
    cee_source_fragment_indexes_in_range(source_token_map, range, indexes);
    
    for (cee_int i = kCEESourceFragmentIndexPrepDirective;
         i < kCEESourceFragmentIndexMax;
         i ++) {
        
        if (!indexes[i])
            continue;
        
        p = indexes[i]->node_ref;
        while (p) {
            fragment = p->data;
            
            if (!references_in_source_fragment_parse(parser_ref,
                                                     filepath,
                                                     fragment,
                                                     subject,
                                                     prep_directive,
                                                     statement,
                                                     range,
                                                     references))
                break;
            
            p = SOURCE_FREGMENT_NEXT(p);
        }
    }
    
    return TRUE;
}

static cee_boolean references_in_source_fragment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
                                                       CEESourceFragment* fragment,
                                                       const cee_char* subject,
                                                       CEESourceFragment* prep_directive,
                                                       CEESourceFragment* statement,
                                                       CEERange range,
                                                       CEEList** references)
{
    CEESourceFragment* reference_fragment = NULL;
    CEEList* p = NULL;
    
    if (cee_source_fragment_over_range(fragment, range))
        return FALSE;
    
    reference_fragment = js_referernce_fragment_convert(fragment, subject);
    js_reference_fragment_parse(parser_ref,
                                filepath,
                                reference_fragment,
                                subject,
                                prep_directive,
                                statement,
                                references);
    cee_source_fragment_free(reference_fragment);
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fragment);
    while (p) {
        if (!references_in_source_fragment_parse(parser_ref,
                                                 filepath,
                                                 p->data,
                                                 subject,
                                                 prep_directive,
                                                 statement,
                                                 range,
                                                 references))
            return FALSE;
        
        p = SOURCE_FREGMENT_NEXT(p);
    }
    
    return TRUE;
}

static CEESourceFragment* js_referernce_fragment_convert(CEESourceFragment* fragment,
                                                         const cee_char* subject)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEESourceFragment* reference_fragment = NULL;
    CEESourceFragment* current = reference_fragment;
    
    if (cee_source_fragment_parent_type_is(fragment, kCEESourceFragmentTypeSquareBracketList)) {
        reference_fragment = cee_source_fragment_create(kCEESourceFragmentTypeSquareBracketList,
                                                        fragment->filepath_ref,
                                                        fragment->subject_ref,
                                                        "java_script");
        reference_fragment = cee_source_fragment_sub_attach(reference_fragment,
                                                            kCEESourceFragmentTypeStatement,
                                                            fragment->filepath_ref,
                                                            fragment->subject_ref,
                                                            "java_script");
    }
    else {
        reference_fragment = cee_source_fragment_create(kCEESourceFragmentTypeStatement,
                                                        fragment->filepath_ref,
                                                        fragment->subject_ref,
                                                        "java_script");
    }
    
    current = reference_fragment;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    
    while (p) {
        token = p->data;
        
        if (!(token->state & kCEETokenStateSymbolOccupied)) {
            if (token->identifier == '{') {   /** ^{ */
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fragment_push(current,
                                                   kCEESourceFragmentTypeCurlyBracketList,
                                                   fragment->filepath_ref,
                                                   fragment->subject_ref,
                                                   "java_script");
            }
            else if (token->identifier == '}') {
                current = cee_source_fragment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else if (token->identifier == '[') {
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fragment_push(current,
                                                   kCEESourceFragmentTypeSquareBracketList,
                                                   fragment->filepath_ref,
                                                   fragment->subject_ref,
                                                   "java_script");
            }
            else if (token->identifier == ']') {
                current = cee_source_fragment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else if (token->identifier == '(') {
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fragment_push(current,
                                                   kCEESourceFragmentTypeRoundBracketList,
                                                   fragment->filepath_ref,
                                                   fragment->subject_ref,
                                                   "java_script");
            }
            else if (token->identifier == ')') {
                current = cee_source_fragment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else {
                if (token->identifier == kCEETokenID_IDENTIFIER ||
                    token_id_is_punctuation(token->identifier))
                    SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
        }
        p = TOKEN_NEXT(p);
    }
    
    if (!current)
        cee_source_fragment_free(reference_fragment);
    
    return current;
}


static void js_reference_fragment_parse(CEESourceParserRef parser_ref,
                                        const cee_char* filepath,
                                        CEESourceFragment* fragment,
                                        const cee_char* subject,
                                        CEESourceFragment* prep_directive,
                                        CEESourceFragment* statement,
                                        CEEList** references)
{
    CEEList* p = NULL;
    CEESourceSymbolReference* reference = NULL;
    CEEList* sub = NULL;
    CEESourceReferenceType type = kCEESourceReferenceTypeUnknow;
    
    if (!fragment)
        return;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        
        type = kCEESourceReferenceTypeUnknow;
        
        if (cee_source_fragment_tokens_pattern_match(fragment, p, '.', kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fragment_tokens_break(fragment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeMember;
        }
        else if (cee_source_fragment_tokens_pattern_match(fragment, p, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch any other identifier */
            p = cee_source_fragment_tokens_break(fragment, p, cee_range_make(0, 1), &sub);
        }
        else {
            p = TOKEN_NEXT(p);
        }
        
        if (sub) {
            reference = cee_source_symbol_reference_create(parser_ref,
                                                           (const cee_char*)filepath,
                                                           subject,
                                                           sub,
                                                           type);
            *references = cee_list_prepend(*references, reference);
            reference = NULL;
            sub = NULL;
        }
    }
    
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fragment);
    while (p) {
        js_reference_fragment_parse(parser_ref,
                                    filepath,
                                    p->data,
                                    subject,
                                    prep_directive,
                                    statement,
                                    references);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

static void js_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        js_token_type_map[i] = 0;
    
    js_token_type_map[kCEETokenID_ABSTRACT]                             = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_ARGUMENTS]                            = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_AWAIT]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_BOOLEAN]                              = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_BREAK]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_BYTE]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_CASE]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_CATCH]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_CHAR]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_CLASS]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_CONST]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_CONTINUE]                             = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_DEBUGGER]                             = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_DEFAULT]                              = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_DELETE]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_DO]                                   = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_DOUBLE]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_ELSE]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_ENUM]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_EVAL]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_EXPORT]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_EXTENDS]                              = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_FALSE]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_FINAL]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_FINALLY]                              = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_FLOAT]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_FOR]                                  = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_FUNCTION]                             = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_GOTO]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_IF]                                   = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_IMPLEMENTS]                           = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_IMPORT]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_IN]                                   = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_INSTANCEOF]                           = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_INT]                                  = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_INTERFACE]                            = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_LET]                                  = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    js_token_type_map[kCEETokenID_LONG]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_NATIVE]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_NEW]                                  = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_NULL]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_PACKAGE]                              = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_PRIVATE]                              = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_PROTECTED]                            = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_PUBLIC]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_RETURN]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_SHORT]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_STATIC]                               = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    js_token_type_map[kCEETokenID_SUPER]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_SWITCH]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_SYNCHRONIZED]                         = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_THIS]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_THROW]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_THROWS]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_TRANSIENT]                            = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_TRUE]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_TRY]                                  = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_TYPEOF]                               = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_VAR]                                  = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    js_token_type_map[kCEETokenID_VOID]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_VOLATILE]                             = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_WHILE]                                = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_WITH]                                 = kCEETokenTypeKeyword;
    js_token_type_map[kCEETokenID_YIELD]                                = kCEETokenTypeKeyword;
    js_token_type_map['+']                                              = kCEETokenTypePunctuation;
    js_token_type_map['-']                                              = kCEETokenTypePunctuation;
    js_token_type_map['*']                                              = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_EXP]                                  = kCEETokenTypePunctuation;
    js_token_type_map['/']                                              = kCEETokenTypePunctuation;
    js_token_type_map['%']                                              = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_INCREMENT]                            = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_DECREMENT]                            = kCEETokenTypePunctuation;
    js_token_type_map['&']                                              = kCEETokenTypePunctuation;
    js_token_type_map['|']                                              = kCEETokenTypePunctuation;
    js_token_type_map['^']                                              = kCEETokenTypePunctuation;
    js_token_type_map['~']                                              = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_ZERO_FILL_LEFT_SHIFT]                 = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_SIGNED_RIGHT_SHIFT]                   = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_ZERO_FILL_RIGHT_SHIFT]                = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_LOGIC_EQUAL]                          = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_EQUAL_VALUE_AND_TYPE]                 = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_LOGIC_UNEQUAL]                        = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_NOT_EQUAL_VALUE_AND_TYPE]             = kCEETokenTypePunctuation;
    js_token_type_map['<']                                              = kCEETokenTypePunctuation;
    js_token_type_map['>']                                              = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]                    = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_LOGIC_LESS_EQUAL]                     = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_LOGIC_AND]                            = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_LOGIC_OR]                             = kCEETokenTypePunctuation;
    js_token_type_map['!']                                              = kCEETokenTypePunctuation;
    js_token_type_map['=']                                              = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_ADD_ASSIGNMENT]                       = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_MINUS_ASSIGNMENT]                     = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_MULTI_ASSIGNMENT]                     = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_DIV_ASSIGNMENT]                       = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_MOD_ASSIGNMENT]                       = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_EXP_ASSIGNMENT]                       = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_ZERO_FILL_LEFT_SHIFT_ASSIGNMENT]      = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_SIGNED_RIGHT_SHIFT_ASSIGNMENT]        = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_ZERO_FILL_RIGHT_SHIFT_ASSIGNMENT]     = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_AND_ASSIGNMENT]                       = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_XOR_ASSIGNMENT]                       = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_OR_ASSIGNMENT]                        = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    js_token_type_map[kCEETokenID_ASSIGNMENT_ARROW]                     = kCEETokenTypePunctuation;
    js_token_type_map[kCEETokenID_LINES_COMMENT]                        = kCEETokenTypeComment;
    js_token_type_map[kCEETokenID_LINE_COMMENT]                         = kCEETokenTypeComment;
    js_token_type_map[kCEETokenID_LITERAL]                              = kCEETokenTypeLiteral;
    js_token_type_map[kCEETokenID_CHARACTER]                            = kCEETokenTypeCharacter;
    js_token_type_map[kCEETokenID_CONSTANT]                             = kCEETokenTypeConstant | kCEETokenTypeDeclarationSpecifier;
    js_token_type_map[kCEETokenID_IDENTIFIER]                           = kCEETokenTypeIdentifier;
}

static void symbol_parse_init(JSParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fragment_create(kCEESourceFragmentTypeRoot,
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "java_script");
    parser->statement_current = cee_source_fragment_sub_attach(parser->statement_root,
                                                               kCEESourceFragmentTypeSourceList,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "java_script");
    parser->statement_current = cee_source_fragment_sub_attach(parser->statement_current,
                                                               kCEESourceFragmentTypeStatement,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "java_script");
    parser->comment_root = cee_source_fragment_create(kCEESourceFragmentTypeRoot,
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "java_script");
    parser->comment_current = cee_source_fragment_sub_attach(parser->comment_root,
                                                             kCEESourceFragmentTypeSourceList,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "java_script");
    parser->comment_current = cee_source_fragment_sub_attach(parser->comment_current,
                                                             kCEESourceFragmentTypeComment,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "java_script");
}

static void symbol_parse_clear(JSParser* parser)
{
    parser->subject_ref = NULL;
    parser->filepath_ref = NULL;
    parser->statement_root = NULL;
    parser->statement_current = NULL;
    parser->comment_root = NULL;
    parser->comment_current = NULL;
}

/**
 *  comment
 */
static cee_boolean token_is_comment(CEEToken* token)
{
    return token->identifier == kCEETokenID_LINES_COMMENT ||
            token->identifier == kCEETokenID_LINE_COMMENT;
}

static cee_boolean comment_token_push(JSParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fragment_reduce(JSParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(JSParser* parser)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fragment_attach(parser->comment_current,
                                          kCEESourceFragmentTypeComment,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "java_script");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

static void label_parse(JSParser* parser)
{
    
}

static cee_boolean label_reduce(JSParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    if (cee_source_fragment_type_is(parser->statement_current, kCEESourceFragmentTypeLabelDeclaration))
        statement_attach(parser, kCEESourceFragmentTypeStatement);
    
    return TRUE;
}

/** statement */
static void statement_parse(JSParser* parser)
{
    CEESourceFragment* current = parser->statement_current;
    if (!current || !current->tokens_ref)
        return;
    
    if (js_declaration_parse(current))
        return;
        
    return;
}

static void js_declaration_translate_table_init(void)
{
    /**
     *                          declaration_specifier       identifier      assign_operator     ,           ;
     *  Init                    DeclarationSpecifier        Error           Error               Error       Error
     *  DeclarationSpecifier    Error                       Identifier      Error               Error       Error
     *  Identifier              Error                       Error           Confirm             Confirm     Confirm
     *
     */
    TRANSLATE_STATE_INI(js_declaration_translate_table, kJSDeclarationTranslateStateMax                     , kJSDeclarationTranslateStateError                                                             );
    TRANSLATE_FUNCS_SET(js_declaration_translate_table, kJSDeclarationTranslateStateInit                    , token_id_is_declaration_specifier     , kJSDeclarationTranslateStateDeclarationSpecifier      );
    TRANSLATE_STATE_SET(js_declaration_translate_table, kJSDeclarationTranslateStateDeclarationSpecifier    , kCEETokenID_IDENTIFIER                , kJSDeclarationTranslateStateIdentifier                );
    TRANSLATE_FUNCS_SET(js_declaration_translate_table, kJSDeclarationTranslateStateIdentifier               , token_id_is_assignment               , kJSDeclarationTranslateStateConfirm                   );
    TRANSLATE_STATE_SET(js_declaration_translate_table, kJSDeclarationTranslateStateIdentifier               , ','                                  , kJSDeclarationTranslateStateConfirm                   );
    TRANSLATE_STATE_SET(js_declaration_translate_table, kJSDeclarationTranslateStateIdentifier               , ';'                                  , kJSDeclarationTranslateStateConfirm                   );
}

static cee_boolean js_declaration_parse(CEESourceFragment* fragment)
{
    CEEList* p = NULL;
    CEEList* identifier = NULL;
    CEEToken* token = NULL;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    cee_boolean seek_variable = FALSE;
    cee_boolean ret = FALSE;
    JSDeclarationTranslateState current = kJSDeclarationTranslateStateInit;
    
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        if (token_id_is_declaration_specifier(token->identifier)) {
            seek_variable = TRUE;
            break;
        }
        p = TOKEN_NEXT(p);
    }
    
    if (!seek_variable)
        return FALSE;
    
    
    while (p) {
        token = p->data;
        current = js_declaration_translate_table[current][token->identifier];
        
        if (current == kJSDeclarationTranslateStateError) {
            break;
        }
        else if (current == kJSDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kJSDeclarationTranslateStateConfirm) {
            if (identifier) {
                declaration = js_identifier_declaration_create(fragment,
                                                               identifier);
            }
                    
            if (declaration)
                declarations = cee_list_prepend(declarations, declaration);
            
            identifier = NULL;
            
            if (!cee_token_is_identifier(p, ',') &&
                !cee_token_is_identifier(p, ';')) {
                p = skip_js_declaration_interval(p);
                if (!p)
                    break;
            }
            
            current = kJSDeclarationTranslateStateDeclarationSpecifier;
        }
        
        p = TOKEN_NEXT(p);
    }

    if (declarations) {
        fragment->symbols = cee_list_concat(fragment->symbols, declarations);
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_DECLARATION
    if (fragment->symbols)
        cee_source_symbols_print(fragment->symbols);
#endif
    
    return ret;
}

static CEESourceSymbol* js_identifier_declaration_create(CEESourceFragment* fragment,
                                                         CEEList* identifier)
{
    CEESourceSymbol* declaration = NULL;
    declaration = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                            fragment->subject_ref,
                                                            identifier,
                                                            identifier,
                                                            kCEESourceSymbolTypeVariableDeclaration,
                                                            "java_script",
                                                            kCEETokenStringOptionCompact);
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    return declaration;
}

static CEEList* skip_js_declaration_interval(CEEList* p)
{
    BRACKET_SIGN_DECLARATION();
    CEEToken* token = NULL;
    while (p) {
        token = p->data;
        
        SIGN_BRACKET(token->identifier);
        if (BRACKETS_IS_CLEAN()) {
            if (token->identifier == ',' ||
                token->identifier == ';')
                return p;
        }
        
        p = TOKEN_NEXT(p);
    }
    return NULL;
}

static cee_boolean statement_reduce(JSParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
   
    statement_attach(parser, kCEESourceFragmentTypeStatement);
   
    return TRUE;
}

static cee_boolean statement_pop(JSParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
   
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}

static cee_boolean statement_attach(JSParser* parser,
                                    CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
   
    if (!parser->statement_current)
        return FALSE;
   
    attached = cee_source_fragment_attach(parser->statement_current,
                                          type,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "java_script");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(JSParser* parser,
                                        CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fragment_sub_attach(parser->statement_current,
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              "java_script");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_token_push(JSParser* parser,
                                        CEEToken* push)
{
    if (!parser->statement_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->statement_current, push, TRUE);
    
    return TRUE;
}

/**
 * block
 */
static void block_header_parse(JSParser* parser)
{
    CEESourceFragment* current = parser->statement_current;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    ParseTrap trap = NULL;
    
    if (!current || !current->tokens_ref)
        return;
    
    BRACKET_SIGN_DECLARATION();
    p = SOURCE_FREGMENT_TOKENS_FIRST(current);
    while (p) {
        token = p->data;
        SIGN_BRACKET(token->identifier);
        
        if (BRACKETS_IS_CLEAN()) {
            trap = parser->block_header_traps[token->identifier];
            if (trap) {
                if (trap(current, &p))
                    return;
            }
        }
        if (p)
            p = TOKEN_NEXT(p);
    }
    
    if (js_method_definition_parse(current))
        return;
    
    if (js_object_definition_parse(current))
        return;
        
    if (statement_block_parse(current))
        return;
    
    return;
}

static void js_method_definition_translate_table_init(void)
{
    /**
     *                      static  get     set     await   async   identifier      *           :           function    (               )                   {
     *  Init                Init    Init    Init    Init    Init    Identifier      Init        Error       Error       Error           Error               Error
     *  Identifier          Error   Error   Error   Error   Error   Error           Error       Colon       Error       ParameterList   Error               Error
     *  ParameterList       Error   Error   Error   Error   Error   Error           Error       Error       Error       Error           ParameterListEnd    Error
     *  ParameterListEnd    Error   Error   Error   Error   Error   Error           Error       Error       Error       Error           Error               Confirm
     *  Colon               Error   Error   Error   Colon   Colon   Error           Error       Error       Function    Error           Error               Error
     *  Function            Error   Error   Error   Error   Error   Error           Function    Error       Error       ParameterList   Error               Error
     */
    TRANSLATE_STATE_INI(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateMax              , kJSMethodDefinitionTranslateStateError                                        );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateInit             , kCEETokenID_STATIC        , kJSMethodDefinitionTranslateStateInit             );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateInit             , kCEETokenID_GET           , kJSMethodDefinitionTranslateStateInit             );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateInit             , kCEETokenID_SET           , kJSMethodDefinitionTranslateStateInit             );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateInit             , kCEETokenID_AWAIT         , kJSMethodDefinitionTranslateStateInit             );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateInit             , kCEETokenID_ASYNC         , kJSMethodDefinitionTranslateStateInit             );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateInit             , kCEETokenID_IDENTIFIER    , kJSMethodDefinitionTranslateStateIdentifier       );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateInit             , '*'                       , kJSMethodDefinitionTranslateStateInit             );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateIdentifier       , '('                       , kJSMethodDefinitionTranslateStateParameterList    );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateIdentifier       , ':'                       , kJSMethodDefinitionTranslateStateColon            );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateColon            , kCEETokenID_AWAIT         , kJSMethodDefinitionTranslateStateColon            );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateColon            , kCEETokenID_ASYNC         , kJSMethodDefinitionTranslateStateColon            );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateColon            , kCEETokenID_FUNCTION      , kJSMethodDefinitionTranslateStateFunction         );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateFunction         , '*'                       , kJSMethodDefinitionTranslateStateFunction         );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateFunction         , '('                       , kJSMethodDefinitionTranslateStateParameterList    );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateParameterList    , ')'                       , kJSMethodDefinitionTranslateStateParameterListEnd );
    TRANSLATE_STATE_SET(js_method_definition_translate_table, kJSMethodDefinitionTranslateStateParameterListEnd , '{'                       , kJSMethodDefinitionTranslateStateConfirm          );
}

static cee_boolean js_method_definition_parse(CEESourceFragment* fragment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEESourceSymbol* definition = NULL;
    CEESourceFragment* child = NULL;
    JSMethodDefinitionTranslateState current = kJSMethodDefinitionTranslateStateInit;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        current = js_method_definition_translate_table[current][token->identifier];
        
        if (current == kJSMethodDefinitionTranslateStateError) {
            return FALSE;
        }
        else if (current == kJSMethodDefinitionTranslateStateConfirm) {
            break;
        }
        else if (current == kJSMethodDefinitionTranslateStateParameterList) {
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kJSMethodDefinitionTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
        else if (current == kJSMethodDefinitionTranslateStateIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                identifier = p;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kJSMethodDefinitionTranslateStateConfirm) {
        if (!identifier)
            return FALSE;
        
        definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                               fragment->subject_ref,
                                                               identifier,
                                                               identifier,
                                                               kCEESourceSymbolTypeFunctionDefinition,
                                                               "java_script",
                                                               kCEETokenStringOptionCompact);
        if (definition) {
            cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
            cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeFunctionDefinition);
            child = cee_source_fragment_child_index_by_leaf(fragment, parameter_list->data);
            if (child) {
                child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
                js_function_parameters_parse(child);
            }
            
            definition->proto_descriptor =
                js_function_proto_descriptor_create(fragment,
                                                    SOURCE_FREGMENT_TOKENS_FIRST(fragment),
                                                    TOKEN_PREV(identifier),
                                                    identifier,
                                                    parameter_list,
                                                    parameter_list_end);
        }
    }
    
    if (definition) {
        fragment->symbols = cee_list_prepend(fragment->symbols, definition);
    }
    
#ifdef DEBUG_FUNCTION_DEFINITION
    if (fragment->symbols)
        cee_source_symbols_print(fragment->symbols);
#endif
    
    return TRUE;
}

static cee_boolean js_object_definition_parse(CEESourceFragment* fragment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    CEESourceSymbol* definition = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_LAST(fragment);
    q = cee_token_not_whitespace_newline_before(p);
    if (q) {
        token = q->data;
        if (token_id_is_assignment(token->identifier)) {
            q = cee_token_not_whitespace_newline_before(q);
            if (q) {
                token = q->data;
                if (token->identifier == kCEETokenID_IDENTIFIER) {
                    definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                           fragment->subject_ref,
                                                                           q,
                                                                           q,
                                                                           kCEESourceSymbolTypeObjectDefinition,
                                                                           "java_script",
                                                                           kCEETokenStringOptionCompact);
                    if (definition) {
                        cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
                        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeAssignmentBlock);
                        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeObjectDefinition);
                        fragment->symbols = cee_list_prepend(fragment->symbols, definition);
                    }
                }
            }
        }
    }
    
    return TRUE;
}

static cee_boolean statement_block_parse(CEESourceFragment* fragment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* q = NULL;
    
    if (cee_source_fragment_type_is(fragment, kCEESourceFragmentTypeAssignmentBlock) ||
        cee_source_fragment_type_is(fragment, kCEESourceFragmentTypeStatementBlock))
        return TRUE;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        if (token->identifier == '{') {
            q = cee_token_not_whitespace_newline_before(p);
            if (q) {
                token = q->data;
                if (token_id_is_assignment(token->identifier)) {
                    cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeAssignmentBlock);
                }
                else {
                    cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeStatementBlock);
                }
            }
            else {
                cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeStatementBlock);
            }
        }
        p = TOKEN_NEXT(p);
    }
    
    return TRUE;
}

static void block_push(JSParser* parser)
{
    statement_sub_attach(parser, kCEESourceFragmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFragmentTypeStatement);
}

static cee_boolean block_pop(JSParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void block_parse(JSParser* parser)
{
    js_object_property_parse(parser->statement_current);
}

static cee_boolean block_reduce(JSParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fragment_symbols_fragment_range_mark(parser->statement_current);
    
    if (cee_source_fragment_type_is(parser->statement_current, kCEESourceFragmentTypeAssignmentBlock)) {
        /** expect statement terminate ';' */
    }
    else {
        statement_attach(parser, kCEESourceFragmentTypeStatement);
    }
    return TRUE;
}


void js_function_definition_translate_table_init(void)
{
    
    /**
     *                      keyword     identifier          (               )                   {           others
     *  Init                Keyword     Error               Error           Error               Error       Error
     *  Keyword             Error       Identifier          Error           Error               Error       Error
     *  Identifier          Error       Error               ParameterList   Error               Error       Error
     *  ParameterList       Error       Error               Error           ParameterListEnd    Error       Error
     *  ParameterListEnd    Error       Error               Error           Error               Confirm     Error
     *
     */
    TRANSLATE_STATE_INI(js_function_definition_translate_table, kJSFunctionDefinitionTranslateStateMax              , kJSFunctionDefinitionTranslateStateError                                              );
    TRANSLATE_STATE_SET(js_function_definition_translate_table, kJSFunctionDefinitionTranslateStateInit             , kCEETokenID_FUNCTION          , kJSFunctionDefinitionTranslateStateKeyword            );
    TRANSLATE_STATE_SET(js_function_definition_translate_table, kJSFunctionDefinitionTranslateStateKeyword          , kCEETokenID_IDENTIFIER        , kJSFunctionDefinitionTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(js_function_definition_translate_table, kJSFunctionDefinitionTranslateStateIdentifier       , '('                           , kJSFunctionDefinitionTranslateStateParameterList      );
    TRANSLATE_STATE_SET(js_function_definition_translate_table, kJSFunctionDefinitionTranslateStateParameterList    , ')'                           , kJSFunctionDefinitionTranslateStateParameterListEnd   );
    TRANSLATE_STATE_SET(js_function_definition_translate_table, kJSFunctionDefinitionTranslateStateParameterListEnd , '{'                           , kJSFunctionDefinitionTranslateStateConfirm            );
}

cee_boolean js_function_definition_trap(CEESourceFragment* fragment,
                                        CEEList** pp)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEESourceSymbol* definition = NULL;
    CEEToken* token = NULL;
    CEESourceFragment* child = NULL;
    JSFunctionDefinitionTranslateState current = kJSFunctionDefinitionTranslateStateInit;
    
    p = *pp;
    while (p) {
        token = p->data;
        
        current = js_function_definition_translate_table[current][token->identifier];
        
        if (current == kJSFunctionDefinitionTranslateStateError) {
            return FALSE;
        }
        else if (current == kJSFunctionDefinitionTranslateStateConfirm) {
            break;
        }
        else if (current == kJSFunctionDefinitionTranslateStateParameterList) {
            if (!identifier) {
                q = cee_token_not_whitespace_newline_before(p);
                if (cee_token_is_identifier(q, kCEETokenID_IDENTIFIER))
                    identifier = q;
            }
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kJSFunctionDefinitionTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }

        p = TOKEN_NEXT(p);
    }
    
    if (current == kJSFunctionDefinitionTranslateStateConfirm) {
        definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                               fragment->subject_ref,
                                                               q,
                                                               q,
                                                               kCEESourceSymbolTypeFunctionDefinition,
                                                               "java_script",
                                                               kCEETokenStringOptionCompact);
        if (definition) {
            cee_token_slice_state_mark(q, r, kCEETokenStateSymbolOccupied);
            cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeFunctionDefinition);
            child = cee_source_fragment_child_index_by_leaf(fragment, parameter_list->data);
            if (child) {
                child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
                js_function_parameters_parse(child);
            }
            
            definition->proto_descriptor = js_function_proto_descriptor_create(fragment,
                                                                               SOURCE_FREGMENT_TOKENS_FIRST(fragment),
                                                                               TOKEN_PREV(q),
                                                                               q,
                                                                               parameter_list,
                                                                               parameter_list_end);
            
        }
    }
    
    if (definition) {
        fragment->symbols = cee_list_prepend(fragment->symbols, definition);
        *pp = NULL;
    }
    
#ifdef DEBUG_FUNCTION_DEFINITION
    if (fragment->symbols)
        cee_source_symbols_print(fragment->symbols);
#endif
    
    return TRUE;
}

static void js_function_parameters_declaration_translate_table_init(void)
{
    /**
     *                  identifier      ,           ...     others
     *  Init            identifier      Error       Init    Error
     *  Identifier      identifier      Confirm     Error   Error
     */
    TRANSLATE_STATE_INI(js_function_parameters_declaration_translate_table, kJSFunctionParametersDeclarationTranslateStateMax           , kJSFunctionParametersDeclarationTranslateStateError                                   );
    TRANSLATE_STATE_SET(js_function_parameters_declaration_translate_table, kJSFunctionParametersDeclarationTranslateStateInit          , kCEETokenID_IDENTIFIER    , kJSFunctionParametersDeclarationTranslateStateIdentifier  );
    TRANSLATE_STATE_SET(js_function_parameters_declaration_translate_table, kJSFunctionParametersDeclarationTranslateStateInit          , kCEETokenID_ELLIPSIS      , kJSFunctionParametersDeclarationTranslateStateInit        );
    TRANSLATE_STATE_SET(js_function_parameters_declaration_translate_table, kJSFunctionParametersDeclarationTranslateStateIdentifier    , ','                       , kJSFunctionParametersDeclarationTranslateStateConfirm     );
}

static cee_boolean js_function_parameters_parse(CEESourceFragment* fragment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    JSFunctionParametersDeclarationState current = kJSFunctionParametersDeclarationTranslateStateInit;
    CEESourceSymbol* declaration = NULL;
    CEEList* identifier = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
                
        current = js_function_parameters_declaration_translate_table[current][token->identifier];
        
        if (current == kJSFunctionParametersDeclarationTranslateStateError) {
            return FALSE;
        }
        
        if (current == kJSFunctionParametersDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }

        if (current == kJSFunctionParametersDeclarationTranslateStateConfirm || !TOKEN_NEXT(p)) {
            if (identifier)
                declaration = js_function_parameter_identifier_create(fragment, identifier);
                        
            if (declaration)
                fragment->symbols = cee_list_prepend(fragment->symbols, declaration);
            
            declaration = NULL;
            identifier = NULL;
            
            current = kJSFunctionParametersDeclarationTranslateStateInit;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (fragment->symbols)
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeDeclaration);
    
#ifdef DEBUG_FUNCTION_DEFINITION
    if (fragment->symbols)
        cee_source_symbols_print(fragment->symbols);
#endif
    return TRUE;
}

static CEESourceSymbol* js_function_parameter_identifier_create(CEESourceFragment* fragment,
                                                                CEEList* identifier)
{
    CEESourceSymbol* parameter = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                           fragment->subject_ref,
                                                                           identifier,
                                                                           identifier,
                                                                           kCEESourceSymbolTypeFunctionParameter,
                                                                           "java_script",
                                                                           kCEETokenStringOptionCompact);
    if (!parameter)
        return NULL;
    
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    return parameter;
}

static cee_char* js_function_proto_descriptor_create(CEESourceFragment* fragment,
                                                     CEEList* prefix,
                                                     CEEList* prefix_tail,
                                                     CEEList* identifier,
                                                     CEEList* parameter_list,
                                                     CEEList* parameter_list_end)
{
    
    if (!fragment ||
        !identifier ||
        !parameter_list ||
        !parameter_list_end)
        return NULL;
    
    const cee_char* subject = fragment->subject_ref;
    CEEList* tokens = cee_source_fragment_tokens_expand(fragment);
    CEEList* p = NULL;
    cee_char* descriptor = NULL;
    cee_char* formater_str = NULL;
    CEEToken* token_prev = NULL;
    CEEToken* token = NULL;
    CEEList* parameters_tokens = NULL;
    cee_size l = 0;
        
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"function\"", NULL);
    
    /** create identifier descriptor */
    formater_str = cee_string_from_token(fragment->subject_ref, identifier->data);
    if (formater_str) {
        cee_strconcat0(&descriptor, ", \"name\":", "\"", formater_str, "\"", NULL);
        cee_free(formater_str);
        formater_str = NULL;
    }
    
    /** create parameters descriptor */
    p = TOKEN_FIRST(tokens);
    while (p) {
        if (p->data == parameter_list->data)
            break;
        p = TOKEN_NEXT(p);
    }
    if (p)
        p = TOKEN_NEXT(p);
    while (p) {
        if (p->data == parameter_list_end->data)
            break;
        TOKEN_APPEND(parameters_tokens, p->data);
        p = TOKEN_NEXT(p);
    }
    
    token_prev = NULL;
    cee_strconcat0(&descriptor, ", \"parameters\":[", NULL);
    p = TOKEN_FIRST(parameters_tokens);
    
    while (p) {
        token = p->data;
        
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            cee_string_concat_with_token(&formater_str, subject, token);
            token_prev = token;
        }
        
        if (token->identifier == ',' || !TOKEN_NEXT(p)) {
            if (formater_str) {
                l = strlen(formater_str);
                if (formater_str[l-1]== ',')
                    formater_str[l-1] = '\0';
                
                cee_strconcat0(&descriptor, "\"", formater_str, "\"", ",", NULL);
                cee_free(formater_str);
                formater_str = NULL;
                token_prev = NULL;
            }
        }
                
        p = TOKEN_NEXT(p);
    }
    /** remove the last comma in jason array */
    l = strlen(descriptor);
    if (descriptor[l-1]== ',')
        descriptor[l-1] = '\0';
    cee_strconcat0(&descriptor, "]", NULL);
        
    cee_strconcat0(&descriptor, "}", NULL);
        
    if (parameters_tokens)
        cee_list_free(parameters_tokens);
    
    if (tokens)
        cee_list_free(tokens);
    
    return descriptor;
}

static void js_class_definition_translate_table_init(void)
{
    /**
     *                          object_specifier            identifier              extends     {
     *  Init                    ObjectSpecifier             Error                   Error       Error
     *  ObjectSpecifier         Error                       ObjectIdentifier        Error       Confirm
     *  ObjectIdentifier        Error                       Error                   Extends     Confirm
     *  Extends                 Error                       ExtendedIdentifier      Error       Error
     *  ExtendedIdentifier      Error                       Error                   Error       Confirm
     */
    TRANSLATE_STATE_INI(js_class_definition_translate_table,    kJSClassDefinitionTranslateStateMax                     , kJSClassDefinitionTranslateStateError   );
    TRANSLATE_STATE_SET(js_class_definition_translate_table,    kJSClassDefinitionTranslateStateInit                    , kCEETokenID_CLASS                     , kJSClassDefinitionTranslateStateObjectSpecifier   );
    TRANSLATE_STATE_SET(js_class_definition_translate_table,    kJSClassDefinitionTranslateStateObjectSpecifier         , kCEETokenID_IDENTIFIER                , kJSClassDefinitionTranslateStateObjectIdentifier  );
    TRANSLATE_STATE_SET(js_class_definition_translate_table,    kJSClassDefinitionTranslateStateObjectIdentifier        , '{'                                   , kJSClassDefinitionTranslateStateConfirm           );
    TRANSLATE_STATE_SET(js_class_definition_translate_table,    kJSClassDefinitionTranslateStateObjectIdentifier        , kCEETokenID_EXTENDS                   , kJSClassDefinitionTranslateStateExtends           );
    TRANSLATE_STATE_SET(js_class_definition_translate_table,    kJSClassDefinitionTranslateStateExtends                 , kCEETokenID_IDENTIFIER                , kJSClassDefinitionTranslateStateExtendedIdentifier);
    TRANSLATE_STATE_SET(js_class_definition_translate_table,    kJSClassDefinitionTranslateStateExtendedIdentifier      , '{'                                   , kJSClassDefinitionTranslateStateConfirm           );
}

static cee_boolean js_class_definition_trap(CEESourceFragment* fragment,
                                            CEEList** pp)
{
    CEEList* p = *pp;
    CEEList* trap_keyword = *pp;
    JSClassDefinitionTranslateState current = kJSClassDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* identifier = NULL;
    CEEList* extened = NULL;
    CEESourceSymbol* definition = NULL;
    
    while (p) {
        token = p->data;
        current = js_class_definition_translate_table[current][token->identifier];
        
        if (current == kJSClassDefinitionTranslateStateError) {
            return FALSE;
        }
        else if (current == kJSClassDefinitionTranslateStateObjectIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                identifier = p;
        }
        else if (current == kJSClassDefinitionTranslateStateExtendedIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                extened = p;
        }
        else if (current == kJSClassDefinitionTranslateStateConfirm) {
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (identifier)
        definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                               fragment->subject_ref,
                                                               identifier,
                                                               identifier,
                                                               kCEESourceSymbolTypeClassDefinition,
                                                               "java_script",
                                                               kCEETokenStringOptionCompact);
    else
        definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                               fragment->subject_ref,
                                                               trap_keyword,
                                                               trap_keyword,
                                                               kCEESourceSymbolTypeClassDefinition,
                                                               "java_script",
                                                               kCEETokenStringOptionCompact);
    if (!definition)
        return FALSE;
        
    if (extened)
        definition->derives = cee_string_from_token_slice(fragment->subject_ref,
                                                          extened,
                                                          extened,
                                                          kCEETokenStringOptionCompact);
    if (identifier)
        cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
        
    definition->proto_descriptor = js_class_proto_descriptor_create(fragment,
                                                                    definition,
                                                                    definition->derives);
    
    fragment->symbols = cee_list_prepend(fragment->symbols, definition);
    cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeClassDefinition);
    *pp = NULL;
    
#ifdef DEBUG_CLASS
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
    return TRUE;
}

static cee_char* js_class_proto_descriptor_create(CEESourceFragment* fragment,
                                                  CEESourceSymbol* definition,
                                                  const cee_char* derives_str)
{
    if (!fragment || !definition)
        return NULL;
    
    cee_char* descriptor = NULL;
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"class\"", NULL);
        
    if (definition->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", definition->name, "\"", NULL);
    
    if (derives_str)
        cee_strconcat0(&descriptor, ", \"derivers\":", "\"", derives_str, "\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

/**
 * parameter list
 */
static void parameter_list_push(JSParser* parser)
{
    statement_sub_attach(parser, kCEESourceFragmentTypeRoundBracketList);
    statement_sub_attach(parser, kCEESourceFragmentTypeStatement);
}

static cee_boolean parameter_list_pop(JSParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

/**
 * subscript
 */
static void subscript_push(JSParser* parser)
{
    statement_sub_attach(parser, kCEESourceFragmentTypeSquareBracketList);
    statement_sub_attach(parser, kCEESourceFragmentTypeStatement);
}

static cee_boolean subscript_pop(JSParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFragment* prep_directive,
                                          CEESourceFragment* statement,
                                          CEESourceSymbol** symbol)
{
    cee_boolean ret = FALSE;
    CEESourceSymbol* statement_symbol = NULL;
    CEESourceSymbol* prep_directive_symbol = NULL;
    CEEList* searched_symbols = NULL;
    CEESourceFragment* current = cee_source_fragment_from_reference_get(reference);
    
    if (!current || !symbol || reference->type == kCEESourceReferenceTypeMember)
        goto exit;
    
    *symbol = NULL;
    
    statement_symbol = symbol_search_in_scope_recursive(current, reference->name);
    if (statement_symbol)
        statement_symbol = cee_source_symbol_copy(statement_symbol);
    
    searched_symbols =
        cee_source_fragment_tree_symbols_search(prep_directive,
                                                cee_source_symbol_matcher_by_name,
                                                (cee_pointer)reference->name);
    if (searched_symbols) {
        prep_directive_symbol = cee_source_symbol_copy(cee_list_nth_data(searched_symbols, 0));
        cee_list_free(searched_symbols);
    }
    
    if (statement_symbol && prep_directive_symbol) {
        CEERange range0 = cee_range_consistent_from_discrete(statement_symbol->ranges);
        CEERange range1 = cee_range_consistent_from_discrete(prep_directive_symbol->ranges);
        CEERange range3 = cee_range_consistent_from_discrete(reference->ranges);
        
        if (range1.location > range3.location || range0.location > range1.location) {
            *symbol = statement_symbol;
            cee_source_symbol_free(prep_directive_symbol);
        }
        else if (range1.location > range0.location){
            *symbol = prep_directive_symbol;
            cee_source_symbol_free(statement_symbol);
        }
    }
    else if (statement_symbol) {
        *symbol = statement_symbol;
    }
    else if (prep_directive_symbol) {
        *symbol = prep_directive_symbol;
    }
    
exit:
    if (symbol && *symbol)
        ret = TRUE;
    
    return ret;
}

static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFragment* current,
                                                         const cee_char* name)
{
    CEEList* symbols = NULL;
    CEESourceSymbol* referenced = NULL;
    CEESourceFragment* searching = current;
    CEEList* p = NULL;
    
    if (!searching)
        goto exit;
    
    /** search current fragment */
    symbols = cee_source_fragment_symbols_search_by_name(searching, name);
    if (symbols) {
        referenced = cee_list_nth_data(symbols, 0);
        goto exit;
    }
    
    /**
     * Search current round bracket list child fragment,
     * function parameters , "for" / "while" / "if" statments
     */
    p = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
    while (p) {
        searching = p->data;
        if (searching->type[kCEESourceFragmentTypeRoundBracketList]) {
            symbols = cee_source_fragment_symbols_in_children_search_by_name(searching,
                                                                             name);
            if (symbols) {
                referenced = cee_list_nth_data(symbols, 0);
                goto exit;
            }
        }
        p = SOURCE_FREGMENT_NEXT(p);
    }
    
    
    searching = current;
    
    /** search current siblings */
    if (searching->node_ref) {
        p = SOURCE_FREGMENT_PREV(searching->node_ref);
        while (p) {
            searching = p->data;
            symbols = cee_source_fragment_symbols_search_by_name(searching, name);
            if (symbols) {
                referenced = cee_list_nth_data(symbols, 0);
                goto exit;
            }
            
            /** enumerator search */
            if (searching->type[kCEESourceFragmentTypeEnumDefinition]) {
                CEEList* q = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
                while (q) {
                    searching = q->data;
                    symbols = cee_source_fragment_symbols_in_children_search_by_name(searching,
                                                                                     name);
                    if (symbols) {
                        referenced = cee_list_nth_data(symbols, 0);
                        goto exit;
                    }
                    q = SOURCE_FREGMENT_NEXT(q);
                }
            }
            
            p = SOURCE_FREGMENT_PREV(p);
        }
    }
    
    searching = current;
    if (cee_source_fragment_parent_type_is(searching, kCEESourceFragmentTypeRoundBracketList) ||
        cee_source_fragment_parent_type_is(searching, kCEESourceFragmentTypeCurlyBracketList) ||
        cee_source_fragment_parent_type_is(searching, kCEESourceFragmentTypeSquareBracketList)) {
        searching = cee_source_fragment_grandfather_get(searching);
        referenced = symbol_search_in_scope_recursive(searching, name);
    }
    
exit:
    return referenced;
}
