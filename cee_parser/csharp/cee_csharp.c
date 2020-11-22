#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_csharp.h"

//#define DEBUG_CLASS
//#define DEBUG_STRUCT
//#define DEBUG_ENUM
//#define DEBUG_INTERFACE
//#define DEBUG_METHOD_DEFINITION
//#define DEBUG_DECLARATION

typedef enum _CSharpClassDefinitionTranslateState {
    kCSharpClassDefinitionTranslateStateInit = 0,
    kCSharpClassDefinitionTranslateStateClassSpecifier,
    kCSharpClassDefinitionTranslateStateClassIdentifier,
    kCSharpClassDefinitionTranslateStateClassBase,
    kCSharpClassDefinitionTranslateStateBaseIdentifier,
    kCSharpClassDefinitionTranslateStateConstraintClause,
    kCSharpClassDefinitionTranslateStateConstraintsTypeParameter,
    kCSharpClassDefinitionTranslateStateConstraints,
    kCSharpClassDefinitionTranslateStateConstraint,
    kCSharpClassDefinitionTranslateStateTypeParameters,
    kCSharpClassDefinitionTranslateStateCommit,
    kCSharpClassDefinitionTranslateStateError,
    kCSharpClassDefinitionTranslateStateMax,
} CSharpClassDefinitionTranslateState;

typedef enum _CSharpStructDefinitionTranslateState {
    kCSharpStructDefinitionTranslateStateInit = 0,
    kCSharpStructDefinitionTranslateStateStructSpecifier,
    kCSharpStructDefinitionTranslateStateStructIdentifier,
    kCSharpStructDefinitionTranslateStateStructBase,
    kCSharpStructDefinitionTranslateStateBaseIdentifier,
    kCSharpStructDefinitionTranslateStateConstraintClause,
    kCSharpStructDefinitionTranslateStateConstraintsTypeParameter,
    kCSharpStructDefinitionTranslateStateConstraints,
    kCSharpStructDefinitionTranslateStateConstraint,
    kCSharpStructDefinitionTranslateStateTypeParameters,
    kCSharpStructDefinitionTranslateStateCommit,
    kCSharpStructDefinitionTranslateStateError,
    kCSharpStructDefinitionTranslateStateMax,
} CSharpStructDefinitionTranslateState;

typedef cee_boolean (*ParseTrap)(CEESourceFregment*,
                                 CEEList**);

typedef enum _CSharpParserState {
    kCSharpParserStateStatementParsing = 0x1 << 0,
    kCSharpParserStatePrepDirectiveParsing = 0x1 << 1,
} CSharpParserState;

typedef struct _CSharpParser {
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
    CSharpParserState state;
} CSharpParser;

static CEETokenType csharp_token_type_map[CEETokenID_MAX];
static cee_int csharp_class_definition_translate_table[kCSharpClassDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_struct_definition_translate_table[kCSharpStructDefinitionTranslateStateMax][CEETokenID_MAX];

static void csharp_token_type_map_init(void);
static CEEList* skip_type_parameter_list(CEEList* p,
                                         cee_boolean reverse);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type);
static cee_boolean token_id_is_assignment(CEETokenID identifier);
static cee_boolean token_id_is_builtin_type(CEETokenID identifier);
static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier);
static cee_boolean token_id_is_punctuation(CEETokenID identifier);
static cee_boolean token_id_is_prep_directive(CEETokenID identifier);
static CSharpParser* parser_create(void);
static void parser_free(cee_pointer data);
static void csharp_block_header_trap_init(CSharpParser* parser);
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
static cee_boolean references_in_source_fregment_parse(const cee_uchar* filepath,
                                                       CEESourceFregment* fregment,
                                                       const cee_uchar* subject,
                                                       CEESourceFregment* prep_directive,
                                                       CEESourceFregment* statement,
                                                       CEERange range,
                                                       CEEList** references);
static CEESourceFregment* csharp_referernce_fregment_convert(CEESourceFregment* fregment,
                                                             const cee_uchar* subject);
static void csharp_reference_fregment_parse(const cee_uchar* filepath,
                                            CEESourceFregment* fregment,
                                            const cee_uchar* subject,
                                            CEESourceFregment* prep_directive,
                                            CEESourceFregment* statement,
                                            CEEList** references);
