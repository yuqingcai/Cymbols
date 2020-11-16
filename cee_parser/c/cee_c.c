#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_c.h"
#include "cee_symbol.h"
#include "cee_reference.h"

//#define DEBUG_PREP_DIRECTIVE
//#define DEBUG_CLASS
//#define DEBUG_UNION
//#define DEBUG_ENUM
//#define DEBUG_TEMPLATE
//#define DEBUG_MESSAGE_DEFINITION
//#define DEBUG_MESSAGE_DECLARATION
//#define DEBUG_PROPERTY_DECLARATION
//#define DEBUG_DECLARATION
//#define DEBUG_INTERFACE
//#define DEBUG_IMPLEMENTATION
//#define DEBUG_PROTOCOL
//#define DEBUG_NAMESPACE
//#define DEBUG_FUNCTION_DEFINITION
//#define DEBUG_LABEL
//#define DEBUG_PROTOS

typedef enum _ObjectiveCDefinitionType {
    kObjectiveCDefinitionTypeInit = 0,
    kObjectiveCDefinitionTypeInterface,
    kObjectiveCDefinitionTypeImplementation,
    kObjectiveCDefinitionTypeProtocol,
} ObjectiveCDefinitionType;

typedef enum _ObjectiveCMessageDefinitionTranslateState {
    kObjectiveCMessageDefinitionTranslateStateInit = 0,
    kObjectiveCMessageDefinitionTranslateStateMessage,
    kObjectiveCMessageDefinitionTranslateStateReturnType,
    kObjectiveCMessageDefinitionTranslateStateReturnTypeEnd,
    kObjectiveCMessageDefinitionTranslateStateComponent,
    kObjectiveCMessageDefinitionTranslateStateColon,
    kObjectiveCMessageDefinitionTranslateStateParameterType,
    kObjectiveCMessageDefinitionTranslateStateParameterTypeEnd,
    kObjectiveCMessageDefinitionTranslateStateParameter,
    kObjectiveCMessageDefinitionTranslateStateCommit,
    kObjectiveCMessageDefinitionTranslateStateError,
    kObjectiveCMessageDefinitionTranslateStateMax
} ObjectiveCMessageDefinitionTranslateState;

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

typedef enum _ObjectiveCPropertyDeclarationTranslateState {
    kObjectiveCPropertyDeclarationTranslateStateInit = 0,
    kObjectiveCPropertyDeclarationTranslateStateProperty,
    kObjectiveCPropertyDeclarationTranslateStateAttribute,
    kObjectiveCPropertyDeclarationTranslateStateAttributeEnd,
    kObjectiveCPropertyDeclarationTranslateStateCommit,
    kObjectiveCPropertyDeclarationTranslateStateTerminate,
    kObjectiveCPropertyDeclarationTranslateStateError,
    kObjectiveCPropertyDeclarationTranslateStateMax,
} ObjectiveCPropertyDeclarationTranslateState;

typedef enum _ObjectiveCMessageDeclarationTranslateState {
    kObjectiveCMessageDeclarationTranslateStateInit = 0,
    kObjectiveCMessageDeclarationTranslateStateMessage,
    kObjectiveCMessageDeclarationTranslateStateReturnType, 
    kObjectiveCMessageDeclarationTranslateStateReturnTypeEnd, 
    kObjectiveCMessageDeclarationTranslateStateComponent, 
    kObjectiveCMessageDeclarationTranslateStateColon, 
    kObjectiveCMessageDeclarationTranslateStateParameterType, 
    kObjectiveCMessageDeclarationTranslateStateParameterTypeEnd, 
    kObjectiveCMessageDeclarationTranslateStateParameter, 
    kObjectiveCMessageDeclarationTranslateStateTerminate, 
    kObjectiveCMessageDeclarationTranslateStateError,
    kObjectiveCMessageDeclarationTranslateStateMax,
} ObjectiveCMessageDeclarationTranslateState;

typedef enum _CTemplateDeclarationTranslateState {
    kCTemplateDeclarationTranslateStateInit = 0,
    kCTemplateDeclarationTranslateStateCommit,
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
    kCFunctionDefinitionTranslateStateCommit,
    kCFunctionDefinitionTranslateStateTrailing,
    kCFunctionDefinitionTranslateStateTry,
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
    kCFunctionParametersDeclarationTranslateStateCommit,
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
    kCDeclarationTranslateStateThrow,
    kCDeclarationTranslateStateThrowList,
    kCDeclarationTranslateStateOperator,
    kCDeclarationTranslateStateOverloadOperator,
    kCDeclarationTranslateStateOverloadNew,
    kCDeclarationTranslateStateOverloadDelete,
    kCDeclarationTranslateStateOverloadType,
    kCDeclarationTranslateStateOverloadRoundBracket,
    kCDeclarationTranslateStateOverloadRoundBracketEnd,
    kCDeclarationTranslateStateOverloadTypeSurrounded,
    kCDeclarationTranslateStateOverloadTypeSurroundedEnd,
    kCDeclarationteStateTypeCommited,
    kCDeclarationTranslateStateTemplateParameter,
    kCDeclarationTranslateStateCommit,
    kCDeclarationTranslateStateBitField,
    kCDeclarationTranslateStateBitSize,
    kCDeclarationTranslateStateTrailing,
    kCDeclarationTranslateStateTypeInit,
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
    kCInheritanceDefinitionTranslateStateCommit,
    kCInheritanceDefinitionTranslateStateError,
    kCInheritanceDefinitionTranslateStateMax,
} CInheritanceDefinitionTranslateState;

typedef enum _CPrepDirectiveIncludeTranslateState {
    kCPrepDirectiveIncludeTranslateStateInit = 0,
    kCPrepDirectiveIncludeTranslateStateDirective,
    kCPrepDirectiveIncludeTranslateStatePath,
    kCPrepDirectiveIncludeTranslateStateCommit,
    kCPrepDirectiveIncludeTranslateStateError,
    kCPrepDirectiveIncludeTranslateStateMax,
} CPrepDirectiveIncludeTranslateState;

typedef enum _CPrepDirectiveDefineTranslateState {
    kCPrepDirectiveDefineTranslateStateInit = 0,
    kCPrepDirectiveDefineTranslateStateDirective,
    kCPrepDirectiveDefineTranslateStateDefineName,
    kCPrepDirectiveDefineTranslateStateParameterList,
    kCPrepDirectiveDefineTranslateStateParameter,
    kCPrepDirectiveDefineTranslateStateCommit,
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
    kCNamespaceDefinitionTranslateStateCommit,
    kCNamespaceDefinitionTranslateStateError,
    kCNamespaceDefinitionTranslateStateMax,
} CNamespaceDefinitionTranslateState;

typedef enum _CExternBlockTranslateState {
    kCExternBlockTranslateStateInit = 0,
    kCExternBlockTranslateStateExtern,
    kCExternBlockTranslateStateIdentifier,
    kCExternBlockTranslateStateCommit,
    kCExternBlockTranslateStateError,
    kCExternBlockTranslateStateMax,
} CExternBlockTranslateState;

typedef enum _CReferenceTranslateState {
    kCReferenceTranslateStateInit = 0,
    kCReferenceTranslateStateIdentifier,
    kCReferenceTranslateStateTilde,
    kCReferenceTranslateStateFunction,
    kCReferenceTranslateStateCommit,
    kCReferenceTranslateStateError,
    kCReferenceTranslateStateMax,
} CReferenceTranslateState;

typedef enum _CProtosTranslateState {
    kCProtosTranslateStateInit = 0,
    kCProtosTranslateStateAccessSpecifier,
    kCProtosTranslateStateDecltype,
    kCProtosTranslateStateDecltypeList,
    kCProtosTranslateStateDecltypeCommit,
    kCProtosTranslateStateCustomType,
    kCProtosTranslateStateCustomTypeCommit,
    kCProtosTranslateStateTemplateParameter,
    kCProtosTranslateStateCustomTypeScope,
    kCProtosTranslateStateAlignasSpecifier,
    kCProtosTranslateStateAlignasList,
    kCProtosTranslateStateAlignasListEnd,
    kCProtosTranslateStateObjectSpecifier,
    kCProtosTranslateStateObjectIdentifier,
    kCProtosTranslateStateObjectIdentifierCommit,
    kCProtosTranslateStateObjectIdentifierScope,
    kCProtosTranslateStateObjectDerive,
    kCProtosTranslateStateObjectDeriveIdentifier,
    kCProtosTranslateStateObjectDeriveIdentifierScope,
    kCProtosTranslateStateObjectDefinition,
    kCProtosTranslateStateObjectDefinitionCommit,
    kCProtosTranslateStateBuiltinCommit,
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
    kObjectiveCEnumDefinitionTranslateStateCommit,
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
    const cee_uchar* filepath_ref;
    const cee_uchar* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* prep_directive_root;
    CEESourceFregment* prep_directive_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
    ParseTrap block_header_traps[CEETokenID_MAX];
    CParserState state;
    cee_int prep_if_directive;
} CParser;

