#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_swift.h"
#include "cee_symbol.h"
#include "cee_reference.h"

typedef struct _SwiftParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
} SwiftParser;

static CEETokenType swift_token_type_map[CEETokenID_MAX];
static SwiftParser* parser_create(void);
static void parser_free(cee_pointer data);
static void swift_token_type_map_init(void);
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
static void symbol_parse_init(SwiftParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
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
static cee_boolean current_statement_type_is(SwiftParser* parser,
                                             CEESourceFregmentType type);
static void current_statement_type_transform(SwiftParser* parser,
                                             CEESourceFregmentType type);
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

/**
 * parser
 */
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
        swift_token_type_map[i] = 0;
    
    swift_token_type_map[kCEETokenID_ASSOCIATEDTYPE]              = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CLASS]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_DEINIT]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_ENUM]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_EXTENSION]                   = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_FILEPRIVATE]                 = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_FUNC]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_IMPORT]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_INIT]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_INOUT]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_INTERNAL]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_LET]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_OPEN]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_OPERATOR]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_PRIVATE]                     = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_PROTOCOL]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_PUBLIC]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_RETHROWS]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_STATIC]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_STRUCT]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_SUBSCRIPT]                   = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_TYPEALIAS]                   = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_VAR]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_BREAK]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CASE]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CONTINUE]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_DEFAULT]                     = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_DEFER]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_DO]                          = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_ELSE]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_FALLTHROUGH]                 = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_FOR]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_GUARD]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_IF]                          = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_IN]                          = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_REPEAT]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_RETURN]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_SWITCH]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_WHERE]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_WHILE]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_AS]                          = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_ANY]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CATCH]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_FALSE]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_IS]                          = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_NIL]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_SUPER]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_SELF]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CAPITAL_SELF]                = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_THROW]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_THROWS]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_TRUE]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_TRY]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_UNDERLINE_DOT]               = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_AT_IDENTIFIER]               = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_AVAILABLE]              = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_COLORLITERAL]           = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_COLUMN]                 = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_ELSE]                   = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_ELSEIF]                 = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_ENDIF]                  = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_ERROR]                  = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_FILE]                   = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_FILEID]                 = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_FILELITERAL]            = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_FILEPATH]               = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_FUNCTION]               = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_IF]                     = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_IMAGELITERAL]           = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_LINE]                   = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_SELECTOR]               = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_SOURCELOCATION]         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_HASH_WARNING]                = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_ASSOCIATIVITY]               = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CONVENIENCE]                 = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_DYNAMIC]                     = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_DIDSET]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_FINAL]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_GET]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_INFIX]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_INDIRECT]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_LAZY]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_LEFT]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_MUTATING]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_NONE]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_NONMUTATING]                 = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_OPTIONAL]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_OVERRIDE]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_POSTFIX]                     = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_PRECEDENCE]                  = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_PREFIX]                      = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CAPITAL_PROTOCOL]            = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_REQUIRED]                    = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_RIGHT]                       = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_SET]                         = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_CAPITAL_TYPE]                = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_UNOWNED]                     = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_WEAK]                        = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_WILLSET]                     = kCEETokenTypeKeyword;
    swift_token_type_map[kCEETokenID_INCREMENT]                   = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_DECREMENT]                   = kCEETokenTypePunctuation;
    swift_token_type_map['+']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['-']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['!']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['~']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['*']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['/']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['%']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_AND_MULTI]                   = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_AND_DIV]                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_AND_MOD]                     = kCEETokenTypePunctuation;
    swift_token_type_map['&']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_AND_ADD]                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_AND_MINUS]                   = kCEETokenTypePunctuation;
    swift_token_type_map['|']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['^']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_HALF_OPEN_RANGE]             = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_ELLIPSIS]                    = kCEETokenTypePunctuation;
    swift_token_type_map['<']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_LESS_EQUAL]            = kCEETokenTypePunctuation;
    swift_token_type_map['>']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]           = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_EQUAL]                 = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_UNEQUAL]               = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_IDENTICALLY_EQUAL]     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_IDENTICALLY_UNEQUAL]   = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_PATTERN_MATCH]               = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_AND]                   = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LOGIC_OR]                    = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_NIL_COALESCING]              = kCEETokenTypePunctuation;
    swift_token_type_map['=']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_MULTI_ASSIGNMENT]            = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_DIV_ASSIGNMENT]              = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_MOD_ASSIGNMENT]              = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_ADD_ASSIGNMENT]              = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_MINUS_ASSIGNMENT]            = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LEFT_SHIFT_ASSIGNMENT]       = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_RIGHT_SHIFT_ASSIGNMENT]      = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_AND_ASSIGNMENT]              = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_XOR_ASSIGNMENT]              = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_OR_ASSIGNMENT]               = kCEETokenTypePunctuation;
    swift_token_type_map['.']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['[']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[']']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['(']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[')']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['{']                                     = kCEETokenTypePunctuation;
    swift_token_type_map['}']                                     = kCEETokenTypePunctuation;
    swift_token_type_map[kCEETokenID_LINES_COMMENT]               = kCEETokenTypeComment;
    swift_token_type_map[kCEETokenID_LINE_COMMENT]                = kCEETokenTypeComment;
    swift_token_type_map[kCEETokenID_LITERAL]                     = kCEETokenTypeLiteral;
    swift_token_type_map[kCEETokenID_MULTI_LITERAL]               = kCEETokenTypeLiteral;
    swift_token_type_map[kCEETokenID_CONSTANT]                    = kCEETokenTypeConstant;
    swift_token_type_map[kCEETokenID_IDENTIFIER]                  = kCEETokenTypeIdentifier;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type)
{
    return (swift_token_type_map[token->identifier] & type) != 0;
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
    SwiftParser* parser = parser_ref->imp;
    cee_int ret = 0;
    cee_boolean pushed = TRUE;
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
        
        pushed = FALSE;
        
        if (current_statement_type_is(parser, kCEESourceFregmentTypeDeclaration)) {
            //pushed = import_statement_token_push(parser, token);
        }
        else if (current_statement_type_is(parser, kCEESourceFregmentTypeClassDefinition)) {
            //pushed = class_definition_statement_token_push(parser, token);
        }
        else if (current_statement_type_is(parser, kCEESourceFregmentTypeFunctionDefinition)) {
            //pushed = function_definition_statement_token_push(parser, token);
        }
        
        if (!pushed) {
            if (token->identifier == kCEETokenID_IMPORT) {
                current_statement_type_transform(parser, kCEESourceFregmentTypeDeclaration);
                statement_token_push(parser, token);
            }
            else if (token->identifier == kCEETokenID_CLASS ||
                     token->identifier == kCEETokenID_STRUCT) {
                current_statement_type_transform(parser, kCEESourceFregmentTypeClassDefinition);
                statement_token_push(parser, token);
            }
            else if (token->identifier == kCEETokenID_FUNC) {
                current_statement_type_transform(parser, kCEESourceFregmentTypeFunctionDefinition);
                statement_token_push(parser, token);
            }
            else if (token->identifier == '{') { 
                statement_token_push(parser, token);
                block_push(parser);
            }
            else if (token->identifier == '}') {
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
            else {
                statement_token_push(parser, token);
            }
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
                                   const cee_char* filepath,
                                   const cee_char* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFregment* prep_directive,
                                   CEESourceFregment* statement,
                                   CEERange range,
                                   CEEList** references)
{
    return FALSE;
}

static void symbol_parse_init(SwiftParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{    
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "swift");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "swift");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "swift");
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "swift");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "swift");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "swift");
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
    
    attached = cee_source_fregment_attach(parser->comment_current,
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "swift");
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
    
    attached = cee_source_fregment_attach(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "swift");
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
                                              "swift");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static void statement_parse(SwiftParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    
    if (!current || !current->tokens_ref)
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

static cee_boolean current_statement_type_is(SwiftParser* parser,
                                             CEESourceFregmentType type)
{
    if (parser->statement_current)
        return cee_source_fregment_type_is(parser->statement_current, type);
        
    return FALSE;
}

static void current_statement_type_transform(SwiftParser* parser,
                                             CEESourceFregmentType type)
{
    cee_source_fregment_type_set(parser->statement_current, type);
}