static void symbol_parse_init(CSharpParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
static void symbol_parse_clear(CSharpParser* parser);

/** comment */
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_attach(CSharpParser* parser);
static cee_boolean comment_token_push(CSharpParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(CSharpParser* parser);

/** prep directive */
static cee_boolean prep_directive_parsing(CSharpParser* parser);
static cee_boolean prep_directive_token_push(CSharpParser* parser,
                                             CEEToken* push);
static cee_boolean prep_directive_terminated(CSharpParser* parser);
static cee_boolean prep_directive_attach(CSharpParser* parser,
                                         CEESourceFregmentType type);
static cee_boolean prep_directive_sub_attach(CSharpParser* parser,
                                             CEESourceFregmentType type);
static void prep_directive_branch_push(CSharpParser* parser);
static cee_boolean prep_directive_branch_pop(CSharpParser* parser);
static cee_boolean prep_directive_pop(CSharpParser* parser);
static void prep_directive_parse(CSharpParser* parser);
static cee_boolean should_statement_parsing(CSharpParser* parser);

/** block */
static void block_header_parse(CSharpParser* parser);
static void block_push(CSharpParser* parser);
static cee_boolean block_pop(CSharpParser* parser);
static void block_parse(CSharpParser* parser);
static cee_boolean block_reduce(CSharpParser* parser);
static void label_parse(CSharpParser* parser);
static cee_boolean label_reduce(CSharpParser* parser);

/** statement */
static void statement_parse(CSharpParser* parser);
static cee_boolean statement_reduce(CSharpParser* parser);
static cee_boolean statement_parsing(CSharpParser* parser);
static cee_boolean statement_attach(CSharpParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(CSharpParser* parser,
                                        CEESourceFregmentType type);
static cee_boolean statement_pop(CSharpParser* parser);
static cee_boolean statement_token_push(CSharpParser* parser,
                                        CEEToken* push);
static void parameter_list_push(CSharpParser* parser);
static cee_boolean parameter_list_pop(CSharpParser* parser);
static void subscript_push(CSharpParser* parser);
static cee_boolean subscript_pop(CSharpParser* parser);

static void csharp_class_definition_translate_table_init(void);
static cee_boolean csharp_class_definition_trap(CEESourceFregment* fregment,
                                                CEEList** pp);
static void csharp_struct_definition_translate_table_init(void);
static cee_boolean csharp_struct_definition_trap(CEESourceFregment* fregment,
                                                 CEEList** pp);
static void csharp_interface_definition_translate_table_init(void);
static cee_boolean csharp_interface_definition_trap(CEESourceFregment* fregment,
                                                    CEEList** pp);
static void csharp_enum_definition_translate_table_init(void);
static cee_boolean csharp_enum_definition_trap(CEESourceFregment* fregment,
                                               CEEList** pp);
static CEEList* enumerators_extract(CEEList* tokens,
                                    const cee_uchar* filepath,
                                    const cee_uchar* subject);
static void csharp_method_definition_translate_table_init(void);
static cee_boolean csharp_method_definition_parse(CEESourceFregment* fregment);
static void csharp_method_parameters_declaration_translate_table_init(void);
static cee_boolean csharp_method_parameters_parse(CEESourceFregment* fregment);
static void csharp_declaration_translate_table_init(void);


static void csharp_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        csharp_token_type_map[i] = 0;
    
    csharp_token_type_map[kCEETokenID_ABSTRACT]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_AS]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BASE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BOOL]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BREAK]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BYTE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CASE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CATCH]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CHAR]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CHECKED]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CLASS]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CONST]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CONTINUE]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DECIMAL]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DEFAULT]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DELEGATE]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DO]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DOUBLE]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ELSE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ENUM]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_EVENT]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_EXPLICIT]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_EXTERN]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FALSE]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FINALLY]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FIXED]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FLOAT]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FOR]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FOREACH]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_GOTO]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_IF]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_IMPLICIT]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_IN]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_INT]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_INTERFACE]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_INTERNAL]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_IS]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_LOCK]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_LONG]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_NAMESPACE]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_NEW]                              = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_NULL]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_OBJECT]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_OPERATOR]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_OUT]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_OVERRIDE]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_PARAMS]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_PRIVATE]                          = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_PROTECTED]                        = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_PUBLIC]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_READONLY]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_REF]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_RETURN]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_SBYTE]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_SEALED]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_SHORT]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_SIZEOF]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_STACKALLOC]                       = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_STATIC]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;;
    csharp_token_type_map[kCEETokenID_STRING]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_STRUCT]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_SWITCH]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_THIS]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_THROW]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_TRUE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_TRY]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_TYPEOF]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_UINT]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ULONG]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_UNCHECKED]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_UNSAFE]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;;;
    csharp_token_type_map[kCEETokenID_USHORT]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_USING]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_VIRTUAL]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_VOID]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_VOLATILE]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_WHILE]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ADD]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ALIAS]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ASCENDING]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ASYNC]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_AWAIT]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BY]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DESCENDING]                       = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DYNAMIC]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_EQUALS]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FROM]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_GET]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_GLOBAL]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_GROUP]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_INTO]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_JOIN]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_LET]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_NAMEOF]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_NOTNULL]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ON]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ORDERBY]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_PARTIAL]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_REMOVE]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_SELECT]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_SET]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_UNMANAGED]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_VALUE]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_VAR]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_WHEN]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_WHERE]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_WITH]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_YIELD]                            = kCEETokenTypeKeyword;
    csharp_token_type_map['{']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['}']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['[']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map[']']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['(']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map[')']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['.']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map[',']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map[':']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map[';']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['+']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['-']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['*']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['/']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['%']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['&']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['|']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['^']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['!']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['~']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['=']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['<']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['>']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map['?']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_NULL_COALESCING]                  = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_SCOPE]                            = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_INCREMENT]                        = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_DECREMENT]                        = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_AND]                        = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_OR]                         = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_MEMBER_POINTER]                   = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_EQUAL]                      = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_UNEQUAL]                    = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_LESS_EQUAL]                 = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]                = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_ADD_ASSIGNMENT]                   = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_MINUS_ASSIGNMENT]                 = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_MULTI_ASSIGNMENT]                 = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_DIV_ASSIGNMENT]                   = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_MOD_ASSIGNMENT]                   = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_AND_ASSIGNMENT]                   = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_OR_ASSIGNMENT]                    = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_XOR_ASSIGNMENT]                   = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LEFT_OFFSET_ASSIGNMENT]           = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LAMBDA]                           = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_RIGHT_OFFSET_ASSIGNMENT]          = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_HASH_DEFINE]                      = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_IF]                          = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_ENDIF]                       = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_ELSE]                        = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_ELIF]                        = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_UNDEF]                       = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_LINE]                        = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_ERROR]                       = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_WARNING]                     = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_PRAGMA]                      = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_REGION]                      = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_HASH_ENDREGION]                   = kCEETokenTypePrepDirective;
    csharp_token_type_map[kCEETokenID_CONSTANT]                         = kCEETokenTypeConstant;
    csharp_token_type_map[kCEETokenID_LINES_COMMENT]                    = kCEETokenTypeComment;
    csharp_token_type_map[kCEETokenID_LINE_COMMENT]                     = kCEETokenTypeComment;
    csharp_token_type_map[kCEETokenID_LITERAL]                          = kCEETokenTypeLiteral;
    csharp_token_type_map[kCEETokenID_CHARACTER]                        = kCEETokenTypeCharacter;
    csharp_token_type_map[kCEETokenID_IDENTIFIER]                       = kCEETokenTypeIdentifier;
}

