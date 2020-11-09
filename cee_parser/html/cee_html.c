#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_html.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef struct _HTMLParser {
    CEESourceParserRef super;
    const cee_uchar* filepath_ref;
    const cee_uchar* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
} HTMLParser;

typedef enum _XMLTagType {
    kXMLTagTypeUnknow,
    kXMLTagTypeBegin,
    kXMLTagTypeClose,
    kXMLTagTypeEmpty,
} XMLTagType;

static HTMLParser* parser_create(void);
static void parser_free(cee_pointer data);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_uchar* filepath,
                                const cee_uchar* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static void symbol_parse_init(HTMLParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
static void symbol_parse_clear(HTMLParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(HTMLParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(HTMLParser* parser);
static cee_boolean comment_attach(HTMLParser* parser);
static XMLTagType tag_parse(CEESourceFregment* fregment);
static cee_boolean token_is_empty_tag(CEEToken* token);
static void html_tag_symbols_create(CEESourceFregment* fregment);
static cee_char* attribute_name_get(CEESourceFregment* fregment,
                                    CEEList* p);
static cee_char* attribute_value_get(CEESourceFregment* fregment,
                                     CEEList* p);
static cee_boolean statement_push(HTMLParser* parser,
                                  CEESourceFregment* fregment);
static cee_boolean statement_pop(HTMLParser* parser);
/**
 * parser
 */
CEESourceParserRef cee_html_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    
    HTMLParser* html = parser_create();
    html->super = parser;
    parser->imp = html;
    return parser;
}

void cee_html_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static HTMLParser* parser_create(void)
{
    return cee_malloc0(sizeof(HTMLParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    HTMLParser* parser = (HTMLParser*)data;
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
    HTMLParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    cee_boolean parsing = FALSE;
    
    if (!subject)
        return FALSE;
    
    CEESourceFregment* fregment = NULL;
    
    map = cee_source_token_map_create(subject);
    cee_lexer_html_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
        
    do {
        if (!fregment)
            fregment = cee_source_fregment_create(kCEESourceFregmentTypeXMLTagStart,
                                                  filepath, 
                                                  subject,
                                                  (const cee_uchar*)"html");
        
        ret = cee_lexer_html_token_get(&token);
        
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
                    break;
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
    cee_lexer_html_buffer_free();
    
    return TRUE;
}


static void symbol_parse_init(HTMLParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject)
{    
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        (const cee_uchar*)"html");
    parser->statement_current = parser->statement_root;
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      (const cee_uchar*)"html");    
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"html");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"html");
}

static void symbol_parse_clear(HTMLParser* parser)
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
        
    if (!fregment || !fregment->tokens)
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
            else if (token->identifier == kCEETokenID_IDENTIFIER) {
                if (token_is_empty_tag(token))
                    empty_element = TRUE;
            }
            break;
        }
        
        p = TOKEN_NEXT(p);
    }

    html_tag_symbols_create(fregment);
    
    if (tag_close)
        return kXMLTagTypeClose;
    
    if (empty_element)
        return kXMLTagTypeEmpty;
    
    return kXMLTagTypeBegin;
}

static cee_boolean token_is_empty_tag(CEEToken* token)
{
    cee_boolean ret = FALSE;
    
    if (!token)
        return ret;
    
    CEESourceFregment* fregment = token->fregment_ref;    
    cee_char* name = (cee_char*)cee_strndup((cee_char*)&fregment->subject_ref[token->offset], 
                                            token->length);
    if (!name)
        return ret;
    
    if(!strcmp(name, "area") ||
       !strcmp(name, "base") ||
       !strcmp(name, "br") ||
       !strcmp(name, "col") ||
       !strcmp(name, "embed") ||
       !strcmp(name, "hr") ||
       !strcmp(name, "img") ||
       !strcmp(name, "input") ||
       !strcmp(name, "keygen") ||
       !strcmp(name, "link") ||
       !strcmp(name, "meta") ||
       !strcmp(name, "param") ||
       !strcmp(name, "source") ||
       !strcmp(name, "track") ||
       !strcmp(name, "wbr"))
        ret = TRUE;
        
    cee_free(name);
    return ret;
}

