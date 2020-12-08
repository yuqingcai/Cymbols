#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_gnu_asm.h"

typedef enum _GNUASMPrepDirectiveIncludeTranslateState {
    kGNUASMPrepDirectiveIncludeTranslateStateInit = 0,
    kGNUASMPrepDirectiveIncludeTranslateStateDirective,
    kGNUASMPrepDirectiveIncludeTranslateStatePath,
    kGNUASMPrepDirectiveIncludeTranslateStateCommit,
    kGNUASMPrepDirectiveIncludeTranslateStateError,
    kGNUASMPrepDirectiveIncludeTranslateStateMax,
} GNUASMPrepDirectiveIncludeTranslateState;

typedef enum _GNUASMPrepDirectiveDefineTranslateState {
    kGNUASMPrepDirectiveDefineTranslateStateInit = 0,
    kGNUASMPrepDirectiveDefineTranslateStateDirective,
    kGNUASMPrepDirectiveDefineTranslateStateDefineName,
    kGNUASMPrepDirectiveDefineTranslateStateParameterList,
    kGNUASMPrepDirectiveDefineTranslateStateParameter,
    kGNUASMPrepDirectiveDefineTranslateStateCommit,
    kGNUASMPrepDirectiveDefineTranslateStateError,
    kGNUASMPrepDirectiveDefineTranslateStateMax,
} GNUASMPrepDirectiveDefineTranslateState;

typedef cee_boolean (*ParseTrap)(CEESourceFregment*, 
                                 CEEList**);

typedef enum _GNUASMParserState {
    kGNUASMParserStateStatementParsing = 0x1 << 0,
    kGNUASMParserStatePrepDirectiveParsing = 0x1 << 1,
} GNUASMParserState;

typedef struct _GNUASMParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* prep_directive_root;
    CEESourceFregment* prep_directive_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
    ParseTrap block_header_traps[CEETokenID_MAX];
    GNUASMParserState state;
} GNUASMParser;

static CEETokenType gnu_asm_token_type_map[CEETokenID_MAX];
static cee_int gnu_asm_prep_directive_include_translate_table[kGNUASMPrepDirectiveIncludeTranslateStateMax][CEETokenID_MAX];
static cee_int gnu_asm_prep_directive_define_translate_table[kGNUASMPrepDirectiveDefineTranslateStateMax][CEETokenID_MAX];

static void gnu_asm_token_type_map_init(void);
static GNUASMParser* parser_create(void);
static void parser_free(cee_pointer data);
static void parser_state_set(GNUASMParser* parser,
                             GNUASMParserState state);
static void parser_state_clear(GNUASMParser* parser,
                               GNUASMParserState state);
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
static cee_boolean references_in_source_fregment_parse(const cee_char* filepath,
                                                       CEESourceFregment* fregment,
                                                       const cee_char* subject,
                                                       CEESourceFregment* prep_directive,
                                                       CEESourceFregment* statement,
                                                       CEERange range,
                                                       CEEList** references);
static CEESourceFregment* gnu_asm_referernce_fregment_convert(CEESourceFregment* fregment,
                                                              const cee_char* subject);
static void gnu_asm_reference_fregment_parse(const cee_char* filepath,
                                             CEESourceFregment* fregment,
                                             const cee_char* subject,
                                             CEESourceFregment* prep_directive,
                                             CEESourceFregment* statement,
                                             CEEList** references);
static void symbol_parse_init(GNUASMParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
static void symbol_parse_clear(GNUASMParser* parser);
static CEEList* skip_include_path(CEEList* p);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type);