/**
 * p should point to a '<' or '>'
 */
static CEEList* skip_type_parameter_list(CEEList* p,
                                         cee_boolean reverse)
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
        
        if (reverse)
            p = TOKEN_PREV(p);
        else
            p = TOKEN_NEXT(p);
    }
    return p;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type)
{
    return (csharp_token_type_map[token->identifier] & type) != 0;
}

static cee_boolean token_id_is_assignment(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypeAssignment) != 0;
}

static cee_boolean token_id_is_builtin_type(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypeBuildinType) != 0;
}

static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypeDeclarationSpecifier) != 0;
}

static cee_boolean token_id_is_punctuation(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypePunctuation) != 0;
}

static cee_boolean token_id_is_prep_directive(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypePrepDirective) != 0;
}

/**
 * parser
 */
CEESourceParserRef cee_csharp_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    
    CSharpParser* csharp = parser_create();
    csharp->super = parser;
    
    csharp_block_header_trap_init(csharp);
    csharp_token_type_map_init();
    
    csharp_class_definition_translate_table_init();
    csharp_struct_definition_translate_table_init();
    csharp_interface_definition_translate_table_init();
    csharp_enum_definition_translate_table_init();
    csharp_method_definition_translate_table_init();
    csharp_method_parameters_declaration_translate_table_init();
        
    parser->imp = csharp;
    
    return parser;
}