static void c_token_type_map_init(void);
static CEETokenIdentifierType c_token_identifier_type_map[CEETokenID_MAX];
static cee_short objective_c_message_definition_translate_table[kObjectiveCMessageDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_short objective_c_definition_translate_table[kObjectiveCDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_short objective_c_property_declaration_translate_table[kObjectiveCPropertyDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_short objective_c_message_declaration_translate_table[kObjectiveCMessageDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_short c_template_declaration_translate_table[kCTemplateDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_short c_function_definition_translate_table[kCFunctionDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_short c_function_parameters_declaration_translate_table[kCFunctionParametersDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_short c_declaration_translate_table[kCDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_short c_inheritance_definition_translate_table[kCInheritanceDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_short c_prep_directive_include_translate_table[kCPrepDirectiveIncludeTranslateStateMax][CEETokenID_MAX];
static cee_short c_prep_directive_define_translate_table[kCPrepDirectiveDefineTranslateStateMax][CEETokenID_MAX];
static cee_short c_namespace_definition_translate_table[kCNamespaceDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_short c_protos_translate_table[kCProtosTranslateStateMax][CEETokenID_MAX];
static cee_short c_extern_block_translate_table[kCExternBlockTranslateStateMax][CEETokenID_MAX];
static cee_short objective_c_enum_definition_translate_table[kObjectiveCEnumDefinitionTranslateStateMax][CEETokenID_MAX];

static cee_boolean token_id_is_prep_directive(CEETokenID identifier);
static cee_boolean token_id_is_prep_directive_condition(CEETokenID identifier);
static cee_boolean token_id_is_keyword(CEETokenID identifier);
static cee_boolean token_id_is_punctuation(CEETokenID identifier);
static cee_boolean token_id_is_assignment(CEETokenID identifier);
static cee_boolean token_id_is_builtin_type(CEETokenID identifier);
static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier);
static cee_boolean token_id_is_access_specifier(CEETokenID identifier);
static cee_boolean token_id_is_alignas_specifier(CEETokenID identifier);
static cee_boolean token_id_is_object_sepcifier(CEETokenID identifier);
static cee_boolean token_id_is_virtual_sepcifier(CEETokenID identifier);
static cee_boolean is_name_scope(CEEList* p);
static cee_boolean statement_attach(CParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(CParser* parser,
                                        CEESourceFregmentType type);
static cee_boolean statement_pop(CParser* parser);
static cee_boolean prep_directive_attach(CParser* parser,
                                         CEESourceFregmentType type);
static cee_boolean prep_directive_sub_attach(CParser* parser,
                                             CEESourceFregmentType type);
static void prep_directive_branch_push(CParser* parser);
static cee_boolean prep_directive_branch_pop(CParser* parser);
static cee_boolean prep_directive_pop(CParser* parser);
static cee_boolean comment_attach(CParser* parser);
static cee_boolean prep_directive_terminated(CParser* parser);
static cee_boolean statement_parsing(CParser* parser);
static void c_prep_directive_include_translate_table_init(void);
static cee_boolean prep_directive_include_parse(CEESourceFregment* fregment);
static void c_prep_directive_define_translate_table_init(void);
static cee_boolean prep_directive_define_parse(CEESourceFregment* fregment);
static cee_boolean prep_directive_common_parse(CEESourceFregment* fregment);
static void prep_directive_parse(CParser* parser);
static cee_boolean prep_directive_reduce(CParser* parser);
static void block_push(CParser* parser);
static cee_boolean block_pop(CParser* parser);
static void block_parse(CParser* parser);
static void c_template_declaration_translate_table_init(void);
static cee_boolean c_template_token_push(CParser* parser,
                                         CEEToken* push);
static void c_template_parameters_declaration_translate_table_init(void);
static cee_boolean c_template_parameters_parse(CEESourceFregment* fregment);
static void template_push(CParser* parser);
static cee_boolean template_pop(CParser* parser);
static void c_inheritance_definition_translate_table_init(void);
static cee_boolean c_inheritance_parse(CEEList* tokens,
                                       CEEList** inherited,
                                       CEEList** derives);
CEESourceSymbol* c_inheritance_definition_create(CEEList* tokens,
                                                 const cee_uchar* filepath,
                                                 const cee_uchar* subject);
static cee_char* c_name_scope_list_string_create(CEEList* scopes,
                                                 const cee_uchar* subject);
static cee_char* c_name_scope_create(CEEList* tokens,
                                     const cee_uchar* subject);
static CEEList* c_name_scope_head_get(CEEList* p,
                                      const cee_uchar* subject);
static void derived_chain_free(cee_pointer data);
static cee_boolean c_class_definition_trap(CEESourceFregment* fregment,
                                           CEEList** pp);
static cee_boolean c_enum_definition_trap(CEESourceFregment* fregment,
                                          CEEList** pp);
static CEEList* enumerators_extract(CEEList* tokens,
                                    const cee_uchar* filepath,
                                    const cee_uchar* subject);
static cee_boolean c_union_definition_trap(CEESourceFregment* fregment,
                                           CEEList** pp);
static cee_boolean objective_c_enum_definition_trap(CEESourceFregment* fregment,
                                                    CEEList** pp);
static cee_boolean c_statement_block_parse(CEESourceFregment* fregment);
static void c_function_definition_translate_table_init(void);
static cee_boolean c_function_definition_parse(CEESourceFregment* fregment);
static void c_function_parameters_declaration_translate_table_init(void);
static cee_boolean c_function_parameters_parse(CEESourceFregment* fregment);
static CEESourceSymbol* c_function_parameter_identifier_create(CEESourceFregment* fregment,
                                                               CEEList* head,
                                                               CEEList* begin,
                                                               CEEList* end);
static CEESourceSymbol* c_function_parameter_surrounded_create(CEESourceFregment* fregment,
                                                               CEEList* head,
                                                               CEEList* surrounded);
static void objective_c_message_definition_translate_table_init(void);
static cee_boolean objective_c_message_definition_parse(CEESourceFregment* fregment);
static cee_char* objective_c_message_protos_dump(CEESourceFregment* fregment);
static cee_char* objective_c_message_parameter_protos_dump(CEESourceFregment* fregment,
                                                           cee_int parameter_index);
static void c_protos_translate_table_init(void);
static cee_char* c_protos_string_from_token_slice(const cee_uchar* subject,
                                                  CEEList* p,
                                                  CEEList* q);
static CEESourceSymbol* objective_c_interface_extract(CEEList* tokens,
                                                      const cee_uchar* filepath,
                                                      const cee_uchar* subject);
static CEESourceSymbol* objective_c_implementation_extract(CEEList* tokens,
                                                           const cee_uchar* filepath,
                                                           const cee_uchar* subject);
static CEESourceSymbol* objective_c_protocol_extract(CEEList* tokens,
                                                     const cee_uchar* filepath,
                                                     const cee_uchar* subject);
static cee_char* c_name_list_create(CEEList* tokens,
                                    const cee_uchar* subject);
static void objective_c_property_declaration_translate_table_init(void);
static cee_boolean objective_c_property_declaration_parse(CEESourceFregment* fregment);
static void objective_c_message_declaration_translate_table_init(void);
static cee_boolean objective_c_message_declaration_parse(CEESourceFregment* fregment);
static void c_declaration_translate_table_init(void);
static CEEList* skip_template_parameter_list(CEEList* p,
                                             cee_boolean reverse);
static CEEList* skip_include_path(CEEList* p);
static CEEList* skip_c_declaration_interval(CEEList* p);
static cee_boolean c_declaration_parse(CEESourceFregment* fregment);
static cee_char* c_declaration_protos_create(CEESourceFregment* fregment,
                                             CEEList* overload,
                                             CEEList* surrounded,
                                             CEEList* parameter_list,
                                             CEEList* identifier,
                                             CEEList* object_identifier);
static CEESourceSymbol* c_operator_overload_declaration_create(CEESourceFregment* fregment,
                                                               const cee_char* proto,
                                                               CEEList* overload,
                                                               CEEList* parameter_list);
static CEESourceSymbol* c_surrounded_declaration_create(CEESourceFregment* fregment,
                                                        const cee_char* proto,
                                                        CEEList* surrounded);
static CEESourceSymbol* c_function_declaration_create(CEESourceFregment* fregment,
                                                      const cee_char* proto,
                                                      CEEList* parameter_list);
static CEESourceSymbol* c_custom_type_declaration_create(CEESourceFregment* fregment,
                                                         const cee_char* proto,
                                                         CEEList* identifier);
static CEESourceSymbol* c_identifier_declaration_create(CEESourceFregment* fregment,
                                                        const cee_char* proto,
                                                        CEEList* identifier);
static void c_namespace_definition_translate_table_init(void);
static cee_boolean c_namespace_definition_trap(CEESourceFregment* fregment,
                                               CEEList** pp);
static void c_extern_block_translate_table_init(void);
static cee_boolean c_extern_block_parse(CEESourceFregment* fregment);
static void block_header_parse(CParser* parser);
static cee_boolean block_reduce(CParser* parser);
static void label_parse(CParser* parser);
static cee_boolean label_reduce(CParser* parser);
static void parameter_list_push(CParser* parser);
static cee_boolean parameter_list_pop(CParser* parser);
static void subscript_push(CParser* parser);
static cee_boolean subscript_pop(CParser* parser);
static void objective_c_definition_translate_table_init(void);
static cee_boolean objective_c_token_push(CParser* parser,
                                          CEEToken* push);
static cee_boolean objective_c_definition_parse(CEESourceFregment* fregment,
                                                const cee_uchar* subject);
static void statement_parse(CParser* parser);
static cee_boolean statement_reduce(CParser* parser);
static void parser_block_header_trap_init(CParser* parser);
static CParser* parser_create(void);
static void parser_free(cee_pointer data);
static void symbol_parse_init(CParser* parser,
                              const cee_uchar* filepath,
                              const cee_uchar* subject);
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
static CEESourceFregment* c_referernce_fregment_convert(CEESourceFregment* fregment,
                                                        const cee_uchar* subject);
static void c_reference_fregment_parse(const cee_uchar* filepath,
                                       CEESourceFregment* fregment,
                                       const cee_uchar* subject,
                                       CEESourceFregment* prep_directive,
                                       CEESourceFregment* statement,
                                       CEEList** references);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenIdentifierType type);

static void c_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        c_token_identifier_type_map[i] = 0;
    
    c_token_identifier_type_map[kCEETokenID_LINE_COMMENT]                 = kCEETokenIdentifierTypeComment;
    c_token_identifier_type_map[kCEETokenID_LINES_COMMENT]                = kCEETokenIdentifierTypeComment;
    c_token_identifier_type_map[kCEETokenID_CONST]                        = kCEETokenIdentifierTypeConstant;
    c_token_identifier_type_map[kCEETokenID_CHARACTER]                    = kCEETokenIdentifierTypeCharacter;
    c_token_identifier_type_map[kCEETokenID_LITERAL]                      = kCEETokenIdentifierTypeLiteral;
    c_token_identifier_type_map[kCEETokenID_HASH_INCLUDE]                 = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_IMPORT]                  = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_DEFINE]                  = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_UNDEF]                   = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_IF]                      = kCEETokenIdentifierTypePrepDirective | kCEETokenIdentifierTypePrepDirectiveCondition;
    c_token_identifier_type_map[kCEETokenID_HASH_IFDEF]                   = kCEETokenIdentifierTypePrepDirective | kCEETokenIdentifierTypePrepDirectiveCondition;
    c_token_identifier_type_map[kCEETokenID_HASH_IFNDEF]                  = kCEETokenIdentifierTypePrepDirective | kCEETokenIdentifierTypePrepDirectiveCondition;
    c_token_identifier_type_map[kCEETokenID_HASH_ENDIF]                   = kCEETokenIdentifierTypePrepDirective | kCEETokenIdentifierTypePrepDirectiveCondition;
    c_token_identifier_type_map[kCEETokenID_HASH_ELSE]                    = kCEETokenIdentifierTypePrepDirective | kCEETokenIdentifierTypePrepDirectiveCondition;
    c_token_identifier_type_map[kCEETokenID_HASH_ELIF]                    = kCEETokenIdentifierTypePrepDirective | kCEETokenIdentifierTypePrepDirectiveCondition;
    c_token_identifier_type_map[kCEETokenID_HASH_LINE]                    = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_ERROR]                   = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_WARNING]                 = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_PRAGMA]                  = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map[kCEETokenID_HASH_UNKNOW]                  = kCEETokenIdentifierTypePrepDirective;
    c_token_identifier_type_map['=']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map['+']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['-']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['*']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['/']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['\\']                                     = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map['%']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['~']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['&']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['|']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['^']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['!']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['<']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['>']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['.']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[',']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['?']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[':']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map['(']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[')']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map['{']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map['}']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map['[']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[']']                                      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map['#']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[';']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map['@']                                      = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[kCEETokenID_ADD_ASSIGNMENT]               = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_MINUS_ASSIGNMENT]             = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_MULTI_ASSIGNMENT]             = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_DIV_ASSIGNMENT]               = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_MOD_ASSIGNMENT]               = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_AND_ASSIGNMENT]               = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_OR_ASSIGNMENT]                = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_XOR_ASSIGNMENT]               = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_LEFT_OFFSET_ASSIGNMENT]       = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_RIGHT_OFFSET_ASSIGNMENT]      = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator | kCEETokenIdentifierTypeAssignment;
    c_token_identifier_type_map[kCEETokenID_INCREMENT]                    = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_DECREMENT]                    = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_LOGIC_AND]                    = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_LOGIC_OR]                     = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_LOGIC_EQUAL]                  = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_LOGIC_UNEQUAL]                = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_LOGIC_LESS_EQUAL]             = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]            = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_MEMBER_POINTER]               = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_COMPARISON]                   = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_POINTER_MEMBER_POINTER]       = kCEETokenIdentifierTypePunctuation | kCEETokenIdentifierTypeOverloadOperator;
    c_token_identifier_type_map[kCEETokenID_TOKEN_PASTING]                = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[kCEETokenID_ELLIPSIS]                     = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[kCEETokenID_SCOPE]                        = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[kCEETokenID_AT_BRACE]                     = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[kCEETokenID_AT_BRACKET]                   = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[kCEETokenID_CARAT_BRACE]                  = kCEETokenIdentifierTypePunctuation;
    c_token_identifier_type_map[kCEETokenID_ALIGNAS]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAlignasSpecifier;
    c_token_identifier_type_map[kCEETokenID_ALIGNOF]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AND]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AND_EQ]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ASM]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ATOMIC_CANCEL]                = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ATOMIC_COMMIT]                = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ATOMIC_NOEXCEPT]              = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AUTO]                         = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_BITAND]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_BITOR]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_BOOL]                         = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_BREAK]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_CASE]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_CATCH]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_CHAR]                         = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_CHAR16_T]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_CHAR32_T]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_CLASS]                        = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeObjectSpecifier;
    c_token_identifier_type_map[kCEETokenID_COMPL]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_CONCEPT]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_CONST]                        = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_CONSTEVAL]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_CONSTEXPR]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_CONST_CAST]                   = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_CONTINUE]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_DECLTYPE]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_DEFAULT]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_DELETE]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_DO]                           = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_DOUBLE]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_DYNAMIC_CAST]                 = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ELSE]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ENUM]                         = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeObjectSpecifier;
    c_token_identifier_type_map[kCEETokenID_EXPLICIT]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_EXPORT]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_EXTERN]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_FALSE]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_FLOAT]                        = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_FOR]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_FRIEND]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_GOTO]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_IF]                           = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_IMPORT]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_INLINE]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_INT]                          = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_LONG]                         = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_MODULE]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_MUTABLE]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_NAMESPACE]                    = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NEW]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NOEXCEPT]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NOT]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NOT_EQ]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NULLPTR]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_OPERATOR]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_OR]                           = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_OR_EQ]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_PRIVATE]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAccessSpecifier;
    c_token_identifier_type_map[kCEETokenID_PROTECTED]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAccessSpecifier;
    c_token_identifier_type_map[kCEETokenID_PUBLIC]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAccessSpecifier;
    c_token_identifier_type_map[kCEETokenID_REGISTER]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_REINTERPRET_CAST]             = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_REQUIRES]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_RESTRICT]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_RETURN]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_SHORT]                        = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIZEOF]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_STATIC]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_STATIC_ASSERT]                = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_STATIC_CAST]                  = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_STRUCT]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeObjectSpecifier;
    c_token_identifier_type_map[kCEETokenID_SWITCH]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_SYNCHRONIZED]                 = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_TEMPLATE]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_THIS]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_THREAD_LOCAL]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_THROW]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_TRUE]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_TRY]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_TYPEDEF]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_TYPEID]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_TYPENAME]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_UNION]                        = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeObjectSpecifier;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_USING]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_VIRTUAL]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_VOLATILE]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_WCHAR_T]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_WHILE]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_XOR]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_XOR_EQ]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_OVERRIDE]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeVirtualSpecifier;
    c_token_identifier_type_map[kCEETokenID_FINAL]                        = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeVirtualSpecifier;
    c_token_identifier_type_map[kCEETokenID_TRANSACTION_SAFE]             = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_TRANSACTION_SAFE_DYNAMIC]     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID__ALIGNAS]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAlignasSpecifier;
    c_token_identifier_type_map[kCEETokenID__ALIGNOF]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID__ATOMIC]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID__BOOL]                        = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID__COMPLEX]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID__GENERIC]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID__IMAGINARY]                   = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID__NORETURN]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID__STATIC_ASSERT]               = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID__THREAD_LOCAL]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_VOID]                         = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_LONG_DOUBLE]                  = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_CHAR]                  = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_CHAR]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SHORT_INT]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_SHORT]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_SHORT_INT]             = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_SHORT]               = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_SHORT_INT]           = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_INT]                   = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_INT]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_LONG_INT]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_LONG]                  = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_LONG_INT]              = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_LONG]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_LONG_INT]            = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_LONG_LONG]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_LONG_LONG_INT]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_LONG_LONG]             = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SIGNED_LONG_LONG_INT]         = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_LONG_LONG]           = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UNSIGNED_LONG_LONG_INT]       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT8_T]                       = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT16_T]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT32_T]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT64_T]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_FAST8_T]                  = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_FAST16_T]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_FAST32_T]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_FAST64_T]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_LEAST8_T]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_LEAST16_T]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_LEAST32_T]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INT_LEAST64_T]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INTMAX_T]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INTPTR_T]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT8_T]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT16_T]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT32_T]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT64_T]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_FAST8_T]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_FAST16_T]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_FAST32_T]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_FAST64_T]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_LEAST8_T]                = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_LEAST16_T]               = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_LEAST32_T]               = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINT_LEAST64_T]               = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINTMAX_T]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_UINTPTR_T]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_AT_INTERFACE]                 = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_IMPLEMENTATION]            = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_PROTOCOL]                  = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_END]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_CLASS]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_PUBLIC]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAccessSpecifier;
    c_token_identifier_type_map[kCEETokenID_AT_PACKAGE]                   = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_PROTECTED]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAccessSpecifier;
    c_token_identifier_type_map[kCEETokenID_AT_PRIVATE]                   = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeAccessSpecifier;
    c_token_identifier_type_map[kCEETokenID_AT_PROPERTY]                  = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_SYNTHESIZE]                = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_DYNAMIC]                   = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_REQUIRED]                  = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_OPTIONAL]                  = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_TRY]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_CATCH]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_FINALLY]                   = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_THROW]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_SELECTOR]                  = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_ENCODE]                    = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_DEFS]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_AUTORELEASEPOOL]           = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_SYNCHRONIZED]              = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_AT_COMPATIBILITY_ALIAS]       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ID]                           = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_INSTANCETYPE]                 = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_IMP]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_IN]                           = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_INOUT]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NIL]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ONEWAY]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_OUT]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_SEL]                          = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeBuildinType;
    c_token_identifier_type_map[kCEETokenID_SELF]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_SUPER]                        = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_YES]                          = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NO]                           = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ATOMIC]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NONATOMIC]                    = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_STRONG]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_WEAK]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_ASSIGN]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_COPY]                         = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_RETAIN]                       = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_READONLY]                     = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_READWRITE]                    = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID___BRIDGE]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID___BRIDGE_TRANSFER]            = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID___BRIDGE_RETAINED]            = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID___BLOCK]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID__NONNULL]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID__NULLABLE]                    = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_NONNULL]                      = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID_NULLABLE]                     = kCEETokenIdentifierTypeKeyword | kCEETokenIdentifierTypeDeclarationSpecifier;
    c_token_identifier_type_map[kCEETokenID___KIND_OF]                    = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NS_ENUM]                      = kCEETokenIdentifierTypeKeyword;
    c_token_identifier_type_map[kCEETokenID_NS_OPTIONS]                   = kCEETokenIdentifierTypeKeyword;
}

static cee_boolean token_id_is_prep_directive(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypePrepDirective) != 0;
}

static cee_boolean token_id_is_prep_directive_condition(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypePrepDirectiveCondition) != 0;
}

static cee_boolean token_id_is_keyword(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeKeyword) != 0;
}

static cee_boolean token_id_is_punctuation(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypePunctuation) != 0;
}

static cee_boolean token_id_is_assignment(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeAssignment) != 0;
}

static cee_boolean token_id_is_builtin_type(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeBuildinType) != 0;
}

static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeDeclarationSpecifier) != 0;
}

static cee_boolean token_id_is_access_specifier(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeAccessSpecifier) != 0;
}

static cee_boolean token_id_is_alignas_specifier(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeAlignasSpecifier) != 0;
}

static cee_boolean token_id_is_object_sepcifier(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeObjectSpecifier) != 0;
}

static cee_boolean token_id_is_virtual_sepcifier(CEETokenID identifier) 
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeVirtualSpecifier) != 0;   
}
static cee_boolean token_id_is_overload_operator(CEETokenID identifier)
{
    return (c_token_identifier_type_map[identifier] & kCEETokenIdentifierTypeOverloadOperator) != 0;
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
                                          (const cee_uchar*)"c");
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
                                              (const cee_uchar*)"c");
    if (!attached)
        return FALSE;
    
    parser->prep_directive_current = attached;
    return TRUE;
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
            parser->prep_if_directive ++;
        }
        else if (token->identifier == kCEETokenID_HASH_ELIF ||
                 token->identifier == kCEETokenID_HASH_ELSE) {
            if (parser->state & kCParserStateStatementParsing)
                parser->state &= ~kCParserStateStatementParsing;
        }
        else if (token->identifier == kCEETokenID_HASH_ENDIF) {
            parser->prep_if_directive --;
            if (!parser->prep_if_directive) {
                if (!(parser->state & kCParserStateStatementParsing))
                    parser->state |= kCParserStateStatementParsing;
            }
        }
        else if (token->identifier == kCEETokenID_HASH_INCLUDE ||
                 token->identifier == kCEETokenID_HASH_IMPORT) {
            prep_directive_include_parse(fregment);
        }
        else if (token->identifier == kCEETokenID_HASH_DEFINE) {
            prep_directive_define_parse(fregment);
        }
        else {
            prep_directive_common_parse(fregment);
        }
    }
}

static cee_boolean prep_directive_reduce(CParser* parser)
{
    if (!parser->prep_directive_current)
        return FALSE;
    
    prep_directive_attach(parser, kCEESourceFregmentTypePrepDirective);
    return TRUE;
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
                                          (const cee_uchar*)"c");
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
                                              (const cee_uchar*)"c");
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
    
    if (objective_c_property_declaration_parse(current) ||
        objective_c_message_declaration_parse(current))
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
 *  comment
 */
static cee_boolean comment_attach(CParser* parser)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->comment_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->comment_current, 
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          (const cee_uchar*)"c");
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

static void c_prep_directive_include_translate_table_init(void)
{
    /**
     *                      include             import              identifier  literal  \                  <           >       others
     *  Init                IncludeDirective    IncludeDirective    Error       Error    Error              Error       Error   Error
     *  IncludeDirective    Error               Error               Commit      Commit   IncludeDirective   *PathBegin  Error   Error
     *  
     *  PathBegin, skip to Commit
     */
    TRANSLATE_STATE_INI(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateMax             , kCPrepDirectiveIncludeTranslateStateError                                               );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateInit            , kCEETokenID_HASH_INCLUDE                , kCPrepDirectiveIncludeTranslateStateDirective );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateInit            , kCEETokenID_HASH_IMPORT                 , kCPrepDirectiveIncludeTranslateStateDirective );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective       , kCEETokenID_IDENTIFIER                  , kCPrepDirectiveIncludeTranslateStateCommit    );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective       , kCEETokenID_LITERAL                     , kCPrepDirectiveIncludeTranslateStateCommit    );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective       , '\\'                                    , kCPrepDirectiveIncludeTranslateStateDirective );
    TRANSLATE_STATE_SET(c_prep_directive_include_translate_table,   kCPrepDirectiveIncludeTranslateStateDirective       , '<'                                     , kCPrepDirectiveIncludeTranslateStatePath      );
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
                current = kCPrepDirectiveIncludeTranslateStateCommit;
            }
            break;
        }
        
        if (current == kCPrepDirectiveIncludeTranslateStateCommit) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) ||
                cee_token_is_identifier(p, kCEETokenID_LITERAL))
                r = t = p;
            q = p;
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCPrepDirectiveIncludeTranslateStateCommit && s && q) {
        include_directive = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                      fregment->subject_ref,
                                                                      s, 
                                                                      q, 
                                                                      kCEESourceSymbolTypePrepDirectiveInclude, 
                                                                      "c");
        cee_source_symbol_name_format(include_directive->name);
        cee_token_slice_state_mark(TOKEN_NEXT(s), q, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, include_directive);
        
        if (r && t) {
            include_path = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     r,
                                                                     t,
                                                                     kCEESourceSymbolTypePrepDirectiveIncludePath,
                                                                     "c");
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

static void c_prep_directive_define_translate_table_init(void)
{
    /**
     *                      define              identifier      (               )                   ,               \               others
     *  Init                DefineDirective     Error           Error           Error               Error           Error           Error
     *  DefineDirective     Error               DefineName      Error           Error               Error           DefineDirective Error
     *  DefineName          Error               Commit          ParameterList   Error               Error           DefineName      Commit
     *  ParameterList       Error               Parameter       Error           Commit              Error           ParameterList   Error
     *  Parameter           Error               Error           Error           Commit              ParameterList   Parameter       Error
     */
    TRANSLATE_STATE_INI(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateMax              , kCPrepDirectiveDefineTranslateStateError                                                      );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateInit             , kCEETokenID_HASH_DEFINE                   , kCPrepDirectiveDefineTranslateStateDirective      );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDirective        , kCEETokenID_IDENTIFIER                    , kCPrepDirectiveDefineTranslateStateDefineName     );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDirective        , '\\'                                      , kCPrepDirectiveDefineTranslateStateDirective      );
    TRANSLATE_STATE_ANY(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDefineName       , kCPrepDirectiveDefineTranslateStateCommit                                                     );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDefineName       , '('                                       , kCPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateDefineName       , '\\'                                      , kCPrepDirectiveDefineTranslateStateDefineName     );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameterList    , kCEETokenID_IDENTIFIER                    , kCPrepDirectiveDefineTranslateStateParameter      );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameterList    , ')'                                       , kCPrepDirectiveDefineTranslateStateCommit         );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameterList    , '\\'                                      , kCPrepDirectiveDefineTranslateStateParameterList  );
    TRANSLATE_STATE_SET(c_prep_directive_define_translate_table,    kCPrepDirectiveDefineTranslateStateParameter        , ')'                                       , kCPrepDirectiveDefineTranslateStateCommit         );
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
        
        if (current == kCPrepDirectiveDefineTranslateStateCommit) {
            t = p;
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCPrepDirectiveDefineTranslateStateCommit && s) {
        /** define macro */
        define_directive = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     s, 
                                                                     s, 
                                                                     kCEESourceSymbolTypePrepDirectiveDefine,
                                                                     "c");
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
                                                                  "c");
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
                                                                     "c");
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
    CEESourceFregment* current = parser->statement_current;
    const cee_uchar* subject = parser->subject_ref;
    const cee_uchar* filepath = current->filepath_ref;
    CEEList* enumerators = NULL;
    
    if (!current || !current->tokens_ref)
        return;
    
    if (cee_source_fregment_grandfather_type_is(current, kCEESourceFregmentTypeEnumDefinition)) {
        enumerators = enumerators_extract(current->tokens_ref, filepath, subject);
        if (enumerators) {
            current->symbols = cee_list_concat(current->symbols, enumerators);
            cee_source_fregment_type_set(current, kCEESourceFregmentTypeEnumurators);
        }
    }
}

/**
 * c template
 */