/** comment */
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_attach(GNUASMParser* parser);
static cee_boolean comment_token_push(GNUASMParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(GNUASMParser* parser);

/** prep directive */
static cee_boolean prep_directive_parsing(GNUASMParser* parser);
static cee_boolean prep_directive_token_push(GNUASMParser* parser,
                                             CEEToken* push);
static cee_boolean prep_directive_terminated(GNUASMParser* parser);
static cee_boolean prep_directive_attach(GNUASMParser* parser,
                                         CEESourceFregmentType type);
static cee_boolean prep_directive_sub_attach(GNUASMParser* parser,
                                             CEESourceFregmentType type);
static void prep_directive_branch_push(GNUASMParser* parser);
static cee_boolean prep_directive_branch_pop(GNUASMParser* parser);
static cee_boolean prep_directive_pop(GNUASMParser* parser);
static void prep_directive_parse(GNUASMParser* parser);
static cee_boolean should_statement_parsing(GNUASMParser* parser);
static void gnu_asm_prep_directive_include_translate_table_init(void);
static cee_boolean prep_directive_include_parse(CEESourceFregment* fregment);
static void gnu_asm_prep_directive_define_translate_table_init(void);
static cee_boolean prep_directive_define_parse(CEESourceFregment* fregment);
static cee_boolean prep_directive_common_parse(CEESourceFregment* fregment);

/** statement */
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
    gnu_asm_prep_directive_include_translate_table_init();
    gnu_asm_prep_directive_define_translate_table_init();
    
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
        gnu_asm_token_type_map[i] = 0;
    
    gnu_asm_token_type_map[kCEETokenID_HASH_INCLUDE]                 = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map[kCEETokenID_HASH_IMPORT]                  = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map[kCEETokenID_HASH_DEFINE]                  = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map[kCEETokenID_HASH_UNDEF]                   = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map[kCEETokenID_HASH_IF]                      = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    gnu_asm_token_type_map[kCEETokenID_HASH_IFDEF]                   = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    gnu_asm_token_type_map[kCEETokenID_HASH_IFNDEF]                  = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    gnu_asm_token_type_map[kCEETokenID_HASH_ENDIF]                   = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    gnu_asm_token_type_map[kCEETokenID_HASH_ELSE]                    = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    gnu_asm_token_type_map[kCEETokenID_HASH_ELIF]                    = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    gnu_asm_token_type_map[kCEETokenID_HASH_LINE]                    = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map[kCEETokenID_HASH_ERROR]                   = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map[kCEETokenID_HASH_WARNING]                 = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map[kCEETokenID_HASH_PRAGMA]                  = kCEETokenTypePrepDirective;
    gnu_asm_token_type_map['=']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['+']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['-']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['*']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['/']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['\\']                                     = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['%']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['~']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['&']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['|']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['^']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['!']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['<']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['>']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['.']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map[',']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['?']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map[':']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map['(']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map[')']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map[';']                                      = kCEETokenTypePunctuation;
    gnu_asm_token_type_map[kCEETokenID_LINE_COMMENT]                 = kCEETokenTypeComment;
    gnu_asm_token_type_map[kCEETokenID_LINES_COMMENT]                = kCEETokenTypeComment;
    gnu_asm_token_type_map[kCEETokenID_CONSTANT]                     = kCEETokenTypeConstant;
    gnu_asm_token_type_map[kCEETokenID_CHARACTER]                    = kCEETokenTypeCharacter;
    gnu_asm_token_type_map[kCEETokenID_LITERAL]                      = kCEETokenTypeLiteral;
    gnu_asm_token_type_map[kCEETokenID_IDENTIFIER]                   = kCEETokenTypeIdentifier;

}

static cee_boolean token_id_is_prep_directive(CEETokenID identifier)
{
    return (gnu_asm_token_type_map[identifier] & kCEETokenTypePrepDirective) != 0;
}

static cee_boolean token_id_is_punctuation(CEETokenID identifier)
{
    return (gnu_asm_token_type_map[identifier] & kCEETokenTypePunctuation) != 0;
}

