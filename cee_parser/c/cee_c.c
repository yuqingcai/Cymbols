#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_c.h"
#include "cee_symbol.h"
#include "cee_reference.h"

//#define DEBUG_PREP_DIRECTIVE
//#define DEBUG_CLASS
//#define DEBUG_STRUCT
//#define DEBUG_UNION
//#define DEBUG_ENUM
//#define DEBUG_TEMPLATE
//#define DEBUG_MESSAGE
//#define DEBUG_PROPERTY
//#define DEBUG_DECLARATION
//#define DEBUG_INTERFACE
//#define DEBUG_IMPLEMENTATION
//#define DEBUG_PROTOCOL
//#define DEBUG_NAMESPACE
//#define DEBUG_FUNCTION_DEFINITION
//#define DEBUG_LABEL

typedef enum _ObjectiveCDefinitionType {
    kObjectiveCDefinitionTypeInit = 0,
    kObjectiveCDefinitionTypeInterface,
    kObjectiveCDefinitionTypeImplementation,
    kObjectiveCDefinitionTypeProtocol,
} ObjectiveCDefinitionType;

typedef enum _ObjectiveCMessageTranslateState {
    kObjectiveCMessageTranslateStateInit = 0,
    kObjectiveCMessageTranslateStateMessage,
    kObjectiveCMessageTranslateStateReturnType,
    kObjectiveCMessageTranslateStateReturnTypeEnd,
    kObjectiveCMessageTranslateStateComponent,
    kObjectiveCMessageTranslateStateColon,
    kObjectiveCMessageTranslateStateParameterType,
    kObjectiveCMessageTranslateStateParameterTypeEnd,
    kObjectiveCMessageTranslateStateParameter,
    kObjectiveCMessageTranslateStateDefinition,
    kObjectiveCMessageTranslateStateDeclaration,
    kObjectiveCMessageTranslateStateError,
    kObjectiveCMessageTranslateStateMax
} ObjectiveCMessageTranslateState;

typedef enum _ObjectiveCDefinitionTranslateState {
    kObjectiveCDefinitionTranslateStateInit = 0,
    kObjectiveCDefinitionTranslateStateInterface,
    kObjectiveCDefinitionTranslateStateImplementation,
    kObjectiveCDefinitionTranslateStateProtocol,
    kObjectiveCDefinitionTranslateStateDefinitionName,
    kObjectiveCDefinitionTranslateStateColon,
    kObjectiveCDefinitionTranslateStateDerived,
    kObjectiveCDefinitionTranslateStateExtendBegin,
    kObjectiveCDefinitionTranslateStateExtend,
    kObjectiveCDefinitionTranslateStateExtendEnd,
    kObjectiveCDefinitionTranslateStateProtocolListBegin,
    kObjectiveCDefinitionTranslateStateProtocolNames,
    kObjectiveCDefinitionTranslateStateProtocolListEnd,
    kObjectiveCDefinitionTranslateStateVariableBlock,
    kObjectiveCDefinitionTranslateStateError,
    kObjectiveCDefinitionTranslateStateMax,
} ObjectiveCDefinitionTranslateState;

typedef enum _ObjectiveCPropertyTranslateState {
    kObjectiveCPropertyTranslateStateInit = 0,
    kObjectiveCPropertyTranslateStateProperty,
    kObjectiveCPropertyTranslateStateAttribute,
    kObjectiveCPropertyTranslateStateAttributeEnd,
    kObjectiveCPropertyTranslateStateDeclarationSpecifier,
    kObjectiveCPropertyTranslateStateBuiltinType,
    kObjectiveCPropertyTranslateStateObjectSpecifier,
    kObjectiveCPropertyTranslateStateObjectIdentifier,
    kObjectiveCPropertyTranslateStateObjectDefinition,
    kObjectiveCPropertyTranslateStateObjectDefinitionEnd,
    kObjectiveCPropertyTranslateStateCustomType,
    kObjectiveCPropertyTranslateStateIdentifier,
    kObjectiveCPropertyTranslateStateTypeInit,
    kObjectiveCPropertyTranslateStateConfirm,
    kObjectiveCPropertyTranslateStateError,
    kObjectiveCPropertyTranslateStateMax,
} ObjectiveCPropertyTranslateState;

typedef enum _CTemplateDeclarationTranslateState {
    kCTemplateDeclarationTranslateStateInit = 0,
    kCTemplateDeclarationTranslateStateConfirm,
    kCTemplateDeclarationTranslateStateParameterList,
    kCTemplateDeclarationTranslateStateError,
    kCTemplateDeclarationTranslateStateMax,
} CTemplateDeclarationTranslateState;

typedef enum _CFunctionDefinitionTranslateState {
    kCFunctionDefinitionTranslateStateInit = 0,
    kCFunctionDefinitionTranslateStateDeclarationSpecifier, 
    kCFunctionDefinitionTranslateStateDecltype, 
    kCFunctionDefinitionTranslateStateDecltypeList, 
    kCFunctionDefinitionTranslateStateBuiltinType, 
    kCFunctionDefinitionTranslateStateObjectSpecifier, 
    kCFunctionDefinitionTranslateStateAccessSpecifier, 
    kCFunctionDefinitionTranslateStateIdentifier,
    kCFunctionDefinitionTranslateStateIdentifierScope,
    kCFunctionDefinitionTranslateStateParameterList, 
    kCFunctionDefinitionTranslateStateParameterListEnd, 
    kCFunctionDefinitionTranslateStateVirtualSpecifier, 
    kCFunctionDefinitionTranslateStateNoexcept, 
    kCFunctionDefinitionTranslateStateNoexceptList, 
    kCFunctionDefinitionTranslateStateThrow, 
    kCFunctionDefinitionTranslateStateThrowList, 
    kCFunctionDefinitionTranslateStateInitializer, 
    kCFunctionDefinitionTranslateStateMember, 
    kCFunctionDefinitionTranslateStateMemberInit, 
    kCFunctionDefinitionTranslateStateMemberInitEnd, 
    kCFunctionDefinitionTranslateStateOperator,
    kCFunctionDefinitionTranslateStateOverloadOperator,
    kCFunctionDefinitionTranslateStateOverloadNew,
    kCFunctionDefinitionTranslateStateOverloadDelete,
    kCFunctionDefinitionTranslateStateOverloadType,
    kCFunctionDefinitionTranslateStateOverloadRoundBracket,
    kCFunctionDefinitionTranslateStateOverloadRoundBracketEnd,
    kCFunctionDefinitionTranslateStateOverloadTypeSurrounded,
    kCFunctionDefinitionTranslateStateOverloadTypeSurroundedEnd,
    kCFunctionDefinitionTranslateStateTemplateParameter,
    kCFunctionDefinitionTranslateStateConfirm,
    kCFunctionDefinitionTranslateStateTrailing,
    kCFunctionDefinitionTranslateStateTry,
    kCFunctionDefinitionTranslateStateAttributeSpecifier,
    kCFunctionDefinitionTranslateStateAttributeList,
    kCFunctionDefinitionTranslateStateDeclarationAttributeSpecifier,
    kCFunctionDefinitionTranslateStateDeclarationAttributeList,
    kCFunctionDefinitionTranslateStateBuiltinTypeAttributeSpecifier,
    kCFunctionDefinitionTranslateStateBuiltinTypeAttributeList,
    kCFunctionDefinitionTranslateStateObjectAttributeSpecifier,
    kCFunctionDefinitionTranslateStateObjectAttributeList,
    kCFunctionDefinitionTranslateStateAccessAttributeSpecifier,
    kCFunctionDefinitionTranslateStateAccessAttributeList,
    kCFunctionDefinitionTranslateStateIdentifierAttributeSpecifier,
    kCFunctionDefinitionTranslateStateIdentifierAttributeList,
    kCFunctionDefinitionTranslateStateParameterListEndAttributeSpecifier,
    kCFunctionDefinitionTranslateStateParameterListEndAttributeList,
    kCFunctionDefinitionTranslateStateError,
    kCFunctionDefinitionTranslateStateMax,
} CFunctionDefinitionTranslateState;

typedef enum _CFunctionParametersDeclarationState {
    kCFunctionParametersDeclarationTranslateStateInit = 0,
    kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier,
    kCFunctionParametersDeclarationTranslateStateDecltype,
    kCFunctionParametersDeclarationTranslateStateDecltypeList,
    kCFunctionParametersDeclarationTranslateStateBuiltinType,
    kCFunctionParametersDeclarationTranslateStateObjectSpecifier,
    kCFunctionParametersDeclarationTranslateStateCustomType,
    kCFunctionParametersDeclarationTranslateStateCustomTypeScope,
    kCFunctionParametersDeclarationTranslateStateIdentifier,
    kCFunctionParametersDeclarationTranslateStateIdentifierScope,
    kCFunctionParametersDeclarationTranslateStateTemplateParameter,
    kCFunctionParametersDeclarationTranslateStateParameterList,
    kCFunctionParametersDeclarationTranslateStateParameterListEnd,
    kCFunctionParametersDeclarationTranslateStateSurrounded,
    kCFunctionParametersDeclarationTranslateStateSurroundedEnd,
    kCFunctionParametersDeclarationTranslateStateEllipsis,
    kCFunctionParametersDeclarationTranslateStateConfirm,
    kCFunctionParametersDeclarationTranslateStateError,
    kCFunctionParametersDeclarationTranslateStateMax,
} CFunctionParametersDeclarationState;

typedef enum _CDeclarationTranslateState {
    kCDeclarationTranslateStateInit = 0,
    kCDeclarationTranslateStateTypedef,
    kCDeclarationTranslateStateDeclarationSpecifier,
    kCDeclarationTranslateStateAccessSpecifier,
    kCDeclarationTranslateStateBuiltinType,
    kCDeclarationTranslateStateCustomType,
    kCDeclarationTranslateStateStructor,
    kCDeclarationTranslateStateStructorEnd,
    kCDeclarationTranslateStateCustomTypeScope,
    kCDeclarationTranslateStateObjectSpecifier,
    kCDeclarationTranslateStateObjectAlignasSpecifier,
    kCDeclarationTranslateStateObjectAlignasList,
    kCDeclarationTranslateStateObjectIdentifier,
    kCDeclarationTranslateStateObjectIdentifierScope,
    kCDeclarationTranslateStateObjectDerive,
    kCDeclarationTranslateStateObjectDeriveIdentifier,
    kCDeclarationTranslateStateObjectDeriveIdentifierScope,
    kCDeclarationTranslateStateObjectDefinition,
    kCDeclarationTranslateStateObjectDefinitionEnd,
    kCDeclarationTranslateStateAlignasSpecifier,
    kCDeclarationTranslateStateAlignasList,
    kCDeclarationTranslateStateDecltype,
    kCDeclarationTranslateStateDecltypeList,
    kCDeclarationTranslateStateIdentifier,
    kCDeclarationTranslateStateIdentifierScope,
    kCDeclarationTranslateStateParameterList,
    kCDeclarationTranslateStateParameterListEnd,
    kCDeclarationTranslateStateInitList,
    kCDeclarationTranslateStateInitListEnd,
    kCDeclarationTranslateStateSurrounded,
    kCDeclarationTranslateStateSurroundedEnd,
    kCDeclarationTranslateStateNoexcept,
    kCDeclarationTranslateStateNoexceptList,
    kCDeclarationTranslateStateNoexceptListEnd,
    kCDeclarationTranslateStateThrow,
    kCDeclarationTranslateStateThrowList,
    kCDeclarationTranslateStateThrowListEnd,
    kCDeclarationTranslateStateOperator,
    kCDeclarationTranslateStateOverloadOperator,
    kCDeclarationTranslateStateOverloadNew,
    kCDeclarationTranslateStateOverloadDelete,
    kCDeclarationTranslateStateOverloadType,
    kCDeclarationTranslateStateOverloadRoundBracket,
    kCDeclarationTranslateStateOverloadRoundBracketEnd,
    kCDeclarationTranslateStateOverloadTypeSurrounded,
    kCDeclarationTranslateStateOverloadTypeSurroundedEnd,
    kCDeclarationteStateTypeConfirmed,
    kCDeclarationTranslateStateTemplateParameter,
    kCDeclarationTranslateStateConfirm,
    kCDeclarationTranslateStateBitField,
    kCDeclarationTranslateStateBitSize,
    kCDeclarationTranslateStateTrailing,
    kCDeclarationTranslateStateTypeInit,
    kCDeclarationTranslateStateAttributeSpecifier,
    kCDeclarationTranslateStateAttributeList,
    kCDeclarationTranslateStateTypedefAttributeSpecifier,
    kCDeclarationTranslateStateTypedefAttributeList,
    kCDeclarationTranslateStateDeclarationAttributeSpecifier,
    kCDeclarationTranslateStateDeclarationAttributeList,
    kCDeclarationTranslateStateAccessAttributeSpecifier,
    kCDeclarationTranslateStateAccessAttributeList,
    kCDeclarationTranslateStateBuiltinTypeAttributeSpecifier,
    kCDeclarationTranslateStateBuiltinTypeAttributeList,
    kCDeclarationTranslateStateCustomTypeAttributeSpecifier,
    kCDeclarationTranslateStateCustomTypeAttributeList,
    kCDeclarationTranslateStateObjectAttributeSpecifier,
    kCDeclarationTranslateStateObjectAttributeList,
    kCDeclarationTranslateStateObjectDefinitionEndAttributeSpecifier,
    kCDeclarationTranslateStateObjectDefinitionEndAttributeList,
    kCDeclarationTranslateStateIdentifierAttributeSpecifier,
    kCDeclarationTranslateStateIdentifierAttributeList,
    kCDeclarationTranslateStateParameterListEndAttributeSpecifier,
    kCDeclarationTranslateStateParameterListEndAttributeList,
    kCDeclarationTranslateStateTypeInitAttributeSpecifier,
    kCDeclarationTranslateStateTypeInitAttributeList,
    kCDeclarationTranslateStateObjectIdentifierAttributeSpecifier,
    kCDeclarationTranslateStateObjectIdentifierAttributeList,
    kCDeclarationTranslateStateObjectDeriveIdentifierAttributeSpecifier,
    kCDeclarationTranslateStateObjectDeriveIdentifierAttributeList,
    kCDeclarationTranslateStateError,
    kCDeclarationTranslateStateMax,
} CDeclarationTranslateState;

typedef enum _CInheritanceDefinitionTranslateState {
    kCInheritanceDefinitionTranslateStateInit = 0,
    kCInheritanceDefinitionTranslateStateObjectSpecifier,
    kCInheritanceDefinitionTranslateStateAlignasSpecifier,
    kCInheritanceDefinitionTranslateStateAlignasList,
    kCInheritanceDefinitionTranslateStateObjectIdentifier,
    kCInheritanceDefinitionTranslateStateObjectIdentifierScope,
    kCInheritanceDefinitionTranslateStateObjectDerive,
    kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier,
    kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope,
    kCInheritanceDefinitionTranslateStateTemplateParameter,
    kCInheritanceDefinitionTranslateStateConfirm,
    kCInheritanceDefinitionTranslateStateObjectAttributeSpecifier,
    kCInheritanceDefinitionTranslateStateObjectAttributeSpecifierList,
    kCInheritanceDefinitionTranslateStateObjectIdentifierAttributeSpecifier,
    kCInheritanceDefinitionTranslateStateObjectIdentifierAttributeList,
    kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierAttributeSpecifier,
    kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierAttributeList,
    kCInheritanceDefinitionTranslateStateError,
    kCInheritanceDefinitionTranslateStateMax,
} CInheritanceDefinitionTranslateState;

typedef enum _CPrepDirectiveIncludeTranslateState {
    kCPrepDirectiveIncludeTranslateStateInit = 0,
    kCPrepDirectiveIncludeTranslateStateDirective,
    kCPrepDirectiveIncludeTranslateStatePath,
    kCPrepDirectiveIncludeTranslateStateConfirm,
    kCPrepDirectiveIncludeTranslateStateError,
    kCPrepDirectiveIncludeTranslateStateMax,
} CPrepDirectiveIncludeTranslateState;

typedef enum _CPrepDirectiveDefineTranslateState {
    kCPrepDirectiveDefineTranslateStateInit = 0,
    kCPrepDirectiveDefineTranslateStateDirective,
    kCPrepDirectiveDefineTranslateStateDefineName,
    kCPrepDirectiveDefineTranslateStateParameterList,
    kCPrepDirectiveDefineTranslateStateParameter,
    kCPrepDirectiveDefineTranslateStateConfirm,
    kCPrepDirectiveDefineTranslateStateError,
    kCPrepDirectiveDefineTranslateStateMax,
} CPrepDirectiveDefineTranslateState;

typedef enum _CNamespaceDefinitionTranslateState {
    kCNamespaceDefinitionTranslateStateInit = 0,
    kCNamespaceDefinitionTranslateStateNamespace,
    kCNamespaceDefinitionTranslateStateDefinitionName,
    kCNamespaceDefinitionTranslateStateNameScope,
    kCNamespaceDefinitionTranslateStateInlineSpecifier,
    kCNamespaceDefinitionTranslateStateAlignas,
    kCNamespaceDefinitionTranslateStateAlignasBegin,
    kCNamespaceDefinitionTranslateStateConfirm,
    kCNamespaceDefinitionTranslateStateError,
    kCNamespaceDefinitionTranslateStateMax,
} CNamespaceDefinitionTranslateState;

typedef enum _CExternBlockTranslateState {
    kCExternBlockTranslateStateInit = 0,
    kCExternBlockTranslateStateExtern,
    kCExternBlockTranslateStateIdentifier,
    kCExternBlockTranslateStateConfirm,
    kCExternBlockTranslateStateError,
    kCExternBlockTranslateStateMax,
} CExternBlockTranslateState;

typedef enum _CReferenceTranslateState {
    kCReferenceTranslateStateInit = 0,
    kCReferenceTranslateStateIdentifier,
    kCReferenceTranslateStateTilde,
    kCReferenceTranslateStateFunction,
    kCReferenceTranslateStateConfirm,
    kCReferenceTranslateStateError,
    kCReferenceTranslateStateMax,
} CReferenceTranslateState;

typedef enum _CProtosTranslateState {
    kCProtosTranslateStateInit = 0,
    kCProtosTranslateStateAccessSpecifier,
    kCProtosTranslateStateDecltype,
    kCProtosTranslateStateDecltypeList,
    kCProtosTranslateStateDecltypeConfirm,
    kCProtosTranslateStateCustomType,
    kCProtosTranslateStateCustomTypeConfirm,
    kCProtosTranslateStateTemplateParameter,
    kCProtosTranslateStateCustomTypeScope,
    kCProtosTranslateStateAlignasSpecifier,
    kCProtosTranslateStateAlignasList,
    kCProtosTranslateStateAlignasListEnd,
    kCProtosTranslateStateObjectSpecifier,
    kCProtosTranslateStateObjectIdentifier,
    kCProtosTranslateStateObjectIdentifierConfirm,
    kCProtosTranslateStateObjectIdentifierScope,
    kCProtosTranslateStateObjectDerive,
    kCProtosTranslateStateObjectDeriveIdentifier,
    kCProtosTranslateStateObjectDeriveIdentifierScope,
    kCProtosTranslateStateObjectDefinition,
    kCProtosTranslateStateObjectDefinitionConfirm,
    kCProtosTranslateStateBuiltinConfirm,
    kCProtosTranslateStateError,
    kCProtosTranslateStateMax,
} CProtosTranslateState;

typedef enum _ObjectiveCEnumDefinitionTranslateState {
    kObjectiveCEnumDefinitionTranslateStateInit,
    kObjectiveCEnumDefinitionTranslateStateEnum,
    kObjectiveCEnumDefinitionTranslateStateOPtions,
    kObjectiveCEnumDefinitionTranslateStateParameterList,
    kObjectiveCEnumDefinitionTranslateStateTypeSpecifier,
    kObjectiveCEnumDefinitionTranslateStateComma,
    kObjectiveCEnumDefinitionTranslateStateIdentifier,
    kObjectiveCEnumDefinitionTranslateStateParameterListEnd,
    kObjectiveCEnumDefinitionTranslateStateConfirm,
    kObjectiveCEnumDefinitionTranslateStateError,
    kObjectiveCEnumDefinitionTranslateStateMax,
} ObjectiveCEnumDefinitionTranslateState;

typedef cee_boolean (*ParseTrap)(CEESourceFregment*, 
                                 CEEList**);

typedef enum _CParserState {
    kCParserStateStatementParsing = 0x1 << 0,
    kCParserStatePrepDirectiveParsing = 0x1 << 1,
} CParserState;

typedef struct _CParser {
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
    CParserState state;
} CParser;