void cee_csharp_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static CSharpParser* parser_create(void)
{
    return cee_malloc0(sizeof(CSharpParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    CSharpParser* parser = (CSharpParser*)data;
    cee_free(parser);
}

static void csharp_block_header_trap_init(CSharpParser* parser)
{
    
    for (int i = 0; i < kCEETokenID_END; i ++)
        parser->block_header_traps[i] = NULL;
    
    parser->block_header_traps[kCEETokenID_CLASS] = csharp_class_definition_trap;
    parser->block_header_traps[kCEETokenID_STRUCT] = csharp_struct_definition_trap;
    parser->block_header_traps[kCEETokenID_INTERFACE] = csharp_interface_definition_trap;
    parser->block_header_traps[kCEETokenID_ENUM] = csharp_enum_definition_trap;
}

static void parser_state_set(CSharpParser* parser,
                             CSharpParserState state)
{
    parser->state |= state;
}

static void parser_state_clear(CSharpParser* parser,
                               CSharpParserState state)
{
    parser->state &= ~state;
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
    CSharpParser* parser = parser_ref->imp;
    cee_int ret = 0;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    
    if (!subject)
        return FALSE;
        
    map = cee_source_token_map_create(subject);
    cee_lexer_csharp_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
    
    do {
        ret = cee_lexer_csharp_token_get(&token);
        
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
                parser_state_clear(parser, kCSharpParserStatePrepDirectiveParsing);
            }
        }
        else if (token_id_is_prep_directive(token->identifier)) {
            parser_state_set(parser, kCSharpParserStatePrepDirectiveParsing);
            if (token->identifier == kCEETokenID_HASH_ELIF ||
                token->identifier == kCEETokenID_HASH_ELSE ||
                token->identifier == kCEETokenID_HASH_ENDIF) {
                if (!prep_directive_branch_pop(parser))
                    parser_state_clear(parser, kCSharpParserStatePrepDirectiveParsing);
                
                prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
                prep_directive_token_push(parser, token);
            }
            else {
                prep_directive_token_push(parser, token);
            }
        }
        else if (statement_parsing(parser)) {
            
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
            else if (token->identifier == ':') {
                statement_token_push(parser, token);
                label_parse(parser);
                label_reduce(parser);
            }
            else if (token->identifier == ';') {
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
    
    cee_source_fregment_tree_symbols_parent_parse(parser->statement_root);
    
    *statement = parser->statement_root;
    *prep_directive = parser->prep_directive_root;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_csharp_buffer_free();
    
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

static cee_boolean references_in_source_fregment_parse(const cee_uchar* filepath,
                                                       CEESourceFregment* fregment,
                                                       const cee_uchar* subject,
                                                       CEESourceFregment* prep_directive,
                                                       CEESourceFregment* statement,
                                                       CEERange range,
                                                       CEEList** references)
{
    CEESourceFregment* reference_fregment = NULL;
    CEEList* p = NULL;
    
    if (cee_source_fregment_over_range(fregment, range))
        return FALSE;
    
    reference_fregment = csharp_referernce_fregment_convert(fregment, subject);
    csharp_reference_fregment_parse(filepath,
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

static CEESourceFregment* csharp_referernce_fregment_convert(CEESourceFregment* fregment,
                                                             const cee_uchar* subject)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* reference_fregment = NULL;
    CEESourceFregment* current = reference_fregment;
    
    if (cee_source_fregment_parent_type_is(fregment, kCEESourceFregmentTypeSquareBracketList)) {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeSquareBracketList,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        (const cee_uchar*)"csharp");
        reference_fregment = cee_source_fregment_sub_attach(reference_fregment,
                                                            kCEESourceFregmentTypeStatement,
                                                            fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            (const cee_uchar*)"csharp");
    }
    else {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeStatement,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        (const cee_uchar*)"csharp");
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
                                                   (const cee_uchar*)"csharp");
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
                                                   (const cee_uchar*)"csharp");
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
                                                   (const cee_uchar*)"csharp");
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

static void csharp_reference_fregment_parse(const cee_uchar* filepath,
                                            CEESourceFregment* fregment,
                                            const cee_uchar* subject,
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
        csharp_reference_fregment_parse(filepath,
                                        p->data,
                                        subject,
                                        prep_directive,
                                        statement,
                                        references);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

static void symbol_parse_init(CSharpParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->prep_directive_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"csharp");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_root, 
                                                                    kCEESourceFregmentTypeSourceList, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    (const cee_uchar*)"csharp");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_current, 
                                                                    kCEESourceFregmentTypePrepDirective, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    (const cee_uchar*)"csharp");
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        (const cee_uchar*)"csharp");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"csharp");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"csharp");
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      (const cee_uchar*)"csharp");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"csharp");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"csharp");
    parser->state = kCSharpParserStateStatementParsing;
}