static cee_boolean token_is_directive(const cee_char* subject,
                                      CEEToken* token)
{
    cee_boolean ret = FALSE;
    cee_char* str = cee_string_from_token(subject, token);
    if (str) {
        if (!cee_strcmp(str, ".abort", FALSE) ||
            !cee_strcmp(str, ".align", FALSE) ||
            !cee_strcmp(str, ".abort", FALSE) ||
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
            !cee_strcmp(str, ".endm", FALSE) ||
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
            !cee_strcmp(str, ".globl", FALSE) ||
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
            !cee_strcmp(str, ".8byte", FALSE) ||
            /** ARM Assembler Directives */
            !cee_strcmp(str, ".arm", FALSE) ||
            !cee_strcmp(str, ".pad", FALSE) ||
            !cee_strcmp(str, ".bss", FALSE) ||
            !cee_strcmp(str, ".cantunwind", FALSE) ||
            !cee_strcmp(str, ".code", FALSE) ||
            !cee_strcmp(str, ".cpu", FALSE) ||
            !cee_strcmp(str, ".dn", FALSE) ||
            !cee_strcmp(str, ".qn", FALSE) ||
            !cee_strcmp(str, ".eabi_attribute", FALSE) ||
            !cee_strcmp(str, ".even", FALSE) ||
            !cee_strcmp(str, ".extend", FALSE) ||
            !cee_strcmp(str, ".ldouble", FALSE) ||
            !cee_strcmp(str, ".fnend", FALSE) ||
            !cee_strcmp(str, ".fnstart", FALSE) ||
            !cee_strcmp(str, ".force_thumb", FALSE) ||
            !cee_strcmp(str, ".fpu", FALSE) ||
            !cee_strcmp(str, ".handlerdata", FALSE) ||
            !cee_strcmp(str, ".inst", FALSE) ||
            !cee_strcmp(str, ".inst.n", FALSE) ||
            !cee_strcmp(str, ".inst.w", FALSE) ||
            !cee_strcmp(str, ".ltorg", FALSE) ||
            !cee_strcmp(str, ".movsp", FALSE) ||
            !cee_strcmp(str, ".object_arch", FALSE) ||
            !cee_strcmp(str, ".packed", FALSE) ||
            !cee_strcmp(str, ".personality", FALSE) ||
            !cee_strcmp(str, ".personalityindex", FALSE) ||
            !cee_strcmp(str, ".pool", FALSE) ||
            !cee_strcmp(str, ".req", FALSE) ||
            !cee_strcmp(str, ".save", FALSE) ||
            !cee_strcmp(str, ".setfp", FALSE) ||
            !cee_strcmp(str, ".secrel32", FALSE) ||
            !cee_strcmp(str, ".syntax", FALSE) ||
            !cee_strcmp(str, ".thumb", FALSE) ||
            !cee_strcmp(str, ".thumb_func", FALSE) ||
            !cee_strcmp(str, ".thumb_set", FALSE) ||
            !cee_strcmp(str, ".unreq", FALSE) ||
            !cee_strcmp(str, ".unwind_raw", FALSE) ||
            !cee_strcmp(str, ".vsave", FALSE)
            )
            ret = TRUE;
        cee_free(str);
    }
    return ret;
}

static CEEList* skip_include_path(CEEList* p)
{
    cee_int angle_bracket = 0;
    CEEToken* token = NULL;
    
    while (p) {
        token = p->data;
        
        if (token->identifier == '<')
            angle_bracket ++;
        else if (token->identifier == '>')
            angle_bracket --;
        
        if (!angle_bracket)
            return p;
    
        p = TOKEN_NEXT(p);
    }
    return p;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type)
{
    if (type & kCEETokenTypeASMDirective) {
        if (token->identifier == kCEETokenID_IDENTIFIER) {
            CEESourceFregment* fregment = token->fregment_ref;
            return token_is_directive(fregment->subject_ref, token);
        }
        else
            return FALSE;
    }
    
    return (gnu_asm_token_type_map[token->identifier] & type) != 0;
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
                parser_state_clear(parser, kGNUASMParserStatePrepDirectiveParsing);
            }
        }
        else if (token_id_is_prep_directive(token->identifier)) {
            parser_state_set(parser, kGNUASMParserStatePrepDirectiveParsing);
            if (token->identifier == kCEETokenID_HASH_ELIF ||
                token->identifier == kCEETokenID_HASH_ELSE ||
                token->identifier == kCEETokenID_HASH_ENDIF) {
                if (!prep_directive_branch_pop(parser))
                    parser_state_clear(parser, kGNUASMParserStatePrepDirectiveParsing);
                
                prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
                prep_directive_token_push(parser, token);
            }
            else {
                prep_directive_token_push(parser, token);
            }
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
        else {
            token->state |= kCEETokenStateIgnore;
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
        
        p = indexes[i]->node;
        while (p) {
            fregment = p->data;
            
            if (!references_in_source_fregment_parse(filepath,
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

static cee_boolean references_in_source_fregment_parse(const cee_char* filepath,
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
    
    reference_fregment = gnu_asm_referernce_fregment_convert(fregment, subject);
    gnu_asm_reference_fregment_parse(filepath,
                                     reference_fregment,
                                     subject,
                                     prep_directive,
                                     statement,
                                     references);
    cee_source_fregment_free(reference_fregment);
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        if (!references_in_source_fregment_parse(filepath,
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

static CEESourceFregment* gnu_asm_referernce_fregment_convert(CEESourceFregment* fregment,
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
                                                        "gnu_asm");
        reference_fregment = cee_source_fregment_sub_attach(reference_fregment,
                                                            kCEESourceFregmentTypeStatement,
                                                            fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            "gnu_asm");
    }
    else {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeStatement,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        "gnu_asm");
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
                                                   "gnu_asm");
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
                                                   "gnu_asm");
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
                                                   "gnu_asm");
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

static void gnu_asm_reference_fregment_parse(const cee_char* filepath,
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
        if (cee_source_fregment_tokens_pattern_match(fregment, p, '.', kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeUnknow;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch any other identifier */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(0, 1), &sub);
            type = kCEESourceReferenceTypeUnknow;
        }
        else {
            p = TOKEN_NEXT(p);
        }
        
        if (sub) {
            reference = cee_source_symbol_reference_create((const cee_char*)filepath,
                                                           subject,
                                                           sub,
                                                           type);
            *references = cee_list_prepend(*references, reference);
            type = kCEESourceReferenceTypeUnknow;
            reference = NULL;
            sub = NULL;
        }
    }
    
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        gnu_asm_reference_fregment_parse(filepath,
                                         p->data,
                                         subject,
                                         prep_directive,
                                         statement,
                                         references);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

static void symbol_parse_init(GNUASMParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->prep_directive_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "gnu_asm");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_root, 
                                                                    kCEESourceFregmentTypeSourceList, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    "gnu_asm");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_current, 
                                                                    kCEESourceFregmentTypePrepDirective, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    "gnu_asm");
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "gnu_asm");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "gnu_asm");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "gnu_asm");
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "gnu_asm");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "gnu_asm");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "gnu_asm");
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