static CEETokenType c_token_type_map[CEETokenID_MAX];
static cee_int objective_c_message_translate_table[kObjectiveCMessageTranslateStateMax][CEETokenID_MAX];
static cee_int objective_c_property_translate_table[kObjectiveCPropertyTranslateStateMax][CEETokenID_MAX];
static cee_int objective_c_definition_translate_table[kObjectiveCDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int objective_c_enum_definition_translate_table[kObjectiveCEnumDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int c_template_declaration_translate_table[kCTemplateDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int c_function_definition_translate_table[kCFunctionDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int c_function_parameters_declaration_translate_table[kCFunctionParametersDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int c_declaration_translate_table[kCDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int c_inheritance_definition_translate_table[kCInheritanceDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int c_prep_directive_include_translate_table[kCPrepDirectiveIncludeTranslateStateMax][CEETokenID_MAX];
static cee_int c_prep_directive_define_translate_table[kCPrepDirectiveDefineTranslateStateMax][CEETokenID_MAX];
static cee_int c_namespace_definition_translate_table[kCNamespaceDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int c_proto_translate_table[kCProtosTranslateStateMax][CEETokenID_MAX];
static cee_int c_extern_block_translate_table[kCExternBlockTranslateStateMax][CEETokenID_MAX];

static void c_token_type_map_init(void);
static cee_boolean token_id_is_prep_directive(CEETokenID identifier);
static cee_boolean token_id_is_prep_directive_condition(CEETokenID identifier);
static cee_boolean token_id_is_keyword(CEETokenID identifier);
static cee_boolean token_id_is_punctuation(CEETokenID identifier);
static cee_boolean token_id_is_assignment(CEETokenID identifier);
static cee_boolean token_id_is_builtin_type(CEETokenID identifier);
static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier);
static cee_boolean token_id_is_access_specifier(CEETokenID identifier);
static cee_boolean token_id_is_alignas_specifier(CEETokenID identifier);
static cee_boolean token_id_is_object_specifier(CEETokenID identifier);
static cee_boolean token_id_is_virtual_sepcifier(CEETokenID identifier);
static cee_boolean is_name_scope(CEEList* p);

/** comment */
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_attach(CParser* parser);
static cee_boolean comment_token_push(CParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(CParser* parser);

/** prep directive */
static cee_boolean prep_directive_parsing(CParser* parser);
static cee_boolean prep_directive_token_push(CParser* parser,
                                             CEEToken* push);
static cee_boolean prep_directive_terminated(CParser* parser);
static cee_boolean prep_directive_attach(CParser* parser,
                                         CEESourceFregmentType type);
static cee_boolean prep_directive_sub_attach(CParser* parser,
                                             CEESourceFregmentType type);
static void prep_directive_branch_push(CParser* parser);
static cee_boolean prep_directive_branch_pop(CParser* parser);
static cee_boolean prep_directive_pop(CParser* parser);
static void prep_directive_parse(CParser* parser);
static cee_boolean should_statement_parsing(CParser* parser);
static void c_prep_directive_include_translate_table_init(void);
static cee_boolean prep_directive_include_parse(CEESourceFregment* fregment);
static void c_prep_directive_define_translate_table_init(void);
static cee_boolean prep_directive_define_parse(CEESourceFregment* fregment);
static cee_boolean prep_directive_common_parse(CEESourceFregment* fregment);

/** statement */
static cee_boolean statement_parsing(CParser* parser);
static cee_boolean statement_attach(CParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(CParser* parser,
                                        CEESourceFregmentType type);
static cee_boolean statement_pop(CParser* parser);
static void block_push(CParser* parser);
static cee_boolean block_pop(CParser* parser);
static void block_parse(CParser* parser);
static void c_template_declaration_translate_table_init(void);
static cee_boolean c_template_token_push(CParser* parser,
                                         CEEToken* push);
static cee_boolean c_template_parameters_parse(CEESourceFregment* fregment);
static cee_char* c_template_proto_descriptor_create(CEESourceFregment* fregment,
                                                    CEESourceSymbol* template);
static void template_push(CParser* parser);
static cee_boolean template_pop(CParser* parser);
static void c_inheritance_definition_translate_table_init(void);
static cee_boolean c_inheritance_parse(CEEList* tokens,
                                       CEEList** inherited,
                                       CEEList** derives);
CEESourceSymbol* c_inheritance_definition_create(CEESourceFregment* fregment,
                                                 CEEList* tokens,
                                                 const cee_char* filepath,
                                                 const cee_char* subject,
                                                 CEESourceSymbolType symbol_type);
static cee_char* c_inheritance_proto_descriptor_create(CEESourceFregment* fregment,
                                                       CEESourceSymbol* inheritance,
                                                       const cee_char* derives_str);
static cee_char* c_name_scope_list_string_create(CEEList* scopes,
                                                 const cee_char* subject);
static cee_char* c_name_scope_create(CEEList* tokens,
                                     const cee_char* subject);
static void derived_chain_free(cee_pointer data);
static cee_boolean c_class_definition_trap(CEESourceFregment* fregment,
                                           CEEList** pp);
static cee_boolean c_struct_definition_trap(CEESourceFregment* fregment,
                                            CEEList** pp);
static cee_boolean c_enum_definition_trap(CEESourceFregment* fregment,
                                          CEEList** pp);
static cee_boolean c_enumerators_parse(CEESourceFregment* fregment);
static cee_char* c_enumerator_proto_descriptor_create(CEESourceFregment* fregment,
                                                      CEESourceSymbol* enumerator,
                                                      const cee_char* proto);
static cee_boolean c_union_definition_trap(CEESourceFregment* fregment,
                                           CEEList** pp);
static cee_boolean objective_c_enum_definition_trap(CEESourceFregment* fregment,
                                                    CEEList** pp);
static cee_boolean statement_block_parse(CEESourceFregment* fregment);
static void c_function_definition_translate_table_init(void);
static cee_boolean c_function_definition_parse(CEESourceFregment* fregment);
static cee_boolean c_function_definition_is_proto(CEESourceFregment* fregment,
                                                  CEEList* identifier);
static cee_boolean should_proto_descriptor_append_whitespace(CEEToken* token,
                                                                    CEEToken* token_prev);
static cee_boolean token_id_is_c_function_proto_header_ignored(CEETokenID identifier);
static void c_function_parameters_declaration_translate_table_init(void);
static cee_boolean c_function_parameters_parse(CEESourceFregment* fregment);
static CEESourceSymbol* c_function_parameter_identifier_create(CEESourceFregment* fregment,
                                                               CEEList* prefix,
                                                               CEEList* identifier);
static CEESourceSymbol* c_function_parameter_surrounded_create(CEESourceFregment* fregment,
                                                               CEEList* prefix,
                                                               CEEList* surrounded);
static cee_char* c_function_parameter_proto_descriptor_create(CEESourceFregment* fregment,
                                                              CEEList* prefix,
                                                              CEEList* prefix_tail,
                                                              CEESourceSymbol* parameter);
static void objective_c_message_translate_table_init(void);
static cee_boolean objective_c_message_parse(CEESourceFregment* fregment);
static cee_char* objective_c_message_proto_descriptor_create(CEESourceFregment* fregment,
                                                             CEEList* message_begin,
                                                             CEEList* return_type,
                                                             CEEList* return_type_end,
                                                             CEEList* name_chain,
                                                             CEEList* parameter_types,
                                                             CEEList* parameters);
static cee_char* objective_c_message_parameter_proto_descriptor_create(CEESourceFregment* fregment,
                                                                       cee_int index,
                                                                       CEESourceSymbol* parameter);
static void c_proto_translate_table_init(void);
static cee_char* c_type_descriptor_from_token_slice(CEESourceFregment* fregment,
                                                    CEEList* p,
                                                    CEEList* q);
static CEESourceSymbol* objective_c_interface_extract(CEESourceFregment* fregment,
                                                      CEEList* tokens,
                                                      const cee_char* filepath,
                                                      const cee_char* subject);
static cee_char* objective_c_interface_proto_descriptor_create(CEESourceFregment* fregment,
                                                               CEESourceSymbol* interface,
                                                               const cee_char* derives_str);
static CEESourceSymbol* objective_c_implementation_extract(CEESourceFregment* fregment,
                                                           CEEList* tokens,
                                                           const cee_char* filepath,
                                                           const cee_char* subject);
static cee_char* objective_c_implementation_proto_descriptor_create(CEESourceFregment* fregment,
                                                                    CEESourceSymbol* implementation);
static CEESourceSymbol* objective_c_protocol_extract(CEESourceFregment* fregment,
                                                     CEEList* tokens,
                                                     const cee_char* filepath,
                                                     const cee_char* subject);
static cee_char* objective_c_protocol_proto_descriptor_create(CEESourceFregment* fregment,
                                                              CEESourceSymbol* protocol);
static cee_char* c_name_list_create(CEEList* tokens,
                                    const cee_char* subject);
static void objective_c_property_translate_table_init(void);
static cee_boolean objective_c_property_parse(CEESourceFregment* fregment);
static void c_declaration_translate_table_init(void);
static CEEList* skip_template_parameter_list(CEEList* p,
                                             cee_boolean reverse);
static CEEList* skip_include_path(CEEList* p);
static CEEList* skip_c_declaration_interval(CEEList* p);
static cee_boolean c_declaration_parse(CEESourceFregment* fregment);
static CEEList* c_declaration_prefix_tail_get(CEESourceFregment* fregment,
                                              CEEList* identifier,
                                              CEEList* object_identifier,
                                              CEEList* parameter_list,
                                              CEEList* surround);
static CEESourceSymbol* c_function_declaration_create(CEESourceFregment* fregment,
                                                      CEEList* prefix,
                                                      CEEList* prefix_tail,
                                                      CEEList* identifier,
                                                      CEEList* parameter_list,
                                                      CEEList* parameter_list_end);
static CEESourceSymbol* c_function_pointer_declaration_create(CEESourceFregment* fregment,
                                                              CEEList* prefix,
                                                              CEEList* prefix_tail,
                                                              CEEList* parameter_list,
                                                              CEEList* parameter_list_end,
                                                              CEEList* surround,
                                                              CEEList* surround_end);
static cee_char* c_function_proto_descriptor_create(CEESourceFregment* fregment,
                                                    CEEList* prefix,
                                                    CEEList* prefix_tail,
                                                    CEEList* identifier,
                                                    CEEList* parameter_list,
                                                    CEEList* parameter_list_end);
static CEESourceSymbol* c_identifier_declaration_create(CEESourceFregment* fregment,
                                                        CEEList* prefix,
                                                        CEEList* prefix_tail,
                                                        CEEList* identifier);
static CEESourceSymbol* c_custom_type_declaration_create(CEESourceFregment* fregment,
                                                         CEEList* prefix,
                                                         CEEList* prefix_tail,
                                                         CEEList* identifier);
static cee_char* c_declaration_proto_descriptor_create(CEESourceFregment* fregment,
                                                       CEEList* prefix,
                                                       CEEList* prefix_tail,
                                                       CEEList* identifier);
static void c_namespace_definition_translate_table_init(void);
static cee_boolean c_namespace_definition_trap(CEESourceFregment* fregment,
                                               CEEList** pp);
static cee_char* c_namespace_proto_descriptor_create(CEESourceFregment* fregment,
                                                     CEESourceSymbol* namespace);
static void c_extern_block_translate_table_init(void);
static cee_boolean c_extern_block_parse(CEESourceFregment* fregment);
static void block_header_parse(CParser* parser);
static cee_boolean block_reduce(CParser* parser);
static cee_boolean colon_parse(CParser* parser);
static cee_boolean label_parse(CEESourceFregment* fregment);
static cee_boolean access_level_parse(CEESourceFregment* fregment);
static void parser_access_level_transform(CParser* parser,
                                          CEEToken* token);
static void access_level_transform(CEESourceFregment* current,
                                   CEEToken* token);
static void parameter_list_push(CParser* parser);
static cee_boolean parameter_list_pop(CParser* parser);
static void subscript_push(CParser* parser);
static cee_boolean subscript_pop(CParser* parser);
static void objective_c_definition_translate_table_init(void);
static cee_boolean objective_c_token_push(CParser* parser,
                                          CEEToken* push);
static cee_boolean objective_c_definition_parse(CEESourceFregment* fregment,
                                                const cee_char* subject);
static void statement_parse(CParser* parser);
static cee_boolean statement_reduce(CParser* parser);
static void c_block_header_trap_init(CParser* parser);
static CParser* parser_create(void);
static void parser_free(cee_pointer data);
static void symbol_parse_init(CParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
static void symbol_parse_clear(CParser* parser);
static void parser_state_set(CParser* parser,
                             CParserState state);
static void parser_state_clear(CParser* parser,
                               CParserState state);
static cee_boolean current_statement_type_is(CParser* parser,
                                             CEESourceFregmentType type);
static void current_statement_type_transform(CParser* parser,
                                             CEESourceFregmentType type);
static cee_boolean statement_token_push(CParser* parser,
                                        CEEToken* push);
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
static cee_boolean references_in_source_fregment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
                                                       CEESourceFregment* fregment,
                                                       const cee_char* subject,
                                                       CEESourceFregment* prep_directive,
                                                       CEESourceFregment* statement,
                                                       CEERange range,
                                                       CEEList** references);
static CEESourceFregment* c_referernce_fregment_convert(CEESourceFregment* fregment,
                                                        const cee_char* subject);
static void c_reference_fregment_parse(CEESourceParserRef parser_ref,
                                       const cee_char* filepath,
                                       CEESourceFregment* fregment,
                                       const cee_char* subject,
                                       CEESourceFregment* prep_directive,
                                       CEESourceFregment* statement,
                                       CEEList** references);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type);
static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFregment* prep_directive,
                                          CEESourceFregment* statement,
                                          CEESourceSymbol** symbol);
static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFregment* searching,
                                                         const cee_char* name);

static void c_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        c_token_type_map[i] = 0;
    
    c_token_type_map[kCEETokenID_ALIGNAS]                      = kCEETokenTypeKeyword | kCEETokenTypeAlignasSpecifier;
    c_token_type_map[kCEETokenID_ALIGNOF]                      = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AND]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AND_EQ]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ASM]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ATOMIC_CANCEL]                = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ATOMIC_COMMIT]                = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ATOMIC_NOEXCEPT]              = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AUTO]                         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_BITAND]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_BITOR]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_BOOL]                         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_BREAK]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_CASE]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_CATCH]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_CHAR]                         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_CHAR16_T]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_CHAR32_T]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_CLASS]                        = kCEETokenTypeKeyword | kCEETokenTypeObjectSpecifier;
    c_token_type_map[kCEETokenID_COMPL]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_CONCEPT]                      = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_CONST]                        = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_CONSTEVAL]                    = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_CONSTEXPR]                    = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_CONST_CAST]                   = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_CONTINUE]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_DECLTYPE]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_DEFAULT]                      = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_DELETE]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_DO]                           = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_DOUBLE]                       = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_DYNAMIC_CAST]                 = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ELSE]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ENUM]                         = kCEETokenTypeKeyword | kCEETokenTypeObjectSpecifier;
    c_token_type_map[kCEETokenID_EXPLICIT]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_EXPORT]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_EXTERN]                       = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_FALSE]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_FLOAT]                        = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_FOR]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_FRIEND]                       = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_GOTO]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_IF]                           = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_IMPORT]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_INLINE]                       = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_INT]                          = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_LONG]                         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_MODULE]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_MUTABLE]                      = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_NAMESPACE]                    = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NEW]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NOEXCEPT]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NOT]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NOT_EQ]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NULLPTR]                      = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_OPERATOR]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_OR]                           = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_OR_EQ]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_PRIVATE]                      = kCEETokenTypeKeyword | kCEETokenTypeAccessSpecifier;
    c_token_type_map[kCEETokenID_PROTECTED]                    = kCEETokenTypeKeyword | kCEETokenTypeAccessSpecifier;
    c_token_type_map[kCEETokenID_PUBLIC]                       = kCEETokenTypeKeyword | kCEETokenTypeAccessSpecifier;
    c_token_type_map[kCEETokenID_REGISTER]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_REINTERPRET_CAST]             = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_REQUIRES]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_RESTRICT]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_RETURN]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_SHORT]                        = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED]                       = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIZEOF]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_STATIC]                       = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_STATIC_ASSERT]                = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_STATIC_CAST]                  = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_STRUCT]                       = kCEETokenTypeKeyword | kCEETokenTypeObjectSpecifier;
    c_token_type_map[kCEETokenID_SWITCH]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_SYNCHRONIZED]                 = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_TEMPLATE]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_THIS]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_THREAD_LOCAL]                 = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_THROW]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_TRUE]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_TRY]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_TYPEDEF]                      = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_TYPEID]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_TYPENAME]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_UNION]                        = kCEETokenTypeKeyword | kCEETokenTypeObjectSpecifier;
    c_token_type_map[kCEETokenID_UNSIGNED]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_USING]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_VIRTUAL]                      = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_VOLATILE]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_WCHAR_T]                      = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_WHILE]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_XOR]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_XOR_EQ]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_OVERRIDE]                     = kCEETokenTypeKeyword | kCEETokenTypeVirtualSpecifier;
    c_token_type_map[kCEETokenID_FINAL]                        = kCEETokenTypeKeyword | kCEETokenTypeVirtualSpecifier;
    c_token_type_map[kCEETokenID_TRANSACTION_SAFE]             = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_TRANSACTION_SAFE_DYNAMIC]     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID__ALIGNAS]                     = kCEETokenTypeKeyword | kCEETokenTypeAlignasSpecifier;
    c_token_type_map[kCEETokenID__ALIGNOF]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID__ATOMIC]                      = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID__BOOL]                        = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID__COMPLEX]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID__GENERIC]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID__IMAGINARY]                   = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID__NORETURN]                    = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID__STATIC_ASSERT]               = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID__THREAD_LOCAL]                = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_VOID]                         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_LONG_DOUBLE]                  = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_CHAR]                  = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_CHAR]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SHORT_INT]                    = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_SHORT]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_SHORT_INT]             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_SHORT]               = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_SHORT_INT]           = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_INT]                   = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_INT]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_LONG_INT]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_LONG]                  = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_LONG_INT]              = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_LONG]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_LONG_INT]            = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_LONG_LONG]                    = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_LONG_LONG_INT]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_LONG_LONG]             = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SIGNED_LONG_LONG_INT]         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_LONG_LONG]           = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UNSIGNED_LONG_LONG_INT]       = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT8_T]                       = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT16_T]                      = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT32_T]                      = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT64_T]                      = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_FAST8_T]                  = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_FAST16_T]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_FAST32_T]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_FAST64_T]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_LEAST8_T]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_LEAST16_T]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_LEAST32_T]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INT_LEAST64_T]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INTMAX_T]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INTPTR_T]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT8_T]                      = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT16_T]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT32_T]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT64_T]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_FAST8_T]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_FAST16_T]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_FAST32_T]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_FAST64_T]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_LEAST8_T]                = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_LEAST16_T]               = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_LEAST32_T]               = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINT_LEAST64_T]               = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINTMAX_T]                    = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_UINTPTR_T]                    = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_AT_INTERFACE]                 = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_IMPLEMENTATION]            = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_PROTOCOL]                  = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_END]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_CLASS]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_PUBLIC]                    = kCEETokenTypeKeyword | kCEETokenTypeAccessSpecifier;
    c_token_type_map[kCEETokenID_AT_PACKAGE]                   = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_PROTECTED]                 = kCEETokenTypeKeyword | kCEETokenTypeAccessSpecifier;
    c_token_type_map[kCEETokenID_AT_PRIVATE]                   = kCEETokenTypeKeyword | kCEETokenTypeAccessSpecifier;
    c_token_type_map[kCEETokenID_AT_PROPERTY]                  = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_SYNTHESIZE]                = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_DYNAMIC]                   = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_REQUIRED]                  = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_OPTIONAL]                  = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_TRY]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_CATCH]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_FINALLY]                   = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_THROW]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_SELECTOR]                  = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_ENCODE]                    = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_DEFS]                      = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_AUTORELEASEPOOL]           = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_SYNCHRONIZED]              = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_AT_COMPATIBILITY_ALIAS]       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ID]                           = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_INSTANCETYPE]                 = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_IMP]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_IN]                           = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_INOUT]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NIL]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ONEWAY]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_OUT]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_SEL]                          = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    c_token_type_map[kCEETokenID_SELF]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_SUPER]                        = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_YES]                          = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NO]                           = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ATOMIC]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NONATOMIC]                    = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_STRONG]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_WEAK]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_ASSIGN]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_COPY]                         = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_RETAIN]                       = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_READONLY]                     = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_READWRITE]                    = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID___BRIDGE]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID___BRIDGE_TRANSFER]            = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID___BRIDGE_RETAINED]            = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID___BLOCK]                      = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID__NONNULL]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID__NULLABLE]                    = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_NONNULL]                      = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID_NULLABLE]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    c_token_type_map[kCEETokenID___KIND_OF]                    = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NS_ENUM]                      = kCEETokenTypeKeyword;
    c_token_type_map[kCEETokenID_NS_OPTIONS]                   = kCEETokenTypeKeyword;
    c_token_type_map['=']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map['+']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['-']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['*']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['/']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['\\']                                     = kCEETokenTypePunctuation;
    c_token_type_map['%']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['~']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['&']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['|']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['^']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['!']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['<']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['>']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['.']                                      = kCEETokenTypePunctuation;
    c_token_type_map[',']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['?']                                      = kCEETokenTypePunctuation;
    c_token_type_map[':']                                      = kCEETokenTypePunctuation;
    c_token_type_map['(']                                      = kCEETokenTypePunctuation;
    c_token_type_map[')']                                      = kCEETokenTypePunctuation;
    c_token_type_map['{']                                      = kCEETokenTypePunctuation;
    c_token_type_map['}']                                      = kCEETokenTypePunctuation;
    c_token_type_map['[']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[']']                                      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map['#']                                      = kCEETokenTypePunctuation;
    c_token_type_map[';']                                      = kCEETokenTypePunctuation;
    c_token_type_map['@']                                      = kCEETokenTypePunctuation;
    c_token_type_map[kCEETokenID_ADD_ASSIGNMENT]               = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_MINUS_ASSIGNMENT]             = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_MULTI_ASSIGNMENT]             = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_DIV_ASSIGNMENT]               = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_MOD_ASSIGNMENT]               = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_AND_ASSIGNMENT]               = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_OR_ASSIGNMENT]                = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_XOR_ASSIGNMENT]               = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_LEFT_OFFSET_ASSIGNMENT]       = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_RIGHT_OFFSET_ASSIGNMENT]      = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator | kCEETokenTypeAssignment;
    c_token_type_map[kCEETokenID_INCREMENT]                    = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_DECREMENT]                    = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_LOGIC_AND]                    = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_LOGIC_OR]                     = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_LOGIC_EQUAL]                  = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_LOGIC_UNEQUAL]                = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_LOGIC_LESS_EQUAL]             = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]            = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_MEMBER_POINTER]               = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_COMPARISON]                   = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_POINTER_MEMBER_POINTER]       = kCEETokenTypePunctuation | kCEETokenTypeOverloadOperator;
    c_token_type_map[kCEETokenID_TOKEN_PASTING]                = kCEETokenTypePunctuation;
    c_token_type_map[kCEETokenID_ELLIPSIS]                     = kCEETokenTypePunctuation;
    c_token_type_map[kCEETokenID_SCOPE]                        = kCEETokenTypePunctuation;
    c_token_type_map[kCEETokenID_AT_BRACE]                     = kCEETokenTypePunctuation;
    c_token_type_map[kCEETokenID_AT_BRACKET]                   = kCEETokenTypePunctuation;
    c_token_type_map[kCEETokenID_CARAT_BRACE]                  = kCEETokenTypePunctuation;
    c_token_type_map[kCEETokenID_HASH_INCLUDE]                 = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_IMPORT]                  = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_DEFINE]                  = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_UNDEF]                   = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_IF]                      = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    c_token_type_map[kCEETokenID_HASH_IFDEF]                   = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    c_token_type_map[kCEETokenID_HASH_IFNDEF]                  = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    c_token_type_map[kCEETokenID_HASH_ENDIF]                   = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    c_token_type_map[kCEETokenID_HASH_ELSE]                    = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    c_token_type_map[kCEETokenID_HASH_ELIF]                    = kCEETokenTypePrepDirective | kCEETokenTypePrepDirectiveCondition;
    c_token_type_map[kCEETokenID_HASH_LINE]                    = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_ERROR]                   = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_WARNING]                 = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_PRAGMA]                  = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_HASH_UNKNOW]                  = kCEETokenTypePrepDirective;
    c_token_type_map[kCEETokenID_LINE_COMMENT]                 = kCEETokenTypeComment;
    c_token_type_map[kCEETokenID_LINES_COMMENT]                = kCEETokenTypeComment;
    c_token_type_map[kCEETokenID_CONSTANT]                     = kCEETokenTypeConstant;
    c_token_type_map[kCEETokenID_CHARACTER]                    = kCEETokenTypeCharacter;
    c_token_type_map[kCEETokenID_LITERAL]                      = kCEETokenTypeLiteral;
    c_token_type_map[kCEETokenID_IDENTIFIER]                   = kCEETokenTypeIdentifier;
    c_token_type_map[kCEETokenID___ATTRIBUTE__]                = kCEETokenTypeKeyword;
}

static cee_boolean token_id_is_prep_directive(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypePrepDirective) != 0;
}

static cee_boolean token_id_is_prep_directive_condition(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypePrepDirectiveCondition) != 0;
}

static cee_boolean token_id_is_keyword(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeKeyword) != 0;
}

static cee_boolean token_id_is_punctuation(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypePunctuation) != 0;
}

static cee_boolean token_id_is_assignment(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeAssignment) != 0;
}

static cee_boolean token_id_is_builtin_type(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeBuildinType) != 0;
}

static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeDeclarationSpecifier) != 0;
}

static cee_boolean token_id_is_access_specifier(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeAccessSpecifier) != 0;
}

static cee_boolean token_id_is_alignas_specifier(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeAlignasSpecifier) != 0;
}

static cee_boolean token_id_is_object_specifier(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeObjectSpecifier) != 0;
}

static cee_boolean token_id_is_virtual_sepcifier(CEETokenID identifier) 
{
    return (c_token_type_map[identifier] & kCEETokenTypeVirtualSpecifier) != 0;
}
static cee_boolean token_id_is_overload_operator(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeOverloadOperator) != 0;
}

static cee_boolean is_name_scope(CEEList* p)
{
    CEEToken* token = NULL;
    CEEList* q = NULL;
    cee_ulong angle_bracket = 0;
    
    q = cee_token_not_whitespace_newline_before(p);
    if (q) {
        token = q->data;
        if (token->identifier == kCEETokenID_SCOPE)
            return TRUE;
    }
    
    q = cee_token_not_whitespace_newline_after(p);
    if (q) {
        token = q->data;
        
        if (token->identifier == '<') {
            angle_bracket ++;
            q = TOKEN_NEXT(q);
            
            while (q) {
                token = q->data;
                if (token->identifier == '<')
                    angle_bracket ++;
                else if (token->identifier == '>')
                    angle_bracket --;
                
                if (!angle_bracket)
                    break;
                
                q = TOKEN_NEXT(q);
            }
            
            q = cee_token_not_whitespace_newline_after(q);
            if (q) {
                token = q->data;
                if (token->identifier == kCEETokenID_SCOPE)
                    return TRUE;
            }
        }
        else if (token->identifier == kCEETokenID_SCOPE) {
            return TRUE;
        }
    }
    
    return FALSE;
}

/**
 *  comment
 */
static cee_boolean token_is_comment(CEEToken* token)
{
    return token->identifier == kCEETokenID_LINE_COMMENT ||
            token->identifier == kCEETokenID_LINES_COMMENT;
}

static cee_boolean comment_attach(CParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->comment_current,
                                          kCEESourceFregmentTypeComment,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "c");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

static cee_boolean comment_token_push(CParser* parser,
                                      CEEToken* push)
{
    if (!parser->comment_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->comment_current, push, TRUE);
    return TRUE;
}

static cee_boolean comment_fregment_reduce(CParser* parser)
{
    if (!parser->comment_current)
        return FALSE;
    
    comment_attach(parser);
    return TRUE;
}

/**
 *  preprocessor directive
 */
static cee_boolean prep_directive_parsing(CParser* parser)
{
    return parser->state & kCParserStatePrepDirectiveParsing;
}

static cee_boolean prep_directive_token_push(CParser* parser,
                                             CEEToken* push)
{
    if (!parser->prep_directive_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->prep_directive_current, push, TRUE);
    return TRUE;
}

static cee_boolean prep_directive_terminated(CParser* parser)
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

static cee_boolean prep_directive_attach(CParser* parser,
                                         CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->prep_directive_current,
                                          type,
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "c");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static cee_boolean prep_directive_sub_attach(CParser* parser,
                                             CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->prep_directive_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->prep_directive_current,
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              "c");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
}

static void prep_directive_branch_push(CParser* parser)
{
    prep_directive_sub_attach(parser, kCEESourceFregmentTypeSourceList);
    prep_directive_sub_attach(parser, kCEESourceFregmentTypePrepDirective);
}

static cee_boolean prep_directive_branch_pop(CParser* parser)
{
    if (prep_directive_pop(parser))
        return prep_directive_pop(parser);
    
    return FALSE;
}

static cee_boolean prep_directive_pop(CParser* parser)
{
    if (!parser->prep_directive_current ||
        !parser->prep_directive_current->parent)
        return FALSE;
    
    parser->prep_directive_current =
        parser->prep_directive_current->parent;
    return TRUE;
}

static void prep_directive_parse(CParser* parser)
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
                parser_state_set(parser, kCParserStateStatementParsing);
            else
                parser_state_clear(parser, kCParserStateStatementParsing);
            cee_source_fregment_type_set(parser->prep_directive_current,
                                         kCEESourceFregmentTypePrepDirectiveCondition);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ELIF ||
                 token->identifier == kCEETokenID_HASH_ELSE) {
            parser_state_clear(parser, kCParserStateStatementParsing);
            cee_source_fregment_type_set(parser->prep_directive_current,
                                         kCEESourceFregmentTypePrepDirectiveBranch);
            prep_directive_branch_push(parser);
        }
        else if (token->identifier == kCEETokenID_HASH_ENDIF) {
            prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
            if (should_statement_parsing(parser))
                parser_state_set(parser, kCParserStateStatementParsing);
            else
                parser_state_clear(parser, kCParserStateStatementParsing);
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

static cee_boolean should_statement_parsing(CParser* parser)
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

static void c_prep_directive_include_translate_table_init(void)
{
    /**
     *                      include             import              identifier  literal  \                  <           >       others
     *  Init                IncludeDirective    IncludeDirective    Error       Error    Error              Error       Error   Error
     *  IncludeDirective    Error               Error               Confirm     Confirm  IncludeDirective   *PathBegin  Error   Error
     *
     *  PathBegin, skip to Confirm
     */
    TRANSLATE_STATE_INI(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateMax         , kCPrepDirectiveIncludeTranslateStateError                                 );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateInit        , kCEETokenID_HASH_INCLUDE  , kCPrepDirectiveIncludeTranslateStateDirective );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateInit        , kCEETokenID_HASH_IMPORT   , kCPrepDirectiveIncludeTranslateStateDirective );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective   , kCEETokenID_IDENTIFIER    , kCPrepDirectiveIncludeTranslateStateConfirm   );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective   , kCEETokenID_LITERAL       , kCPrepDirectiveIncludeTranslateStateConfirm   );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective   , '\\'                      , kCPrepDirectiveIncludeTranslateStateDirective );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective   , '<'                       , kCPrepDirectiveIncludeTranslateStatePath      );
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
    
    CPrepDirectiveIncludeTranslateState current = kCPrepDirectiveIncludeTranslateStateInit;
    
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
        current = c_prep_directive_include_translate_table[current][token->identifier];
        
        if (current == kCPrepDirectiveIncludeTranslateStatePath) {
            r = p;
            p = skip_include_path(p);
            if (p) {
                t = p;
                q = p;
                current = kCPrepDirectiveIncludeTranslateStateConfirm;
            }
            break;
        }
        
        if (current == kCPrepDirectiveIncludeTranslateStateConfirm) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) ||
                cee_token_is_identifier(p, kCEETokenID_LITERAL))
                r = t = p;
            q = p;
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCPrepDirectiveIncludeTranslateStateConfirm && s && q) {
        include_directive = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                      fregment->subject_ref,
                                                                      s,
                                                                      q,
                                                                      kCEESourceSymbolTypePrepDirectiveInclude,
                                                                      "c",
                                                                      kCEETokenStringOptionIncompact);
        cee_source_symbol_name_format(include_directive->name);
        cee_token_slice_state_mark(TOKEN_NEXT(s), q, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, include_directive);
        
        if (r && t) {
            include_path = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     r,
                                                                     t,
                                                                     kCEESourceSymbolTypePrepDirectiveIncludePath,
                                                                     "c",
                                                                     kCEETokenStringOptionIncompact);
            cee_token_slice_state_mark(r, t, kCEETokenStateSymbolOccupied);
            fregment->symbols = cee_list_prepend(fregment->symbols, include_path);
        }
        
        ret = TRUE;
    }
    
    
#ifdef DEBUG_PREP_DIRECTIVE
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static void c_prep_directive_define_translate_table_init(void)
{
    /**
     *                      define              identifier      (               )                   ,               \               others
     *  Init                DefineDirective     Error           Error           Error               Error           Error           Error
     *  DefineDirective     Error               DefineName      Error           Error               Error           DefineDirective Error
     *  DefineName          Error               Confirm         ParameterList   Error               Error           DefineName      Confirm
     *  ParameterList       Error               Parameter       Error           Confirm             Error           ParameterList   Error
     *  Parameter           Error               Error           Error           Confirm             ParameterList   Parameter       Error
     */
    TRANSLATE_STATE_INI(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateMax              , kCPrepDirectiveDefineTranslateStateError                                                      );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateInit             , kCEETokenID_HASH_DEFINE                   , kCPrepDirectiveDefineTranslateStateDirective      );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDirective        , kCEETokenID_IDENTIFIER                    , kCPrepDirectiveDefineTranslateStateDefineName     );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDirective        , '\\'                                      , kCPrepDirectiveDefineTranslateStateDirective      );
    TRANSLATE_STATE_ANY(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDefineName       , kCPrepDirectiveDefineTranslateStateConfirm                                                    );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDefineName       , '('                                       , kCPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDefineName       , '\\'                                      , kCPrepDirectiveDefineTranslateStateDefineName     );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameterList    , kCEETokenID_IDENTIFIER                    , kCPrepDirectiveDefineTranslateStateParameter      );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameterList    , ')'                                       , kCPrepDirectiveDefineTranslateStateConfirm        );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameterList    , '\\'                                      , kCPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameter        , ')'                                       , kCPrepDirectiveDefineTranslateStateConfirm        );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameter        , ','                                       , kCPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameter        , '\\'                                      , kCPrepDirectiveDefineTranslateStateParameter      );
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
    
    CPrepDirectiveDefineTranslateState current = kCPrepDirectiveDefineTranslateStateInit;
    
    CEETokenID token_ids[] = {
        kCEETokenID_HASH_DEFINE,
        -1
    };
    
    p = cee_source_fregment_token_find(fregment, token_ids);
    if (!p)
        return FALSE;
    
    while (p) {
        token = p->data;
        current = c_prep_directive_define_translate_table[current][token->identifier];
        
        if (current == kCPrepDirectiveDefineTranslateStateDefineName) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                s = p;
        }
        
        if (current == kCPrepDirectiveDefineTranslateStateParameter) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(q, token);
        }
        
        if (current == kCPrepDirectiveDefineTranslateStateConfirm) {
            t = p;
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCPrepDirectiveDefineTranslateStateConfirm && s) {
        /** define macro */
        define_directive = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     s,
                                                                     s,
                                                                     kCEESourceSymbolTypePrepDirectiveDefine,
                                                                     "c",
                                                                     kCEETokenStringOptionIncompact);
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
                                                                  "c",
                                                                  kCEETokenStringOptionCompact);
            cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
            fregment->symbols = cee_list_prepend(fregment->symbols, parameter);
            p = TOKEN_NEXT(p);
        }
    }
    
    if (q)
        cee_list_free(q);
        
#ifdef DEBUG_PREP_DIRECTIVE
    if (fregment->symbols)
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
                                                                     "c",
                                                                     kCEETokenStringOptionIncompact);
        cee_token_slice_state_mark(p, q, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, common_directive);
        ret = TRUE;
    }
        
#ifdef DEBUG_PREP_DIRECTIVE
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

/**
 * statement
 */
static cee_boolean statement_attach(CParser* parser,
                                    CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->statement_current,
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "c");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_sub_attach(CParser* parser,
                                        CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_sub_attach(parser->statement_current, 
                                              type,
                                              parser->filepath_ref,
                                              parser->subject_ref,
                                              "c");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
    return TRUE;
}

static cee_boolean statement_pop(CParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}

static cee_boolean statement_parsing(CParser* parser)
{
    return parser->state & kCParserStateStatementParsing;
}

