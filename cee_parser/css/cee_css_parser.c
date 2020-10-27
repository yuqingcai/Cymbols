#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_css_parser.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef struct _CSSParser {
    CEESourceParserRef super;
    const cee_uchar* filepath_ref;
    const cee_uchar* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
} CSSParser;

static CSSParser* parser_create(void);
static void parser_free(cee_pointer data);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_uchar* filepath,
                                const cee_uchar* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static void symbol_parse_init(CSSParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
static void symbol_parse_clear(CSSParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(CSSParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(CSSParser* parser);
static cee_boolean comment_attach(CSSParser* parser);
static cee_boolean statement_token_push(CSSParser* parser,
                                        CEEToken* push);
static void block_header_parse(CSSParser* parser);
static void block_push(CSSParser* parser);
static cee_boolean block_pop(CSSParser* parser);
static void block_parse(CSSParser* parser);
static cee_boolean statement_pop(CSSParser* parser);
static cee_boolean block_reduce(CSSParser* parser);
static void statement_parse(CSSParser* parser);
static cee_boolean statement_reduce(CSSParser* parser);
static cee_boolean statement_attach(CSSParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(CSSParser* parser,
                                        CEESourceFregmentType type);

/**
 * parser
 */
CEESourceParserRef cee_css_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    
    CSSParser* css = parser_create();
    css->super = parser;
        
    parser->imp = css;
    return (CEESourceParserRef)parser;
}

void cee_css_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static CSSParser* parser_create(void)
{
    return cee_malloc0(sizeof(CSSParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    CSSParser* parser = (CSSParser*)data;
    cee_free(parser);
}

static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_uchar* filepath,
                                const cee_uchar* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map)
{
    CSSParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    
    if (!subject)
        return FALSE;
        
    map = cee_source_token_map_create(subject);
    cee_lexer_css_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
    
    do {
        ret = cee_lexer_css_token_get(&token);
        
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
    
    *statement = parser->statement_root;
    *prep_directive = NULL;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_css_buffer_free();
    
    return TRUE;
}


static void symbol_parse_init(CSSParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject)
{    
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref);
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref);
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref);
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref);    
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref);
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref);
}

static void symbol_parse_clear(CSSParser* parser)
{
    parser->subject_ref = NULL;
    parser->filepath_ref = NULL;
    parser->statement_root = NULL;
    parser->statement_current = NULL;
    parser->comment_root = NULL;
    parser->comment_current = NULL;
}

static cee_boolean token_is_comment(CEEToken* token)
{
    return token->identifier == kCEETokenID_C_COMMENT;
}

static cee_boolean comment_token_push(CSSParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fregment_reduce(CSSParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(CSSParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_append(parser->comment_current, 
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref);
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

static cee_boolean statement_token_push(CSSParser* parser,
                                        CEEToken* push)
{
    if (!parser->statement_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->statement_current, push, TRUE);
    
    return TRUE;
}

static void block_header_parse(CSSParser* parser)
{
    CEESourceFregment* fregment = parser->statement_current;
    CEESourceSymbol* symbol = NULL;
    
    if (!fregment || !fregment->tokens)
        return;
        
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    p = cee_token_not_whitespace_newline_after(p);  /** skip whitesapce and newline */
    CEEList* q = SOURCE_FREGMENT_TOKENS_LAST(fregment);
    q = TOKEN_PREV(q);  /** skip '{' */
    q = cee_token_not_whitespace_newline_before(q); /** skip whitesapce and newline */
    
    symbol = cee_source_symbol_create_from_token_slice(fregment->filepath_ref, 
                                                       fregment->subject_ref, 
                                                       p, 
                                                       q, 
                                                       kCEESourceSymbolTypeCSSSelector, 
                                                       "CSS");
    fregment->symbols = cee_list_prepend(fregment->symbols, symbol);
    cee_source_fregment_type_set_exclusive(fregment, kCEESourceFregmentTypeCSSBlock);
    
    return;
}

/**
 * block
 */
static void block_push(CSSParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean block_pop(CSSParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void block_parse(CSSParser* parser)
{
}

static cee_boolean statement_pop(CSSParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}


static cee_boolean block_reduce(CSSParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    return TRUE;
}

/**
 * statement
 */
static void statement_parse(CSSParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    
    if (!current || !current->tokens)
        return;
    
    return;
}

static cee_boolean statement_reduce(CSSParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}

static cee_boolean statement_attach(CSSParser* parser,
                                    CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_append(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref);
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(CSSParser* parser,
                                        CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->statement_current, 
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref);
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}
