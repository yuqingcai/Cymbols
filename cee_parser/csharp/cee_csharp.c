#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_csharp.h"

//#define DEBUG_CLASS
//#define DEBUG_STRUCT
//#define DEBUG_INTERFACE
//#define DEBUG_ENUM
//#define DEBUG_METHOD_DEFINITION
//#define DEBUG_PROPERTY_DECLARATION
//#define DEBUG_DECLARATION
//#define DEBUG_EVENTS
//#define DEBUG_NAMESPACE
//#define DEBUG_USING_DIRECTIVE
//#define DEBUG_DELEGATE

typedef enum _CSharpObjectTypeDefinitionTranslateState {
    kCSharpObjectTypeDefinitionTranslateStateInit = 0,
    kCSharpObjectTypeDefinitionTranslateStateObjectTypeSpecifier,
    kCSharpObjectTypeDefinitionTranslateStateIdentifier,
    kCSharpObjectTypeDefinitionTranslateStateIdentifierSeparator,
    kCSharpObjectTypeDefinitionTranslateStateBase,
    kCSharpObjectTypeDefinitionTranslateStateConstraintClause,
    kCSharpObjectTypeDefinitionTranslateStateConstraintsTypeParameter,
    kCSharpObjectTypeDefinitionTranslateStateConstraints,
    kCSharpObjectTypeDefinitionTranslateStateConstraint,
    kCSharpObjectTypeDefinitionTranslateStateConstructor,
    kCSharpObjectTypeDefinitionTranslateStateConstructorList,
    kCSharpObjectTypeDefinitionTranslateStateTypeParameters,
    kCSharpObjectTypeDefinitionTranslateStateComma,
    kCSharpObjectTypeDefinitionTranslateStateConfirm,
    kCSharpObjectTypeDefinitionTranslateStateError,
    kCSharpObjectTypeDefinitionTranslateStateMax,
} CSharpObjectTypeDefinitionTranslateState;

typedef enum _CSharpEnumDefinitionTranslateState {
    kCSharpEnumDefinitionTranslateStateInit = 0,
    kCSharpEnumDefinitionTranslateStateEnum,
    kCSharpEnumDefinitionTranslateStateIdentifier,
    kCSharpEnumDefinitionTranslateStateBase,
    kCSharpEnumDefinitionTranslateStateIntegralType,
    kCSharpEnumDefinitionTranslateStateTypeParameters,
    kCSharpEnumDefinitionTranslateStateConfirm,
    kCSharpEnumDefinitionTranslateStateError,
    kCSharpEnumDefinitionTranslateStateMax,
} CSharpEnumDefinitionTranslateState;

typedef enum _CSharpMethodTranslateState {
    kCSharpMethodTranslateStateInit = 0,
    kCSharpMethodTranslateStateDeclarationSpecifier,
    kCSharpMethodTranslateStateBuiltinType,
    kCSharpMethodTranslateStateBuiltinReference,
    kCSharpMethodTranslateStateIdentifier,
    kCSharpMethodTranslateStateIdentifierSeparator,
    kCSharpMethodTranslateStateParameterList,
    kCSharpMethodTranslateStateParameterListEnd,
    kCSharpMethodTranslateStateConstraintClause,
    kCSharpMethodTranslateStateConstraintsTypeParameter,
    kCSharpMethodTranslateStateConstraints,
    kCSharpMethodTranslateStateConstraint,
    kCSharpMethodTranslateStateConstructor,
    kCSharpMethodTranslateStateConstructorList,
    kCSharpMethodTranslateStateTypeParameters,
    kCSharpMethodTranslateStateConstructorInitializer,
    kCSharpMethodTranslateStateConstructorInitializerThis,
    kCSharpMethodTranslateStateConstructorInitializerBase,
    kCSharpMethodTranslateStateInitializerList,
    kCSharpMethodTranslateStateInitializerListEnd,
    kCSharpMethodTranslateStateDefinition,
    kCSharpMethodTranslateStateDeclaration,
    kCSharpMethodTranslateStateError,
    kCSharpMethodTranslateStateMax,
} CSharpMethodTranslateState;

typedef enum _CSharpMethodParametersDeclarationTranslateState {
    kCSharpMethodParametersDeclarationTranslateStateInit = 0,
    kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier,
    kCSharpMethodParametersDeclarationTranslateStateBuiltinType,
    kCSharpMethodParametersDeclarationTranslateStateBuiltinReference,
    kCSharpMethodParametersDeclarationTranslateStateParams,
    kCSharpMethodParametersDeclarationTranslateStateIdentifier,
    kCSharpMethodParametersDeclarationTranslateStateIdentifierSeparator,
    kCSharpMethodParametersDeclarationTranslateStateTypeParameters,
    kCSharpMethodParametersDeclarationTranslateStateConfirm,
    kCSharpMethodParametersDeclarationTranslateStateError,
    kCSharpMethodParametersDeclarationTranslateStateMax,
} CSharpMethodParametersDeclarationTranslateState;

typedef enum _CSharpOperatorTranslateState {
    kCSharpOperatorTranslateStateInit = 0,
    kCSharpOperatorTranslateStateDeclarationSpecifier,
    kCSharpOperatorTranslateStateBuiltinType,
    kCSharpOperatorTranslateStateBuiltinReference,
    kCSharpOperatorTranslateStateIdentifier,
    kCSharpOperatorTranslateStateIdentifierSeparator,
    kCSharpOperatorTranslateStateOperator,
    kCSharpOperatorTranslateStateOverlaodable,
    kCSharpOperatorTranslateStateParameterList,
    kCSharpOperatorTranslateStateParameterListEnd,
    kCSharpOperatorTranslateStateTypeParameters,
    kCSharpOperatorTranslateStateDefinition,
    kCSharpOperatorTranslateStateDeclaration,
    kCSharpOperatorTranslateStateError,
    kCSharpOperatorTranslateStateMax,
} CSharpOperatorTranslateState;

typedef enum _CSharpPropertyDeclarationTranslateState {
    kCSharpPropertyDeclarationTranslateStateInit = 0,
    kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier,
    kCSharpPropertyDeclarationTranslateStateBuiltinType,
    kCSharpPropertyDeclarationTranslateStateBuiltinReference,
    kCSharpPropertyDeclarationTranslateStateIdentifier,
    kCSharpPropertyDeclarationTranslateStateIdentifierSeparator,
    kCSharpPropertyDeclarationTranslateStateTypeParameters,
    kCSharpPropertyDeclarationTranslateStateConfirm,
    kCSharpPropertyDeclarationTranslateStateError,
    kCSharpPropertyDeclarationTranslateStateMax,
} CSharpPropertyDeclarationTranslateState;

typedef enum _CSharpEventTranslateState {
    kCSharpEventTranslateStateInit = 0,
    kCSharpEventTranslateStateDeclarationSpecifier,
    kCSharpEventTranslateStateEvent,
    kCSharpEventTranslateStateBuiltinType,
    kCSharpEventTranslateStateBuiltinReference,
    kCSharpEventTranslateStateCustomType,
    kCSharpEventTranslateStateCustomTypeSeparator,
    kCSharpEventTranslateStateIdentifier,
    kCSharpEventTranslateStateTypeParameters,
    kCSharpEventTranslateStateConfirm,
    kCSharpEventTranslateStateError,
    kCSharpEventTranslateStateMax,
} CSharpEventTranslateState;

typedef enum _CSharpDeclarationTranslateState {
    kCSharpDeclarationTranslateStateInit = 0,
    kCSharpDeclarationTranslateStateUsing,
    kCSharpDeclarationTranslateStateDeclarationSpecifier,
    kCSharpDeclarationTranslateStateBuiltinType,
    kCSharpDeclarationTranslateStateBuiltinReference,
    kCSharpDeclarationTranslateStateCustomType,
    kCSharpDeclarationTranslateStateCustomTypeSeparator,
    kCSharpDeclarationTranslateStateIdentifier,
    kCSharpDeclarationTranslateStateTypeParameters,
    kCSharpDeclarationTranslateStateConfirm,
    kCSharpDeclarationTranslateStateError,
    kCSharpDeclarationTranslateStateMax,
} CSharpDeclarationTranslateState;

typedef enum _CSharpIndexerDeclarationTranslateState {
    kCSharpIndexerDeclarationTranslateStateInit = 0,
    kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier,
    kCSharpIndexerDeclarationTranslateStateBuiltinType,
    kCSharpIndexerDeclarationTranslateStateBuiltinReference,
    kCSharpIndexerDeclarationTranslateStateType,
    kCSharpIndexerDeclarationTranslateStateTypeSeparator,
    kCSharpIndexerDeclarationTranslateStateInterfaceType,
    kCSharpIndexerDeclarationTranslateStateInterfaceTypeSeparator,
    kCSharpIndexerDeclarationTranslateStateThis,
    kCSharpIndexerDeclarationTranslateStateParameterList,
    kCSharpIndexerDeclarationTranslateStateParameterListEnd,
    kCSharpIndexerDeclarationTranslateStateTypeParameters,
    kCSharpIndexerDeclarationTranslateStateConfirm,
    kCSharpIndexerDeclarationTranslateStateError,
    kCSharpIndexerDeclarationTranslateStateMax,
} CSharpIndexerDeclarationTranslateState;

typedef enum _CSharpNamespaceDefinitionTranslateState{
    kCSharpNamespaceDefinitionTranslateStateInit = 0,
    kCSharpNamespaceDefinitionTranslateStateNamespace,
    kCSharpNamespaceDefinitionTranslateStateIdentifier,
    kCSharpNamespaceDefinitionTranslateStateIdentifierSeparator,
    kCSharpNamespaceDefinitionTranslateStateConfirm,
    kCSharpNamespaceDefinitionTranslateStateError,
    kCSharpNamespaceDefinitionTranslateStateMax,
} CSharpNamespaceDefinitionTranslateState;

typedef enum _CSharpUsingDirectiveTranslateState{
    kCSharpUsingDirectiveTranslateStateInit = 0,
    kCSharpUsingDirectiveTranslateStateUsing,
    kCSharpUsingDirectiveTranslateStateAssignment,
    kCSharpUsingDirectiveTranslateStateIdentifier,
    kCSharpUsingDirectiveTranslateStateIdentifierSeparator,
    kCSharpUsingDirectiveTranslateStateAliasIdentifier,
    kCSharpUsingDirectiveTranslateStateAliasIdentifierSeparator,
    kCSharpUsingDirectiveTranslateStateConfirm,
    kCSharpUsingDirectiveTranslateStateError,
    kCSharpUsingDirectiveTranslateStateMax,
} CSharpUsingDirectiveTranslateState;

typedef enum _CSharpDelegateDeclarationTranslateState {
    kCSharpDelegateDeclarationTranslateStateInit = 0,
    kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier,
    kCSharpDelegateDeclarationTranslateStateBuiltinType,
    kCSharpDelegateDeclarationTranslateStateBuiltinReference,
    kCSharpDelegateDeclarationTranslateStateIdentifier,
    kCSharpDelegateDeclarationTranslateStateIdentifierSeparator,
    kCSharpDelegateDeclarationTranslateStateDelegate,
    kCSharpDelegateDeclarationTranslateStateParameterList,
    kCSharpDelegateDeclarationTranslateStateParameterListEnd,
    kCSharpDelegateDeclarationTranslateStateConstraintClause,
    kCSharpDelegateDeclarationTranslateStateConstraintsTypeParameter,
    kCSharpDelegateDeclarationTranslateStateConstraints,
    kCSharpDelegateDeclarationTranslateStateConstraint,
    kCSharpDelegateDeclarationTranslateStateConstructor,
    kCSharpDelegateDeclarationTranslateStateConstructorList,
    kCSharpDelegateDeclarationTranslateStateComma,
    kCSharpDelegateDeclarationTranslateStateTypeParameters,
    kCSharpDelegateDeclarationTranslateStateConfirm,
    kCSharpDelegateDeclarationTranslateStateError,
    kCSharpDelegateDeclarationTranslateStateMax,
} CSharpDelegateDeclarationTranslateState;

typedef cee_boolean (*ParseTrap)(CEESourceFragment*,
                                 CEEList**);

typedef enum _CSharpParserState {
    kCSharpParserStateStatementParsing = 0x1 << 0,
    kCSharpParserStatePrepDirectiveParsing = 0x1 << 1,
} CSharpParserState;

typedef struct _CSharpParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFragment* statement_root;
    CEESourceFragment* statement_current;
    CEESourceFragment* prep_directive_root;
    CEESourceFragment* prep_directive_current;
    CEESourceFragment* comment_root;
    CEESourceFragment* comment_current;
    ParseTrap block_header_traps[CEETokenID_MAX];
    CSharpParserState state;
} CSharpParser;

static CEETokenType csharp_token_type_map[CEETokenID_MAX];
static cee_int csharp_object_type_definition_translate_table[kCSharpObjectTypeDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_enum_definition_translate_table[kCSharpEnumDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_method_translate_table[kCSharpMethodTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_method_parameters_declaration_translate_table[kCSharpMethodParametersDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_operator_translate_table[kCSharpOperatorTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_property_declaration_translate_table[kCSharpPropertyDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_event_declaration_table[kCSharpEventTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_declaration_translate_table[kCSharpDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_indexer_declaration_translate_table[kCSharpIndexerDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_namespace_definition_translate_table[kCSharpNamespaceDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_using_directive_translate_table[kCSharpUsingDirectiveTranslateStateMax][CEETokenID_MAX];
static cee_int csharp_delegate_declaration_translate_table[kCSharpDelegateDeclarationTranslateStateMax][CEETokenID_MAX];

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
static cee_boolean token_id_is_integral_type(CEETokenID identifier);
static cee_boolean token_id_is_object_specifier(CEETokenID identifier);
static cee_boolean token_id_is_builtin_reference(CEETokenID identifier);
static cee_boolean token_id_is_parameter_specifier(CEETokenID identifier);
static cee_boolean token_id_is_overloadable(CEETokenID identifier);
static CSharpParser* parser_create(void);
static void parser_free(cee_pointer data);
static void csharp_block_header_trap_init(CSharpParser* parser);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFragment** prep_directive,
                                CEESourceFragment** statement,
                                CEESourceFragment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);
static cee_boolean reference_parse(CEESourceParserRef parser_ref,
                                   const cee_char* filepath,
                                   const cee_char* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFragment* prep_directive,
                                   CEESourceFragment* statement,
                                   CEERange range,
                                   CEEList** references);
static cee_boolean references_in_source_fragment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
                                                       CEESourceFragment* fragment,
                                                       const cee_char* subject,
                                                       CEESourceFragment* prep_directive,
                                                       CEESourceFragment* statement,
                                                       CEERange range,
                                                       CEEList** references);
static CEESourceFragment* csharp_referernce_fragment_convert(CEESourceFragment* fragment,
                                                             const cee_char* subject);
static void csharp_reference_fragment_parse(CEESourceParserRef parser_ref,
                                            const cee_char* filepath,
                                            CEESourceFragment* fragment,
                                            const cee_char* subject,
                                            CEESourceFragment* prep_directive,
                                            CEESourceFragment* statement,
                                            CEEList** references);
static void symbol_parse_init(CSharpParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
static void symbol_parse_clear(CSharpParser* parser);

/** comment */
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_attach(CSharpParser* parser);
static cee_boolean comment_token_push(CSharpParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fragment_reduce(CSharpParser* parser);

/** prep directive */
static cee_boolean prep_directive_parsing(CSharpParser* parser);
static cee_boolean prep_directive_token_push(CSharpParser* parser,
                                             CEEToken* push);
static cee_boolean prep_directive_terminated(CSharpParser* parser);
static cee_boolean prep_directive_attach(CSharpParser* parser,
                                         CEESourceFragmentType type);
static cee_boolean prep_directive_sub_attach(CSharpParser* parser,
                                             CEESourceFragmentType type);
static void prep_directive_branch_push(CSharpParser* parser);
static cee_boolean prep_directive_branch_pop(CSharpParser* parser);
static cee_boolean prep_directive_pop(CSharpParser* parser);
static void prep_directive_parse(CSharpParser* parser);
static cee_boolean should_statement_parsing(CSharpParser* parser);

/** block */
static void block_header_parse(CSharpParser* parser);
static cee_boolean statement_block_parse(CEESourceFragment* fragment);
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
                                    CEESourceFragmentType type);
static cee_boolean statement_sub_attach(CSharpParser* parser,
                                        CEESourceFragmentType type);
static cee_boolean statement_pop(CSharpParser* parser);
static cee_boolean statement_token_push(CSharpParser* parser,
                                        CEEToken* push);
static CEEToken* statement_token_pop(CSharpParser* parser);
static void parameter_list_push(CSharpParser* parser);
static cee_boolean parameter_list_pop(CSharpParser* parser);
static void subscript_push(CSharpParser* parser);
static cee_boolean subscript_pop(CSharpParser* parser);
static void csharp_declaration_translate_table_init(void);
static cee_boolean csharp_declaration_parse(CEESourceFragment* fragment);
static void csharp_object_type_definition_translate_table_init(void);
static cee_boolean csharp_object_type_definition_trap(CEESourceFragment* fragment,
                                                      CEEList** pp);
static void csharp_enum_definition_translate_table_init(void);
static cee_boolean csharp_enum_definition_trap(CEESourceFragment* fragment,
                                               CEEList** pp);
static cee_boolean csharp_enumerators_parse(CEESourceFragment* fragment);
static cee_char* csharp_enumerator_proto_descriptor_create(CEESourceFragment* fragment,
                                                           CEESourceSymbol* enumerator,
                                                           const cee_char* proto);
static void base_free(cee_pointer data);
static cee_char* csharp_object_type_proto_descriptor_create(CEESourceFragment* fragment,
                                                            CEESourceSymbol* definition,
                                                            const cee_char* derives_str);
static cee_char* csharp_declaration_proto_descriptor_create(CEESourceFragment* fragment,
                                                            CEEList* prefix,
                                                            CEEList* prefix_tail,
                                                            CEEList* identifier);
static cee_char* csharp_namespace_proto_descriptor_create(CEESourceFragment* fragment,
                                                          CEESourceSymbol* definition);
static cee_char* csharp_name_scope_list_string_create(CEEList* scopes,
                                                      const cee_char* subject);
static cee_char* csharp_name_scope_create(CEEList* tokens,
                                          const cee_char* subject);
static void csharp_method_translate_table_init(void);
static cee_boolean csharp_method_parse(CEESourceFragment* fragment);
static void csharp_method_parameters_declaration_translate_table_init(void);
static cee_boolean csharp_method_parameters_parse(CEESourceFragment* fragment);
static CEESourceSymbol* csharp_method_parameter_create(CEESourceFragment* fragment,
                                                       CEEList* prefix,
                                                       CEEList* identifier);
static cee_char* csharp_method_parameter_proto_descriptor_create(CEESourceFragment* fragment,
                                                                 CEEList* prefix,
                                                                 CEEList* prefix_tail,
                                                                 CEESourceSymbol* parameter);
static cee_char* csharp_type_descriptor_from_token_slice(CEESourceFragment* fragment,
                                                         CEEList* p,
                                                         CEEList* q);
static cee_char* csharp_method_proto_descriptor_create(CEESourceFragment* fragment,
                                                       CEEList* prefix,
                                                       CEEList* prefix_tail,
                                                       CEEList* identifier,
                                                       CEEList* parameter_list,
                                                       CEEList* parameter_list_end);
static cee_boolean should_proto_descriptor_append_whitespace(CEEToken* token,
                                                             CEEToken* token_prev);
static const cee_char* csharp_access_level_search(CEESourceFragment* fragment,
                                                  CEEList* begin,
                                                  CEEList* end);
static CEEList* skip_csharp_declaration_interval(CEEList* p);
static void csharp_operator_translate_table_init(void);
static cee_boolean csharp_operator_parse(CEESourceFragment* fragment);
static cee_char* csharp_operator_name_create(CEESourceFragment* fragment);
static void csharp_property_declaration_translate_table_init(void);
static cee_boolean csharp_property_declaration_parse(CEESourceFragment* fragment);
static cee_char* csharp_property_proto_descriptor_create(CEESourceFragment* fragment,
                                                         CEEList* prefix,
                                                         CEEList* prefix_tail,
                                                         CEEList* identifier);
static void csharp_event_declaration_table_init(void);
static cee_boolean csharp_event_declaration_parse(CEESourceFragment* fragment);
static cee_char* csharp_event_proto_descriptor_create(CEESourceFragment* fragment,
                                                      CEEList* prefix,
                                                      CEEList* prefix_tail,
                                                      CEEList* identifier);
static void csharp_indexer_declaration_translate_table_init(void);
static cee_boolean csharp_indexer_declaration_parse(CEESourceFragment* fragment);
static cee_char* csharp_indexer_proto_descriptor_create(CEESourceFragment* fragment,
                                                        CEEList* prefix,
                                                        CEEList* prefix_tail);
static void csharp_namespace_definition_translate_table_init(void);
static cee_boolean csharp_namespace_definition_trap(CEESourceFragment* fragment,
                                                    CEEList** pp);
static void csharp_using_directive_translate_table_init(void);
static cee_boolean csharp_using_directive_parse(CEESourceFragment* fragment);
static cee_char* csharp_using_alias_proto_descriptor_create(CEESourceFragment* fragment,
                                                            CEEList* identifier,
                                                            CEEList* alias);
static void csharp_delegate_declaration_translate_table_init(void);
static cee_boolean csharp_delegate_declaration_parse(CEESourceFragment* fragment);
static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFragment* prep_directive,
                                          CEESourceFragment* statement,
                                          CEESourceSymbol** symbol);
static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFragment* searching,
                                                         const cee_char* name);
static void csharp_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        csharp_token_type_map[i] = 0;
    
    csharp_token_type_map[kCEETokenID_ABSTRACT]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_AS]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BASE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BOOL]                             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_BREAK]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BYTE]                             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_CASE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CATCH]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CHAR]                             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_CHECKED]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_CLASS]                            = kCEETokenTypeKeyword | kCEETokenTypeObjectSpecifier;
    csharp_token_type_map[kCEETokenID_CONST]                            = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_CONTINUE]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DECIMAL]                          = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_DEFAULT]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DELEGATE]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DO]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DOUBLE]                           = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_ELSE]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ENUM]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_EVENT]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_EXPLICIT]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_EXTERN]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_FALSE]                            = kCEETokenTypeKeyword | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_FINALLY]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FIXED]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FLOAT]                            = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_FOR]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_FOREACH]                          = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_GOTO]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_IF]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_IMPLICIT]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_IN]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_INT]                              = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_INTERFACE]                        = kCEETokenTypeKeyword | kCEETokenTypeObjectSpecifier;
    csharp_token_type_map[kCEETokenID_INTERNAL]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_IS]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_LOCK]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_LONG]                             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_NAMESPACE]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_NEW]                              = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_NULL]                             = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_OBJECT]                           = kCEETokenTypeKeyword | kCEETokenTypeBuildinReference;
    csharp_token_type_map[kCEETokenID_OPERATOR]                         = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_OUT]                              = kCEETokenTypeKeyword | kCEETokenTypeParameterSpecifier;
    csharp_token_type_map[kCEETokenID_OVERRIDE]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_PARAMS]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_PRIVATE]                          = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_PROTECTED]                        = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_PUBLIC]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_READONLY]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_REF]                              = kCEETokenTypeKeyword | kCEETokenTypeParameterSpecifier;
    csharp_token_type_map[kCEETokenID_RETURN]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_SBYTE]                            = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_SEALED]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_SHORT]                            = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_SIZEOF]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_STACKALLOC]                       = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_STATIC]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_STRING]                           = kCEETokenTypeKeyword | kCEETokenTypeBuildinReference;
    csharp_token_type_map[kCEETokenID_STRUCT]                           = kCEETokenTypeKeyword | kCEETokenTypeObjectSpecifier;
    csharp_token_type_map[kCEETokenID_SWITCH]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_THIS]                             = kCEETokenTypeKeyword | kCEETokenTypeParameterSpecifier;
    csharp_token_type_map[kCEETokenID_THROW]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_TRUE]                             = kCEETokenTypeKeyword | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_TRY]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_TYPEOF]                           = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_UINT]                             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_ULONG]                            = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_UNCHECKED]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_UNSAFE]                           = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_USHORT]                           = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_USING]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_VIRTUAL]                          = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_VOID]                             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    csharp_token_type_map[kCEETokenID_VOLATILE]                         = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_WHILE]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ADD]                              = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ALIAS]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ASCENDING]                        = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_ASYNC]                            = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    csharp_token_type_map[kCEETokenID_AWAIT]                            = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_BY]                               = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DESCENDING]                       = kCEETokenTypeKeyword;
    csharp_token_type_map[kCEETokenID_DYNAMIC]                          = kCEETokenTypeKeyword | kCEETokenTypeBuildinReference;
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
    csharp_token_type_map[kCEETokenID_VAR]                              = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
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
    csharp_token_type_map['+']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['-']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['*']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['/']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['%']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['&']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['|']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['^']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['!']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['~']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['=']                                          = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map['<']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['>']                                          = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map['?']                                          = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_NULL_COALESCING]                  = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_SCOPE]                            = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_INCREMENT]                        = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_DECREMENT]                        = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_LOGIC_AND]                        = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_OR]                         = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_MEMBER_POINTER]                   = kCEETokenTypePunctuation;
    csharp_token_type_map[kCEETokenID_LOGIC_EQUAL]                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_LOGIC_UNEQUAL]                    = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_LOGIC_LESS_EQUAL]                 = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]                = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    csharp_token_type_map[kCEETokenID_ADD_ASSIGNMENT]                   = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_MINUS_ASSIGNMENT]                 = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_MULTI_ASSIGNMENT]                 = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_DIV_ASSIGNMENT]                   = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_MOD_ASSIGNMENT]                   = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_AND_ASSIGNMENT]                   = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_OR_ASSIGNMENT]                    = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_XOR_ASSIGNMENT]                   = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_LEFT_OFFSET_ASSIGNMENT]           = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_RIGHT_OFFSET_ASSIGNMENT]          = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    csharp_token_type_map[kCEETokenID_LAMBDA]                           = kCEETokenTypePunctuation;
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