static void statement_parse(CParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    
    if (!current || !current->tokens_ref)
        return;
        
    if (objective_c_property_parse(current) ||
        objective_c_message_parse(current))
        return;
    
    if (c_declaration_parse(current))
        return;
    
    return;
}

static cee_boolean statement_reduce(CParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    if (cee_source_fregment_grandfather_type_is(parser->statement_current, 
                                                kCEESourceFregmentTypeTemplateDeclaration))
        if (!template_pop(parser))
            return FALSE;
    
    statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
}

/**
 * block
 */
static void block_push(CParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean block_pop(CParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void block_parse(CParser* parser)
{
    c_enumerators_parse(parser->statement_current);
}

/**
 * c template
 */
static void c_template_declaration_translate_table_init(void)
{
    /*
     *             template    <           >         ws/nl      others
     *  Init       Confirm     Init        Init      Init       Init
     *  Confirm    Error       Parameter   Error     Confirm    Error
     *  Parameter  Parameter   Parameter   Parameter Parameter  Parameter
     */
    TRANSLATE_STATE_INI(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateMax              , kCTemplateDeclarationTranslateStateError                                                              );
    TRANSLATE_STATE_ANY(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateInit             , kCTemplateDeclarationTranslateStateInit                                                               );
    TRANSLATE_STATE_SET(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateInit             , kCEETokenID_TEMPLATE                              , kCTemplateDeclarationTranslateStateConfirm        );
    TRANSLATE_STATE_SET(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateConfirm          , '<'                                               , kCTemplateDeclarationTranslateStateParameterList  );
    TRANSLATE_STATE_ANY(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateParameterList    , kCTemplateDeclarationTranslateStateParameterList                                                      );
}

static cee_boolean c_template_token_push(CParser* parser,
                                         CEEToken* push)
{
    CTemplateDeclarationTranslateState current = kCTemplateDeclarationTranslateStateInit;
    cee_int angle_brackets = 0;
    
    CEEToken* token = NULL;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(parser->statement_current);
    
    while (p) {
        token = p->data;
        current = c_template_declaration_translate_table[current][token->identifier];
        p = TOKEN_NEXT(p);
    }
    
    current = c_template_declaration_translate_table[current][push->identifier];
    if (current == kCTemplateDeclarationTranslateStateError) {
        statement_attach(parser, kCEESourceFregmentTypeStatement);
        return FALSE;
    }
    
    statement_token_push(parser, push);
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(parser->statement_current);
    while (p) {
        token = p->data;
        if (token->identifier == '<')
            angle_brackets ++;
        else if (token->identifier == '>')
            angle_brackets --;
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCTemplateDeclarationTranslateStateParameterList && !angle_brackets) {
        c_template_parameters_parse(parser->statement_current);
        template_push(parser);
    }
    
    return TRUE;
}

static cee_boolean c_template_parameters_parse(CEESourceFregment* fregment)
{
    CEESourceSymbol* template = NULL;
    CEEList* p = NULL;
    CEEList* begin = NULL;
    CEEList* end = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_TEMPLATE))
            begin = p;
        
        if (!TOKEN_NEXT(p))
            end = p;
        
        p = TOKEN_NEXT(p);
    }
    
    if (begin && end) {
        template = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                             fregment->subject_ref,
                                                             begin, 
                                                             end, 
                                                             kCEESourceSymbolTypeTemplateDeclaration,
                                                             "c",
                                                             kCEETokenStringOptionCompact);
        template->proto_descriptor = c_template_proto_descriptor_create(fregment, template);
        fregment->symbols = cee_list_prepend(fregment->symbols, template);
    }

#ifdef DEBUG_TEMPLATE
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return TRUE;
}

static cee_char* c_template_proto_descriptor_create(CEESourceFregment* fregment,
                                                    CEESourceSymbol* template)
{
    if (!fregment || !template)
        return NULL;
    
    cee_char* descriptor = NULL;
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"template\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static void template_push(CParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeSourceList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean template_pop(CParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void c_inheritance_definition_translate_table_init(void)
{
    /**
     *                                              object_specifier    declaration_specifier   builtin_type            virtual         alignas_specifier   access_specifier    attribute_specifier                         identifier              <                   >       ::                              (                                       )                       [                   ]                   :               ,               {
     *  Init                                        ObjectSpecifier     Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           Error                                   Error                   Error               Error               Error           Error           Error
     *  ObjectSpecifier                             ObjectSpecifier     Error                   Error                   Error           AlignasSpecifier    Error               ObjectAttributeSpecifier                    ObjectIdentifier        Error               Error   ObjectIdentifierScope           Error                                   Error                   ObjectSpecifier     ObjectSpecifier     Error           Error           Confirm
     *  ObjectAttributeSpecifier                    Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           ObjectAttributeSpecifierList            Error                   Error               Error               Error           Error           Error
     *  ObjectAttributeSpecifierList                Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           Error                                   ObjectSpecifier         Error               Error               Error           Error           Error
     *  ObjectIdentifier                            Error               Error                   Error                   Error           Error               Error               ObjectIdentifierAttributeSpecifier          ObjectIdentifier        *TemplateParameter  Error   ObjectIdentifierScope           Error                                   Error                   Error               Error               ObjectDerive    Error           Confirm
     *  ObjectIdentifierAttributeSpecifier          Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           ObjectIdentifierAttributeList           Error                   Error               Error               Error           Error           Error
     *  ObjectIdentifierAttributeList               Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           Error                                   ObjectIdentifier        Error               Error               Error           Error           Error
     *  ObjectIdentifierScope                       Error               Error                   Error                   Error           Error               Error               Error                                       ObjectIdentifier        Error               Error   Error                           Error                                   Error                   Error               Error               Error           Error           Error
     *  ObjectDerive                                Error               Error                   ObjectDeriveIdentifier  ObjectDerive    Error               ObjectDerive        Error                                       ObjectDeriveIdentifier  Error               Error   ObjectDeriveIdentifierScope     Error                                   Error                   ObjectDerive        ObjectDerive        Error           ObjectDerive    Error
     *  ObjectDeriveIdentifier                      Error               Error                   Error                   Error           Error               Error               ObjectDeriveIdentifierAttributeSpecifier    Error                   *TemplateParameter  Error   ObjectDeriveIdentifierScope     Error                                   Error                   Error               Error               Error           ObjectDerive    Confirm
     *  ObjectDeriveIdentifierAttributeSpecifier    Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           ObjectDeriveIdentifierAttributeList     Error                   Error               Error               Error           Error           Error
     *  ObjectDeriveIdentifierAttributeList         Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           Error                                   ObjectDeriveIdentifier  Error               Error               Error           Error           Error
     *  ObjectDeriveIdentifierScope                 Error               Error                   ObjectDeriveIdentifier  Error           Error               Error               Error                                       ObjectDeriveIdentifier  Error               Error   Error                           Error                                   Error                   Error               Error               Error           Error           Error
     *  AlignasSpecifier                            Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           AlignasList                             Error                   Error               Error               Error           Error           Error
     *  AlignasList                                 Error               Error                   Error                   Error           Error               Error               Error                                       Error                   Error               Error   Error                           Error                                   ObjectSpecifier         Error               Error               Error           Error           Error
     *  
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'  
     */
    TRANSLATE_STATE_INI(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateMax                                      , kCInheritanceDefinitionTranslateStateError                                                                        );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateInit                                     , token_id_is_object_specifier  , kCInheritanceDefinitionTranslateStateObjectSpecifier                              );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , kCEETokenID_IDENTIFIER        , kCInheritanceDefinitionTranslateStateObjectIdentifier                             );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , token_id_is_object_specifier  , kCInheritanceDefinitionTranslateStateObjectSpecifier                              );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , token_id_is_alignas_specifier , kCInheritanceDefinitionTranslateStateAlignasSpecifier                             );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , kCEETokenID_SCOPE             , kCInheritanceDefinitionTranslateStateObjectIdentifierScope                        );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , '['                           , kCInheritanceDefinitionTranslateStateObjectSpecifier                              );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , ']'                           , kCInheritanceDefinitionTranslateStateObjectSpecifier                              );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , '{'                           , kCInheritanceDefinitionTranslateStateConfirm                                      );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier                          , kCEETokenID___ATTRIBUTE__     , kCInheritanceDefinitionTranslateStateObjectAttributeSpecifier                     );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectAttributeSpecifier                 , '('                           , kCInheritanceDefinitionTranslateStateObjectAttributeSpecifierList                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectAttributeSpecifierList             , ')'                           , kCInheritanceDefinitionTranslateStateObjectSpecifier                              );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier                         , kCEETokenID_IDENTIFIER        , kCInheritanceDefinitionTranslateStateObjectIdentifier                             );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier                         , '<'                           , kCInheritanceDefinitionTranslateStateTemplateParameter                            );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier                         , kCEETokenID_SCOPE             , kCInheritanceDefinitionTranslateStateObjectIdentifierScope                        );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier                         , ':'                           , kCInheritanceDefinitionTranslateStateObjectDerive                                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier                         , '{'                           , kCInheritanceDefinitionTranslateStateConfirm                                      );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier                         , kCEETokenID___ATTRIBUTE__     , kCInheritanceDefinitionTranslateStateObjectIdentifierAttributeSpecifier           );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifierAttributeSpecifier       , '('                           , kCInheritanceDefinitionTranslateStateObjectIdentifierAttributeList                );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifierAttributeList            , ')'                           , kCInheritanceDefinitionTranslateStateObjectIdentifier                             );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifierScope                    , kCEETokenID_IDENTIFIER        , kCInheritanceDefinitionTranslateStateObjectIdentifier                             );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , kCEETokenID_VIRTUAL           , kCInheritanceDefinitionTranslateStateObjectDerive                                 );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , token_id_is_access_specifier  , kCInheritanceDefinitionTranslateStateObjectDerive                                 );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , token_id_is_builtin_type      , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , kCEETokenID_IDENTIFIER        , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , kCEETokenID_SCOPE             , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope                  );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , ','                           , kCInheritanceDefinitionTranslateStateObjectDerive                                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , '['                           , kCInheritanceDefinitionTranslateStateObjectDerive                                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                             , ']'                           , kCInheritanceDefinitionTranslateStateObjectDerive                                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                   , '<'                           , kCInheritanceDefinitionTranslateStateTemplateParameter                            );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                   , kCEETokenID_SCOPE             , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope                  );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                   , ','                           , kCInheritanceDefinitionTranslateStateObjectDerive                                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                   , '{'                           , kCInheritanceDefinitionTranslateStateConfirm                                      );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                   , kCEETokenID___ATTRIBUTE__     , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierAttributeSpecifier     );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierAttributeSpecifier , '('                           , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierAttributeList          );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierAttributeList      , ')'                           , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope              , kCEETokenID_IDENTIFIER        , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                       );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope              , token_id_is_builtin_type      , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier                       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateAlignasSpecifier                         , '('                           , kCInheritanceDefinitionTranslateStateAlignasList                                  );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateAlignasList                              , ')'                           , kCInheritanceDefinitionTranslateStateObjectSpecifier                              );
}

static cee_boolean c_inheritance_parse(CEEList* tokens,
                                       CEEList** inherited,
                                       CEEList** derives)
{
    CEEList* p = tokens;
    CInheritanceDefinitionTranslateState current = kCInheritanceDefinitionTranslateStateInit;
    CInheritanceDefinitionTranslateState prev = kCInheritanceDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* derive = NULL;
    
    while (p) {
        token = p->data;
        
        prev = current;
        current = c_inheritance_definition_translate_table[current][token->identifier];
        
        if (current == kCInheritanceDefinitionTranslateStateTemplateParameter) {
            p = skip_template_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        
        if (current == kCInheritanceDefinitionTranslateStateConfirm ||
            current == kCInheritanceDefinitionTranslateStateError) {
            if (derive) {
                *derives = cee_list_prepend(*derives, derive);
                derive = NULL;
            }
            break;
        }
        else if (current == kCInheritanceDefinitionTranslateStateObjectIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(*inherited, token);
        }
        else if (current == kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(derive, token);
        }
        else if (current == kCInheritanceDefinitionTranslateStateObjectDerive) {
            if (derive) {
                *derives = cee_list_prepend(*derives, derive);
                derive = NULL;
            }
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current != kCInheritanceDefinitionTranslateStateConfirm) {
        if (*inherited)
            cee_list_free(*inherited);
        
        if (*derives)
            cee_list_free_full(*derives, derived_chain_free);
        
        *inherited = NULL;
        *derives = NULL;
        
        return FALSE;
    }
        
    return TRUE;
}

CEESourceSymbol* c_inheritance_definition_create(CEESourceFregment* fregment,
                                                 CEEList* tokens,
                                                 const cee_char* filepath,
                                                 const cee_char* subject,
                                                 CEESourceSymbolType symbol_type)
{
    CEEList* name = NULL;
    CEEList* derives_chains = NULL;
    CEESourceSymbol* inheritance = NULL;
    CEEList* trap_keyword = tokens;
    
    if (!c_inheritance_parse(tokens, &name, &derives_chains))
        return NULL;
    
    if (name)
        inheritance = cee_source_symbol_create_from_token_slice(filepath,
                                                               subject, 
                                                               name, 
                                                               name, 
                                                               symbol_type,
                                                               "c",
                                                               kCEETokenStringOptionCompact);
    else
        inheritance = cee_source_symbol_create_from_token_slice(filepath,
                                                                subject,
                                                                trap_keyword,
                                                                trap_keyword,
                                                                symbol_type,
                                                                "c",
                                                                kCEETokenStringOptionCompact);
    if (inheritance) {
        /** base class names found */
        if (derives_chains)
            inheritance->derives = c_name_scope_list_string_create(derives_chains, subject);
        
        if (name)
            cee_token_slice_state_mark(name, name, kCEETokenStateSymbolOccupied);
        
        inheritance->proto_descriptor = c_inheritance_proto_descriptor_create(fregment,
                                                                              inheritance,
                                                                              inheritance->derives);
    }
    
    if (name)
        cee_list_free(name);
    
    if (derives_chains)
        cee_list_free_full(derives_chains, derived_chain_free);
    
    return inheritance;
}

static cee_char* c_inheritance_proto_descriptor_create(CEESourceFregment* fregment,
                                                       CEESourceSymbol* inheritance,
                                                       const cee_char* derives_str)
{
    if (!fregment || !inheritance)
        return NULL;
    
    cee_char* descriptor = NULL;
            
    cee_strconcat0(&descriptor, "{", NULL);
    
    if (inheritance->type == kCEESourceSymbolTypeClassDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"class\"", NULL);
    if (inheritance->type == kCEESourceSymbolTypeStructDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"struct\"", NULL);
    else if (inheritance->type == kCEESourceSymbolTypeEnumDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"enum\"", NULL);
    else if (inheritance->type == kCEESourceSymbolTypeUnionDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"union\"", NULL);
    else if (inheritance->type == kCEESourceSymbolTypeInterfaceDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"interface\"", NULL);
    else if (inheritance->type == kCEESourceSymbolTypeImplementationDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"implementation\"", NULL);
    else if (inheritance->type == kCEESourceSymbolTypeProtocolDeclaration)
        cee_strconcat0(&descriptor, "\"type\":", "\"protocol\"", NULL);
    else
        cee_strconcat0(&descriptor, "\"type\":", "\"class\"", NULL);
    
    if (inheritance->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", inheritance->name, "\"", NULL);
    
    if (derives_str)
        cee_strconcat0(&descriptor, ", \"derivers\":", "\"", derives_str, "\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static cee_char* c_name_scope_list_string_create(CEEList* scopes,
                                                 const cee_char* subject)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    cee_char* str = NULL;
    cee_char* scopes_str = NULL;
    
    p = cee_list_last(scopes);
    while (p) {
        q = p->data;
        str = c_name_scope_create(q, subject);
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

static cee_char* c_name_scope_create(CEEList* tokens,
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
                cee_strconcat0(&scope, "::", NULL);
            cee_free(str);
        }
        p = TOKEN_NEXT(p);
    }
    return scope;
}

static void derived_chain_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_list_free((CEEList*)data);
}

static cee_boolean c_class_definition_trap(CEESourceFregment* fregment, 
                                           CEEList** pp)
{
    CEESourceSymbol* definition = c_inheritance_definition_create(fregment,
                                                                  *pp,
                                                                  fregment->filepath_ref,
                                                                  fregment->subject_ref,
                                                                  kCEESourceSymbolTypeClassDefinition);
    if (!definition)
        return FALSE;
    
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeClassDefinition);
    *pp = NULL;
    
#ifdef DEBUG_CLASS
    if (definition)
        cee_source_symbol_print(definition);
#endif
    return TRUE;
}

static cee_boolean c_struct_definition_trap(CEESourceFregment* fregment,
                                            CEEList** pp)
{
    CEESourceSymbol* definition = c_inheritance_definition_create(fregment,
                                                                  *pp,
                                                                  fregment->filepath_ref,
                                                                  fregment->subject_ref,
                                                                  kCEESourceSymbolTypeStructDefinition);
    if (!definition)
        return FALSE;
    
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeStructDefinition);
    *pp = NULL;
    
#ifdef DEBUG_STRUCT
    if (definition)
        cee_source_symbol_print(definition);
#endif
    return TRUE;
}

static cee_boolean c_enum_definition_trap(CEESourceFregment* fregment, 
                                          CEEList** pp)
{
    CEESourceSymbol* definition = c_inheritance_definition_create(fregment,
                                                                  *pp,
                                                                  fregment->filepath_ref,
                                                                  fregment->subject_ref,
                                                                  kCEESourceSymbolTypeEnumDefinition);
    if (!definition)
        return FALSE;
    
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeEnumDefinition);
    *pp = NULL;
        
#ifdef DEBUG_ENUM
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
    return TRUE;
}

static cee_boolean c_enumerators_parse(CEESourceFregment* fregment)
{
    if (!cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeEnumDefinition))
        return FALSE;
    
    cee_boolean ret = FALSE;
    CEESourceFregment* grandfather = NULL;
    CEESourceSymbol* enum_symbol = NULL;
    CEEList* enum_symbols = NULL;
    CEESourceSymbol* enumerator = NULL;
    CEEList* enumerators = NULL;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    cee_boolean enumurating = TRUE;
    
    grandfather = cee_source_fregment_grandfather_get(fregment);
    enum_symbols = cee_source_fregment_symbols_search_by_type(grandfather,
                                                              kCEESourceSymbolTypeEnumDefinition);
    if (enum_symbols)
        enum_symbol = cee_list_nth_data(enum_symbols, 0);
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        if (token->identifier == kCEETokenID_IDENTIFIER && enumurating)
            q = p;
        
        if (token->identifier == ',' || token->identifier == ';' || !TOKEN_NEXT(p)) {
            if (q) {
                enumerator = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                       fregment->subject_ref,
                                                                       q,
                                                                       q,
                                                                       kCEESourceSymbolTypeEnumerator,
                                                                       "c",
                                                                       kCEETokenStringOptionCompact);
                if (enumerator) {
                    enumerator->proto_descriptor =
                        c_enumerator_proto_descriptor_create(token->fregment_ref,
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
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeEnumerators);
        fregment->symbols = cee_list_concat(fregment->symbols, enumerators);
        ret = TRUE;
    }
    
#ifdef DEBUG_ENUM
    if (enumerators)
        cee_source_symbols_print(enumerators);
#endif
    
exit:
    return ret;
}

static cee_char* c_enumerator_proto_descriptor_create(CEESourceFregment* fregment,
                                                      CEESourceSymbol* enumerator,
                                                      const cee_char* proto)
{
    if (!fregment || !enumerator)
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

static cee_boolean c_union_definition_trap(CEESourceFregment* fregment, 
                                           CEEList** pp)
{
    CEESourceSymbol* definition = c_inheritance_definition_create(fregment,
                                                                  *pp,
                                                                  fregment->filepath_ref,
                                                                  fregment->subject_ref,
                                                                  kCEESourceSymbolTypeUnionDefinition);
    if (!definition)
        return TRUE;
    
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeUnionDefinition);
    *pp = NULL;
        
#ifdef DEBUG_UNION
    if (definition)
        cee_source_symbol_print(definition);
#endif
    return FALSE;
}

static void objective_c_enum_definition_translate_table_init(void)
{
    /**
     *                      NS_EMUM     NS_OPTIONS      (               )                   {           others
     *  Init                Enum        OPtions         Error           Error               Error       Error
     *  Enum                Error       Error           ParameterList   Error               Error       Error
     *  OPtions             Error       Error           ParameterList   Error               Error       Error
     *  ParameterList       Error       Error           Error           ParameterListEnd    Error       Error
     *  ParameterListEnd    Error       Error           Error           Error               Confirm     Error
     *
     */
    TRANSLATE_STATE_INI(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateMax                 , kObjectiveCEnumDefinitionTranslateStateError                                          );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateInit                , kCEETokenID_NS_ENUM       , kObjectiveCEnumDefinitionTranslateStateEnum               );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateInit                , kCEETokenID_NS_OPTIONS    , kObjectiveCEnumDefinitionTranslateStateOPtions            );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateEnum                , '('                       , kObjectiveCEnumDefinitionTranslateStateParameterList      );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateOPtions             , '('                       , kObjectiveCEnumDefinitionTranslateStateParameterList      );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateParameterList       , ')'                       , kObjectiveCEnumDefinitionTranslateStateParameterListEnd   );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateParameterListEnd    , '{'                       , kObjectiveCEnumDefinitionTranslateStateConfirm            );
}

static cee_boolean objective_c_enum_definition_trap(CEESourceFregment* fregment,
                                                    CEEList** pp)
{
    cee_boolean ret = FALSE;
    CEESourceSymbol* definition = NULL;
    CEEList* p = *pp;
    CEEList* r = NULL;
    CEEToken* token = NULL;
    CEEList* parameter_list = NULL;
    CEESourceFregment* child = NULL;
    ObjectiveCEnumDefinitionTranslateState current = kObjectiveCEnumDefinitionTranslateStateInit;
    ObjectiveCEnumDefinitionTranslateState prev = kObjectiveCEnumDefinitionTranslateStateInit;
    
    while (p) {
        token = p->data;
        
        prev = current;
        current = objective_c_enum_definition_translate_table[current][token->identifier];
        
        if (current == kObjectiveCEnumDefinitionTranslateStateParameterList) {
            parameter_list = p;
        }
        else if (current == kObjectiveCEnumDefinitionTranslateStateConfirm) {
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kObjectiveCEnumDefinitionTranslateStateConfirm) {
        if (parameter_list) {
            child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
            if (child) {
                child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
                r = child->tokens_ref_last;
                while (r) {
                    token = r->data;
                    if (token->identifier == kCEETokenID_IDENTIFIER)
                        break;
                    r = TOKEN_PREV(r);
                }
            }
        }
        
        definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                               fregment->subject_ref,
                                                               r,
                                                               r,
                                                               kCEESourceSymbolTypeEnumDefinition,
                                                               "c",
                                                               kCEETokenStringOptionCompact);
        cee_token_slice_state_mark(r, r, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeEnumDefinition);
        ret = TRUE;
    }
    
    *pp = NULL;
    
#ifdef DEBUG_ENUM
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
    return TRUE;
}

static cee_boolean statement_block_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* q = NULL;
    
    if (cee_source_fregment_type_is(fregment, kCEESourceFregmentTypeAssignmentBlock) ||
        cee_source_fregment_type_is(fregment, kCEESourceFregmentTypeStatementBlock))
        return TRUE;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        if (token->identifier == '{' ||
            token->identifier == kCEETokenID_CARAT_BRACE) {
            q = cee_token_not_whitespace_newline_before(p);
            if (q) {
                token = q->data;
                if (token_id_is_assignment(token->identifier)) {
#ifdef DEBUG_STATEMENT_BLOCK
                    cee_source_fregment_string_print(fregment);
#endif
                    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeAssignmentBlock);
                }
                else {
#ifdef DEBUG_STATEMENT_BLOCK
                    cee_source_fregment_string_print(fregment);
#endif
                    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeStatementBlock);
                }
            }
            else {
#ifdef DEBUG_STATEMENT_BLOCK
                cee_source_fregment_string_print(fregment);
#endif
                cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeStatementBlock);
            }
        }
        p = TOKEN_NEXT(p);
    }
    
    return TRUE;
}

