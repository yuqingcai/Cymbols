#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_java.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef cee_boolean (*ParseTrap)(CEESourceFregment*, 
                                 CEEList**);

typedef struct _JavaParser {
    CEESourceParserRef super;
    const cee_uchar* filepath_ref;
    const cee_uchar* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
    ParseTrap block_header_traps[CEETokenID_MAX];
} JavaParser;

static CEETokenIdentifierType java_token_identifier_type_map[CEETokenID_MAX];

static JavaParser* parser_create(void);
static void parser_free(cee_pointer data);
static void parser_block_header_trap_init(JavaParser* parser);
static void java_token_type_map_init(void);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenIdentifierType type);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_uchar* filepath,
                                const cee_uchar* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static void symbol_parse_init(JavaParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
static void symbol_parse_clear(JavaParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(JavaParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(JavaParser* parser);
static cee_boolean comment_attach(JavaParser* parser);
static cee_boolean statement_token_push(JavaParser* parser,
                                        CEEToken* push);
static void block_header_parse(JavaParser* parser);
static void block_push(JavaParser* parser);
static cee_boolean block_pop(JavaParser* parser);
static void block_parse(JavaParser* parser);
static cee_boolean statement_pop(JavaParser* parser);
static cee_boolean block_reduce(JavaParser* parser);
static void statement_parse(JavaParser* parser);
static cee_boolean statement_reduce(JavaParser* parser);
static cee_boolean statement_attach(JavaParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(JavaParser* parser,
                                        CEESourceFregmentType type);
static cee_boolean java_class_definition_trap(CEESourceFregment* fregment, 
                                              CEEList** pp);

/**
 * parser
 */
CEESourceParserRef cee_java_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->token_type_matcher = token_type_matcher;
    
    JavaParser* java = parser_create();
    java->super = parser;
    

    parser_block_header_trap_init(java);
    java_token_type_map_init();
    
    parser->imp = java;
    return parser;
}

void cee_java_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static JavaParser* parser_create(void)
{
    return cee_malloc0(sizeof(JavaParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    JavaParser* parser = (JavaParser*)data;
    cee_free(parser);
}

static void parser_block_header_trap_init(JavaParser* parser)
{
    
    for (int i = 0; i < kCEETokenID_END; i ++)
        parser->block_header_traps[i] = NULL;
    
    parser->block_header_traps[kCEETokenID_CLASS] = java_class_definition_trap;
}

static void java_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        java_token_identifier_type_map[i] = 0;
    
    java_token_identifier_type_map[kCEETokenID_ABSTRACT]                    = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_ASSERT]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_BOOLEAN]                     = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_BREAK]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_BYTE]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_CASE]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_CATCH]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_CHAR]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_CLASS]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_CONTINUE]                    = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_CONST]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_DEFAULT]                     = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_DO]                          = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_DOUBLE]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_ELSE]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_ENUM]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_EXPORTS]                     = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_EXTENDS]                     = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_FINAL]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_FINALLY]                     = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_FLOAT]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_FOR]                         = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_GOTO]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_IF]                          = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_IMPLEMENTS]                  = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_IMPORT]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_INSTANCEOF]                  = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_INT]                         = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_INTERFACE]                   = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_LONG]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_MODULE]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_NATIVE]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_NEW]                         = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_PACKAGE]                     = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_PRIVATE]                     = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_PROTECTED]                   = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_PUBLIC]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_REQUIRES]                    = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_RETURN]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_SHORT]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_STATIC]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_STRICTFP]                    = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_SUPER]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_SWITCH]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_SYNCHRONIZED]                = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_THIS]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_THROW]                       = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_THROWS]                      = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_TRANSIENT]                   = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_TRY]                         = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_VAR]                         = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_VOID]                        = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_VOLATILE]                    = kCEETokenIdentifierTypeKeyword;
    java_token_identifier_type_map[kCEETokenID_WHILE]                       = kCEETokenIdentifierTypeKeyword;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenIdentifierType type)
{
    return (java_token_identifier_type_map[token->identifier] & type) != 0;
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
    JavaParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    
    if (!subject)
        return FALSE;
        
    map = cee_source_token_map_create(subject);
    cee_lexer_java_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
    
    do {
        ret = cee_lexer_java_token_get(&token);
        
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
    
    cee_source_fregment_tree_string_print(parser->statement_root);
    
    *statement = parser->statement_root;
    *prep_directive = NULL;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_java_buffer_free();
    
    return TRUE;
}


static void symbol_parse_init(JavaParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject)
{    
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        (const cee_uchar*)"java");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"java");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"java");
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      (const cee_uchar*)"java");    
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"java");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"java");
}

static void symbol_parse_clear(JavaParser* parser)
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
    return token->identifier == kCEETokenID_LINES_COMMENT ||
            token->identifier == kCEETokenID_LINE_COMMENT;
}

static cee_boolean comment_token_push(JavaParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fregment_reduce(JavaParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(JavaParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_append(parser->comment_current, 
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"java");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

static cee_boolean statement_token_push(JavaParser* parser,
                                        CEEToken* push)
{
    if (!parser->statement_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->statement_current, push, TRUE);
    
    return TRUE;
}

static void block_header_parse(JavaParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    ParseTrap trap = NULL;
    
    if (!current || !current->tokens)
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
    
    return;
}

/**
 * block
 */
static void block_push(JavaParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean block_pop(JavaParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void block_parse(JavaParser* parser)
{
}

static cee_boolean statement_pop(JavaParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}


static cee_boolean block_reduce(JavaParser* parser)
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
static void statement_parse(JavaParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    
    if (!current || !current->tokens)
        return;
    
    return;
}

static cee_boolean statement_reduce(JavaParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}

static cee_boolean statement_attach(JavaParser* parser,
                                    CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_append(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"java");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(JavaParser* parser,
                                        CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->statement_current, 
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              (const cee_uchar*)"java");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean java_class_definition_trap(CEESourceFregment* fregment, 
                                              CEEList** pp)
{
//    CEESourceSymbol* definition = c_inheritance_definition_create(*pp, 
//                                                                  fregment->filepath_ref,
//                                                                  fregment->subject_ref);
//    if (!definition)
//        return FALSE;
//    
//    definition->type = kCEESourceSymbolTypeClassDefinition;
//    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
//    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeClassDefinition);
//    *pp = NULL;
//    
//#ifdef DEBUG_CLASS
//    cee_source_symbol_print(definition);
//#endif
    return TRUE;
}
