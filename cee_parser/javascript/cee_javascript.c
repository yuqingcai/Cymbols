#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_javascript.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef struct _JSParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
} JSParser;

static CEETokenType js_token_type_map[CEETokenID_MAX];
static JSParser* parser_create(void);
static void parser_free(cee_pointer data);
static cee_boolean token_id_is_assignment(CEETokenID identifier);
static cee_boolean token_id_is_punctuation(CEETokenID identifier);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static cee_boolean reference_parse(CEESourceParserRef parser_ref,
                                   const cee_char* filepath,
                                   const cee_char* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFregment* prep_directive,
                                   CEESourceFregment* statement,
                                   CEERange range,
                                   CEEList** references);
static cee_boolean references_in_source_fregment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
                                                       CEESourceFregment* fregment,
                                                       const cee_char* subject,
                                                       CEESourceFregment* prep_directive,
                                                       CEESourceFregment* statement,
                                                       CEERange range,
                                                       CEEList** references);
static CEESourceFregment* js_referernce_fregment_convert(CEESourceFregment* fregment,
                                                         const cee_char* subject);
static void js_reference_fregment_parse(CEESourceParserRef parser_ref,
                                        const cee_char* filepath,
                                        CEESourceFregment* fregment,
                                        const cee_char* subject,
                                        CEESourceFregment* prep_directive,
                                        CEESourceFregment* statement,
                                        CEEList** references);
static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFregment* prep_directive,
                                          CEESourceFregment* statement,
                                          CEESourceSymbol** symbol);
static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFregment* searching,
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
static cee_boolean comment_fregment_reduce(JSParser* parser);
static cee_boolean comment_attach(JSParser* parser);
static void label_parse(JSParser* parser);
static cee_boolean label_reduce(JSParser* parser);
/** statement */
static void statement_parse(JSParser* parser);
static cee_boolean statement_reduce(JSParser* parser);
static cee_boolean statement_pop(JSParser* parser);
static cee_boolean statement_attach(JSParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(JSParser* parser,
                                        CEESourceFregmentType type);
static cee_boolean statement_token_push(JSParser* parser,
                                        CEEToken* push);
/** block */
static void block_header_parse(JSParser* parser);
static cee_boolean statement_block_parse(CEESourceFregment* fregment);
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

CEESourceParserRef cee_js_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    parser->symbol_search_in_scope = symbol_search_in_scope;
    
    JSParser* js = parser_create();
    js->super = parser;
    
    js_token_type_map_init();
    
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

static cee_boolean token_id_is_punctuation(CEETokenID identifier)
{
    return (js_token_type_map[identifier] & kCEETokenTypePunctuation) != 0;
}

static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
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
            comment_fregment_reduce(parser);
            
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
        else {
            statement_token_push(parser, token);
        }
        
        if (!ret)
            break;
    
    } while(1);
    
    cee_source_fregment_tree_symbols_parent_parse(parser->statement_root);
    
    *statement = parser->statement_root;
    *prep_directive = NULL;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_js_buffer_free();
        
    return TRUE;
}

