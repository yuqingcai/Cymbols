#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_css.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef struct _CSSParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFragment* statement_root;
    CEESourceFragment* statement_current;
    CEESourceFragment* comment_root;
    CEESourceFragment* comment_current;
} CSSParser;

static CSSParser* parser_create(void);
static void parser_free(cee_pointer data);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFragment** prep_directive,
                                CEESourceFragment** statement,
                                CEESourceFragment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static void symbol_parse_init(CSSParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
static void symbol_parse_clear(CSSParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(CSSParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fragment_reduce(CSSParser* parser);
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
                                    CEESourceFragmentType type);
static cee_boolean statement_sub_attach(CSSParser* parser,
                                        CEESourceFragmentType type);

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
    return parser;
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
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFragment** prep_directive,
                                CEESourceFragment** statement,
                                CEESourceFragment** comment,
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
                              const cee_char* filepath,
                              const cee_char* subject)
{    
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fragment_create(kCEESourceFragmentTypeRoot,
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "css");
    parser->statement_current = cee_source_fragment_sub_attach(parser->statement_root,
                                                               kCEESourceFragmentTypeSourceList,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "css");
    parser->statement_current = cee_source_fragment_sub_attach(parser->statement_current,
                                                               kCEESourceFragmentTypeStatement,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "css");
    
    parser->comment_root = cee_source_fragment_create(kCEESourceFragmentTypeRoot,
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "css");
    parser->comment_current = cee_source_fragment_sub_attach(parser->comment_root,
                                                             kCEESourceFragmentTypeSourceList,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "css");
    parser->comment_current = cee_source_fragment_sub_attach(parser->comment_current,
                                                             kCEESourceFragmentTypeComment,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "css");
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
    return token->identifier == kCEETokenID_LINES_COMMENT;
}

static cee_boolean comment_token_push(CSSParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fragment_reduce(CSSParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(CSSParser* parser)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fragment_attach(parser->comment_current,
                                          kCEESourceFragmentTypeComment,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "css");
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
    CEESourceFragment* fragment = parser->statement_current;
    CEESourceSymbol* symbol = NULL;
    
    if (!fragment || !fragment->tokens_ref)
        return;
    
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    /** skip whitesapce and newline */
    if (cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE) ||
        cee_token_is_identifier(p, kCEETokenID_NEW_LINE))
        p = cee_token_not_whitespace_newline_after(p);
    
    CEEList* q = SOURCE_FREGMENT_TOKENS_LAST(fragment);
    /** skip '{' */
    if (cee_token_is_identifier(q, '{'))
        q = TOKEN_PREV(q);
    /** skip whitesapce and newline */
    if (cee_token_is_identifier(q, kCEETokenID_WHITE_SPACE) ||
        cee_token_is_identifier(q, kCEETokenID_NEW_LINE))
        q = cee_token_not_whitespace_newline_before(q);
    
    if (!p || !q)
        return;
    
    symbol = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                       fragment->subject_ref,
                                                       p, 
                                                       q, 
                                                       kCEESourceSymbolTypeCSSSelector, 
                                                       "css",
                                                       kCEETokenStringOptionCompact);
    fragment->symbols = cee_list_prepend(fragment->symbols, symbol);
    cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeCSSBlock);
    
    return;
}

/**
 * block
 */
static void block_push(CSSParser* parser)
{
    statement_sub_attach(parser, kCEESourceFragmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFragmentTypeStatement);
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
    
    cee_source_fragment_symbols_fragment_range_mark(parser->statement_current);
    statement_attach(parser, kCEESourceFragmentTypeStatement);
    return TRUE;
}

/**
 * statement
 */
static void statement_parse(CSSParser* parser)
{
    CEESourceFragment* current = parser->statement_current;
    
    if (!current || !current->tokens_ref)
        return;
    
    return;
}

static cee_boolean statement_reduce(CSSParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    statement_attach(parser, kCEESourceFragmentTypeStatement);
    
    return TRUE;
}

static cee_boolean statement_attach(CSSParser* parser,
                                    CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fragment_attach(parser->statement_current,
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "css");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(CSSParser* parser,
                                        CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fragment_sub_attach(parser->statement_current,
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              "css");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}