static void c_function_definition_translate_table_init(void)
{
    /**
     *                                      declaration_specifier   literal                 decltype_specifier      builtin_type    object_specifier    access_specifier    attribute_specifier                 overload_operator   new             delete          virtual_specifier   identifier          try     noexcept    throw       <                   >           ::                  *                       &                       &&                      ~                       (                               )                           [                       ]                       :               ,               operator            {           }               ->          others
     *  Init                                DeclarationSpecifier    Error                   Decltype                BuiltinType     ObjectSpecifier     AccessSpecifier     AttributeSpecifier                  Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     Error                   Error                   Error                   Init                    Error                           Error                       Init                    Init                    Error           Error           Error               Error       Error           Error       Error
     *  AttributeSpecifier                  Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   AttributeList                   Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  AttributeList                       Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           Init                        Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  DeclarationSpecifier                DeclarationSpecifier    DeclarationSpecifier    Decltype                BuiltinType     ObjectSpecifier     Error               Error                               Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    Error                           Error                       DeclarationSpecifier    DeclarationSpecifier    Error           Error           Operator            Error       Error           Error       Error
     *  DeclarationAttributeSpecifier       Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   DeclarationAttributeList        Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  DeclarationAttributeList            Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           DeclarationSpecifier        Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  Decltype                            Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   DecltypeList                    Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  DecltypeList                        Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           BuiltinType                 Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  BuiltinType                         BuiltinType             Error                   Decltype                BuiltinType     Error               Error               BuiltinTypeAttributeSpecifier       Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     BuiltinType             BuiltinType             BuiltinType             BuiltinType             Error                           Error                       BuiltinType             BuiltinType             Error           Error           Operator            Error       Error           Error       Error
     *  BuiltinTypeAttributeSpecifier       Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   BuiltinTypeAttributeList        Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  BuiltinTypeAttributeList            Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           BuiltinType                 Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  ObjectSpecifier                     Error                   Error                   Error                   Error           Error               Error               ObjectAttributeSpecifier            Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     Error                   Error                   Error                   Error                   Error                           Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  ObjectAttributeSpecifier            Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ObjectAttributeList             Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  ObjectAttributeList                 Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           ObjectSpecifier             Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  AccessSpecifier                     DeclarationSpecifier    Error                   Decltype                BuiltinType     ObjectSpecifier     AccessSpecifier     AccessAttributeSpecifier            Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     Error                   Error                   Error                   AccessSpecifier         Error                           Error                       AccessSpecifier         AccessSpecifier         AccessSpecifier Error           Error               Error       Error           Error       Error
     *  AccessAttributeSpecifier            Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   AccessAttributeList             Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  AccessAttributeList                 Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   AccessSpecifier                 Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  Identifier                          Identifier              Error                   Decltype                BuiltinType     ObjectSpecifier     AccessSpecifier     IdentifierAttributeSpecifier        Error               Error           Error           Error               Identifier          Error   Error       Error       *TemplateParameter  Error       IdentifierScope     Identifier              Identifier              Identifier              Identifier              ParameterList                   Error                       Identifier              Identifier              Error           Error           Operator            Error       Error           Error       Error
     *  IdentifierAttributeSpecifier        Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   IdentifierAttributeList         Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  IdentifierAttributeList             Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           Identifier                  Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  IdentifierScope                     Identifier              Error                   Error                   Identifier      Error               Error               Error                               Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   IdentifierScope         Error                           Error                       Error                   Error                   Error           Error           Operator            Error       Error           Error       Error
     *  ParameterList                       Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           ParameterListEnd            Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  ParameterListEnd                    ParameterListEnd        Error                   Error                   Error           Error               Error               ParameterListEndAttributeSpecifier  Error               Error           Error           ParameterListEnd    ParameterListEnd    Try     Noexcept    Throw       Error               Error       Error               Error                   Error                   ParameterListEnd        Error                   ParameterList                   Error                       ParameterListEnd        ParameterListEnd        Initializer     Error           Error               Confirm     Error           *Trailing   Error
     *  ParameterListEndAttributeSpecifier  Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterListEndAttributeList   Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  ParameterListEndAttributeList       Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           ParameterListEnd            Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  Noexcept                            Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           ParameterListEnd    ParameterListEnd    Try     Error       Throw       Error               Error       Error               Error                   Error                   ParameterListEnd        Error                   NoexceptList                    Error                       ParameterListEnd        ParameterListEnd        Initializer     Error           Error               Confirm     Error           *Trailing   Error
     *  NoexceptList                        Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           ParameterListEnd            Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  Throw                               Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           ParameterListEnd    ParameterListEnd    Try     Noexcept    Error       Error               Error       Error               Error                   Error                   ParameterListEnd        Error                   ThrowList                       Error                       ParameterListEnd        ParameterListEnd        Initializer     Error           Error               Confirm     Error           *Trailing   Error
     *  ThrowList                           Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           ParameterListEnd            Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  Initializer                         Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Member              Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           Error                       Error                   Error                   Error           Initializer     Error               Error       Error           Error       Error
     *  Member                              Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   MemberInit                      Error                       Error                   Error                   Error           Error           Error               MemberInit  Error           Error       Error
     *  MemberInit                          Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           MemberInitEnd               Error                   Error                   Error           Error           Error               Error       MemberInitEnd   Error       Error
     *  MemberInitEnd                       Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Try     Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           Error                       MemberInitEnd           MemberInitEnd           Error           Initializer     Error               Confirm     Error           Error       Error
     *  Try                                 Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           Error                       Error                   Error                   Error           Error           Error               Confirm     Error           Error       Error
     *  Operator                            Error                   Error                   Error                   OverloadType    Error               Error               Error                               OverloadOperator    OverloadNew     OverloadDelete  Error               OverloadType        Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   OverloadRoundBracket            Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadOperator                    Error                   Error                   Error                   Error           Error               Error               Error                               OverloadOperator    Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList                   Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadNew                         Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList                   Error                       OverloadNew             OverloadNew             Error           Error           Error               Error       Error           Error       Error
     *  OverloadDelete                      Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList                   Error                       OverloadDelete          OverloadDelete          Error           Error           Error               Error       Error           Error       Error
     *  OverloadType                        Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   OverloadTypeSurrounded          Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadTypeSurrounded              Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           OverloadTypeSurroundedEnd   Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadTypeSurroundedEnd           Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList                   Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadRoundBracket                Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                           OverloadRoundBracketEnd     Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadRoundBracketEnd             Error                   Error                   Error                   Error           Error               Error               Error                               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList                   Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *
     *  Trailing: skip to Confirm
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'  
     */
    
    
    
    TRANSLATE_STATE_INI(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMax                                , kCFunctionDefinitionTranslateStateError                                                                       );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , token_id_is_declaration_specifier     , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , kCEETokenID_DECLTYPE                  , kCFunctionDefinitionTranslateStateDecltype                            );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , token_id_is_builtin_type              , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , token_id_is_object_specifier          , kCFunctionDefinitionTranslateStateObjectSpecifier                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , token_id_is_access_specifier          , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , kCEETokenID_SCOPE                     , kCFunctionDefinitionTranslateStateIdentifierScope                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , '~'                                   , kCFunctionDefinitionTranslateStateInit                                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , '['                                   , kCFunctionDefinitionTranslateStateInit                                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , ']'                                   , kCFunctionDefinitionTranslateStateInit                                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                               , kCEETokenID___ATTRIBUTE__             , kCFunctionDefinitionTranslateStateAttributeSpecifier                  );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAttributeSpecifier                 , '('                                   , kCFunctionDefinitionTranslateStateAttributeList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAttributeList                      , ')'                                   , kCFunctionDefinitionTranslateStateInit                                );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , token_id_is_declaration_specifier     , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID_LITERAL                   , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID_DECLTYPE                  , kCFunctionDefinitionTranslateStateDecltype                            );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , token_id_is_builtin_type              , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , token_id_is_object_specifier          , kCFunctionDefinitionTranslateStateObjectSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID_SCOPE                     , kCFunctionDefinitionTranslateStateIdentifierScope                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , '*'                                   , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , '&'                                   , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID_LOGIC_AND                 , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , '~'                                   , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , '['                                   , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , ']'                                   , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID_OPERATOR                  , kCFunctionDefinitionTranslateStateOperator                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID_OPERATOR                  , kCFunctionDefinitionTranslateStateOperator                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier               , kCEETokenID___ATTRIBUTE__             , kCFunctionDefinitionTranslateStateDeclarationAttributeSpecifier       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationAttributeSpecifier      , '('                                   , kCFunctionDefinitionTranslateStateDeclarationAttributeList            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationAttributeList           , ')'                                   , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDecltype                           , '('                                   , kCFunctionDefinitionTranslateStateDecltypeList                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDecltypeList                       , ')'                                   , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , token_id_is_declaration_specifier     , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , kCEETokenID_DECLTYPE                  , kCFunctionDefinitionTranslateStateDecltype                            );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , token_id_is_builtin_type              , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , kCEETokenID_SCOPE                     , kCFunctionDefinitionTranslateStateIdentifierScope                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , '*'                                   , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , '&'                                   , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , kCEETokenID_LOGIC_AND                 , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , '['                                   , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , ']'                                   , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , '~'                                   , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , kCEETokenID_OPERATOR                  , kCFunctionDefinitionTranslateStateOperator                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                        , kCEETokenID___ATTRIBUTE__             , kCFunctionDefinitionTranslateStateBuiltinTypeAttributeSpecifier       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinTypeAttributeSpecifier      , '('                                   , kCFunctionDefinitionTranslateStateBuiltinTypeAttributeList            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinTypeAttributeList           , ')'                                   , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateObjectSpecifier                    , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateObjectSpecifier                    , kCEETokenID_SCOPE                     , kCFunctionDefinitionTranslateStateIdentifierScope                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateObjectSpecifier                    , kCEETokenID___ATTRIBUTE__             , kCFunctionDefinitionTranslateStateObjectAttributeSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateObjectAttributeSpecifier           , '('                                   , kCFunctionDefinitionTranslateStateObjectAttributeList                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateObjectAttributeList                , ')'                                   , kCFunctionDefinitionTranslateStateObjectSpecifier                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , token_id_is_declaration_specifier     , kCFunctionDefinitionTranslateStateDeclarationSpecifier                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , kCEETokenID_DECLTYPE                  , kCFunctionDefinitionTranslateStateDecltype                            );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , token_id_is_builtin_type              , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , token_id_is_object_specifier          , kCFunctionDefinitionTranslateStateObjectSpecifier                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , token_id_is_access_specifier          , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , kCEETokenID_SCOPE                     , kCFunctionDefinitionTranslateStateIdentifierScope                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , '~'                                   , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , '['                                   , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , ']'                                   , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , ':'                                   , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier                    , kCEETokenID___ATTRIBUTE__             , kCFunctionDefinitionTranslateStateAccessAttributeSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessAttributeSpecifier           , '('                                   , kCFunctionDefinitionTranslateStateAccessAttributeList                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessAttributeList                , ')'                                   , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , token_id_is_declaration_specifier     , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , kCEETokenID_DECLTYPE                  , kCFunctionDefinitionTranslateStateDecltype                            );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , token_id_is_builtin_type              , kCFunctionDefinitionTranslateStateBuiltinType                         );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , token_id_is_object_specifier          , kCFunctionDefinitionTranslateStateObjectSpecifier                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , token_id_is_access_specifier          , kCFunctionDefinitionTranslateStateAccessSpecifier                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , '<'                                   , kCFunctionDefinitionTranslateStateTemplateParameter                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , kCEETokenID_SCOPE                     , kCFunctionDefinitionTranslateStateIdentifierScope                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , '*'                                   , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , '&'                                   , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , kCEETokenID_LOGIC_AND                 , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , '('                                   , kCFunctionDefinitionTranslateStateParameterList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , '['                                   , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , ']'                                   , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , '~'                                   , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , kCEETokenID_OPERATOR                  , kCFunctionDefinitionTranslateStateOperator                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                         , kCEETokenID___ATTRIBUTE__             , kCFunctionDefinitionTranslateStateIdentifierAttributeSpecifier        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierAttributeSpecifier       , '('                                   , kCFunctionDefinitionTranslateStateIdentifierAttributeList             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierAttributeList            , ')'                                   , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope                    , token_id_is_declaration_specifier     , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope                    , token_id_is_builtin_type              , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope                    , '~'                                   , kCFunctionDefinitionTranslateStateIdentifierScope                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope                    , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateIdentifier                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope                    , kCEETokenID_OPERATOR                  , kCFunctionDefinitionTranslateStateOperator                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterList                      , ')'                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , token_id_is_declaration_specifier     , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , token_id_is_virtual_sepcifier         , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , kCEETokenID_TRY                       , kCFunctionDefinitionTranslateStateTry                                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , kCEETokenID_NOEXCEPT                  , kCFunctionDefinitionTranslateStateNoexcept                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , kCEETokenID_THROW                     , kCFunctionDefinitionTranslateStateThrow                               );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , kCEETokenID_LOGIC_AND                 , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , '('                                   , kCFunctionDefinitionTranslateStateParameterList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , '['                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , ']'                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , ':'                                   , kCFunctionDefinitionTranslateStateInitializer                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , '{'                                   , kCFunctionDefinitionTranslateStateConfirm                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , kCEETokenID_MEMBER_POINTER            , kCFunctionDefinitionTranslateStateTrailing                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd                   , kCEETokenID___ATTRIBUTE__             , kCFunctionDefinitionTranslateStateParameterListEndAttributeSpecifier  );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEndAttributeSpecifier , '('                                   , kCFunctionDefinitionTranslateStateParameterListEndAttributeList       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEndAttributeList      , ')'                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , token_id_is_virtual_sepcifier         , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , kCEETokenID_TRY                       , kCFunctionDefinitionTranslateStateTry                                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , kCEETokenID_THROW                     , kCFunctionDefinitionTranslateStateThrow                               );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , kCEETokenID_LOGIC_AND                 , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , '('                                   , kCFunctionDefinitionTranslateStateNoexceptList                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , '['                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , ']'                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , ':'                                   , kCFunctionDefinitionTranslateStateInitializer                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , kCEETokenID_MEMBER_POINTER            , kCFunctionDefinitionTranslateStateTrailing                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                           , '{'                                   , kCFunctionDefinitionTranslateStateConfirm                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexceptList                       , ')'                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , token_id_is_virtual_sepcifier         , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , kCEETokenID_TRY                       , kCFunctionDefinitionTranslateStateTry                                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , kCEETokenID_NOEXCEPT                  , kCFunctionDefinitionTranslateStateNoexcept                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , kCEETokenID_LOGIC_AND                 , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , '('                                   , kCFunctionDefinitionTranslateStateThrowList                           );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , '['                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , ']'                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , ':'                                   , kCFunctionDefinitionTranslateStateInitializer                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , kCEETokenID_MEMBER_POINTER            , kCFunctionDefinitionTranslateStateTrailing                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                              , '{'                                   , kCFunctionDefinitionTranslateStateConfirm                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrowList                          , ')'                                   , kCFunctionDefinitionTranslateStateParameterListEnd                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInitializer                        , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateMember                              );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInitializer                        , ','                                   , kCFunctionDefinitionTranslateStateInitializer                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMember                             , '('                                   , kCFunctionDefinitionTranslateStateMemberInit                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMember                             , '{'                                   , kCFunctionDefinitionTranslateStateMemberInit                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInit                         , ')'                                   , kCFunctionDefinitionTranslateStateMemberInitEnd                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInit                         , '}'                                   , kCFunctionDefinitionTranslateStateMemberInitEnd                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd                      , kCEETokenID_TRY                       , kCFunctionDefinitionTranslateStateTry                                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd                      , ','                                   , kCFunctionDefinitionTranslateStateInitializer                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd                      , '['                                   , kCFunctionDefinitionTranslateStateMemberInitEnd                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd                      , ']'                                   , kCFunctionDefinitionTranslateStateMemberInitEnd                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd                      , '{'                                   , kCFunctionDefinitionTranslateStateConfirm                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateTry                                , '{'                                   , kCFunctionDefinitionTranslateStateConfirm                             );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                           , token_id_is_builtin_type              , kCFunctionDefinitionTranslateStateOverloadType                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                           , token_id_is_overload_operator         , kCFunctionDefinitionTranslateStateOverloadOperator                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                           , kCEETokenID_IDENTIFIER                , kCFunctionDefinitionTranslateStateOverloadType                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                           , kCEETokenID_NEW                       , kCFunctionDefinitionTranslateStateOverloadNew                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                           , kCEETokenID_DELETE                    , kCFunctionDefinitionTranslateStateOverloadDelete                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                           , '('                                   , kCFunctionDefinitionTranslateStateOverloadRoundBracket                );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadOperator                   , token_id_is_overload_operator         , kCFunctionDefinitionTranslateStateOverloadOperator                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadOperator                   , '('                                   , kCFunctionDefinitionTranslateStateParameterList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadNew                        , '['                                   , kCFunctionDefinitionTranslateStateOverloadNew                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadNew                        , ']'                                   , kCFunctionDefinitionTranslateStateOverloadNew                         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadNew                        , '('                                   , kCFunctionDefinitionTranslateStateParameterList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadDelete                     , '['                                   , kCFunctionDefinitionTranslateStateOverloadDelete                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadDelete                     , ']'                                   , kCFunctionDefinitionTranslateStateOverloadDelete                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadDelete                     , '('                                   , kCFunctionDefinitionTranslateStateParameterList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadType                       , '('                                   , kCFunctionDefinitionTranslateStateOverloadTypeSurrounded              );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadTypeSurrounded             , ')'                                   , kCFunctionDefinitionTranslateStateOverloadTypeSurroundedEnd           );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadTypeSurroundedEnd          , '('                                   , kCFunctionDefinitionTranslateStateParameterList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadRoundBracket               , ')'                                   , kCFunctionDefinitionTranslateStateOverloadRoundBracketEnd             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadRoundBracketEnd            , '('                                   , kCFunctionDefinitionTranslateStateParameterList                       );
}

static cee_boolean c_function_definition_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEEList* trialing = NULL;
    CEESourceSymbol* definition = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* child = NULL;
    CFunctionDefinitionTranslateState current = kCFunctionDefinitionTranslateStateInit;
    CFunctionDefinitionTranslateState prev = kCFunctionDefinitionTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = c_function_definition_translate_table[current][token->identifier];
        
        if (current == kCFunctionDefinitionTranslateStateError) {
            return FALSE;
        }
        else if (current == kCFunctionDefinitionTranslateStateTemplateParameter) {
            p = skip_template_parameter_list(p, FALSE);
            if (!p) 
                return FALSE;
            current = prev;
        }
        else if (current == kCFunctionDefinitionTranslateStateTrailing) {
            trialing = p;
            break;
        }
        else if (current == kCFunctionDefinitionTranslateStateConfirm) {
            break;
        }
        else if (current == kCFunctionDefinitionTranslateStateOperator) {
            identifier = p;
        }
        else if (current == kCFunctionDefinitionTranslateStateParameterList) {
            if (!identifier) {
                q = cee_token_not_whitespace_newline_before(p);
                if (cee_token_is_identifier(q, kCEETokenID_IDENTIFIER))
                    identifier = q;
            }
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kCFunctionDefinitionTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCFunctionDefinitionTranslateStateConfirm ||
        current == kCFunctionDefinitionTranslateStateTrailing) {
        
        if (identifier) {
            /** destructor detected */
            r = identifier;
            q = cee_token_not_whitespace_newline_before(identifier);
            if (!cee_token_is_identifier(q, '~'))
                q = identifier;
        }
        
        definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                               fregment->subject_ref,
                                                               q,
                                                               r,
                                                               kCEESourceSymbolTypeFunctionDefinition,
                                                               "c",
                                                               kCEETokenStringOptionCompact);
        if (definition) {
            cee_token_slice_state_mark(q, r, kCEETokenStateSymbolOccupied);
            cee_source_fregment_type_set_exclusive(fregment, kCEESourceFregmentTypeFunctionDefinition);
            child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
            if (child) {
                child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
                c_function_parameters_parse(child);
            }
            
            if (q && c_function_definition_is_proto(fregment, q)) {
                definition->proto_descriptor = c_function_proto_descriptor_create(fregment,
                                                                                  SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                                                  TOKEN_PREV(q),
                                                                                  q,
                                                                                  parameter_list,
                                                                                  parameter_list_end);
            }
        }
    }
    else if (current == kCFunctionDefinitionTranslateStateMemberInit) {
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeAssignmentBlock);
    }
    
    if (definition)
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    
#ifdef DEBUG_FUNCTION_DEFINITION
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return TRUE;
}

static cee_boolean c_function_definition_is_proto(CEESourceFregment* fregment,
                                                  CEEList* identifier)
{
    CEEList* p = cee_token_not_whitespace_newline_before(identifier);
    return !cee_token_is_identifier(p, kCEETokenID_SCOPE);
}

static cee_boolean should_proto_descriptor_append_whitespace(CEEToken* token,
                                                             CEEToken* token_prev)
{
    return (token_prev &&
            ((!token_id_is_punctuation(token_prev->identifier) &&
              !token_id_is_punctuation(token->identifier)) ||
             ((token_prev->identifier == '*' || token_prev->identifier == '&')
              && !token_id_is_punctuation(token->identifier))));
}

static cee_boolean token_id_is_c_function_proto_header_ignored(CEETokenID identifier)
{
    return (c_token_type_map[identifier] & kCEETokenTypeAccessSpecifier) != 0;
}

static void c_function_parameters_declaration_translate_table_init(void)
{
    /**
     *                          declaration_specifier   decltype_specifier  builtin_type    object_specifier    assign_operator     identifier          <                   >       ::                  ...         *                       &                       &&                      (               )                   [                       ]                       ,           others
     *  Init                    DeclarationSpecifier    Decltype            BuiltinType     ObjectSpecifier     Error               CustomType          Error               Error   CustomTypeScope     Ellipsis    Error                   Error                   Error                   Error           Error               Init                    Init                    Error       Error
     *  DeclarationSpecifier    DeclarationSpecifier    Decltype            BuiltinType     ObjectSpecifier     Error               CustomType          Error               Error   CustomTypeScope     Error       DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    Error           Error               DeclarationSpecifier    DeclarationSpecifier    Error       Error
     *  BuiltinType             BuiltinType             Decltype            BuiltinType     Error               Error               Identifier          Error               Error   IdentifierScope     Error       BuiltinType             BuiltinType             BuiltinType             ParameterList   Error               BuiltinType             BuiltinType             Error       Error
     *  Decltype                Error                   Error               Error           Error               Error               Error               Error               Error   Error               Error       Error                   Error                   Error                   DecltypeList    Error               Error                   Error                   Error       Error
     *  DecltypeList            Error                   Error               Error           Error               Error               Error               Error               Error   Error               Error       Error                   Error                   Error                   Error           BuiltinType         Error                   Error                   Error       Error
     *  ObjectSpecifier         Error                   Error               Error           Error               Error               CustomType          Error               Error   CustomTypeScope     Error       Error                   Error                   Error                   Error           Error               Error                   Error                   Error       Error
     *  CustomType              CustomType              Decltype            BuiltinType     ObjectSpecifier     Error               Identifier          *TemplateParameter  Error   CustomTypeScope     Error       CustomType              CustomType              CustomType              ParameterList   Error               CustomType              CustomType              Error       Error
     *  CustomTypeScope         CustomType              Error               CustomType      Error               Error               CustomType          Error               Error   Error               Error       Error                   Error                   Error                   Error           Error               Error                   Error                   Error       Error
     *  Identifier              DeclarationSpecifier    Decltype            BuiltinType     ObjectSpecifier     Confirm             Identifier          *TemplateParameter  Error   IdentifierScope     Error       Identifier              Identifier              Identifier              ParameterList   Error               Identifier              Identifier              Confirm     Error
     *  IdentifierScope         Identifier              Error               Identifier      Error               Error               Identifier          Error               Error   Error               Error       Error                   Error                   Error                   Error           Error               Error                   Error                   Error       Error
     *  ParameterList           Error                   Error               Error           Error               Error               Error               Error               Error   Error               Error       Error                   Error                   Error                   Errror          ParameterListEnd    Error                   Error                   Error       Error
     *  ParameterListEnd        Error                   Error               Error           Error               Confirm             Error               Error               Error   Error               Error       Error                   Error                   Error                   Surrounded      Error               Surrounded              Error                   Confirm     Error
     *  Surrounded              Error                   Error               Error           Error               Error               Error               Error               Error   Error               Error       Error                   Error                   Error                   Error           SurroundedEnd       Error                   SurroundedEnd           Error       Error
     *  SurroundedEnd           Error                   Error               Error           Error               Confirm             Error               Error               Error   Error               Error       Error                   Error                   Error                   SurroundedEnd   SurroundedEnd       SurroundedEnd           SurroundedEnd           Confirm     Error
     *  Ellipsis                Error                   Error               Error           Error               Confirm             Error               Error               Error   Error               Error       Error                   Error                   Error                   Error           Error               Error                   Error                   Confirm     Error
     */
    TRANSLATE_STATE_INI(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateMax                     , kCFunctionParametersDeclarationTranslateStateError                                                    );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , token_id_is_declaration_specifier , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , kCEETokenID_DECLTYPE              , kCFunctionParametersDeclarationTranslateStateDecltype             );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , token_id_is_builtin_type          , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , token_id_is_object_specifier      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , kCEETokenID_SCOPE                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , kCEETokenID_ELLIPSIS              , kCFunctionParametersDeclarationTranslateStateEllipsis             );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , '['                               , kCFunctionParametersDeclarationTranslateStateInit                 );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                    , ']'                               , kCFunctionParametersDeclarationTranslateStateInit                 );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , token_id_is_declaration_specifier , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , kCEETokenID_DECLTYPE              , kCFunctionParametersDeclarationTranslateStateDecltype             );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , token_id_is_builtin_type          , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , token_id_is_object_specifier      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , kCEETokenID_SCOPE                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , '*'                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , '&'                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , kCEETokenID_LOGIC_AND             , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , '['                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier    , ']'                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , token_id_is_declaration_specifier , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , kCEETokenID_DECLTYPE              , kCFunctionParametersDeclarationTranslateStateDecltype             );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , token_id_is_builtin_type          , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , kCEETokenID_SCOPE                 , kCFunctionParametersDeclarationTranslateStateIdentifierScope      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , '*'                               , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , '&'                               , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , kCEETokenID_LOGIC_AND             , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , '['                               , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , ']'                               , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , ','                               , kCFunctionParametersDeclarationTranslateStateConfirm              );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType             , '('                               , kCFunctionParametersDeclarationTranslateStateParameterList        );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDecltype                , '('                               , kCFunctionParametersDeclarationTranslateStateDecltypeList         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDecltypeList            , ')'                               , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateObjectSpecifier         , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateObjectSpecifier         , kCEETokenID_SCOPE                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope      );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , token_id_is_declaration_specifier , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , kCEETokenID_DECLTYPE              , kCFunctionParametersDeclarationTranslateStateDecltype             );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , token_id_is_builtin_type          , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , token_id_is_object_specifier      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , '<'                               , kCFunctionParametersDeclarationTranslateStateTemplateParameter    );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , kCEETokenID_SCOPE                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , '*'                               , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , '&'                               , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , kCEETokenID_LOGIC_AND             , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , '['                               , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , ']'                               , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType              , '('                               , kCFunctionParametersDeclarationTranslateStateParameterList        );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomTypeScope         , token_id_is_declaration_specifier , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomTypeScope         , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomTypeScope         , token_id_is_builtin_type          , kCFunctionParametersDeclarationTranslateStateCustomType           );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , token_id_is_declaration_specifier , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , kCEETokenID_DECLTYPE              , kCFunctionParametersDeclarationTranslateStateDecltype             );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , token_id_is_builtin_type          , kCFunctionParametersDeclarationTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , token_id_is_object_specifier      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier      );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , token_id_is_assignment            , kCFunctionParametersDeclarationTranslateStateConfirm              );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , '<'                               , kCFunctionParametersDeclarationTranslateStateTemplateParameter    );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , kCEETokenID_SCOPE                 , kCFunctionParametersDeclarationTranslateStateIdentifierScope      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , '*'                               , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , '&'                               , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , kCEETokenID_LOGIC_AND             , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , '['                               , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , ']'                               , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , ','                               , kCFunctionParametersDeclarationTranslateStateConfirm              );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier              , '('                               , kCFunctionParametersDeclarationTranslateStateParameterList        );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifierScope         , token_id_is_builtin_type          , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifierScope         , token_id_is_declaration_specifier , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifierScope         , kCEETokenID_IDENTIFIER            , kCFunctionParametersDeclarationTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterList           , ')'                               , kCFunctionParametersDeclarationTranslateStateParameterListEnd     );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterListEnd        , '('                               , kCFunctionParametersDeclarationTranslateStateSurrounded           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterListEnd        , '['                               , kCFunctionParametersDeclarationTranslateStateSurrounded           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterListEnd        , ','                               , kCFunctionParametersDeclarationTranslateStateConfirm              );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurrounded              , ')'                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd        );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurrounded              , ']'                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd        );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd           , '('                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd        );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd           , ')'                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd        );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd           , '['                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd        );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd           , ']'                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd        );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd           , ','                               , kCFunctionParametersDeclarationTranslateStateConfirm              );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateEllipsis                , ','                               , kCFunctionParametersDeclarationTranslateStateConfirm              );
}

static cee_boolean c_function_parameters_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* prefix = NULL;
    CEEToken* token = NULL;
    CFunctionParametersDeclarationState current = kCFunctionParametersDeclarationTranslateStateInit;
    CFunctionParametersDeclarationState prev = kCFunctionParametersDeclarationTranslateStateInit;
    CEESourceSymbol* declaration = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEEList* surrounded = NULL;
    CEEList* surrounded_end = NULL;
    CEEList* ellipsis = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    prefix = p;
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = c_function_parameters_declaration_translate_table[current][token->identifier];
        
        if (current == kCFunctionParametersDeclarationTranslateStateError) {
            return FALSE;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateTemplateParameter) {
            p = skip_template_parameter_list(p, FALSE);
            if (!p) 
                return FALSE;
            current = prev;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateParameterList) {
            if (!parameter_list)
                parameter_list = p;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
            
        if (current == kCFunctionParametersDeclarationTranslateStateSurrounded) {
            if (!surrounded)
                surrounded = p;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateSurroundedEnd) {
            if (!surrounded_end)
                surrounded_end = p;
        }
                
        if (current == kCFunctionParametersDeclarationTranslateStateEllipsis) {
            if (cee_token_is_identifier(p, kCEETokenID_ELLIPSIS))
                ellipsis = p;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateConfirm || !TOKEN_NEXT(p)) {
            if (surrounded) {
                declaration = c_function_parameter_surrounded_create(fregment,
                                                                     prefix,
                                                                     parameter_list);
            }
            else if (parameter_list) {
                q = cee_token_not_whitespace_newline_before(parameter_list);
                if (cee_token_is_identifier(q, kCEETokenID_IDENTIFIER))
                    declaration = c_function_parameter_identifier_create(fregment,
                                                                         prefix,
                                                                         q);
            }
            else if (identifier) {
                declaration = c_function_parameter_identifier_create(fregment,
                                                                     prefix,
                                                                     identifier);
            }
            else if (ellipsis) {
                /** ignore ellipsis */
            }
            
            if (declaration)
                fregment->symbols = cee_list_prepend(fregment->symbols, declaration);
            
            declaration = NULL;
            identifier = NULL;
            parameter_list = NULL;
            parameter_list_end = NULL;
            surrounded = NULL;
            surrounded_end = NULL;
            ellipsis = NULL;
            
            if (!cee_token_is_identifier(p, ',')) {
                p = skip_c_declaration_interval(p);
                if (!p)
                    break;
            }
            
            prefix = TOKEN_NEXT(p);
            current = kCFunctionParametersDeclarationTranslateStateInit;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (fregment->symbols)
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
    
#ifdef DEBUG_FUNCTION_DEFINITION
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif

    return TRUE;
}

static CEESourceSymbol* c_function_parameter_identifier_create(CEESourceFregment* fregment,
                                                               CEEList* prefix,
                                                               CEEList* identifier)
{
    CEESourceSymbol* parameter = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                           fregment->subject_ref,
                                                                           identifier,
                                                                           identifier,
                                                                           kCEESourceSymbolTypeFunctionParameter,
                                                                           "c",
                                                                           kCEETokenStringOptionCompact);
    if (!parameter)
        return NULL;
    
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    if (prefix && TOKEN_PREV(identifier))
        parameter->proto_descriptor = c_function_parameter_proto_descriptor_create(fregment,
                                                                                   prefix,
                                                                                   TOKEN_PREV(identifier),
                                                                                   parameter);
    return parameter;
}

static CEESourceSymbol* c_function_parameter_surrounded_create(CEESourceFregment* fregment,
                                                               CEEList* prefix,
                                                               CEEList* surrounded)
{
    CEESourceSymbol* parameter = NULL;
    CEESourceFregment* child = NULL;
    CEEList* tokens = NULL;
    CEEList* p = NULL;
    
    child = cee_source_fregment_child_index_by_leaf(fregment, surrounded->data);
    if (!child)
        return NULL;
    
    child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
    if (!child)
        return NULL;
    
    tokens = cee_source_fregment_tokens_expand(child);
    if (!tokens)
        return NULL;
    
    p = TOKEN_FIRST(tokens);
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
            break;
        p = TOKEN_NEXT(p);
    }
    
    if (p) {
        parameter = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                              fregment->subject_ref,
                                                              p, 
                                                              p,
                                                              kCEESourceSymbolTypeFunctionParameter,
                                                              "c",
                                                              kCEETokenStringOptionCompact);
        if (!parameter)
            return NULL;
        
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        if (prefix && TOKEN_PREV(surrounded))
            parameter->proto_descriptor = c_function_parameter_proto_descriptor_create(fregment,
                                                                                       prefix,
                                                                                       TOKEN_PREV(surrounded),
                                                                                       parameter);
    }
    cee_list_free(tokens);
    
    return parameter;
}