static cee_boolean comment_attach(GNUASMParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->comment_current,
                                          kCEESourceFregmentTypeComment,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "gnu_asm");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
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

/**
 *  preprocess directive
 */
static cee_boolean prep_directive_parsing(GNUASMParser* parser)
{
    return parser->state & kGNUASMParserStatePrepDirectiveParsing;
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

static cee_boolean prep_directive_attach(GNUASMParser* parser,
                                         CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->prep_directive_current,
                                          type,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "gnu_asm");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static cee_boolean prep_directive_sub_attach(GNUASMParser* parser,
                                             CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->prep_directive_current,
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              "gnu_asm");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static void prep_directive_branch_push(GNUASMParser* parser)
{
    prep_directive_sub_attach(parser, kCEESourceFregmentTypeSourceList);
    prep_directive_sub_attach(parser, kCEESourceFregmentTypePrepDirective);
}

static cee_boolean prep_directive_branch_pop(GNUASMParser* parser)
{
    if (prep_directive_pop(parser))
        return prep_directive_pop(parser);
    
    return FALSE;
}

static cee_boolean prep_directive_pop(GNUASMParser* parser)
{
    if (!parser->prep_directive_current ||
        !parser->prep_directive_current->parent)
        return FALSE;
    
    parser->prep_directive_current =
        parser->prep_directive_current->parent;
    return TRUE;
}

static void prep_directive_parse(GNUASMParser* parser)
{
    CEESourceFregment* fregment = parser->prep_directive_current;
    CEEToken* token = NULL;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
        
    while (p) {
        token = p->data;
        if (token_id_is_prep_directive(token->identifier))
            break;
        p = TOKEN_NEXT(p);
    }
    
    if (p) {
        token = p->data;
        
        if (token->identifier == kCEETokenID_HASH_IF ||
            token->identifier == kCEETokenID_HASH_IFDEF ||
            token->identifier == kCEETokenID_HASH_IFNDEF) {
            if (should_statement_parsing(parser))
                parser_state_set(parser, kGNUASMParserStateStatementParsing);
            else
                parser_state_clear(parser, kGNUASMParserStateStatementParsing);
            cee_source_fregment_type_set(parser->prep_directive_current,
                                         kCEESourceFregmentTypePrepDirectiveCondition);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ELIF ||
                 token->identifier == kCEETokenID_HASH_ELSE) {
            parser_state_clear(parser, kGNUASMParserStateStatementParsing);
            cee_source_fregment_type_set(parser->prep_directive_current,
                                         kCEESourceFregmentTypePrepDirectiveBranch);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ENDIF) {
            prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
            if (should_statement_parsing(parser))
                parser_state_set(parser, kGNUASMParserStateStatementParsing);
            else
                parser_state_clear(parser, kGNUASMParserStateStatementParsing);
        }
        else {
            if (token->identifier == kCEETokenID_HASH_INCLUDE ||
                token->identifier == kCEETokenID_HASH_IMPORT) {
                prep_directive_include_parse(fregment);
            }
            else if (token->identifier == kCEETokenID_HASH_DEFINE) {
                prep_directive_define_parse(fregment);
            }
            else {
                prep_directive_common_parse(fregment);
            }
            prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
        }
    }
}

static cee_boolean should_statement_parsing(GNUASMParser* parser)
{
    cee_boolean ret = TRUE;
    CEESourceFregment* current = parser->prep_directive_current->parent;
    while (current) {
        if (cee_source_fregment_type_is(current, kCEESourceFregmentTypeRoot)) {
            break;
        }
        else if (cee_source_fregment_type_is(current, kCEESourceFregmentTypePrepDirectiveBranch)) {
            ret = FALSE;
            break;
        }
        current = current->parent;
    }
    return ret;
}

static void gnu_asm_prep_directive_include_translate_table_init(void)
{
    
    /**
     *                      include             import              identifier  literal  \                  <           >       others
     *  Init                IncludeDirective    IncludeDirective    Error       Error    Error              Error       Error   Error
     *  IncludeDirective    Error               Error               Commit      Commit   IncludeDirective   *PathBegin  Error   Error
     *
     *  PathBegin, skip to Commit
     */
    TRANSLATE_STATE_INI(gnu_asm_prep_directive_include_translate_table, kGNUASMPrepDirectiveIncludeTranslateStateMax        , kGNUASMPrepDirectiveIncludeTranslateStateError                                    );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_include_translate_table, kGNUASMPrepDirectiveIncludeTranslateStateInit       , kCEETokenID_HASH_INCLUDE  , kGNUASMPrepDirectiveIncludeTranslateStateDirective    );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_include_translate_table, kGNUASMPrepDirectiveIncludeTranslateStateInit       , kCEETokenID_HASH_IMPORT   , kGNUASMPrepDirectiveIncludeTranslateStateDirective    );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_include_translate_table, kGNUASMPrepDirectiveIncludeTranslateStateDirective  , kCEETokenID_IDENTIFIER    , kGNUASMPrepDirectiveIncludeTranslateStateCommit       );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_include_translate_table, kGNUASMPrepDirectiveIncludeTranslateStateDirective  , kCEETokenID_LITERAL       , kGNUASMPrepDirectiveIncludeTranslateStateCommit       );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_include_translate_table, kGNUASMPrepDirectiveIncludeTranslateStateDirective  , '\\'                      , kGNUASMPrepDirectiveIncludeTranslateStateDirective    );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_include_translate_table, kGNUASMPrepDirectiveIncludeTranslateStateDirective  , '<'                       , kGNUASMPrepDirectiveIncludeTranslateStatePath         );
}