static cee_boolean token_id_is_integral_type(CEETokenID identifier)
{
    return (identifier == kCEETokenID_SBYTE ||
            identifier == kCEETokenID_BYTE ||
            identifier == kCEETokenID_SHORT ||
            identifier == kCEETokenID_USHORT ||
            identifier == kCEETokenID_INT ||
            identifier == kCEETokenID_UINT ||
            identifier == kCEETokenID_LONG ||
            identifier == kCEETokenID_ULONG);
}

static cee_boolean token_id_is_object_specifier(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypeObjectSpecifier) != 0;
}

static cee_boolean token_id_is_builtin_reference(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypeBuildinReference) != 0;
}

static cee_boolean token_id_is_parameter_specifier(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypeParameterSpecifier) != 0;
}

static cee_boolean token_id_is_overloadable(CEETokenID identifier)
{
    return (csharp_token_type_map[identifier] & kCEETokenTypeOverloadOperator) != 0;
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
    parser->symbol_search_in_scope = symbol_search_in_scope;
    
    CSharpParser* csharp = parser_create();
    csharp->super = parser;
    
    csharp_block_header_trap_init(csharp);
    csharp_token_type_map_init();
    
    csharp_object_type_definition_translate_table_init();
    csharp_enum_definition_translate_table_init();
    csharp_method_translate_table_init();
    csharp_method_parameters_declaration_translate_table_init();
    csharp_operator_translate_table_init();
    csharp_property_declaration_translate_table_init();
    csharp_event_declaration_table_init();
    csharp_declaration_translate_table_init();
    csharp_indexer_declaration_translate_table_init();
    csharp_namespace_definition_translate_table_init();
    csharp_using_directive_translate_table_init();
    csharp_delegate_declaration_translate_table_init();
    
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
    
    parser->block_header_traps[kCEETokenID_CLASS] = csharp_object_type_definition_trap;
    parser->block_header_traps[kCEETokenID_STRUCT] = csharp_object_type_definition_trap;
    parser->block_header_traps[kCEETokenID_INTERFACE] = csharp_object_type_definition_trap;
    parser->block_header_traps[kCEETokenID_ENUM] = csharp_enum_definition_trap;
    parser->block_header_traps[kCEETokenID_NAMESPACE] = csharp_namespace_definition_trap;
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
                                const cee_char* filepath,
                                const cee_char* subject,
                                CEESourceFragment** prep_directive,
                                CEESourceFragment** statement,
                                CEESourceFragment** comment,
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
            comment_fragment_reduce(parser);
            
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
                
                prep_directive_attach(parser, kCEESourceFragmentTypePrepDirective);
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
                    parser_state_clear(parser, kCSharpParserStateStatementParsing);
                               
                statement_token_push(parser, token);
                
                if (!block_reduce(parser))
                    parser_state_clear(parser, kCSharpParserStateStatementParsing);
            }
            else if (token->identifier == '[') {
                statement_token_push(parser, token);
                subscript_push(parser);
            }
            else if (token->identifier == ']') {
                if (!subscript_pop(parser))
                    parser_state_clear(parser, kCSharpParserStateStatementParsing);
                
                statement_token_push(parser, token);
            }
            else if (token->identifier == '(') {
                statement_token_push(parser, token);
                parameter_list_push(parser);
            }
            else if (token->identifier == ')') {
                if (!parameter_list_pop(parser))
                    parser_state_clear(parser, kCSharpParserStateStatementParsing);
                
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
            else if (token->identifier == kCEETokenID_LAMBDA) {
                statement_token_push(parser, token);
                statement_parse(parser);
                token = statement_token_pop(parser);
                statement_reduce(parser);
                statement_token_push(parser, token);
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
    
    cee_source_fragment_tree_symbols_parent_parse(parser->statement_root);
    
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
                                   const cee_char* filepath,
                                   const cee_char* subject,
                                   CEESourceTokenMap* source_token_map,
                                   CEESourceFragment* prep_directive,
                                   CEESourceFragment* statement,
                                   CEERange range,
                                   CEEList** references)
{
    CEESourceFragment* fragment = NULL;
    CEEList* p = NULL;
    
    CEESourceFragment* indexes[kCEESourceFragmentIndexMax];
    memset(indexes, 0, sizeof(CEESourceFragment*)*kCEESourceFragmentIndexMax);
    
    cee_source_fragment_indexes_in_range(source_token_map, range, indexes);
    
    for (cee_int i = kCEESourceFragmentIndexPrepDirective;
         i < kCEESourceFragmentIndexMax;
         i ++) {
        
        if (!indexes[i])
            continue;
        
        p = indexes[i]->node_ref;
        while (p) {
            fragment = p->data;
            
            if (!references_in_source_fragment_parse(parser_ref,
                                                     filepath,
                                                     fragment,
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

static cee_boolean references_in_source_fragment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
                                                       CEESourceFragment* fragment,
                                                       const cee_char* subject,
                                                       CEESourceFragment* prep_directive,
                                                       CEESourceFragment* statement,
                                                       CEERange range,
                                                       CEEList** references)
{
    CEESourceFragment* reference_fragment = NULL;
    CEEList* p = NULL;
    
    if (cee_source_fragment_over_range(fragment, range))
        return FALSE;
    
    reference_fragment = csharp_referernce_fragment_convert(fragment, subject);
    csharp_reference_fragment_parse(parser_ref,
                                    filepath,
                                    reference_fragment,
                                    subject,
                                    prep_directive,
                                    statement,
                                    references);
    cee_source_fragment_free(reference_fragment);
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fragment);
    while (p) {
        if (!references_in_source_fragment_parse(parser_ref,
                                                 filepath,
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

static CEESourceFragment* csharp_referernce_fragment_convert(CEESourceFragment* fragment,
                                                             const cee_char* subject)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEESourceFragment* reference_fragment = NULL;
    CEESourceFragment* current = reference_fragment;
    
    if (cee_source_fragment_parent_type_is(fragment, kCEESourceFragmentTypeSquareBracketList)) {
        reference_fragment = cee_source_fragment_create(kCEESourceFragmentTypeSquareBracketList,
                                                        fragment->filepath_ref,
                                                        fragment->subject_ref,
                                                        "csharp");
        reference_fragment = cee_source_fragment_sub_attach(reference_fragment,
                                                            kCEESourceFragmentTypeStatement,
                                                            fragment->filepath_ref,
                                                            fragment->subject_ref,
                                                            "csharp");
    }
    else {
        reference_fragment = cee_source_fragment_create(kCEESourceFragmentTypeStatement,
                                                        fragment->filepath_ref,
                                                        fragment->subject_ref,
                                                        "csharp");
    }
    
    current = reference_fragment;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    
    while (p) {
        token = p->data;
        
        if (!(token->state & kCEETokenStateSymbolOccupied)) {
            if (token->identifier == '{') {   /** ^{ */
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fragment_push(current,
                                                   kCEESourceFragmentTypeCurlyBracketList,
                                                   fragment->filepath_ref,
                                                   fragment->subject_ref,
                                                   "csharp");
            }
            else if (token->identifier == '}') {
                current = cee_source_fragment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else if (token->identifier == '[') {
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fragment_push(current,
                                                   kCEESourceFragmentTypeSquareBracketList,
                                                   fragment->filepath_ref,
                                                   fragment->subject_ref,
                                                   "csharp");
            }
            else if (token->identifier == ']') {
                current = cee_source_fragment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else if (token->identifier == '(') {
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fragment_push(current,
                                                   kCEESourceFragmentTypeRoundBracketList,
                                                   fragment->filepath_ref,
                                                   fragment->subject_ref,
                                                   "csharp");
            }
            else if (token->identifier == ')') {
                current = cee_source_fragment_pop(current);
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
        cee_source_fragment_free(reference_fragment);
    
    return current;
}

static void csharp_reference_fragment_parse(CEESourceParserRef parser_ref,
                                            const cee_char* filepath,
                                            CEESourceFragment* fragment,
                                            const cee_char* subject,
                                            CEESourceFragment* prep_directive,
                                            CEESourceFragment* statement,
                                            CEEList** references)
{
    CEEList* p = NULL;
    CEESourceSymbolReference* reference = NULL;
    CEEList* sub = NULL;
    CEESourceReferenceType type = kCEESourceReferenceTypeUnknow;
    
    if (!fragment)
        return;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        
        type = kCEESourceReferenceTypeUnknow;
        
        if (cee_source_fragment_tokens_pattern_match(fragment, p, '.', kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fragment_tokens_break(fragment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeMember;
        }
        else if (cee_source_fragment_tokens_pattern_match(fragment, p, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch any other identifier */
            p = cee_source_fragment_tokens_break(fragment, p, cee_range_make(0, 1), &sub);
        }
        else {
            p = TOKEN_NEXT(p);
        }
        
        if (sub) {
            reference = cee_source_symbol_reference_create(parser_ref,
                                                           (const cee_char*)filepath,
                                                           subject,
                                                           sub,
                                                           type);
            *references = cee_list_prepend(*references, reference);
            reference = NULL;
            sub = NULL;
        }
    }
    
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fragment);
    while (p) {
        csharp_reference_fragment_parse(parser_ref,
                                        filepath,
                                        p->data,
                                        subject,
                                        prep_directive,
                                        statement,
                                        references);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

static void symbol_parse_init(CSharpParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->prep_directive_root = cee_source_fragment_create(kCEESourceFragmentTypeRoot,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "csharp");
    parser->prep_directive_current = cee_source_fragment_sub_attach(parser->prep_directive_root, 
                                                                    kCEESourceFragmentTypeSourceList,
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    "csharp");
    parser->prep_directive_current = cee_source_fragment_sub_attach(parser->prep_directive_current, 
                                                                    kCEESourceFragmentTypePrepDirective,
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    "csharp");
    
    parser->statement_root = cee_source_fragment_create(kCEESourceFragmentTypeRoot,
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "csharp");
    parser->statement_current = cee_source_fragment_sub_attach(parser->statement_root, 
                                                               kCEESourceFragmentTypeSourceList,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "csharp");
    parser->statement_current = cee_source_fragment_sub_attach(parser->statement_current, 
                                                               kCEESourceFragmentTypeStatement,
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "csharp");
    
    parser->comment_root = cee_source_fragment_create(kCEESourceFragmentTypeRoot,
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "csharp");
    parser->comment_current = cee_source_fragment_sub_attach(parser->comment_root, 
                                                             kCEESourceFragmentTypeSourceList,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "csharp");
    parser->comment_current = cee_source_fragment_sub_attach(parser->comment_current, 
                                                             kCEESourceFragmentTypeComment,
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "csharp");
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
    CEESourceFragment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fragment_attach(parser->comment_current,
                                          kCEESourceFragmentTypeComment,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "csharp");
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

static cee_boolean comment_fragment_reduce(CSharpParser* parser)
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
    CEESourceFragment* fragment = parser->prep_directive_current;
    
    if (!fragment->tokens_ref)
        return FALSE;
    
    CEEList* p = SOURCE_FREGMENT_TOKENS_LAST(fragment);
    
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
                                         CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fragment_attach(parser->prep_directive_current,
                                          type,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "csharp");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static cee_boolean prep_directive_sub_attach(CSharpParser* parser,
                                             CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fragment_sub_attach(parser->prep_directive_current,
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              "csharp");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static void prep_directive_branch_push(CSharpParser* parser)
{
    prep_directive_sub_attach(parser, kCEESourceFragmentTypeSourceList);
    prep_directive_sub_attach(parser, kCEESourceFragmentTypePrepDirective);
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
    CEESourceFragment* fragment = parser->prep_directive_current;
    CEEToken* token = NULL;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
        
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
            cee_source_fragment_type_set(parser->prep_directive_current,
                                         kCEESourceFragmentTypePrepDirectiveCondition);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ELIF ||
                 token->identifier == kCEETokenID_HASH_ELSE) {
            parser_state_clear(parser, kCSharpParserStateStatementParsing);
            cee_source_fragment_type_set(parser->prep_directive_current,
                                         kCEESourceFragmentTypePrepDirectiveBranch);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ENDIF) {
            prep_directive_attach(parser, kCEESourceFragmentTypePrepDirective);
            if (should_statement_parsing(parser))
                parser_state_set(parser, kCSharpParserStateStatementParsing);
            else
                parser_state_clear(parser, kCSharpParserStateStatementParsing);
        }
        else {
            prep_directive_attach(parser, kCEESourceFragmentTypePrepDirective);
        }
    }
}

static cee_boolean should_statement_parsing(CSharpParser* parser)
{
    cee_boolean ret = TRUE;
    CEESourceFragment* current = parser->prep_directive_current->parent;
    while (current) {
        if (cee_source_fragment_type_is(current, kCEESourceFragmentTypeRoot)) {
            break;
        }
        else if (cee_source_fragment_type_is(current, kCEESourceFragmentTypePrepDirectiveBranch)) {
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
    CEESourceFragment* current = parser->statement_current;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    ParseTrap trap = NULL;
        
    if (!current || !current->tokens_ref)
        return;
    
    if (csharp_method_parse(current) ||
        csharp_operator_parse(current) ||
        csharp_property_declaration_parse(current) ||
        csharp_event_declaration_parse(current) ||
        csharp_indexer_declaration_parse(current))
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
    
    if (statement_block_parse(current))
        return;
    
    return;
}

static cee_boolean statement_block_parse(CEESourceFragment* fragment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    
    if (cee_source_fragment_type_is(fragment, kCEESourceFragmentTypeAssignmentBlock) ||
        cee_source_fragment_type_is(fragment, kCEESourceFragmentTypeStatementBlock))
        return TRUE;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (token->identifier == '<') {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                return FALSE;
        }
        else {
            if (token_id_is_assignment(token->identifier)) {
                cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeAssignmentBlock);
                break;
            }
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (!cee_source_fragment_type_is(fragment, kCEESourceFragmentTypeAssignmentBlock))
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeStatementBlock);
    
    return TRUE;
}

static void block_push(CSharpParser* parser)
{
    statement_sub_attach(parser, kCEESourceFragmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFragmentTypeStatement);
}

static cee_boolean block_pop(CSharpParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void block_parse(CSharpParser* parser)
{
    csharp_enumerators_parse(parser->statement_current);
}

static cee_boolean block_reduce(CSharpParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fragment_symbols_fragment_range_mark(parser->statement_current);
    
    if (cee_source_fragment_type_is(parser->statement_current, kCEESourceFragmentTypeAssignmentBlock)) {
        /** expect statement terminate ';' */
    }
    else {
        statement_attach(parser, kCEESourceFragmentTypeStatement);
    }
    return TRUE;
}

static void label_parse(CSharpParser* parser)
{
    CEESourceFragment* fragment = NULL;
    const cee_char* subject = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEESourceSymbol* declaration = NULL;
    
    fragment = parser->statement_current;
    if (!fragment || !fragment->tokens_ref)
        return;
    
    if (!cee_source_fragment_grandfather_type_is(fragment, kCEESourceFragmentTypeFunctionDefinition) &&
        !cee_source_fragment_grandfather_type_is(fragment, kCEESourceFragmentTypeStatementBlock))
        return;
    
    subject = parser->subject_ref;
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
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
                    declaration = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                            subject,
                                                                            p,
                                                                            p,
                                                                            kCEESourceSymbolTypeLabel,
                                                                            "csharp",
                                                                            kCEETokenStringOptionCompact);
                    cee_token_slice_state_mark(p, q, kCEETokenStateSymbolOccupied);
                    
                    cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeLabelDeclaration);
                    fragment->symbols = cee_list_prepend(fragment->symbols, declaration);
                }
                break;
            }
            else
                break;
        }
    }
    
#ifdef DEBUG_LABEL
    if (declaration)
        cee_source_symbol_print(declaration);
#endif
    
}

static cee_boolean label_reduce(CSharpParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    if (cee_source_fragment_type_is(parser->statement_current, kCEESourceFragmentTypeLabelDeclaration))
        statement_attach(parser, kCEESourceFragmentTypeStatement);
    
    return TRUE;
}

/**
 * statement
 */
static void statement_parse(CSharpParser* parser)
{
    CEESourceFragment* current = parser->statement_current;
    if (!current || !current->tokens_ref)
        return;
        
    if (csharp_method_parse(current) ||
        csharp_declaration_parse(current) ||
        csharp_operator_parse(current) ||
        csharp_property_declaration_parse(current) ||
        csharp_event_declaration_parse(current) ||
        csharp_indexer_declaration_parse(current) ||
        csharp_using_directive_parse(current) ||
        csharp_delegate_declaration_parse(current))
        return;
    
    return;
}

static cee_boolean statement_reduce(CSharpParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
        
    statement_attach(parser, kCEESourceFragmentTypeStatement);
    
    return TRUE;
}
static cee_boolean statement_parsing(CSharpParser* parser)
{
    return parser->state & kCSharpParserStateStatementParsing;
}

static cee_boolean statement_attach(CSharpParser* parser,
                                    CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fragment_attach(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "csharp");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(CSharpParser* parser,
                                        CEESourceFragmentType type)
{
    CEESourceFragment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fragment_sub_attach(parser->statement_current, 
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              "csharp");
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

static CEEToken* statement_token_pop(CSharpParser* parser)
{
    if (!parser->statement_current)
        return NULL;
    SOURCE_FREGMENT_TOKEN_POP(parser->statement_current);
}


/**
 * parameter list
 */
static void parameter_list_push(CSharpParser* parser)
{
    statement_sub_attach(parser, kCEESourceFragmentTypeRoundBracketList);
    statement_sub_attach(parser, kCEESourceFragmentTypeStatement);
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
    statement_sub_attach(parser, kCEESourceFragmentTypeSquareBracketList);
    statement_sub_attach(parser, kCEESourceFragmentTypeStatement);
}

static cee_boolean subscript_pop(CSharpParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void csharp_declaration_translate_table_init(void)
{
    /**
     *                          [                       ]                       using       (       )       declaration_specifier   builtin_type    builtin_reference   identifier      ?                   assign_operator     .                       <               ,       ;
     *  Init                    Init                    Init                    Using       Error   Error   DeclarationSpecifier    BuiltinType     BuiltinReference    CustomType      Error               Error               Error                   Error           Error   Error
     *  Using                   Error                   Error                   Error       Using   Using   DeclarationSpecifier    BuiltinType     BuiltinReference    CustomType      Error               Error               Error                   Error           Error   Error
     *  DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    Error       Error   Error   DeclarationSpecifier    BuiltinType     BuiltinReference    CustomType      Error               Error               Error                   Error           Error   Error
     *  BuiltinType             BuiltinType             BuiltinType             Error       Error   Error   Error                   BuiltinType     BuiltinReference    Identifier      BuiltinType         Error               Error                   Error           Error   Error
     *  BuiltinReference        BuiltinReference        BuiltinReference        Error       Error   Error   Error                   BuiltinType     BuiltinReference    Identifier      BuiltinReference    Error               Error                   Error           Error   Error
     *  CustomType              CustomType              CustomType              Error       Error   Error   Error                   Error           Error               Identifier      CustomType          Error               CustomTypeSeparator     *TypeParameters Error   Error
     *  Identifier              Identifier              Identifier              Error       Error   Error   Error                   Error           Error               Identifier      Error               Confirm             Error                   Error           Confirm Confirm
     *  CustomTypeSeparator     Error                   Error                   Error       Error   Error   Error                   Error           Error               CustomType      Error               Error               Error                   Error           Error   Error
     *  Confirm                 Error                   Error                   Error       Error   Error   Error                   Error           Error               Identifier      Error               Error               Error                   Error           Confirm Confirm
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateMax                     , kCSharpDeclarationTranslateStateError                                                     );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateInit                    , '['                               , kCSharpDeclarationTranslateStateInit                  );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateInit                    , ']'                               , kCSharpDeclarationTranslateStateInit                  );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateInit                    , kCEETokenID_USING                 , kCSharpDeclarationTranslateStateUsing                 );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateInit                    , token_id_is_declaration_specifier , kCSharpDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateInit                    , token_id_is_builtin_type          , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateInit                    , token_id_is_builtin_reference     , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateInit                    , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateUsing                   , '('                               , kCSharpDeclarationTranslateStateUsing                 );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateUsing                   , ')'                               , kCSharpDeclarationTranslateStateUsing                 );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateUsing                   , token_id_is_declaration_specifier , kCSharpDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateUsing                   , token_id_is_builtin_type          , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateUsing                   , token_id_is_builtin_reference     , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateUsing                   , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateDeclarationSpecifier    , '['                               , kCSharpDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateDeclarationSpecifier    , ']'                               , kCSharpDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateDeclarationSpecifier    , token_id_is_declaration_specifier , kCSharpDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateDeclarationSpecifier    , token_id_is_builtin_type          , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateDeclarationSpecifier    , token_id_is_builtin_reference     , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateDeclarationSpecifier    , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinType             , '['                               , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinType             , ']'                               , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinType             , token_id_is_builtin_type          , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinType             , token_id_is_builtin_reference     , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinType             , '?'                               , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinType             , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinReference        , '['                               , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinReference        , ']'                               , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinReference        , token_id_is_builtin_type          , kCSharpDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinReference        , token_id_is_builtin_reference     , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinReference        , '?'                               , kCSharpDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateBuiltinReference        , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateCustomType              , '['                               , kCSharpDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateCustomType              , ']'                               , kCSharpDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateCustomType              , '?'                               , kCSharpDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateCustomType              , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateCustomType              , '.'                               , kCSharpDeclarationTranslateStateCustomTypeSeparator   );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateCustomType              , '<'                               , kCSharpDeclarationTranslateStateTypeParameters        );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateCustomTypeSeparator     , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateIdentifier              , '['                               , kCSharpDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateIdentifier              , ']'                               , kCSharpDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateIdentifier              , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateIdentifier            );
    TRANSLATE_FUNCS_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateIdentifier              , token_id_is_assignment            , kCSharpDeclarationTranslateStateConfirm               );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateIdentifier              , ','                               , kCSharpDeclarationTranslateStateConfirm               );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateIdentifier              , ';'                               , kCSharpDeclarationTranslateStateConfirm               );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateConfirm                 , kCEETokenID_IDENTIFIER            , kCSharpDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateConfirm                 , ','                               , kCSharpDeclarationTranslateStateConfirm               );
    TRANSLATE_STATE_SET(csharp_declaration_translate_table, kCSharpDeclarationTranslateStateConfirm                 , ';'                               , kCSharpDeclarationTranslateStateConfirm               );
}

static cee_boolean csharp_declaration_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    CEEList* identifier = NULL;
    CEEList* prefix = NULL;
    CEEList* prefix_tail = NULL;
    cee_boolean prefix_parse = FALSE;
    CSharpDeclarationTranslateState current = kCSharpDeclarationTranslateStateInit;
    CSharpDeclarationTranslateState prev = kCSharpDeclarationTranslateStateInit;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
                
        prev = current;
        current = csharp_declaration_translate_table[current][token->identifier];
        
        if (current == kCSharpDeclarationTranslateStateError) {
            break;
        }
        else if (current == kCSharpDeclarationTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kCSharpDeclarationTranslateStateConfirm) {
            if (!prefix_parse) {
                prefix = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
                if (identifier)
                    prefix_tail = TOKEN_PREV(identifier);
                prefix_parse = TRUE;
            }
            
            if (identifier)
                declaration = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                        fragment->subject_ref,
                                                                        identifier,
                                                                        identifier,
                                                                        kCEESourceSymbolTypeVariableDeclaration,
                                                                        "csharp",
                                                                        kCEETokenStringOptionCompact);
            if (declaration) {
                cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
                if (prefix && prefix_tail)
                    declaration->proto_descriptor = csharp_declaration_proto_descriptor_create(fragment,
                                                                                               prefix,
                                                                                               prefix_tail,
                                                                                               identifier);
                declarations = cee_list_prepend(declarations, declaration);
            }
            
            declaration = NULL;
            identifier = NULL;
            
            if (!cee_token_is_identifier(p, ',') &&
                !cee_token_is_identifier(p, ';')) {
                p = skip_csharp_declaration_interval(p);
                if (!p)
                    break;
            }
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (declarations) {
        fragment->symbols = cee_list_concat(fragment->symbols, declarations);
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_DECLARATION
    if (declarations)
        cee_source_symbols_print(declarations);
#endif
    
exit:
    
    return ret;
}

static void csharp_object_type_definition_translate_table_init(void)
{
    /**
     *                              object_specifier        identifier                  struct      new         where               <                   :               ,               .                       (               )               {
     *  Init                        ObjectTypeSpecifier     Error                       Error       Error       Error               Error               Error           Error           Error                   Error           Error           Error
     *  ObjectTypeSpecifier         Error                   Identifier                  Error       Error       Error               Error               Error           Error           Error                   Error           Error           Error
     *  Identifier                  Error                   Error                       Error       Error       ConstraintClause    *TypeParameters     Base            Comma           IdentifierSeparator     Error           Error           Confirm
     *  IdentifierSeparator         Error                   Identifier                  Error       Error       Error               Error               Error           Error           Error                   Error           Error           Error
     *  Base                        Error                   Identifier                  Error       Error       Error               Error               Error           Error           Error                   Error           Error           Error
     *  Comma                       Error                   Identifier                  Error       Error       Error               Error               Error           Error           Error                   Error           Error           Error
     *  ConstraintClause            Error                   ConstraintsTypeParameter    Error       Error       Error               Error               Error           Error           Error                   Error           Error           Error
     *  ConstraintsTypeParameter    Error                   Error                       Error       Error       Error               *TypeParameters     Constraints     Error           Error                   Error           Error           Error
     *  Constraints                 Constraint              Constraint                  Constraint  Constructor Error               Error               Error           Error           Error                   Error           Error           Error
     *  Constraint                  Constraint              Constraint                  Constraint  Constructor ConstraintClause    *TypeParameters     Error           Constraint      Constraint              Error           Error           Confirm
     *  Constructor                 Error                   Error                       Error       Error       Error               Error               Error           Error           Error                   ConstructorList Error           Error
     *  ConstructorList             Error                   Error                       Error       Error       Error               Error               Error           Error           Error                   Error           Constraint      Error
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateMax                        , kCSharpObjectTypeDefinitionTranslateStateError                                                    );
    TRANSLATE_FUNCS_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateInit                       , token_id_is_object_specifier  , kCSharpObjectTypeDefinitionTranslateStateObjectTypeSpecifier      );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateObjectTypeSpecifier        , kCEETokenID_IDENTIFIER        , kCSharpObjectTypeDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateIdentifier                 , kCEETokenID_WHERE             , kCSharpObjectTypeDefinitionTranslateStateConstraintClause         );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateIdentifier                 , '<'                           , kCSharpObjectTypeDefinitionTranslateStateTypeParameters           );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateIdentifier                 , ':'                           , kCSharpObjectTypeDefinitionTranslateStateBase                     );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateIdentifier                 , ','                           , kCSharpObjectTypeDefinitionTranslateStateComma                    );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateIdentifier                 , '.'                           , kCSharpObjectTypeDefinitionTranslateStateIdentifierSeparator      );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateIdentifier                 , '{'                           , kCSharpObjectTypeDefinitionTranslateStateConfirm                  );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateIdentifierSeparator        , kCEETokenID_IDENTIFIER        , kCSharpObjectTypeDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateBase                       , kCEETokenID_IDENTIFIER        , kCSharpObjectTypeDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateComma                      , kCEETokenID_IDENTIFIER        , kCSharpObjectTypeDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraintClause           , kCEETokenID_IDENTIFIER        , kCSharpObjectTypeDefinitionTranslateStateConstraintsTypeParameter );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraintsTypeParameter   , '<'                           , kCSharpObjectTypeDefinitionTranslateStateTypeParameters           );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraintsTypeParameter   , ':'                           , kCSharpObjectTypeDefinitionTranslateStateConstraints              );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraints                , kCEETokenID_CLASS             , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraints                , kCEETokenID_IDENTIFIER        , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraints                , kCEETokenID_STRUCT            , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraints                , kCEETokenID_NEW               , kCSharpObjectTypeDefinitionTranslateStateConstructor              );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , kCEETokenID_CLASS             , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , kCEETokenID_IDENTIFIER        , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , kCEETokenID_STRUCT            , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , kCEETokenID_NEW               , kCSharpObjectTypeDefinitionTranslateStateConstructor              );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , kCEETokenID_WHERE             , kCSharpObjectTypeDefinitionTranslateStateConstraintClause         );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , '<'                           , kCSharpObjectTypeDefinitionTranslateStateTypeParameters           );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , ','                           , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstructor                , '('                           , kCSharpObjectTypeDefinitionTranslateStateConstructorList          );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstructorList            , ')'                           , kCSharpObjectTypeDefinitionTranslateStateConstraint               );
    TRANSLATE_STATE_SET(csharp_object_type_definition_translate_table,  kCSharpObjectTypeDefinitionTranslateStateConstraint                 , '{'                           , kCSharpObjectTypeDefinitionTranslateStateConfirm                  );
}

static cee_boolean csharp_object_type_definition_trap(CEESourceFragment* fragment,
                                                      CEEList** pp)
{
    enum _State {
        kStateObjectType = 0,
        kStateObjectTypeBase,
        kStateConstraint,
    } state = kStateObjectType;
        
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    CSharpObjectTypeDefinitionTranslateState current = kCSharpObjectTypeDefinitionTranslateStateInit;
    CSharpObjectTypeDefinitionTranslateState prev = kCSharpObjectTypeDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    
    CEEList* class_identifier = NULL;
    
    CEEList* base = NULL;
    CEEList* bases = NULL;
    cee_char* bases_str = NULL;
        
    CEESourceSymbol* definition = NULL;
    CEESourceSymbolType definition_type = kCEESourceSymbolTypeUnknow;
    CEESourceFragmentType fragment_type = kCEESourceFragmentTypeRoot;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_object_type_definition_translate_table[current][token->identifier];
        
        if (current == kCSharpObjectTypeDefinitionTranslateStateObjectTypeSpecifier) {
            if (cee_token_is_identifier(p, kCEETokenID_CLASS)) {
                definition_type = kCEESourceSymbolTypeClassDefinition;
                fragment_type = kCEESourceFragmentTypeClassDefinition;
            }
            else if (cee_token_is_identifier(p, kCEETokenID_STRUCT)) {
                definition_type = kCEESourceSymbolTypeStructDefinition;
                fragment_type = kCEESourceFragmentTypeStructDefinition;
            }
            else if (cee_token_is_identifier(p, kCEETokenID_INTERFACE)) {
                definition_type = kCEESourceSymbolTypeInterfaceDefinition;
                fragment_type = kCEESourceFragmentTypeInterfaceDefinition;
            }
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateBase) {
            state = kStateObjectTypeBase;
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateConstraintClause) {
            state = kStateConstraint;
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateComma) {
            if (cee_token_is_identifier(p, ',')) {
                if (state == kStateObjectTypeBase) {
                    if (base) {
                        bases = cee_list_prepend(bases, base);
                        base = NULL;
                    }
                }
            }
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (state == kStateObjectType)
                    TOKEN_APPEND(class_identifier, token);
                else if (state == kStateObjectTypeBase)
                    TOKEN_APPEND(base, token);
            }
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateConstraintClause) {
            state = kStateConstraint;
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateConfirm) {
            if (base) {
                bases = cee_list_prepend(bases, base);
                base = NULL;
            }
            
            ret = TRUE;
            break;
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateError) {
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCSharpObjectTypeDefinitionTranslateStateConfirm)
        goto exit;
    
    p = TOKEN_LAST(class_identifier);
    definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                           fragment->subject_ref,
                                                           p,
                                                           p,
                                                           definition_type,
                                                           "csharp",
                                                           kCEETokenStringOptionCompact);
    if (definition) {
        
        if (bases)
            bases_str = csharp_name_scope_list_string_create(bases,
                                                             fragment->subject_ref);
        
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        definition->derives = bases_str;
        definition->proto_descriptor = csharp_object_type_proto_descriptor_create(fragment,
                                                                                  definition,
                                                                                  definition->derives);
        fragment->symbols = cee_list_prepend(fragment->symbols, definition);
        cee_source_fragment_type_set_exclusive(fragment, fragment_type);
    }
    
    *pp = NULL;
        
#ifdef DEBUG_CLASS
    if (declaration)
        cee_source_symbol_print(definition);
#endif
    
exit:
    
    if (class_identifier)
        cee_list_free(class_identifier);
    
    if (base)
        cee_list_free(base);
        
    if (bases)
        cee_list_free_full(bases, base_free);
        
    return ret;
}

static void csharp_enum_definition_translate_table_init(void)
{
    
    /**
     *                  enum            identifier  :       integral_type   {
     *  Init            Enum            Error       Error   Error           Error
     *  Enum            Error           Identifier  Error   Error           Error
     *  Identifier      Error           Error       Base    Error           Confirm
     *  Base            Error           Error       Error   IntegralType    Error
     *  IntegralType    Error           Error       Error   Error           Confirm
     */
    TRANSLATE_STATE_INI(csharp_enum_definition_translate_table, kCSharpEnumDefinitionTranslateStateMax              , kCSharpEnumDefinitionTranslateStateError                                      );
    TRANSLATE_STATE_SET(csharp_enum_definition_translate_table, kCSharpEnumDefinitionTranslateStateInit             , kCEETokenID_ENUM          , kCSharpEnumDefinitionTranslateStateEnum           );
    TRANSLATE_STATE_SET(csharp_enum_definition_translate_table, kCSharpEnumDefinitionTranslateStateEnum             , kCEETokenID_IDENTIFIER    , kCSharpEnumDefinitionTranslateStateIdentifier     );
    TRANSLATE_STATE_SET(csharp_enum_definition_translate_table, kCSharpEnumDefinitionTranslateStateIdentifier       , ':'                       , kCSharpEnumDefinitionTranslateStateBase           );
    TRANSLATE_STATE_SET(csharp_enum_definition_translate_table, kCSharpEnumDefinitionTranslateStateIdentifier       , '{'                       , kCSharpEnumDefinitionTranslateStateConfirm        );
    TRANSLATE_FUNCS_SET(csharp_enum_definition_translate_table, kCSharpEnumDefinitionTranslateStateBase             , token_id_is_integral_type , kCSharpEnumDefinitionTranslateStateIntegralType   );
    TRANSLATE_STATE_SET(csharp_enum_definition_translate_table, kCSharpEnumDefinitionTranslateStateIntegralType     , '{'                       , kCSharpEnumDefinitionTranslateStateConfirm        );
}

static cee_boolean csharp_enum_definition_trap(CEESourceFragment* fragment,
                                               CEEList** pp)
{
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    CEEToken* token = NULL;
    CEEList* identifier = NULL;
    CEEList* base = NULL;
    cee_char* bases_str = NULL;
    CEESourceSymbol* definition = NULL;
    CSharpEnumDefinitionTranslateState current = kCSharpEnumDefinitionTranslateStateInit;
    CSharpEnumDefinitionTranslateState prev = kCSharpEnumDefinitionTranslateStateInit;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_enum_definition_translate_table[current][token->identifier];
        
        if (current == kCSharpEnumDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpEnumDefinitionTranslateStateIntegralType) {
            if (token_id_is_integral_type(token->identifier))
                base = p;
        }
        else if (current == kCSharpEnumDefinitionTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kCSharpEnumDefinitionTranslateStateConfirm) {
            ret = TRUE;
            break;
        }
        else if (current == kCSharpEnumDefinitionTranslateStateError) {
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCSharpEnumDefinitionTranslateStateConfirm)
        goto exit;
    
    definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                           fragment->subject_ref,
                                                           identifier,
                                                           identifier,
                                                           kCEESourceSymbolTypeEnumDefinition,
                                                           "csharp",
                                                           kCEETokenStringOptionCompact);
    if (definition) {
        
        if (base)
            bases_str = cee_string_from_token(fragment->subject_ref, base->data);
        
        cee_token_slice_state_mark(identifier,
                                   identifier,
                                   kCEETokenStateSymbolOccupied);
        
        definition->derives = bases_str;
        definition->proto_descriptor = csharp_object_type_proto_descriptor_create(fragment,
                                                                                  definition,
                                                                                  definition->derives);
        fragment->symbols = cee_list_prepend(fragment->symbols, definition);
        cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeEnumDefinition);
    }
    
    *pp = NULL;
    
#ifdef DEBUG_ENUM
    if (declaration)
        cee_source_symbol_print(definition);
#endif
    
exit:
    
    return ret;
}

static cee_boolean csharp_enumerators_parse(CEESourceFragment* fragment)
{
    if (!cee_source_fragment_grandfather_type_is(fragment, kCEESourceFragmentTypeEnumDefinition))
        return FALSE;
    
    cee_boolean ret = FALSE;
    CEESourceFragment* grandfather = NULL;
    CEESourceSymbol* enum_symbol = NULL;
    CEEList* enum_symbols = NULL;
    CEESourceSymbol* enumerator = NULL;
    CEEList* enumerators = NULL;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    cee_boolean enumurating = TRUE;
    
    grandfather = cee_source_fragment_grandfather_get(fragment);
    enum_symbols = cee_source_fragment_symbols_search_by_type(grandfather,
                                                              kCEESourceSymbolTypeEnumDefinition);
    if (enum_symbols)
        enum_symbol = cee_list_nth_data(enum_symbols, 0);
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        if (token->identifier == kCEETokenID_IDENTIFIER && enumurating)
            q = p;
        
        if (token->identifier == ',' || token->identifier == ';' || !TOKEN_NEXT(p)) {
            if (q) {
                enumerator = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                       fragment->subject_ref,
                                                                       q,
                                                                       q,
                                                                       kCEESourceSymbolTypeEnumerator,
                                                                       "csharp",
                                                                       kCEETokenStringOptionCompact);
                if (enumerator) {
                    enumerator->proto_descriptor =
                        csharp_enumerator_proto_descriptor_create(token->fragment_ref,
                                                                  enumerator,
                                                                  enum_symbol->name);
                    cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
                    enumerators = cee_list_prepend(enumerators, enumerator);
                }
            }
            q = NULL;
            enumurating = TRUE;
        }
        else if (token_id_is_assignment(token->identifier) && enumurating) {
            enumurating = FALSE;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (enum_symbols)
        cee_list_free(enum_symbols);
    
    if (enumerators) {
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeEnumerators);
        fragment->symbols = cee_list_concat(fragment->symbols, enumerators);
        ret = TRUE;
    }
    
#ifdef DEBUG_ENUM
    if (enumerators)
        cee_source_symbols_print(enumerators);
#endif
    
exit:
    return ret;
}

static cee_char* csharp_enumerator_proto_descriptor_create(CEESourceFragment* fragment,
                                                           CEESourceSymbol* enumerator,
                                                           const cee_char* proto)
{
    if (!fragment || !enumerator)
        return NULL;
    
    cee_char* descriptor = NULL;
    cee_strconcat0(&descriptor, "{", NULL);
        
    cee_strconcat0(&descriptor, "\"type\":", "\"enumerator\"", NULL);
    
    if (proto)
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto, "\"", NULL);
    
    if (enumerator->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", enumerator->name, "\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static void base_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_list_free((CEEList*)data);
}

static cee_char* csharp_object_type_proto_descriptor_create(CEESourceFragment* fragment,
                                                            CEESourceSymbol* definition,
                                                            const cee_char* derives_str)
{
    if (!fragment || !definition)
        return NULL;
    
    CEESourceFragment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* descriptor = NULL;
    
    access_level = csharp_access_level_search(fragment, SOURCE_FREGMENT_TOKENS_FIRST(fragment), NULL);
    if(!access_level) {
        grandfather = cee_source_fragment_grandfather_get(fragment);
        if (grandfather) {
            if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    if (definition->type == kCEESourceSymbolTypeClassDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"class\"", NULL);
    if (definition->type == kCEESourceSymbolTypeStructDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"struct\"", NULL);
    else if (definition->type == kCEESourceSymbolTypeInterfaceDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"interface\"", NULL);
    else if (definition->type == kCEESourceSymbolTypeEnumDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"enum\"", NULL);
    
    if (definition->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", definition->name, "\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (derives_str)
        cee_strconcat0(&descriptor, ", \"derivers\":", "\"", derives_str, "\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static cee_char* csharp_declaration_proto_descriptor_create(CEESourceFragment* fragment,
                                                            CEEList* prefix,
                                                            CEEList* prefix_tail,
                                                            CEEList* identifier)
{
    if (!fragment || !identifier)
        return NULL;
    
    CEESourceFragment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* proto = NULL;
    cee_char* descriptor = NULL;
    const cee_char* subject = fragment->subject_ref;
    cee_char* identifier_str = NULL;
    
    access_level = csharp_access_level_search(fragment, SOURCE_FREGMENT_TOKENS_FIRST(fragment), NULL);
    if (!access_level) {
        grandfather = cee_source_fragment_grandfather_get(fragment);
        if (grandfather) {
            if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"declaration\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto = csharp_type_descriptor_from_token_slice(fragment, prefix, prefix_tail);
    if (proto) {
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto, "\"", NULL);
        cee_free(proto);
    }
    
    if (identifier)
        identifier_str = cee_string_from_token(subject, identifier->data);
    if (identifier_str) {
        cee_strconcat0(&descriptor, ", \"name\":", "\"", identifier_str, "\"", NULL);
        cee_free(identifier_str);
    }
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static cee_char* csharp_namespace_proto_descriptor_create(CEESourceFragment* fragment,
                                                          CEESourceSymbol* definition)
{
    if (!fragment || !definition)
        return NULL;
    
    cee_char* descriptor = NULL;
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"namespace\"", ",", NULL);
    
    if (definition->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", definition->name, "\"", ",", NULL);
        
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static cee_char* csharp_name_scope_list_string_create(CEEList* scopes,
                                                      const cee_char* subject)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    cee_char* str = NULL;
    cee_char* scopes_str = NULL;
    
    p = cee_list_last(scopes);
    while (p) {
        q = p->data;
        str = csharp_name_scope_create(q, subject);
        if (str) {
            cee_strconcat0(&scopes_str, str, NULL);
            if (p->prev)
                cee_strconcat0(&scopes_str, ",", NULL);
            cee_free(str);
        }
        p = p->prev;
    }
    return scopes_str;
}

static cee_char* csharp_name_scope_create(CEEList* tokens,
                                          const cee_char* subject)
{
    CEEList* p = NULL;
    cee_char* scope = NULL;
    cee_char* str = NULL;
    CEEToken* token = NULL;
    
    p = TOKEN_FIRST(tokens);
    while (p) {
        token = p->data;
        str = cee_string_from_token(subject, token);
        if (str) {
            cee_strconcat0(&scope, str, NULL);
            if (TOKEN_NEXT(p))
                cee_strconcat0(&scope, ".", NULL);
            cee_free(str);
        }
        p = TOKEN_NEXT(p);
    }
    return scope;
}

static void csharp_method_translate_table_init(void)
{
    /**
     *                              [                       ]                       declaration_specifier       builtin_type        builtin_reference       ?                   identifier                  ~                       .                       <                   (               )                   where               :                       struct          new             ,           base                        this                        {           ;               =>
     *  Init                        Init                    Init                    DeclarationSpecifier        BuiltinType         BuiltinReference        Error               Identifier                  Init                    Error                   Error               Error           Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  DeclarationSpecifier        DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier        BuiltinType         BuiltinReference        Error               Identifier                  DeclarationSpecifier    Error                   Error               Error           Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  BuiltinType                 BuiltinType             BuiltinType             Error                       BuiltinType         BuiltinReference        BuiltinType         Identifier                  BuiltinType             Error                   Error               Error           Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  BuiltinReference            BuiltinReference        BuiltinReference        Error                       BuiltinType         BuiltinReference        BuiltinReference    Identifier                  BuiltinReference        Error                   Error               Error           Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  Identifier                  Identifier              Identifier              Error                       BuiltinType         BuiltinReference        Identifier          Identifier                  Error                   IdentifierSeparator     *TypeParameters     ParameterList   Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  IdentifierSeparator         Error                   Error                   Error                       Error               Error                   Error               Identifier                  Error                   Error                   Error               Error           Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  ParameterList               Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               Error           ParameterListEnd    Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  ParameterListEnd            Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               Error           Error               ConstraintClause    ConstructorInitializer  Error           Error           Error       Error                       Error                       Definition  Declaration     Definition
     *  ConstraintClause            Error                   Error                   Error                       Error               Error                   Error               ConstraintsTypeParameter    Error                   Error                   Error               Error           Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  ConstraintsTypeParameter    Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   *TypeParameters     Error           Error               Error               Constraints             Error           Error           Error       Error                       Error                       Error       Error           Error
     *  Constraints                 Error                   Error                   Error                       Constraint          Constraint              Error               Constraint                  Error                   Error                   Error               Error           Error               Error               Error                   Constraint      Constructor     Error       Error                       Error                       Error       Error           Error
     *  Constraint                  Error                   Error                   Error                       Constraint          Constraint              Error               Constraint                  Error                   Constraint              *TypeParameters     Error           Error               ConstraintClause    Error                   Constraint      Constructor     Constraint  Error                       Error                       Definition  Declaration     Definition
     *  Constructor                 Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               ConstructorList Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  ConstructorList             Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               Error           Constraint          Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  ConstructorInitializer      Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               Error           Constraint          Error               Error                   Error           Error           Error       ConstructorInitializerBase  ConstructorInitializerThis  Error       Error           Error
     *  ConstructorInitializerBase  Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               InitializerList Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  ConstructorInitializerThis  Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               InitializerList Error               Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  InitializerList             Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               Error           InitializerListEnd  Error               Error                   Error           Error           Error       Error                       Error                       Error       Error           Error
     *  InitializerListEnd          Error                   Error                   Error                       Error               Error                   Error               Error                       Error                   Error                   Error               Error           Error               Error               Error                   Error           Error           Error       Error                       Error                       Definition  Declaration     Definition
     *
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_method_translate_table,   kCSharpMethodTranslateStateMax                         , kCSharpMethodTranslateStateError                                                          );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInit                        , '['                               , kCSharpMethodTranslateStateInit                       );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInit                        , ']'                               , kCSharpMethodTranslateStateInit                       );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInit                        , token_id_is_declaration_specifier , kCSharpMethodTranslateStateDeclarationSpecifier       );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInit                        , token_id_is_builtin_type          , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInit                        , token_id_is_builtin_reference     , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInit                        , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInit                        , '~'                               , kCSharpMethodTranslateStateInit                       );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateDeclarationSpecifier        , '['                               , kCSharpMethodTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateDeclarationSpecifier        , ']'                               , kCSharpMethodTranslateStateDeclarationSpecifier       );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateDeclarationSpecifier        , token_id_is_declaration_specifier , kCSharpMethodTranslateStateDeclarationSpecifier       );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateDeclarationSpecifier        , token_id_is_builtin_type          , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateDeclarationSpecifier        , token_id_is_builtin_reference     , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateDeclarationSpecifier        , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateDeclarationSpecifier        , '~'                               , kCSharpMethodTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinType                 , '['                               , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinType                 , ']'                               , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinType                 , '?'                               , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinType                 , token_id_is_builtin_type          , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinType                 , token_id_is_builtin_reference     , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinType                 , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinType                 , '~'                               , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinReference            , '['                               , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinReference            , ']'                               , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinReference            , '?'                               , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinReference            , token_id_is_builtin_type          , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinReference            , token_id_is_builtin_reference     , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinReference            , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateBuiltinReference            , '~'                               , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , '['                               , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , ']'                               , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , '?'                               , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , token_id_is_builtin_type          , kCSharpMethodTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , token_id_is_builtin_reference     , kCSharpMethodTranslateStateBuiltinReference           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , '.'                               , kCSharpMethodTranslateStateIdentifierSeparator        );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , '<'                               , kCSharpMethodTranslateStateTypeParameters             );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifier                  , '('                               , kCSharpMethodTranslateStateParameterList              );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateIdentifierSeparator         , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateParameterList               , ')'                               , kCSharpMethodTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateParameterListEnd            , kCEETokenID_WHERE                 , kCSharpMethodTranslateStateConstraintClause           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateParameterListEnd            , ':'                               , kCSharpMethodTranslateStateConstructorInitializer     );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateParameterListEnd            , '{'                               , kCSharpMethodTranslateStateDefinition                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateParameterListEnd            , ';'                               , kCSharpMethodTranslateStateDeclaration                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateParameterListEnd            , kCEETokenID_LAMBDA                , kCSharpMethodTranslateStateDefinition                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraintClause            , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateConstraintsTypeParameter   );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraintsTypeParameter    , '<'                               , kCSharpMethodTranslateStateTypeParameters             );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraintsTypeParameter    , ':'                               , kCSharpMethodTranslateStateConstraints                );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraints                 , token_id_is_builtin_type          , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraints                 , token_id_is_builtin_reference     , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraints                 , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraints                 , kCEETokenID_STRUCT                , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraints                 , kCEETokenID_NEW                   , kCSharpMethodTranslateStateConstructor                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstructor                 , '('                               , kCSharpMethodTranslateStateConstructorList            );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstructorList             , ')'                               , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , token_id_is_builtin_type          , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_FUNCS_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , token_id_is_builtin_reference     , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , kCEETokenID_IDENTIFIER            , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , '.'                               , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , '<'                               , kCSharpMethodTranslateStateTypeParameters             );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , kCEETokenID_WHERE                 , kCSharpMethodTranslateStateConstraintClause           );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , kCEETokenID_STRUCT                , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , kCEETokenID_NEW                   , kCSharpMethodTranslateStateConstructor                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , ','                               , kCSharpMethodTranslateStateConstraint                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , '{'                               , kCSharpMethodTranslateStateDefinition                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , ';'                               , kCSharpMethodTranslateStateDeclaration                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstraint                  , kCEETokenID_LAMBDA                , kCSharpMethodTranslateStateDefinition                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstructorInitializer      , kCEETokenID_THIS                  , kCSharpMethodTranslateStateConstructorInitializerThis );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstructorInitializer      , kCEETokenID_BASE                  , kCSharpMethodTranslateStateConstructorInitializerBase );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstructorInitializerThis  , '('                               , kCSharpMethodTranslateStateInitializerList            );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInitializerList             , ')'                               , kCSharpMethodTranslateStateInitializerListEnd         );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateConstructorInitializerBase  , '('                               , kCSharpMethodTranslateStateInitializerList            );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInitializerList             , ')'                               , kCSharpMethodTranslateStateInitializerListEnd         );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInitializerListEnd          , '{'                               , kCSharpMethodTranslateStateDefinition                 );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInitializerListEnd          , ';'                               , kCSharpMethodTranslateStateDeclaration                );
    TRANSLATE_STATE_SET(csharp_method_translate_table,   kCSharpMethodTranslateStateInitializerListEnd          , kCEETokenID_LAMBDA                , kCSharpMethodTranslateStateDefinition                 );
}

static cee_boolean csharp_method_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    cee_boolean is_class_member = FALSE;
    CSharpMethodTranslateState current  = kCSharpMethodTranslateStateInit;
    CSharpMethodTranslateState prev = kCSharpMethodTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEESourceFragment* child = NULL;
    CEESourceFragment* grandfather = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEESourceSymbol* method = NULL;
    CEESourceSymbolType symbol_type = kCEESourceSymbolTypeUnknow;
    
    grandfather = cee_source_fragment_grandfather_get(fragment);
    if (grandfather) {
        if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
            cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
            cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
            is_class_member = TRUE;
    }
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_method_translate_table[current][token->identifier];
        
        if (current == kCSharpMethodTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpMethodTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (!parameter_list)
                    identifier = p;
            }
        }
        else if (current == kCSharpMethodTranslateStateParameterList) {
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kCSharpMethodTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
        else if (current == kCSharpMethodTranslateStateDeclaration) {
            if (!is_class_member) {
                current = kCSharpMethodTranslateStateError;
                ret = FALSE;
            }
            else {
                symbol_type = kCEESourceSymbolTypeFunctionDeclaration;
                ret = TRUE;
            }
            break;
        }
        else if (current == kCSharpMethodTranslateStateDefinition) {
            symbol_type = kCEESourceSymbolTypeFunctionDefinition;
            ret = TRUE;
            break;
        }
        else if (current == kCSharpMethodTranslateStateError) {
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if ((current != kCSharpMethodTranslateStateDeclaration &&
         current != kCSharpMethodTranslateStateDefinition) ||
        !identifier || !parameter_list)
        goto exit;
    
    /** destructor detected */
    p = cee_token_not_whitespace_newline_before(identifier);
    if (p && cee_token_is_identifier(p, '~'))
        q = p;
    else
        q = identifier;
    
    method = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                       fragment->subject_ref,
                                                       q,
                                                       identifier,
                                                       symbol_type,
                                                       "csharp",
                                                       kCEETokenStringOptionCompact);
    if (method) {
        cee_token_slice_state_mark(q, identifier, kCEETokenStateSymbolOccupied);
        
        if (symbol_type == kCEESourceSymbolTypeFunctionDefinition)
            cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeFunctionDefinition);
        else if (symbol_type == kCEESourceSymbolTypeFunctionDeclaration)
            cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeFunctionDeclaration);
        
        child = cee_source_fragment_child_index_by_leaf(fragment, parameter_list->data);
        if (child) {
            child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
            csharp_method_parameters_parse(child);
        }
        
        method->proto_descriptor = csharp_method_proto_descriptor_create(fragment,
                                                                         SOURCE_FREGMENT_TOKENS_FIRST(fragment),
                                                                         TOKEN_PREV(q),
                                                                         q,
                                                                         parameter_list,
                                                                         parameter_list_end);
        fragment->symbols = cee_list_prepend(fragment->symbols, method);
    }
        
#ifdef DEBUG_METHOD_DEFINITION
    if (method)
        cee_source_symbol_print(method);
#endif
    
exit:
    
    return ret;
}

static void csharp_method_parameters_declaration_translate_table_init(void)
{
    /**
     *                      [                   ]                   parameter_specifier     builtin_type    builtin_reference   params      identifier      ?                   <                   assign_operator     .                       ,
     *  Init                Init                Init                ParameterSpecifier      BuiltinType     BuiltinReference    Params      identifier      Error               Error               Error               Error                   Error
     *  ParameterSpecifier  ParameterSpecifier  ParameterSpecifier  ParameterSpecifier      BuiltinType     BuiltinReference    Params      identifier      Error               Error               Error               Error                   Error
     *  BuiltinType         BuiltinType         BuiltinType         Error                   BuiltinType     BuiltinReference    Params      identifier      BuiltinType         Error               Error               Error                   Error
     *  BuiltinReference    BuiltinReference    BuiltinReference    Error                   BuiltinType     BuiltinReference    Params      identifier      BuiltinReference    Error               Error               Error                   Error
     *  Params              Error               Error               Error                   BuiltinType     BuiltinReference    Error       identifier      Error               Error               Error               Error                   Error
     *  identifier          identifier          identifier          Error                   Error           Error               Error       identifier      identifier          *TypeParameters     Confirm             IdentifierSeparator     Confirm
     *  IdentifierSeparator Error               Error               Error                   Error           Error               Error       identifier      Error               Error               Error               Error                   Error
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     *  Confirm: set 'current state' to Init
     */
    TRANSLATE_STATE_INI(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateMax                 , kCSharpMethodParametersDeclarationTranslateStateError                                                     );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateInit                , '['                               , kCSharpMethodParametersDeclarationTranslateStateInit                  );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateInit                , ']'                               , kCSharpMethodParametersDeclarationTranslateStateInit                  );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateInit                , token_id_is_parameter_specifier   , kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier    );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateInit                , token_id_is_builtin_type          , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateInit                , token_id_is_builtin_reference     , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateInit                , kCEETokenID_PARAMS                , kCSharpMethodParametersDeclarationTranslateStateParams                );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateInit                , kCEETokenID_IDENTIFIER            , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier  , '['                               , kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier    );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier  , ']'                               , kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier    );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier  , token_id_is_parameter_specifier   , kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier    );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier  , token_id_is_builtin_type          , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier  , token_id_is_builtin_reference     , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier  , kCEETokenID_PARAMS                , kCSharpMethodParametersDeclarationTranslateStateParams                );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParameterSpecifier  , kCEETokenID_IDENTIFIER            , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinType         , '['                               , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinType         , ']'                               , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinType         , '?'                               , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinType         , token_id_is_builtin_type          , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinType         , token_id_is_builtin_reference     , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinType         , kCEETokenID_PARAMS                , kCSharpMethodParametersDeclarationTranslateStateParams                );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinType         , kCEETokenID_IDENTIFIER            , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinReference    , '['                               , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinReference    , ']'                               , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinReference    , '?'                               , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinReference    , token_id_is_builtin_type          , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinReference    , token_id_is_builtin_reference     , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinReference    , kCEETokenID_PARAMS                , kCSharpMethodParametersDeclarationTranslateStateParams                );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateBuiltinReference    , kCEETokenID_IDENTIFIER            , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParams              , token_id_is_builtin_type          , kCSharpMethodParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParams              , token_id_is_builtin_reference     , kCSharpMethodParametersDeclarationTranslateStateBuiltinReference      );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateParams              , kCEETokenID_IDENTIFIER            , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , '['                               , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , ']'                               , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , '?'                               , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , kCEETokenID_IDENTIFIER            , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , '<'                               , kCSharpMethodParametersDeclarationTranslateStateTypeParameters        );
    TRANSLATE_FUNCS_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , token_id_is_assignment            , kCSharpMethodParametersDeclarationTranslateStateConfirm               );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , '.'                               , kCSharpMethodParametersDeclarationTranslateStateIdentifierSeparator   );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifier          , ','                               , kCSharpMethodParametersDeclarationTranslateStateConfirm               );
    TRANSLATE_STATE_SET(csharp_method_parameters_declaration_translate_table,   kCSharpMethodParametersDeclarationTranslateStateIdentifierSeparator , kCEETokenID_IDENTIFIER            , kCSharpMethodParametersDeclarationTranslateStateIdentifier            );
}