static cee_char* c_function_parameter_proto_descriptor_create(CEESourceFregment* fregment,
                                                              CEEList* prefix,
                                                              CEEList* prefix_tail,
                                                              CEESourceSymbol* parameter)
{
    if (!fregment || !parameter)
        return NULL;
    
    cee_char* descriptor = NULL;
    cee_char* proto_str = NULL;
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"declaration\"", NULL);
    
    if (parameter->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", parameter->name, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto_str = c_type_descriptor_from_token_slice(fregment, prefix, prefix_tail);
    if (proto_str) {
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto_str, "\"", NULL);
        cee_free(proto_str);
    }
        
    cee_strconcat0(&descriptor, "}", NULL);
        
    return descriptor;
}

static void objective_c_message_translate_table_init(void)
{
    /**
     *                       -           +          identifier  (               )                   :           {           ;               others
     *  Init                 Message     Message    Error       Error           Error               Error       Error       Error           Error
     *  Message              Error       Error      Error       ReturnType      Error               Error       Error       Error           Error
     *  ReturnType           Error       Error      Error       Error           ReturnTypeEnd       Error       Error       Error           Error
     *  ReturnTypeEnd        Error       Error      Component   Error           Error               Error       Error       Error           Error
     *  Component            Error       Error      Error       Error           Error               Colon       Definition  Declaration     Error
     *  Colon                Error       Error      Error       ParameterType   Error               Error       Error       Error           Error
     *  ParameterType        Error       Error      Error       Error           ParameterTypeEnd    Error       Error       Error           Error
     *  ParameterTypeEnd     Error       Error      Parameter   Error           Error               Error       Error       Error           Error
     *  Parameter            Error       Error      Component   Error           Error               Error       Definition  Declaration     Error
     */
    TRANSLATE_STATE_INI(objective_c_message_translate_table, kObjectiveCMessageTranslateStateMax               , kObjectiveCMessageTranslateStateError                                          );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateInit              , '-'                       , kObjectiveCMessageTranslateStateMessage            );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateInit              , '+'                       , kObjectiveCMessageTranslateStateMessage            );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateMessage           , '('                       , kObjectiveCMessageTranslateStateReturnType         );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateReturnType        , ')'                       , kObjectiveCMessageTranslateStateReturnTypeEnd      );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateReturnTypeEnd     , kCEETokenID_IDENTIFIER    , kObjectiveCMessageTranslateStateComponent          );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateComponent         , ':'                       , kObjectiveCMessageTranslateStateColon              );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateComponent         , '{'                       , kObjectiveCMessageTranslateStateDefinition         );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateComponent         , ';'                       , kObjectiveCMessageTranslateStateDeclaration        );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateColon             , '('                       , kObjectiveCMessageTranslateStateParameterType      );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateParameterType     , ')'                       , kObjectiveCMessageTranslateStateParameterTypeEnd   );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateParameterTypeEnd  , kCEETokenID_IDENTIFIER    , kObjectiveCMessageTranslateStateParameter          );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateParameter         , kCEETokenID_IDENTIFIER    , kObjectiveCMessageTranslateStateComponent          );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateParameter         , '{'                       , kObjectiveCMessageTranslateStateDefinition         );
    TRANSLATE_STATE_SET(objective_c_message_translate_table, kObjectiveCMessageTranslateStateParameter         , ';'                       , kObjectiveCMessageTranslateStateDeclaration        );
}

static cee_boolean objective_c_message_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* message_begin = NULL;
    CEEList* name_chain = NULL;
    CEEList* parameters = NULL;
    CEEList* return_type = NULL;
    CEEList* return_type_end = NULL;
    CEEList* parameter_types = NULL;
    CEEList* q = NULL;
    cee_int i = 0;
    CEESourceSymbolType symbol_type = kCEESourceSymbolTypeUnknow;
    CEESourceFregmentType fregment_type = kCEESourceFregmentTypeStatement;
    ObjectiveCMessageTranslateState current = kObjectiveCMessageTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        current = objective_c_message_translate_table[current][token->identifier];
        
        if (current == kObjectiveCMessageTranslateStateError)
            break;
        else if (current == kObjectiveCMessageTranslateStateMessage)
                TOKEN_APPEND(message_begin, token);
        else if (current == kObjectiveCMessageTranslateStateReturnType)
                TOKEN_APPEND(return_type, token);
        else if (current == kObjectiveCMessageTranslateStateReturnTypeEnd)
                TOKEN_APPEND(return_type_end, token);
        else if (current == kObjectiveCMessageTranslateStateParameterType)
                TOKEN_APPEND(parameter_types, token);
        else if (current == kObjectiveCMessageTranslateStateComponent)
                TOKEN_APPEND(name_chain, token);
        else if (current == kObjectiveCMessageTranslateStateColon)
                TOKEN_APPEND(name_chain, token);
        else if (current == kObjectiveCMessageTranslateStateParameter)
                TOKEN_APPEND(parameters, token);
        else if (current == kObjectiveCMessageTranslateStateDefinition ||
                 current == kObjectiveCMessageTranslateStateDeclaration)
            break;
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current == kObjectiveCMessageTranslateStateError)
        goto exit;
    
    if (current == kObjectiveCMessageTranslateStateDefinition) {
        symbol_type = kCEESourceSymbolTypeMessageDefinition;
        fregment_type = kCEESourceFregmentTypeFunctionDefinition;
    }
    else if (current == kObjectiveCMessageTranslateStateDeclaration) {
        symbol_type = kCEESourceSymbolTypeMessageDeclaration;
        fregment_type = kCEESourceFregmentTypeDeclaration;
    }
    
    if (name_chain) {
        symbol = cee_source_symbol_create_from_tokens(fregment->filepath_ref,
                                                      fregment->subject_ref,
                                                      name_chain,
                                                      symbol_type,
                                                      "c",
                                                      kCEETokenStringOptionCompact);
        if (symbol) {
            if (symbol_type == kCEESourceSymbolTypeMessageDeclaration)
                symbol->proto_descriptor = objective_c_message_proto_descriptor_create(fregment,
                                                                                       message_begin,
                                                                                       return_type,
                                                                                       return_type_end,
                                                                                       name_chain,
                                                                                       parameter_types,
                                                                                       parameters);
            symbols = cee_list_prepend(symbols, symbol);
        }
        cee_tokens_state_mark(TOKEN_FIRST(name_chain), kCEETokenStateSymbolOccupied);
    }
                
    if (parameters) {
        q = TOKEN_FIRST(parameters);
        i = 0;
        while (q) {
            symbol = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                               fregment->subject_ref,
                                                               q,
                                                               q,
                                                               kCEESourceSymbolTypeMessageParameter,
                                                               "c",
                                                               kCEETokenStringOptionCompact);
            if (symbol) {
                symbol->proto_descriptor = objective_c_message_parameter_proto_descriptor_create(fregment,
                                                                                                 i,
                                                                                                 symbol);
                symbols = cee_list_prepend(symbols, symbol);
            }
            q = TOKEN_NEXT(q);
            i ++;
        }
        cee_tokens_state_mark(TOKEN_FIRST(parameters), kCEETokenStateSymbolOccupied);
    }
    
    if (symbols) {
        fregment->symbols = cee_list_concat(symbols, fregment->symbols);
        cee_source_fregment_type_set(fregment, fregment_type);
        ret = TRUE;
    }
    
exit:
    if (parameters)
        cee_list_free(parameters);
    
    if (name_chain)
        cee_list_free(name_chain);
    
    if (parameter_types)
        cee_list_free(parameter_types);
    
    if (return_type_end)
        cee_list_free(return_type_end);
    
    if (return_type)
        cee_list_free(return_type);
    
    if (message_begin)
        cee_list_free(message_begin);
    
#ifdef DEBUG_MESSAGE
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif   
    return ret;
}

static cee_char* objective_c_message_proto_descriptor_create(CEESourceFregment* fregment,
                                                             CEEList* message_begin,
                                                             CEEList* return_type,
                                                             CEEList* return_type_end,
                                                             CEEList* name_chain,
                                                             CEEList* parameter_types,
                                                             CEEList* parameters)
{
    if (!fregment ||
        !message_begin ||
        !name_chain ||
        !return_type ||
        !return_type_end ||
        !parameter_types ||
        !parameters)
        return NULL;
    
    const cee_char* subject = fregment->subject_ref;
    CEEList* tokens = cee_source_fregment_tokens_expand(fregment);
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    CEEList* s = NULL;
    CEEToken* token = NULL;
    CEEToken* token_prev = NULL;
    cee_char* descriptor = NULL;
    cee_char* formater_str = NULL;
    
    cee_boolean found_header = FALSE;
    cee_boolean found_parameter = FALSE;
    CEEList* header = NULL;
    cee_size l = 0;
    CEESourceFregment* grandfather = NULL;
    const cee_char* access_level = NULL;
    
    grandfather = cee_source_fregment_grandfather_get(fregment);
    if (grandfather)
        access_level = cee_source_fregment_attribute_get(grandfather, "access_level");
    
    p = TOKEN_FIRST(tokens);
    
    /** header */
    while (p) {
        if (p->data == message_begin->data)
            found_header = TRUE;
        
        if (found_header)
            TOKEN_APPEND(header, p->data);
        
        if (p->data == return_type_end->data)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"message\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    /** create header descriptor */
    token_prev = NULL;
    q = TOKEN_FIRST(header);
    while (q) {
        token = q->data;
        
        if (!cee_token_id_is_newline(token->identifier) &&
            !cee_token_id_is_whitespace(token->identifier)) {

            if (should_proto_descriptor_append_whitespace(token, token_prev))
                cee_strconcat0(&formater_str, " ", NULL);
            cee_string_concat_with_token(&formater_str, subject, token);
            token_prev = token;
        }
        
        q = TOKEN_NEXT(q);
    }
    if (formater_str) {
        cee_strconcat0(&descriptor, ", \"header\":", "\"", formater_str, "\"", NULL);
        cee_free(formater_str);
        formater_str = NULL;
    }
    
    formater_str = cee_string_from_tokens(subject,
                                          TOKEN_FIRST(name_chain),
                                          kCEETokenStringOptionCompact);
    if (formater_str) {
        cee_strconcat0(&descriptor, ", \"name\":", "\"", formater_str, "\"", NULL);
        cee_free(formater_str);
        formater_str = NULL;
    }
    
    token_prev = NULL;
    cee_strconcat0(&descriptor, ", \"parameters\":[", NULL);
    r = TOKEN_FIRST(parameter_types);
    s = TOKEN_FIRST(parameters);
    while (r && s) {
        
        while (p) {
            token = p->data;
            
            if (p->data == r->data)
                found_parameter = TRUE;
            
            if (found_parameter) {
                if (!cee_token_id_is_newline(token->identifier) &&
                    !cee_token_id_is_whitespace(token->identifier)) {
                    
                    if (should_proto_descriptor_append_whitespace(token, token_prev))
                        cee_strconcat0(&formater_str, " ", NULL);
                    cee_string_concat_with_token(&formater_str, subject, token);
                    token_prev = token;
                }
            }
            
            if (p->data == s->data) {
                cee_strconcat0(&descriptor, "\"", formater_str, "\"", ",", NULL);
                cee_free(formater_str);
                formater_str = NULL;
                token_prev = NULL;
                break;
            }
            
            p = TOKEN_NEXT(p);
        }
        
        found_parameter = FALSE;
        r = TOKEN_NEXT(r);
        s = TOKEN_NEXT(s);
    }
    l = strlen(descriptor);
    if (descriptor[l-1]== ',')
        descriptor[l-1] = '\0';
    cee_strconcat0(&descriptor, "]", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
exit:
        
    if (header)
        cee_list_free(header);
    
    if (tokens)
        cee_list_free(tokens);
    
    return descriptor;
}

static cee_char* objective_c_message_parameter_proto_descriptor_create(CEESourceFregment* fregment,
                                                                       cee_int index,
                                                                       CEESourceSymbol* parameter)
{
    cee_char* descriptor = NULL;
    cee_int i = 0;
    CEEList* p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    if (!p)
        return NULL;
    
    /** first child is return type */
    p = SOURCE_FREGMENT_NEXT(p);
    while (p) {
        if (i == index)
            break;
        p = SOURCE_FREGMENT_NEXT(p);
        i ++;
    }
    
    if (!p)
        return NULL;
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(p->data);
    if (!p)
        return NULL;
    
    descriptor = c_function_parameter_proto_descriptor_create(fregment,
                                                              SOURCE_FREGMENT_TOKENS_FIRST(p->data),
                                                              SOURCE_FREGMENT_TOKENS_LAST(p->data),
                                                              parameter);
    
    return descriptor;
}

static void objective_c_property_translate_table_init(void)
{
    /**
     *                          @optional   @required   @property   (           )               IBOutlet                IBAction                declaration_specifier       builtin_type    object_specifier    identifier          *                       {                   }                       ,           ;           others
     *  Init                    Init        Init        Property    Error       Error           Error                   Error                   Error                       Error           Error               Error               Error                   Error               Error                   Error       Confirm     Error
     *  Property                Error       Error       Error       Attribute   Error           Property                Property                DeclarationSpecifier        BuiltinType     ObjectSpecifier     CustomType          Error                   Error               Error                   Error       Error       Error
     *  Attribute               Error       Error       Error       Error       AttributeEnd    Error                   Error                   Error                       Error           Error               Error               Error                   Error               Error                   Error       Error       Error
     *  AttributeEnd            Error       Error       Error       Error       Error           AttributeEnd            AttributeEnd            DeclarationSpecifier        BuiltinType     ObjectSpecifier     CustomType          Error                   Error               Error                   Error       Error       Error
     *  BuiltinType             Error       Error       Error       Error       Error           BuiltinType             BuiltinType             DeclarationSpecifier        BuiltinType     Error               Identifier          BuiltinType             Error               Error                   Error       Error       Error
     *  DeclarationSpecifier    Error       Error       Error       Error       Error           DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier        BuiltinType     ObjectSpecifier     CustomType          DeclarationSpecifier    Error               Error                   Error       Error       Error
     *  ObjectSpecifier         Error       Error       Error       Error       Error           Error                   Error                   Error                       Error           Error               ObjectIdentifier    Error                   ObjectDefinition    Error                   Error       Error       Error
     *  ObjectIdentifier        Error       Error       Error       Error       Error           Error                   Error                   Error                       Error           Error               ObjectIdentifier    ObjectIdentifier        ObjectDefinition    Error                   Confirm     Error       Error
     *  ObjectDefinition        Error       Error       Error       Error       Error           Error                   Error                   Error                       Error           Error               Error               Error                   Error               ObjectDefinitionEnd     Error       Error       Error
     *  ObjectDefinitionEnd     Error       Error       Error       Error       Error           Error                   Error                   Error                       Error           Error               Identifier          ObjectDefinitionEnd     Error               Error                   Error       Error       Error
     *  CustomType              Error       Error       Error       Error       Error           CustomType              CustomType              Error                       BuiltinType     Error               Identifier          CustomType              Error               Error                   Error       Error       Error
     *  Identifier              Error       Error       Error       Error       Error           Error                   Error                   Error                       Error           Error               Identifier          Error                   Error               Error                   Confirm     Confirm     Error
     *  TypeInit                Error       Error       Error       Error       Error           TypeInit                TypeInit                Error                       Error           Error               Identifier          TypeInit                Error               Error                   Confirm     Confirm     Error
     *
     */
    TRANSLATE_STATE_INI(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateMax                  , kObjectiveCPropertyTranslateStateError                                                            );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateInit                 , kCEETokenID_AT_OPTIONAL                   , kObjectiveCPropertyTranslateStateInit                 );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateInit                 , kCEETokenID_AT_REQUIRED                   , kObjectiveCPropertyTranslateStateInit                 );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateInit                 , kCEETokenID_AT_PROPERTY                   , kObjectiveCPropertyTranslateStateProperty             );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateInit                 , ';'                                       , kObjectiveCPropertyTranslateStateConfirm              );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateProperty             , '('                                       , kObjectiveCPropertyTranslateStateAttribute            );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateProperty             , kCEETokenID_IB_OUTLET                     , kObjectiveCPropertyTranslateStateProperty             );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateProperty             , kCEETokenID_IB_ACTION                     , kObjectiveCPropertyTranslateStateProperty             );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateProperty             , token_id_is_declaration_specifier         , kObjectiveCPropertyTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateProperty             , token_id_is_builtin_type                  , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateProperty             , token_id_is_object_specifier              , kObjectiveCPropertyTranslateStateObjectSpecifier      );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateProperty             , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateCustomType           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateAttribute            , ')'                                       , kObjectiveCPropertyTranslateStateAttributeEnd         );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateAttributeEnd         , kCEETokenID_IB_OUTLET                     , kObjectiveCPropertyTranslateStateAttributeEnd         );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateAttributeEnd         , kCEETokenID_IB_ACTION                     , kObjectiveCPropertyTranslateStateAttributeEnd         );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateAttributeEnd         , token_id_is_declaration_specifier         , kObjectiveCPropertyTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateAttributeEnd         , token_id_is_builtin_type                  , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateAttributeEnd         , token_id_is_object_specifier              , kObjectiveCPropertyTranslateStateObjectSpecifier      );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateAttributeEnd         , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateCustomType           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateBuiltinType          , kCEETokenID_IB_OUTLET                     , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateBuiltinType          , kCEETokenID_IB_ACTION                     , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateBuiltinType          , token_id_is_declaration_specifier         , kObjectiveCPropertyTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateBuiltinType          , token_id_is_builtin_type                  , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateBuiltinType          , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateBuiltinType          , '*'                                       , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateDeclarationSpecifier , kCEETokenID_IB_OUTLET                     , kObjectiveCPropertyTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateDeclarationSpecifier , kCEETokenID_IB_ACTION                     , kObjectiveCPropertyTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateDeclarationSpecifier , token_id_is_declaration_specifier         , kObjectiveCPropertyTranslateStateDeclarationSpecifier );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateDeclarationSpecifier , token_id_is_builtin_type                  , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateDeclarationSpecifier , token_id_is_object_specifier              , kObjectiveCPropertyTranslateStateObjectSpecifier      );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateDeclarationSpecifier , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateCustomType           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateDeclarationSpecifier , '*'                                       , kObjectiveCPropertyTranslateStateDeclarationSpecifier );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectSpecifier      , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateObjectIdentifier     );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectSpecifier      , '{'                                       , kObjectiveCPropertyTranslateStateObjectDefinition     );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectIdentifier     , '*'                                       , kObjectiveCPropertyTranslateStateObjectIdentifier     );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectIdentifier     , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateObjectIdentifier     );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectIdentifier     , '{'                                       , kObjectiveCPropertyTranslateStateObjectDefinition     );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectDefinition     , '}'                                       , kObjectiveCPropertyTranslateStateObjectDefinitionEnd  );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectDefinitionEnd  , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateObjectDefinitionEnd  , '*'                                       , kObjectiveCPropertyTranslateStateObjectDefinitionEnd  );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateCustomType           , kCEETokenID_IB_OUTLET                     , kObjectiveCPropertyTranslateStateCustomType           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateCustomType           , kCEETokenID_IB_ACTION                     , kObjectiveCPropertyTranslateStateCustomType           );
    TRANSLATE_FUNCS_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateCustomType           , token_id_is_builtin_type                  , kObjectiveCPropertyTranslateStateBuiltinType          );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateCustomType           , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateCustomType           , '*'                                       , kObjectiveCPropertyTranslateStateCustomType           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateIdentifier           , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateIdentifier           , ','                                       , kObjectiveCPropertyTranslateStateConfirm              );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateIdentifier           , ';'                                       , kObjectiveCPropertyTranslateStateConfirm              );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateTypeInit             , kCEETokenID_IB_OUTLET                     , kObjectiveCPropertyTranslateStateTypeInit             );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateTypeInit             , kCEETokenID_IB_ACTION                     , kObjectiveCPropertyTranslateStateTypeInit             );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateTypeInit             , kCEETokenID_IDENTIFIER                    , kObjectiveCPropertyTranslateStateIdentifier           );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateTypeInit             , '*'                                       , kObjectiveCPropertyTranslateStateTypeInit             );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateTypeInit             , ','                                       , kObjectiveCPropertyTranslateStateConfirm              );
    TRANSLATE_STATE_SET(objective_c_property_translate_table, kObjectiveCPropertyTranslateStateTypeInit             , ';'                                       , kObjectiveCPropertyTranslateStateConfirm              );
}

static cee_boolean objective_c_property_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* property = NULL;
    CEEList* attribute_end = NULL;
    CEEList* identifier = NULL;
    CEEList* prefix = NULL;
    CEEList* prefix_tail = NULL;
    cee_boolean prefix_parse = FALSE;
    ObjectiveCPropertyTranslateState current = kObjectiveCPropertyTranslateStateInit;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        current = objective_c_property_translate_table[current][token->identifier];
        
        if (current == kObjectiveCPropertyTranslateStateError) {
            break;
        }
        else if (current == kObjectiveCPropertyTranslateStateProperty) {
            if (!property)
                property = p;
        }
        else if (current == kObjectiveCPropertyTranslateStateAttributeEnd) {
            if (!attribute_end)
                attribute_end = p;
        }
        else if (current == kObjectiveCPropertyTranslateStateIdentifier) {
            identifier = p;
        }
        else if (current == kObjectiveCPropertyTranslateStateObjectIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kObjectiveCPropertyTranslateStateConfirm) {
            
            if (!prefix_parse) {
                /**
                 *  - property (strong) NSView *view0, *view1;
                 *  ^~~~~~~~~~~~~~~~~~~~~~~~~~ prefix
                 *                      ^
                 *                      +~~~~~ prefix tail
                 */
                if (attribute_end)
                    prefix = TOKEN_NEXT(attribute_end);
                else if (property)
                    prefix = TOKEN_NEXT(property);
                
                if (identifier)
                    prefix_tail = TOKEN_PREV(identifier);
                
                prefix_parse = TRUE;
            }
            
            if (identifier) {
                declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                        fregment->subject_ref,
                                                                        identifier,
                                                                        identifier,
                                                                        kCEESourceSymbolTypePropertyDeclaration,
                                                                        "c",
                                                                        kCEETokenStringOptionCompact);
                if (declaration) {
                    declaration->alias = cee_strconcat("_", declaration->name, NULL);
                    declaration->proto_descriptor = c_declaration_proto_descriptor_create(fregment,
                                                                                          prefix,
                                                                                          prefix_tail,
                                                                                          identifier);
                    declarations = cee_list_prepend(declarations, declaration);
                    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
                }
                
                identifier = NULL;
            }
            
            current = kObjectiveCPropertyTranslateStateTypeInit;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (declarations) {
        fregment->symbols = cee_list_concat(declarations, fregment->symbols);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_PROPERTY
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static void c_proto_translate_table_init(void)
{
    /**
     *                                  declaration_specifier   decltype_specifier  builtin_type        object_specifier    alignas_specifier   access_specifier    identifier                  ::                              <                       >           (               )                   {                   }                       *                   &                   &&                  [                   ]                   :                   ,               others
     *  Init                            Init                    Decltype            BuiltinConfirm      ObjectSpecifier     *AlignasSpecifier   AccessSpecifier     CustomType                  CustomTypeScope                 Error                   Error       Error           Error               Error               Error                   Error               Error               Error               Init                Init                Error               Error           Error
     *  AccessSpecifier                 AccessSpecifier         Decltype            BuiltinConfirm      ObjectSpecifier     *AlignasSpecifier   AccessSpecifier     CustomType                  CustomTypeScope                 Error                   Error       Error           Error               Error               Error                   Error               Error               Error               AccessSpecifier     AccessSpecifier     AccessSpecifier     Error           Error
     *  Decltype                        Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                   Error       DecltypeList    Error               Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  DecltypeList                    Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                   Error       Error           DecltypeConfirm     Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  CustomType                      CustomType              Decltype            BuiltinConfirm      ObjectSpecifier     *AlignasSpecifier   AccessSpecifier     CustomTypeConfirm           CustomTypeScope                 *TemplateParameter      Error       Error           Error               Error               Error                   CustomType          CustomType          CustomType          CustomType          CustomType          Error               Error           Error
     *  CustomTypeScope                 Error                   Error               CustomType          Error               Error               Error               CustomType                  Error                           Error                   Error       Error           Error               Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  AlignasSpecifier                Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                   Error       AlignasList     Error               Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  AlignasList                     Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                   Error       Error           *AlignasListEnd     Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  ObjectSpecifier                 Error                   Error               Error               ObjectSpecifier     *AlignasSpecifier   Error               ObjectIdentifier            ObjectIdentifierScope           Error                   Error       Error           Error               ObjectDefinition    Error                   Error               Error               Error               ObjectSpecifier     ObjectSpecifier     Error               Error           Error
     *  ObjectIdentifier                Error                   Error               Error               Error               Error               Error               ObjectIdentifierConfirm     ObjectIdentifierScope           *TemplateParameter      Error       Error           Error               ObjectDefinition    Error                   ObjectIdentifier    ObjectIdentifier    ObjectIdentifier    ObjectIdentifier    ObjectIdentifier    ObjectDerive        Error           Error
     *  ObjectIdentifierScope           Error                   Error               Error               Error               Error               Error               ObjectIdentifier            Error                           Error                   Error       Error           Error               Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  ObjectDerive                    Error                   Error               Error               Error               Error               ObjectDerive        ObjectDeriveIdentifier      ObjectDeriveIdentifierScope     Error                   Error       Error           Error               Error               Error                   Error               Error               Error               Error               Error               Error               ObjectDerive    Error
     *  ObjectDeriveIdentifier          Error                   Error               Error               Error               Error               Error               Error                       ObjectDeriveIdentifierScope     *TemplateParameter      Error       Error           Error               ObjectDefinition    Error                   Error               Error               Error               Error               Error               Error               ObjectDerive    Error
     *  ObjectDeriveIdentifierScope     Error                   Error               Error               Error               Error               Error               ObjectDeriveIdentifier      Error                           Error                   Error       Error           Error               Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  ObjectDefinition                Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                   Error       Error           Error               Error               ObjectDefinitionConfirm Error               Error               Error               Error               Error               Error               Error           Error
     *  
     *  
     *  AlignasSpecifier: save 'current state'
     *  AlignasListEnd: restore 'current state'
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'
     *
     */
    TRANSLATE_STATE_INI(c_proto_translate_table, kCProtosTranslateStateMax                         , kCProtosTranslateStateError                                                           );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , token_id_is_declaration_specifier , kCProtosTranslateStateInit                        );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_DECLTYPE              , kCProtosTranslateStateDecltype                    );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , token_id_is_builtin_type          , kCProtosTranslateStateBuiltinConfirm              );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , token_id_is_object_specifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , token_id_is_access_specifier      , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_SCOPE                 , kCProtosTranslateStateCustomTypeScope             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , '['                               , kCProtosTranslateStateInit                        );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateInit                        , ']'                               , kCProtosTranslateStateInit                        );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_declaration_specifier , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_DECLTYPE              , kCProtosTranslateStateDecltype                    );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_builtin_type          , kCProtosTranslateStateBuiltinConfirm              );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_object_specifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_access_specifier      , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_SCOPE                 , kCProtosTranslateStateCustomTypeScope             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , '['                               , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , ']'                               , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAccessSpecifier             , ':'                               , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateDecltype                    , '('                               , kCProtosTranslateStateDecltypeList                );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateDecltypeList                , ')'                               , kCProtosTranslateStateDecltypeConfirm             );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_declaration_specifier , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_DECLTYPE              , kCProtosTranslateStateDecltype                    );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_builtin_type          , kCProtosTranslateStateBuiltinConfirm              );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_object_specifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_access_specifier      , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomTypeConfirm           );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_SCOPE                 , kCProtosTranslateStateCustomTypeScope             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , '<'                               , kCProtosTranslateStateTemplateParameter           );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , '*'                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , '&'                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_LOGIC_AND             , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , '['                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomType                  , ']'                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateCustomTypeScope             , token_id_is_builtin_type          , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateCustomTypeScope             , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAlignasSpecifier            , '('                               , kCProtosTranslateStateAlignasList                 );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateAlignasList                 , ')'                               , kCProtosTranslateStateAlignasListEnd              );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateObjectSpecifier             , token_id_is_object_specifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectSpecifier             , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectSpecifier             , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectSpecifier             , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectIdentifierScope       );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectSpecifier             , '{'                               , kCProtosTranslateStateObjectDefinition            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectSpecifier             , '['                               , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectSpecifier             , ']'                               , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectIdentifierConfirm     );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectIdentifierScope       );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , '<'                               , kCProtosTranslateStateTemplateParameter           );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , '{'                               , kCProtosTranslateStateObjectDefinition            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , '*'                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , '&'                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , kCEETokenID_LOGIC_AND             , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , '['                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , ']'                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifier            , ':'                               , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectIdentifierScope       , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_FUNCS_SET(c_proto_translate_table, kCProtosTranslateStateObjectDerive                , token_id_is_access_specifier      , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDerive                , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectDeriveIdentifier      );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDerive                , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectDeriveIdentifierScope );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDerive                , ','                               , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectDeriveIdentifierScope );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , '<'                               , kCProtosTranslateStateTemplateParameter           );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , '{'                               , kCProtosTranslateStateObjectDefinition            );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , ','                               , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDeriveIdentifierScope , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectDeriveIdentifier      );
    TRANSLATE_STATE_SET(c_proto_translate_table, kCProtosTranslateStateObjectDefinition            , '}'                               , kCProtosTranslateStateObjectDefinitionConfirm     );
}