static void html_tag_symbols_create(CEESourceFregment* fregment)
{
    if (!fregment)
        return;
    
    CEEList* p = NULL;
    CEEToken* token = NULL;
    cee_int i = 0;
    CEESourceSymbol* symbol = NULL;
    CEESourceSymbolType type = kCEESourceSymbolTypeUnknow;
    cee_char* identifier = NULL;
    cee_char* class = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        type = kCEESourceSymbolTypeUnknow;
        
        token = p->data;
        
        if (token->identifier == '<') {
            type = kCEESourceSymbolTypeXMLTagStart;
        }
        else if (token->identifier == '>' || token->identifier == '/') {
            type = kCEESourceSymbolTypeXMLTagEnd;
        }
        else if (token->identifier == kCEETokenID_IDENTIFIER) {
            if (i == 0)  {
                type = kCEESourceSymbolTypeXMLTagName;
            }
            else {
                type = kCEESourceSymbolTypeXMLTagAttribute;
                cee_char* name = attribute_name_get(fregment, p);
                if (name) {
                    if (!strcmp(name, "id") && !identifier)
                        identifier = attribute_value_get(fregment, p);
                    else if (!strcmp(name, "class") && !class)
                        class = attribute_value_get(fregment, p);
                    cee_free(name);
                }
            }
            
            i ++;
        }
        
        if (type != kCEESourceSymbolTypeUnknow) {
            symbol = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                               fregment->subject_ref,
                                                               p,
                                                               p,
                                                               type,
                                                               "html");
            fregment->symbols = cee_list_prepend(fregment->symbols, symbol);
        }
        p = TOKEN_NEXT(p);
    }
    
    p = fregment->symbols;
    while (p) {
        symbol = p->data;
        
        if (symbol->type == kCEESourceSymbolTypeXMLTagName) {
            if (symbol->name) {
                cee_char* str = NULL;
                if (identifier)
                    str = cee_strconcat(symbol->name, "#", identifier, NULL);
                else if (class)
                    str = cee_strconcat(symbol->name, ".", class, NULL);
        
                if (str) {
                    cee_free(symbol->name);
                    symbol->name = str;
                }
            }
            break;
        }
        p = p->next;
    }

    if (identifier)
        cee_free(identifier);
    if (class)
        cee_free(class);
    
}

static cee_char* attribute_name_get(CEESourceFregment* fregment,
                                    CEEList* p)
{
    CEEToken* token = p->data;
    if (!token)
        return NULL;
    cee_char* subject_ref = (cee_char*)fregment->subject_ref;
    cee_char* name = cee_strndup(&subject_ref[token->offset], 
                                 token->length);
    return name;
}

static cee_char* attribute_value_get(CEESourceFregment* fregment,
                                     CEEList* p)
{
    CEEToken* token = NULL;
    cee_char* value = NULL;
    cee_char* subject_ref = (cee_char*)fregment->subject_ref;
    
    while (p) {
        token = p->data;
        if (token->identifier == '=') {
            p = cee_token_not_whitespace_newline_after(p);
            if (p) {
                token = p->data;
                value = cee_strndup(&subject_ref[token->offset],
                                    token->length);
                value = cee_strtrim(value, "\"");
                return value;
            }
            else {
                return NULL;
            }
        }
        p = TOKEN_NEXT(p);
    }
        
    return NULL;
}

static cee_boolean token_is_comment(CEEToken* token)
{
    return token->identifier == kCEETokenID_HTML_COMMENT;
}


static cee_boolean comment_token_push(HTMLParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
        
    return TRUE;
}

static cee_boolean comment_fregment_reduce(HTMLParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(HTMLParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_append(parser->comment_current, 
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"html");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

static cee_boolean statement_push(HTMLParser* parser,
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
                                                  (const cee_uchar*)"html");
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

static cee_boolean statement_pop(HTMLParser* parser)
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