static cee_boolean csharp_method_parameters_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = TRUE;
    CSharpMethodParametersDeclarationTranslateState current = kCSharpMethodParametersDeclarationTranslateStateInit;
    CSharpMethodParametersDeclarationTranslateState prev = kCSharpMethodParametersDeclarationTranslateStateInit;
    CEESourceSymbol* parameter = NULL;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* prefix = NULL;
    CEEList* identifier = NULL;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    prefix = p;
    while (p) {
        token = p->data;
        
        prev = current;
        current = csharp_method_parameters_declaration_translate_table[current][token->identifier];
        
        if (current == kCSharpMethodParametersDeclarationTranslateStateError) {
            ret = FALSE;
            break;
        }
        
        if (current == kCSharpMethodParametersDeclarationTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        
        if (current == kCSharpMethodParametersDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
                
        if (current == kCSharpMethodParametersDeclarationTranslateStateConfirm || !TOKEN_NEXT(p)) {
            if (identifier)
                parameter = csharp_method_parameter_create(fragment, prefix, identifier);
            
            if (parameter)
                fragment->symbols = cee_list_append(fragment->symbols, parameter);
            
            parameter = NULL;
            identifier = NULL;
            
            if (!cee_token_is_identifier(p, ',')) {
                p = skip_csharp_declaration_interval(p);
                if (!p)
                    break;
            }
            
            prefix = TOKEN_NEXT(p);
            current = kCSharpMethodParametersDeclarationTranslateStateInit;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (fragment->symbols)
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeDeclaration);
    
#ifdef DEBUG_METHOD_DEFINITION
    if (fragment->symbols)
        cee_source_symbols_print(fragment->symbols);
#endif
    
    return ret;
}

static CEESourceSymbol* csharp_method_parameter_create(CEESourceFragment* fragment,
                                                       CEEList* prefix,
                                                       CEEList* identifier)
{
    CEESourceSymbol* parameter = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                           fragment->subject_ref,
                                                                           identifier,
                                                                           identifier,
                                                                           kCEESourceSymbolTypeFunctionParameter,
                                                                           "csharp",
                                                                           kCEETokenStringOptionCompact);
    if (!parameter)
        return NULL;
    
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    if (prefix && TOKEN_PREV(identifier)) {
        parameter->proto_descriptor = csharp_method_parameter_proto_descriptor_create(fragment,
                                                                                      prefix,
                                                                                      TOKEN_PREV(identifier),
                                                                                      parameter);
    }
    return parameter;
}

static cee_char* csharp_method_parameter_proto_descriptor_create(CEESourceFragment* fragment,
                                                                 CEEList* prefix,
                                                                 CEEList* prefix_tail,
                                                                 CEESourceSymbol* parameter)
{
    if (!fragment || !parameter)
        return NULL;
    
    cee_char* descriptor = NULL;
    cee_char* proto_str = NULL;
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"declaration\"", NULL);
    
    if (parameter->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", parameter->name, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto_str = csharp_type_descriptor_from_token_slice(fragment, prefix, prefix_tail);
    if (proto_str) {
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto_str, "\"", NULL);
        cee_free(proto_str);
    }
        
    cee_strconcat0(&descriptor, "}", NULL);
        
    return descriptor;
}