static cee_boolean reference_parse(CEESourceParserRef parser_ref,
                                   const cee_char* filepath,
                                   const cee_char* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFregment* prep_directive,
                                   CEESourceFregment* statement,
                                   CEERange range,
                                   CEEList** references)
{
    CEESourceFregment* fregment = NULL;
    CEEList* p = NULL;
    
    CEESourceFregment* indexes[kCEESourceFregmentIndexMax];
    memset(indexes, 0, sizeof(CEESourceFregment*)*kCEESourceFregmentIndexMax);
    
    cee_source_fregment_indexes_in_range(source_token_map, range, indexes);
    
    for (cee_int i = kCEESourceFregmentIndexPrepDirective;
         i < kCEESourceFregmentIndexMax;
         i ++) {
        
        if (!indexes[i])
            continue;
        
        p = indexes[i]->node_ref;
        while (p) {
            fregment = p->data;
            
            if (!references_in_source_fregment_parse(parser_ref,
                                                     filepath,
                                                     fregment,
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

static cee_boolean references_in_source_fregment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
                                                       CEESourceFregment* fregment,
                                                       const cee_char* subject,
                                                       CEESourceFregment* prep_directive,
                                                       CEESourceFregment* statement,
                                                       CEERange range,
                                                       CEEList** references)
{
    CEESourceFregment* reference_fregment = NULL;
    CEEList* p = NULL;
    
    if (cee_source_fregment_over_range(fregment, range))
        return FALSE;
    
    reference_fregment = js_referernce_fregment_convert(fregment, subject);
    js_reference_fregment_parse(parser_ref,
                                filepath,
                                reference_fregment,
                                subject,
                                prep_directive,
                                statement,
                                references);
    cee_source_fregment_free(reference_fregment);
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        if (!references_in_source_fregment_parse(parser_ref,
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

static CEESourceFregment* js_referernce_fregment_convert(CEESourceFregment* fregment,
                                                         const cee_char* subject)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* reference_fregment = NULL;
    CEESourceFregment* current = reference_fregment;
    
    if (cee_source_fregment_parent_type_is(fregment, kCEESourceFregmentTypeSquareBracketList)) {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeSquareBracketList,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        "java_script");
        reference_fregment = cee_source_fregment_sub_attach(reference_fregment,
                                                            kCEESourceFregmentTypeStatement,
                                                            fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            "java_script");
    }
    else {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeStatement,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        "java_script");
    }
    
    current = reference_fregment;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    
    while (p) {
        token = p->data;
        
        if (!(token->state & kCEETokenStateSymbolOccupied)) {
            if (token->identifier == '{') {   /** ^{ */
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fregment_push(current,
                                                   kCEESourceFregmentTypeCurlyBracketList,
                                                   fregment->filepath_ref,
                                                   fregment->subject_ref,
                                                   "java_script");
            }
            else if (token->identifier == '}') {
                current = cee_source_fregment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else if (token->identifier == '[') {
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fregment_push(current,
                                                   kCEESourceFregmentTypeSquareBracketList,
                                                   fregment->filepath_ref,
                                                   fregment->subject_ref,
                                                   "java_script");
            }
            else if (token->identifier == ']') {
                current = cee_source_fregment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else if (token->identifier == '(') {
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fregment_push(current,
                                                   kCEESourceFregmentTypeRoundBracketList,
                                                   fregment->filepath_ref,
                                                   fregment->subject_ref,
                                                   "java_script");
            }
            else if (token->identifier == ')') {
                current = cee_source_fregment_pop(current);
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
        cee_source_fregment_free(reference_fregment);
    
    return current;
}


static void js_reference_fregment_parse(CEESourceParserRef parser_ref,
                                        const cee_char* filepath,
                                        CEESourceFregment* fregment,
                                        const cee_char* subject,
                                        CEESourceFregment* prep_directive,
                                        CEESourceFregment* statement,
                                        CEEList** references)
{
    CEEList* p = NULL;
    CEESourceSymbolReference* reference = NULL;
    CEEList* sub = NULL;
    CEESourceReferenceType type = kCEESourceReferenceTypeUnknow;
    
    if (!fregment)
        return;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        
        type = kCEESourceReferenceTypeUnknow;
        
        if (cee_source_fregment_tokens_pattern_match(fregment, p, '.', kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeMember;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch any other identifier */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(0, 1), &sub);
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
    
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        js_reference_fregment_parse(parser_ref,
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
    js_token_type_map[kCEETokenID_LET]                                  = kCEETokenTypeKeyword;
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
    js_token_type_map[kCEETokenID_STATIC]                               = kCEETokenTypeKeyword;
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
    js_token_type_map[kCEETokenID_VAR]                                  = kCEETokenTypeKeyword;
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
    js_token_type_map['&']                                              = kCEETokenTypePunctuation ;
    js_token_type_map['|']                                              = kCEETokenTypePunctuation ;
    js_token_type_map['^']                                              = kCEETokenTypePunctuation ;
    js_token_type_map['~']                                              = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_ZERO_FILL_LEFT_SHIFT]                 = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_SIGNED_RIGHT_SHIFT]                   = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_ZERO_FILL_RIGHT_SHIFT]                = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_LOGIC_EQUAL]                          = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_EQUAL_VALUE_AND_TYPE]                 = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_LOGIC_UNEQUAL]                        = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_NOT_EQUAL_VALUE_AND_TYPE]             = kCEETokenTypePunctuation ;
    js_token_type_map['<']                                              = kCEETokenTypePunctuation ;
    js_token_type_map['>']                                              = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]                    = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_LOGIC_LESS_EQUAL]                     = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_LOGIC_AND]                            = kCEETokenTypePunctuation ;
    js_token_type_map[kCEETokenID_LOGIC_OR]                             = kCEETokenTypePunctuation ;
    js_token_type_map['!']                                              = kCEETokenTypePunctuation ;
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
    js_token_type_map[kCEETokenID_LINES_COMMENT]                        = kCEETokenTypeComment;
    js_token_type_map[kCEETokenID_LINE_COMMENT]                         = kCEETokenTypeComment;
    js_token_type_map[kCEETokenID_LITERAL]                              = kCEETokenTypeLiteral;
    js_token_type_map[kCEETokenID_CHARACTER]                            = kCEETokenTypeCharacter;
    js_token_type_map[kCEETokenID_CONSTANT]                             = kCEETokenTypeConstant;
    js_token_type_map[kCEETokenID_IDENTIFIER]                           = kCEETokenTypeIdentifier;
}

static void symbol_parse_init(JSParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot,
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "java_script");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root,
                                                               kCEESourceFregmentTypeSourceList,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "java_script");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current,
                                                               kCEESourceFregmentTypeStatement,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "java_script");
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot,
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "java_script");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root,
                                                             kCEESourceFregmentTypeSourceList,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "java_script");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current,
                                                             kCEESourceFregmentTypeComment,
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

