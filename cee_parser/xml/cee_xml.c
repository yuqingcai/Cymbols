#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_xml.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef struct _XMLParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
} XMLParser;

typedef enum _XMLTagType {
    kXMLTagTypeUnknow,
    kXMLTagTypeBegin,
    kXMLTagTypeClose,
    kXMLTagTypeEmpty,
} XMLTagType;

static CEETokenType xml_token_type_map[CEETokenID_MAX];
static XMLParser* parser_create(void);
static void xml_token_type_map_init(void);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type);
static void parser_free(cee_pointer data);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static void symbol_parse_init(XMLParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
static void symbol_parse_clear(XMLParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(XMLParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(XMLParser* parser);
static cee_boolean comment_attach(XMLParser* parser);
static XMLTagType tag_parse(CEESourceFregment* fregment);
static void xml_tag_symbols_create(CEESourceFregment* fregment);
static cee_boolean statement_push(XMLParser* parser,
                                  CEESourceFregment* fregment);
static cee_boolean statement_pop(XMLParser* parser);

/**
 * parser
 */
CEESourceParserRef cee_xml_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->token_type_matcher = token_type_matcher;
    
    XMLParser* xml = parser_create();
    xml->super = parser;
    parser->imp = xml;
    
    xml_token_type_map_init();
    return parser;
}

static void xml_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        xml_token_type_map[i] = 0;
    xml_token_type_map['<'] = kCEETokenTypePunctuation;
    xml_token_type_map['>'] = kCEETokenTypePunctuation;
    xml_token_type_map['/'] = kCEETokenTypePunctuation;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type)
{
    return (xml_token_type_map[token->identifier] & type) != 0;
}

void cee_xml_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static XMLParser* parser_create(void)
{
    return cee_malloc0(sizeof(XMLParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    XMLParser* parser = (XMLParser*)data;
    cee_free(parser);
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
    XMLParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    cee_boolean parsing = FALSE;
    
    if (!subject)
        return FALSE;
    
    CEESourceFregment* fregment = NULL;
    
    map = cee_source_token_map_create(subject);
    cee_lexer_xml_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
        
    do {
        if (!fregment)
            fregment = cee_source_fregment_create(kCEESourceFregmentTypeXMLTagStart,
                                                  filepath, 
                                                  subject,
                                                  "xml");
        
        ret = cee_lexer_xml_token_get(&token);
                
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
        
        if (token->identifier == '<') {
            SOURCE_FREGMENT_TOKEN_PUSH(fregment, token, TRUE);
            if (!parsing)
                parsing = TRUE;
        }
        else if (token->identifier == '>') {
            SOURCE_FREGMENT_TOKEN_PUSH(fregment, token, TRUE);
            
            if (parsing) {
                
                XMLTagType type = tag_parse(fregment);
                
                if (type == kXMLTagTypeBegin) {
                    if (!statement_push(parser, fregment))
                        break;
                    fregment = NULL;
                }
                else if (type == kXMLTagTypeClose) {
                    cee_source_fregment_type_set_exclusive(fregment,
                                                           kCEESourceFregmentTypeXMLTagEnd);
                    if (!statement_pop(parser))
                        break;
                                
                    if (!statement_push(parser, fregment))
                        break;
                    
                    if (!statement_pop(parser))
                        break;
                    
                    fregment = NULL;
                }
                else if (type == kXMLTagTypeEmpty) {
                    if (!statement_push(parser, fregment))
                        break;
                        
                    if (!statement_pop(parser))
                        break;
                        
                    fregment = NULL;
                }
                else {
                    break;;
                }
                
                parsing = FALSE;
            }
        }
        else {
            if (parsing)
                SOURCE_FREGMENT_TOKEN_PUSH(fregment, token, TRUE);
        }
        
        if (!ret)
            break;
        
    } while(1);
    
    if (fregment) {
        cee_source_fregment_free(fregment);
        CEEList* p = TOKEN_FIRST(tokens);
        while (p) {
            CEEToken* token = p->data;
            if (token->fregment_ref == fregment)
                token->fregment_ref = NULL;
            p = TOKEN_NEXT(p);
        }
        fregment = NULL;
        
    }
    
    *statement = parser->statement_root;
    *prep_directive = NULL;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_xml_buffer_free();
    
    return TRUE;
}

static void symbol_parse_init(XMLParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{    
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "xml");
    parser->statement_current = parser->statement_root;
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "xml");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "xml");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "xml");
}