static cee_char* c_type_descriptor_from_token_slice(CEESourceFregment* fregment,
                                                    CEEList* p,
                                                    CEEList* q)
{
    const cee_char* subject = fregment->subject_ref;
    CProtosTranslateState prev = kCProtosTranslateStateInit;
    CProtosTranslateState current = kCProtosTranslateStateInit;
    CProtosTranslateState alignas_prev_status = kCProtosTranslateStateInit;
    cee_char* proto_str = NULL;
    CEEToken* token = NULL;
            
    while (p) {
        token = p->data;

        prev = current;
        current = c_proto_translate_table[current][token->identifier];
        
        if (current == kCProtosTranslateStateError) {
            break;
        }
        else if (current == kCProtosTranslateStateTemplateParameter) {
            p = skip_template_parameter_list(p, FALSE);
            if (!p) {
                current = kCProtosTranslateStateError;
                break;
            }
            current = prev;
        }
        else if (current == kCProtosTranslateStateAlignasSpecifier) {
            alignas_prev_status = prev;
        }
        else if (current == kCProtosTranslateStateAlignasListEnd) {
            current = alignas_prev_status;
        }
        else if (current == kCProtosTranslateStateBuiltinConfirm) {
            if (proto_str)
                cee_free(proto_str);
            proto_str = NULL;
            proto_str = cee_string_from_token(subject, token);
            break;
        }
        else if (current == kCProtosTranslateStateDecltype) {
            if (proto_str)
                cee_free(proto_str);
            proto_str = NULL;
            
            if (token->identifier == kCEETokenID_DECLTYPE)
                cee_string_concat_with_token(&proto_str, subject, token);
        }
        else if (current == kCProtosTranslateStateDecltypeList) {
            if (token->identifier == '(')
                cee_string_concat_with_token(&proto_str, subject, token);
        }
        else if (current == kCProtosTranslateStateDecltypeConfirm) {
            if (token->identifier == ')')
                cee_string_concat_with_token(&proto_str, subject, token);
            break;
        }
        else if (current == kCProtosTranslateStateCustomType) {
            if (token->identifier == kCEETokenID_IDENTIFIER ||
                token_id_is_builtin_type(token->identifier))
                cee_string_concat_with_token(&proto_str, subject, token);
        }
        else if (current == kCProtosTranslateStateCustomTypeScope) {
            if (token->identifier == kCEETokenID_SCOPE)
                cee_string_concat_with_token(&proto_str, subject, token);
        }
        else if (current == kCProtosTranslateStateCustomTypeConfirm) {
            cee_strconcat0(&proto_str, ",", NULL);
            if (token->identifier == kCEETokenID_IDENTIFIER)
                cee_string_concat_with_token(&proto_str, subject, token);
            current = kCProtosTranslateStateCustomType;
        }
        else if (current == kCProtosTranslateStateObjectSpecifier) {
            if (proto_str)
                cee_free(proto_str);
            proto_str = NULL;
        }
        else if (current == kCProtosTranslateStateObjectIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                cee_string_concat_with_token(&proto_str, subject, token);
        }
        else if (current == kCProtosTranslateStateObjectIdentifierScope) {
            if (token->identifier == kCEETokenID_SCOPE)
                cee_string_concat_with_token(&proto_str, subject, token);
        }
        else if (current == kCProtosTranslateStateObjectIdentifierConfirm) {
            cee_strconcat0(&proto_str, ",", NULL);
            if (token->identifier == kCEETokenID_IDENTIFIER)
                cee_string_concat_with_token(&proto_str, subject, token);
            current = kCProtosTranslateStateObjectIdentifier;
        }
        else if (current == kCProtosTranslateStateObjectDefinitionConfirm) {
            break;
        }
        
        if (q && p == q)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCProtosTranslateStateError) {
        if (proto_str) {
            cee_free(proto_str);
            proto_str = NULL;
        }
    }
    
    return proto_str;
}

static void c_declaration_translate_table_init(void)
{
    /**
     *                                              typedef     declaration_specifier   literal                 virtual         decltype_specifier  access_specifier    builtin_type            object_specifier    alignas_specifier           attribute_specifier                         overload_operator   new             delete          virtual_specifier   identifier              noexcept    throw       assign_operator     <                   >           ::                          *                       &                       &&                      ~                       (                                   )                           [                       ]                       :                   operator    ,               ;           {                   }                       ->          Const       others
     *  Init                                        Typedef     DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType             ObjectSpecifier     AlignasSpecifier            AttributeSpecifier                          Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   Init                    Error                               Error                       Init                    Init                    Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  AttributeSpecifier                          Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   AttributeList                       Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  AttributeList                               Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Init                        Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  Typedef                                     Error       DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType             ObjectSpecifier     AlignasSpecifier            TypedefAttributeSpecifier                   Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   Init                    Error                               Error                       Init                    Init                    Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  TypedefAttributeSpecifier                   Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   TypedefAttributeList                Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  TypedefAttributeList                        Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Typedef                     Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  DeclarationSpecifier                        Error       DeclarationSpecifier    DeclarationSpecifier    Error           Decltype            Error               BuiltinType             objectSpecifier     Error                       DeclarationAttributeSpecifier               Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   DeclarationSpecifier    Error                               Error                       DeclarationSpecifier    DeclarationSpecifier    Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  DeclarationAttributeSpecifier               Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   DeclarationAttributeList            Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  DeclarationAttributeList                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               DeclarationSpecifier        Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  AccessSpecifier                             Typedef     DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType             ObjectSpecifier     AlignasSpecifier            AccessAttributeSpecifier                    Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   AccessSpecifier         Error                               Error                       AccessSpecifier         AccessSpecifier         AccessSpecifier     Error       Error           Error       Error               Error                   Error       Error       Error
     *  AccessAttributeSpecifier                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   AccessAttributeList                 Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  AccessAttributeList                         Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               AccessSpecifier             Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  BuiltinType                                 Error       BuiltinType             Error                   Error           Decltype            Error               BuiltinType             Error               Error                       BuiltinTypeAttributeSpecifier               Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       IdentifierScope             BuiltinType             BuiltinType             BuiltinType             BuiltinType             ParameterList                       Error                       BuiltinType             BuiltinType             Error               Operator    Error           Error       Error               Error                   Error       Error       Error
     *  BuiltinTypeAttributeSpecifier               Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   BuiltinTypeAttributeList            Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  BuiltinTypeAttributeList                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               BuiltinType                 Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  CustomType                                  Error       CustomType              Error                   Error           Decltype            AccessSpecifier     BuiltinType             ObjectSpecifier     Error                       CustomTypeAttributeSpecifier                Error               Error           Error           Error               Identifier              Error       Error       Error               *TemplateParameter  Error       CustomTypeScope             CustomType              CustomType              CustomType              CustomType              Structor                            Error                       CustomType              CustomType              Error               Operator    Error           Error       Error               Error                   Error       Error       Error
     *  CustomTypeAttributeSpecifier                Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   CustomTypeAttributeList             Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  CustomTypeAttributeList                     Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               CustomType                  Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  Structor                                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               StructorEnd                 Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  StructorEnd                                 Error       StructorEnd             Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           StructorEnd         StructorEnd             Noexcept    Throw       Confirm             Error               Error       Error                       StructorEnd             StructorEnd             StructorEnd             Error                   Surrounded                          Error                       Surrounded              Error                   Error               Error       Confirm         Confirm     Error               Error                   *Trailing   Error       Error
     *  CustomTypeScope                             Error       CustomType              Error                   Error           Error               Error               CustomType              Error               Error                       Error                                       Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectSpecifier                             Error       Error                   Error                   Error           Error               Error               Error                   ObjectSpecifier     ObjectAlignasSpecifier      ObjectAttributeSpecifier                    Error               Error           Error           Error               ObjectIdentifier        Error       Error       Error               Error               Error       ObjectIdentifierScope       Error                   Error                   Error                   Error                   Error                               Error                       ObjectSpecifier         ObjectSpecifier         Error               Error       Error           Error       ObjectDefinition    Error                   Error       Error       Error
     *  ObjectAlignasSpecifier                      Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ObjectAlignasList                   Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectAlignasList                           Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ObjectSpecifier             Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectAttributeSpecifier                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ObjectAttributeList                 Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectAttributeList                         Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ObjectSpecifier             Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectIdentifier                            Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       ObjectIdentifierAttributeSpecifier          Error               Error           Error           Error               ObjectIdentifier        Error       Error       Confirm             Error               Error       ObjectIdentifierScope       ObjectIdentifier        ObjectIdentifier        ObjectIdentifier        Error                   Error                               Error                       ObjectIdentifier        ObjectIdentifier        ObjectDerive        Operator    Confirm         Confirm     ObjectDefinition    Error                   Error       Error       Error
     *  ObjectIdentifierAttributeSpecifier          Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ObjectIdentifierAttributeList       Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectIdentifierAttributeList               Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ObjectIdentifier            Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectIdentifierScope                       Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               ObjectIdentifier        Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectDerive                                Error       Error                   Error                   ObjectDerive    Error               ObjectDerive        ObjectDeriveIdentifier  Error               Error                       Error                                       Error               Error           Error           Error               ObjectDeriveIdentifier  Error       Error       Error               Error               Error       ObjectDeriveIdentifierScope Error                   Error                   Error                   Error                   Error                               Error                       ObjectDerive            ObjectDerive            Error               Error       ObjectDerive    Error       Error               Error                   Error       Error       Error
     *  ObjectDeriveIdentifier                      Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       ObjectDeriveIdentifierAttributeSpecifier    Error               Error           Error           Error               Error                   Error       Error       Error               *TemplateParameter  Error       ObjectDeriveIdentifierScope Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       ObjectDerive    Error       ObjectDefinition    Error                   Error       Error       Error
     *  ObjectDeriveIdentifierAttributeSpecifier    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ObjectDeriveIdentifierAttributeList Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectDeriveIdentifierAttributeList         Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ObjectDeriveIdentifier      Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectDeriveIdentifierScope                 Error       Error                   Error                   Error           Error               Error               ObjectDeriveIdentifier  Error               Error                       Error                                       Error               Error           Error           Error               ObjectDeriveIdentifier  Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectDefinition                            Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       Error           Error       Error               ObjectDefinitionEnd     Error       Error       Error
     *  ObjectDefinitionEnd                         Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       AttributeSpecifier                          Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       Error                       ObjectDefinitionEnd     ObjectDefinitionEnd     ObjectDefinitionEnd     Error                   Error                               Error                       ObjectDefinitionEnd     ObjectDefinitionEnd     Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectDefinitionEndAttributeSpecifier       Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ObjectDefinitionEndAttributeList    Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ObjectDefinitionEndAttributeList            Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ObjectDefinitionEnd         Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  AlignasSpecifier                            Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   AlignasList                         Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  AlignasList                                 Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Init                        Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  Decltype                                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   DecltypeList                        Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  DecltypeList                                Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               BuiltinType                 Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  Identifier                                  Error       DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType             objectSpecifier     Error                       IdentifierAttributeSpecifier                Error               Error           Error           Error               Identifier              Error       Error       Confirm             *TemplateParameter  Error       IdentifierScope             Identifier              Identifier              Identifier              Identifier              ParameterList                       Error                       Identifier              Identifier              BitField            Operator    Confirm         Confirm     InitList            Error                   Error       Error       Error
     *  IdentifierAttributeSpecifier                Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   IdentifierAttributeList             Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  IdentifierAttributeList                     Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Identifier                  Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  IdentifierScope                             Error       Identifier              Error                   Error           Error               Error               Identifier              Error               Error                       Error                                       Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   IdentifierScope         Error                               Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ParameterList                               Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ParameterListEnd            Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ParameterListEnd                            Error       ParameterListEnd        Error                   Error           Error               Error               Error                   Error               Error                       ParameterListEndAttributeSpecifier          Error               Error           Error           ParameterListEnd    ParameterListEnd        Noexcept    Throw       Confirm             Error               Error       Error                       ParameterListEnd        ParameterListEnd        ParameterListEnd        Error                   Surrounded                          Error                       Surrounded              Error                   Error               Error       Confirm         Confirm     Error               Error                   *Trailing   Error       Error
     *  ParameterListEndAttributeSpecifier          Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterListEndAttributeList       Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ParameterListEndAttributeList               Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ParameterListEnd            Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  InitList                                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       Error           Error       Error               InitListEnd             Error       Error       Error
     *  InitListEnd                                 Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       Confirm         Confirm     Error               Error                   Error       Error       Error
     *  Surrounded                                  Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               SurroundedEnd               Error                   SurroundedEnd           Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  SurroundedEnd                               Error       SurroundedEnd           Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           SurroundedEnd       SurroundedEnd           Noexcept    Throw       Confirm             Error               Error       Error                       SurroundedEnd           SurroundedEnd           SurroundedEnd           Error                   Error                               Error                       SurroundedEnd           SurroundedEnd           Error               Error       Confirm         Confirm     Error               Error                   *Trailing   Error       Error
     *  Noexcept                                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   NoexceptList                        Error                       Error                   Error                   Error               Error       Confirm         Confirm     Error               Error                   *Trailing   Error       Error
     *  NoexceptList                                Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               NoexceptListEnd             Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  NoexceptListEnd                             Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       NoexceptListEnd         NoexceptListEnd         Error               Error       Confirm         Confirm     Error               Error                   *Trailing   Error       Error
     *  Throw                                       Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ThrowList                           Error                       Error                   Error                   Error               Error       Confirm         Confirm     Error               Error                   *Trailing   Error       Error
     *  ThrowList                                   Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               ThrowListEnd                Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  ThrowListEnd                                Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       ThrowListEnd            ThrowListEnd            Error               Error       Confirm         Confirm     Error               Error                   *Trailing   Error       Error
     *  BitField                                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               *BitSize                Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       *BitSize    Error
     *  Operator                                    Error       Error                   Error                   Error           Error               Error               OverloadType            Error               Error                       Error                                       OverloadOperator    OverloadNew     OverloadDelete  Error               OverloadType            Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   OverloadRoundBracket                Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadOperator                            Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       OverloadOperator    Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList                       Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadNew                                 Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList                       Error                       OverloadNew             OverloadNew             Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadDelete                              Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList                       Error                       OverloadDelete          OverloadDelete          Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadType                                Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   OverloadTypeSurrounded              Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadTypeSurrounded                      Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               OverloadTypeSurroundedEnd   Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadTypeSurroundedEnd                   Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList                       Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadRoundBracket                        Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                               OverloadRoundBracketEnd     Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  OverloadRoundBracketEnd                     Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList                       Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  TypeInit                                    Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       TypeInitAttributeSpecifier                  Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       Error                       TypeInit                TypeInit                TypeInit                TypeInit                ParameterList                       Error                       TypeInit                TypeInit                Error               Operator    Error           Error       Error               Error                   Error       Error       Error
     *  TypeInitAttributeSpecifier                  Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   TypeInitAttributeList               Error                       Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     *  TypeInitAttributeList                       Error       Error                   Error                   Error           Error               Error               Error                   Error               Error                       Error                                       Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   TypeInit                            ParameterListEnd            Error                   Error                   Error               Error       Error           Error       Error               Error                   Error       Error       Error
     
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'
     *  Assignment: skip to Confirm/Terminate
     *  Trailing: skip to Confirm/Terminate
     *  BitSize: set any current to 'Identifier' after BitSize
     */
    TRANSLATE_STATE_INI(c_declaration_translate_table, kCDeclarationTranslateStateMax                                       , kCDeclarationTranslateStateError                                                                              );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , token_id_is_object_specifier          , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , token_id_is_alignas_specifier         , kCDeclarationTranslateStateAlignasSpecifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , '~'                                   , kCDeclarationTranslateStateInit                                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , '['                                   , kCDeclarationTranslateStateInit                                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , ']'                                   , kCDeclarationTranslateStateInit                                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , kCEETokenID_TYPEDEF                   , kCDeclarationTranslateStateTypedef                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                                      , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateAttributeSpecifier                         );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAttributeSpecifier                        , '('                                   , kCDeclarationTranslateStateAttributeList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAttributeList                             , ')'                                   , kCDeclarationTranslateStateInit                                       );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , token_id_is_object_specifier          , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , token_id_is_alignas_specifier         , kCDeclarationTranslateStateAlignasSpecifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , '~'                                   , kCDeclarationTranslateStateInit                                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , '['                                   , kCDeclarationTranslateStateInit                                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , ']'                                   , kCDeclarationTranslateStateInit                                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                                   , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateTypedefAttributeSpecifier                  );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedefAttributeSpecifier                 , '('                                   , kCDeclarationTranslateStateTypedefAttributeList                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedefAttributeList                      , ')'                                   , kCDeclarationTranslateStateTypedefAttributeList                       );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , kCEETokenID_LITERAL                   , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , token_id_is_object_specifier          , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , '~'                                   , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , '['                                   , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , ']'                                   , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier                      , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateDeclarationAttributeSpecifier              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationAttributeSpecifier             , '('                                   , kCDeclarationTranslateStateDeclarationAttributeList                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationAttributeList                  , ')'                                   , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , kCEETokenID_TYPEDEF                   , kCDeclarationTranslateStateTypedef                                    );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , token_id_is_object_specifier          , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , token_id_is_alignas_specifier         , kCDeclarationTranslateStateAlignasSpecifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , '~'                                   , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , '['                                   , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , ']'                                   , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , ':'                                   , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier                           , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateAccessAttributeSpecifier                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessAttributeSpecifier                  , '('                                   , kCDeclarationTranslateStateAccessAttributeList                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessAttributeList                       , ')'                                   , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , token_id_is_declaration_specifier     , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateIdentifierScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , '*'                                   , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , '&'                                   , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , '~'                                   , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , '('                                   , kCDeclarationTranslateStateParameterList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , '['                                   , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , ']'                                   , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                               , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateBuiltinTypeAttributeSpecifier              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinTypeAttributeSpecifier             , '('                                   , kCDeclarationTranslateStateBuiltinTypeAttributeList                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinTypeAttributeList                  , ')'                                   , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , token_id_is_declaration_specifier     , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , token_id_is_object_specifier          , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , '<'                                   , kCDeclarationTranslateStateTemplateParameter                          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , '*'                                   , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , '&'                                   , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , '~'                                   , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , '('                                   , kCDeclarationTranslateStateStructor                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , '['                                   , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , ']'                                   , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                                , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateCustomTypeAttributeSpecifier               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeAttributeSpecifier              , '('                                   , kCDeclarationTranslateStateCustomTypeAttributeList                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeAttributeList                   , ')'                                   , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructor                                  , ')'                                   , kCDeclarationTranslateStateStructorEnd                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , token_id_is_declaration_specifier     , kCDeclarationTranslateStateStructorEnd                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , token_id_is_virtual_sepcifier         , kCDeclarationTranslateStateStructorEnd                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateStructorEnd                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , kCEETokenID_NOEXCEPT                  , kCDeclarationTranslateStateNoexcept                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , kCEETokenID_THROW                     , kCDeclarationTranslateStateThrow                                      );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , token_id_is_assignment                , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , '*'                                   , kCDeclarationTranslateStateStructorEnd                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , '&'                                   , kCDeclarationTranslateStateStructorEnd                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateStructorEnd                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , '('                                   , kCDeclarationTranslateStateSurrounded                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , '['                                   , kCDeclarationTranslateStateSurrounded                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                               , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeScope                           , token_id_is_declaration_specifier     , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeScope                           , token_id_is_builtin_type              , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeScope                           , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , token_id_is_alignas_specifier         , kCDeclarationTranslateStateObjectAlignasSpecifier                     );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , token_id_is_object_specifier          , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectIdentifierScope                      );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , '['                                   , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , ']'                                   , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , '{'                                   , kCDeclarationTranslateStateObjectDefinition                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier                           , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateObjectAttributeSpecifier                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectAttributeSpecifier                  , '('                                   , kCDeclarationTranslateStateObjectAttributeList                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectAttributeList                       , ')'                                   , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectAlignasSpecifier                    , '('                                   , kCDeclarationTranslateStateObjectAlignasList                          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectAlignasList                         , ')'                                   , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , token_id_is_assignment                , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectIdentifierScope                      );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , ':'                                   , kCDeclarationTranslateStateObjectDerive                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , '{'                                   , kCDeclarationTranslateStateObjectDefinition                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , '*'                                   , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , '&'                                   , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , '['                                   , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , ']'                                   , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier                          , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateObjectIdentifierAttributeSpecifier         );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifierAttributeSpecifier        , '('                                   , kCDeclarationTranslateStateObjectIdentifierAttributeList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifierAttributeList             , ')'                                   , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifierScope                     , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectIdentifier                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , kCEETokenID_VIRTUAL                   , kCDeclarationTranslateStateObjectDerive                               );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , token_id_is_access_specifier          , kCDeclarationTranslateStateObjectDerive                               );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , token_id_is_builtin_type              , kCDeclarationTranslateStateObjectDeriveIdentifier                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectDeriveIdentifier                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectDeriveIdentifierScope                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , ','                                   , kCDeclarationTranslateStateObjectDerive                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , '['                                   , kCDeclarationTranslateStateObjectDerive                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                              , ']'                                   , kCDeclarationTranslateStateObjectDerive                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier                    , '<'                                   , kCDeclarationTranslateStateTemplateParameter                          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier                    , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectDeriveIdentifierScope                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier                    , ','                                   , kCDeclarationTranslateStateObjectDerive                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier                    , '{'                                   , kCDeclarationTranslateStateObjectDefinition                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier                    , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateObjectDeriveIdentifierAttributeSpecifier   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifierAttributeSpecifier  , '('                                   , kCDeclarationTranslateStateObjectDeriveIdentifierAttributeList        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifierAttributeList       , ')'                                   , kCDeclarationTranslateStateObjectDeriveIdentifier                     );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifierScope               , token_id_is_builtin_type              , kCDeclarationTranslateStateObjectDeriveIdentifier                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifierScope               , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectDeriveIdentifier                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinition                          , '}'                                   , kCDeclarationTranslateStateObjectDefinitionEnd                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd                       , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd                       , '*'                                   , kCDeclarationTranslateStateObjectDefinitionEnd                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd                       , '&'                                   , kCDeclarationTranslateStateObjectDefinitionEnd                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd                       , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateObjectDefinitionEnd                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd                       , '['                                   , kCDeclarationTranslateStateObjectDefinitionEnd                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd                       , ']'                                   , kCDeclarationTranslateStateObjectDefinitionEnd                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd                       , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateObjectDefinitionEndAttributeSpecifier      );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEndAttributeSpecifier     , '('                                   , kCDeclarationTranslateStateObjectDefinitionEndAttributeList           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEndAttributeList          , ')'                                   , kCDeclarationTranslateStateObjectDefinitionEnd                        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDecltype                                  , '('                                   , kCDeclarationTranslateStateDecltypeList                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDecltypeList                              , ')'                                   , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier                            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , token_id_is_object_specifier          , kCDeclarationTranslateStateObjectSpecifier                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , token_id_is_assignment                , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , ':'                                   , kCDeclarationTranslateStateBitField                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , '<'                                   , kCDeclarationTranslateStateTemplateParameter                          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateIdentifierScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , '*'                                   , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , '&'                                   , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , '~'                                   , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , '('                                   , kCDeclarationTranslateStateParameterList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , '['                                   , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , ']'                                   , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , '{'                                   , kCDeclarationTranslateStateInitList                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                                , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateIdentifierAttributeSpecifier               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierAttributeSpecifier              , '('                                   , kCDeclarationTranslateStateIdentifierAttributeList                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierAttributeList                   , ')'                                   , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope                           , token_id_is_declaration_specifier     , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope                           , token_id_is_builtin_type              , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope                           , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope                           , '~'                                   , kCDeclarationTranslateStateIdentifierScope                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterList                             , ')'                                   , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , token_id_is_declaration_specifier     , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , token_id_is_virtual_sepcifier         , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , kCEETokenID_NOEXCEPT                  , kCDeclarationTranslateStateNoexcept                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , kCEETokenID_THROW                     , kCDeclarationTranslateStateThrow                                      );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , token_id_is_assignment                , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , '*'                                   , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , '&'                                   , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , '('                                   , kCDeclarationTranslateStateSurrounded                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , '['                                   , kCDeclarationTranslateStateSurrounded                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd                          , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateParameterListEndAttributeSpecifier         );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEndAttributeSpecifier        , '('                                   , kCDeclarationTranslateStateParameterListEndAttributeList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEndAttributeList             , ')'                                   , kCDeclarationTranslateStateParameterListEnd                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInitList                                  , '}'                                   , kCDeclarationTranslateStateInitListEnd                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInitListEnd                               , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInitListEnd                               , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurrounded                                , ')'                                   , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurrounded                                , ']'                                   , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , token_id_is_assignment                , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , token_id_is_virtual_sepcifier         , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , kCEETokenID_NOEXCEPT                  , kCDeclarationTranslateStateNoexcept                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , kCEETokenID_THROW                     , kCDeclarationTranslateStateThrow                                      );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , token_id_is_assignment                , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , '*'                                   , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , '&'                                   , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , '['                                   , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , ']'                                   , kCDeclarationTranslateStateSurroundedEnd                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                             , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                                  , '('                                   , kCDeclarationTranslateStateNoexceptList                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                                  , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                                  , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                                  , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexceptList                              , ')'                                   , kCDeclarationTranslateStateNoexceptListEnd                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexceptListEnd                           , '['                                   , kCDeclarationTranslateStateNoexceptListEnd                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexceptListEnd                           , ']'                                   , kCDeclarationTranslateStateNoexceptListEnd                            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexceptListEnd                           , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexceptListEnd                           , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexceptListEnd                           , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                                     , '('                                   , kCDeclarationTranslateStateThrowList                                  );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                                     , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                                     , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                                     , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrowList                                 , ')'                                   , kCDeclarationTranslateStateThrowListEnd                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrowListEnd                              , '['                                   , kCDeclarationTranslateStateThrowListEnd                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrowListEnd                              , ']'                                   , kCDeclarationTranslateStateThrowListEnd                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrowListEnd                              , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrowListEnd                              , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrowListEnd                              , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , '*'                                   , kCDeclarationTranslateStateTypeInit                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , '&'                                   , kCDeclarationTranslateStateTypeInit                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateTypeInit                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , '~'                                   , kCDeclarationTranslateStateTypeInit                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , '('                                   , kCDeclarationTranslateStateParameterList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , '['                                   , kCDeclarationTranslateStateTypeInit                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , ']'                                   , kCDeclarationTranslateStateTypeInit                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                                  , kCEETokenID___ATTRIBUTE__             , kCDeclarationTranslateStateTypeInitAttributeSpecifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInitAttributeSpecifier                , '('                                   , kCDeclarationTranslateStateTypeInitAttributeList                      );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInitAttributeList                     , ')'                                   , kCDeclarationTranslateStateTypeInit                                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitField                                  , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateBitSize                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitField                                  , kCEETokenID_CONSTANT                  , kCDeclarationTranslateStateBitSize                                    );
    TRANSLATE_STATE_ANY(c_declaration_translate_table, kCDeclarationTranslateStateBitSize                                   , kCDeclarationTranslateStateIdentifier                                                                         );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitSize                                   , ','                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitSize                                   , ';'                                   , kCDeclarationTranslateStateConfirm                                    );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                                  , token_id_is_builtin_type              , kCDeclarationTranslateStateOverloadType                               );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                                  , token_id_is_overload_operator         , kCDeclarationTranslateStateOverloadOperator                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                                  , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateOverloadType                               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                                  , kCEETokenID_NEW                       , kCDeclarationTranslateStateOverloadNew                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                                  , kCEETokenID_DELETE                    , kCDeclarationTranslateStateOverloadDelete                             );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                                  , '('                                   , kCDeclarationTranslateStateOverloadRoundBracket                       );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadOperator                          , token_id_is_overload_operator         , kCDeclarationTranslateStateOverloadOperator                           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadOperator                          , '('                                   , kCDeclarationTranslateStateParameterList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadNew                               , '['                                   , kCDeclarationTranslateStateOverloadNew                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadNew                               , ']'                                   , kCDeclarationTranslateStateOverloadNew                                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadNew                               , '('                                   , kCDeclarationTranslateStateParameterList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadDelete                            , '['                                   , kCDeclarationTranslateStateOverloadDelete                             );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadDelete                            , ']'                                   , kCDeclarationTranslateStateOverloadDelete                             );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadDelete                            , '('                                   , kCDeclarationTranslateStateParameterList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadType                              , '('                                   , kCDeclarationTranslateStateOverloadTypeSurrounded                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadTypeSurrounded                    , ')'                                   , kCDeclarationTranslateStateOverloadTypeSurroundedEnd                  );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadTypeSurroundedEnd                 , '('                                   , kCDeclarationTranslateStateParameterList                              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadRoundBracket                      , ')'                                   , kCDeclarationTranslateStateOverloadRoundBracketEnd                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadRoundBracketEnd                   , '('                                   , kCDeclarationTranslateStateParameterList                              );
}