static cee_char* csharp_type_descriptor_from_token_slice(CEESourceFragment* fragment,
                                                         CEEList* p,
                                                         CEEList* q)
{
    const cee_char* subject = fragment->subject_ref;
    cee_char* proto = NULL;
    CEEToken* token = NULL;
    cee_boolean found_type = FALSE;
    cee_char* str = NULL;
    
    while (p) {
        token = p->data;
        
        /** find type descriptor */
        if (token_id_is_builtin_type(token->identifier) ||
            token_id_is_builtin_reference(token->identifier) ||
            cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) ||
            cee_token_is_identifier(p, '.')) {
            
            str = cee_strndup((cee_char*)&subject[token->offset],
                              token->length);
            if (str) {
                cee_strconcat0(&proto, str, NULL);
                cee_free(str);
            }
            if (!found_type)
                found_type = TRUE;
            
        }   /** find array descriptor */
        else if ((cee_token_is_identifier(p, '[') ||
                  cee_token_is_identifier(p, ']')) &&
                 found_type) {
            
            str = cee_strndup((cee_char*)&subject[token->offset],
                              token->length);
            if (str) {
                cee_strconcat0(&proto, str, NULL);
                cee_free(str);
            }
        }
        else if (cee_token_is_identifier(p, '<')) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p || (q && p == q))
                break;
        }
        
        if (q && p == q)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    return proto;
}