static cee_boolean comment_fregment_reduce(JSParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}


static cee_boolean comment_attach(JSParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->comment_current,
                                          kCEESourceFregmentTypeComment,
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
    
    if (cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeLabelDeclaration))
        statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}

/** statement */
static void statement_parse(JSParser* parser)
{
   CEESourceFregment* current = parser->statement_current;
   if (!current || !current->tokens_ref)
       return;
   
   return;
}

static cee_boolean statement_reduce(JSParser* parser)
{
   if (!parser->statement_current)
       return FALSE;
   
   statement_attach(parser, kCEESourceFregmentTypeStatement);
   
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
                                    CEESourceFregmentType type)
{
   CEESourceFregment* attached = NULL;
   
   if (!parser->statement_current)
       return FALSE;
   
   attached = cee_source_fregment_attach(parser->statement_current,
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
                                        CEESourceFregmentType type)
{
   CEESourceFregment* attached = NULL;
   
   if (!parser->statement_current)
       return FALSE;
   
   attached = cee_source_fregment_sub_attach(parser->statement_current,
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
    CEESourceFregment* current = parser->statement_current;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    
    if (!current || !current->tokens_ref)
        return;
    
    return;
}

static cee_boolean statement_block_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    
    return TRUE;
}

static void block_push(JSParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean block_pop(JSParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void block_parse(JSParser* parser)
{
}

static cee_boolean block_reduce(JSParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    
    if (cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeAssignmentBlock)) {
        /** expect statement terminate ';' */
    }
    else {
        statement_attach(parser, kCEESourceFregmentTypeStatement);
    }
    return TRUE;
}

/**
 * parameter list
 */
static void parameter_list_push(JSParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeRoundBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
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
    statement_sub_attach(parser, kCEESourceFregmentTypeSquareBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean subscript_pop(JSParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFregment* prep_directive,
                                          CEESourceFregment* statement,
                                          CEESourceSymbol** symbol)
{
    cee_boolean ret = FALSE;
    CEESourceSymbol* statement_symbol = NULL;
    CEESourceSymbol* prep_directive_symbol = NULL;
    CEEList* searched_symbols = NULL;
    CEESourceFregment* current = cee_source_fregment_from_reference_get(reference);
    
    if (!current || !symbol || reference->type == kCEESourceReferenceTypeMember)
        goto exit;
    
    *symbol = NULL;
    
    statement_symbol = symbol_search_in_scope_recursive(current, reference->name);
    if (statement_symbol)
        statement_symbol = cee_source_symbol_copy(statement_symbol);
    
    searched_symbols =
        cee_source_fregment_tree_symbols_search(prep_directive,
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

static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFregment* current,
                                                         const cee_char* name)
{
    CEEList* symbols = NULL;
    CEESourceSymbol* referenced = NULL;
    CEESourceFregment* searching = current;
    CEEList* p = NULL;
    
    if (!searching)
        goto exit;
    
    /** search current fregment */
    symbols = cee_source_fregment_symbols_search_by_name(searching, name);
    if (symbols) {
        referenced = cee_list_nth_data(symbols, 0);
        goto exit;
    }
    
    /**
     * Search current round bracket list child fregment,
     * function parameters , "for" / "while" / "if" statments
     */
    p = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
    while (p) {
        searching = p->data;
        if (searching->type[kCEESourceFregmentTypeRoundBracketList]) {
            symbols = cee_source_fregment_symbols_in_children_search_by_name(searching,
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
            symbols = cee_source_fregment_symbols_search_by_name(searching, name);
            if (symbols) {
                referenced = cee_list_nth_data(symbols, 0);
                goto exit;
            }
            
            /** enumerator search */
            if (searching->type[kCEESourceFregmentTypeEnumDefinition]) {
                CEEList* q = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
                while (q) {
                    searching = q->data;
                    symbols = cee_source_fregment_symbols_in_children_search_by_name(searching,
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
    if (cee_source_fregment_parent_type_is(searching, kCEESourceFregmentTypeRoundBracketList) ||
        cee_source_fregment_parent_type_is(searching, kCEESourceFregmentTypeCurlyBracketList) ||
        cee_source_fregment_parent_type_is(searching, kCEESourceFregmentTypeSquareBracketList)) {
        searching = cee_source_fregment_grandfather_get(searching);
        referenced = symbol_search_in_scope_recursive(searching, name);
    }
    
exit:
    return referenced;
}
