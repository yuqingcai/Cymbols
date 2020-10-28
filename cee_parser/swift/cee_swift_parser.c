#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_swift_parser.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef struct _SwiftParser {
    CEESourceParserRef super;
    const cee_uchar* filepath_ref;
    const cee_uchar* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
} SwiftParser;

static SwiftParser* parser_create(void);
static void parser_free(cee_pointer data);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_uchar* filepath,
                                const cee_uchar* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static void symbol_parse_init(SwiftParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
static void symbol_parse_clear(SwiftParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(SwiftParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(SwiftParser* parser);
static cee_boolean comment_attach(SwiftParser* parser);
/**
 * parser
 */
CEESourceParserRef cee_swift_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    
    SwiftParser* swift = parser_create();
    swift->super = parser;
    
    parser->imp = swift;
    return parser;
}

void cee_swift_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static SwiftParser* parser_create(void)
{
    return cee_malloc0(sizeof(SwiftParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    SwiftParser* parser = (SwiftParser*)data;
    
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
    SwiftParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    
    if (!subject)
        return FALSE;
    
    map = cee_source_token_map_create(subject);
    cee_lexer_swift_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
    
    
    do {
        ret = cee_lexer_swift_token_get(&token);
        
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
        
                
        if (!ret)
            break;
        
    } while(1);
    
    
    *statement = parser->statement_root;
    *prep_directive = NULL;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_swift_buffer_free();
    
    return TRUE;
}

static void symbol_parse_init(SwiftParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject)
{    
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        (const cee_uchar*)"swift");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"swift");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"swift");
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      (const cee_uchar*)"swift");    
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"swift");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"swift");
}

static void symbol_parse_clear(SwiftParser* parser)
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
    return token->identifier == kCEETokenID_LINE_COMMENT ||
            token->identifier == kCEETokenID_LINES_COMMENT;
}

static cee_boolean comment_token_push(SwiftParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fregment_reduce(SwiftParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(SwiftParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_append(parser->comment_current, 
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"swift");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}


