#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_gnu_asm.h"

typedef cee_boolean (*ParseTrap)(CEESourceFregment*, 
                                 CEEList**);

typedef enum _GNUASMParserState {
    kGNUASMParserStateStatementParsing = 0x1 << 0,
    kGNUASMParserStateDirectiveParsing = 0x1 << 1,
} GNUASMParserState;

typedef struct _GNUASMParser {
    CEESourceParserRef super;
    const cee_uchar* filepath_ref;
    const cee_uchar* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* prep_directive_root;
    CEESourceFregment* prep_directive_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
    ParseTrap block_header_traps[CEETokenID_MAX];
    GNUASMParserState state;
} GNUASMParser;


static CEETokenIdentifierType gnu_asm_token_identifier_type_map[CEETokenID_MAX];

static GNUASMParser* parser_create(void);
static void parser_free(cee_pointer data);
static void parser_state_set(GNUASMParser* parser,
                             GNUASMParserState state);
static void parser_state_clear(GNUASMParser* parser,
                               GNUASMParserState state);
static void gnu_asm_token_type_map_init(void);
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
static void symbol_parse_init(GNUASMParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
static void symbol_parse_clear(GNUASMParser* parser);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenIdentifierType type);

static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(GNUASMParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(GNUASMParser* parser);
static cee_boolean comment_attach(GNUASMParser* parser);
static cee_boolean prep_directive_parsing(GNUASMParser* parser);
static cee_boolean prep_directive_token_push(GNUASMParser* parser,
                                             CEEToken* push);
static cee_boolean prep_directive_terminated(GNUASMParser* parser);
static void prep_directive_parse(GNUASMParser* parser);
static cee_boolean prep_directive_reduce(GNUASMParser* parser);
static cee_boolean prep_directive_attach(GNUASMParser* parser);
static void statement_parse(GNUASMParser* parser);
static cee_boolean statement_reduce(GNUASMParser* parser);
static cee_boolean statement_parsing(GNUASMParser* parser);
static cee_boolean statement_token_push(GNUASMParser* parser,
                                        CEEToken* push);
static cee_boolean statement_attach(GNUASMParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(GNUASMParser* parser,
                                        CEESourceFregmentType type);
static cee_boolean statement_pop(GNUASMParser* parser);
/**
 * parser
 */
CEESourceParserRef cee_gnu_asm_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    
    GNUASMParser* gnu_asm = parser_create();
    gnu_asm->super = parser;
    
    gnu_asm_token_type_map_init();
    
    parser->imp = gnu_asm;
    
    return parser;
}

void cee_gnu_asm_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static GNUASMParser* parser_create(void)
{
    return cee_malloc0(sizeof(GNUASMParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    GNUASMParser* parser = (GNUASMParser*)data;
    cee_free(parser);
}

static void parser_state_set(GNUASMParser* parser,
                             GNUASMParserState state)
{
    parser->state |= state;
}

static void parser_state_clear(GNUASMParser* parser,
                               GNUASMParserState state)
{
    parser->state &= ~state;
}

static void gnu_asm_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        gnu_asm_token_identifier_type_map[i] = 0;
}

static cee_boolean token_id_is_prep_directive(CEETokenID identifier)
{
    return (gnu_asm_token_identifier_type_map[identifier] & kCEETokenIdentifierTypePrepDirective) != 0;
}

static cee_boolean token_is_directive(const cee_uchar* subject,
                                      CEEToken* token)
{
    cee_boolean ret = FALSE;
    cee_char* str = cee_string_from_token(subject, token);
    if (str) {
        if (!cee_strcmp(str, ".abort", FALSE) ||
            !cee_strcmp(str, ".align", FALSE) ||
            !cee_strcmp(str, ".abort", FALSE) ||
            !cee_strcmp(str, ".align", FALSE) ||
            !cee_strcmp(str, ".altmacro", FALSE) ||
            !cee_strcmp(str, ".ascii", FALSE) ||
            !cee_strcmp(str, ".asciz", FALSE) ||
            !cee_strcmp(str, ".balign", FALSE) ||
            !cee_strcmp(str, ".balignw", FALSE) ||
            !cee_strcmp(str, ".balignl", FALSE) ||
            !cee_strcmp(str, ".bundle_align_mode", FALSE) ||
            !cee_strcmp(str, ".bundle_lock", FALSE) ||
            !cee_strcmp(str, ".bundle_unlock", FALSE) ||
            !cee_strcmp(str, ".byte", FALSE) ||
            !cee_strcmp(str, ".cfi_sections", FALSE) ||
            !cee_strcmp(str, ".cfi_startproc", FALSE) ||
            !cee_strcmp(str, ".cfi_endproc", FALSE) ||
            !cee_strcmp(str, ".cfi_personality", FALSE) ||
            !cee_strcmp(str, ".cfi_personality_id", FALSE) ||
            !cee_strcmp(str, ".cfi_fde_data", FALSE) ||
            !cee_strcmp(str, ".cfi_lsda", FALSE) ||
            !cee_strcmp(str, ".cfi_inline_lsda", FALSE) ||
            !cee_strcmp(str, ".cfi_def_cfa", FALSE) ||
            !cee_strcmp(str, ".cfi_def_cfa_register", FALSE) ||
            !cee_strcmp(str, ".cfi_def_cfa_offset", FALSE) ||
            !cee_strcmp(str, ".cfi_adjust_cfa_offset", FALSE) ||
            !cee_strcmp(str, ".cfi_offset", FALSE) ||
            !cee_strcmp(str, ".cfi_val_offset", FALSE) ||
            !cee_strcmp(str, ".cfi_rel_offset", FALSE) ||
            !cee_strcmp(str, ".cfi_register", FALSE) ||
            !cee_strcmp(str, ".cfi_restore", FALSE) ||
            !cee_strcmp(str, ".cfi_undefined", FALSE) ||
            !cee_strcmp(str, ".cfi_same_value", FALSE) ||
            !cee_strcmp(str, ".cfi_remember_state", FALSE) ||
            !cee_strcmp(str, ".cfi_restore_state", FALSE) ||
            !cee_strcmp(str, ".cfi_return_column", FALSE) ||
            !cee_strcmp(str, ".cfi_signal_frame", FALSE) ||
            !cee_strcmp(str, ".cfi_window_save", FALSE) ||
            !cee_strcmp(str, ".cfi_escape", FALSE) ||
            !cee_strcmp(str, ".cfi_val_encoded_addr", FALSE) ||
            !cee_strcmp(str, ".comm", FALSE) ||
            !cee_strcmp(str, ".data", FALSE) ||
            !cee_strcmp(str, ".dc.a", FALSE) ||
            !cee_strcmp(str, ".dc.b", FALSE) ||
            !cee_strcmp(str, ".dc.d", FALSE) ||
            !cee_strcmp(str, ".dc.l", FALSE) ||
            !cee_strcmp(str, ".dc.s", FALSE) ||
            !cee_strcmp(str, ".dc.w", FALSE) ||
            !cee_strcmp(str, ".dc.x", FALSE) ||
            !cee_strcmp(str, ".dcb.b", FALSE) ||
            !cee_strcmp(str, ".dcb.d", FALSE) ||
            !cee_strcmp(str, ".dcb.l", FALSE) ||
            !cee_strcmp(str, ".dcb.s", FALSE) ||
            !cee_strcmp(str, ".dcb.w", FALSE) ||
            !cee_strcmp(str, ".dcb.x", FALSE) ||
            !cee_strcmp(str, ".ds.b", FALSE) ||
            !cee_strcmp(str, ".ds.d", FALSE) ||
            !cee_strcmp(str, ".ds.l", FALSE) ||
            !cee_strcmp(str, ".ds.p", FALSE) ||
            !cee_strcmp(str, ".ds.s", FALSE) ||
            !cee_strcmp(str, ".ds.w", FALSE) ||
            !cee_strcmp(str, ".ds.x", FALSE) ||
            !cee_strcmp(str, ".def", FALSE) ||
            !cee_strcmp(str, ".desc", FALSE) ||
            !cee_strcmp(str, ".dim", FALSE) ||
            !cee_strcmp(str, ".double", FALSE) ||
            !cee_strcmp(str, ".eject", FALSE) ||
            !cee_strcmp(str, ".else", FALSE) ||
            !cee_strcmp(str, ".elseif", FALSE) ||
            !cee_strcmp(str, ".end", FALSE) ||
            !cee_strcmp(str, ".endef", FALSE) ||
            !cee_strcmp(str, ".endfunc", FALSE) ||
            !cee_strcmp(str, ".endif", FALSE) ||
            !cee_strcmp(str, ".equ", FALSE) ||
            !cee_strcmp(str, ".equiv", FALSE) ||
            !cee_strcmp(str, ".eqv", FALSE) ||
            !cee_strcmp(str, ".err", FALSE) ||
            !cee_strcmp(str, ".error", FALSE) ||
            !cee_strcmp(str, ".exitm", FALSE) ||
            !cee_strcmp(str, ".extern", FALSE) ||
            !cee_strcmp(str, ".fail", FALSE) ||
            !cee_strcmp(str, ".file", FALSE) ||
            !cee_strcmp(str, ".fill", FALSE) ||
            !cee_strcmp(str, ".float", FALSE) ||
            !cee_strcmp(str, ".func", FALSE) ||
            !cee_strcmp(str, ".global", FALSE) ||
            !cee_strcmp(str, ".gnu_attribute", FALSE) ||
            !cee_strcmp(str, ".hidden", FALSE) ||
            !cee_strcmp(str, ".hword", FALSE) ||
            !cee_strcmp(str, ".ident", FALSE) ||
            !cee_strcmp(str, ".ifdef", FALSE) ||
            !cee_strcmp(str, ".ifb", FALSE) ||
            !cee_strcmp(str, ".ifc", FALSE) ||
            !cee_strcmp(str, ".ifeq", FALSE) ||
            !cee_strcmp(str, ".ifeqs", FALSE) ||
            !cee_strcmp(str, ".ifge", FALSE) ||
            !cee_strcmp(str, ".ifgt", FALSE) ||
            !cee_strcmp(str, ".ifle", FALSE) ||
            !cee_strcmp(str, ".iflt", FALSE) ||
            !cee_strcmp(str, ".ifnb", FALSE) ||
            !cee_strcmp(str, ".ifnc", FALSE) ||
            !cee_strcmp(str, ".ifndef", FALSE) ||
            !cee_strcmp(str, ".ifnotdef", FALSE) ||
            !cee_strcmp(str, ".ifne", FALSE) ||
            !cee_strcmp(str, ".ifnes", FALSE) ||
            !cee_strcmp(str, ".incbin", FALSE) ||
            !cee_strcmp(str, ".include", FALSE) ||
            !cee_strcmp(str, ".int", FALSE) ||
            !cee_strcmp(str, ".internal", FALSE) ||
            !cee_strcmp(str, ".irp", FALSE) ||
            !cee_strcmp(str, ".irpc", FALSE) ||
            !cee_strcmp(str, ".lcomm", FALSE) ||
            !cee_strcmp(str, ".lflags", FALSE) ||
            !cee_strcmp(str, ".line", FALSE) ||
            !cee_strcmp(str, ".linkonce", FALSE) ||
            !cee_strcmp(str, ".list", FALSE) ||
            !cee_strcmp(str, ".ln", FALSE) ||
            !cee_strcmp(str, ".loc", FALSE) ||
            !cee_strcmp(str, ".loc_mark_labels", FALSE) ||
            !cee_strcmp(str, ".local", FALSE) ||
            !cee_strcmp(str, ".long", FALSE) ||
            !cee_strcmp(str, ".macro", FALSE) ||
            !cee_strcmp(str, ".mri", FALSE) ||
            !cee_strcmp(str, ".noaltmacro", FALSE) ||
            !cee_strcmp(str, ".nolist", FALSE) ||
            !cee_strcmp(str, ".nops", FALSE) ||
            !cee_strcmp(str, ".octa", FALSE) ||
            !cee_strcmp(str, ".offset", FALSE) ||
            !cee_strcmp(str, ".org", FALSE) ||
            !cee_strcmp(str, ".p2align", FALSE) ||
            !cee_strcmp(str, ".p2alignw", FALSE) ||
            !cee_strcmp(str, ".p2alignl", FALSE) ||
            !cee_strcmp(str, ".popsection", FALSE) ||
            !cee_strcmp(str, ".previous", FALSE) ||
            !cee_strcmp(str, ".print", FALSE) ||
            !cee_strcmp(str, ".protected", FALSE) ||
            !cee_strcmp(str, ".psize", FALSE) ||
            !cee_strcmp(str, ".purgem", FALSE) ||
            !cee_strcmp(str, ".pushsection", FALSE) ||
            !cee_strcmp(str, ".quad", FALSE) ||
            !cee_strcmp(str, ".reloc", FALSE) ||
            !cee_strcmp(str, ".rept", FALSE) ||
            !cee_strcmp(str, ".sbttl", FALSE) ||
            !cee_strcmp(str, ".scl", FALSE) ||
            !cee_strcmp(str, ".section", FALSE) ||
            !cee_strcmp(str, ".set", FALSE) ||
            !cee_strcmp(str, ".short", FALSE) ||
            !cee_strcmp(str, ".single", FALSE) ||
            !cee_strcmp(str, ".size", FALSE) ||
            !cee_strcmp(str, ".skip", FALSE) ||
            !cee_strcmp(str, ".sleb128", FALSE) ||
            !cee_strcmp(str, ".space", FALSE) ||
            !cee_strcmp(str, ".stabd, ", FALSE) ||
            !cee_strcmp(str, ".stabn, ", FALSE) ||
            !cee_strcmp(str, ".stabs", FALSE) ||
            !cee_strcmp(str, ".string", FALSE) ||
            !cee_strcmp(str, ".string8", FALSE) ||
            !cee_strcmp(str, ".string16", FALSE) ||
            !cee_strcmp(str, ".struct", FALSE) ||
            !cee_strcmp(str, ".subsection", FALSE) ||
            !cee_strcmp(str, ".symver", FALSE) ||
            !cee_strcmp(str, ".tag", FALSE) ||
            !cee_strcmp(str, ".text", FALSE) ||
            !cee_strcmp(str, ".title", FALSE) ||
            !cee_strcmp(str, ".type", FALSE) ||
            !cee_strcmp(str, ".uleb128", FALSE) ||
            !cee_strcmp(str, ".val", FALSE) ||
            !cee_strcmp(str, ".version", FALSE) ||
            !cee_strcmp(str, ".vtable_entry", FALSE) ||
            !cee_strcmp(str, ".vtable_inherit", FALSE) ||
            !cee_strcmp(str, ".warning", FALSE) ||
            !cee_strcmp(str, ".weak", FALSE) ||
            !cee_strcmp(str, ".weakref", FALSE) ||
            !cee_strcmp(str, ".word", FALSE) ||
            !cee_strcmp(str, ".zero", FALSE) ||
            !cee_strcmp(str, ".2byte", FALSE) ||
            !cee_strcmp(str, ".4byte", FALSE) ||
            !cee_strcmp(str, ".8byte", FALSE))
            ret = TRUE;
        cee_free(str);
    }
    return ret;
}


static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenIdentifierType type)
{
    return (gnu_asm_token_identifier_type_map[token->identifier] & type) != 0;
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
    GNUASMParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    
    if (!subject)
        return FALSE;
        
    map = cee_source_token_map_create(subject);
    cee_lexer_gnu_asm_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
    
    do {
        ret = cee_lexer_gnu_asm_token_get(&token);
        
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
                
        if (prep_directive_parsing(parser)) {
            prep_directive_token_push(parser, token);
            if (prep_directive_terminated(parser)) {
                prep_directive_parse(parser);
                prep_directive_reduce(parser);
                parser_state_clear(parser, kGNUASMParserStateDirectiveParsing);
            }
        }
        else if (token_id_is_prep_directive(token->identifier)) {
            parser_state_set(parser, kGNUASMParserStateDirectiveParsing);
            prep_directive_token_push(parser, token);
        }
        else if (statement_parsing(parser)) {
            /**
             * It is an error to end any statement with end-of-file,
             * the last character of any input file should be a newline.
             */
            if (cee_token_id_is_newline(token->identifier)) {
                statement_token_push(parser, token);
                statement_parse(parser);
                statement_reduce(parser);
            }
            else {
                statement_token_push(parser, token);
            }
        }
        
        if (!ret)
            break;
        
    } while(1);
        
    *statement = parser->statement_root;
    *prep_directive = parser->prep_directive_root;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_gnu_asm_buffer_free();
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
    return TRUE;
}

static void symbol_parse_init(GNUASMParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    parser->prep_directive_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"gnu_asm");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_root, 
                                                                    kCEESourceFregmentTypeSourceList, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    (const cee_uchar*)"gnu_asm");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_current, 
                                                                    kCEESourceFregmentTypePrepDirective, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    (const cee_uchar*)"gnu_asm");
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        (const cee_uchar*)"gnu_asm");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"gnu_asm");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"gnu_asm");
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      (const cee_uchar*)"gnu_asm");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"gnu_asm");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"gnu_asm");
    parser->state = kGNUASMParserStateStatementParsing;
}