static void symbol_parse_clear(XMLParser* parser)
{
    parser->subject_ref = NULL;
    parser->filepath_ref = NULL;
    parser->statement_root = NULL;
    parser->statement_current = NULL;
    parser->comment_root = NULL;
    parser->comment_current = NULL;
}

static XMLTagType tag_parse(CEESourceFregment* fregment)
{
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    
    cee_boolean tag_close = FALSE;
    cee_boolean empty_element = FALSE;
        
    if (!fregment || !fregment->tokens_ref)
        return kXMLTagTypeUnknow;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (token->identifier == '<') {
            q = cee_token_not_whitespace_newline_after(p);
            if (!q)
                break;
                
            token = q->data;
            
            if (token->identifier == '/') {
                tag_close = TRUE;
            }
            else {
                q = cee_token_until_identifier(p, '>', FALSE);
                q = cee_token_not_whitespace_newline_before(q);
                if (q) {
                    token = q->data;
                    if (token->identifier == '/')
                        empty_element = TRUE;
                }
            }
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    xml_tag_symbols_create(fregment);
    
    if (tag_close)
        return kXMLTagTypeClose;
    
    if (empty_element)
        return kXMLTagTypeEmpty;
    
    return kXMLTagTypeBegin;
}

static void xml_tag_symbols_create(CEESourceFregment* fregment)
{
    if (!fregment)
        return;
    
    CEEList* p = NULL;
    CEEToken* token = NULL;
    cee_int i = 0;
    CEESourceSymbol* symbol = NULL;
    CEESourceSymbolType type = kCEESourceSymbolTypeUnknow;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        type = kCEESourceSymbolTypeUnknow;
        
        token = p->data;
        
        if (token->identifier == kCEETokenID_IDENTIFIER) {
            if (i == 0)
                type = kCEESourceSymbolTypeXMLTagName;
            else
                type = kCEESourceSymbolTypeXMLTagAttribute;
            i ++;
        }
        
        if (type != kCEESourceSymbolTypeUnknow) {
            symbol = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                               fregment->subject_ref,
                                                               p,
                                                               p,
                                                               type,
                                                               "xml",
                                                               kCEETokenStringOptionCompact);
            fregment->symbols = cee_list_prepend(fregment->symbols, symbol);
        }
        p = TOKEN_NEXT(p);
    }
}

static cee_boolean token_is_comment(CEEToken* token)
{
    return token->identifier == kCEETokenID_HTML_COMMENT;
}


static cee_boolean comment_token_push(XMLParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
        
    return TRUE;
}

static cee_boolean comment_fregment_reduce(XMLParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(XMLParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->comment_current, 
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "xml");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

static cee_boolean statement_push(XMLParser* parser,
                                  CEESourceFregment* fregment)
{
    CEESourceFregment* attached = NULL;
    if (!parser->statement_current)
        return FALSE;
    
    if (!parser->statement_current->children) {
        attached = cee_source_fregment_sub_attach(parser->statement_current, 
                                                  kCEESourceFregmentTypeXMLTagList,
                                                  parser->filepath_ref,
                                                  parser->subject_ref,
                                                  "xml");
        if (!attached)
            return FALSE;

        parser->statement_current = attached;
    }
    else {
        parser->statement_current = SOURCE_FREGMENT_CHILDREN_FIRST(parser->statement_current)->data;
    }
    
    
    SOURCE_FREGMENT_CHILD_APPEND(parser->statement_current, fregment);
    fregment->parent = parser->statement_current;
    parser->statement_current = fregment;
        
    return TRUE;
}

static cee_boolean statement_pop(XMLParser* parser)
{
    /** pop to tag list */
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    parser->statement_current = parser->statement_current->parent;
    
    /** pop to parent tag */
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    parser->statement_current = parser->statement_current->parent;
        
    return TRUE;
}