static void c_template_declaration_translate_table_init(void)
{
    /*
     *             template    <           >         ws/nl      others
     *  Init       Commit      Init        Init      Init       Init
     *  Commit     Error       Parameter   Error     Commit     Error
     *  Parameter  Parameter   Parameter   Parameter Parameter  Parameter
     */
    TRANSLATE_STATE_INI(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateMax              , kCTemplateDeclarationTranslateStateError                                                  );
    TRANSLATE_STATE_ANY(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateInit             , kCTemplateDeclarationTranslateStateInit                                                   );
    TRANSLATE_STATE_SET(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateInit             , kCEETokenID_TEMPLATE                  , kCTemplateDeclarationTranslateStateCommit         );
    TRANSLATE_STATE_SET(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateCommit           , '<'                                   , kCTemplateDeclarationTranslateStateParameterList  );
    TRANSLATE_STATE_ANY(c_template_declaration_translate_table, kCTemplateDeclarationTranslateStateParameterList    , kCTemplateDeclarationTranslateStateParameterList                                          );
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

static void c_template_parameters_declaration_translate_table_init(void)
{
    
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
                                                             "c");
        fregment->symbols = cee_list_prepend(fregment->symbols, template);
    }
    
    return TRUE;
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
     *                                  object_specifier    declaration_specifier   virtual         alignas_specifier   access_specifier    identifier              <                   >       ::                              (               )               [                   ]                   :               ,               {
     *  Init                            ObjectSpecifier     Error                   Error           Error               Error               Error                   Error               Error   Error                           Error           Error           Error               Error               Error           Error           Error
     *  ObjectSpecifier                 ObjectSpecifier     Error                   Error           AlignasSpecifier    Error               ObjectIdentifier        Error               Error   ObjectIdentifierScope           Error           Error           ObjectSpecifier     ObjectSpecifier     Error           Error           Commit
     *  ObjectIdentifier                Error               Error                   Error           Error               Error               ObjectIdentifier        *TemplateParameter  Error   ObjectIdentifierScope           Error           Error           Error               Error               ObjectDerive    Error           Commit
     *  ObjectIdentifierScope           Error               Error                   Error           Error               Error               ObjectIdentifier        Error               Error   Error                           Error           Error           Error               Error               Error           Error           Error
     *  ObjectDerive                    Error               Error                   ObjectDerive    Error               ObjectDerive        ObjectDeriveIdentifier  Error               Error   ObjectDeriveIdentifierScope     Error           Error           ObjectDerive        ObjectDerive        Error           ObjectDerive    Error
     *  ObjectDeriveIdentifier          Error               Error                   Error           Error               Error               Error                   *TemplateParameter  Error   ObjectDeriveIdentifierScope     Error           Error           Error               Error               Error           ObjectDerive    Commit
     *  ObjectDeriveIdentifierScope     Error               Error                   Error           Error               Error               ObjectDeriveIdentifier  Error               Error   Error                           Error           Error           Error               Error               Error           Error           Error
     *  AlignasSpecifier                Error               Error                   Error           Error               Error               Error                   Error               Error   Error                           AlignasList     Error           Error               Error               Error           Error           Error
     *  AlignasList                     Error               Error                   Error           Error               Error               Error                   Error               Error   Error                           Error           ObjectSpecifier Error               Error               Error           Error           Error
     *  
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'  
     */
    TRANSLATE_STATE_INI(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateMax                          , kCInheritanceDefinitionTranslateStateError                                                            );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateInit                         , token_id_is_object_sepcifier      , kCInheritanceDefinitionTranslateStateObjectSpecifier              );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier              , kCEETokenID_IDENTIFIER            , kCInheritanceDefinitionTranslateStateObjectIdentifier             );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier              , token_id_is_object_sepcifier      , kCInheritanceDefinitionTranslateStateObjectSpecifier              );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier              , token_id_is_alignas_specifier     , kCInheritanceDefinitionTranslateStateAlignasSpecifier             );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier              , kCEETokenID_SCOPE                 , kCInheritanceDefinitionTranslateStateObjectIdentifierScope        );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier              , '['                               , kCInheritanceDefinitionTranslateStateObjectSpecifier              );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier              , ']'                               , kCInheritanceDefinitionTranslateStateObjectSpecifier              );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectSpecifier              , '{'                               , kCInheritanceDefinitionTranslateStateCommit                       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier             , kCEETokenID_IDENTIFIER            , kCInheritanceDefinitionTranslateStateObjectIdentifier             );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier             , '<'                               , kCInheritanceDefinitionTranslateStateTemplateParameter            );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier             , kCEETokenID_SCOPE                 , kCInheritanceDefinitionTranslateStateObjectIdentifierScope        );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier             , ':'                               , kCInheritanceDefinitionTranslateStateObjectDerive                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifier             , '{'                               , kCInheritanceDefinitionTranslateStateCommit                       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectIdentifierScope        , kCEETokenID_IDENTIFIER            , kCInheritanceDefinitionTranslateStateObjectIdentifier             );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                 , kCEETokenID_VIRTUAL               , kCInheritanceDefinitionTranslateStateObjectDerive                 );
    TRANSLATE_FUNCS_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                 , token_id_is_access_specifier      , kCInheritanceDefinitionTranslateStateObjectDerive                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                 , kCEETokenID_IDENTIFIER            , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                 , kCEETokenID_SCOPE                 , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope  );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                 , ','                               , kCInheritanceDefinitionTranslateStateObjectDerive                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                 , '['                               , kCInheritanceDefinitionTranslateStateObjectDerive                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDerive                 , ']'                               , kCInheritanceDefinitionTranslateStateObjectDerive                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier       , '<'                               , kCInheritanceDefinitionTranslateStateTemplateParameter            );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier       , kCEETokenID_SCOPE                 , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope  );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier       , ','                               , kCInheritanceDefinitionTranslateStateObjectDerive                 );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier       , '{'                               , kCInheritanceDefinitionTranslateStateCommit                       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateObjectDeriveIdentifierScope  , kCEETokenID_IDENTIFIER            , kCInheritanceDefinitionTranslateStateObjectDeriveIdentifier       );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateAlignasSpecifier             , '('                               , kCInheritanceDefinitionTranslateStateAlignasList                  );
    TRANSLATE_STATE_SET(c_inheritance_definition_translate_table, kCInheritanceDefinitionTranslateStateAlignasList                  , ')'                               , kCInheritanceDefinitionTranslateStateObjectSpecifier              );
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
        
        if (current == kCInheritanceDefinitionTranslateStateCommit ||
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
    
    if (current != kCInheritanceDefinitionTranslateStateCommit) {
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

CEESourceSymbol* c_inheritance_definition_create(CEEList* tokens,
                                                 const cee_uchar* filepath,
                                                 const cee_uchar* subject)
{
    CEEList* name = NULL;
    CEEList* derives_chains = NULL;
    CEESourceSymbol* definition = NULL;
    cee_char* derives_str = NULL;
    CEEList* trap_keyword = tokens;
    
    if (!c_inheritance_parse(tokens, &name, &derives_chains))
        return NULL;
    
    /** base class names found */
    if (derives_chains)
        derives_str = c_name_scope_list_string_create(derives_chains, subject);
    
    if (name) {
        definition = cee_source_symbol_create_from_token_slice(filepath,
                                                               subject, 
                                                               name, 
                                                               name, 
                                                               kCEESourceSymbolTypeUnknow, 
                                                               "c");
        definition->proto = cee_string_from_token(subject, name->data);
        cee_token_slice_state_mark(name, name, kCEETokenStateSymbolOccupied);
    }
    else {
        definition = cee_source_symbol_create_from_token_slice(filepath,
                                                               subject, 
                                                               trap_keyword, 
                                                               trap_keyword, 
                                                               kCEESourceSymbolTypeUnknow, 
                                                               "c");
    }
    
    definition->derives = derives_str;
        
    if (name)
        cee_list_free(name);
    
    if (derives_chains)
        cee_list_free_full(derives_chains, derived_chain_free);
    
    return definition;
}

static cee_char* c_name_scope_list_string_create(CEEList* scopes,
                                                 const cee_uchar* subject)
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
                                     const cee_uchar* subject)
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

static CEEList* c_name_scope_head_get(CEEList* p,
                                      const cee_uchar* subject)
{
    CEEToken* token = NULL;
    CEEList* q = NULL;
    
    token = p->data;
    
    if (token->identifier != kCEETokenID_IDENTIFIER &&
        token->identifier != '~' &&
        token->identifier != kCEETokenID_OPERATOR &&
        token->identifier != kCEETokenID_SCOPE) {
        return NULL;
    }
    
    while (p) {
        q = cee_token_not_whitespace_newline_before(p);
        if (!q)
            return p;
        
        if (cee_token_is_identifier(q, '>')) {
            q = skip_template_parameter_list(q, TRUE);
            q = cee_token_not_whitespace_newline_before(q);
            if (!q)
                return NULL;
        }
        
        if ((cee_token_is_identifier(p, kCEETokenID_IDENTIFIER) ||
             cee_token_is_identifier(p, '~') ||
             cee_token_is_identifier(p, kCEETokenID_OPERATOR)) &&
            !cee_token_is_identifier(q, kCEETokenID_SCOPE))
            return p;
        
        if (cee_token_is_identifier(p, kCEETokenID_SCOPE) &&
            !cee_token_is_identifier(q, kCEETokenID_IDENTIFIER))
            return p;
        
        p = q;
    }
    
    return NULL;
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
    CEESourceSymbol* definition = c_inheritance_definition_create(*pp, 
                                                                  fregment->filepath_ref,
                                                                  fregment->subject_ref);
    if (!definition)
        return FALSE;
    
    definition->type = kCEESourceSymbolTypeClassDefinition;
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeClassDefinition);
    *pp = NULL;
    
#ifdef DEBUG_CLASS
    cee_source_symbol_print(definition);
#endif
    return TRUE;
}

static cee_boolean c_enum_definition_trap(CEESourceFregment* fregment, 
                                          CEEList** pp)
{
    CEESourceSymbol* definition = c_inheritance_definition_create(*pp, 
                                                                  fregment->filepath_ref,
                                                                  fregment->subject_ref);
    if (!definition)
        return FALSE;
    
    definition->type = kCEESourceSymbolTypeEnumDefinition;
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeEnumDefinition);
    *pp = NULL;
        
#ifdef DEBUG_ENUM
    cee_source_symbol_print(definition);
#endif
    return TRUE;
}

static CEEList* enumerators_extract(CEEList* tokens,
                                    const cee_uchar* filepath,
                                    const cee_uchar* subject)
{
    CEEList* p = TOKEN_FIRST(tokens);
    CEEToken* token = NULL;
    CEETokenID identifier = kCEETokenID_UNKNOW;
    cee_boolean enumurating = TRUE;
    CEEList* q = NULL;
    CEESourceSymbol* enumerator = NULL;
    CEEList* enumerators = NULL;
    CEESourceFregment* grandfather = NULL;
    CEESourceSymbol* enum_symbol = NULL;
    CEEList* enum_symbols = NULL;
    
    if (p) {
        token = p->data;
        if (token) {
            grandfather = cee_source_fregment_grandfather_get(token->fregment_ref);
            if (grandfather) {
                enum_symbols = cee_source_fregment_symbols_search_by_type(grandfather, 
                                                                          kCEESourceSymbolTypeEnumDefinition);
                if (enum_symbols)
                    enum_symbol = cee_list_nth_data(enum_symbols, 0);
            }
        }
    }
    
    while (p) {
        token = p->data;
        
        identifier = token->identifier;
        
        if (identifier == kCEETokenID_IDENTIFIER && !is_name_scope(p) && enumurating)
            q = p;
        
        if (identifier == ',' || identifier == ';' || !TOKEN_NEXT(p)) {
            if (q) {
                enumerator = cee_source_symbol_create_from_token_slice(filepath,
                                                                       subject, 
                                                                       q, 
                                                                       q, 
                                                                       kCEESourceSymbolTypeEnumerator, 
                                                                       "c");
                enumerator->proto = cee_strdup(enum_symbol->name);
                cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
                enumerators = cee_list_prepend(enumerators, enumerator);
            }
            q = NULL;
            enumurating = TRUE;
        }
        else if (token_id_is_assignment(identifier) && enumurating) {
            enumurating = FALSE;
        }
        
        p = TOKEN_NEXT(p);
    }
    
#ifdef DEBUG_ENUM
    cee_source_symbols_print(enumerators);
#endif
    
    if (enum_symbols)
        cee_list_free(enum_symbols);
    
    return enumerators;
}

static cee_boolean c_union_definition_trap(CEESourceFregment* fregment, 
                                           CEEList** pp)
{
    CEESourceSymbol* definition = c_inheritance_definition_create(*pp, 
                                                                  fregment->filepath_ref,
                                                                  fregment->subject_ref);
    if (!definition)
        return TRUE;
    
    definition->type = kCEESourceSymbolTypeUnionDefinition;
    fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeUnionDefinition);
    *pp = NULL;
        
#ifdef DEBUG_UNION
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
     *  ParameterListEnd    Error       Error           Error           Error               Commit      Error
     *
     */
    TRANSLATE_STATE_INI(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateMax                         , kObjectiveCEnumDefinitionTranslateStateError                                                           );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateInit                        , kCEETokenID_NS_ENUM                           , kObjectiveCEnumDefinitionTranslateStateEnum            );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateInit                        , kCEETokenID_NS_OPTIONS                        , kObjectiveCEnumDefinitionTranslateStateOPtions         );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateEnum                        , '('                                           , kObjectiveCEnumDefinitionTranslateStateParameterList   );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateOPtions                     , '('                                           , kObjectiveCEnumDefinitionTranslateStateParameterList   );
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateParameterList               , ')'                                           , kObjectiveCEnumDefinitionTranslateStateParameterListEnd);
    TRANSLATE_STATE_SET(objective_c_enum_definition_translate_table, kObjectiveCEnumDefinitionTranslateStateParameterListEnd            , '{'                                           , kObjectiveCEnumDefinitionTranslateStateCommit          );
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
        else if (current == kObjectiveCEnumDefinitionTranslateStateCommit) {
            break;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kObjectiveCEnumDefinitionTranslateStateCommit) {
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
                                                               "c");
        cee_token_slice_state_mark(r, r, kCEETokenStateSymbolOccupied);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeEnumDefinition);
        ret = TRUE;
    }
    
    *pp = NULL;
    
#ifdef DEBUG_ENUM
    cee_source_symbol_print(definition);
#endif
    
    return TRUE;
}

static cee_boolean c_statement_block_parse(CEESourceFregment* fregment)
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
                    cee_source_fregment_type_set(fregment,
                                                 kCEESourceFregmentTypeAssignmentBlock);
                }
                else {
#ifdef DEBUG_STATEMENT_BLOCK
                    cee_source_fregment_string_print(fregment);
#endif
                    cee_source_fregment_type_set(fregment,
                                                 kCEESourceFregmentTypeStatementBlock);
                }
            }
            else {
#ifdef DEBUG_STATEMENT_BLOCK
                cee_source_fregment_string_print(fregment);
#endif
                cee_source_fregment_type_set(fregment,
                                             kCEESourceFregmentTypeStatementBlock);
            }
        }
        p = TOKEN_NEXT(p);
    }
    
    return TRUE;
}