static cee_boolean prep_directive_include_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* s = NULL;
    CEEList* r = NULL;
    CEEList* t = NULL;
    CEEToken* token = NULL;
    CEESourceSymbol* include_directive = NULL;
    CEESourceSymbol* include_path = NULL;
    cee_boolean ret = FALSE;
    
    GNUASMPrepDirectiveIncludeTranslateState current = kGNUASMPrepDirectiveIncludeTranslateStateInit;
    
    CEETokenID token_ids[] = {
        kCEETokenID_HASH_INCLUDE,
        kCEETokenID_HASH_IMPORT,
        -1
    };
    
    p = cee_source_fregment_token_find(fregment, token_ids);
    if (!p)
        return FALSE;
    
    s = p;
    
    while (p) {
        token = p->data;
        current = gnu_asm_prep_directive_include_translate_table[current][token->identifier];
        
        if (current == kGNUASMPrepDirectiveIncludeTranslateStatePath) {
            r = p;
            p = skip_include_path(p);
            if (p) {
                t = p;
                q = p;
                current = kGNUASMPrepDirectiveIncludeTranslateStateCommit;
            }
            break;
        }
        
        if (current == kGNUASMPrepDirectiveIncludeTranslateStateCommit) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) ||
                cee_token_is_identifier(p, kCEETokenID_LITERAL))
                r = t = p;
            q = p;
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kGNUASMPrepDirectiveIncludeTranslateStateCommit && s && q) {
        include_directive = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                      fregment->subject_ref,
                                                                      s,
                                                                      q,
                                                                      kCEESourceSymbolTypePrepDirectiveInclude,
                                                                      "gnu_asm",
                                                                      kCEETokenStringOptionCompact);
        cee_source_symbol_name_format(include_directive->name);
        cee_token_slice_state_mark(TOKEN_NEXT(s), q, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, include_directive);
        
        if (r && t) {
            include_path = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     r,
                                                                     t,
                                                                     kCEESourceSymbolTypePrepDirectiveIncludePath,
                                                                     "gnu_asm",
                                                                     kCEETokenStringOptionCompact);
            cee_token_slice_state_mark(r, t, kCEETokenStateSymbolOccupied);
            fregment->symbols = cee_list_prepend(fregment->symbols, include_path);
        }
        
        ret = TRUE;
    }
    
    