static void symbol_parse_clear(CSharpParser* parser)
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

static cee_boolean comment_attach(CSharpParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->comment_current,
                                          kCEESourceFregmentTypeComment,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"csharp");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

static cee_boolean comment_token_push(CSharpParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fregment_reduce(CSharpParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

/**
 *  preprocess directive
 */
static cee_boolean prep_directive_parsing(CSharpParser* parser)
{
    return parser->state & kCSharpParserStatePrepDirectiveParsing;
}

static cee_boolean prep_directive_token_push(CSharpParser* parser,
                                             CEEToken* push)
{
    if (!parser->prep_directive_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->prep_directive_current, push, TRUE);
    return TRUE;
}

static cee_boolean prep_directive_terminated(CSharpParser* parser)
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

static cee_boolean prep_directive_attach(CSharpParser* parser,
                                         CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->prep_directive_current,
                                          type,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"csharp");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static cee_boolean prep_directive_sub_attach(CSharpParser* parser,
                                             CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->prep_directive_current,
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              (const cee_uchar*)"csharp");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static void prep_directive_branch_push(CSharpParser* parser)
{
    prep_directive_sub_attach(parser, kCEESourceFregmentTypeSourceList);
    prep_directive_sub_attach(parser, kCEESourceFregmentTypePrepDirective);
}

static cee_boolean prep_directive_branch_pop(CSharpParser* parser)
{
    if (prep_directive_pop(parser))
        return prep_directive_pop(parser);
    
    return FALSE;
}

static cee_boolean prep_directive_pop(CSharpParser* parser)
{
    if (!parser->prep_directive_current ||
        !parser->prep_directive_current->parent)
        return FALSE;
    
    parser->prep_directive_current =
        parser->prep_directive_current->parent;
    return TRUE;
}

static void prep_directive_parse(CSharpParser* parser)
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
        
        if (token->identifier == kCEETokenID_HASH_IF) {
            if (should_statement_parsing(parser))
                parser_state_set(parser, kCSharpParserStateStatementParsing);
            else
                parser_state_clear(parser, kCSharpParserStateStatementParsing);
            cee_source_fregment_type_set(parser->prep_directive_current,
                                         kCEESourceFregmentTypePrepDirectiveCondition);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ELIF ||
                 token->identifier == kCEETokenID_HASH_ELSE) {
            parser_state_clear(parser, kCSharpParserStateStatementParsing);
            cee_source_fregment_type_set(parser->prep_directive_current,
                                         kCEESourceFregmentTypePrepDirectiveBranch);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ENDIF) {
            prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
            if (should_statement_parsing(parser))
                parser_state_set(parser, kCSharpParserStateStatementParsing);
            else
                parser_state_clear(parser, kCSharpParserStateStatementParsing);
        }
        else {
            prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
        }
    }
}

static cee_boolean should_statement_parsing(CSharpParser* parser)
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

/**
 *  block
 */