static cee_boolean c_declaration_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    cee_boolean is_class_member = FALSE;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    CEEList* identifier = NULL;
    CEEList* object_identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEEList* surrounded = NULL;
    CEEList* surrounded_end = NULL;
    cee_boolean is_typedef = FALSE;
    CEEList* prefix = NULL;
    CEEList* prefix_tail = NULL;
    cee_boolean prefix_parse = FALSE;
    CDeclarationTranslateState current = kCDeclarationTranslateStateInit;
    CDeclarationTranslateState prev = kCDeclarationTranslateStateInit;
    
    if (cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeClassDefinition) ||
        cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeStructDefinition))
        is_class_member = TRUE;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
                
        prev = current;
        current = c_declaration_translate_table[current][token->identifier];
        
        if (prev == kCDeclarationTranslateStateStructorEnd &&
            current != kCDeclarationTranslateStateSurrounded &&
            !is_class_member) {
            /**
             *  class Point {
             *      Point();
             *  };
             *  "Point" is a Structor
             *
             *  void func() {
             *      Point();
             *  }
             *  "Point" is a function invoke
             *
             *  void func () {
             *      Point (*Point)();
             *  }
             *  "*Point" is a function pointer declaration
             */
                break;
        }
        
        if (current == kCDeclarationTranslateStateError) {
            break;
        }
        else if (current == kCDeclarationTranslateStateBitField) {
            if (!is_class_member)
                break;
        }
        else if (current == kCDeclarationTranslateStateTypedef) {
            is_typedef = TRUE;
        }
        else if (current == kCDeclarationTranslateStateTemplateParameter) {
            p = skip_template_parameter_list(p, FALSE);
            if (!p) 
                break;
            current = prev;
        }
        else if (current == kCDeclarationTranslateStateCustomType) {
            identifier = p;
        }
        else if (current == kCDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kCDeclarationTranslateStateStructor) {
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kCDeclarationTranslateStateStructorEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
        else if (current == kCDeclarationTranslateStateParameterList) {
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kCDeclarationTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
        else if (current == kCDeclarationTranslateStateOperator) {
            identifier = p;
        }
        else if (current == kCDeclarationTranslateStateObjectIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                object_identifier = p;
        }
        else if (current == kCDeclarationTranslateStateSurrounded) {
            if (!surrounded)
                surrounded = p;
        }
        else if (current == kCDeclarationTranslateStateSurroundedEnd) {
            if (!surrounded_end)
                surrounded_end = p;
        }
        else if (current == kCDeclarationTranslateStateTrailing ||
                 current == kCDeclarationTranslateStateConfirm) {
            
            if (!prefix_parse) {
                prefix = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
                prefix_tail = c_declaration_prefix_tail_get(fregment,
                                                            identifier,
                                                            object_identifier,
                                                            parameter_list,
                                                            surrounded);
                prefix_parse = TRUE;
            }
            
            if (parameter_list && surrounded) {
                /**
                 *  static int (*foo) (int a, int b)
                 *             ^parameter_list
                 *                  ^parameter_list_end
                 *                    ^surrounded
                 *                                 ^surrounded_end
                 *  ^~~~~~~~~~ prefix
                 *         ^prefix_tail
                 */
                declaration = c_function_pointer_declaration_create(fregment,
                                                                    prefix,
                                                                    prefix_tail,
                                                                    parameter_list,
                                                                    parameter_list_end,
                                                                    surrounded,
                                                                    surrounded_end);
            }
            else if (parameter_list) {
                declaration = c_function_declaration_create(fregment,
                                                            prefix,
                                                            prefix_tail,
                                                            identifier,
                                                            parameter_list,
                                                            parameter_list_end);
            }
            else if (identifier) {
                declaration = c_identifier_declaration_create(fregment,
                                                              prefix,
                                                              prefix_tail,
                                                              identifier);
            }
            else if (object_identifier) {
                q = cee_token_not_whitespace_newline_before(object_identifier);
                if (q)
                    token = q->data;
                if (token_id_is_object_specifier(token->identifier))
                    declaration = c_custom_type_declaration_create(fregment,
                                                                   prefix,
                                                                   prefix_tail,
                                                                   object_identifier);
                else
                    declaration = c_identifier_declaration_create(fregment,
                                                                  prefix,
                                                                  prefix_tail,
                                                                  object_identifier);
            }
            
            if (declaration) {
                declarations = cee_list_prepend(declarations, declaration);
                if (is_typedef)
                    declaration->type = kCEESourceSymbolTypeTypeDefine;
            }
            
            declaration = NULL;
            identifier = NULL;
            parameter_list = NULL;
            parameter_list_end = NULL;
            object_identifier = NULL;
            surrounded = NULL;
            surrounded_end = NULL;
            
            if (!cee_token_is_identifier(p, ',') &&
                !cee_token_is_identifier(p, ';')) {
                p = skip_c_declaration_interval(p);
                if (!p)
                    break;
            }
            
            current = kCDeclarationTranslateStateTypeInit;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
        
    if (declarations) {
        fregment->symbols = cee_list_concat(fregment->symbols, declarations);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_DECLARATION
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

/**
 *  "static int" is declaration prefix, "int" is prefix tail
 *
 *  static int a, func(), c = 1.0;
 *  ^~~~~~~~~~ prefix
 *         ^
 *         +~~ prefix tail
 *
 */
static CEEList* c_declaration_prefix_tail_get(CEESourceFregment* fregment,
                                              CEEList* identifier,
                                              CEEList* object_identifier,
                                              CEEList* parameter_list,
                                              CEEList* surround)
{
    CEEList* tail = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    
    if (parameter_list && surround) {
        tail = cee_token_not_whitespace_newline_before(parameter_list);
    }
    else if (identifier) {
        q = identifier;
        if (cee_token_is_identifier(q, kCEETokenID_IDENTIFIER)) {
            r = cee_token_not_whitespace_newline_before(q);
            if (cee_token_is_identifier(r, '~'))
                q = r;
        }
        tail = TOKEN_PREV(q);
    }
    else if (object_identifier) {
        tail = TOKEN_PREV(object_identifier);
    }
    
    return tail;
}

static CEESourceSymbol* c_function_declaration_create(CEESourceFregment* fregment,
                                                      CEEList* prefix,
                                                      CEEList* prefix_tail,
                                                      CEEList* identifier,
                                                      CEEList* parameter_list,
                                                      CEEList* parameter_list_end)
{
    CEESourceSymbol* declaration = NULL;
    CEESourceFregment* child = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    
    if (!identifier ||
        !parameter_list ||
        !parameter_list_end)
        return NULL;
    
    q = identifier;
    r = cee_token_not_whitespace_newline_before(q);
    if (!cee_token_is_identifier(r, '~')) /** cpp destructor */
        r = q;
    
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            r,
                                                            q,
                                                            kCEESourceSymbolTypeFunctionDeclaration,
                                                            "c",
                                                            kCEETokenStringOptionCompact);
    if (!declaration)
        return NULL;
    
    cee_token_slice_state_mark(r, q, kCEETokenStateSymbolOccupied);
    
    child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
    if (child) {
        child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
        c_function_parameters_parse(child);
    }
    
    declaration->proto_descriptor = c_function_proto_descriptor_create(fregment,
                                                                       prefix,
                                                                       prefix_tail,
                                                                       r,
                                                                       parameter_list,
                                                                       parameter_list_end);
    return declaration;
}

static CEESourceSymbol* c_function_pointer_declaration_create(CEESourceFregment* fregment,
                                                              CEEList* prefix,
                                                              CEEList* prefix_tail,
                                                              CEEList* parameter_list,
                                                              CEEList* parameter_list_end,
                                                              CEEList* surround,
                                                              CEEList* surround_end)
{
    
    CEESourceSymbol* declaration = NULL;
    CEESourceFregment* child = NULL;
    CEEList* q = NULL;
    
    if (!parameter_list ||
        !parameter_list_end ||
        !surround ||
        !surround_end)
        return NULL;

    child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
    if (child) {
        child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
        q = cee_token_until_identifier(SOURCE_FREGMENT_TOKENS_FIRST(child),
                                       kCEETokenID_IDENTIFIER,
                                       FALSE);
    }
    if (q)
        declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                fregment->subject_ref,
                                                                q,
                                                                q,
                                                                kCEESourceSymbolTypeFunctionDeclaration,
                                                                "c",
                                                                kCEETokenStringOptionCompact);
    if (!declaration)
        return NULL;
    
    cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
        
    child = cee_source_fregment_child_index_by_leaf(fregment, surround->data);
    if (child) {
        child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
        c_function_parameters_parse(child);
    }
    
    declaration->proto_descriptor = c_function_proto_descriptor_create(fregment,
                                                                       prefix,
                                                                       prefix_tail,
                                                                       q,
                                                                       surround,
                                                                       surround_end);
    
    return declaration;
}

static cee_char* c_function_proto_descriptor_create(CEESourceFregment* fregment,
                                                    CEEList* prefix,
                                                    CEEList* prefix_tail,
                                                    CEEList* identifier,
                                                    CEEList* parameter_list,
                                                    CEEList* parameter_list_end)
{
    if (!fregment ||
        !identifier ||
        !parameter_list ||
        !parameter_list_end)
        return NULL;
    
    const cee_char* subject = fregment->subject_ref;
    CEEList* tokens = cee_source_fregment_tokens_expand(fregment);
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
    CEESourceFregment* grandfather = NULL;
    const cee_char* access_level = NULL;
    cee_char* return_type_str = NULL;
    CEESourceSymbol* parent_symbol = NULL;
    
    grandfather = cee_source_fregment_grandfather_get(fregment);
    if (grandfather) {
        access_level = cee_source_fregment_attribute_get(grandfather, "access_level");
        
        if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition))
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather,
                                                                        kCEESourceSymbolTypeClassDefinition);
        else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeStructDefinition))
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather,
                                                                        kCEESourceSymbolTypeStructDefinition);
        
        if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition) ||
            cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeStructDefinition))
            is_class_member = TRUE;
    }
    
    if (!access_level) {
        if (grandfather) {
            if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition))
                access_level = "private";
            else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeStructDefinition))
                access_level = "public";
        }
        if (!access_level)
            access_level = "public";
    }
    
    if (prefix && prefix_tail)
        return_type_str = c_type_descriptor_from_token_slice(fregment,
                                                             prefix,
                                                             prefix_tail);
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
        
        if (!token_id_is_c_function_proto_header_ignored(token->identifier) &&
            !cee_token_id_is_newline(token->identifier) &&
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

static CEESourceSymbol* c_identifier_declaration_create(CEESourceFregment* fregment,
                                                        CEEList* prefix,
                                                        CEEList* prefix_tail,
                                                        CEEList* identifier)
{
    CEESourceSymbol* declaration = NULL;
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            identifier,
                                                            identifier,
                                                            kCEESourceSymbolTypeVariableDeclaration,
                                                            "c",
                                                            kCEETokenStringOptionCompact);
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    if (prefix && prefix_tail)
        declaration->proto_descriptor = c_declaration_proto_descriptor_create(fregment,
                                                                              prefix,
                                                                              prefix_tail,
                                                                              identifier);
    return declaration;
}

static CEESourceSymbol* c_custom_type_declaration_create(CEESourceFregment* fregment,
                                                         CEEList* prefix,
                                                         CEEList* prefix_tail,
                                                         CEEList* identifier)
{
    CEESourceSymbol* declaration = NULL;
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            identifier,
                                                            identifier,
                                                            kCEESourceSymbolTypeCustomTypeDeclaration,
                                                            "c",
                                                            kCEETokenStringOptionCompact);
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    
    if (prefix && prefix_tail)
        declaration->proto_descriptor = c_declaration_proto_descriptor_create(fregment,
                                                                              prefix,
                                                                              prefix_tail,
                                                                              identifier);
    return declaration;
}

static cee_char* c_declaration_proto_descriptor_create(CEESourceFregment* fregment,
                                                       CEEList* prefix,
                                                       CEEList* prefix_tail,
                                                       CEEList* identifier)
{
    CEESourceFregment* grandfather = NULL;
    const cee_char* access_level = NULL;
    cee_char* proto = NULL;
    cee_char* descriptor = NULL;
    const cee_char* subject = fregment->subject_ref;
    cee_char* identifier_str = NULL;
    
    grandfather = cee_source_fregment_grandfather_get(fregment);
    if (grandfather)
        access_level = cee_source_fregment_attribute_get(grandfather, "access_level");
    
    if (!access_level) {
        if (grandfather) {
            if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeStructDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDeclaration)) {
                access_level = "private";
            }
        }
        if (!access_level)
            access_level = "public";
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"declaration\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto = c_type_descriptor_from_token_slice(fregment, prefix, prefix_tail);
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

/**
 * p should point to a '<' or '>'
 */
static CEEList* skip_template_parameter_list(CEEList* p,
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

static CEEList* skip_include_path(CEEList* p)
{
    return skip_template_parameter_list(p, FALSE);
}

static CEEList* skip_c_declaration_interval(CEEList* p)
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

static void c_namespace_definition_translate_table_init(void)
{
    
    /*
     *                  namespace       identifier      ::          [               ]           inline_specifier    {       alignas         (               )           others
     *  Init            Namespace       Error           Error       Error           Error       Error               Error   Error           Error           Error       Error
     *  Namespace       Error           Name            Namescope   Namespace       Namespace   InlineSpecifier     Confirm Alignas         Error           Error       Error
     *  Name            Error           Error           Namescope   Error           Error       Error               Confirm Error           Error           Error       Error
     *  Namescope       Error           Name            Error       Error           Error       InlineSpecifier     Error   Error           Error           Error       Error
     *  InlineSpecifier Error           Name            Error       Error           Error       Error               Error   Error           Error           Error       Error
     *  Alignas         Error           Error           Error       Error           Error       Error               Error   Error           AlignasBegin    Error       Error
     *  AlignasBegin    Error           Error           Error       Error           Error       Error               Error   Error           Error           Namespace   Error
     */
    
    TRANSLATE_STATE_INI(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateMax              , kCNamespaceDefinitionTranslateStateError                                          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateInit             , kCEETokenID_NAMESPACE     , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_IDENTIFIER    , kCNamespaceDefinitionTranslateStateDefinitionName     );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_SCOPE         , kCNamespaceDefinitionTranslateStateNameScope          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , '['                       , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , ']'                       , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_INLINE        , kCNamespaceDefinitionTranslateStateInlineSpecifier    );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , '{'                       , kCNamespaceDefinitionTranslateStateConfirm            );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_ALIGNAS       , kCNamespaceDefinitionTranslateStateAlignas            );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateAlignas          , '('                       , kCNamespaceDefinitionTranslateStateAlignasBegin       );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateAlignasBegin     , ')'                       , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateDefinitionName   , kCEETokenID_SCOPE         , kCNamespaceDefinitionTranslateStateNameScope          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateDefinitionName   , '{'                       , kCNamespaceDefinitionTranslateStateConfirm            );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNameScope        , kCEETokenID_IDENTIFIER    , kCNamespaceDefinitionTranslateStateDefinitionName     );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNameScope        , kCEETokenID_INLINE        , kCNamespaceDefinitionTranslateStateInlineSpecifier    );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateInlineSpecifier  , kCEETokenID_IDENTIFIER    , kCNamespaceDefinitionTranslateStateDefinitionName     );
}

static cee_boolean c_namespace_definition_trap(CEESourceFregment* fregment,
                                               CEEList** pp)
{
    cee_boolean ret = FALSE;
    CEESourceSymbol* definition = NULL;
    CEEList* p = *pp;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    CEEList* trap_keyword = NULL;
    CNamespaceDefinitionTranslateState current = kCNamespaceDefinitionTranslateStateInit;
    CNamespaceDefinitionTranslateState prev = kCNamespaceDefinitionTranslateStateInit;
    
    while (p) {
        token = p->data;
        
        prev = current;
        current = c_namespace_definition_translate_table[current][token->identifier];
        
        if (current == kCNamespaceDefinitionTranslateStateDefinitionName) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                q = p;
        }
        else if (current == kCNamespaceDefinitionTranslateStateNamespace) {
            if (token->identifier == kCEETokenID_NAMESPACE)
                trap_keyword = p;
        }
        else if (current == kCNamespaceDefinitionTranslateStateConfirm)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCNamespaceDefinitionTranslateStateConfirm) {
        if (q) {
            definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                   fregment->subject_ref,
                                                                   q, 
                                                                   q, 
                                                                   kCEESourceSymbolTypeNamespaceDefinition, 
                                                                   "c",
                                                                   kCEETokenStringOptionCompact);
            cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
        }
        else {
            definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                   fregment->subject_ref, 
                                                                   trap_keyword, 
                                                                   trap_keyword, 
                                                                   kCEESourceSymbolTypeNamespaceDefinition, 
                                                                   "c",
                                                                   kCEETokenStringOptionCompact);
        }
        
        if (definition) {
            definition->proto_descriptor = c_namespace_proto_descriptor_create(fregment, definition);
            fregment->symbols = cee_list_prepend(fregment->symbols, definition);
            cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeNamespaceDefinition);
            ret = TRUE;
        }
    }
    
    *pp = NULL;
    
#ifdef DEBUG_NAMESPACE
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static cee_char* c_namespace_proto_descriptor_create(CEESourceFregment* fregment,
                                                     CEESourceSymbol* namespace)
{
    if (!fregment || !namespace)
        return NULL;
    
    cee_char* descriptor = NULL;
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"namespace\"", NULL);
    
    if (namespace->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", namespace->name, "\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static void c_extern_block_translate_table_init(void)
{
    /**
     *                  extern  literal      identifier     {       others
     *  Init            Extern  Error        Error          Error   Error
     *  Extern          Error   Identifier   Identifier     Error   Error
     *  Identifier      Error   Identifier   Identifier     Confirm Error
     */
    TRANSLATE_STATE_INI(c_extern_block_translate_table, kCExternBlockTranslateStateMax          , kCExternBlockTranslateStateError                                          );
    TRANSLATE_STATE_SET(c_extern_block_translate_table, kCExternBlockTranslateStateInit         , kCEETokenID_EXTERN        , kCExternBlockTranslateStateExtern             );
    TRANSLATE_STATE_SET(c_extern_block_translate_table, kCExternBlockTranslateStateExtern       , kCEETokenID_LITERAL       , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table, kCExternBlockTranslateStateExtern       , kCEETokenID_IDENTIFIER    , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table, kCExternBlockTranslateStateIdentifier   , kCEETokenID_LITERAL       , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table, kCExternBlockTranslateStateIdentifier   , kCEETokenID_IDENTIFIER    , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table, kCExternBlockTranslateStateIdentifier   , '{'                       , kCExternBlockTranslateStateConfirm            );
}

static cee_boolean c_extern_block_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    CEEToken* token = NULL;
    CEESourceSymbol* extern_block = NULL;
    CExternBlockTranslateState current = kCExternBlockTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        current = c_extern_block_translate_table[current][token->identifier];
        
        if (current == kCExternBlockTranslateStateError) {
            break;
        }
        else if (current == kCExternBlockTranslateStateExtern) {
            if (!q && cee_token_is_identifier(p, kCEETokenID_EXTERN))
                q = p;
        }
        else if (current == kCExternBlockTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) ||
                cee_token_is_identifier(p, kCEETokenID_LITERAL))
                r = p;
        }
        else if (current == kCExternBlockTranslateStateConfirm) {
            extern_block = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     q,
                                                                     r,
                                                                     kCEESourceSymbolTypeExternBlock,
                                                                     "c",
                                                                     kCEETokenStringOptionCompact);
            
            if (extern_block->name)
                cee_free(extern_block->name);
            extern_block->name = cee_string_from_token_slice(fregment->subject_ref,
                                                             q,
                                                             r,
                                                             kCEETokenStringOptionDefault);
            if (extern_block->ranges)
                cee_list_free_full(extern_block->ranges, cee_range_free);
            extern_block->ranges = cee_range_from_token(r->data);
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (extern_block) {
        fregment->symbols = cee_list_prepend(fregment->symbols, extern_block);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeIdentifierBlock);
        ret = TRUE;
    }
    
#ifdef DEBUG_DECLARATION
    if (extern_block)
        cee_source_symbol_print(extern_block);
#endif
    
    return ret;
}

static void block_header_parse(CParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    ParseTrap trap = NULL;
    
    if (!current || !current->tokens_ref)
        return;
    
    if (c_extern_block_parse(current))
        return;
    
    if (c_function_definition_parse(current) ||
        objective_c_message_parse(current))
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

static cee_boolean block_reduce(CParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    
    if (cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeClassDefinition) ||
        cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeStructDefinition) ||
        cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeEnumDefinition) ||
        cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeUnionDefinition) ||
        cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeAssignmentBlock)) {
        /** expect statement terminate ';' */
    }
    else {
        if (cee_source_fregment_grandfather_type_is(parser->statement_current,
                                                    kCEESourceFregmentTypeTemplateDeclaration))
            if (!template_pop(parser))
                return FALSE;
        
        statement_attach(parser, kCEESourceFregmentTypeStatement);
    }
    return TRUE;
}

static cee_boolean colon_parse(CParser* parser)
{
    CEESourceFregment* fregment = parser->statement_current;
    if (!fregment || !fregment->tokens_ref)
        return FALSE;
    
    if (cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeFunctionDefinition) ||
        cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeStatementBlock)) {
        return label_parse(fregment);
    }
    else if (cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeClassDefinition) ||
             cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeStructDefinition) ||
             cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeUnionDefinition) ||
             cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeEnumDefinition) ) {
        return access_level_parse(fregment);
    }
    return FALSE;
}

static cee_boolean label_parse(CEESourceFregment* fregment)
{
    const cee_char* subject = fregment->subject_ref;
    cee_boolean ret = FALSE;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEESourceSymbol* label = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_NEW_LINE) ||
            cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE)) {
            goto next_token;
        }
        else {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                q = cee_token_not_whitespace_newline_after(p);
                if (q && cee_token_is_identifier(q, ':')) {
                    label = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                      subject,
                                                                      p,
                                                                      p,
                                                                      kCEESourceSymbolTypeLabel,
                                                                      "c",
                                                                      kCEETokenStringOptionCompact);
                    if (label) {
                        cee_token_slice_state_mark(p, q, kCEETokenStateSymbolOccupied);
                        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeLabelDeclaration);
                        fregment->symbols = cee_list_prepend(fregment->symbols, label);
                        ret = TRUE;
                        break;
                    }
                }
            }
            else {
                break;
            }
        }
next_token:
        p = TOKEN_NEXT(p);
    }
    
#ifdef DEBUG_LABEL
    if (label)
        cee_source_symbol_print(label);
#endif
    
    return ret;
}

static cee_boolean access_level_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_is_identifier(p, kCEETokenID_NEW_LINE) ||
            cee_token_is_identifier(p, kCEETokenID_WHITE_SPACE))
            goto next_token;
        
        if (cee_token_is_identifier(p, ':')) {
            q = cee_token_not_whitespace_newline_before(p);
            if (q) {
                token = q->data;
                if (token_id_is_access_specifier(token->identifier)) {
                    access_level_transform(fregment, token);
                    ret = TRUE;
                }
            }
            break;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    return ret;
}

static void parser_access_level_transform(CParser* parser,
                                          CEEToken* token)
{
    if (!parser->statement_current || !token)
        return;
    access_level_transform(parser->statement_current, token);
}

static void access_level_transform(CEESourceFregment* fregment,
                                   CEEToken* token)
{
    if (!fregment || !token)
        return;
    
    CEESourceFregment* grandfather = cee_source_fregment_grandfather_get(fregment);
    if (!grandfather)
        return;
    
    if (token->identifier == kCEETokenID_PUBLIC ||
        token->identifier == kCEETokenID_AT_PUBLIC)
        cee_source_fregment_attribute_set(grandfather,
                                          "access_level",
                                          "public");
    else if (token->identifier == kCEETokenID_PRIVATE ||
             token->identifier == kCEETokenID_AT_PRIVATE)
        cee_source_fregment_attribute_set(grandfather,
                                          "access_level",
                                          "private");
    else if (token->identifier == kCEETokenID_PROTECTED ||
             token->identifier == kCEETokenID_AT_PROTECTED)
        cee_source_fregment_attribute_set(grandfather,
                                          "access_level",
                                          "protected");
}

/**
 * parameter list
 */
static void parameter_list_push(CParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeRoundBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean parameter_list_pop(CParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

/**
 * subscript
 */
static void subscript_push(CParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeSquareBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean subscript_pop(CParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

/**
 * objective-c
 */
static void objective_c_definition_translate_table_init(void)
{
    /*
     *                    @interface @implementation @protocol identifier       (           )          :       <                 >               ,             {              others
     *  Init              Interface  implementation  protocol  Init             Init        Init       Init    Init              Init            Init          Init           Init
     *  Interface         Error      Error           Error     DefinitionName   Error       Error      Error   Error             Error           Error         Error          Error
     *  Implementation    Error      Error           Error     DefinitionName   Error       Error      Error   Error             Error           Error         Error          Error
     *  Protocol          Error      Error           Error     DefinitionName   Error       Error      Error   Error             Error           Error         Error          Error
     *  DefinitionName    Error      Error           Error     Error            ExtendBegin Error      Colon   ProtocolListBegin Error           Error         VariableBlock  Error
     *  Colon             Error      Error           Error     Derived          Error       Error      Error   Error             Error           Error         Error          Error
     *  Derived           Error      Error           Error     Error            Error       Error      Error   ProtocolListBegin Error           Error         VariableBlock  Error
     *  ExtendBegin       Error      Error           Error     Extend           Error       ExtendEnd  Error   Error             Error           Error         Error          Error
     *  Extend            Error      Error           Error     Error            Error       ExtendEnd  Error   Error             Error           Error         Error          Error
     *  ExtendEnd         Error      Error           Error     Error            Error       Error      Error   ProtocolListBegin Error           Error         VariableBlock  Error
     *  ProtocolListBegin Error      Error           Error     ProtocolNames    Error       Error      Error   Error             ProtocolListEnd Error         Error          Error
     *  ProtocolNames     Error      Error           Error     ProtocolNames    Error       Error      Error   Error             ProtocolListEnd ProtocolNames Error          Error
     *  ProtocolListEnd   Error      Error           Error     Error            Error       Error      Error   Error             Error           Error         VariableBlock  Error
     *  VariableBlock     Error      Error           Error     Error            Error       Error      Error   Error             Error           Error         Error          Error
     */
    TRANSLATE_STATE_INI(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateMax                  , kObjectiveCDefinitionTranslateStateError                                                  );
    TRANSLATE_STATE_ANY(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kObjectiveCDefinitionTranslateStateInit                                                   );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kCEETokenID_AT_INTERFACE      , kObjectiveCDefinitionTranslateStateInterface              );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kCEETokenID_AT_IMPLEMENTATION , kObjectiveCDefinitionTranslateStateImplementation         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kCEETokenID_AT_PROTOCOL       , kObjectiveCDefinitionTranslateStateProtocol               );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInterface            , kCEETokenID_IDENTIFIER        , kObjectiveCDefinitionTranslateStateDefinitionName         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateImplementation       , kCEETokenID_IDENTIFIER        , kObjectiveCDefinitionTranslateStateDefinitionName         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocol             , kCEETokenID_IDENTIFIER        , kObjectiveCDefinitionTranslateStateDefinitionName         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , '('                           , kObjectiveCDefinitionTranslateStateExtendBegin            );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , ':'                           , kObjectiveCDefinitionTranslateStateColon                  );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , '<'                           , kObjectiveCDefinitionTranslateStateProtocolListBegin      );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , '{'                           , kObjectiveCDefinitionTranslateStateVariableBlock          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateColon                , kCEETokenID_IDENTIFIER        , kObjectiveCDefinitionTranslateStateDerived                );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDerived              , '<'                           , kObjectiveCDefinitionTranslateStateProtocolListBegin      );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDerived              , '{'                           , kObjectiveCDefinitionTranslateStateVariableBlock          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendBegin          , kCEETokenID_IDENTIFIER        , kObjectiveCDefinitionTranslateStateExtend                 );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendBegin          , ')'                           , kObjectiveCDefinitionTranslateStateExtendEnd              );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtend               , ')'                           , kObjectiveCDefinitionTranslateStateExtendEnd              );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendEnd            , '<'                           , kObjectiveCDefinitionTranslateStateProtocolListBegin      );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendEnd            , '{'                           , kObjectiveCDefinitionTranslateStateVariableBlock          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolListBegin    , kCEETokenID_IDENTIFIER        , kObjectiveCDefinitionTranslateStateProtocolNames          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolListBegin    , '>'                           , kObjectiveCDefinitionTranslateStateProtocolListEnd        );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolNames        , kCEETokenID_IDENTIFIER        , kObjectiveCDefinitionTranslateStateProtocolNames          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolNames        , ','                           , kObjectiveCDefinitionTranslateStateProtocolNames          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolNames        , '>'                           , kObjectiveCDefinitionTranslateStateProtocolListEnd        );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolListEnd      , '{'                           , kObjectiveCDefinitionTranslateStateVariableBlock          );
}