static cee_char* csharp_method_proto_descriptor_create(CEESourceFragment* fragment,
                                                       CEEList* prefix,
                                                       CEEList* prefix_tail,
                                                       CEEList* identifier,
                                                       CEEList* parameter_list,
                                                       CEEList* parameter_list_end)
{
    if (!fragment ||
        !identifier ||
        !parameter_list ||
        !parameter_list_end)
        return NULL;
    
    const cee_char* subject = fragment->subject_ref;
    CEEList* tokens = cee_source_fragment_tokens_expand(fragment);
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEToken* token_prev = NULL;
    CEEList* header_tokens = NULL;
    CEEList* name_chain_tokens = NULL;
    CEEList* parameters_tokens = NULL;
    CEEList* trialing_tokens = NULL;
    cee_char* descriptor = NULL;
    cee_char* formater_str = NULL;
    cee_size l = 0;
    cee_boolean is_class_member = FALSE;
    CEEList* parent_symbols = NULL;
    CEESourceSymbol* parent_symbol = NULL;
    CEESourceFragment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* return_type_str = NULL;
    
    grandfather = cee_source_fragment_grandfather_get(fragment);
    if (grandfather) {
        if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition))
            parent_symbols = cee_source_fragment_symbols_search_by_type(grandfather,
                                                                        kCEESourceSymbolTypeClassDefinition);
        else if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition))
            parent_symbols = cee_source_fragment_symbols_search_by_type(grandfather,
                                                                        kCEESourceSymbolTypeStructDefinition);
        
        if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
            cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
            cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
            is_class_member = TRUE;
    }
    
    access_level = csharp_access_level_search(fragment, SOURCE_FREGMENT_TOKENS_FIRST(fragment), NULL);
    if (!access_level) {
        if (grandfather) {
            if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    if (prefix && prefix_tail)
        return_type_str = csharp_type_descriptor_from_token_slice(fragment, prefix, prefix_tail);
    /**
     * Constructor and Destructor doesn't has return type declaraton,
     * use its parent symbol type as return type
     */
    if (!return_type_str && is_class_member && parent_symbols) {
        parent_symbol = cee_list_nth_data(parent_symbols, 0);
        return_type_str = cee_strdup(parent_symbol->name);
    }
    
    p = TOKEN_FIRST(tokens);
    
    /** header */
    if (prefix_tail) {
        while (p) {
            TOKEN_APPEND(header_tokens, p->data);
            if (p->data == prefix_tail->data)
                break;
            p = TOKEN_NEXT(p);
        }
        p = TOKEN_NEXT(p);
    }
    
    /** name_chain */
    while (p) {
        if (p->data == identifier->data)
            break;
        p = TOKEN_NEXT(p);
    }
    while (p) {
        if (p->data == parameter_list->data)
            break;
        TOKEN_APPEND(name_chain_tokens, p->data);
        p = TOKEN_NEXT(p);
    }
    
    /** parameters */
    if (p)
        p = TOKEN_NEXT(p);
    while (p) {
        if (p->data == parameter_list_end->data)
            break;
        TOKEN_APPEND(parameters_tokens, p->data);
        p = TOKEN_NEXT(p);
    }
    
    /** trailing */
    if (p)
        p = TOKEN_NEXT(p);
    BRACKET_SIGN_DECLARATION();
    while (p) {
        token = p->data;
        SIGN_BRACKET(token->identifier);
        if (BRACKETS_IS_CLEAN()) {
            if (token->identifier == ',' ||
                token->identifier == ';' ||
                token->identifier == '{')
                break;
        }
        TOKEN_APPEND(trialing_tokens, p->data);
        p = TOKEN_NEXT(p);
    }
        
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"function\"", NULL);
        
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (return_type_str)
        cee_strconcat0(&descriptor, ", \"return_type\":", "\"", return_type_str, "\"", NULL);
    
    /** create header descriptor */
    token_prev = NULL;
    p = TOKEN_FIRST(header_tokens);
    while (p) {
        token = p->data;
        
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {

            if (should_proto_descriptor_append_whitespace(token, token_prev))
                cee_strconcat0(&formater_str, " ", NULL);
            cee_string_concat_with_token(&formater_str, subject, token);
            token_prev = token;
        }
        
        p = TOKEN_NEXT(p);
    }
    if (formater_str) {
        cee_strconcat0(&descriptor, ", \"header\":", "\"", formater_str, "\"", NULL);
        cee_free(formater_str);
        formater_str = NULL;
    }
    
    /** create identifier descriptor */
    token_prev = NULL;
    p = TOKEN_FIRST(name_chain_tokens);
    while (p) {
        token = p->data;
        
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            
            if (should_proto_descriptor_append_whitespace(token, token_prev))
                cee_strconcat0(&formater_str, " ", NULL);
            cee_string_concat_with_token(&formater_str, subject, token);
            token_prev = token;
        }
        
        p = TOKEN_NEXT(p);
    }
    if (formater_str) {
        cee_strconcat0(&descriptor, ", \"name\":", "\"", formater_str, "\"", NULL);
        cee_free(formater_str);
        formater_str = NULL;
    }
    
    /** create parameters descriptor */
    token_prev = NULL;
    cee_strconcat0(&descriptor, ", \"parameters\":[", NULL);
    p = TOKEN_FIRST(parameters_tokens);
    CLEAR_BRACKETS();
    while (p) {
        token = p->data;
        
        SIGN_BRACKET(token->identifier);
        
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            
            if (should_proto_descriptor_append_whitespace(token, token_prev))
                cee_strconcat0(&formater_str, " ", NULL);
            cee_string_concat_with_token(&formater_str, subject, token);
            
            token_prev = token;
        }
        
        if (BRACKETS_IS_CLEAN()) {
            if (token->identifier == ',' || !TOKEN_NEXT(p)) {
                if (formater_str) {
                    l = strlen(formater_str);
                    if (formater_str[l-1]== ',')
                        formater_str[l-1] = '\0';
                    
                    cee_strconcat0(&descriptor, "\"", formater_str, "\"", ",", NULL);
                    cee_free(formater_str);
                    formater_str = NULL;
                    token_prev = NULL;
                }
            }
        }
        
        p = TOKEN_NEXT(p);
    }
    /** remove the last comma in jason array */
    l = strlen(descriptor);
    if (descriptor[l-1]== ',')
        descriptor[l-1] = '\0';
    cee_strconcat0(&descriptor, "]", NULL);
    
    /** create trialing descriptor */
    token_prev = NULL;
    p = TOKEN_FIRST(trialing_tokens);
    while (p) {
        token = p->data;
        
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {
            
            if (should_proto_descriptor_append_whitespace(token, token_prev))
                cee_strconcat0(&formater_str, " ", NULL);
            cee_string_concat_with_token(&formater_str, subject, token);
            
            token_prev = token;
        }
        
        p = TOKEN_NEXT(p);
    }
    if (formater_str) {
        cee_strconcat0(&descriptor, ", \"trialing\":", "\"", formater_str, "\"", NULL);
        cee_free(formater_str);
        formater_str = NULL;
    }
    cee_strconcat0(&descriptor, "}", NULL);
    
    
exit:
    if (return_type_str)
        cee_free(return_type_str);
    
    if (parent_symbols)
        cee_list_free(parent_symbols);
    
    if (trialing_tokens)
        cee_list_free(trialing_tokens);
    
    if (parameters_tokens)
        cee_list_free(parameters_tokens);
    
    if (name_chain_tokens)
        cee_list_free(name_chain_tokens);
    
    if (header_tokens)
        cee_list_free(header_tokens);
    
    if (tokens)
        cee_list_free(tokens);
        
    return descriptor;
}

static cee_boolean should_proto_descriptor_append_whitespace(CEEToken* token,
                                                             CEEToken* token_prev)
{
    return (token_prev &&
            (!token_id_is_punctuation(token_prev->identifier) &&
             !token_id_is_punctuation(token->identifier)));
}

static const cee_char* csharp_access_level_search(CEESourceFragment* fragment,
                                                  CEEList* begin,
                                                  CEEList* end)
{
    
    if (!begin)
        return NULL;
    
    CEEList* p = begin;
    CEEList* q = NULL;
    
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_PUBLIC)) {
            return "public";
        }
        else if (cee_token_is_identifier(p, kCEETokenID_INTERNAL)) {
            return "internal";
        }
        else if (cee_token_is_identifier(p, kCEETokenID_PROTECTED)) {
            q = cee_token_not_whitespace_newline_after(p);
            if (q && cee_token_is_identifier(q, kCEETokenID_INTERNAL))
                return "protected internal";
            else
                return "protected";
        }
        else if (cee_token_is_identifier(p, kCEETokenID_PRIVATE)) {
            q = cee_token_not_whitespace_newline_after(p);
            if (q && cee_token_is_identifier(q, kCEETokenID_PROTECTED))
                return "private protected";
            else
                return "private";
        }
        
        if (end && p == end)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    return NULL;
}