static void c_function_definition_translate_table_init(void)
{
    /**
     *                              declaration_specifier   literal                 decltype_specifier      builtin_type    object_specifier    access_specifier    overload_operator   new             delete          virtual_specifier   identifier          try     noexcept    throw       <                   >           ::                  *                       &                       &&                      ~                       (                       )                           [                       ]                       :               ,               operator            {           }               ->          others
     *  Init                        DeclarationSpecifier    Error                   Decltype                BuiltinType     ObjectSpecifier     AccessSpecifier     Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     Error                   Error                   Error                   Init                    Error                   Error                       Init                    Init                    Error           Error           Error               Error       Error           Error       Error
     *  DeclarationSpecifier        DeclarationSpecifier    DeclarationSpecifier    Decltype                BuiltinType     ObjectSpecifier     Error               Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    DeclarationSpecifier    Error                   Error                       DeclarationSpecifier    DeclarationSpecifier    Error           Error           Operator            Error       Error           Error       Error
     *  Decltype                    Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   DecltypeList            Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  DecltypeList                Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   BuiltinType                 Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  BuiltinType                 BuiltinType             Error                   Decltype                BuiltinType     Error               Error               Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     BuiltinType             BuiltinType             BuiltinType             BuiltinType             Error                   Error                       BuiltinType             BuiltinType             Error           Error           Operator            Error       Error           Error       Error
     *  ObjectSpecifier             Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  AccessSpecifier             DeclarationSpecifier    Error                   Decltype                BuiltinType     ObjectSpecifier     AccessSpecifier     Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       IdentifierScope     Error                   Error                   Error                   AccessSpecifier         Error                   Error                       AccessSpecifier         AccessSpecifier         AccessSpecifier Error           Error               Error       Error           Error       Error
     *  Identifier                  Identifier              Error                   Decltype                BuiltinType     ObjectSpecifier     AccessSpecifier     Error               Error           Error           Error               Identifier          Error   Error       Error       *TemplateParameter  Error       IdentifierScope     Identifier              Identifier              Identifier              Identifier              ParameterList           Error                       Identifier              Identifier              Error           Error           Operator            Error       Error           Error       Error
     *  IdentifierScope             Identifier              Error                   Error                   Identifier      Error               Error               Error               Error           Error           Error               Identifier          Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   IdentifierScope         Error                   Error                       Error                   Error                   Error           Error           Operator            Error       Error           Error       Error
     *  ParameterList               Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   ParameterListEnd            Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  ParameterListEnd            ParameterListEnd        Error                   Error                   Error           Error               Error               Error               Error           Error           ParameterListEnd    ParameterListEnd    Try     Noexcept    Throw       Error               Error       Error               Error                   Error                   ParameterListEnd        Error                   ParameterList           Error                       ParameterListEnd        ParameterListEnd        Initializer     Error           Error               Commit      Error           *Trailing   Error
     *  Noexcept                    Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           ParameterListEnd    ParameterListEnd    Try     Error       Throw       Error               Error       Error               Error                   Error                   ParameterListEnd        Error                   NoexceptList            Error                       ParameterListEnd        ParameterListEnd        Initializer     Error           Error               Commit      Error           *Trailing   Error
     *  NoexceptList                Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   ParameterListEnd            Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  Throw                       Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           ParameterListEnd    ParameterListEnd    Try     Noexcept    Error       Error               Error       Error               Error                   Error                   ParameterListEnd        Error                   ThrowList               Error                       ParameterListEnd        ParameterListEnd        Initializer     Error           Error               Commit      Error           *Trailing   Error
     *  ThrowList                   Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   ParameterListEnd            Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  Initializer                 Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Member              Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error           Initializer     Error               Error       Error           Error       Error
     *  Member                      Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   MemberInit              Error                       Error                   Error                   Error           Error           Error               MemberInit  Error           Error       Error
     *  MemberInit                  Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   MemberInitEnd               Error                   Error                   Error           Error           Error               Error       MemberInitEnd   Error       Error
     *  MemberInitEnd               Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Try     Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   Error                       MemberInitEnd           MemberInitEnd           Error           Initializer     Error               Commit      Error           Error       Error
     *  Try                         Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error           Error           Error               Commit      Error           Error       Error
     *  Operator                    Error                   Error                   Error                   OverloadType    Error               Error               OverloadOperator    OverloadNew     OverloadDelete  Error               OverloadType        Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   OverloadRoundBracket    Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadOperator            Error                   Error                   Error                   Error           Error               Error               OverloadOperator    Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList           Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadNew                 Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList           Error                       OverloadNew             OverloadNew             Error           Error           Error               Error       Error           Error       Error
     *  OverloadDelete              Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList           Error                       OverloadDelete          OverloadDelete          Error           Error           Error               Error       Error           Error       Error
     *  OverloadType                Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   OverloadTypeSurrounded  Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadTypeSurrounded      Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   OverloadTypeSurroundedEnd   Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadTypeSurroundedEnd   Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList           Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadRoundBracket        Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   Error                   OverloadRoundBracketEnd     Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *  OverloadRoundBracketEnd     Error                   Error                   Error                   Error           Error               Error               Error               Error           Error           Error               Error               Error   Error       Error       Error               Error       Error               Error                   Error                   Error                   Error                   ParameterList           Error                       Error                   Error                   Error           Error           Error               Error       Error           Error       Error
     *
     *  Trailing: skip to Commit
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'  
     */
    TRANSLATE_STATE_INI(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMax                        , kCFunctionDefinitionTranslateStateError                                                                               );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , token_id_is_declaration_specifier                 , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , kCEETokenID_DECLTYPE                              , kCFunctionDefinitionTranslateStateDecltype                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , token_id_is_builtin_type                          , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , token_id_is_object_sepcifier                      , kCFunctionDefinitionTranslateStateObjectSpecifier                 );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , token_id_is_access_specifier                      , kCFunctionDefinitionTranslateStateAccessSpecifier                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , kCEETokenID_SCOPE                                 , kCFunctionDefinitionTranslateStateIdentifierScope                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , '~'                                               , kCFunctionDefinitionTranslateStateInit                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , '['                                               , kCFunctionDefinitionTranslateStateInit                            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInit                       , ']'                                               , kCFunctionDefinitionTranslateStateInit                            );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , token_id_is_declaration_specifier                 , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , kCEETokenID_LITERAL                               , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , kCEETokenID_DECLTYPE                              , kCFunctionDefinitionTranslateStateDecltype                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , token_id_is_builtin_type                          , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , token_id_is_object_sepcifier                      , kCFunctionDefinitionTranslateStateObjectSpecifier                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , kCEETokenID_SCOPE                                 , kCFunctionDefinitionTranslateStateIdentifierScope                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , '*'                                               , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , '&'                                               , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , kCEETokenID_LOGIC_AND                             , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , '~'                                               , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , '['                                               , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , ']'                                               , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDeclarationSpecifier       , kCEETokenID_OPERATOR                              , kCFunctionDefinitionTranslateStateOperator                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDecltype                   , '('                                               , kCFunctionDefinitionTranslateStateDecltypeList                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateDecltypeList               , ')'                                               , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , token_id_is_declaration_specifier                 , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , kCEETokenID_DECLTYPE                              , kCFunctionDefinitionTranslateStateDecltype                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , token_id_is_builtin_type                          , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , kCEETokenID_SCOPE                                 , kCFunctionDefinitionTranslateStateIdentifierScope                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , '*'                                               , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , '&'                                               , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , kCEETokenID_LOGIC_AND                             , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , '['                                               , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , ']'                                               , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , '~'                                               , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateBuiltinType                , kCEETokenID_OPERATOR                              , kCFunctionDefinitionTranslateStateOperator                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateObjectSpecifier            , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateObjectSpecifier            , kCEETokenID_SCOPE                                 , kCFunctionDefinitionTranslateStateIdentifierScope                 );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , token_id_is_declaration_specifier                 , kCFunctionDefinitionTranslateStateDeclarationSpecifier            );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , kCEETokenID_DECLTYPE                              , kCFunctionDefinitionTranslateStateDecltype                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , token_id_is_builtin_type                          , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , token_id_is_object_sepcifier                      , kCFunctionDefinitionTranslateStateObjectSpecifier                 );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , token_id_is_access_specifier                      , kCFunctionDefinitionTranslateStateAccessSpecifier                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , kCEETokenID_SCOPE                                 , kCFunctionDefinitionTranslateStateIdentifierScope                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , '~'                                               , kCFunctionDefinitionTranslateStateAccessSpecifier                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , '['                                               , kCFunctionDefinitionTranslateStateAccessSpecifier                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , ']'                                               , kCFunctionDefinitionTranslateStateAccessSpecifier                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateAccessSpecifier            , ':'                                               , kCFunctionDefinitionTranslateStateAccessSpecifier                 );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , token_id_is_declaration_specifier                 , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , kCEETokenID_DECLTYPE                              , kCFunctionDefinitionTranslateStateDecltype                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , token_id_is_builtin_type                          , kCFunctionDefinitionTranslateStateBuiltinType                     );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , token_id_is_object_sepcifier                      , kCFunctionDefinitionTranslateStateObjectSpecifier                 );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , token_id_is_access_specifier                      , kCFunctionDefinitionTranslateStateAccessSpecifier                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , '<'                                               , kCFunctionDefinitionTranslateStateTemplateParameter               );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , kCEETokenID_SCOPE                                 , kCFunctionDefinitionTranslateStateIdentifierScope                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , '*'                                               , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , '&'                                               , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , kCEETokenID_LOGIC_AND                             , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , '('                                               , kCFunctionDefinitionTranslateStateParameterList                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , '['                                               , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , ']'                                               , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , '~'                                               , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifier                 , kCEETokenID_OPERATOR                              , kCFunctionDefinitionTranslateStateOperator                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope            , token_id_is_declaration_specifier                 , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope            , token_id_is_builtin_type                          , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope            , '~'                                               , kCFunctionDefinitionTranslateStateIdentifierScope                 );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope            , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateIdentifier                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateIdentifierScope            , kCEETokenID_OPERATOR                              , kCFunctionDefinitionTranslateStateOperator                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterList              , ')'                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , token_id_is_declaration_specifier                 , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , token_id_is_virtual_sepcifier                     , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , kCEETokenID_TRY                                   , kCFunctionDefinitionTranslateStateTry                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , kCEETokenID_NOEXCEPT                              , kCFunctionDefinitionTranslateStateNoexcept                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , kCEETokenID_THROW                                 , kCFunctionDefinitionTranslateStateThrow                           );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , kCEETokenID_LOGIC_AND                             , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , '('                                               , kCFunctionDefinitionTranslateStateParameterList                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , '['                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , ']'                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , ':'                                               , kCFunctionDefinitionTranslateStateInitializer                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , '{'                                               , kCFunctionDefinitionTranslateStateCommit                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateParameterListEnd           , kCEETokenID_MEMBER_POINTER                        , kCFunctionDefinitionTranslateStateTrailing                        );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , token_id_is_virtual_sepcifier                     , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , kCEETokenID_TRY                                   , kCFunctionDefinitionTranslateStateTry                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , kCEETokenID_THROW                                 , kCFunctionDefinitionTranslateStateThrow                           );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , kCEETokenID_LOGIC_AND                             , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , '('                                               , kCFunctionDefinitionTranslateStateNoexceptList                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , '['                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , ']'                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , ':'                                               , kCFunctionDefinitionTranslateStateInitializer                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , kCEETokenID_MEMBER_POINTER                        , kCFunctionDefinitionTranslateStateTrailing                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexcept                   , '{'                                               , kCFunctionDefinitionTranslateStateCommit                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateNoexceptList               , ')'                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , token_id_is_virtual_sepcifier                     , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , kCEETokenID_TRY                                   , kCFunctionDefinitionTranslateStateTry                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , kCEETokenID_NOEXCEPT                              , kCFunctionDefinitionTranslateStateNoexcept                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , kCEETokenID_LOGIC_AND                             , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , '('                                               , kCFunctionDefinitionTranslateStateThrowList                       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , '['                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , ']'                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , ':'                                               , kCFunctionDefinitionTranslateStateInitializer                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , kCEETokenID_MEMBER_POINTER                        , kCFunctionDefinitionTranslateStateTrailing                        );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrow                      , '{'                                               , kCFunctionDefinitionTranslateStateCommit                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateThrowList                  , ')'                                               , kCFunctionDefinitionTranslateStateParameterListEnd                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInitializer                , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateMember                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateInitializer                , ','                                               , kCFunctionDefinitionTranslateStateInitializer                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMember                     , '('                                               , kCFunctionDefinitionTranslateStateMemberInit                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMember                     , '{'                                               , kCFunctionDefinitionTranslateStateMemberInit                      );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInit                 , ')'                                               , kCFunctionDefinitionTranslateStateMemberInitEnd                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInit                 , '}'                                               , kCFunctionDefinitionTranslateStateMemberInitEnd                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd              , kCEETokenID_TRY                                   , kCFunctionDefinitionTranslateStateTry                             );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd              , ','                                               , kCFunctionDefinitionTranslateStateInitializer                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd              , '['                                               , kCFunctionDefinitionTranslateStateMemberInitEnd                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd              , ']'                                               , kCFunctionDefinitionTranslateStateMemberInitEnd                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateMemberInitEnd              , '{'                                               , kCFunctionDefinitionTranslateStateCommit                          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateTry                        , '{'                                               , kCFunctionDefinitionTranslateStateCommit                          );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                   , token_id_is_builtin_type                          , kCFunctionDefinitionTranslateStateOverloadType                    );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                   , token_id_is_overload_operator                     , kCFunctionDefinitionTranslateStateOverloadOperator                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                   , kCEETokenID_IDENTIFIER                            , kCFunctionDefinitionTranslateStateOverloadType                    );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                   , kCEETokenID_NEW                                   , kCFunctionDefinitionTranslateStateOverloadNew                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                   , kCEETokenID_DELETE                                , kCFunctionDefinitionTranslateStateOverloadDelete                  );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOperator                   , '('                                               , kCFunctionDefinitionTranslateStateOverloadRoundBracket            );
    TRANSLATE_FUNCS_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadOperator           , token_id_is_overload_operator                     , kCFunctionDefinitionTranslateStateOverloadOperator                );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadOperator           , '('                                               , kCFunctionDefinitionTranslateStateParameterList                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadNew                , '['                                               , kCFunctionDefinitionTranslateStateOverloadNew                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadNew                , ']'                                               , kCFunctionDefinitionTranslateStateOverloadNew                     );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadNew                , '('                                               , kCFunctionDefinitionTranslateStateParameterList                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadDelete             , '['                                               , kCFunctionDefinitionTranslateStateOverloadDelete                  );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadDelete             , ']'                                               , kCFunctionDefinitionTranslateStateOverloadDelete                  );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadDelete             , '('                                               , kCFunctionDefinitionTranslateStateParameterList                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadType               , '('                                               , kCFunctionDefinitionTranslateStateOverloadTypeSurrounded          );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadTypeSurrounded     , ')'                                               , kCFunctionDefinitionTranslateStateOverloadTypeSurroundedEnd       );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadTypeSurroundedEnd  , '('                                               , kCFunctionDefinitionTranslateStateParameterList                   );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadRoundBracket       , ')'                                               , kCFunctionDefinitionTranslateStateOverloadRoundBracketEnd         );
    TRANSLATE_STATE_SET(c_function_definition_translate_table, kCFunctionDefinitionTranslateStateOverloadRoundBracketEnd    , '('                                               , kCFunctionDefinitionTranslateStateParameterList                   );
}

static cee_boolean c_function_definition_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEEList* overload = NULL;
    CEESourceSymbol* definition = NULL;
    CEEToken* token = NULL;
    CEESourceFregment* child = NULL;
    CFunctionDefinitionTranslateState current = kCFunctionDefinitionTranslateStateInit;
    CFunctionDefinitionTranslateState prev = kCFunctionDefinitionTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        prev = current;
        current = c_function_definition_translate_table[current][token->identifier];
        
        if (current == kCFunctionDefinitionTranslateStateError) {
            /*current = kCFunctionDefinitionTranslateStateInit;*/
            return FALSE;
        }
        else if (current == kCFunctionDefinitionTranslateStateTemplateParameter) {
            p = skip_template_parameter_list(p, FALSE);
            if (!p) 
                return FALSE;
            current = prev;
        }
        else if (current == kCFunctionDefinitionTranslateStateTrailing ||
                 current == kCFunctionDefinitionTranslateStateCommit) {
            break;
        }
        else if (current == kCFunctionDefinitionTranslateStateOperator) {
            if (token->identifier == kCEETokenID_OPERATOR)
                overload = p;
        }
        else if (current == kCFunctionDefinitionTranslateStateParameterList) {
            if (!identifier) {
                q = cee_token_not_whitespace_newline_before(p);
                if (cee_token_is_identifier(q, kCEETokenID_IDENTIFIER))
                    identifier = q;
            }
            if (!parameter_list && token->identifier == '(')
                parameter_list = p;
        }
        else if (current == kCFunctionDefinitionTranslateStateParameterListEnd) {
            if (!parameter_list_end && token->identifier == ')')
                parameter_list_end = p;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCFunctionDefinitionTranslateStateCommit || 
        current == kCFunctionDefinitionTranslateStateTrailing) {
        if (overload) {
            q = r = overload;
        }
        else if (identifier) {
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
                                                               "c");
        cee_token_slice_state_mark(q, r, kCEETokenStateSymbolOccupied);
        cee_source_fregment_type_set_exclusive(fregment, kCEESourceFregmentTypeFunctionDefinition);
        
        q = c_name_scope_head_get(q, fregment->subject_ref);
        if (q && TOKEN_PREV(q))
            definition->proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                                                 SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                                 TOKEN_PREV(q));
        if (parameter_list) {
            child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
            if (child) {
                child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
                c_function_parameters_parse(child);
            }
        }
    }
    else if (current == kCFunctionDefinitionTranslateStateMemberInit) {
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeAssignmentBlock);
    }
    
    if (definition)
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    
#ifdef DEBUG_FUNCTION_DEFINITION
    cee_source_symbols_print(fregment->symbols);
#endif
    
    return TRUE;
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
     *  Identifier              DeclarationSpecifier    Decltype            BuiltinType     ObjectSpecifier     Commit              Identifier          *TemplateParameter  Error   IdentifierScope     Error       Identifier              Identifier              Identifier              ParameterList   Error               Identifier              Identifier              Commit      Error
     *  IdentifierScope         Identifier              Error               Identifier      Error               Error               Identifier          Error               Error   Error               Error       Error                   Error                   Error                   Error           Error               Error                   Error                   Error       Error
     *  ParameterList           Error                   Error               Error           Error               Error               Error               Error               Error   Error               Error       Error                   Error                   Error                   Errror          ParameterListEnd    Error                   Error                   Error       Error
     *  ParameterListEnd        Error                   Error               Error           Error               Commit              Error               Error               Error   Error               Error       Error                   Error                   Error                   Surrounded      Error               Surrounded              Error                   Commit      Error
     *  Surrounded              Error                   Error               Error           Error               Error               Error               Error               Error   Error               Error       Error                   Error                   Error                   Error           SurroundedEnd       Error                   SurroundedEnd           Error       Error
     *  SurroundedEnd           Error                   Error               Error           Error               Commit              Error               Error               Error   Error               Error       Error                   Error                   Error                   SurroundedEnd   SurroundedEnd       SurroundedEnd           SurroundedEnd           Commit      Error
     *  Ellipsis                Error                   Error               Error           Error               Commit              Error               Error               Error   Error               Error       Error                   Error                   Error                   Error           Error               Error                   Error                   Commit      Error
     */
    TRANSLATE_STATE_INI(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateMax                   , kCFunctionParametersDeclarationTranslateStateError                                                                     );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , token_id_is_declaration_specifier                 , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , kCEETokenID_DECLTYPE                              , kCFunctionParametersDeclarationTranslateStateDecltype              );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , token_id_is_builtin_type                          , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , token_id_is_object_sepcifier                      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , kCEETokenID_SCOPE                                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , kCEETokenID_ELLIPSIS                              , kCFunctionParametersDeclarationTranslateStateEllipsis              );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , '['                                               , kCFunctionParametersDeclarationTranslateStateInit                  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateInit                  , ']'                                               , kCFunctionParametersDeclarationTranslateStateInit                  );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , token_id_is_declaration_specifier                 , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_DECLTYPE                              , kCFunctionParametersDeclarationTranslateStateDecltype              );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , token_id_is_builtin_type                          , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , token_id_is_object_sepcifier                      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_SCOPE                                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , '*'                                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , '&'                                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_LOGIC_AND                             , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , '['                                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  , ']'                                               , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , token_id_is_declaration_specifier                 , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , kCEETokenID_DECLTYPE                              , kCFunctionParametersDeclarationTranslateStateDecltype              );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , token_id_is_builtin_type                          , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , kCEETokenID_SCOPE                                 , kCFunctionParametersDeclarationTranslateStateIdentifierScope       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , '*'                                               , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , '&'                                               , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , kCEETokenID_LOGIC_AND                             , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , '['                                               , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , ']'                                               , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , ','                                               , kCFunctionParametersDeclarationTranslateStateCommit                );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateBuiltinType           , '('                                               , kCFunctionParametersDeclarationTranslateStateParameterList         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDecltype              , '('                                               , kCFunctionParametersDeclarationTranslateStateDecltypeList          );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateDecltypeList          , ')'                                               , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateObjectSpecifier       , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateObjectSpecifier       , kCEETokenID_SCOPE                                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope       );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , token_id_is_declaration_specifier                 , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , kCEETokenID_DECLTYPE                              , kCFunctionParametersDeclarationTranslateStateDecltype              );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , token_id_is_builtin_type                          , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , token_id_is_object_sepcifier                      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , '<'                                               , kCFunctionParametersDeclarationTranslateStateTemplateParameter     );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , kCEETokenID_SCOPE                                 , kCFunctionParametersDeclarationTranslateStateCustomTypeScope       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , '*'                                               , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , '&'                                               , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , kCEETokenID_LOGIC_AND                             , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , '['                                               , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , ']'                                               , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomType            , '('                                               , kCFunctionParametersDeclarationTranslateStateParameterList         );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomTypeScope       , token_id_is_declaration_specifier                 , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomTypeScope       , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateCustomTypeScope       , token_id_is_builtin_type                          , kCFunctionParametersDeclarationTranslateStateCustomType            );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , token_id_is_declaration_specifier                 , kCFunctionParametersDeclarationTranslateStateDeclarationSpecifier  );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , kCEETokenID_DECLTYPE                              , kCFunctionParametersDeclarationTranslateStateDecltype              );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , token_id_is_builtin_type                          , kCFunctionParametersDeclarationTranslateStateBuiltinType           );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , token_id_is_object_sepcifier                      , kCFunctionParametersDeclarationTranslateStateObjectSpecifier       );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , token_id_is_assignment                            , kCFunctionParametersDeclarationTranslateStateCommit                );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , '<'                                               , kCFunctionParametersDeclarationTranslateStateTemplateParameter     );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , kCEETokenID_SCOPE                                 , kCFunctionParametersDeclarationTranslateStateIdentifierScope       );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , '*'                                               , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , '&'                                               , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , kCEETokenID_LOGIC_AND                             , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , '['                                               , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , ']'                                               , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , ','                                               , kCFunctionParametersDeclarationTranslateStateCommit                );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifier            , '('                                               , kCFunctionParametersDeclarationTranslateStateParameterList         );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifierScope       , token_id_is_builtin_type                          , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_FUNCS_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifierScope       , token_id_is_declaration_specifier                 , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateIdentifierScope       , kCEETokenID_IDENTIFIER                            , kCFunctionParametersDeclarationTranslateStateIdentifier            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterList         , ')'                                               , kCFunctionParametersDeclarationTranslateStateParameterListEnd      );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterListEnd      , '('                                               , kCFunctionParametersDeclarationTranslateStateSurrounded            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterListEnd      , '['                                               , kCFunctionParametersDeclarationTranslateStateSurrounded            );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateParameterListEnd      , ','                                               , kCFunctionParametersDeclarationTranslateStateCommit                );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurrounded            , ')'                                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurrounded            , ']'                                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd         , '('                                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd         , ')'                                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd         , '['                                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd         , ']'                                               , kCFunctionParametersDeclarationTranslateStateSurroundedEnd         );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateSurroundedEnd         , ','                                               , kCFunctionParametersDeclarationTranslateStateCommit                );
    TRANSLATE_STATE_SET(c_function_parameters_declaration_translate_table, kCFunctionParametersDeclarationTranslateStateEllipsis              , ','                                               , kCFunctionParametersDeclarationTranslateStateCommit                );
}