static void symbol_parse_clear(GNUASMParser* parser)
{
    parser->subject_ref = NULL;
    parser->filepath_ref = NULL;
    parser->statement_root = NULL;
    parser->statement_current = NULL;
    parser->prep_directive_root = NULL;
    parser->prep_directive_current = NULL;
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

static cee_boolean comment_token_push(GNUASMParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fregment_reduce(GNUASMParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

static cee_boolean comment_attach(GNUASMParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->comment_current,
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"gnu_asm");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

/**
 *  preprocess directive
 */
static cee_boolean prep_directive_parsing(GNUASMParser* parser)
{
    return parser->state & kGNUASMParserStateDirectiveParsing;
}

static cee_boolean prep_directive_token_push(GNUASMParser* parser,
                                        CEEToken* push)
{
    if (!parser->prep_directive_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->prep_directive_current, push, TRUE);
    return TRUE;
}

static cee_boolean prep_directive_terminated(GNUASMParser* parser)
{
    CEESourceFregment* fregment = parser->prep_directive_current;
    
    if (!fregment->tokens_ref)
        return FALSE;
    
    CEEList* p = SOURCE_FREGMENT_TOKENS_LAST(fregment);
    
    if (cee_token_is_identifier(p, 0))
        return TRUE;
    
    if (cee_token_is_identifier(p, kCEETokenID_NEW_LINE)) {
        p = TOKEN_PREV(p);
        
        if (cee_token_is_identifier(p, '\\'))
            return FALSE;
        
        return TRUE;
    }
    
    return FALSE;
}

static void prep_directive_parse(GNUASMParser* parser)
{
    CEESourceFregment* fregment = parser->prep_directive_current;
    CEEToken* token = NULL;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
}

static cee_boolean prep_directive_reduce(GNUASMParser* parser)
{
    if (!parser->prep_directive_current)
        return FALSE;
    
    prep_directive_attach(parser);
    return TRUE;
}

static cee_boolean prep_directive_attach(GNUASMParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->prep_directive_current,
                                          kCEESourceFregmentTypePrepDirective, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"gnu_asm");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}
/**
 * statement
 */

static void statement_parse(GNUASMParser* parser)
{
    
    CEESourceFregment* current = parser->statement_current;
    
    if (!current || !current->tokens_ref)
        return;
    
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    cee_boolean with_label = FALSE;
    
    return;
}

static cee_boolean statement_reduce(GNUASMParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
        
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}
static cee_boolean statement_parsing(GNUASMParser* parser)
{
    return parser->state & kGNUASMParserStateStatementParsing;
}

static cee_boolean statement_token_push(GNUASMParser* parser,
                                        CEEToken* push)
{
    if (!parser->statement_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->statement_current, push, TRUE);
    
    return TRUE;
}

static cee_boolean statement_attach(GNUASMParser* parser,
                                    CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"c");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(GNUASMParser* parser,
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

static cee_boolean statement_pop(GNUASMParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}