static CEEList* skip_csharp_declaration_interval(CEEList* p)
{
    BRACKET_SIGN_DECLARATION();
    CEEToken* token = NULL;
    while (p) {
        token = p->data;
        
        SIGN_BRACKET(token->identifier);
        if (BRACKETS_IS_CLEAN()) {
            if (token->identifier == ',' ||
                token->identifier == ';')
                return p;
        }
        
        p = TOKEN_NEXT(p);
    }
    return NULL;
}

static void csharp_operator_translate_table_init(void)
{
    /**
     *                          [                       ]                       declaration_specifier   builtin_type    builtin_reference   identifier      operator    overlaodable    .                       ?                   <                   (               )                   {               ;               =>
     *  Init                    Init                    Init                    DeclarationSpecifier    BuiltinType     BuiltinReference    Identifier      Operator    Error           Error                   Error               Error               Error           Error               Error           Error           Error
     *  DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    BuiltinType     BuiltinReference    Identifier      Operator    Error           Error                   Error               Error               Error           Error               Error           Error           Error
     *  BuiltinType             BuiltinType             BuiltinType             Error                   BuiltinType     BuiltinReference    Identifier      Operator    Error           Error                   BuiltinType         Error               Error           Error               Error           Error           Error
     *  BuiltinReference        BuiltinReference        BuiltinReference        Error                   BuiltinType     BuiltinReference    Identifier      Operator    Error           Error                   BuiltinReference    Error               Error           Error               Error           Error           Error
     *  Identifier              Identifier              Identifier              Error                   BuiltinType     BuiltinReference    Identifier      Operator    Error           IdentifierSeparator     Identifier          *TypeParameters     ParameterList   Error               Error           Error           Error
     *  IdentifierSeparator     Error                   Error                   Error                   Error           Error               Identifier      Error       Error           Error                   Error               Error               Error           Error               Error           Error           Error
     *  Operator                Error                   Error                   Error                   Error           Error               Identifier      Error       Overlaodable    Error                   Error               Error               Error           Error               Error           Error           Error
     *  Overlaodable            Error                   Error                   Error                   Error           Error               Error           Error       Overlaodable    Error                   Error               Error               ParameterList   Error               Error           Error           Error
     *  ParameterList           Error                   Error                   Error                   Error           Error               Error           Error       Error           Error                   Error               Error               Error           ParameterListEnd    Error           Error           Error
     *  ParameterListEnd        Error                   Error                   Error                   Error           Error               Error           Error       Error           Error                   Error               Error               Error           Error               Definition      Declaration     Definition
     */
    TRANSLATE_STATE_INI(csharp_operator_translate_table, kCSharpOperatorTranslateStateMax                  , kCSharpOperatorTranslateStateError                                                     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateInit                 , '['                                , kCSharpOperatorTranslateStateInit                 );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateInit                 , ']'                                , kCSharpOperatorTranslateStateInit                 );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateInit                 , token_id_is_declaration_specifier  , kCSharpOperatorTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateInit                 , token_id_is_builtin_type           , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateInit                 , token_id_is_builtin_reference      , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateInit                 , kCEETokenID_IDENTIFIER             , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateInit                 , kCEETokenID_OPERATOR               , kCSharpOperatorTranslateStateOperator             );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateDeclarationSpecifier , token_id_is_declaration_specifier  , kCSharpOperatorTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateDeclarationSpecifier , token_id_is_builtin_type           , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateDeclarationSpecifier , token_id_is_builtin_reference      , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateDeclarationSpecifier , kCEETokenID_IDENTIFIER             , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateDeclarationSpecifier , kCEETokenID_OPERATOR               , kCSharpOperatorTranslateStateOperator             );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateDeclarationSpecifier , '['                                , kCSharpOperatorTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateDeclarationSpecifier , ']'                                , kCSharpOperatorTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinType          , token_id_is_builtin_type           , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinType          , token_id_is_builtin_reference      , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinType          , kCEETokenID_IDENTIFIER             , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinType          , kCEETokenID_OPERATOR               , kCSharpOperatorTranslateStateOperator             );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinType          , '['                                , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinType          , ']'                                , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinType          , '?'                                , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinReference     , token_id_is_builtin_type           , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinReference     , token_id_is_builtin_reference      , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinReference     , kCEETokenID_IDENTIFIER             , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinReference     , kCEETokenID_OPERATOR               , kCSharpOperatorTranslateStateOperator             );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinReference     , '['                                , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinReference     , ']'                                , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateBuiltinReference     , '?'                                , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , token_id_is_builtin_type           , kCSharpOperatorTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , token_id_is_builtin_reference      , kCSharpOperatorTranslateStateBuiltinReference     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , kCEETokenID_IDENTIFIER             , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , kCEETokenID_OPERATOR               , kCSharpOperatorTranslateStateOperator             );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , '.'                                , kCSharpOperatorTranslateStateIdentifierSeparator  );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , '<'                                , kCSharpOperatorTranslateStateTypeParameters       );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , '['                                , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , ']'                                , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , '?'                                , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifier           , '('                                , kCSharpOperatorTranslateStateParameterList        );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateIdentifierSeparator  , kCEETokenID_IDENTIFIER             , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateOperator             , kCEETokenID_IDENTIFIER             , kCSharpOperatorTranslateStateIdentifier           );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateOperator             , token_id_is_overloadable           , kCSharpOperatorTranslateStateOverlaodable         );
    TRANSLATE_FUNCS_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateOverlaodable         , token_id_is_overloadable           , kCSharpOperatorTranslateStateOverlaodable         );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateOverlaodable         , '('                                , kCSharpOperatorTranslateStateParameterList        );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateParameterList        , ')'                                , kCSharpOperatorTranslateStateParameterListEnd     );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateParameterListEnd     , '{'                                , kCSharpOperatorTranslateStateDefinition           );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateParameterListEnd     , ';'                                , kCSharpOperatorTranslateStateDeclaration          );
    TRANSLATE_STATE_SET(csharp_operator_translate_table, kCSharpOperatorTranslateStateParameterListEnd     , kCEETokenID_LAMBDA                 , kCSharpOperatorTranslateStateDefinition           );
}

static cee_boolean csharp_operator_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    cee_boolean is_class_member = FALSE;
    CSharpOperatorTranslateState current = kCSharpOperatorTranslateStateInit;
    CSharpOperatorTranslateState prev = kCSharpOperatorTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEESourceFragment* child = NULL;
    CEESourceFragment* grandfather = NULL;
    CEEList* operator = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEESourceSymbol* operator_overload = NULL;
    CEESourceSymbolType symbol_type = kCEESourceSymbolTypeUnknow;
    cee_char* type = "function_definition";
        
    grandfather = cee_source_fragment_grandfather_get(fragment);
    if (grandfather) {
        if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
            cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
            cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
            is_class_member = TRUE;
    }
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_operator_translate_table[current][token->identifier];
        
        if (current == kCSharpOperatorTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpOperatorTranslateStateOperator) {
            if (cee_token_is_identifier(p, kCEETokenID_OPERATOR)) {
                if (!parameter_list)
                    operator = p;
            }
        }
        else if (current == kCSharpOperatorTranslateStateParameterList) {
            if (!parameter_list) {
                parameter_list = p;
                q = cee_token_not_whitespace_newline_before(parameter_list);
            }
        }
        else if (current == kCSharpOperatorTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
        else if (current == kCSharpOperatorTranslateStateDeclaration) {
            if (!is_class_member) {
                current = kCSharpOperatorTranslateStateError;
                ret = FALSE;
            }
            else {
                symbol_type = kCEESourceSymbolTypeOperatorOverloadDeclaration;
                ret = TRUE;
            }
            break;
        }
        else if (current == kCSharpOperatorTranslateStateDefinition) {
            symbol_type = kCEESourceSymbolTypeOperatorOverloadDefinition;
            ret = TRUE;
            break;
        }
        else if (current == kCSharpOperatorTranslateStateError) {
            break;
        }
    
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if ((current != kCSharpOperatorTranslateStateDeclaration &&
         current != kCSharpOperatorTranslateStateDefinition) ||
        !operator || !parameter_list)
        goto exit;
    
    operator_overload = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                  fragment->subject_ref,
                                                                  operator,
                                                                  q,
                                                                  symbol_type,
                                                                  "csharp",
                                                                  kCEETokenStringOptionCompact);
    if (operator_overload) {
        if (symbol_type == kCEESourceSymbolTypeOperatorOverloadDefinition) {
            cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeFunctionDefinition);
            type = "function_definition";
        }
        else if (symbol_type == kCEESourceSymbolTypeOperatorOverloadDeclaration) {
            cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeFunctionDeclaration);
            type = "function_declaration";
        }
            
        if (operator_overload->name) {
            cee_free(operator_overload->name);
            operator_overload->name = csharp_operator_name_create(fragment);
        }
        
        child = cee_source_fragment_child_index_by_leaf(fragment, parameter_list->data);
        if (child) {
            child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
            csharp_method_parameters_parse(child);
        }
        
        operator_overload->proto_descriptor = csharp_method_proto_descriptor_create(fragment,
                                                                                    SOURCE_FREGMENT_TOKENS_FIRST(fragment),
                                                                                    TOKEN_PREV(operator),
                                                                                    operator,
                                                                                    parameter_list,
                                                                                    parameter_list_end);
        fragment->symbols = cee_list_prepend(fragment->symbols, operator_overload);
    }
    
#ifdef DEBUG_METHOD_DEFINITION
    if (operator_overload)
        cee_source_symbol_print(operator_overload);
#endif
    
exit:
    
    return ret;
}

static cee_char* csharp_operator_name_create(CEESourceFragment* fragment)
{
    CSharpOperatorTranslateState current = kCSharpOperatorTranslateStateInit;
    CSharpOperatorTranslateState prev = kCSharpOperatorTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    cee_char* name = NULL;
    cee_boolean operator_catch = FALSE;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_operator_translate_table[current][token->identifier];
        
        if (current == kCSharpOperatorTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpOperatorTranslateStateOperator) {
            if (cee_token_is_identifier(p, kCEETokenID_OPERATOR)) {
                cee_strconcat0(&name, "operator ", NULL);
                operator_catch = TRUE;
            }
        }
        else if (current == kCSharpOperatorTranslateStateIdentifier ||
                 current == kCSharpOperatorTranslateStateIdentifierSeparator ||
                 current == kCSharpOperatorTranslateStateOverlaodable) {
            if (operator_catch) {
                if (!cee_token_id_is_whitespace(token->identifier) &&
                    !cee_token_id_is_newline(token->identifier)) {
                    cee_char* str = cee_string_from_token(fragment->subject_ref, token);
                    if (str) {
                        cee_strconcat0(&name, str, NULL);
                        cee_free(str);
                    }
                }
            }
                
        }
        else if (current == kCSharpOperatorTranslateStateParameterList) {
            break;
        }
    
next_token:
        p = TOKEN_NEXT(p);
    }
    
    return name;
}

