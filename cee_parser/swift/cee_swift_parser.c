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

static CEETokenIdentifierType swift_token_identifier_type_map[CEETokenID_MAX];

static SwiftParser* parser_create(void);
static void parser_free(cee_pointer data);
static void swift_token_type_map_init(void);
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
static cee_boolean reference_parse(CEESourceParserRef parser_ref,
                                   const cee_uchar* filepath,
                                   const cee_uchar* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFregment* prep_directive,
                                   CEESourceFregment* statement,
                                   CEERange range,
                                   CEEList** references);
static void symbol_parse_init(SwiftParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
static void symbol_parse_clear(SwiftParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(SwiftParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(SwiftParser* parser);
static cee_boolean comment_attach(SwiftParser* parser);
static cee_boolean statement_token_push(SwiftParser* parser,
                                        CEEToken* push);
static void block_push(SwiftParser* parser);
static cee_boolean block_pop(SwiftParser* parser);
static cee_boolean block_reduce(SwiftParser* parser);
static void block_parse(SwiftParser* parser);
static void block_header_parse(SwiftParser* parser);
static void parameter_list_push(SwiftParser* parser);
static cee_boolean parameter_list_pop(SwiftParser* parser);
static void subscript_push(SwiftParser* parser);
static cee_boolean subscript_pop(SwiftParser* parser);
static cee_boolean statement_attach(SwiftParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(SwiftParser* parser,
                                        CEESourceFregmentType type);
static void statement_parse(SwiftParser* parser);
static cee_boolean statement_pop(SwiftParser* parser);
static cee_boolean statement_reduce(SwiftParser* parser);
static cee_boolean statement_complete(SwiftParser* parser);

/**
 * parser
 */
CEESourceParserRef cee_swift_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    
    SwiftParser* swift = parser_create();
    swift->super = parser;
    
    swift_token_type_map_init();
    
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

static void swift_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        swift_token_identifier_type_map[i] = 0;
    
    swift_token_identifier_type_map[kCEETokenID_ASSOCIATEDTYPE]              = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CLASS]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_DEINIT]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_ENUM]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_EXTENSION]                   = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_FILEPRIVATE]                 = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_FUNC]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_IMPORT]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_INIT]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_INOUT]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_INTERNAL]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_LET]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_OPEN]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_OPERATOR]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_PRIVATE]                     = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_PROTOCOL]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_PUBLIC]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_RETHROWS]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_STATIC]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_STRUCT]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_SUBSCRIPT]                   = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_TYPEALIAS]                   = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_VAR]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_BREAK]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CASE]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CONTINUE]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_DEFAULT]                     = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_DEFER]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_DO]                          = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_ELSE]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_FALLTHROUGH]                 = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_FOR]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_GUARD]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_IF]                          = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_IN]                          = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_REPEAT]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_RETURN]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_SWITCH]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_WHERE]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_WHILE]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_AS]                          = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_ANY]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CATCH]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_FALSE]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_IS]                          = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_NIL]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_SUPER]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_SELF]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CAPITAL_SELF]                = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_THROW]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_THROWS]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_TRUE]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_TRY]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_UNDERLINE_DOT]               = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_AT_IDENTIFIER]               = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_AVAILABLE]              = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_COLORLITERAL]           = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_COLUMN]                 = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_ELSE]                   = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_ELSEIF]                 = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_ENDIF]                  = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_ERROR]                  = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_FILE]                   = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_FILEID]                 = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_FILELITERAL]            = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_FILEPATH]               = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_FUNCTION]               = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_IF]                     = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_IMAGELITERAL]           = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_LINE]                   = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_SELECTOR]               = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_SOURCELOCATION]         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_HASH_WARNING]                = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_ASSOCIATIVITY]               = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CONVENIENCE]                 = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_DYNAMIC]                     = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_DIDSET]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_FINAL]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_GET]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_INFIX]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_INDIRECT]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_LAZY]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_LEFT]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_MUTATING]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_NONE]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_NONMUTATING]                 = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_OPTIONAL]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_OVERRIDE]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_POSTFIX]                     = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_PRECEDENCE]                  = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_PREFIX]                      = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CAPITAL_PROTOCOL]            = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_REQUIRED]                    = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_RIGHT]                       = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_SET]                         = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_CAPITAL_TYPE]                = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_UNOWNED]                     = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_WEAK]                        = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_WILLSET]                     = kCEETokenIdentifierTypeKeyword;
    swift_token_identifier_type_map[kCEETokenID_INCREMENT]                   = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_DECREMENT]                   = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['+']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['-']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['!']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['~']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['*']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['/']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['%']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_AND_MULTI]                   = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_AND_DIV]                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_AND_MOD]                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['&']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_AND_ADD]                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_AND_MINUS]                   = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['|']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['^']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_HALF_OPEN_RANGE]             = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_ELLIPSIS]                    = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['<']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_LESS_EQUAL]            = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['>']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]           = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_EQUAL]                 = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_UNEQUAL]               = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_IDENTICALLY_EQUAL]     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_IDENTICALLY_UNEQUAL]   = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_PATTERN_MATCH]               = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_AND]                   = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LOGIC_OR]                    = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_NIL_COALESCING]              = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['=']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_MULTI_ASSIGNMENT]            = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_DIV_ASSIGNMENT]              = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_MOD_ASSIGNMENT]              = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_ADD_ASSIGNMENT]              = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_MINUS_ASSIGNMENT]            = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LEFT_SHIFT_ASSIGNMENT]       = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_RIGHT_SHIFT_ASSIGNMENT]      = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_AND_ASSIGNMENT]              = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_XOR_ASSIGNMENT]              = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_OR_ASSIGNMENT]               = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['.']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['[']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[']']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['(']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[')']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['{']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map['}']                                     = kCEETokenIdentifierTypePunctuation;
    swift_token_identifier_type_map[kCEETokenID_LINES_COMMENT]               = kCEETokenIdentifierTypeComment;
    swift_token_identifier_type_map[kCEETokenID_LINE_COMMENT]                = kCEETokenIdentifierTypeComment;
    swift_token_identifier_type_map[kCEETokenID_LITERAL]                     = kCEETokenIdentifierTypeLiteral;
    swift_token_identifier_type_map[kCEETokenID_MULTI_LITERAL]               = kCEETokenIdentifierTypeLiteral;
    swift_token_identifier_type_map[kCEETokenID_CONSTANT]                    = kCEETokenIdentifierTypeConstant;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenIdentifierType type)
{
    return (swift_token_identifier_type_map[token->identifier] & type) != 0;
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
        else if (token->identifier == ';') {
            statement_token_push(parser, token);
            statement_parse(parser);
            if (!statement_reduce(parser))
                break;
        }
        else if (token->identifier == kCEETokenID_NEW_LINE) {
            statement_token_push(parser, token);
            //if (statement_complete(parser)) {
                statement_parse(parser);
                if (!statement_reduce(parser))
                    break;
            //}
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
    cee_lexer_swift_buffer_free();
    
    
    return TRUE;
}

static cee_boolean reference_parse(CEESourceParserRef parser_ref,
                                   const cee_uchar* filepath,
                                   const cee_uchar* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFregment* prep_directive,
                                   CEESourceFregment* statement,
                                   CEERange range,
                                   CEEList** references)
{
    return FALSE;
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

static cee_boolean statement_token_push(SwiftParser* parser,
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
static void block_push(SwiftParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean block_pop(SwiftParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static cee_boolean block_reduce(SwiftParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    return TRUE;
}


static void block_parse(SwiftParser* parser)
{
}

static void block_header_parse(SwiftParser* parser)
{
    
}

/**
 * parameter list
 */
static void parameter_list_push(SwiftParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeRoundBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean parameter_list_pop(SwiftParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

/**
 * subscript
 */
static void subscript_push(SwiftParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeSquareBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean subscript_pop(SwiftParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

/**
* statement
*/
static cee_boolean statement_attach(SwiftParser* parser,
                                    CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_append(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"c");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(SwiftParser* parser,
                                        CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->statement_current, 
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              (const cee_uchar*)"c");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static void statement_parse(SwiftParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    
    if (!current || !current->tokens)
        return;
        
    return;
}

static cee_boolean statement_pop(SwiftParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}

static cee_boolean statement_reduce(SwiftParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
       
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}

static cee_boolean statement_complete(SwiftParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    CEESourceFregment* current = parser->statement_current;
    CEEList* p = SOURCE_FREGMENT_TOKENS_LAST(current);
    CEEToken* token = NULL;
    
    if (token->identifier == ';') {
        return TRUE;
    }
    else if (token->identifier == kCEETokenID_NEW_LINE) {
        p = cee_token_not_whitespace_newline_before(p);
        if (p) {
            token = p->data;
            
            //Assignment Operator
            //Arithmetic Operator
            //Remainder Operator
            //Compound Assignment Operator
            //Comparison Operator
            //Ternary Conditional Operator
            //Nil-Coalescing Operator
            //Closed Range Operator
            //Half-Open Range Operator
            //Logical Operator
            //Bitwise Operator
            
            if (token->identifier == ',')
                return FALSE;
        }
    }
    return TRUE;
}