static cee_boolean c_function_parameters_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEList* head = NULL;
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
    head = p; 
    while (p) {
        token = p->data;
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
            if (!parameter_list &&
                cee_token_is_identifier(p, '('))
                parameter_list = p;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateParameterListEnd) {
            if (!parameter_list_end &&
                cee_token_is_identifier(p, ')'))
                parameter_list_end = p;
        }
            
        if (current == kCFunctionParametersDeclarationTranslateStateSurrounded) {
            if (!surrounded &&
                (cee_token_is_identifier(p, '(') ||
                 cee_token_is_identifier(p, '[')))
                surrounded = p;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateSurroundedEnd) {
            if (!surrounded_end &&
                (cee_token_is_identifier(p, '(') ||
                 cee_token_is_identifier(p, '[')))
                surrounded_end = p;
        }
                
        if (current == kCFunctionParametersDeclarationTranslateStateEllipsis) {
            if (cee_token_is_identifier(p, kCEETokenID_ELLIPSIS))
                ellipsis = p;
        }
        
        if (current == kCFunctionParametersDeclarationTranslateStateCommit ||
            !TOKEN_NEXT(p)) {
            
            if (surrounded) {
                declaration = c_function_parameter_surrounded_create(fregment,
                                                                     head,
                                                                     parameter_list);
            }
            else if (parameter_list) {
                q = cee_token_not_whitespace_newline_before(parameter_list);
                if (cee_token_is_identifier(q, kCEETokenID_IDENTIFIER)) {
                    declaration = c_function_parameter_identifier_create(fregment,
                                                                         head,
                                                                         q,
                                                                         q);
                }
            }
            else if (identifier) {
                declaration = c_function_parameter_identifier_create(fregment,
                                                                     head,
                                                                     identifier,
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
            
            head = TOKEN_NEXT(p);
            current = kCFunctionParametersDeclarationTranslateStateInit;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (fregment->symbols)
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
    
#ifdef DEBUG_FUNCTION_DEFINITION
    cee_source_symbols_print(fregment->symbols);
#endif

    return TRUE;
}

static CEESourceSymbol* c_function_parameter_identifier_create(CEESourceFregment* fregment,
                                                               CEEList* head,
                                                               CEEList* begin,
                                                               CEEList* end)
{
    CEESourceSymbol* parameter = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                           fregment->subject_ref,
                                                                           begin, 
                                                                           end,
                                                                           kCEESourceSymbolTypeFunctionParameter,
                                                                           "c");
    cee_token_slice_state_mark(begin, end, kCEETokenStateSymbolOccupied);
    if (head && TOKEN_PREV(begin))
        parameter->proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                                            head,
                                                            TOKEN_PREV(begin));
    return parameter;
}

static CEESourceSymbol* c_function_parameter_surrounded_create(CEESourceFregment* fregment,
                                                               CEEList* head,
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
    
    p = tokens;
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
                                                              "c");
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        if (head && TOKEN_PREV(surrounded))
            parameter->proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                                                head,
                                                                TOKEN_PREV(surrounded));
    }
    cee_list_free(tokens);
    
    return parameter;
}

static void objective_c_message_definition_translate_table_init(void)
{
    /**
     *                       -           +          identifier  (               )                   :           {          others
     *  Init                 Message     Message    Error       Error           Error               Error       Error      Error
     *  Message              Error       Error      Error       ReturnType      Error               Error       Error      Error
     *  ReturnType           Error       Error      Error       Error           ReturnTypeEnd       Error       Error      Error
     *  ReturnTypeEnd        Error       Error      Component   Error           Error               Error       Error      Error
     *  Component            Error       Error      Error       Error           Error               Colon       Commit     Error
     *  Colon                Error       Error      Error       ParameterType   Error               Error       Error      Error
     *  ParameterType        Error       Error      Error       Error           ParameterTypeEnd    Error       Error      Error
     *  ParameterTypeEnd     Error       Error      Parameter   Error           Error               Error       Error      Error
     *  Parameter            Error       Error      Component   Error           Error               Error       Commit     Error
     */
    TRANSLATE_STATE_INI(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateMax               , kObjectiveCMessageDefinitionTranslateStateError                                                                   );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateInit              , '-'                                               , kObjectiveCMessageDefinitionTranslateStateMessage             );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateInit              , '+'                                               , kObjectiveCMessageDefinitionTranslateStateMessage             );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateMessage           , '('                                               , kObjectiveCMessageDefinitionTranslateStateReturnType          );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateReturnType        , ')'                                               , kObjectiveCMessageDefinitionTranslateStateReturnTypeEnd       );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateReturnTypeEnd     , kCEETokenID_IDENTIFIER                            , kObjectiveCMessageDefinitionTranslateStateComponent           );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateComponent         , ':'                                               , kObjectiveCMessageDefinitionTranslateStateColon               );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateComponent         , '{'                                               , kObjectiveCMessageDefinitionTranslateStateCommit              );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateColon             , '('                                               , kObjectiveCMessageDefinitionTranslateStateParameterType       );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateParameterType     , ')'                                               , kObjectiveCMessageDefinitionTranslateStateParameterTypeEnd    );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateParameterTypeEnd  , kCEETokenID_IDENTIFIER                            , kObjectiveCMessageDefinitionTranslateStateParameter           );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateParameter         , kCEETokenID_IDENTIFIER                            , kObjectiveCMessageDefinitionTranslateStateComponent           );
    TRANSLATE_STATE_SET(objective_c_message_definition_translate_table, kObjectiveCMessageDefinitionTranslateStateParameter         , '{'                                               , kObjectiveCMessageDefinitionTranslateStateCommit              );
}

static cee_boolean objective_c_message_definition_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* symbols = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* p = NULL;
    CEEToken* token = NULL;
    CEEList* components = NULL;
    CEEList* parameters = NULL;
    CEEList* q = NULL;
    cee_int parameter_index = 0;
    ObjectiveCMessageDefinitionTranslateState current = kObjectiveCMessageDefinitionTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        current = objective_c_message_definition_translate_table[current][token->identifier];
        
        if (current == kObjectiveCMessageDefinitionTranslateStateError) {
            break;
        }
        else if (current == kObjectiveCMessageDefinitionTranslateStateComponent) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(components, token);
        }
        else if (current == kObjectiveCMessageDefinitionTranslateStateColon) {
            if (token->identifier == ':')
                TOKEN_APPEND(components, token);
        }
        else if (current == kObjectiveCMessageDefinitionTranslateStateParameter) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(parameters, token);
        }
        else if (current == kObjectiveCMessageDefinitionTranslateStateCommit) {
            if (components) {
                symbol = cee_source_symbol_create_from_tokens(fregment->filepath_ref,
                                                              fregment->subject_ref,
                                                              components, 
                                                              kCEESourceSymbolTypeMessageDefinition, 
                                                              "c");
                if (symbol) {
                    symbol->proto = objective_c_message_protos_dump(fregment);
                    symbols = cee_list_prepend(symbols, symbol);
                }
                cee_tokens_state_mark(components, kCEETokenStateSymbolOccupied);
                cee_list_free(components);
            }
            
            if (parameters) {
                q = TOKEN_FIRST(parameters);
                parameter_index = 0;
                while (q) {
                    symbol = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                       fregment->subject_ref,
                                                                       q, 
                                                                       q, 
                                                                       kCEESourceSymbolTypeMessageParameter, 
                                                                       "c");
                    if (symbol) {
                        symbol->proto = objective_c_message_parameter_protos_dump(fregment,
                                                                                  parameter_index);
                        symbols = cee_list_prepend(symbols, symbol);
                    }
                    q = TOKEN_NEXT(q);
                    parameter_index ++;
                }
                cee_tokens_state_mark(parameters, kCEETokenStateSymbolOccupied);
                cee_list_free(parameters);
            }
            break;
        }
        p = TOKEN_NEXT(p);
    }
    
    if (symbols) {
        fregment->symbols = cee_list_concat(symbols, fregment->symbols);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeFunctionDefinition);
        ret = TRUE;
    }

    
#ifdef DEBUG_MESSAGE_DEFINITION
    cee_source_symbols_print(fregment->symbols);
#endif   
    return ret;
}

static void objective_c_property_declaration_translate_table_init(void)
{
    /**
     *                  @optional   @required   @property   (           )               ,       ;           others
     *  Init            Init        Init        Property    Error       Error           Error   Terminate   Error
     *  Property        Error       Error       Error       Attribute   Error           Commit  Terminate   Property
     *  Attribute       Error       Error       Error       Error       AttributeEnd    Error   Error       Error
     *  AttributeEnd    Error       Error       Error       Error       Error           Commit  Terminate   AttributeEnd
     */
    TRANSLATE_STATE_INI(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateMax           , kObjectiveCPropertyDeclarationTranslateStateError                                                                     );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateInit          , kCEETokenID_AT_OPTIONAL                                   , kObjectiveCPropertyDeclarationTranslateStateInit          );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateInit          , kCEETokenID_AT_REQUIRED                                   , kObjectiveCPropertyDeclarationTranslateStateInit          );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateInit          , kCEETokenID_AT_PROPERTY                                   , kObjectiveCPropertyDeclarationTranslateStateProperty      );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateInit          , ';'                                                       , kObjectiveCPropertyDeclarationTranslateStateTerminate     );
    TRANSLATE_STATE_ANY(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateProperty      , kObjectiveCPropertyDeclarationTranslateStateProperty                                                                  );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateProperty      , kCEETokenID_AT_PROPERTY                                   , kObjectiveCPropertyDeclarationTranslateStateError         );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateProperty      , '('                                                       , kObjectiveCPropertyDeclarationTranslateStateAttribute     );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateProperty      , ')'                                                       , kObjectiveCPropertyDeclarationTranslateStateError         );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateProperty      , ','                                                       , kObjectiveCPropertyDeclarationTranslateStateCommit        );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateProperty      , ';'                                                       , kObjectiveCPropertyDeclarationTranslateStateTerminate     );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateAttribute     , ')'                                                       , kObjectiveCPropertyDeclarationTranslateStateAttributeEnd  );
    TRANSLATE_STATE_ANY(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateAttributeEnd  , kObjectiveCPropertyDeclarationTranslateStateAttributeEnd                                                              );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateAttributeEnd  , ','                                                       , kObjectiveCPropertyDeclarationTranslateStateCommit        );
    TRANSLATE_STATE_SET(objective_c_property_declaration_translate_table, kObjectiveCPropertyDeclarationTranslateStateAttributeEnd  , ';'                                                       , kObjectiveCPropertyDeclarationTranslateStateTerminate     );
}

static cee_boolean objective_c_property_declaration_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    CEESourceSymbol* member_variable = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    cee_char* proto = NULL;
    CEEList* property = NULL;
    CEEList* attribute_end = NULL;
    ObjectiveCPropertyDeclarationTranslateState current = 
        kObjectiveCPropertyDeclarationTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (token->identifier == kCEETokenID_IDENTIFIER)
            q = p;
        
        current = objective_c_property_declaration_translate_table[current][token->identifier];
        
        if (current == kObjectiveCPropertyDeclarationTranslateStateError) {
            break;
        }
        else if (current == kObjectiveCPropertyDeclarationTranslateStateProperty) {
            if (token->identifier == kCEETokenID_AT_PROPERTY && !property)
                property = p;
        }
        else if (current == kObjectiveCPropertyDeclarationTranslateStateAttributeEnd) {
            if (token->identifier == ')' && !attribute_end)
                attribute_end = p;
        }
        else if (current == kObjectiveCPropertyDeclarationTranslateStateCommit ||
                 current == kObjectiveCPropertyDeclarationTranslateStateTerminate) {
            if (q) {
                if (!proto) {
                    if (attribute_end)
                        proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                                                 TOKEN_NEXT(attribute_end), 
                                                                 TOKEN_PREV(q));
                    else 
                        proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                                                 TOKEN_NEXT(property), 
                                                                 TOKEN_PREV(q));
                }
                
                declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                        fregment->subject_ref,
                                                                        q,
                                                                        q,
                                                                        kCEESourceSymbolTypeProperty,
                                                                        "c");
                if (declaration) {
                    declaration->proto = cee_strdup(proto);
                    declarations = cee_list_prepend(declarations, declaration);
                    
                    /** create member variable */
                    member_variable = cee_source_symbol_copy(declaration);
                    member_variable->name = cee_strconcat("_", declaration->name, NULL);
                    member_variable->type = kCEESourceSymbolTypeVariableDeclaration;
                    declarations = cee_list_prepend(declarations, member_variable);
                }
                cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
                
                
                q = NULL;
            }
            
            if (current == kObjectiveCPropertyDeclarationTranslateStateTerminate)
                break;
            
            current = kObjectiveCPropertyDeclarationTranslateStateProperty;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (proto)
        cee_free(proto);
    
    if (declarations) {
        fregment->symbols = cee_list_concat(declarations, fregment->symbols);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_PROPERTY_DECLARATION
    cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static void objective_c_message_declaration_translate_table_init(void)
{
    /**
     *                      @optional   @required       -           +           identifier  (               )                   :           ;           others
     *  Init                Init        Init            Message     Message     Error       Error           Error               Error       Terminate   Error
     *  Message             Error       Error           Error       Error       Error       ReturnType      Error               Error       Error       Error
     *  ReturnType          Error       Error           Error       Error       Error       Error           ReturnTypeEnd       Error       Error       Error
     *  ReturnTypeEnd       Error       Error           Error       Error       Component   Error           Error               Error       Error       Error
     *  Component           Error       Error           Error       Error       Error       Error           Error               Colon       Terminate   Error
     *  Colon               Error       Error           Error       Error       Error       ParameterType   Error               Error       Error       Error
     *  ParameterType       Error       Error           Error       Error       Error       Error           ParameterTypeEnd    Error       Error       Error
     *  ParameterTypeEnd    Error       Error           Error       Error       Parameter   Error           Error               Error       Error       Error
     *  Parameter           Error       Error           Error       Error       Component   Error           Error               Error       Terminate   Error
     */
    TRANSLATE_STATE_INI(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateMax                 , kObjectiveCMessageDeclarationTranslateStateError                                          );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateInit                , kCEETokenID_AT_OPTIONAL   , kObjectiveCMessageDeclarationTranslateStateInit               );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateInit                , kCEETokenID_AT_REQUIRED   , kObjectiveCMessageDeclarationTranslateStateInit               );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateInit                , '-'                       , kObjectiveCMessageDeclarationTranslateStateMessage            );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateInit                , '+'                       , kObjectiveCMessageDeclarationTranslateStateMessage            );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateInit                , ';'                       , kObjectiveCMessageDeclarationTranslateStateTerminate          );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateMessage             , '('                       , kObjectiveCMessageDeclarationTranslateStateReturnType         );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateReturnType          , ')'                       , kObjectiveCMessageDeclarationTranslateStateReturnTypeEnd      );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateReturnTypeEnd       , kCEETokenID_IDENTIFIER    , kObjectiveCMessageDeclarationTranslateStateComponent          );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateComponent           , ':'                       , kObjectiveCMessageDeclarationTranslateStateColon              );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateComponent           , ';'                       , kObjectiveCMessageDeclarationTranslateStateTerminate          );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateColon               , '('                       , kObjectiveCMessageDeclarationTranslateStateParameterType      );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateParameterType       , ')'                       , kObjectiveCMessageDeclarationTranslateStateParameterTypeEnd   );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateParameterTypeEnd    , kCEETokenID_IDENTIFIER    , kObjectiveCMessageDeclarationTranslateStateParameter          );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateParameter           , kCEETokenID_IDENTIFIER    , kObjectiveCMessageDeclarationTranslateStateComponent          );
    TRANSLATE_STATE_SET(objective_c_message_declaration_translate_table, kObjectiveCMessageDeclarationTranslateStateParameter           , ';'                       , kObjectiveCMessageDeclarationTranslateStateTerminate          );
}

static cee_boolean objective_c_message_declaration_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    CEEList* p = NULL;
    CEEToken* token = NULL;
    ObjectiveCMessageDeclarationTranslateState current = kObjectiveCMessageDeclarationTranslateStateInit;
    CEEList* components = NULL;
    CEEList* parameters = NULL;
    CEEList* q = NULL;
    cee_int parameter_index = 0;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        current = objective_c_message_declaration_translate_table[current][token->identifier];
        
        if (current == kObjectiveCMessageDeclarationTranslateStateError) {
            break;
        }
        else if (current == kObjectiveCMessageDeclarationTranslateStateComponent) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(components, token);
        }
        else if (current == kObjectiveCMessageDeclarationTranslateStateColon) {
            if (token->identifier == ':')
                TOKEN_APPEND(components, token);
        }
        else if (current == kObjectiveCMessageDeclarationTranslateStateParameter) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                TOKEN_APPEND(parameters, token);
        }
        else if (current == kObjectiveCMessageDeclarationTranslateStateTerminate) {
            if (components) {
                declaration = cee_source_symbol_create_from_tokens(fregment->filepath_ref,
                                                                   fregment->subject_ref,
                                                                   components, 
                                                                   kCEESourceSymbolTypeMessageDeclaration, 
                                                                   "c");
                if (declaration) {
                    declaration->proto = objective_c_message_protos_dump(fregment);
                    declarations = cee_list_prepend(declarations, declaration);
                }
                cee_tokens_state_mark(components, kCEETokenStateSymbolOccupied);
                cee_list_free(components);
            }
            
            if (parameters) {
                q = TOKEN_FIRST(parameters);
                parameter_index = 0;
                while (q) {
                    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                            fregment->subject_ref,
                                                                            q, 
                                                                            q, 
                                                                            kCEESourceSymbolTypeMessageParameter, 
                                                                            "c");
                    if (declaration) {
                        declaration->proto = objective_c_message_parameter_protos_dump(fregment,
                                                                                       parameter_index);
                        declarations = cee_list_prepend(declarations, declaration);
                    }
                    q = TOKEN_NEXT(q);
                    parameter_index ++;
                }
                cee_tokens_state_mark(parameters, kCEETokenStateSymbolOccupied);
                cee_list_free(parameters);
            }
            break;
        }
        p = TOKEN_NEXT(p);
    }
    
    if (declarations) {
        fregment->symbols = cee_list_concat(declarations, fregment->symbols);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }

#ifdef DEBUG_MESSAGE_DECLARATION
    cee_source_symbols_print(fregment->symbols);
#endif
    return ret;
}

static cee_char* objective_c_message_protos_dump(CEESourceFregment* fregment)
{
    cee_char* proto = NULL;
    CEEList* p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    if (!p)
        return NULL;
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(p->data);
    if (!p)
        return NULL;
    
    proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                             SOURCE_FREGMENT_TOKENS_FIRST(p->data), 
                                             NULL);
    return proto;
}

static cee_char* objective_c_message_parameter_protos_dump(CEESourceFregment* fregment,
                                                           cee_int parameter_index)
{   
    cee_char* proto = NULL;
    cee_int i = 0;
    CEEList* p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    if (!p)
        return NULL;
    
    /** first child is return type */
    p = SOURCE_FREGMENT_NEXT(p);
    while (p) {
        if (i == parameter_index)
            break;
        p = SOURCE_FREGMENT_NEXT(p);
        i ++;
    }
    
    if (!p)
        return NULL;
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(p->data);
    if (!p)
        return NULL;
    
    proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                             SOURCE_FREGMENT_TOKENS_FIRST(p->data), 
                                             NULL);
    return proto;
}

static void c_protos_translate_table_init(void)
{
    /**
     *                                  declaration_specifier   decltype_specifier  builtin_type        object_specifier    alignas_specifier   access_specifier    identifier                  ::                              <                           >           (                   )               {                   }                       *                   &                   &&                  [                   ]                   :                   ,               others
     *  Init                            Init                    Decltype            BuiltinCommit       ObjectSpecifier     *AlignasSpecifier   AccessSpecifier     CustomType                  CustomTypeScope                 Error                       Error       Error               Error           Error               Error                   Error               Error               Error               Init                Init                Error               Error           Error
     *  AccessSpecifier                 AccessSpecifier         Decltype            BuiltinCommit       ObjectSpecifier     *AlignasSpecifier   AccessSpecifier     CustomType                  CustomTypeScope                 Error                       Error       Error               Error           Error               Error                   Error               Error               Error               AccessSpecifier     AccessSpecifier     AccessSpecifier     Error           Error
     *  Decltype                        Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                       Error       DecltypeList        Error           Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  DecltypeList                    Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                       Error       Error               DecltypeCommit  Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  CustomType                      CustomType              Decltype            BuiltinCommit       ObjectSpecifier     *AlignasSpecifier   AccessSpecifier     CustomTypeCommit            CustomTypeScope                 *TemplateParameter          Error       Error               Error           Error               Error                   CustomType          CustomType          CustomType          CustomType          CustomType          Error               Error           Error
     *  CustomTypeScope                 Error                   Error               CustomType          Error               Error               Error               CustomType                  Error                           Error                       Error       Error               Error           Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  AlignasSpecifier                Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                       Error       AlignasList         Error           Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  AlignasList                     Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                       Error       Error               *AlignasListEnd Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  ObjectSpecifier                 Error                   Error               Error               ObjectSpecifier     *AlignasSpecifier   Error               ObjectIdentifier            ObjectIdentifierScope           Error                       Error       Error               Error           ObjectDefinition    Error                   Error               Error               Error               ObjectSpecifier     ObjectSpecifier     Error               Error           Error
     *  ObjectIdentifier                Error                   Error               Error               Error               Error               Error               ObjectIdentifierCommit      ObjectIdentifierScope           *TemplateParameter          Error       Error               Error           ObjectDefinition    Error                   ObjectIdentifier    ObjectIdentifier    ObjectIdentifier    ObjectIdentifier    ObjectIdentifier    ObjectDerive        Error           Error
     *  ObjectIdentifierScope           Error                   Error               Error               Error               Error               Error               ObjectIdentifier            Error                           Error                       Error       Error               Error           Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  ObjectDerive                    Error                   Error               Error               Error               Error               ObjectDerive        ObjectDeriveIdentifier      ObjectDeriveIdentifierScope     Error                       Error       Error               Error           Error               Error                   Error               Error               Error               Error               Error               Error               ObjectDerive    Error
     *  ObjectDeriveIdentifier          Error                   Error               Error               Error               Error               Error               Error                       ObjectDeriveIdentifierScope     *TemplateParameter          Error       Error               Error           ObjectDefinition    Error                   Error               Error               Error               Error               Error               Error               ObjectDerive    Error
     *  ObjectDeriveIdentifierScope     Error                   Error               Error               Error               Error               Error               ObjectDeriveIdentifier      Error                           Error                       Error       Error               Error           Error               Error                   Error               Error               Error               Error               Error               Error               Error           Error
     *  ObjectDefinition                Error                   Error               Error               Error               Error               Error               Error                       Error                           Error                       Error       Error               Error           Error               ObjectDefinitionCommit  Error               Error               Error               Error               Error               Error               Error           Error
     *  
     *  
     *  AlignasSpecifier: save 'current state'
     *  AlignasListEnd: restore 'current state'
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'
     *
     */
    TRANSLATE_STATE_INI(c_protos_translate_table, kCProtosTranslateStateMax                         , kCProtosTranslateStateError                                                           );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , token_id_is_declaration_specifier , kCProtosTranslateStateInit                        );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_DECLTYPE              , kCProtosTranslateStateDecltype                    );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , token_id_is_builtin_type          , kCProtosTranslateStateBuiltinCommit               );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , token_id_is_object_sepcifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , token_id_is_access_specifier      , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , kCEETokenID_SCOPE                 , kCProtosTranslateStateCustomTypeScope             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , '['                               , kCProtosTranslateStateInit                        );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateInit                        , ']'                               , kCProtosTranslateStateInit                        );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_declaration_specifier , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_DECLTYPE              , kCProtosTranslateStateDecltype                    );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_builtin_type          , kCProtosTranslateStateBuiltinCommit               );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_object_sepcifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , token_id_is_access_specifier      , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , kCEETokenID_SCOPE                 , kCProtosTranslateStateCustomTypeScope             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , '['                               , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , ']'                               , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAccessSpecifier             , ':'                               , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateDecltype                    , '('                               , kCProtosTranslateStateDecltypeList                );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateDecltypeList                , ')'                               , kCProtosTranslateStateDecltypeCommit              );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_declaration_specifier , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_DECLTYPE              , kCProtosTranslateStateDecltype                    );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_builtin_type          , kCProtosTranslateStateBuiltinCommit               );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_object_sepcifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , token_id_is_access_specifier      , kCProtosTranslateStateAccessSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomTypeCommit            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_SCOPE                 , kCProtosTranslateStateCustomTypeScope             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , '<'                               , kCProtosTranslateStateTemplateParameter           );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , '*'                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , '&'                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , kCEETokenID_LOGIC_AND             , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , '['                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomType                  , ']'                               , kCProtosTranslateStateCustomType                  );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateCustomTypeScope             , token_id_is_builtin_type          , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateCustomTypeScope             , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateCustomType                  );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAlignasSpecifier            , '('                               , kCProtosTranslateStateAlignasList                 );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateAlignasList                 , ')'                               , kCProtosTranslateStateAlignasListEnd              );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateObjectSpecifier             , token_id_is_object_sepcifier      , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectSpecifier             , kCEETokenID_ALIGNAS               , kCProtosTranslateStateAlignasSpecifier            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectSpecifier             , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectSpecifier             , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectIdentifierScope       );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectSpecifier             , '{'                               , kCProtosTranslateStateObjectDefinition            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectSpecifier             , '['                               , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectSpecifier             , ']'                               , kCProtosTranslateStateObjectSpecifier             );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectIdentifierCommit      );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectIdentifierScope       );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , '<'                               , kCProtosTranslateStateTemplateParameter           );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , '{'                               , kCProtosTranslateStateObjectDefinition            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , '*'                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , '&'                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , kCEETokenID_LOGIC_AND             , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , '['                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , ']'                               , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifier            , ':'                               , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectIdentifierScope       , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectIdentifier            );
    TRANSLATE_FUNCS_SET(c_protos_translate_table, kCProtosTranslateStateObjectDerive                , token_id_is_access_specifier      , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDerive                , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectDeriveIdentifier      );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDerive                , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectDeriveIdentifierScope );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDerive                , ','                               , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , kCEETokenID_SCOPE                 , kCProtosTranslateStateObjectDeriveIdentifierScope );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , '<'                               , kCProtosTranslateStateTemplateParameter           );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , '{'                               , kCProtosTranslateStateObjectDefinition            );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDeriveIdentifier      , ','                               , kCProtosTranslateStateObjectDerive                );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDeriveIdentifierScope , kCEETokenID_IDENTIFIER            , kCProtosTranslateStateObjectDeriveIdentifier      );
    TRANSLATE_STATE_SET(c_protos_translate_table, kCProtosTranslateStateObjectDefinition            , '}'                               , kCProtosTranslateStateObjectDefinitionCommit      );
}

static cee_char* c_protos_string_from_token_slice(const cee_uchar* subject,
                                                  CEEList* p,
                                                  CEEList* q)
{
    CProtosTranslateState prev = kCProtosTranslateStateInit;
    CProtosTranslateState current = kCProtosTranslateStateInit;
    CProtosTranslateState alignas_prev_status = kCProtosTranslateStateInit;
    cee_char* protos_str = NULL;
    CEEToken* token = NULL;
            
    while (p) {
        token = p->data;

        prev = current;
        current = c_protos_translate_table[current][token->identifier];
        
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
        else if (current == kCProtosTranslateStateBuiltinCommit) {
            if (protos_str)
                cee_free(protos_str);
            protos_str = NULL;
            protos_str = cee_string_from_token(subject, token);
            break;
        }
        else if (current == kCProtosTranslateStateDecltype) {
            if (protos_str)
                cee_free(protos_str);
            protos_str = NULL;
            
            if (token->identifier == kCEETokenID_DECLTYPE)
                cee_string_concat_with_token(&protos_str, subject, token);
        }
        else if (current == kCProtosTranslateStateDecltypeList) {
            if (token->identifier == '(')
                cee_string_concat_with_token(&protos_str, subject, token);
        }
        else if (current == kCProtosTranslateStateDecltypeCommit) {
            if (token->identifier == ')')
                cee_string_concat_with_token(&protos_str, subject, token);
            break;
        }
        else if (current == kCProtosTranslateStateCustomType) {
            if (token->identifier == kCEETokenID_IDENTIFIER ||
                token_id_is_builtin_type(token->identifier))
                cee_string_concat_with_token(&protos_str, subject, token);
        }
        else if (current == kCProtosTranslateStateCustomTypeScope) {
            if (token->identifier == kCEETokenID_SCOPE)
                cee_string_concat_with_token(&protos_str, subject, token);
        }
        else if (current == kCProtosTranslateStateCustomTypeCommit) {
            cee_strconcat0(&protos_str, ",", NULL);
            if (token->identifier == kCEETokenID_IDENTIFIER)
                cee_string_concat_with_token(&protos_str, subject, token);
            current = kCProtosTranslateStateCustomType;
        }
        else if (current == kCProtosTranslateStateObjectSpecifier) {
            if (protos_str)
                cee_free(protos_str);
            protos_str = NULL;
        }
        else if (current == kCProtosTranslateStateObjectIdentifier) {
            if (token->identifier == kCEETokenID_IDENTIFIER)
                cee_string_concat_with_token(&protos_str, subject, token);
        }
        else if (current == kCProtosTranslateStateObjectIdentifierScope) {
            if (token->identifier == kCEETokenID_SCOPE)
                cee_string_concat_with_token(&protos_str, subject, token);
        }
        else if (current == kCProtosTranslateStateObjectIdentifierCommit) {
            cee_strconcat0(&protos_str, ",", NULL);
            if (token->identifier == kCEETokenID_IDENTIFIER)
                cee_string_concat_with_token(&protos_str, subject, token);
            current = kCProtosTranslateStateObjectIdentifier;
        }
        else if (current == kCProtosTranslateStateObjectDefinitionCommit) {
            break;
        }
        
        if (q && p == q)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCProtosTranslateStateError) {
        if (protos_str) {
            cee_free(protos_str);
            protos_str = NULL;
        }
    }
    
    return protos_str;
}