static void csharp_property_declaration_translate_table_init(void)
{
    /**
     *                              [                       ]                       declaration_specifier   builtin_type    builtin_reference   identifier      ?                   .                   <               {       =>
     *  Init                        Init                    Init                    DeclarationSpecifier    BuiltinType     BuiltinReference    Identifier      Error               Error               Error           Error   Error
     *  DeclarationSpecifier        DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    BuiltinType     BuiltinReference    Identifier      Error               Error               Error           Error   Error
     *  BuiltinType                 BuiltinType             BuiltinType             Error                   BuiltinType     BuiltinReference    Identifier      BuiltinType         Error               Error           Error   Error
     *  BuiltinReference            BuiltinReference        BuiltinReference        Error                   BuiltinType     BuiltinReference    Identifier      BuiltinReference    Error               Error           Error   Error
     *  Identifier                  Identifier              Identifier              Error                   Error           Error               Identifier      Identifier          IdentifierSeparator *TypeParameters Confirm Confirm
     *
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateMax                  , kCSharpPropertyDeclarationTranslateStateError                                                           );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateInit                 , '['                                   , kCSharpPropertyDeclarationTranslateStateInit                    );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateInit                 , ']'                                   , kCSharpPropertyDeclarationTranslateStateInit                    );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateInit                 , token_id_is_declaration_specifier     , kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateInit                 , token_id_is_builtin_type              , kCSharpPropertyDeclarationTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateInit                 , token_id_is_builtin_reference         , kCSharpPropertyDeclarationTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateInit                 , kCEETokenID_IDENTIFIER                , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier , '['                                   , kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier , ']'                                   , kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier , token_id_is_declaration_specifier     , kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier , token_id_is_builtin_type              , kCSharpPropertyDeclarationTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier , token_id_is_builtin_reference         , kCSharpPropertyDeclarationTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateDeclarationSpecifier , kCEETokenID_IDENTIFIER                , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinType          , '['                                   , kCSharpPropertyDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinType          , ']'                                   , kCSharpPropertyDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinType          , '?'                                   , kCSharpPropertyDeclarationTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinType          , token_id_is_builtin_type              , kCSharpPropertyDeclarationTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinType          , token_id_is_builtin_reference         , kCSharpPropertyDeclarationTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinType          , kCEETokenID_IDENTIFIER                , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinReference     , '['                                   , kCSharpPropertyDeclarationTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinReference     , ']'                                   , kCSharpPropertyDeclarationTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinReference     , '?'                                   , kCSharpPropertyDeclarationTranslateStateBuiltinReference        );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinReference     , token_id_is_builtin_type              , kCSharpPropertyDeclarationTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinReference     , token_id_is_builtin_reference         , kCSharpPropertyDeclarationTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateBuiltinReference     , kCEETokenID_IDENTIFIER                , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , '['                                   , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , ']'                                   , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , '?'                                   , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , kCEETokenID_IDENTIFIER                , kCSharpPropertyDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , '.'                                   , kCSharpPropertyDeclarationTranslateStateIdentifierSeparator     );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , '<'                                   , kCSharpPropertyDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , '{'                                   , kCSharpPropertyDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifier           , kCEETokenID_LAMBDA                    , kCSharpPropertyDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_property_declaration_translate_table, kCSharpPropertyDeclarationTranslateStateIdentifierSeparator  , kCEETokenID_IDENTIFIER                , kCSharpPropertyDeclarationTranslateStateIdentifier              );
}

static cee_boolean csharp_property_declaration_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    CSharpPropertyDeclarationTranslateState current = kCSharpPropertyDeclarationTranslateStateInit;
    CSharpPropertyDeclarationTranslateState prev = kCSharpPropertyDeclarationTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* prefix = NULL;
    CEEList* prefix_tail = NULL;
    CEEList* identifier = NULL;
    CEESourceSymbol* declaration = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_property_declaration_translate_table[current][token->identifier];
        
        if (current == kCSharpPropertyDeclarationTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpPropertyDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kCSharpPropertyDeclarationTranslateStateConfirm) {
            prefix = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
            if (identifier)
                prefix_tail = TOKEN_PREV(identifier);
            ret = TRUE;
            break;
        }
        else if (current == kCSharpPropertyDeclarationTranslateStateError) {
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCSharpPropertyDeclarationTranslateStateConfirm || !identifier)
        goto exit;
    
    declaration = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                            fragment->subject_ref,
                                                            identifier,
                                                            identifier,
                                                            kCEESourceSymbolTypePropertyDeclaration,
                                                            "csharp",
                                                            kCEETokenStringOptionCompact);
    if (declaration) {
        cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeDeclaration);
        declaration->proto_descriptor = csharp_property_proto_descriptor_create(fragment,
                                                                                prefix,
                                                                                prefix_tail,
                                                                                identifier);
        fragment->symbols = cee_list_prepend(fragment->symbols, declaration);
    }
    
#ifdef DEBUG_PROPERTY_DECLARATION
    if (declaration)
        cee_source_symbol_print(declaration);
#endif
    
exit:
    
    return ret;
}

static cee_char* csharp_property_proto_descriptor_create(CEESourceFragment* fragment,
                                                         CEEList* prefix,
                                                         CEEList* prefix_tail,
                                                         CEEList* identifier)
{
    if (!fragment || !identifier)
        return NULL;
    
    CEESourceFragment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* proto = NULL;
    cee_char* descriptor = NULL;
    const cee_char* subject = fragment->subject_ref;
    cee_char* identifier_str = NULL;
    
    access_level = csharp_access_level_search(fragment, SOURCE_FREGMENT_TOKENS_FIRST(fragment), NULL);
    if (!access_level) {
        grandfather = cee_source_fragment_grandfather_get(fragment);
        if (grandfather) {
            if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"property\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto = csharp_type_descriptor_from_token_slice(fragment, prefix, prefix_tail);
    if (proto) {
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto, "\"", NULL);
        cee_free(proto);
    }
    
    if (identifier)
        identifier_str = cee_string_from_token(subject, identifier->data);
    if (identifier_str) {
        cee_strconcat0(&descriptor, ", \"name\":", "\"", identifier_str, "\"", NULL);
        cee_free(identifier_str);
    }
    
    cee_strconcat0(&descriptor, "}", NULL);
        
    return descriptor;
}

static void csharp_event_declaration_table_init(void)
{
    /**
     *                          [                       ]                       declaration_specifier   event   builtin_type    builtin_reference   identifier      assign_operator     ?                   .                       <               ,       {       ;
     *  Init                    Init                    Init                    DeclarationSpecifier    Event   Error           Error               Error           Error               Error               Error                   Error           Error   Error   Error
     *  DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    Event   Error           Error               Error           Error               Error               Error                   Error           Error   Error   Error
     *  Event                   Event                   Event                   Error                   Error   BuiltinType     BuiltinReference    CustomType      Error               Error               Error                   Error           Error   Error   Error
     *  BuiltinType             BuiltinType             BuiltinType             Error                   Error   BuiltinType     BuiltinReference    Identifier      Error               BuiltinType         Error                   Error           Error   Error   Error
     *  BuiltinReference        BuiltinReference        BuiltinReference        Error                   Error   BuiltinType     BuiltinReference    Identifier      Error               BuiltinReference    Error                   Error           Error   Error   Error
     *  CustomType              CustomType              CustomType              Error                   Error   Error           Error               Identifier      Error               CustomType          CustomTypeSeparator     *TypeParameters Error   Error   Error
     *  Identifier              Identifier              Identifier              Error                   Error   Error           Error               Identifier      Confirm             Error               Error                   Error           Confirm Confirm Confirm
     *  CustomTypeSeparator     Error                   Error                   Error                   Error   Error           Error               CustomType      Error               Error               Error                   Error           Error   Error   Error
     *  Confirm                 Error                   Error                   Error                   Error   Error           Error               Identifier      Error               Error               Error                   Error           Confirm Error   Confirm
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_event_declaration_table, kCSharpEventTranslateStateMax                     , kCSharpEventTranslateStateError                                                       );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateInit                    , '['                               , kCSharpEventTranslateStateInit                    );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateInit                    , ']'                               , kCSharpEventTranslateStateInit                    );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateInit                    , token_id_is_declaration_specifier , kCSharpEventTranslateStateDeclarationSpecifier    );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateInit                    , kCEETokenID_EVENT                 , kCSharpEventTranslateStateEvent                   );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateDeclarationSpecifier    , '['                               , kCSharpEventTranslateStateDeclarationSpecifier    );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateDeclarationSpecifier    , ']'                               , kCSharpEventTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateDeclarationSpecifier    , token_id_is_declaration_specifier , kCSharpEventTranslateStateDeclarationSpecifier    );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateDeclarationSpecifier    , kCEETokenID_EVENT                 , kCSharpEventTranslateStateEvent                   );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateEvent                   , '['                               , kCSharpEventTranslateStateEvent                   );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateEvent                   , ']'                               , kCSharpEventTranslateStateEvent                   );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateEvent                   , token_id_is_builtin_type          , kCSharpEventTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateEvent                   , token_id_is_builtin_reference     , kCSharpEventTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateEvent                   , kCEETokenID_IDENTIFIER            , kCSharpEventTranslateStateCustomType              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinType             , '['                               , kCSharpEventTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinType             , ']'                               , kCSharpEventTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinType             , '?'                               , kCSharpEventTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinType             , token_id_is_builtin_type          , kCSharpEventTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinType             , token_id_is_builtin_reference     , kCSharpEventTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinType             , kCEETokenID_IDENTIFIER            , kCSharpEventTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinReference        , '['                               , kCSharpEventTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinReference        , ']'                               , kCSharpEventTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinReference        , '?'                               , kCSharpEventTranslateStateBuiltinReference        );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinReference        , token_id_is_builtin_type          , kCSharpEventTranslateStateBuiltinType             );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinReference        , token_id_is_builtin_reference     , kCSharpEventTranslateStateBuiltinReference        );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateBuiltinReference        , kCEETokenID_IDENTIFIER            , kCSharpEventTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateCustomType              , '['                               , kCSharpEventTranslateStateCustomType              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateCustomType              , ']'                               , kCSharpEventTranslateStateCustomType              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateCustomType              , '?'                               , kCSharpEventTranslateStateCustomType              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateCustomType              , kCEETokenID_IDENTIFIER            , kCSharpEventTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateCustomType              , '.'                               , kCSharpEventTranslateStateCustomTypeSeparator     );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateCustomType              , '<'                               , kCSharpEventTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateCustomTypeSeparator     , kCEETokenID_IDENTIFIER            , kCSharpEventTranslateStateCustomType              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateIdentifier              , '['                               , kCSharpEventTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateIdentifier              , ']'                               , kCSharpEventTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateIdentifier              , kCEETokenID_IDENTIFIER            , kCSharpEventTranslateStateIdentifier              );
    TRANSLATE_FUNCS_SET(csharp_event_declaration_table, kCSharpEventTranslateStateIdentifier              , token_id_is_assignment            , kCSharpEventTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateIdentifier              , ','                               , kCSharpEventTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateIdentifier              , ';'                               , kCSharpEventTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateIdentifier              , '{'                               , kCSharpEventTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateConfirm                 , kCEETokenID_IDENTIFIER            , kCSharpEventTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateConfirm                 , ','                               , kCSharpEventTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateConfirm                 , ';'                               , kCSharpEventTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(csharp_event_declaration_table, kCSharpEventTranslateStateConfirm                 , '{'                               , kCSharpEventTranslateStateConfirm                 );
}

static cee_boolean csharp_event_declaration_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    CSharpEventTranslateState current = kCSharpEventTranslateStateInit;
    CSharpEventTranslateState prev = kCSharpEventTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* identifier = NULL;
    CEEList* prefix = NULL;
    CEEList* prefix_tail = NULL;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    cee_boolean prefix_parse = FALSE;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_event_declaration_table[current][token->identifier];
        
        if (current == kCSharpEventTranslateStateError) {
            break;
        }
        else if (current == kCSharpEventTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpEventTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kCSharpEventTranslateStateConfirm) {
            if (!prefix_parse) {
                prefix = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
                if (identifier)
                    prefix_tail = TOKEN_PREV(identifier);
                prefix_parse = TRUE;
            }
            
            if (identifier) {
                declaration = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                        fragment->subject_ref,
                                                                        identifier,
                                                                        identifier,
                                                                        kCEESourceSymbolTypeVariableDeclaration,
                                                                        "csharp",
                                                                        kCEETokenStringOptionCompact);
            }
            
            if (declaration) {
                cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
                declaration->proto_descriptor = csharp_event_proto_descriptor_create(fragment,
                                                                                     prefix,
                                                                                     prefix_tail,
                                                                                     identifier);
                declarations = cee_list_prepend(declarations, declaration);
            }
            
            declaration = NULL;
            identifier = NULL;
            
            if (!cee_token_is_identifier(p, ',') &&
                !cee_token_is_identifier(p, ';')) {
                p = skip_csharp_declaration_interval(p);
                if (!p)
                    break;
            }
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (declarations) {
        fragment->symbols = cee_list_concat(fragment->symbols, declarations);
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_EVENTS
    if (declarations)
        cee_source_symbols_print(declarations);
#endif
    
exit:
    
    return ret;
}

static cee_char* csharp_event_proto_descriptor_create(CEESourceFragment* fragment,
                                                      CEEList* prefix,
                                                      CEEList* prefix_tail,
                                                      CEEList* identifier)
{
    if (!fragment || !identifier)
        return NULL;
    
    CEESourceFragment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* proto = NULL;
    cee_char* descriptor = NULL;
    const cee_char* subject = fragment->subject_ref;
    cee_char* identifier_str = NULL;
    
    access_level = csharp_access_level_search(fragment, SOURCE_FREGMENT_TOKENS_FIRST(fragment), NULL);
    if (!access_level) {
        grandfather = cee_source_fragment_grandfather_get(fragment);
        if (grandfather) {
            if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"event\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto = csharp_type_descriptor_from_token_slice(fragment, prefix, prefix_tail);
    if (proto) {
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto, "\"", NULL);
        cee_free(proto);
    }
    
    if (identifier)
        identifier_str = cee_string_from_token(subject, identifier->data);
    if (identifier_str) {
        cee_strconcat0(&descriptor, ", \"name\":", "\"", identifier_str, "\"", NULL);
        cee_free(identifier_str);
    }
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static void csharp_indexer_declaration_translate_table_init(void)
{
    /**
     *                          [                       ]                       declaration_specifier   builtin_type    builtin_reference   identifier      ?                   <               .                       this    {       =>
     *  Init                    Init                    Init                    DeclarationSpecifier    BuiltinType     BuiltinReference    Type            Error               Error           Error                   Error   Error   Error
     *  DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    BuiltinType     BuiltinReference    Type            Error               Error           Error                   Error   Error   Error
     *  BuiltinType             BuiltinType             BuiltinType             Error                   BuiltinType     BuiltinReference    InterfaceType   BuiltinType         Error           Error                   This    Error   Error
     *  BuiltinReference        BuiltinReference        BuiltinReference        Error                   BuiltinType     BuiltinReference    InterfaceType   BuiltinReference    Error           Error                   This    Error   Error
     *  Type                    Type                    Type                    Error                   Error           Error               InterfaceType   Type                *TypeParameters TypeSeparator           This    Error   Error
     *  TypeSeparator           Error                   Error                   Error                   Error           Error               Type            Error               Error           Error                   Error   Error   Error
     *  InterfaceType           Error                   Error                   Error                   Error           Error               Error           Error               *TypeParameters InterfaceTypeSeparator  Error   Error   Error
     *  InterfaceTypeSeparator  Error                   Error                   Error                   Error           Error               InterfaceType   Error               Error           Error                   This    Error   Error
     *  This                    ParameterList           Error                   Error                   Error           Error               Error           Error               Error           Error                   Error   Error   Error
     *  ParameterList           Error                   ParameterListEnd        Error                   Error           Error               Error           Error               Error           Error                   Error   Error   Error
     *  ParameterListEnd        Error                   Error                   Error                   Error           Error               Error           Error               Error           Error                   Error   Confirm Confirm
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     *
     **/
    TRANSLATE_STATE_INI(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateMax                      , kCSharpIndexerDeclarationTranslateStateError                                                      );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInit                     , '['                               , kCSharpIndexerDeclarationTranslateStateInit                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInit                     , ']'                               , kCSharpIndexerDeclarationTranslateStateInit                   );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInit                     , token_id_is_declaration_specifier , kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInit                     , token_id_is_builtin_type          , kCSharpIndexerDeclarationTranslateStateBuiltinType            );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInit                     , token_id_is_builtin_reference     , kCSharpIndexerDeclarationTranslateStateBuiltinReference       );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInit                     , kCEETokenID_IDENTIFIER            , kCSharpIndexerDeclarationTranslateStateType                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier     , '['                               , kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier     , ']'                               , kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier     , token_id_is_declaration_specifier , kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier     , token_id_is_builtin_type          , kCSharpIndexerDeclarationTranslateStateBuiltinType            );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier     , token_id_is_builtin_reference     , kCSharpIndexerDeclarationTranslateStateBuiltinReference       );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateDeclarationSpecifier     , kCEETokenID_IDENTIFIER            , kCSharpIndexerDeclarationTranslateStateType                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinType              , '['                               , kCSharpIndexerDeclarationTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinType              , ']'                               , kCSharpIndexerDeclarationTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinType              , '?'                               , kCSharpIndexerDeclarationTranslateStateBuiltinType            );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinType              , token_id_is_builtin_type          , kCSharpIndexerDeclarationTranslateStateBuiltinType            );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinType              , token_id_is_builtin_reference     , kCSharpIndexerDeclarationTranslateStateBuiltinReference       );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinType              , kCEETokenID_IDENTIFIER            , kCSharpIndexerDeclarationTranslateStateInterfaceType          );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinType              , kCEETokenID_THIS                  , kCSharpIndexerDeclarationTranslateStateThis                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinReference         , '['                               , kCSharpIndexerDeclarationTranslateStateBuiltinReference       );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinReference         , ']'                               , kCSharpIndexerDeclarationTranslateStateBuiltinReference       );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinReference         , '?'                               , kCSharpIndexerDeclarationTranslateStateBuiltinReference       );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinReference         , token_id_is_builtin_type          , kCSharpIndexerDeclarationTranslateStateBuiltinType            );
    TRANSLATE_FUNCS_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinReference         , token_id_is_builtin_reference     , kCSharpIndexerDeclarationTranslateStateBuiltinReference       );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinReference         , kCEETokenID_IDENTIFIER            , kCSharpIndexerDeclarationTranslateStateInterfaceType          );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateBuiltinReference         , kCEETokenID_THIS                  , kCSharpIndexerDeclarationTranslateStateThis                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateType                     , '['                               , kCSharpIndexerDeclarationTranslateStateType                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateType                     , ']'                               , kCSharpIndexerDeclarationTranslateStateType                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateType                     , '?'                               , kCSharpIndexerDeclarationTranslateStateType                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateType                     , kCEETokenID_IDENTIFIER            , kCSharpIndexerDeclarationTranslateStateInterfaceType          );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateType                     , '<'                               , kCSharpIndexerDeclarationTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateType                     , '.'                               , kCSharpIndexerDeclarationTranslateStateTypeSeparator          );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateType                     , kCEETokenID_THIS                  , kCSharpIndexerDeclarationTranslateStateThis                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateTypeSeparator            , kCEETokenID_IDENTIFIER            , kCSharpIndexerDeclarationTranslateStateType                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInterfaceType            , '<'                               , kCSharpIndexerDeclarationTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInterfaceType            , '.'                               , kCSharpIndexerDeclarationTranslateStateInterfaceTypeSeparator );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInterfaceTypeSeparator   , kCEETokenID_IDENTIFIER            , kCSharpIndexerDeclarationTranslateStateInterfaceType          );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateInterfaceTypeSeparator   , kCEETokenID_THIS                  , kCSharpIndexerDeclarationTranslateStateThis                   );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateThis                     , '['                               , kCSharpIndexerDeclarationTranslateStateParameterList          );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateParameterList            , ']'                               , kCSharpIndexerDeclarationTranslateStateParameterListEnd       );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateParameterListEnd         , '{'                               , kCSharpIndexerDeclarationTranslateStateConfirm                );
    TRANSLATE_STATE_SET(csharp_indexer_declaration_translate_table, kCSharpIndexerDeclarationTranslateStateParameterListEnd         , kCEETokenID_LAMBDA                , kCSharpIndexerDeclarationTranslateStateConfirm                );
}

static cee_boolean csharp_indexer_declaration_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    CSharpIndexerDeclarationTranslateState current = kCSharpIndexerDeclarationTranslateStateInit;
    CSharpIndexerDeclarationTranslateState prev = kCSharpIndexerDeclarationTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEEList* this = NULL;
    CEEList* interface_type = NULL;
    CEEList* prefix = NULL;
    CEEList* prefix_tail = NULL;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    cee_boolean prefix_parse = FALSE;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
                
        prev = current;
        current = csharp_indexer_declaration_translate_table[current][token->identifier];
        
        if (current == kCSharpIndexerDeclarationTranslateStateError) {
            break;
        }
        else if (current == kCSharpIndexerDeclarationTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpIndexerDeclarationTranslateStateParameterList) {
            if (cee_token_is_identifier(p, '['))
                parameter_list = p;
        }
        else if (current == kCSharpIndexerDeclarationTranslateStateParameterListEnd) {
            if (cee_token_is_identifier(p, ']'))
                parameter_list_end = p;
        }
        else if (current == kCSharpIndexerDeclarationTranslateStateThis) {
            if (cee_token_is_identifier(p, kCEETokenID_THIS))
                if (!this)
                    this = p;
        }
        else if (current == kCSharpIndexerDeclarationTranslateStateInterfaceType) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                if (!interface_type)
                    interface_type = p;
        }
        else if (current == kCSharpIndexerDeclarationTranslateStateConfirm) {
            if (!prefix_parse) {
                prefix = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
                if (interface_type)
                    prefix_tail = TOKEN_PREV(interface_type);
                else if (this)
                    prefix_tail = TOKEN_PREV(this);
                prefix_parse = TRUE;
            }
            
            if (parameter_list && parameter_list_end) {
                declaration = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                        fragment->subject_ref,
                                                                        parameter_list,
                                                                        parameter_list_end,
                                                                        kCEESourceSymbolTypeVariableDeclaration,
                                                                        "csharp",
                                                                        kCEETokenStringOptionCompact);
            }
            
            if (declaration) {
                if (declaration->name) {
                    cee_free(declaration->name);
                    declaration->name = cee_strdup("indexer[]");
                }
                
                declaration->proto_descriptor = csharp_indexer_proto_descriptor_create(fragment,
                                                                                       prefix,
                                                                                       prefix_tail);
                declarations = cee_list_prepend(declarations, declaration);
            }
            
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCSharpIndexerDeclarationTranslateStateConfirm)
        goto exit;
    
    if (declarations) {
        fragment->symbols = cee_list_concat(fragment->symbols, declarations);
        cee_source_fragment_type_set(fragment, kCEESourceFragmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_EVENTS
    if (declarations)
        cee_source_symbols_print(declarations);
#endif
    
exit:
    
    return ret;
}

static cee_char* csharp_indexer_proto_descriptor_create(CEESourceFragment* fragment,
                                                        CEEList* prefix,
                                                        CEEList* prefix_tail)
{
    if (!fragment)
        return NULL;
    
    CEESourceFragment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* proto = NULL;
    cee_char* descriptor = NULL;
    
    access_level = csharp_access_level_search(fragment, SOURCE_FREGMENT_TOKENS_FIRST(fragment), NULL);
    if (!access_level) {
        grandfather = cee_source_fragment_grandfather_get(fragment);
        if (grandfather) {
            if (cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeClassDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeStructDefinition) ||
                cee_source_fragment_type_is(grandfather, kCEESourceFragmentTypeInterfaceDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"indexer\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto = csharp_type_descriptor_from_token_slice(fragment, prefix, prefix_tail);
    if (proto) {
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto, "\"", NULL);
        cee_free(proto);
    }
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    fprintf(stdout, "%s\n", descriptor);
    
    return descriptor;
}

static void csharp_namespace_definition_translate_table_init(void)
{
    /**
     *                      namespace   identifier      .                       {
     * Init                 Namespace   Error           Error                   Error
     * Namespace            Error       Identifier      Error                   Error
     * Identifier           Error       Error           IdentifierSeparator     Confirm
     * IdentifierSeparator  Error       Identifier      Error                   Error
     */
    TRANSLATE_STATE_INI(csharp_namespace_definition_translate_table, kCSharpNamespaceDefinitionTranslateStateMax                    , kCSharpNamespaceDefinitionTranslateStateError                                             );
    TRANSLATE_STATE_SET(csharp_namespace_definition_translate_table, kCSharpNamespaceDefinitionTranslateStateInit                   , kCEETokenID_NAMESPACE     , kCSharpNamespaceDefinitionTranslateStateNamespace             );
    TRANSLATE_STATE_SET(csharp_namespace_definition_translate_table, kCSharpNamespaceDefinitionTranslateStateNamespace              , kCEETokenID_IDENTIFIER    , kCSharpNamespaceDefinitionTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(csharp_namespace_definition_translate_table, kCSharpNamespaceDefinitionTranslateStateIdentifier             , '.'                       , kCSharpNamespaceDefinitionTranslateStateIdentifierSeparator   );
    TRANSLATE_STATE_SET(csharp_namespace_definition_translate_table, kCSharpNamespaceDefinitionTranslateStateIdentifier             , '{'                       , kCSharpNamespaceDefinitionTranslateStateConfirm               );
    TRANSLATE_STATE_SET(csharp_namespace_definition_translate_table, kCSharpNamespaceDefinitionTranslateStateIdentifierSeparator    , kCEETokenID_IDENTIFIER    , kCSharpNamespaceDefinitionTranslateStateIdentifier            );
}

static cee_boolean csharp_namespace_definition_trap(CEESourceFragment* fragment,
                                                    CEEList** pp)
{
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    CSharpNamespaceDefinitionTranslateState current = kCSharpNamespaceDefinitionTranslateStateInit;
    CSharpNamespaceDefinitionTranslateState prev = kCSharpNamespaceDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    
    CEEList* identifier = NULL;
    CEESourceSymbol* definition = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_namespace_definition_translate_table[current][token->identifier];
    
        if (current == kCSharpNamespaceDefinitionTranslateStateError) {
            break;
        }
        else if (current == kCSharpNamespaceDefinitionTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                TOKEN_APPEND(identifier, token);
        }
        else if (current == kCSharpNamespaceDefinitionTranslateStateConfirm) {
            ret = TRUE;
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCSharpNamespaceDefinitionTranslateStateConfirm)
        goto exit;
    
    
    definition = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                           fragment->subject_ref,
                                                           TOKEN_FIRST(identifier),
                                                           TOKEN_LAST(identifier),
                                                           kCEESourceSymbolTypeNamespaceDefinition,
                                                           "csharp",
                                                           kCEETokenStringOptionCompact);
    if (definition) {
        if (definition->name) {
            cee_free(definition->name);
            definition->name = csharp_name_scope_create(identifier, fragment->subject_ref);
        }
        
        cee_token_slice_state_mark(TOKEN_FIRST(identifier), TOKEN_LAST(identifier), kCEETokenStateSymbolOccupied);
        definition->proto_descriptor = csharp_namespace_proto_descriptor_create(fragment, definition);
        fragment->symbols = cee_list_prepend(fragment->symbols, definition);
        cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeNamespaceDefinition);
    }
    
    
    *pp = NULL;
    
#ifdef DEBUG_NAMESPACE
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
exit:
    
    if (identifier)
        cee_list_free(identifier);
    
    return ret;
}


static void csharp_using_directive_translate_table_init(void)
{
    /**
     *                          [       ]       using   static      Identifier      .                           =               ;
     *  Init                    Init    Init    Using   Error       Error           Error                       Error           Error
     *  Using                   Error   Error   Error   Using       Identifier      Error                       Error           Error
     *  Identifier              Error   Error   Error   Error       Error           IdentifierSeparator         Assignment      Confirm
     *  IdentifierSeparator     Error   Error   Error   Error       Identifier      Error                       Error           Error
     *  Assignment              Error   Error   Error   Error       AliasIdentifier Error                       Error           Error
     *  AliasIdentifier         Error   Error   Error   Error       Error           AliasIdentifierSeparator    Error           Confirm
     *
     */
    TRANSLATE_STATE_INI(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateMax                      , kCSharpUsingDirectiveTranslateStateError                                                  );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateInit                     , '['                       , kCSharpUsingDirectiveTranslateStateInit                       );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateInit                     , ']'                       , kCSharpUsingDirectiveTranslateStateInit                       );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateInit                     , kCEETokenID_USING         , kCSharpUsingDirectiveTranslateStateUsing                      );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateUsing                    , kCEETokenID_STATIC        , kCSharpUsingDirectiveTranslateStateUsing                      );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateUsing                    , kCEETokenID_IDENTIFIER    , kCSharpUsingDirectiveTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateIdentifier               , '.'                       , kCSharpUsingDirectiveTranslateStateIdentifierSeparator        );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateIdentifier               , '='                       , kCSharpUsingDirectiveTranslateStateAssignment                 );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateIdentifier               , ';'                       , kCSharpUsingDirectiveTranslateStateConfirm                    );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateIdentifierSeparator      , kCEETokenID_IDENTIFIER    , kCSharpUsingDirectiveTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateAssignment               , kCEETokenID_IDENTIFIER    , kCSharpUsingDirectiveTranslateStateAliasIdentifier            );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateAliasIdentifier          , '.'                       , kCSharpUsingDirectiveTranslateStateAliasIdentifierSeparator   );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateAliasIdentifier          , ';'                       , kCSharpUsingDirectiveTranslateStateConfirm                    );
    TRANSLATE_STATE_SET(csharp_using_directive_translate_table, kCSharpUsingDirectiveTranslateStateAliasIdentifierSeparator , kCEETokenID_IDENTIFIER    , kCSharpUsingDirectiveTranslateStateAliasIdentifier            );
}