#ifdef DEBUG_PREP_DIRECTIVE
    cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static void gnu_asm_prep_directive_define_translate_table_init(void)
{
    /**
     *                      define              identifier      (               )                   ,               \               others
     *  Init                DefineDirective     Error           Error           Error               Error           Error           Error
     *  DefineDirective     Error               DefineName      Error           Error               Error           DefineDirective Error
     *  DefineName          Error               Commit          ParameterList   Error               Error           DefineName      Commit
     *  ParameterList       Error               Parameter       Error           Commit              Error           ParameterList   Error
     *  Parameter           Error               Error           Error           Commit              ParameterList   Parameter       Error
     */
    TRANSLATE_STATE_INI(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateMax             , kGNUASMPrepDirectiveDefineTranslateStateError                                                      );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateInit            , kCEETokenID_HASH_DEFINE                   , kGNUASMPrepDirectiveDefineTranslateStateDirective      );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateDirective       , kCEETokenID_IDENTIFIER                    , kGNUASMPrepDirectiveDefineTranslateStateDefineName     );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateDirective       , '\\'                                      , kGNUASMPrepDirectiveDefineTranslateStateDirective      );
    TRANSLATE_STATE_ANY(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateDefineName      , kGNUASMPrepDirectiveDefineTranslateStateCommit                                                     );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateDefineName      , '('                                       , kGNUASMPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateDefineName      , '\\'                                      , kGNUASMPrepDirectiveDefineTranslateStateDefineName     );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateParameterList   , kCEETokenID_IDENTIFIER                    , kGNUASMPrepDirectiveDefineTranslateStateParameter      );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateParameterList   , ')'                                       , kGNUASMPrepDirectiveDefineTranslateStateCommit         );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateParameterList   , '\\'                                      , kGNUASMPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateParameter       , ')'                                       , kGNUASMPrepDirectiveDefineTranslateStateCommit         );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateParameter       , ','                                       , kGNUASMPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(gnu_asm_prep_directive_define_translate_table,  kGNUASMPrepDirectiveDefineTranslateStateParameter       , '\\'                                      , kGNUASMPrepDirectiveDefineTranslateStateParameter      );
}

static cee_boolean prep_directive_define_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* s = NULL;
    CEEList* t = NULL;
    CEEToken* token = NULL;
    CEESourceSymbol* define_directive = NULL;
    CEESourceSymbol* parameter = NULL;
    cee_boolean ret = FALSE;
    
    GNUASMPrepDirectiveDefineTranslateState current = kGNUASMPrepDirectiveDefineTranslateStateInit;
    
    CEETokenID token_ids[] = {
        kCEETokenID_HASH_DEFINE,
        -1
    };
    
    p = cee_source_fregment_token_find(fregment, token_ids);
    if (!p)
        return FALSE;
    
    while (p) {
        token = p->data;
        current = gnu_asm_prep_directive_define_translate_table[current][token->identifier];
        
        if (current == kGNUASMPrepDirectiveDefineTranslateStateDefineName) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                s = p;
        }
        
        if (current == kGNUASMPrepDirectiveDefineTranslateStateParameter) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(q, token);
        }
        
        if (current == kGNUASMPrepDirectiveDefineTranslateStateCommit) {
            t = p;
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kGNUASMPrepDirectiveDefineTranslateStateCommit && s) {
        /** define macro */
        define_directive = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     s,
                                                                     s,
                                                                     kCEESourceSymbolTypePrepDirectiveDefine,
                                                                     "gnu_asm",
                                                                     kCEETokenStringOptionCompact);
        cee_token_slice_state_mark(s, t, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, define_directive);
        ret = TRUE;
        
        /** parameters */
        p = TOKEN_FIRST(q);
        while (p) {
            parameter = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                  fregment->subject_ref,
                                                                  p,
                                                                  p,
                                                                  kCEESourceSymbolTypePrepDirectiveParameter,
                                                                  "gnu_asm",
                                                                  kCEETokenStringOptionCompact);
            cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
            fregment->symbols = cee_list_prepend(fregment->symbols, parameter);
            p = TOKEN_NEXT(p);
        }
    }
    
    if (q)
        cee_list_free(q);
        