static void c_declaration_translate_table_init(void)
{
    /**
     *                              typedef     declaration_specifier   literal                 virtual         decltype_specifier  access_specifier    builtin_type    object_specifier    alignas_specifier       overload_operator   new             delete          virtual_specifier   identifier              noexcept    throw       assign_operator     <                   >           ::                          *                       &                       &&                      ~                       (                       )                           [                       ]                       :                   operator                ,               ;               {                   }                       ->          Const           others
     *  Init                        Typedef     DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType     ObjectSpecifier     AlignasSpecifier        Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   Init                    Error                   Error                       Init                    Init                    Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  Typedef                     Error       DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType     ObjectSpecifier     AlignasSpecifier        Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   Init                    Error                   Error                       Init                    Init                    Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  DeclarationSpecifier        Error       DeclarationSpecifier    DeclarationSpecifier    Error           Decltype            Error               BuiltinType     objectSpecifier     Error                   Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   DeclarationSpecifier    Error                   Error                       DeclarationSpecifier    DeclarationSpecifier    Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  AccessSpecifier             Typedef     DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType     ObjectSpecifier     AlignasSpecifier        Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       CustomTypeScope             Error                   Error                   Error                   AccessSpecifier         Error                   Error                       AccessSpecifier         AccessSpecifier         AccessSpecifier     Error                   Error           Error           Error               Error                   Error       Error           Error
     *  BuiltinType                 Error       BuiltinType             Error                   Error           Decltype            Error               BuiltinType     Error               Error                   Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       IdentifierScope             BuiltinType             BuiltinType             BuiltinType             BuiltinType             ParameterList           Error                       BuiltinType             BuiltinType             Error               Operator                Error           Error           Error               Error                   Error       Error           Error
     *  CustomType                  Error       CustomType              Error                   Error           Decltype            AccessSpecifier     BuiltinType     ObjectSpecifier     Error                   Error               Error           Error           Error               Identifier              Error       Error       Error               *TemplateParameter  Error       CustomTypeScope             CustomType              CustomType              CustomType              CustomType              Structor                Error                       CustomType              CustomType              Error               Operator                Error           Error           Error               Error                   Error       Error           Error
     *  Structor                    Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   StructorEnd                 Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  StructorEnd                 Error       StructorEnd             Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           StructorEnd         StructorEnd             Noexcept    Throw       Commit              Error               Error       Error                       StructorEnd             StructorEnd             StructorEnd             Error                   Surrounded              Error                       Surrounded              Error                   Error               Error                   Commit          Commit          Error               Error                   *Trailing   Error           Error
     *  CustomTypeScope             Error       CustomType              Error                   Error           Error               Error               CustomType      Error               Error                   Error               Error           Error           Error               CustomType              Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  ObjectSpecifier             Error       Error                   Error                   Error           Error               Error               Error           ObjectSpecifier     ObjectAlignasSpecifier  Error               Error           Error           Error               ObjectIdentifier        Error       Error       Error               Error               Error       ObjectIdentifierScope       Error                   Error                   Error                   Error                   Error                   Error                       ObjectSpecifier         ObjectSpecifier         Error               Error                   Error           Error           ObjectDefinition    Error                   Error       Error           Error
     *  ObjectAlignasSpecifier      Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ObjectAlignasList       Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  ObjectAlignasList           Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   ObjectSpecifier             Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  ObjectIdentifier            Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               ObjectIdentifier        Error       Error       Commit              Error               Error       ObjectIdentifierScope       ObjectIdentifier        ObjectIdentifier        ObjectIdentifier        Error                   Error                   Error                       ObjectIdentifier        ObjectIdentifier        ObjectDerive        Error                   Commit          Commit          ObjectDefinition    Error                   Error       Error           Error
     *  ObjectIdentifierScope       Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               ObjectIdentifier        Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  ObjectDerive                Error       Error                   Error                   ObjectDerive    Error               ObjectDerive        Error           Error               Error                   Error               Error           Error           Error               ObjectDeriveIdentifier  Error       Error       Error               Error               Error       ObjectDeriveIdentifierScope Error                   Error                   Error                   Error                   Error                   Error                       ObjectDerive            ObjectDerive            Error               Error                   ObjectDerive    Error           Error               Error                   Error       Error           Error
     *  ObjectDeriveIdentifier      Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               *TemplateParameter  Error       ObjectDeriveIdentifierScope Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   ObjectDerive    Error           ObjectDefinition    Error                   Error       Error           Error
     *  ObjectDeriveIdentifierScope Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               ObjectDeriveIdentifier  Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  ObjectDefinition            Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   Error           Error           Error               ObjectDefinitionEnd     Error       Error           Error
     *  ObjectDefinitionEnd         Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       Error                       ObjectDefinitionEnd     ObjectDefinitionEnd     ObjectDefinitionEnd     Error                   Error                   Error                       ObjectDefinitionEnd     ObjectDefinitionEnd     Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  AlignasSpecifier            Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   AlignasList             Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  AlignasList                 Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Init                        Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  Decltype                    Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   DecltypeList            Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  DecltypeList                Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   BuiltinType                 Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  Identifier                  Error       DeclarationSpecifier    Error                   Error           Decltype            AccessSpecifier     BuiltinType     objectSpecifier     Error                   Error               Error           Error           Error               Identifier              Error       Error       Commit              *TemplateParameter  Error       IdentifierScope             Identifier              Identifier              Identifier              Identifier              ParameterList           Error                       Identifier              Identifier              BitField            Operator                Commit          Commit          InitList            Error                   Error       Error           Error
     *  IdentifierScope             Error       Identifier              Error                   Error           Error               Error               Identifier      Error               Error                   Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   IdentifierScope         Error                   Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  ParameterList               Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   ParameterListEnd            Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  ParameterListEnd            Error       ParameterListEnd        Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           ParameterListEnd    ParameterListEnd        Noexcept    Throw       Commit              Error               Error       Error                       ParameterListEnd        ParameterListEnd        ParameterListEnd        Error                   Surrounded              Error                       Surrounded              Error                   Error               Error                   Commit          Commit          Error               Error                   *Trailing   Error           Error
     *  InitList                    Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   Error           Error           Error               InitListEnd             Error       Error           Error
     *  InitListEnd                 Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   Commit          Commit          Error               Error                   Error       Error           Error
     *  Surrounded                  Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   SurroundedEnd               Error                   SurroundedEnd           Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  SurroundedEnd               Error       SurroundedEnd           Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           SurroundedEnd       SurroundedEnd           Noexcept    Throw       Commit              Error               Error       Error                       SurroundedEnd           SurroundedEnd           SurroundedEnd           Error                   Error                   Error                       SurroundedEnd           SurroundedEnd           Error               Error                   Commit          Commit          Error               Error                   *Trailing   Error           Error
     *  Noexcept                    Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   NoexceptList            Error                       Error                   Error                   Error               Error                   Commit          Commit          Error               Error                   *Trailing   Error           Error
     *  NoexceptList                Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   ParameterListEnd            Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  Throw                       Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ThrowList               Error                       Error                   Error                   Error               Error                   Commit          Commit          Error               Error                   *Trailing   Error           Error
     *  ThrowList                   Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   ParameterListEnd            Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  TypeInit                    Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Identifier              Error       Error       Error               Error               Error       Error                       TypeCommited            TypeCommited            TypeCommited            TypeCommited            ParameterList           Error                       TypeCommited            TypeCommited            Error               Operator                Error           Error           Error               Error                   Error       Error           Error
     *  BitField                    Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               *BitSize                Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       *BitSize        Error
     *  Operator                    Error       Error                   Error                   Error           Error               Error               OverloadType    Error               Error                   OverloadOperator    OverloadNew     OverloadDelete  Error               OverloadType            Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   OverloadRoundBracket    Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadOperator            Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   OverloadOperator    Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList           Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadNew                 Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList           Error                       OverloadNew             OverloadNew             Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadDelete              Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList           Error                       OverloadDelete          OverloadDelete          Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadType                Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   OverloadTypeSurrounded  Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadTypeSurrounded      Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   Error                   OverloadTypeSurroundedEnd   Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadTypeSurroundedEnd   Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList           Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadRoundBracket        Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   OverloadRoundBracketEnd Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *  OverloadRoundBracketEnd     Error       Error                   Error                   Error           Error               Error               Error           Error               Error                   Error               Error           Error           Error               Error                   Error       Error       Error               Error               Error       Error                       Error                   Error                   Error                   Error                   ParameterList           Error                       Error                   Error                   Error               Error                   Error           Error           Error               Error                   Error       Error           Error
     *
     *  TemplateParameter: save 'current state', skip template parameterlist then restore 'current state'
     *  Assignment: skip to Commit/Terminate
     *  Trailing: skip to Commit/Terminate
     *  BitSize: set any current to 'Identifier' after BitSize
     */
    TRANSLATE_STATE_INI(c_declaration_translate_table, kCDeclarationTranslateStateMax                           , kCDeclarationTranslateStateError                                                              );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , token_id_is_object_sepcifier          , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , token_id_is_alignas_specifier         , kCDeclarationTranslateStateAlignasSpecifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , '~'                                   , kCDeclarationTranslateStateInit                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , '['                                   , kCDeclarationTranslateStateInit                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , ']'                                   , kCDeclarationTranslateStateInit                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInit                          , kCEETokenID_TYPEDEF                   , kCDeclarationTranslateStateTypedef                    );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , token_id_is_object_sepcifier          , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , token_id_is_alignas_specifier         , kCDeclarationTranslateStateAlignasSpecifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , '~'                                   , kCDeclarationTranslateStateInit                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , '['                                   , kCDeclarationTranslateStateInit                       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypedef                       , ']'                                   , kCDeclarationTranslateStateInit                       );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , kCEETokenID_LITERAL                   , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , token_id_is_object_sepcifier          , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , '~'                                   , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , '['                                   , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDeclarationSpecifier          , ']'                                   , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , kCEETokenID_TYPEDEF                   , kCDeclarationTranslateStateTypedef                    );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , token_id_is_object_sepcifier          , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , token_id_is_alignas_specifier         , kCDeclarationTranslateStateAlignasSpecifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , '~'                                   , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , '['                                   , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , ']'                                   , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateAccessSpecifier               , ':'                                   , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , token_id_is_declaration_specifier     , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateIdentifierScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , '*'                                   , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , '&'                                   , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , '~'                                   , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , '('                                   , kCDeclarationTranslateStateParameterList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , '['                                   , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , ']'                                   , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBuiltinType                   , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , token_id_is_declaration_specifier     , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , token_id_is_object_sepcifier          , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , '<'                                   , kCDeclarationTranslateStateTemplateParameter          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateCustomTypeScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , '*'                                   , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , '&'                                   , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , '~'                                   , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , '('                                   , kCDeclarationTranslateStateStructor                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , '['                                   , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , ']'                                   , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomType                    , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructor                      , ')'                                   , kCDeclarationTranslateStateStructorEnd                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , token_id_is_declaration_specifier     , kCDeclarationTranslateStateStructorEnd                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , token_id_is_virtual_sepcifier         , kCDeclarationTranslateStateStructorEnd                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateStructorEnd                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , kCEETokenID_NOEXCEPT                  , kCDeclarationTranslateStateNoexcept                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , kCEETokenID_THROW                     , kCDeclarationTranslateStateThrow                      );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , token_id_is_assignment                , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , '*'                                   , kCDeclarationTranslateStateStructorEnd                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , '&'                                   , kCDeclarationTranslateStateStructorEnd                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateStructorEnd                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , '('                                   , kCDeclarationTranslateStateSurrounded                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , '['                                   , kCDeclarationTranslateStateSurrounded                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateStructorEnd                   , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeScope               , token_id_is_declaration_specifier     , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeScope               , token_id_is_builtin_type              , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateCustomTypeScope               , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateCustomType                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier               , token_id_is_alignas_specifier         , kCDeclarationTranslateStateObjectAlignasSpecifier     );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier               , token_id_is_object_sepcifier          , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier               , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectIdentifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier               , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectIdentifierScope      );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier               , '['                                   , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier               , ']'                                   , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectSpecifier               , '{'                                   , kCDeclarationTranslateStateObjectDefinition           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectAlignasSpecifier        , '('                                   , kCDeclarationTranslateStateObjectAlignasList          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectAlignasList             , ')'                                   , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , token_id_is_assignment                , kCDeclarationTranslateStateCommit                     )
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectIdentifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectIdentifierScope      );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , ':'                                   , kCDeclarationTranslateStateObjectDerive               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , '{'                                   , kCDeclarationTranslateStateObjectDefinition           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , '*'                                   , kCDeclarationTranslateStateObjectIdentifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , '&'                                   , kCDeclarationTranslateStateObjectIdentifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , '['                                   , kCDeclarationTranslateStateObjectIdentifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifier              , ']'                                   , kCDeclarationTranslateStateObjectIdentifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectIdentifierScope         , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectIdentifier           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                  , kCEETokenID_VIRTUAL                   , kCDeclarationTranslateStateObjectDerive               );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                  , token_id_is_access_specifier          , kCDeclarationTranslateStateObjectDerive               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                  , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectDeriveIdentifier     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                  , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectDeriveIdentifierScope);
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                  , ','                                   , kCDeclarationTranslateStateObjectDerive               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                  , '['                                   , kCDeclarationTranslateStateObjectDerive               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDerive                  , ']'                                   , kCDeclarationTranslateStateObjectDerive               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier        , '<'                                   , kCDeclarationTranslateStateTemplateParameter          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier        , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateObjectDeriveIdentifierScope);
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier        , ','                                   , kCDeclarationTranslateStateObjectDerive               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifier        , '{'                                   , kCDeclarationTranslateStateObjectDefinition           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDeriveIdentifierScope   , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateObjectDeriveIdentifier     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinition              , '}'                                   , kCDeclarationTranslateStateObjectDefinitionEnd        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd           , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd           , '*'                                   , kCDeclarationTranslateStateObjectDefinitionEnd        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd           , '&'                                   , kCDeclarationTranslateStateObjectDefinitionEnd        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd           , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateObjectDefinitionEnd        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd           , '['                                   , kCDeclarationTranslateStateObjectDefinitionEnd        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateObjectDefinitionEnd           , ']'                                   , kCDeclarationTranslateStateObjectDefinitionEnd        );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDecltype                      , '('                                   , kCDeclarationTranslateStateDecltypeList               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateDecltypeList                  , ')'                                   , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , token_id_is_declaration_specifier     , kCDeclarationTranslateStateDeclarationSpecifier       );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , kCEETokenID_DECLTYPE                  , kCDeclarationTranslateStateDecltype                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , token_id_is_access_specifier          , kCDeclarationTranslateStateAccessSpecifier            );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , token_id_is_builtin_type              , kCDeclarationTranslateStateBuiltinType                );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , token_id_is_object_sepcifier          , kCDeclarationTranslateStateObjectSpecifier            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , token_id_is_assignment                , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , ':'                                   , kCDeclarationTranslateStateBitField                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , '<'                                   , kCDeclarationTranslateStateTemplateParameter          );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , kCEETokenID_SCOPE                     , kCDeclarationTranslateStateIdentifierScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , '*'                                   , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , '&'                                   , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , '~'                                   , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , '('                                   , kCDeclarationTranslateStateParameterList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , '['                                   , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , ']'                                   , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifier                    , '{'                                   , kCDeclarationTranslateStateInitList                   );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope               , token_id_is_declaration_specifier     , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope               , token_id_is_builtin_type              , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope               , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateIdentifierScope               , '~'                                   , kCDeclarationTranslateStateIdentifierScope            );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterList                 , ')'                                   , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , token_id_is_declaration_specifier     , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , token_id_is_virtual_sepcifier         , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , kCEETokenID_NOEXCEPT                  , kCDeclarationTranslateStateNoexcept                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , kCEETokenID_THROW                     , kCDeclarationTranslateStateThrow                      );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , token_id_is_assignment                , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , '*'                                   , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , '&'                                   , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , '('                                   , kCDeclarationTranslateStateSurrounded                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , '['                                   , kCDeclarationTranslateStateSurrounded                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateParameterListEnd              , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInitList                      , '}'                                   , kCDeclarationTranslateStateInitListEnd                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInitListEnd                   , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateInitListEnd                   , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurrounded                    , ')'                                   , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurrounded                    , ']'                                   , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , token_id_is_assignment                , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , token_id_is_virtual_sepcifier         , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , kCEETokenID_NOEXCEPT                  , kCDeclarationTranslateStateNoexcept                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , kCEETokenID_THROW                     , kCDeclarationTranslateStateThrow                      );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , token_id_is_assignment                , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , '*'                                   , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , '&'                                   , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , '['                                   , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , ']'                                   , kCDeclarationTranslateStateSurroundedEnd              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateSurroundedEnd                 , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                      , '('                                   , kCDeclarationTranslateStateNoexceptList               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                      , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                      , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexcept                      , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateNoexceptList                  , ')'                                   , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                         , '('                                   , kCDeclarationTranslateStateThrowList                  );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                         , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                         , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrow                         , kCEETokenID_MEMBER_POINTER            , kCDeclarationTranslateStateTrailing                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateThrowList                     , ')'                                   , kCDeclarationTranslateStateParameterListEnd           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateIdentifier                 );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , '*'                                   , kCDeclarationTranslateStateTypeInit                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , '&'                                   , kCDeclarationTranslateStateTypeInit                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , kCEETokenID_LOGIC_AND                 , kCDeclarationTranslateStateTypeInit                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , '~'                                   , kCDeclarationTranslateStateTypeInit                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , '('                                   , kCDeclarationTranslateStateParameterList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , '['                                   , kCDeclarationTranslateStateTypeInit                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , ']'                                   , kCDeclarationTranslateStateTypeInit                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateTypeInit                      , kCEETokenID_OPERATOR                  , kCDeclarationTranslateStateOperator                   );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitField                      , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateBitSize                    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitField                      , kCEETokenID_CONSTANT                  , kCDeclarationTranslateStateBitSize                    );
    TRANSLATE_STATE_ANY(c_declaration_translate_table, kCDeclarationTranslateStateBitSize                       , kCDeclarationTranslateStateIdentifier                                                         );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitSize                       , ','                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateBitSize                       , ';'                                   , kCDeclarationTranslateStateCommit                     );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                      , token_id_is_builtin_type              , kCDeclarationTranslateStateOverloadType               );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                      , token_id_is_overload_operator         , kCDeclarationTranslateStateOverloadOperator           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                      , kCEETokenID_IDENTIFIER                , kCDeclarationTranslateStateOverloadType               );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                      , kCEETokenID_NEW                       , kCDeclarationTranslateStateOverloadNew                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                      , kCEETokenID_DELETE                    , kCDeclarationTranslateStateOverloadDelete             );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOperator                      , '('                                   , kCDeclarationTranslateStateOverloadRoundBracket       );
    TRANSLATE_FUNCS_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadOperator              , token_id_is_overload_operator         , kCDeclarationTranslateStateOverloadOperator           );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadOperator              , '('                                   , kCDeclarationTranslateStateParameterList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadNew                   , '['                                   , kCDeclarationTranslateStateOverloadNew                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadNew                   , ']'                                   , kCDeclarationTranslateStateOverloadNew                );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadNew                   , '('                                   , kCDeclarationTranslateStateParameterList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadDelete                , '['                                   , kCDeclarationTranslateStateOverloadDelete             );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadDelete                , ']'                                   , kCDeclarationTranslateStateOverloadDelete             );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadDelete                , '('                                   , kCDeclarationTranslateStateParameterList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadType                  , '('                                   , kCDeclarationTranslateStateOverloadTypeSurrounded     );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadTypeSurrounded        , ')'                                   , kCDeclarationTranslateStateOverloadTypeSurroundedEnd  );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadTypeSurroundedEnd     , '('                                   , kCDeclarationTranslateStateParameterList              );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadRoundBracket          , ')'                                   , kCDeclarationTranslateStateOverloadRoundBracketEnd    );
    TRANSLATE_STATE_SET(c_declaration_translate_table, kCDeclarationTranslateStateOverloadRoundBracketEnd       , '('                                   , kCDeclarationTranslateStateParameterList              );
}

static cee_boolean c_declaration_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    cee_boolean is_class_member = FALSE;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEToken* token = NULL;
    CDeclarationTranslateState current = kCDeclarationTranslateStateInit;
    CDeclarationTranslateState prev = kCDeclarationTranslateStateInit;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    CEEList* identifier = NULL;
    CEEList* object_identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEEList* surrounded = FALSE;
    CEEList* surrounded_end = FALSE;
    CEEList* overload = NULL;
    cee_boolean is_typedef = FALSE;
    cee_char* proto = NULL;
    
    if (cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeClassDefinition))
        is_class_member = TRUE;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        prev = current;
        current = c_declaration_translate_table[current][token->identifier];
        
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
        else if (current == kCDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kCDeclarationTranslateStateStructor) {
            if (!is_class_member)
                break;
            
            if (!parameter_list &&
                cee_token_is_identifier(p, '('))
                parameter_list = p;
        }
        else if (current == kCDeclarationTranslateStateStructorEnd) {
            if (!parameter_list_end &&
                cee_token_is_identifier(p, ')'))
                parameter_list_end = p;
        }
        else if (current == kCDeclarationTranslateStateParameterList) {
            if (!parameter_list &&
                cee_token_is_identifier(p, '('))
                parameter_list = p;
        }
        else if (current == kCDeclarationTranslateStateParameterListEnd) {
            if (!parameter_list_end &&
                cee_token_is_identifier(p, ')'))
                parameter_list_end = p;
        }
        else if (current == kCDeclarationTranslateStateSurrounded) {
            if (!surrounded &&
                (cee_token_is_identifier(p, '(') ||
                 cee_token_is_identifier(p, '[')))
                surrounded = p;
        }
        else if (current == kCDeclarationTranslateStateSurroundedEnd) {
            if (!surrounded_end &&
                (cee_token_is_identifier(p, ')') ||
                 cee_token_is_identifier(p, ']')))
                surrounded_end = p;
        }
        else if (current == kCDeclarationTranslateStateOperator) {
            if (cee_token_is_identifier(p, kCEETokenID_OPERATOR))
                overload = p;
        }
        else if (current == kCDeclarationTranslateStateObjectIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                object_identifier = p;
        }
        else if (current == kCDeclarationTranslateStateTrailing ||
                 current == kCDeclarationTranslateStateCommit) {
            
            if (!proto)
                proto = c_declaration_protos_create(fregment,
                                                    overload,
                                                    surrounded,
                                                    parameter_list,
                                                    identifier,
                                                    object_identifier);
            if (overload)
                declaration = c_operator_overload_declaration_create(fregment,
                                                                     proto,
                                                                     overload,
                                                                     parameter_list);
            else if (surrounded)
                declaration = c_surrounded_declaration_create(fregment,
                                                              proto,
                                                              parameter_list);
            else if (parameter_list)
                declaration = c_function_declaration_create(fregment,
                                                            proto,
                                                            parameter_list);
            else if (identifier)
                declaration = c_identifier_declaration_create(fregment,
                                                              proto,
                                                              identifier);
            else if (object_identifier) {
                token = cee_token_not_whitespace_newline_before(object_identifier)->data;
                if (token_id_is_object_sepcifier(token->identifier))
                    declaration = c_custom_type_declaration_create(fregment,
                                                                   proto,
                                                                   object_identifier);
                else
                    declaration = c_identifier_declaration_create(fregment,
                                                                  proto,
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
            surrounded = NULL;
            surrounded_end = NULL;
            
            
            if (overload) {
                /** if it's "operator," skip the ',' */
                q = cee_token_not_whitespace_newline_after(overload);
                if (q && cee_token_is_identifier(q, ','))
                    p = TOKEN_NEXT(q);
                
                /**
                 * when "<<" overload, "skip_c_declaration_interval"
                 * will failed, so we find the declaration interval
                 * manually.
                 */
                while (p) {
                    if (cee_token_is_identifier(p, ',') ||
                        cee_token_is_identifier(p, ';'))
                        break;
                    p = TOKEN_NEXT(p);
                }
                if (!p)
                    break;
            }
            else {
                if (!cee_token_is_identifier(p, ',') &&
                    !cee_token_is_identifier(p, ';')) {
                    p = skip_c_declaration_interval(p);
                    if (!p)
                        break;
                }
            }
            
            current = kCDeclarationTranslateStateTypeInit;
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (proto)
        cee_free(proto);
    
    if (declarations) {
        fregment->symbols = cee_list_concat(fregment->symbols, declarations);
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_DECLARATION
    cee_source_symbols_print(declarations);
#endif
    
    return ret;
}

static cee_char* c_declaration_protos_create(CEESourceFregment* fregment,
                                             CEEList* overload,
                                             CEEList* surrounded,
                                             CEEList* parameter_list,
                                             CEEList* identifier,
                                             CEEList* object_identifier)
{
    cee_char* proto = NULL;
    CEEList* first_commit = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    
    if (overload) {
        first_commit = overload;
    }
    else if (surrounded) {
        first_commit = parameter_list;
    }
    else if (parameter_list) {
        q = cee_token_not_whitespace_newline_before(parameter_list);
        if (cee_token_is_identifier(q, kCEETokenID_IDENTIFIER)) {
            r = cee_token_not_whitespace_newline_before(q);
            if (cee_token_is_identifier(r, '~'))
                q = r;
        }
        first_commit = q;
    }
    else if (identifier) {
        first_commit = identifier;
    }
    else if (object_identifier) {
        first_commit = object_identifier;
    }
    
    if (!first_commit || !TOKEN_PREV(first_commit))
        proto = cee_strdup("?");
    else
        proto = c_protos_string_from_token_slice(fregment->subject_ref,
                                                 SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                 TOKEN_PREV(first_commit));
    return proto;
}

static CEESourceSymbol* c_function_declaration_create(CEESourceFregment* fregment,
                                                      const cee_char* proto,
                                                      CEEList* parameter_list)
{
    CEESourceSymbol* declaration = NULL;
    CEESourceFregment* child = NULL;
    CEEList* q = NULL;
    CEEList* r = NULL;
    
    q = cee_token_not_whitespace_newline_before(parameter_list);
    if (!cee_token_is_identifier(q, kCEETokenID_IDENTIFIER))
        return NULL;
    
    r = cee_token_not_whitespace_newline_before(q);
    if (!cee_token_is_identifier(r, '~'))
        r = q;
    
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            r,
                                                            q,
                                                            kCEESourceSymbolTypeFunctionDeclaration,
                                                            "c");
    cee_token_slice_state_mark(r, q, kCEETokenStateSymbolOccupied);
    
    child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
    if (child) {
        child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
        c_function_parameters_parse(child);
    }
    
    if (proto)
        declaration->proto = cee_strdup(proto);
    
    return declaration;
}

static CEESourceSymbol* c_custom_type_declaration_create(CEESourceFregment* fregment,
                                                         const cee_char* proto,
                                                         CEEList* identifier)
{
    CEESourceSymbol* declaration = NULL;
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            identifier,
                                                            identifier,
                                                            kCEESourceSymbolTypeCustomTypeDeclaration,
                                                            "c");
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    
    if (proto)
        declaration->proto = cee_strdup(proto);
    
    return declaration;
}

static CEESourceSymbol* c_identifier_declaration_create(CEESourceFregment* fregment,
                                                        const cee_char* proto,
                                                        CEEList* identifier)
{
    CEESourceSymbol* declaration = NULL;
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            identifier,
                                                            identifier,
                                                            kCEESourceSymbolTypeVariableDeclaration,
                                                            "c");
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    
    if (proto)
        declaration->proto = cee_strdup(proto);
    
    return declaration;
}