static cee_boolean objective_c_token_push(CParser* parser,
                                          CEEToken* push)
{
    ObjectiveCDefinitionTranslateState current = kObjectiveCDefinitionTranslateStateInit;
    ObjectiveCDefinitionTranslateState prev = kObjectiveCDefinitionTranslateStateInit;
    const cee_char* subject = parser->subject_ref;
    
    CEEToken* token = NULL;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(parser->statement_current);
    
    while (p) {
        token = p->data;
        
        if (token->identifier == kCEETokenID_AT_INTERFACE)
            prev = kObjectiveCDefinitionTranslateStateInterface;
        else if (token->identifier == kCEETokenID_AT_IMPLEMENTATION)
            prev = kObjectiveCDefinitionTranslateStateImplementation;
        else if (token->identifier == kCEETokenID_AT_PROTOCOL)
            prev = kObjectiveCDefinitionTranslateStateProtocol;
        else
            prev = objective_c_definition_translate_table[prev][token->identifier];
        
        p = TOKEN_NEXT(p);
    }
    
    current = objective_c_definition_translate_table[prev][push->identifier];
    
    if (current == kObjectiveCDefinitionTranslateStateVariableBlock) {
        objective_c_definition_parse(parser->statement_current, subject);
        
        statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
        statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
        statement_token_push(parser, push);
        current_statement_type_transform(parser, kCEESourceFregmentTypeVariableBlock);
        statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
        statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
        
        return TRUE;
    }
    
    if (current == kObjectiveCDefinitionTranslateStateError) {
        if (prev == kObjectiveCDefinitionTranslateStateDefinitionName || 
            prev == kObjectiveCDefinitionTranslateStateDerived || 
            prev == kObjectiveCDefinitionTranslateStateProtocolListEnd || 
            prev == kObjectiveCDefinitionTranslateStateExtendEnd) {
            
            objective_c_definition_parse(parser->statement_current, subject);
            
            statement_sub_attach(parser, kCEESourceFregmentTypeCurlyBracketList);
            statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
        }
        return FALSE;
    }
    
    statement_token_push(parser, push);
    
    return TRUE;
}

static cee_boolean objective_c_definition_parse(CEESourceFregment* fregment,
                                                const cee_char* subject)
{
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    ObjectiveCDefinitionType type = kObjectiveCDefinitionTypeInit;
    CEESourceSymbol* definition = NULL;
    
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_AT_INTERFACE)) {
            type = kObjectiveCDefinitionTypeInterface;
            definition = objective_c_interface_extract(fregment,
                                                       fregment->tokens_ref,
                                                       fregment->filepath_ref,
                                                       subject);
            break;
        }
        else if (cee_token_is_identifier(p, kCEETokenID_AT_IMPLEMENTATION)) {
            type = kObjectiveCDefinitionTypeImplementation;
            definition = objective_c_implementation_extract(fregment,
                                                            fregment->tokens_ref,
                                                            fregment->filepath_ref,
                                                            subject);
            break;
        }
        else if (cee_token_is_identifier(p, kCEETokenID_AT_PROTOCOL)) {
            type = kObjectiveCDefinitionTypeProtocol;
            definition = objective_c_protocol_extract(fregment,
                                                      fregment->tokens_ref,
                                                      fregment->filepath_ref,
                                                      subject);
            break;
        }
        p = TOKEN_NEXT(p);
    }
    
    if (!definition)
        return FALSE;
    
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    
    if (type == kObjectiveCDefinitionTypeInterface)
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeInterfaceDeclaration);
    else if (type == kObjectiveCDefinitionTypeImplementation)
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeImplementationDefinition);
    else if (type == kObjectiveCDefinitionTypeProtocol)
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeProtocolDeclaration);
    
    return TRUE;
}

static CEESourceSymbol* objective_c_interface_extract(CEESourceFregment* fregment,
                                                      CEEList* tokens,
                                                      const cee_char* filepath,
                                                      const cee_char* subject)
{
    CEEList* p = TOKEN_FIRST(tokens);
    CEEToken* token = NULL;
    CEEList* name = NULL;
    CEEList* derives = NULL;
    CEESourceSymbol* interface = NULL;
    ObjectiveCDefinitionTranslateState current = kObjectiveCDefinitionTranslateStateInit;
    
    while (p) {
        token = p->data;
        current = objective_c_definition_translate_table[current][token->identifier];
        
        if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
            if (current == kObjectiveCDefinitionTranslateStateDefinitionName)
                name = p;
            else if (current == kObjectiveCDefinitionTranslateStateDerived ||
                     current == kObjectiveCDefinitionTranslateStateProtocolNames)
                TOKEN_APPEND(derives, token);
        }
        p = TOKEN_NEXT(p);
    }
    
    if (!name)
        goto exit;
    
    interface = cee_source_symbol_create_from_token_slice(filepath,
                                                          subject, 
                                                          name, 
                                                          name, 
                                                          kCEESourceSymbolTypeInterfaceDeclaration, 
                                                          "c",
                                                          kCEETokenStringOptionCompact);
    
    if (interface) {
        cee_token_slice_state_mark(name, name, kCEETokenStateSymbolOccupied);
        if (derives)
            interface->derives = c_name_list_create(derives, subject);
        interface->proto_descriptor = objective_c_interface_proto_descriptor_create(fregment,
                                                                                    interface,
                                                                                    interface->derives);
    }
    
    
exit:
    if (derives)
        cee_list_free(derives);
    
#ifdef DEBUG_INTERFACE
    if (interface)
        cee_source_symbol_print(interface);
#endif
    
    return interface;
}

static cee_char* c_name_list_create(CEEList* tokens,
                                    const cee_char* subject)
{
    cee_char* names_str = NULL;
    cee_char* name_str = NULL;
    CEEToken* token = NULL;
    CEEList* p = TOKEN_FIRST(tokens);
    
    while (p) {
        token = p->data;
        name_str = cee_string_from_token(subject, token);
        if (name_str) {
            cee_strconcat0(&names_str, name_str, NULL);
            cee_free(name_str);
        }
        if (TOKEN_NEXT(p))
            cee_strconcat0(&names_str, ",", NULL);
        
        p = TOKEN_NEXT(p);
    }
    return names_str;
}

static cee_char* objective_c_interface_proto_descriptor_create(CEESourceFregment* fregment,
                                                               CEESourceSymbol* interface,
                                                               const cee_char* derives_str)
{
    if (!fregment || !interface)
        return NULL;
    
    cee_char* descriptor = NULL;
            
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"interface\"", NULL);
    
    if (interface->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", interface->name, "\"", NULL);
    
    if (derives_str)
        cee_strconcat0(&descriptor, ", \"derivers\":", "\"", derives_str, "\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static CEESourceSymbol* objective_c_implementation_extract(CEESourceFregment* fregment,
                                                           CEEList* tokens,
                                                           const cee_char* filepath,
                                                           const cee_char* subject)
{
    CEEList* p = TOKEN_FIRST(tokens);
    CEESourceSymbol* implementation = NULL;
    
    BRACKET_SIGN_DECLARATION();
    CLEAR_BRACKETS();
    
    p = cee_token_until_identifier(p, kCEETokenID_AT_IMPLEMENTATION, FALSE);
    if (!p)
        goto exit;
    
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) && BRACKETS_IS_CLEAN())
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    if (!p)
        goto exit;
    
    implementation = cee_source_symbol_create_from_token_slice(filepath,
                                                               subject, 
                                                               p, 
                                                               p, 
                                                               kCEESourceSymbolTypeImplementationDefinition, 
                                                               "c",
                                                               kCEETokenStringOptionCompact);
    if (implementation) {
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        implementation->proto_descriptor = objective_c_implementation_proto_descriptor_create(fregment,
                                                                                              implementation);
    }
    
exit:
#ifdef DEBUG_IMPLEMENTATION
    if (implementation)
        cee_source_symbol_print(implementation);
#endif
    
    return implementation;
}

static cee_char* objective_c_implementation_proto_descriptor_create(CEESourceFregment* fregment,
                                                                    CEESourceSymbol* implementation)
{
    if (!fregment || !implementation)
        return NULL;
    
    cee_char* descriptor = NULL;
            
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"implementation\"", NULL);
    
    if (implementation->name) {
        cee_strconcat0(&descriptor, ", \"name\":", "\"", implementation->name, "\"", NULL);
    }
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

static CEESourceSymbol* objective_c_protocol_extract(CEESourceFregment* fregment,
                                                     CEEList* tokens,
                                                     const cee_char* filepath,
                                                     const cee_char* subject)
{
    CEEList* p = TOKEN_FIRST(tokens);
    CEESourceSymbol* protocol = NULL;
    
    BRACKET_SIGN_DECLARATION();
    CLEAR_BRACKETS();
    
    p = cee_token_until_identifier(p, kCEETokenID_AT_PROTOCOL, FALSE);
    if (!p)
        goto exit;
    
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) && BRACKETS_IS_CLEAN())
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    if (!p)
        goto exit;
    
    protocol = cee_source_symbol_create_from_token_slice(filepath,
                                                         subject, 
                                                         p, 
                                                         p, 
                                                         kCEESourceSymbolTypeProtocolDeclaration, 
                                                         "c",
                                                         kCEETokenStringOptionCompact);
    if (protocol) {
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        protocol->proto_descriptor = objective_c_protocol_proto_descriptor_create(fregment,
                                                                                  protocol);
    }
    
exit:
#ifdef DEBUG_PROTOCOL
    if (protocol)
        cee_source_symbol_print(protocol);
#endif
    
    return protocol;
}

static cee_char* objective_c_protocol_proto_descriptor_create(CEESourceFregment* fregment,
                                                              CEESourceSymbol* protocol)
{
    if (!fregment || !protocol)
        return NULL;
    
    cee_char* descriptor = NULL;
            
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"protocol\"", NULL);
    
    if (protocol->name)
        cee_strconcat0(&descriptor, ", \"name\":", "\"", protocol->name, "\"", NULL);
    
    cee_strconcat0(&descriptor, "}", NULL);
    
    return descriptor;
}

/**
 * parser
 */
static void c_block_header_trap_init(CParser* parser)
{
    
    for (int i = 0; i < kCEETokenID_END; i ++)
        parser->block_header_traps[i] = NULL;
    
    parser->block_header_traps[kCEETokenID_CLASS] = c_class_definition_trap;
    parser->block_header_traps[kCEETokenID_STRUCT] = c_struct_definition_trap;
    parser->block_header_traps[kCEETokenID_ENUM] = c_enum_definition_trap;
    parser->block_header_traps[kCEETokenID_UNION] = c_union_definition_trap;
    parser->block_header_traps[kCEETokenID_NS_ENUM] = objective_c_enum_definition_trap;
    parser->block_header_traps[kCEETokenID_NS_OPTIONS] = objective_c_enum_definition_trap;
    parser->block_header_traps[kCEETokenID_NAMESPACE] = c_namespace_definition_trap;
}

CEESourceParserRef cee_c_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    parser->symbol_search_in_scope = symbol_search_in_scope;
    
    CParser* c = parser_create();
    c->super = parser;
    
    c_block_header_trap_init(c);
    c_token_type_map_init();
    
    c_prep_directive_include_translate_table_init();
    c_prep_directive_define_translate_table_init();
    objective_c_message_translate_table_init();
    objective_c_definition_translate_table_init();
    objective_c_property_translate_table_init();
    c_function_definition_translate_table_init();
    c_function_parameters_declaration_translate_table_init();
    c_inheritance_definition_translate_table_init();
    c_template_declaration_translate_table_init();
    c_declaration_translate_table_init();
    c_namespace_definition_translate_table_init();
    c_proto_translate_table_init();
    c_extern_block_translate_table_init();
    objective_c_enum_definition_translate_table_init();
    
    parser->imp = c;
        
    return parser;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type)
{
    return (c_token_type_map[token->identifier] & type) != 0;
}

void cee_c_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static CParser* parser_create(void)
{
    return cee_malloc0(sizeof(CParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    CParser* parser = (CParser*)data;
    cee_free(parser);
}

static void parser_state_set(CParser* parser,
                             CParserState state)
{
    parser->state |= state;
}

static void parser_state_clear(CParser* parser,
                               CParserState state)
{
    parser->state &= ~state;
}

static void symbol_parse_init(CParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->prep_directive_root =
        cee_source_fregment_create(kCEESourceFregmentTypeRoot,
                                   parser->filepath_ref,
                                   parser->subject_ref,
                                   "c");
    parser->prep_directive_current =
        cee_source_fregment_sub_attach(parser->prep_directive_root,
                                       kCEESourceFregmentTypeSourceList,
                                       parser->filepath_ref,
                                       parser->subject_ref,
                                       "c");
    parser->prep_directive_current =
        cee_source_fregment_sub_attach(parser->prep_directive_current,
                                       kCEESourceFregmentTypePrepDirective,
                                       parser->filepath_ref,
                                       parser->subject_ref,
                                       "c");
    parser->statement_root =
        cee_source_fregment_create(kCEESourceFregmentTypeRoot,
                                   parser->filepath_ref,
                                   parser->subject_ref,
                                   "c");
    parser->statement_current =
        cee_source_fregment_sub_attach(parser->statement_root,
                                       kCEESourceFregmentTypeSourceList,
                                       parser->filepath_ref,
                                       parser->subject_ref,
                                       "c");
    parser->statement_current =
        cee_source_fregment_sub_attach(parser->statement_current,
                                       kCEESourceFregmentTypeStatement,
                                       parser->filepath_ref,
                                       parser->subject_ref,
                                       "c");
    parser->comment_root =
        cee_source_fregment_create(kCEESourceFregmentTypeRoot,
                                   parser->filepath_ref,
                                   parser->subject_ref,
                                   "c");
    parser->comment_current =
        cee_source_fregment_sub_attach(parser->comment_root,
                                       kCEESourceFregmentTypeSourceList,
                                       parser->filepath_ref,
                                       parser->subject_ref,
                                       "c");
    parser->comment_current =
        cee_source_fregment_sub_attach(parser->comment_current,
                                       kCEESourceFregmentTypeComment,
                                       parser->filepath_ref,
                                       parser->subject_ref,
                                       "c");
    parser->state = kCParserStateStatementParsing;
}

static cee_boolean current_statement_type_is(CParser* parser,
                                             CEESourceFregmentType type)
{
    if (parser->statement_current)
        return cee_source_fregment_type_is(parser->statement_current, type);
        
    return FALSE;
}

static void current_statement_type_transform(CParser* parser,
                                             CEESourceFregmentType type)
{
    cee_source_fregment_type_set(parser->statement_current, type);
}

static cee_boolean statement_token_push(CParser* parser,
                                        CEEToken* push)
{
    if (!parser->statement_current)
        return FALSE;
    
    SOURCE_FREGMENT_TOKEN_PUSH(parser->statement_current, push, TRUE);
    
    return TRUE;
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
    CParser* parser = parser_ref->imp;
    cee_int ret = 0;
    cee_boolean pushed = TRUE;
    CEEToken* token = NULL;
    CEESourceTokenMap* map = NULL;
    CEEList* tokens = NULL;
    
    if (!subject)
        return FALSE;
    
    map = cee_source_token_map_create(subject);
    cee_lexer_c_buffer_create(subject);
    symbol_parse_init(parser, filepath, subject);
    
    do {
        ret = cee_lexer_c_token_get(&token);
    
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
                parser_state_clear(parser, kCParserStatePrepDirectiveParsing);
            }
        }
        else if (token_id_is_prep_directive(token->identifier)) {
            
            parser_state_set(parser, kCParserStatePrepDirectiveParsing);
            
            if (token->identifier == kCEETokenID_HASH_ELIF ||
                token->identifier == kCEETokenID_HASH_ELSE ||
                token->identifier == kCEETokenID_HASH_ENDIF) {
                
                if (!prep_directive_branch_pop(parser))
                    parser_state_clear(parser, kCParserStatePrepDirectiveParsing);
        
                prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
                prep_directive_token_push(parser, token);
            }
            else {
                prep_directive_token_push(parser, token);
            }
        }
        else if (statement_parsing(parser)) {
            
            pushed = FALSE;
            
            if (current_statement_type_is(parser, kCEESourceFregmentTypeInterfaceDeclaration) ||
                current_statement_type_is(parser, kCEESourceFregmentTypeImplementationDefinition) ||
                current_statement_type_is(parser, kCEESourceFregmentTypeProtocolDeclaration)) {
                pushed = objective_c_token_push(parser, token);
            }
            else if (current_statement_type_is(parser, kCEESourceFregmentTypeTemplateDeclaration)) {
                pushed = c_template_token_push(parser, token);
            }
        
            if (!pushed) {
                if (token->identifier == kCEETokenID_AT_INTERFACE) {
                    current_statement_type_transform(parser, kCEESourceFregmentTypeInterfaceDeclaration);
                    statement_token_push(parser, token);
                }
                else if (token->identifier == kCEETokenID_AT_IMPLEMENTATION) {
                    current_statement_type_transform(parser, kCEESourceFregmentTypeImplementationDefinition);
                    statement_token_push(parser, token);
                }
                else if (token->identifier == kCEETokenID_AT_PROTOCOL) {
                    current_statement_type_transform(parser, kCEESourceFregmentTypeProtocolDeclaration);
                    statement_token_push(parser, token);
                }
                else if (token->identifier == kCEETokenID_TEMPLATE) {
                    current_statement_type_transform(parser, kCEESourceFregmentTypeTemplateDeclaration);
                    statement_token_push(parser, token);
                }
                else if (token->identifier == kCEETokenID_AT_END) {
                    if (!block_pop(parser))
                        parser_state_clear(parser, kCParserStateStatementParsing);
        
                    statement_token_push(parser, token);
        
                    if (!block_reduce(parser))
                        parser_state_clear(parser, kCParserStateStatementParsing);
                }
                else if (token->identifier == '{' ||
                         token->identifier == kCEETokenID_AT_BRACE ||     /** @{ */
                         token->identifier == kCEETokenID_CARAT_BRACE) {  /** ^{ */
                    statement_token_push(parser, token);
        
                    block_header_parse(parser);
                    block_push(parser);
                }
                else if (token->identifier == '}') {
                    block_parse(parser);
                    if (!block_pop(parser))
                        parser_state_clear(parser, kCParserStateStatementParsing);
        
                    statement_token_push(parser, token);
        
                    if (!block_reduce(parser))
                        parser_state_clear(parser, kCParserStateStatementParsing);
                }
                else if (token->identifier == '[' ||
                         token->identifier == kCEETokenID_AT_BRACKET) {   /** @[ */
                    statement_token_push(parser, token);
                    subscript_push(parser);
                }
                else if (token->identifier == ']') {
                    if (!subscript_pop(parser))
                        parser_state_clear(parser, kCParserStateStatementParsing);
        
                    statement_token_push(parser, token);
                }
                else if (token->identifier == '(') {
                    statement_token_push(parser, token);
                    parameter_list_push(parser);
                }
                else if (token->identifier == ')') {
                    if (!parameter_list_pop(parser))
                        parser_state_clear(parser, kCParserStateStatementParsing);
        
                    statement_token_push(parser, token);
                }
                else if (token->identifier == ':') {
                    statement_token_push(parser, token);
                    if (colon_parse(parser))
                        statement_reduce(parser);
                }
                else if (token->identifier == ';') {
                    statement_token_push(parser, token);
                    statement_parse(parser);
                    statement_reduce(parser);
                }
                else if (token->identifier == kCEETokenID_AT_PUBLIC ||
                         token->identifier == kCEETokenID_AT_PRIVATE ||
                         token->identifier == kCEETokenID_AT_PROTECTED) {
                    statement_token_push(parser, token);
                    parser_access_level_transform(parser, token);
                    statement_reduce(parser);
                }
                else {
                    statement_token_push(parser, token);
                }
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
    cee_lexer_c_buffer_free();
        
    return TRUE;
}

static void symbol_parse_clear(CParser* parser)
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
    
    for (cee_int i = kCEESourceFregmentIndexPrepDirective; i < kCEESourceFregmentIndexMax; i ++) {
        
        if (!indexes[i])
            continue;
        
        p = indexes[i]->node_ref;
        while (p) {
            fregment = p->data;
            if (!references_in_source_fregment_parse(parser_ref,
                                                     filepath,
                                                     fregment, 
                                                     subject, 
                                                     prep_directive,
                                                     statement,
                                                     range, 
                                                     references))
                break;
            p = SOURCE_FREGMENT_NEXT(p);
        }
                
        while (fregment && !cee_source_fregment_over_range(fregment, range)) {
            fregment = cee_source_fregment_grandfather_get(fregment);
            if (!fregment || !fregment->node_ref ||
                !SOURCE_FREGMENT_HAS_NEXT(fregment->node_ref))
                break;
            
            p = SOURCE_FREGMENT_NEXT(fregment->node_ref);
            while (p) {
                fregment = p->data;
                if (!references_in_source_fregment_parse(parser_ref,
                                                         filepath,
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
    }
    
    return TRUE;
}

static cee_boolean references_in_source_fregment_parse(CEESourceParserRef parser_ref,
                                                       const cee_char* filepath,
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
        
    reference_fregment = c_referernce_fregment_convert(fregment, subject);
    c_reference_fregment_parse(parser_ref,
                               filepath,
                               reference_fregment, 
                               subject, 
                               prep_directive,
                               statement,
                               references);
    cee_source_fregment_free(reference_fregment);
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        if (!references_in_source_fregment_parse(parser_ref,
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

static CEESourceFregment* c_referernce_fregment_convert(CEESourceFregment* fregment,
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
                                                        "c");
        reference_fregment = cee_source_fregment_sub_attach(reference_fregment, 
                                                            kCEESourceFregmentTypeStatement,
                                                            fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            "c");
    }
    else {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeStatement,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        "c");
    }
    
    current = reference_fregment;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    
    while (p) {
        token = p->data;
        
        if (!(token->state & kCEETokenStateSymbolOccupied)) {
            if (token->identifier == '{' || 
                token->identifier == kCEETokenID_AT_BRACE ||      /** @{ */
                token->identifier == kCEETokenID_CARAT_BRACE) {   /** ^{ */
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fregment_push(current,
                                                   kCEESourceFregmentTypeCurlyBracketList,
                                                   fregment->filepath_ref,
                                                   fregment->subject_ref,
                                                   "c");
            }
            else if (token->identifier == '}') {
                current = cee_source_fregment_pop(current);
                if (!current)
                    break;
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
            }
            else if (token->identifier == '[' || 
                     token->identifier == kCEETokenID_AT_BRACKET) {   /** @[ */
                SOURCE_FREGMENT_TOKEN_PUSH(current, token, FALSE);
                current = cee_source_fregment_push(current, 
                                                   kCEESourceFregmentTypeSquareBracketList,
                                                   fregment->filepath_ref,
                                                   fregment->subject_ref,
                                                   "c");
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
                                                   "c");
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

static void c_reference_fregment_parse(CEESourceParserRef parser_ref,
                                       const cee_char* filepath,
                                       CEESourceFregment* fregment,
                                       const cee_char* subject,
                                       CEESourceFregment* prep_directive,
                                       CEESourceFregment* statement,
                                       CEEList** references)
{
    CEEList* p = NULL;
    CEESourceSymbolReference* reference = NULL;
    CEEList* sub = NULL;
    CEEList* objective_c_message = NULL;
    CEESourceReferenceType type = kCEESourceReferenceTypeUnknow;
    if (!fregment)
        return;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        
        type = kCEESourceReferenceTypeUnknow;
        
        if (cee_source_fregment_tokens_pattern_match(fregment, p, '.', kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeMember;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_MEMBER_POINTER, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeMember;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_SCOPE, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeMember;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_IDENTIFIER, ':',  NULL) &&
                 cee_source_fregment_parent_type_is(fregment, kCEESourceFregmentTypeSquareBracketList)) {
            /** catch objective-c message component */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(0, 2), &sub);
            objective_c_message = cee_list_concat(sub, objective_c_message);
            sub = NULL;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, '~', kCEETokenID_IDENTIFIER, '(', ')', NULL)) {
            /** catch desctructor */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(0, 2), &sub);
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch any other identifier */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(0, 1), &sub);
        }
        else {
            p = TOKEN_NEXT(p);
        }
        
        if (sub) {
            reference = cee_source_symbol_reference_create(parser_ref,
                                                           filepath,
                                                           subject, 
                                                           sub, 
                                                           type);
            *references = cee_list_prepend(*references, reference);
            reference = NULL;
            sub = NULL;
        }
    }
    
    if (objective_c_message) {
        reference = cee_source_symbol_reference_create(parser_ref,
                                                       filepath,
                                                       subject, 
                                                       objective_c_message, 
                                                       kCEESourceReferenceTypeUnknow);
        *references = cee_list_prepend(*references, reference);
        objective_c_message = NULL;
    }
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        c_reference_fregment_parse(parser_ref,
                                   filepath,
                                   p->data, 
                                   subject, 
                                   prep_directive, 
                                   statement, 
                                   references);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFregment* prep_directive,
                                          CEESourceFregment* statement,
                                          CEESourceSymbol** symbol)
{

    cee_boolean ret = FALSE;
    CEESourceSymbol* statement_symbol = NULL;
    CEESourceSymbol* prep_directive_symbol = NULL;
    CEEList* searched_symbols = NULL;
    CEESourceFregment* current = cee_source_fregment_from_reference_get(reference);
    
    if (!current || !symbol || reference->type == kCEESourceReferenceTypeMember)
        goto exit;
    
    *symbol = NULL;
    
    statement_symbol = symbol_search_in_scope_recursive(current, reference->name);
    if (statement_symbol)
        statement_symbol = cee_source_symbol_copy(statement_symbol);
    
    searched_symbols =
        cee_source_fregment_tree_symbols_search(prep_directive,
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

static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFregment* current,
                                                         const cee_char* name)
{
    CEEList* symbols = NULL;
    CEESourceSymbol* referenced = NULL;
    CEESourceFregment* searching = current;
    CEEList* p = NULL;
    
    if (!searching)
        goto exit;
    
    /** search current fregment */
    symbols = cee_source_fregment_symbols_search_by_name(searching, name);
    if (symbols) {
        referenced = cee_list_nth_data(symbols, 0);
        goto exit;
    }
    
    /**
     * Search current round bracket list child fregment,
     * function parameters , "for" / "while" / "if" statments
     */
    p = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
    while (p) {
        searching = p->data;
        if (searching->type[kCEESourceFregmentTypeRoundBracketList]) {
            symbols = cee_source_fregment_symbols_in_children_search_by_name(searching,
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
            symbols = cee_source_fregment_symbols_search_by_name(searching, name);
            if (symbols) {
                referenced = cee_list_nth_data(symbols, 0);
                goto exit;
            }
            
            /** enumerator search */
            if (searching->type[kCEESourceFregmentTypeEnumDefinition]) {
                CEEList* q = SOURCE_FREGMENT_CHILDREN_FIRST(searching);
                while (q) {
                    searching = q->data;
                    symbols = cee_source_fregment_symbols_in_children_search_by_name(searching,
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
    if (cee_source_fregment_parent_type_is(searching, kCEESourceFregmentTypeRoundBracketList) ||
        cee_source_fregment_parent_type_is(searching, kCEESourceFregmentTypeCurlyBracketList) ||
        cee_source_fregment_parent_type_is(searching, kCEESourceFregmentTypeSquareBracketList)) {
        searching = cee_source_fregment_grandfather_get(searching);
        referenced = symbol_search_in_scope_recursive(searching, name);
    }
    
exit:
    return referenced;
}