static void block_header_parse(CSharpParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    ParseTrap trap = NULL;
    
    if (!current || !current->tokens_ref)
        return;
    
    if (csharp_method_definition_parse(current))
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

static void block_push(CSharpParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean block_pop(CSharpParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void block_parse(CSharpParser* parser)
{
}

static cee_boolean block_reduce(CSharpParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    return TRUE;
}

static void label_parse(CSharpParser* parser)
{
    CEESourceFregment* fregment = NULL;
    const cee_uchar* subject = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEESourceSymbol* declaration = NULL;
        
    fregment = parser->statement_current;
    if (!fregment || !fregment->tokens_ref)
        return;
    
    if (!cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeFunctionDefinition) &&
        !cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeStatementBlock))
        return;
    
    subject = parser->subject_ref;
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_NEW_LINE) ||
            cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE)) {
            p = TOKEN_NEXT(p);
            continue;
        }
        else {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                q = cee_token_not_whitespace_newline_after(p);
                if (q && cee_token_is_identifier(q, ':')) {
                    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                            subject,
                                                                            p,
                                                                            p,
                                                                            kCEESourceSymbolTypeLabel,
                                                                            "csharp");
                    cee_token_slice_state_mark(p, q, kCEETokenStateSymbolOccupied);
                    
                    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeLabelDeclaration);
                    fregment->symbols = cee_list_prepend(fregment->symbols, declaration);
                }
                break;
            }
            else
                break;
        }
    }
    
#ifdef DEBUG_LABEL
    cee_source_symbol_print(declaration);
#endif
    
}

static cee_boolean label_reduce(CSharpParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    if (cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeLabelDeclaration))
        statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}

/**
 * statement
 */
static void statement_parse(CSharpParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    return;
}

static cee_boolean statement_reduce(CSharpParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
        
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}
static cee_boolean statement_parsing(CSharpParser* parser)
{
    return parser->state & kCSharpParserStateStatementParsing;
}

static cee_boolean statement_attach(CSharpParser* parser,
                                    CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"csharp");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(CSharpParser* parser,
                                        CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->statement_current, 
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              (const cee_uchar*)"csharp");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_pop(CSharpParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}

static cee_boolean statement_token_push(CSharpParser* parser,
                                        CEEToken* push)
{
    if (!parser->statement_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->statement_current, push, TRUE);
    
    return TRUE;
}

/**
 * parameter list
 */