static CEESourceSymbol* c_operator_overload_declaration_create(CEESourceFregment* fregment,
                                                               const cee_char* proto,
                                                               CEEList* overload,
                                                               CEEList* parameter_list)
{
    CEESourceSymbol* declaration = NULL;
    CEESourceFregment* child = NULL;
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            overload,
                                                            overload,
                                                            kCEESourceSymbolTypeFunctionDeclaration,
                                                            "c");
    cee_token_slice_state_mark(overload, overload, kCEETokenStateSymbolOccupied);
    
    if (parameter_list) {
        child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
        if (child) {
            child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
            c_function_parameters_parse(child);
        }
    }
    
    if (proto)
        declaration->proto = cee_strdup(proto);
    
    return declaration;
}

static CEESourceSymbol* c_surrounded_declaration_create(CEESourceFregment* fregment,
                                                        const cee_char* proto,
                                                        CEEList* surrounded)
{
    CEESourceSymbol* declaration = NULL;
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
    
    p = tokens;
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
            break;
        p = TOKEN_NEXT(p);
    }
    
    if (p) {
        declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                fregment->subject_ref,
                                                                p, 
                                                                p,
                                                                kCEESourceSymbolTypeVariableDeclaration,
                                                                "c");
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        
        if (proto)
            declaration->proto = cee_strdup(proto);
    }
    cee_list_free(tokens);
    
    return declaration;
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
     *  Namespace       Error           Name            Namescope   Namespace       Namespace   InlineSpecifier     Commit  Alignas         Error           Error       Error
     *  Name            Error           Error           Namescope   Error           Error       Error               Commit  Error           Error           Error       Error
     *  Namescope       Error           Name            Error       Error           Error       InlineSpecifier     Error   Error           Error           Error       Error
     *  InlineSpecifier Error           Name            Error       Error           Error       Error               Error   Error           Error           Error       Error
     *  Alignas         Error           Error           Error       Error           Error       Error               Error   Error           AlignasBegin    Error       Error
     *  AlignasBegin    Error           Error           Error       Error           Error       Error               Error   Error           Error           Namespace   Error
     */
    
    TRANSLATE_STATE_INI(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateMax              , kCNamespaceDefinitionTranslateStateError                                              );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateInit             , kCEETokenID_NAMESPACE         , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_IDENTIFIER        , kCNamespaceDefinitionTranslateStateDefinitionName     );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_SCOPE             , kCNamespaceDefinitionTranslateStateNameScope          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , '['                           , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , ']'                           , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_INLINE            , kCNamespaceDefinitionTranslateStateInlineSpecifier    );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , '{'                           , kCNamespaceDefinitionTranslateStateCommit             );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNamespace        , kCEETokenID_ALIGNAS           , kCNamespaceDefinitionTranslateStateAlignas            );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateAlignas          , '('                           , kCNamespaceDefinitionTranslateStateAlignasBegin       );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateAlignasBegin     , ')'                           , kCNamespaceDefinitionTranslateStateNamespace          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateDefinitionName   , kCEETokenID_SCOPE             , kCNamespaceDefinitionTranslateStateNameScope          );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateDefinitionName   , '{'                           , kCNamespaceDefinitionTranslateStateCommit             );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNameScope        , kCEETokenID_IDENTIFIER        , kCNamespaceDefinitionTranslateStateDefinitionName     );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateNameScope        , kCEETokenID_INLINE            , kCNamespaceDefinitionTranslateStateInlineSpecifier    );
    TRANSLATE_STATE_SET(c_namespace_definition_translate_table, kCNamespaceDefinitionTranslateStateInlineSpecifier  , kCEETokenID_IDENTIFIER        , kCNamespaceDefinitionTranslateStateDefinitionName     );
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
        else if (current == kCNamespaceDefinitionTranslateStateCommit)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    if (current == kCNamespaceDefinitionTranslateStateCommit) {
        if (q) {
            definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                   fregment->subject_ref,
                                                                   q, 
                                                                   q, 
                                                                   kCEESourceSymbolTypeNamespaceDefinition, 
                                                                   "c");
            definition->proto = cee_string_from_token(fregment->subject_ref, q->data);
            cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
        }
        else {
            definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                   fregment->subject_ref, 
                                                                   trap_keyword, 
                                                                   trap_keyword, 
                                                                   kCEESourceSymbolTypeNamespaceDefinition, 
                                                                   "c");
        }
        
        if (definition) {
            fregment->symbols = cee_list_prepend(fregment->symbols, definition);
            cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeNamespaceDefinition);
            ret = TRUE;
        }
    }
    
    *pp = NULL;
    
#ifdef DEBUG_NAMESPACE
    cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static void c_extern_block_translate_table_init(void)
{
    /**
     *                  extern  literal      identifier     {       others
     *  Init            Extern  Error        Error          Error   Error
     *  Extern          Error   Identifier   Identifier     Error   Error
     *  Identifier      Error   Identifier   Identifier     Commit  Error
     */
    TRANSLATE_STATE_INI(c_extern_block_translate_table,     kCExternBlockTranslateStateMax              , kCExternBlockTranslateStateError                                                );
    TRANSLATE_STATE_SET(c_extern_block_translate_table,     kCExternBlockTranslateStateInit             , kCEETokenID_EXTERN              , kCExternBlockTranslateStateExtern             );
    TRANSLATE_STATE_SET(c_extern_block_translate_table,     kCExternBlockTranslateStateExtern           , kCEETokenID_LITERAL             , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table,     kCExternBlockTranslateStateExtern           , kCEETokenID_IDENTIFIER          , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table,     kCExternBlockTranslateStateIdentifier       , kCEETokenID_LITERAL             , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table,     kCExternBlockTranslateStateIdentifier       , kCEETokenID_IDENTIFIER          , kCExternBlockTranslateStateIdentifier         );
    TRANSLATE_STATE_SET(c_extern_block_translate_table,     kCExternBlockTranslateStateIdentifier       , '{'                             , kCExternBlockTranslateStateCommit             );
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
        else if (current == kCExternBlockTranslateStateCommit) {
            extern_block = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                     fregment->subject_ref,
                                                                     q,
                                                                     r,
                                                                     kCEESourceSymbolTypeExternBlock,
                                                                     "c");
            
            if (extern_block->name)
                cee_free(extern_block->name);
            extern_block->name = cee_string_from_token_slice(fregment->subject_ref,
                                                             q,
                                                             r,
                                                             kCEETokenStringOptionDefault);
            if (extern_block->locations)
                cee_free(extern_block->locations);
            extern_block->locations = cee_range_string_from_token(r->data);
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
        objective_c_message_definition_parse(current))
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
    
    if (c_statement_block_parse(current))
        return;
    
    return;
}

static cee_boolean block_reduce(CParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    
    if (cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeClassDefinition) ||
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

static void label_parse(CParser* parser)
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
                                                                            "c");
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

static cee_boolean label_reduce(CParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    if (cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeLabelDeclaration))
        statement_attach(parser, kCEESourceFregmentTypeStatement);
    
    return TRUE;
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
    TRANSLATE_STATE_INI(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateMax                  , kObjectiveCDefinitionTranslateStateError                                                      );
    TRANSLATE_STATE_ANY(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kObjectiveCDefinitionTranslateStateInit                                                       );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kCEETokenID_AT_INTERFACE          , kObjectiveCDefinitionTranslateStateInterface              );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kCEETokenID_AT_IMPLEMENTATION     , kObjectiveCDefinitionTranslateStateImplementation         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInit                 , kCEETokenID_AT_PROTOCOL           , kObjectiveCDefinitionTranslateStateProtocol               );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateInterface            , kCEETokenID_IDENTIFIER            , kObjectiveCDefinitionTranslateStateDefinitionName         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateImplementation       , kCEETokenID_IDENTIFIER            , kObjectiveCDefinitionTranslateStateDefinitionName         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocol             , kCEETokenID_IDENTIFIER            , kObjectiveCDefinitionTranslateStateDefinitionName         );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , '('                               , kObjectiveCDefinitionTranslateStateExtendBegin            );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , ':'                               , kObjectiveCDefinitionTranslateStateColon                  );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , '<'                               , kObjectiveCDefinitionTranslateStateProtocolListBegin      );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDefinitionName       , '{'                               , kObjectiveCDefinitionTranslateStateVariableBlock          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateColon                , kCEETokenID_IDENTIFIER            , kObjectiveCDefinitionTranslateStateDerived                );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDerived              , '<'                               , kObjectiveCDefinitionTranslateStateProtocolListBegin      );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateDerived              , '{'                               , kObjectiveCDefinitionTranslateStateVariableBlock          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendBegin          , kCEETokenID_IDENTIFIER            , kObjectiveCDefinitionTranslateStateExtend                 );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendBegin          , ')'                               , kObjectiveCDefinitionTranslateStateExtendEnd              );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtend               , ')'                               , kObjectiveCDefinitionTranslateStateExtendEnd              );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendEnd            , '<'                               , kObjectiveCDefinitionTranslateStateProtocolListBegin      );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateExtendEnd            , '{'                               , kObjectiveCDefinitionTranslateStateVariableBlock          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolListBegin    , kCEETokenID_IDENTIFIER            , kObjectiveCDefinitionTranslateStateProtocolNames          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolListBegin    , '>'                               , kObjectiveCDefinitionTranslateStateProtocolListEnd        );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolNames        , kCEETokenID_IDENTIFIER            , kObjectiveCDefinitionTranslateStateProtocolNames          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolNames        , ','                               , kObjectiveCDefinitionTranslateStateProtocolNames          );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolNames        , '>'                               , kObjectiveCDefinitionTranslateStateProtocolListEnd        );
    TRANSLATE_STATE_SET(objective_c_definition_translate_table, kObjectiveCDefinitionTranslateStateProtocolListEnd      , '{'                               , kObjectiveCDefinitionTranslateStateVariableBlock          );
    
}

static cee_boolean objective_c_token_push(CParser* parser,
                                          CEEToken* push)
{
    ObjectiveCDefinitionTranslateState current = kObjectiveCDefinitionTranslateStateInit;
    ObjectiveCDefinitionTranslateState prev = kObjectiveCDefinitionTranslateStateInit;
    const cee_uchar* subject = parser->subject_ref;
    
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
                                                const cee_uchar* subject)
{
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    ObjectiveCDefinitionType type = kObjectiveCDefinitionTypeInit;
    CEESourceSymbol* definition = NULL;
    
    while (p) {        
        if (cee_token_is_identifier(p, kCEETokenID_AT_INTERFACE)) {
            type = kObjectiveCDefinitionTypeInterface;
            definition = objective_c_interface_extract(fregment->tokens_ref,
                                                       fregment->filepath_ref,
                                                       subject);
            break;
        }
        else if (cee_token_is_identifier(p, kCEETokenID_AT_IMPLEMENTATION)) {
            type = kObjectiveCDefinitionTypeImplementation;
            definition = objective_c_implementation_extract(fregment->tokens_ref,
                                                            fregment->filepath_ref,
                                                            subject);
            break;
        }
        else if (cee_token_is_identifier(p, kCEETokenID_AT_PROTOCOL)) {
            type = kObjectiveCDefinitionTypeProtocol;
            definition = objective_c_protocol_extract(fregment->tokens_ref,
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

static CEESourceSymbol* objective_c_interface_extract(CEEList* tokens,
                                                      const cee_uchar* filepath,
                                                      const cee_uchar* subject)
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
                                                          "c");
    cee_token_slice_state_mark(name, name, kCEETokenStateSymbolOccupied);
    
    if (derives)
        interface->derives = c_name_list_create(derives, subject);
    
exit:
    if (derives)
        cee_list_free(derives);
    
#ifdef DEBUG_INTERFACE
    cee_source_symbol_print(interface);
#endif
    
    return interface;
}

static cee_char* c_name_list_create(CEEList* tokens,
                                    const cee_uchar* subject)
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

static CEESourceSymbol* objective_c_implementation_extract(CEEList* tokens,
                                                           const cee_uchar* filepath,
                                                           const cee_uchar* subject)
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
                                                               "c");
    cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
    
exit:
#ifdef DEBUG_IMPLEMENTATION
    cee_source_symbol_print(implementation);
#endif
    
    return implementation;
}

static CEESourceSymbol* objective_c_protocol_extract(CEEList* tokens,
                                                     const cee_uchar* filepath,
                                                     const cee_uchar* subject)
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
                                                         "c");
    cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
exit:
#ifdef DEBUG_PROTOCOL
    cee_source_symbol_print(protocol);
#endif
    
    return protocol;
}

/**
 * parser
 */
static void parser_block_header_trap_init(CParser* parser)
{
    
    for (int i = 0; i < kCEETokenID_END; i ++)
        parser->block_header_traps[i] = NULL;
    
    parser->block_header_traps[kCEETokenID_CLASS] = c_class_definition_trap;
    parser->block_header_traps[kCEETokenID_STRUCT] = c_class_definition_trap;
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
    
    CParser* c = parser_create();
    c->super = parser;
    
    parser_block_header_trap_init(c);
    c_token_type_map_init();
    
    c_prep_directive_include_translate_table_init();
    c_prep_directive_define_translate_table_init();
    objective_c_message_definition_translate_table_init();
    objective_c_definition_translate_table_init();
    objective_c_property_declaration_translate_table_init();
    objective_c_message_declaration_translate_table_init();
    c_function_definition_translate_table_init();
    c_function_parameters_declaration_translate_table_init();
    c_inheritance_definition_translate_table_init();
    c_template_parameters_declaration_translate_table_init();
    c_template_declaration_translate_table_init();
    c_declaration_translate_table_init();
    c_namespace_definition_translate_table_init();
    c_protos_translate_table_init();
    c_extern_block_translate_table_init();
    objective_c_enum_definition_translate_table_init();
    
    parser->imp = c;
        
    return parser;
}

static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenIdentifierType type)
{
    return (c_token_identifier_type_map[token->identifier] & type) != 0;
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
                              const cee_uchar* filepath,
                              const cee_uchar* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->prep_directive_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"c");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_root, 
                                                                    kCEESourceFregmentTypeSourceList, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    (const cee_uchar*)"c");
    parser->prep_directive_current = cee_source_fregment_sub_attach(parser->prep_directive_current, 
                                                                    kCEESourceFregmentTypePrepDirective, 
                                                                    parser->filepath_ref,
                                                                    parser->subject_ref,
                                                                    (const cee_uchar*)"c");
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        (const cee_uchar*)"c");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"c");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               (const cee_uchar*)"c");
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      (const cee_uchar*)"c");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"c");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             (const cee_uchar*)"c");
    parser->state = kCParserStateStatementParsing;
    parser->prep_if_directive = 0;
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
    parser->prep_if_directive = 0;
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

static cee_boolean token_is_comment(CEEToken* token)
{
    return token->identifier == kCEETokenID_LINE_COMMENT ||
            token->identifier == kCEETokenID_LINES_COMMENT;
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
                prep_directive_reduce(parser);
                parser_state_clear(parser, kCParserStatePrepDirectiveParsing);
            }
        }
        else if (token_id_is_prep_directive(token->identifier)) {
            parser_state_set(parser, kCParserStatePrepDirectiveParsing);
            prep_directive_token_push(parser, token);
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
        
    reference_fregment = c_referernce_fregment_convert(fregment, subject);
    c_reference_fregment_parse(filepath,
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

static CEESourceFregment* c_referernce_fregment_convert(CEESourceFregment* fregment,
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
                                                        (const cee_uchar*)"c");
        reference_fregment = cee_source_fregment_sub_attach(reference_fregment, 
                                                            kCEESourceFregmentTypeStatement,
                                                            fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            (const cee_uchar*)"c");
    }
    else {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeStatement,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        (const cee_uchar*)"c");
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
                                                   (const cee_uchar*)"c");
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
                                                   (const cee_uchar*)"c");
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
                                                   (const cee_uchar*)"c");
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

static void c_reference_fregment_parse(const cee_uchar* filepath,
                                       CEESourceFregment* fregment,
                                       const cee_uchar* subject,
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
        if (cee_source_fregment_tokens_pattern_match(fregment, p, '.', kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeUnknow;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_MEMBER_POINTER, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeUnknow;
        }
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_SCOPE, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch object member */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(1, 1), &sub);
            type = kCEESourceReferenceTypeUnknow;
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
    
    if (objective_c_message) {
        type = kCEESourceReferenceTypeUnknow;
        reference = cee_source_symbol_reference_create((const cee_char*)filepath,
                                                       subject, 
                                                       objective_c_message, 
                                                       type);
        *references = cee_list_prepend(*references, reference);
        objective_c_message = NULL;
    }
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        c_reference_fregment_parse(filepath,
                                   p->data, 
                                   subject, 
                                   prep_directive, 
                                   statement, 
                                   references);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}