#ifdef DEBUG_PREP_DIRECTIVE
    cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static cee_boolean prep_directive_common_parse(CEESourceFregment* fregment)
{
    
    CEEList* p = NULL;
    CEEList* q = NULL;
    cee_boolean ret = FALSE;
    CEESourceSymbol* common_directive = NULL;
    
    CEETokenID token_ids[] = {
        kCEETokenID_HASH_UNDEF,
        kCEETokenID_HASH_LINE,
        kCEETokenID_HASH_ERROR,
        kCEETokenID_HASH_WARNING,
        kCEETokenID_HASH_PRAGMA,
        kCEETokenID_HASH_UNKNOW,
        -1
    };
    
    p = cee_source_fregment_token_find(fregment, token_ids);
    if (!p)
        return FALSE;
    
    q = SOURCE_FREGMENT_TOKENS_LAST(fregment);
    if (p && q) {
        common_directive = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     p,
                                                                     q,
                                                                     kCEESourceSymbolTypePrepDirectiveCommon,
                                                                     "gnu_asm",
                                                                     kCEETokenStringOptionCompact);
        cee_token_slice_state_mark(p, q, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, common_directive);
        ret = TRUE;
    }
        
#ifdef DEBUG_PREP_DIRECTIVE
    cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

/**
 * statement
 */
static void statement_parse(GNUASMParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    
    if (!current || !current->tokens_ref)
        return;
    
    CEESourceSymbol* symbol = NULL;
    CEEToken* token = NULL;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(current);
    while (p) {
        token = p->data;
        if (token->identifier == kCEETokenID_LABEL) {
            symbol = cee_source_symbol_create_from_token_slice(current->filepath_ref,
                                                               current->subject_ref,
                                                               p,
                                                               p,
                                                               kCEESourceSymbolTypeLabel,
                                                               "gnu_asm",
                                                               kCEETokenStringOptionCompact);
            if (symbol) {
                if (symbol->name)
                    symbol->name = cee_strtrim(symbol->name, ":");
                
                current->symbols = cee_list_prepend(current->symbols, symbol);
                cee_source_fregment_type_set(current, kCEESourceFregmentTypeDeclaration);
            }
            cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        }
        else if (token->identifier == kCEETokenID_IDENTIFIER) {
            cee_char* str = cee_string_from_token(current->subject_ref, token);
            if (str) {
                if (!cee_strcmp(str, ".set", FALSE) ||
                    !cee_strcmp(str, ".equ", FALSE) ||
                    !cee_strcmp(str, ".equiv", FALSE) ||
                    !cee_strcmp(str, ".eqv", FALSE) ||
                    !cee_strcmp(str, ".thumb_func", FALSE)) {
                    
                    CEEList* q = cee_token_not_whitespace_newline_after(p);
                    if (q && cee_token_is_identifier(q, kCEETokenID_IDENTIFIER)) {
                        symbol = cee_source_symbol_create_from_token_slice(current->filepath_ref,
                                                                           current->subject_ref,
                                                                           q,
                                                                           q,
                                                                           kCEESourceSymbolTypeVariableDeclaration,
                                                                           "gnu_asm",
                                                                           kCEETokenStringOptionCompact);
                        if (symbol) {
                            current->symbols = cee_list_prepend(current->symbols, symbol);
                            cee_source_fregment_type_set(current, kCEESourceFregmentTypeDeclaration);
                        }
                        cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
                    }
                                        
                    cee_free(str);
                }
            }
        }
        
        p = TOKEN_NEXT(p);
    }
    
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
                                          "gnu_asm");
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
                                              "gnu_asm");
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