static void parameter_list_push(CSharpParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeRoundBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean parameter_list_pop(CSharpParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

/**
 * subscript
 */
static void subscript_push(CSharpParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeSquareBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean subscript_pop(CSharpParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void csharp_class_definition_translate_table_init(void)
{
    /**
     *                              class_specifier     identifier                  struct      new         where               <                   :               ,               (           )              {
     *  Init                        ClassSpecifier      Error                       Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  ClassSpecifier              Error               ClassIdentifier             Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  ClassIdentifier             Error               Error                       Error       Error       ConstraintClause    *TypeParameters     ClassBase       Error           Error       Error          Commit
     *  ClassBase                   Error               BaseIdentifier              Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  BaseIdentifier              Error               BaseIdentifier              Error       Error       ConstraintClause    *TypeParameters     Error           BaseIdentifier  Error       Error          Commit
     *  ConstraintClause            Error               ConstraintsTypeParameter    Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  ConstraintsTypeParameter    Error               Error                       Error       Error       Error               *TypeParameters     Constraints     Error           Error       Error          Error
     *  Constraints                 Constraint          Constraint                  Constraint  Constraint  Error               Error               Error           Error           Error       Error          Error
     *  Constraint                  Constraint          Constraint                  Constraint  Constraint  ConstraintClause    *TypeParameters     Error           Constraint      Constraint  Constraint     Commit
     *
     *  TypeParameters save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateMax                         , kCSharpClassDefinitionTranslateStateError                                                 );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateInit                        , kCEETokenID_CLASS         , kCSharpClassDefinitionTranslateStateClassSpecifier            );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateClassSpecifier              , kCEETokenID_IDENTIFIER    , kCSharpClassDefinitionTranslateStateClassIdentifier           );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateClassIdentifier             , kCEETokenID_WHERE         , kCSharpClassDefinitionTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateClassIdentifier             , '<'                       , kCSharpClassDefinitionTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateClassIdentifier             , ':'                       , kCSharpClassDefinitionTranslateStateClassBase                 );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateClassIdentifier             , '{'                       , kCSharpClassDefinitionTranslateStateCommit                    );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateClassBase                   , kCEETokenID_IDENTIFIER    , kCSharpClassDefinitionTranslateStateBaseIdentifier            );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateBaseIdentifier              , kCEETokenID_WHERE         , kCSharpClassDefinitionTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateBaseIdentifier              , '<'                       , kCSharpClassDefinitionTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateBaseIdentifier              , ','                       , kCSharpClassDefinitionTranslateStateBaseIdentifier            );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateBaseIdentifier              , '{'                       , kCSharpClassDefinitionTranslateStateCommit                    );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraintClause            , kCEETokenID_IDENTIFIER    , kCSharpClassDefinitionTranslateStateConstraintsTypeParameter  );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraintsTypeParameter    , '<'                       , kCSharpClassDefinitionTranslateStateConstraintsTypeParameter  );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraintsTypeParameter    , ':'                       , kCSharpClassDefinitionTranslateStateConstraints               );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraints                 , kCEETokenID_CLASS         , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraints                 , kCEETokenID_IDENTIFIER    , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraints                 , kCEETokenID_STRUCT        , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraints                 , kCEETokenID_NEW           , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , kCEETokenID_CLASS         , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , kCEETokenID_IDENTIFIER    , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , kCEETokenID_STRUCT        , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , kCEETokenID_NEW           , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , kCEETokenID_WHERE         , kCSharpClassDefinitionTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , '<'                       , kCSharpClassDefinitionTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , ','                       , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , '('                       , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , ')'                       , kCSharpClassDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_class_definition_translate_table,    kCSharpClassDefinitionTranslateStateConstraint                  , '{'                       , kCSharpClassDefinitionTranslateStateCommit                    );
}

static cee_boolean csharp_class_definition_trap(CEESourceFregment* fregment,
                                                CEEList** pp)
{
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    CSharpClassDefinitionTranslateState current = kCSharpClassDefinitionTranslateStateInit;
    CSharpClassDefinitionTranslateState prev = kCSharpClassDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    
    CEESourceSymbol* definition = NULL;
    
    while (p) {
        token = p->data;
        
        prev = current;
        current = csharp_class_definition_translate_table[current][token->identifier];
                
        if (current == kCSharpClassDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpClassDefinitionTranslateStateClassIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER) {
            }
        }
        else if (current == kCSharpClassDefinitionTranslateStateError) {
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    *pp = NULL;
    
    
#ifdef DEBUG_CLASS
    cee_source_symbol_print(definition);
#endif
    
exit:
    
    return ret;
}

static void csharp_struct_definition_translate_table_init(void)
{
    /**
     *                              struct_specifier    identifier                  struct      new         where               <                   :               ,               (           )              {
     *  Init                        StructSpecifier     Error                       Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  StructSpecifier             Error               StructIdentifier            Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  StructIdentifier            Error               Error                       Error       Error       ConstraintClause    *TypeParameters     StructBase      Error           Error       Error          Commit
     *  StructBase                  Error               BaseIdentifier              Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  BaseIdentifier              Error               BaseIdentifier              Error       Error       ConstraintClause    *TypeParameters     Error           BaseIdentifier  Error       Error          Commit
     *  ConstraintClause            Error               ConstraintsTypeParameter    Error       Error       Error               Error               Error           Error           Error       Error          Error
     *  ConstraintsTypeParameter    Error               Error                       Error       Error       Error               *TypeParameters     Constraints     Error           Error       Error          Error
     *  Constraints                 Constraint          Constraint                  Constraint  Constraint  Error               Error               Error           Error           Error       Error          Error
     *  Constraint                  Constraint          Constraint                  Constraint  Constraint  ConstraintClause    *TypeParameters     Error           Constraint      Constraint  Constraint     Commit
     *
     *  TypeParameters save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateMax                         , kCSharpStructDefinitionTranslateStateError                                                 );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateInit                        , kCEETokenID_STRUCT        , kCSharpStructDefinitionTranslateStateStructSpecifier           );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateStructSpecifier             , kCEETokenID_IDENTIFIER    , kCSharpStructDefinitionTranslateStateStructIdentifier          );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateStructIdentifier            , kCEETokenID_WHERE         , kCSharpStructDefinitionTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateStructIdentifier            , '<'                       , kCSharpStructDefinitionTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateStructIdentifier            , ':'                       , kCSharpStructDefinitionTranslateStateStructBase                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateStructIdentifier            , '{'                       , kCSharpStructDefinitionTranslateStateCommit                    );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateStructBase                  , kCEETokenID_IDENTIFIER    , kCSharpStructDefinitionTranslateStateBaseIdentifier            );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateBaseIdentifier              , kCEETokenID_WHERE         , kCSharpStructDefinitionTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateBaseIdentifier              , '<'                       , kCSharpStructDefinitionTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateBaseIdentifier              , ','                       , kCSharpStructDefinitionTranslateStateBaseIdentifier            );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateBaseIdentifier              , '{'                       , kCSharpStructDefinitionTranslateStateCommit                    );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraintClause            , kCEETokenID_IDENTIFIER    , kCSharpStructDefinitionTranslateStateConstraintsTypeParameter  );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraintsTypeParameter    , '<'                       , kCSharpStructDefinitionTranslateStateConstraintsTypeParameter  );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraintsTypeParameter    , ':'                       , kCSharpStructDefinitionTranslateStateConstraints               );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraints                 , kCEETokenID_CLASS         , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraints                 , kCEETokenID_IDENTIFIER    , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraints                 , kCEETokenID_STRUCT        , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraints                 , kCEETokenID_NEW           , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , kCEETokenID_CLASS         , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , kCEETokenID_IDENTIFIER    , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , kCEETokenID_STRUCT        , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , kCEETokenID_NEW           , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , kCEETokenID_WHERE         , kCSharpStructDefinitionTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , '<'                       , kCSharpStructDefinitionTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , ','                       , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , '('                       , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , ')'                       , kCSharpStructDefinitionTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_struct_definition_translate_table,    kCSharpStructDefinitionTranslateStateConstraint                  , '{'                       , kCSharpStructDefinitionTranslateStateCommit                    );
}

static cee_boolean csharp_struct_definition_trap(CEESourceFregment* fregment,
                                                 CEEList** pp)
{
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    CSharpStructDefinitionTranslateState current = kCSharpStructDefinitionTranslateStateInit;
    CSharpStructDefinitionTranslateState prev = kCSharpStructDefinitionTranslateStateInit;
    CEEToken* token = NULL;
        
    CEESourceSymbol* definition = NULL;
    
    while (p) {
        token = p->data;
        
        prev = current;
        current = csharp_struct_definition_translate_table[current][token->identifier];
                
        if (current == kCSharpStructDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpStructDefinitionTranslateStateStructIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER) {
            }
        }
        else if (current == kCSharpStructDefinitionTranslateStateError) {
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    *pp = NULL;
    
#ifdef DEBUG_STRUCT
    cee_source_symbol_print(definition);
#endif
    
exit:
    
    return ret;
}
static void csharp_interface_definition_translate_table_init(void)
{
    
}

static cee_boolean csharp_interface_definition_trap(CEESourceFregment* fregment,
                                                    CEEList** pp)
{
    return FALSE;
}

static void csharp_enum_definition_translate_table_init(void)
{
    
}

static cee_boolean csharp_enum_definition_trap(CEESourceFregment* fregment,
                                               CEEList** pp)
{
    return FALSE;
}

static CEEList* enumerators_extract(CEEList* tokens,
                                    const cee_uchar* filepath,
                                    const cee_uchar* subject)
{
    return NULL;
}

static void csharp_method_definition_translate_table_init(void)
{
    
}

static cee_boolean csharp_method_definition_parse(CEESourceFregment* fregment)
{
    return FALSE;
}

static void csharp_method_parameters_declaration_translate_table_init(void)
{
    
}

static cee_boolean csharp_method_parameters_parse(CEESourceFregment* fregment)
{
    return FALSE;
}

static void csharp_declaration_translate_table_init(void)
{
    
}