static cee_boolean csharp_using_directive_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CSharpUsingDirectiveTranslateState current = kCSharpUsingDirectiveTranslateStateInit;
    CSharpUsingDirectiveTranslateState prev = kCSharpUsingDirectiveTranslateStateInit;
    CEEToken* token = NULL;
    cee_boolean alias = FALSE;
    CEEList* identifier = NULL;
    CEEList* alias_identifier = NULL;
    CEESourceSymbol* declaration = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        if (token->identifier == kCEETokenID_USING)
            break;
        p = TOKEN_NEXT(p);
    }
    if (!p)
        return FALSE;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = csharp_using_directive_translate_table[current][token->identifier];
        
        if (current == kCSharpUsingDirectiveTranslateStateError)
            break;
        else if (current == kCSharpUsingDirectiveTranslateStateUsing)
            q = p;
        else if (current == kCSharpUsingDirectiveTranslateStateIdentifier)
            TOKEN_APPEND(identifier, token);
        else if (current == kCSharpUsingDirectiveTranslateStateAssignment)
            alias = TRUE;
        else if (current == kCSharpUsingDirectiveTranslateStateAliasIdentifier)
            TOKEN_APPEND(alias_identifier, token);
        else if (current == kCSharpUsingDirectiveTranslateStateConfirm)
            break;

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCSharpUsingDirectiveTranslateStateConfirm)
        goto exit;
    
    if (!alias) {
        declaration = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                                fragment->subject_ref,
                                                                q,
                                                                TOKEN_PREV(p),
                                                                kCEESourceSymbolTypeUsingDeclaration,
                                                                "csharp",
                                                                kCEETokenStringOptionIncompact);
    }
    else {
        declaration = cee_source_symbol_create_from_tokens(fragment->filepath_ref,
                                                           fragment->subject_ref,
                                                           identifier,
                                                           kCEESourceSymbolTypeVariableDeclaration,
                                                           "csharp",
                                                           kCEETokenStringOptionIncompact);
        declaration->proto_descriptor = csharp_using_alias_proto_descriptor_create(fragment,
                                                                                   identifier,
                                                                                   alias_identifier);
    }
    
    if (declaration) {
        fragment->symbols = cee_list_prepend(fragment->symbols, declaration);
        cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeDeclaration);
        ret = TRUE;
    }
    
exit:
    
    if (identifier)
        cee_list_free(identifier);
    
    if (alias_identifier)
        cee_list_free(alias_identifier);
    
#ifdef DEBUG_USING_DIRECTIVE
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
    return ret;
}

static cee_char* csharp_using_alias_proto_descriptor_create(CEESourceFragment* fragment,
                                                            CEEList* identifier,
                                                            CEEList* alias)
{
    if (!fragment || !identifier || !alias)
        return NULL;
    
    cee_char* descriptor = NULL;
    cee_char* name = NULL;
    cee_char* alias_str = NULL;
    CEEList* p = NULL;
    CEEToken* token = NULL;
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"alias\"", NULL);
    
    p = TOKEN_FIRST(identifier);
    while (p) {
        token = p->data;
        cee_string_concat_with_token(&name, fragment->subject_ref, token);
        if (TOKEN_NEXT(p))
            cee_strconcat0(&name, ".", NULL);
        p = TOKEN_NEXT(p);
    }
    
    p = TOKEN_FIRST(alias);
    while (p) {
        token = p->data;
        cee_string_concat_with_token(&alias_str, fragment->subject_ref, token);
        if (TOKEN_NEXT(p))
            cee_strconcat0(&alias_str, ".", NULL);
        p = TOKEN_NEXT(p);
    }
    
    if (name && alias_str) {
        cee_strconcat0(&descriptor, ", \"name\":", name, NULL);
        cee_strconcat0(&descriptor, ", \"alias\":", alias_str, NULL);
    }
        
    cee_strconcat0(&descriptor, "}", NULL);
    
    if (name)
        cee_free(name);
    
    if (alias_str)
        cee_free(alias_str);
        
    return descriptor;
}

static void csharp_delegate_declaration_translate_table_init(void)
{
    /**
     *                              [                       ]                       declaration_specifier   builtin_type    builtin_reference   ?                   delegate    identifier                  .                       <                   ,           (               )                   where               :               struct          new             ,           ;
     *  Init                        Init                    Init                    DeclarationSpecifier    Error           Error               Error               Delegate    Error                       Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  DeclarationSpecifier        DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    Error           Error               Error               Delegate    Error                       Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  Delegate                    Delegate                Delegate                Error                   BuiltinType     BuiltinReference    Error               Error       Identifier                  Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  BuiltinType                 BuiltinType             BuiltinType             Error                   BuiltinType     BuiltinReference    BuiltinType         Error       Identifier                  Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  BuiltinReference            BuiltinReference        BuiltinReference        Error                   BuiltinType     BuiltinReference    BuiltinReference    Error       Identifier                  Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  Identifier                  Identifier              Identifier              Error                   BuiltinType     BuiltinReference    Identifier          Error       Identifier                  IdentifierSeparator     *TypeParameters     Comma       EarameterList   Error               Error               Error           Error           Error           Error       Error
     *  IdentifierSeparator         Error                   Error                   Error                   Error           Error               Error               Error       Identifier                  Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  ParameterList               Error                   Error                   Error                   Error           Error               Error               Error       Error                       Error                   Error               Error       Error           ParameterListEnd    Error               Error           Error           Error           Error       Error
     *  ParameterListEnd            Error                   Error                   Error                   Error           Error               Error               Error       Error                       Error                   Error               Error       Error           Error               ConstraintClause    Error           Error           Error           Error       Confirm
     *  Comma                       Error                   Error                   Error                   Error           Error               Error               Error       Identifier                  Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  ConstraintClause            Error                   Error                   Error                   Error           Error               Error               Error       ConstraintsTypeParameter    Error                   Error               Error       Error           Error               Error               Error           Error           Error           Error       Error
     *  ConstraintsTypeParameter    Error                   Error                   Error                   Error           Error               Error               Error       Error                       Error                   *TypeParameters     Error       Error           Error               Error               Constraints     Error           Error           Error       Error
     *  Constraints                 Error                   Error                   Error                   Constraint      Constraint          Error               Error       Constraint                  Error                   Error               Error       Error           Error               Error               Error           Constraint      Constructor     Error       Error
     *  Constraint                  Error                   Error                   Error                   Constraint      Constraint          Error               Error       Constraint                  Constraint              *TypeParameters     Constraint  Error           Error               ConstraintClause    Error           Constraint      Constructor     Constraint  Confirm
     *  Constructor                 Error                   Error                   Error                   Error           Error               Error               Error       Error                       Error                   Error               Error       EonstructorList Error               Error               Error           Error           Error           Error       Error
     *  ConstructorList             Error                   Error                   Error                   Error           Error               Error               Error       Error                       Error                   Error               Error       Error           Constraint          Error               Error           Error           Error           Error       Error
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateMax                        , kCSharpDelegateDeclarationTranslateStateError                                                             );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateInit                       , '['                                   , kCSharpDelegateDeclarationTranslateStateInit                      );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateInit                       , ']'                                   , kCSharpDelegateDeclarationTranslateStateInit                      );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateInit                       , token_id_is_declaration_specifier     , kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier      );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateInit                       , kCEETokenID_DELEGATE                  , kCSharpDelegateDeclarationTranslateStateDelegate                  );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier       , '['                                   , kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier      );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier       , ']'                                   , kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier      );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier       , token_id_is_declaration_specifier     , kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier      );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier       , token_id_is_builtin_type              , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier       , token_id_is_builtin_reference         , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier       , kCEETokenID_DELEGATE                  , kCSharpDelegateDeclarationTranslateStateDelegate                  );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDeclarationSpecifier       , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDelegate                   , '['                                   , kCSharpDelegateDeclarationTranslateStateDelegate                  );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDelegate                   , ']'                                   , kCSharpDelegateDeclarationTranslateStateDelegate                  );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDelegate                   , token_id_is_builtin_type              , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDelegate                   , token_id_is_builtin_reference         , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateDelegate                   , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinType                , '['                                   , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinType                , ']'                                   , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinType                , token_id_is_builtin_type              , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinType                , token_id_is_builtin_reference         , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinType                , '?'                                   , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinType                , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinReference           , '['                                   , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinReference           , ']'                                   , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinReference           , token_id_is_builtin_type              , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinReference           , token_id_is_builtin_reference         , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinReference           , '?'                                   , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateBuiltinReference           , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , '['                                   , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , ']'                                   , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , token_id_is_builtin_type              , kCSharpDelegateDeclarationTranslateStateBuiltinType               );
    TRANSLATE_FUNCS_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , token_id_is_builtin_reference         , kCSharpDelegateDeclarationTranslateStateBuiltinReference          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , '?'                                   , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , '.'                                   , kCSharpDelegateDeclarationTranslateStateIdentifierSeparator       );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , ','                                   , kCSharpDelegateDeclarationTranslateStateComma                     );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , '<'                                   , kCSharpDelegateDeclarationTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifier                 , '('                                   , kCSharpDelegateDeclarationTranslateStateParameterList             );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateIdentifierSeparator        , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateParameterList              , ')'                                   , kCSharpDelegateDeclarationTranslateStateParameterListEnd          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateParameterListEnd           , kCEETokenID_WHERE                     , kCSharpDelegateDeclarationTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateParameterListEnd           , ';'                                   , kCSharpDelegateDeclarationTranslateStateConfirm                   );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateComma                      , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateIdentifier                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraintClause           , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateConstraintsTypeParameter  );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraintsTypeParameter   , '<'                                   , kCSharpDelegateDeclarationTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraintsTypeParameter   , ':'                                   , kCSharpDelegateDeclarationTranslateStateConstraints               );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraints                , kCEETokenID_CLASS                     , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraints                , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraints                , kCEETokenID_STRUCT                    , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraints                , kCEETokenID_NEW                       , kCSharpDelegateDeclarationTranslateStateConstructor               );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , kCEETokenID_CLASS                     , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , kCEETokenID_IDENTIFIER                , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , kCEETokenID_STRUCT                    , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , kCEETokenID_NEW                       , kCSharpDelegateDeclarationTranslateStateConstructor               );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , kCEETokenID_WHERE                     , kCSharpDelegateDeclarationTranslateStateConstraintClause          );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , '<'                                   , kCSharpDelegateDeclarationTranslateStateTypeParameters            );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , ','                                   , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstructor                , '('                                   , kCSharpDelegateDeclarationTranslateStateConstructorList           );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstructorList            , ')'                                   , kCSharpDelegateDeclarationTranslateStateConstraint                );
    TRANSLATE_STATE_SET(csharp_delegate_declaration_translate_table, kCSharpDelegateDeclarationTranslateStateConstraint                 , ';'                                   , kCSharpDelegateDeclarationTranslateStateConfirm                   );
}

static cee_boolean csharp_delegate_declaration_parse(CEESourceFragment* fragment)
{
    cee_boolean ret = FALSE;
    CSharpDelegateDeclarationTranslateState current = kCSharpDelegateDeclarationTranslateStateInit;
    CSharpDelegateDeclarationTranslateState prev = kCSharpDelegateDeclarationTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEESourceFragment* child = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEESourceSymbol* delegate = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fragment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
                
        prev = current;
        current = csharp_delegate_declaration_translate_table[current][token->identifier];
        
        if (current == kCSharpDelegateDeclarationTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kCSharpDelegateDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (!parameter_list)
                    identifier = p;
            }
        }
        else if (current == kCSharpDelegateDeclarationTranslateStateParameterList) {
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kCSharpDelegateDeclarationTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
        else if (current == kCSharpDelegateDeclarationTranslateStateConfirm) {
            ret = TRUE;
            break;
        }
        else if (current == kCSharpObjectTypeDefinitionTranslateStateError) {
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCSharpDelegateDeclarationTranslateStateConfirm || !identifier || !parameter_list)
        goto exit;
    
    delegate = cee_source_symbol_create_from_token_slice(fragment->filepath_ref,
                                                         fragment->subject_ref,
                                                         identifier,
                                                         identifier,
                                                         kCEESourceSymbolTypeFunctionDeclaration,
                                                         "csharp",
                                                         kCEETokenStringOptionCompact);
    if (delegate) {
        cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
        cee_source_fragment_type_set_exclusive(fragment, kCEESourceFragmentTypeFunctionDeclaration);
        
        child = cee_source_fragment_child_index_by_leaf(fragment, parameter_list->data);
        if (child) {
            child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
            csharp_method_parameters_parse(child);
        }
        delegate->proto_descriptor = csharp_method_proto_descriptor_create(fragment,
                                                                           SOURCE_FREGMENT_TOKENS_FIRST(fragment),
                                                                           TOKEN_PREV(identifier),
                                                                           identifier,
                                                                           parameter_list,
                                                                           parameter_list_end);
        fragment->symbols = cee_list_prepend(fragment->symbols, delegate);
    }
            
#ifdef DEBUG_DELEGATE
    if (delegate)
        cee_source_symbol_print(delegate);
#endif
    
exit:
    return ret;
}

static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFragment* prep_directive,
                                          CEESourceFragment* statement,
                                          CEESourceSymbol** symbol)
{

    cee_boolean ret = FALSE;
    CEESourceSymbol* statement_symbol = NULL;
    CEESourceSymbol* prep_directive_symbol = NULL;
    CEEList* searched_symbols = NULL;
    CEESourceFragment* current = cee_source_fragment_from_reference_get(reference);
    
    if (!current || !symbol || reference->type == kCEESourceReferenceTypeMember)
        goto exit;
    
    *symbol = NULL;
    
    statement_symbol = symbol_search_in_scope_recursive(current, reference->name);
    if (statement_symbol)
        statement_symbol = cee_source_symbol_copy(statement_symbol);
    
    searched_symbols =
        cee_source_fragment_tree_symbols_search(prep_directive,
                                                cee_source_symbol_matcher_by_name,
                                                (cee_pointer)reference->name);
    if (searched_symbols) {
        prep_directive_symbol = cee_source_symbol_copy(cee_list_nth_data(searched_symbols, 0));
        cee_list_free(searched_symbols);
    }
    
    if (statement_symbol && prep_directive_symbol) {
        CEERange range0 = cee_range_consistent_from_discrete(statement_symbol->ranges);
        CEERange range1 = cee_range_consistent_from_discrete(prep_directive_symbol->ranges);
        CEERange range3 = cee_range_consistent_from_discrete(reference->ranges);
        
        if (range1.location > range3.location || range0.location > range1.location) {
            *symbol = statement_symbol;
            cee_source_symbol_free(prep_directive_symbol);
        }
        else if (range1.location > range0.location){
            *symbol = prep_directive_symbol;
            cee_source_symbol_free(statement_symbol);
        }
    }
    else if (statement_symbol) {
        *symbol = statement_symbol;
    }
    else if (prep_directive_symbol) {
        *symbol = prep_directive_symbol;
    }
    
exit:
    if (symbol && *symbol)
        ret = TRUE;
    
    return ret;
}

static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFragment* current,
                                                         const cee_char* name)
{
    CEEList* symbols = NULL;
    CEESourceSymbol* referenced = NULL;
    CEESourceFragment* searching = current;
    CEEList* p = NULL;
    
    if (!searching)
        goto exit;
    
    /** search current fragment */
    symbols = cee_source_fragment_symbols_search_by_name(searching, name);
    if (symbols) {
        referenced = cee_list_nth_data(symbols, 0);
        goto exit;
    }
    
    /**
     * Search current round bracket list child fragment,
     * function parameters , "for" / "while" / "if" statments
     */
    p = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
    while (p) {
        searching = p->data;
        if (searching->type[kCEESourceFragmentTypeRoundBracketList]) {
            symbols = cee_source_fragment_symbols_in_children_search_by_name(searching,
                                                                             name);
            if (symbols) {
                referenced = cee_list_nth_data(symbols, 0);
                goto exit;
            }
        }
        p = SOURCE_FREGMENT_NEXT(p);
    }
    
    
    searching = current;
    
    /** search current siblings */
    if (searching->node_ref) {
        p = SOURCE_FREGMENT_PREV(searching->node_ref);
        while (p) {
            searching = p->data;
            symbols = cee_source_fragment_symbols_search_by_name(searching, name);
            if (symbols) {
                referenced = cee_list_nth_data(symbols, 0);
                goto exit;
            }
            
            /** enumerator search */
            if (searching->type[kCEESourceFragmentTypeEnumDefinition]) {
                CEEList* q = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
                while (q) {
                    searching = q->data;
                    symbols = cee_source_fragment_symbols_in_children_search_by_name(searching,
                                                                                     name);
                    if (symbols) {
                        referenced = cee_list_nth_data(symbols, 0);
                        goto exit;
                    }
                    q = SOURCE_FREGMENT_NEXT(q);
                }
            }
            
            p = SOURCE_FREGMENT_PREV(p);
        }
    }
    
    searching = current;
    if (cee_source_fragment_parent_type_is(searching, kCEESourceFragmentTypeRoundBracketList) ||
        cee_source_fragment_parent_type_is(searching, kCEESourceFragmentTypeCurlyBracketList) ||
        cee_source_fragment_parent_type_is(searching, kCEESourceFragmentTypeSquareBracketList)) {
        searching = cee_source_fragment_grandfather_get(searching);
        referenced = symbol_search_in_scope_recursive(searching, name);
    }
    
exit:
    return referenced;
}
