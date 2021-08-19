#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_java.h"
#include "cee_symbol.h"
#include "cee_reference.h"

//#define DEBUG_CLASS
//#define DEBUG_ENUM
//#define DEBUG_INTERFACE
//#define DEBUG_METHOD_DEFINITION
//#define DEBUG_DECLARATION

typedef enum _JavaClassDefinitionTranslateState {
    kJavaClassDefinitionTranslateStateInit = 0,
    kJavaClassDefinitionTranslateStateClassSpecifier,
    kJavaClassDefinitionTranslateStateIdentifier,
    kJavaClassDefinitionTranslateStateIdentifierSeparator,
    kJavaClassDefinitionTranslateStateComma,
    kJavaClassDefinitionTranslateStateSuperclass,
    kJavaClassDefinitionTranslateStateAnnotation,
    kJavaClassDefinitionTranslateStateAnnotationList,
    kJavaClassDefinitionTranslateStateAnnotationListEnd,
    kJavaClassDefinitionTranslateStateSuperinterfaces,
    kJavaClassDefinitionTranslateStateTypeParameters,
    kJavaClassDefinitionTranslateStateConfirm,
    kJavaClassDefinitionTranslateStateError,
    kJavaClassDefinitionTranslateStateMax,
} JavaClassDefinitionTranslateState;

typedef enum _JavaInterfaceDefinitionTranslateState {
    kJavaInterfaceDefinitionTranslateStateInit = 0,
    kJavaInterfaceDefinitionTranslateStateInterfaceSpecifier,
    kJavaInterfaceDefinitionTranslateStateIdentifier,
    kJavaInterfaceDefinitionTranslateStateIdentifierSeparator,
    kJavaInterfaceDefinitionTranslateStateComma,
    kJavaInterfaceDefinitionTranslateStateTypeParameters,
    kJavaInterfaceDefinitionTranslateStateExtendInterface,
    kJavaInterfaceDefinitionTranslateStateAnnotation,
    kJavaInterfaceDefinitionTranslateStateAnnotationList,
    kJavaInterfaceDefinitionTranslateStateAnnotationListEnd,
    kJavaInterfaceDefinitionTranslateStateConfirm,
    kJavaInterfaceDefinitionTranslateStateError,
    kJavaInterfaceDefinitionTranslateStateMax,
} JavaInterfaceDefinitionTranslateState;

typedef enum _JavaEnumDefinitionTranslateState {
    kJavaEnumDefinitionTranslateStateInit = 0, 
    kJavaEnumDefinitionTranslateStateEnumSpecifier, 
    kJavaEnumDefinitionTranslateStateIdentifier, 
    kJavaEnumDefinitionTranslateStateIdentifierSeparator, 
    kJavaEnumDefinitionTranslateStateComma, 
    kJavaEnumDefinitionTranslateStateAnnotation, 
    kJavaEnumDefinitionTranslateStateAnnotationList, 
    kJavaEnumDefinitionTranslateStateAnnotationListEnd, 
    kJavaEnumDefinitionTranslateStateSuperinterfaces, 
    kJavaEnumDefinitionTranslateStateTypeParameters,
    kJavaEnumDefinitionTranslateStateConfirm,
    kJavaEnumDefinitionTranslateStateError, 
    kJavaEnumDefinitionTranslateStateMax, 
} JavaEnumDefinitionTranslateState;

typedef enum _JavaEnumeratorBlockTranslateState {
    kJavaEnumeratorBlockTranslateStateInit = 0,
    kJavaEnumeratorBlockTranslateStateIdentifier,
    kJavaEnumeratorBlockTranslateStateConfirm,
    kJavaEnumeratorBlockTranslateStateError,
    kJavaEnumeratorBlockTranslateStateMax
} JavaEnumeratorBlockTranslateState;

typedef enum _JavaMethodTranslateState {
    kJavaMethodTranslateStateInit = 0,
    kJavaMethodTranslateStateDeclarationSpecifier,
    kJavaMethodTranslateStateBuiltinType,
    kJavaMethodTranslateStateIdentifier,
    kJavaMethodTranslateStateParameterList,
    kJavaMethodTranslateStateParameterListEnd,
    kJavaMethodTranslateStateAnnotation,
    kJavaMethodTranslateStateAnnotationList,
    kJavaMethodTranslateStateAnnotationListEnd,
    kJavaMethodTranslateStateThrows,
    kJavaMethodTranslateStateException,
    kJavaMethodTranslateStateExceptionSeparator,
    kJavaMethodTranslateStateTypeParameters,
    kJavaMethodTranslateStateDefinition,
    kJavaMethodTranslateStateDeclaration,
    kJavaMethodTranslateStateError,
    kJavaMethodTranslateStateMax,
} JavaMethodTranslateState;

typedef enum _JavaMethodParametersDeclarationTranslateState {
    kJavaMethodParametersDeclarationTranslateStateInit = 0, 
    kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier, 
    kJavaMethodParametersDeclarationTranslateStateBuiltinType, 
    kJavaMethodParametersDeclarationTranslateStateIdentifier, 
    kJavaMethodParametersDeclarationTranslateStateAnnotation, 
    kJavaMethodParametersDeclarationTranslateStateAnnotationList, 
    kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd, 
    kJavaMethodParametersDeclarationTranslateStateDot, 
    kJavaMethodParametersDeclarationTranslateStateThis, 
    kJavaMethodParametersDeclarationTranslateStateConfirm,
    kJavaMethodParametersDeclarationTranslateStateTypeParameters, 
    kJavaMethodParametersDeclarationTranslateStateError, 
    kJavaMethodParametersDeclarationTranslateStateMax, 
} JavaMethodParametersDeclarationTranslateState;

typedef enum _JavaDeclarationTranslateState {
    kJavaDeclarationTranslateStateInit = 0, 
    kJavaDeclarationTranslateStateDeclarationSpecifier, 
    kJavaDeclarationTranslateStateBuiltinType, 
    kJavaDeclarationTranslateStateCustomType, 
    kJavaDeclarationTranslateStateStructor, 
    kJavaDeclarationTranslateStateStructorEnd, 
    kJavaDeclarationTranslateStateIdentifier, 
    kJavaDeclarationTranslateStateCustomTypeSeparator, 
    kJavaDeclarationTranslateStateParameterList, 
    kJavaDeclarationTranslateStateParameterListEnd, 
    kJavaDeclarationTranslateStateAnnotation, 
    kJavaDeclarationTranslateStateAnnotationList, 
    kJavaDeclarationTranslateStateAnnotationListEnd, 
    kJavaDeclarationTranslateStateDefault, 
    kJavaDeclarationTranslateStateConfirm,
    kJavaDeclarationTranslateStateTypeInit, 
    kJavaDeclarationTranslateStateTypeParameters, 
    kJavaDeclarationTranslateStateThrows, 
    kJavaDeclarationTranslateStateException, 
    kJavaDeclarationTranslateStateExceptionSeparator, 
    kJavaDeclarationTranslateStateError, 
    kJavaDeclarationTranslateStateMax, 
} JavaDeclarationTranslateState;

typedef enum _JavaImportStatementTranslateState {
    kJavaImportStatementTranslateStateInit = 0, 
    kJavaImportStatementTranslateStateImport, 
    kJavaImportStatementTranslateStateIdentifier, 
    kJavaImportStatementTranslateStateIdentifierSeparator, 
    kJavaImportStatementTranslateStateDemand, 
    kJavaImportStatementTranslateStateConfirm,
    kJavaImportStatementTranslateStateError, 
    kJavaImportStatementTranslateStateMax, 
} JavaImportStatementTranslateState;

typedef enum _JavaPackageStatementTranslateState {
    kJavaPackageStatementTranslateStateInit = 0,
    kJavaPackageStatementTranslateStatePackage,
    kJavaPackageStatementTranslateStateAnnotation,
    kJavaPackageStatementTranslateStateAnnotationList,
    kJavaPackageStatementTranslateStateAnnotationListEnd,
    kJavaPackageStatementTranslateStateIdentifier,
    kJavaPackageStatementTranslateStateIdentifierSeparator,
    kJavaPackageStatementTranslateStateConfirm,
    kJavaPackageStatementTranslateStateError,
    kJavaPackageStatementTranslateStateMax,
} JavaPackageStatementTranslateState;

typedef cee_boolean (*ParseTrap)(CEESourceFregment*, 
                                 CEEList**);

typedef struct _JavaParser {
    CEESourceParserRef super;
    const cee_char* filepath_ref;
    const cee_char* subject_ref;
    CEESourceFregment* statement_root;
    CEESourceFregment* statement_current;
    CEESourceFregment* comment_root;
    CEESourceFregment* comment_current;
    ParseTrap block_header_traps[CEETokenID_MAX];
} JavaParser;

static CEETokenType java_token_type_map[CEETokenID_MAX];
static cee_int java_class_definition_translate_table[kJavaClassDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int java_interface_definition_translate_table[kJavaInterfaceDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int java_enum_definition_translate_table[kJavaEnumDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int java_method_translate_table[kJavaMethodTranslateStateMax][CEETokenID_MAX];
static cee_int java_method_parameters_declaration_translate_table[kJavaMethodParametersDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int java_declaration_translate_table[kJavaDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int java_import_statement_translate_table[kJavaImportStatementTranslateStateMax][CEETokenID_MAX];
static cee_int java_package_statement_translate_table[kJavaPackageStatementTranslateStateMax][CEETokenID_MAX];
static cee_int java_enumerator_block_translate_table[kJavaEnumeratorBlockTranslateStateMax][CEETokenID_MAX];

static JavaParser* parser_create(void);
static void parser_free(cee_pointer data);
static void java_block_header_trap_init(JavaParser* parser);
static void java_token_type_map_init(void);
static CEEList* skip_type_parameter_list(CEEList* p,
                                         cee_boolean reverse);
static cee_boolean token_type_matcher(CEEToken* token,
                                      CEETokenType type);
static cee_boolean token_id_is_assignment(CEETokenID identifier);
static cee_boolean token_id_is_builtin_type(CEETokenID identifier);
static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier);
static cee_boolean token_id_is_punctuation(CEETokenID identifier);
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
static CEESourceFregment* java_referernce_fregment_convert(CEESourceFregment* fregment,
                                                           const cee_char* subject);
static void java_reference_fregment_parse(CEESourceParserRef parser_ref,
                                          const cee_char* filepath,
                                          CEESourceFregment* fregment,
                                          const cee_char* subject,
                                          CEESourceFregment* prep_directive,
                                          CEESourceFregment* statement,
                                          CEEList** references);
static void symbol_parse_init(JavaParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject);
static void symbol_parse_clear(JavaParser* parser);
static cee_boolean token_is_comment(CEEToken* token);
static cee_boolean comment_token_push(JavaParser* parser,
                                      CEEToken* push);
static cee_boolean comment_fregment_reduce(JavaParser* parser);
static cee_boolean comment_attach(JavaParser* parser);
static void block_header_parse(JavaParser* parser);
static cee_boolean statement_block_parse(CEESourceFregment* fregment);
static void block_push(JavaParser* parser);
static cee_boolean block_pop(JavaParser* parser);
static void block_parse(JavaParser* parser);
static cee_boolean block_reduce(JavaParser* parser);
static void label_parse(JavaParser* parser);
static cee_boolean label_reduce(JavaParser* parser);

/** statement */
static void statement_parse(JavaParser* parser);
static cee_boolean statement_reduce(JavaParser* parser);
static cee_boolean statement_attach(JavaParser* parser,
                                    CEESourceFregmentType type);
static cee_boolean statement_sub_attach(JavaParser* parser,
                                        CEESourceFregmentType type);
static cee_boolean statement_pop(JavaParser* parser);
static cee_boolean statement_token_push(JavaParser* parser,
                                        CEEToken* push);
static void parameter_list_push(JavaParser* parser);
static cee_boolean parameter_list_pop(JavaParser* parser);
static void subscript_push(JavaParser* parser);
static cee_boolean subscript_pop(JavaParser* parser);

static void java_declaration_translate_table_init(void);
static cee_boolean java_declaration_parse(CEESourceFregment* fregment);
static CEEList* skip_java_declaration_interval(CEEList* p);
static void java_import_statement_translate_table_init(void);
static cee_boolean java_import_statement_parse(CEESourceFregment* fregment);
static void java_package_statement_translate_table_init(void);
static cee_boolean java_package_statement_parse(CEESourceFregment* fregment);
static void java_class_definition_translate_table_init(void);
static cee_boolean java_class_definition_trap(CEESourceFregment* fregment,
                                              CEEList** pp);
static void java_interface_definition_translate_table_init(void);
static cee_boolean java_interface_definition_trap(CEESourceFregment* fregment,
                                                  CEEList** pp);
static void java_enum_definition_translate_table_init(void);
static cee_boolean java_enum_definition_trap(CEESourceFregment* fregment,
                                             CEEList** pp);
static cee_char* java_enumerator_proto_descriptor_create(CEESourceFregment* fregment,
                                                         CEESourceSymbol* enumerator,
                                                         const cee_char* proto);
static cee_char* java_declaration_proto_descriptor_create(CEESourceFregment* fregment,
                                                          CEEList* prefix,
                                                          CEEList* prefix_tail,
                                                          CEEList* identifier);
static cee_char* java_object_type_proto_descriptor_create(CEESourceFregment* fregment,
                                                          CEESourceSymbol* definition,
                                                          const cee_char* derives_str);
static cee_char* java_name_scope_list_string_create(CEEList* scopes,
                                                    const cee_char* subject);
static cee_char* java_name_scope_create(CEEList* tokens,
                                        const cee_char* subject);
static void superclass_free(cee_pointer data);
static void superinterface_free(cee_pointer data);
static void extendinterface_free(cee_pointer data);
static cee_boolean java_enumerators_parse(CEESourceFregment* fregment);
static void java_enumerator_block_translate_table_init(void);
static cee_boolean java_enumerator_block_parse(CEESourceFregment* fregment);
static void java_method_translate_table_init(void);
static cee_boolean java_method_parse(CEESourceFregment* fregment);
static void java_method_parameters_declaration_translate_table_init(void);
static cee_boolean java_method_parameters_parse(CEESourceFregment* fregment);
static CEESourceSymbol* java_method_parameter_create(CEESourceFregment* fregment,
                                                     CEEList* prefix,
                                                     CEEList* identifier);
static cee_char* java_method_parameter_proto_descriptor_create(CEESourceFregment* fregment,
                                                               CEEList* prefix,
                                                               CEEList* prefix_tail,
                                                               CEESourceSymbol* parameter);
static cee_char* java_type_descriptor_from_token_slice(CEESourceFregment* fregment,
                                                       CEEList* p,
                                                       CEEList* q);
static cee_char* java_method_proto_descriptor_create(CEESourceFregment* fregment,
                                                     CEEList* prefix,
                                                     CEEList* prefix_tail,
                                                     CEEList* identifier,
                                                     CEEList* parameter_list,
                                                     CEEList* parameter_list_end);
static cee_boolean should_proto_descriptor_append_whitespace(CEEToken* token,
                                                             CEEToken* token_prev);
static const cee_char* java_access_level_search(CEESourceFregment* fregment,
                                                CEEList* begin,
                                                CEEList* end);
static cee_boolean symbol_search_in_scope(CEESourceParserRef parser_ref,
                                          CEESourceSymbolReference* reference,
                                          CEESourceFregment* prep_directive,
                                          CEESourceFregment* statement,
                                          CEESourceSymbol** symbol);
static CEESourceSymbol* symbol_search_in_scope_recursive(CEESourceFregment* searching,
                                                         const cee_char* name);
/**
 * parser
 */
CEESourceParserRef cee_java_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    parser->symbol_search_in_scope = symbol_search_in_scope;
    
    JavaParser* java = parser_create();
    java->super = parser;
    
    java_block_header_trap_init(java);
    java_token_type_map_init();
    
    java_class_definition_translate_table_init();
    java_interface_definition_translate_table_init();
    java_enum_definition_translate_table_init();
    java_enumerator_block_translate_table_init();
    java_method_translate_table_init();
    java_method_parameters_declaration_translate_table_init();
    java_declaration_translate_table_init();
    java_import_statement_translate_table_init();
    java_package_statement_translate_table_init();
    
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

static void java_block_header_trap_init(JavaParser* parser)
{
    
    for (int i = 0; i < kCEETokenID_END; i ++)
        parser->block_header_traps[i] = NULL;
    
    parser->block_header_traps[kCEETokenID_CLASS] = java_class_definition_trap;
    parser->block_header_traps[kCEETokenID_INTERFACE] = java_interface_definition_trap;
    parser->block_header_traps[kCEETokenID_ENUM] = java_enum_definition_trap;
}

static void java_token_type_map_init(void)
{
    for (cee_int i = 0; i < CEETokenID_MAX; i ++)
        java_token_type_map[i] = 0;
    
    java_token_type_map[kCEETokenID_ABSTRACT]                    = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_ASSERT]                      = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_BOOLEAN]                     = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_BREAK]                       = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_BYTE]                        = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_CASE]                        = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_CATCH]                       = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_CHAR]                        = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_CLASS]                       = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_CONTINUE]                    = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_CONST]                       = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_DEFAULT]                     = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_DO]                          = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_DOUBLE]                      = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_ELSE]                        = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_ENUM]                        = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_EXPORTS]                     = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_EXTENDS]                     = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_FINAL]                       = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_FINALLY]                     = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_FLOAT]                       = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_FOR]                         = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_GOTO]                        = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_IF]                          = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_IMPLEMENTS]                  = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_IMPORT]                      = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_INSTANCEOF]                  = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_INT]                         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_INTERFACE]                   = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_LONG]                        = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_MODULE]                      = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_NATIVE]                      = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_NEW]                         = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_PACKAGE]                     = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_PRIVATE]                     = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_PROTECTED]                   = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_PUBLIC]                      = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_REQUIRES]                    = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_RETURN]                      = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_SHORT]                       = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_STATIC]                      = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_STRICTFP]                    = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_SUPER]                       = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_SWITCH]                      = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_SYNCHRONIZED]                = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_THIS]                        = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_THROW]                       = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_THROWS]                      = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_TRANSIENT]                   = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_TRY]                         = kCEETokenTypeKeyword;
    java_token_type_map[kCEETokenID_VAR]                         = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_VOID]                        = kCEETokenTypeKeyword | kCEETokenTypeBuildinType;
    java_token_type_map[kCEETokenID_VOLATILE]                    = kCEETokenTypeKeyword | kCEETokenTypeDeclarationSpecifier;
    java_token_type_map[kCEETokenID_WHILE]                       = kCEETokenTypeKeyword;
    java_token_type_map['=']                                     = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map['+']                                     = kCEETokenTypePunctuation;
    java_token_type_map['-']                                     = kCEETokenTypePunctuation;
    java_token_type_map['*']                                     = kCEETokenTypePunctuation;
    java_token_type_map['/']                                     = kCEETokenTypePunctuation;
    java_token_type_map['\\']                                    = kCEETokenTypePunctuation;
    java_token_type_map['%']                                     = kCEETokenTypePunctuation;
    java_token_type_map['~']                                     = kCEETokenTypePunctuation;
    java_token_type_map['&']                                     = kCEETokenTypePunctuation;
    java_token_type_map['|']                                     = kCEETokenTypePunctuation;
    java_token_type_map['^']                                     = kCEETokenTypePunctuation;
    java_token_type_map['!']                                     = kCEETokenTypePunctuation;
    java_token_type_map['<']                                     = kCEETokenTypePunctuation;
    java_token_type_map['>']                                     = kCEETokenTypePunctuation;
    java_token_type_map['.']                                     = kCEETokenTypePunctuation;
    java_token_type_map[',']                                     = kCEETokenTypePunctuation;
    java_token_type_map['?']                                     = kCEETokenTypePunctuation;
    java_token_type_map[':']                                     = kCEETokenTypePunctuation;
    java_token_type_map['(']                                     = kCEETokenTypePunctuation;
    java_token_type_map[')']                                     = kCEETokenTypePunctuation;
    java_token_type_map['{']                                     = kCEETokenTypePunctuation;
    java_token_type_map['}']                                     = kCEETokenTypePunctuation;
    java_token_type_map['[']                                     = kCEETokenTypePunctuation;
    java_token_type_map[']']                                     = kCEETokenTypePunctuation;
    java_token_type_map[';']                                     = kCEETokenTypePunctuation;
    java_token_type_map['@']                                     = kCEETokenTypePunctuation;
    java_token_type_map['.']                                     = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_ADD_ASSIGNMENT]              = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_MINUS_ASSIGNMENT]            = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_MULTI_ASSIGNMENT]            = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_DIV_ASSIGNMENT]              = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_MOD_ASSIGNMENT]              = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_AND_ASSIGNMENT]              = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_OR_ASSIGNMENT]               = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_XOR_ASSIGNMENT]              = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_LEFT_OFFSET_ASSIGNMENT]      = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_RIGHT_OFFSET_ASSIGNMENT]     = kCEETokenTypePunctuation | kCEETokenTypeAssignment;
    java_token_type_map[kCEETokenID_INCREMENT]                   = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_DECREMENT]                   = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_LOGIC_AND]                   = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_LOGIC_OR]                    = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_LOGIC_EQUAL]                 = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_LOGIC_UNEQUAL]               = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_LOGIC_LESS_EQUAL]            = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_LOGIC_LARGE_EQUAL]           = kCEETokenTypePunctuation;
    java_token_type_map[kCEETokenID_LINES_COMMENT]               = kCEETokenTypeComment;
    java_token_type_map[kCEETokenID_LINE_COMMENT]                = kCEETokenTypeComment;
    java_token_type_map[kCEETokenID_LITERAL]                     = kCEETokenTypeLiteral;
    java_token_type_map[kCEETokenID_CHARACTER]                   = kCEETokenTypeCharacter;
    java_token_type_map[kCEETokenID_CONSTANT]                    = kCEETokenTypeConstant;
    java_token_type_map[kCEETokenID_IDENTIFIER]                  = kCEETokenTypeIdentifier;
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
    return (java_token_type_map[token->identifier] & type) != 0;
}

static cee_boolean token_id_is_assignment(CEETokenID identifier)
{
    return (java_token_type_map[identifier] & kCEETokenTypeAssignment) != 0;
}

static cee_boolean token_id_is_builtin_type(CEETokenID identifier)
{
    return (java_token_type_map[identifier] & kCEETokenTypeBuildinType) != 0;
}

static cee_boolean token_id_is_declaration_specifier(CEETokenID identifier)
{
    return (java_token_type_map[identifier] & kCEETokenTypeDeclarationSpecifier) != 0;
}

static cee_boolean token_id_is_punctuation(CEETokenID identifier)
{
    return (java_token_type_map[identifier] & kCEETokenTypePunctuation) != 0;
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
        
        if (!ret)
            break;
        
    } while(1);
    
    cee_source_fregment_tree_symbols_parent_parse(parser->statement_root);
    
    *statement = parser->statement_root;
    *prep_directive = NULL;
    *comment = parser->comment_root;
    *tokens_ref = tokens;
    *source_token_map = map;
    
    symbol_parse_clear(parser);
    cee_lexer_java_buffer_free();
    
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
    
    reference_fregment = java_referernce_fregment_convert(fregment, subject);
    java_reference_fregment_parse(parser_ref,
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

static CEESourceFregment* java_referernce_fregment_convert(CEESourceFregment* fregment,
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
                                                        "java");
        reference_fregment = cee_source_fregment_sub_attach(reference_fregment,
                                                            kCEESourceFregmentTypeStatement,
                                                            fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            "java");
    }
    else {
        reference_fregment = cee_source_fregment_create(kCEESourceFregmentTypeStatement,
                                                        fregment->filepath_ref,
                                                        fregment->subject_ref,
                                                        "java");
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
                                                   "java");
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
                                                   "java");
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
                                                   "java");
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

static void java_reference_fregment_parse(CEESourceParserRef parser_ref,
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
        else if (cee_source_fregment_tokens_pattern_match(fregment, p, kCEETokenID_IDENTIFIER, NULL)) {
            /** catch any other identifier */
            p = cee_source_fregment_tokens_break(fregment, p, cee_range_make(0, 1), &sub);
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
    
    
    p = SOURCE_FREGMENT_CHILDREN_FIRST(fregment);
    while (p) {
        java_reference_fregment_parse(parser_ref,
                                      filepath,
                                      p->data,
                                      subject,
                                      prep_directive,
                                      statement,
                                      references);
        p = SOURCE_FREGMENT_NEXT(p);
    }
}

static void symbol_parse_init(JavaParser* parser,
                              const cee_char* filepath,
                              const cee_char* subject)
{
    parser->filepath_ref = filepath;
    parser->subject_ref = subject;
    
    parser->statement_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                        parser->filepath_ref,
                                                        parser->subject_ref,
                                                        "java");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_root, 
                                                               kCEESourceFregmentTypeSourceList, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "java");
    parser->statement_current = cee_source_fregment_sub_attach(parser->statement_current, 
                                                               kCEESourceFregmentTypeStatement, 
                                                               parser->filepath_ref,
                                                               parser->subject_ref,
                                                               "java");
    
    parser->comment_root = cee_source_fregment_create(kCEESourceFregmentTypeRoot, 
                                                      parser->filepath_ref,
                                                      parser->subject_ref,
                                                      "java");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_root, 
                                                             kCEESourceFregmentTypeSourceList, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "java");
    parser->comment_current = cee_source_fregment_sub_attach(parser->comment_current, 
                                                             kCEESourceFregmentTypeComment, 
                                                             parser->filepath_ref,
                                                             parser->subject_ref,
                                                             "java");
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

/**
 *  comment
 */
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
    
    attached = cee_source_fregment_attach(parser->comment_current,
                                          kCEESourceFregmentTypeComment, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "java");
    if (!attached)
        return FALSE;
    
    parser->comment_current = attached;
    return TRUE;
}

/**
 * block
 */
static void block_header_parse(JavaParser* parser)
{
    CEESourceFregment* current = parser->statement_current;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    ParseTrap trap = NULL;
    
    if (!current || !current->tokens_ref)
        return;
    
    if (java_method_parse(current))
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
    
    if (java_enumerator_block_parse(current) ||
        statement_block_parse(current))
        return;
    
    return;
}

static cee_boolean statement_block_parse(CEESourceFregment* fregment)
{
    CEEList* p = NULL;
    CEEToken* token = NULL;
    
    if (cee_source_fregment_type_is(fregment, kCEESourceFregmentTypeAssignmentBlock) ||
        cee_source_fregment_type_is(fregment, kCEESourceFregmentTypeStatementBlock))
        return TRUE;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (token->identifier == '<') {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                return FALSE;
        }
        else {
            if (token_id_is_assignment(token->identifier)) {
                cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeAssignmentBlock);
                break;
            }
        }
        
        p = TOKEN_NEXT(p);
    }
    
    if (!cee_source_fregment_type_is(fregment, kCEESourceFregmentTypeAssignmentBlock))
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeStatementBlock);
    
    return TRUE;
}

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
    java_enumerators_parse(parser->statement_current);
}

static cee_boolean block_reduce(JavaParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    
    if (cee_source_fregment_type_is(parser->statement_current, kCEESourceFregmentTypeAssignmentBlock)) {
        /** expect statement terminate ';' */
    }
    else {
        statement_attach(parser, kCEESourceFregmentTypeStatement);
    }
    return TRUE;
}

static void label_parse(JavaParser* parser)
{
    CEESourceFregment* fregment = NULL;
    const cee_char* subject = NULL;
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
                                                                            "java",
                                                                            kCEETokenStringOptionCompact);
                    if (declaration) {
                        cee_token_slice_state_mark(p, q, kCEETokenStateSymbolOccupied);
                        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeLabelDeclaration);
                        fregment->symbols = cee_list_prepend(fregment->symbols, declaration);
                    }
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

static cee_boolean label_reduce(JavaParser* parser)
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
static void statement_parse(JavaParser* parser)
{
    CEESourceFregment* current = parser->statement_current;    
    if (!current || !current->tokens_ref)
        return;
    
    if (java_enumerators_parse(current) ||
        java_method_parse(current) ||
        java_declaration_parse(current) ||
        java_import_statement_parse(current) ||
        java_package_statement_parse(current))
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

static cee_boolean statement_pop(JavaParser* parser)
{
    if (!parser->statement_current || !parser->statement_current->parent)
        return FALSE;
    
    parser->statement_current = parser->statement_current->parent;
    return TRUE;
}

static cee_boolean statement_attach(JavaParser* parser,
                                    CEESourceFregmentType type)
{
    CEESourceFregment* attached = NULL;
    
    if (!parser->statement_current)
        return FALSE;
    
    attached = cee_source_fregment_attach(parser->statement_current, 
                                          type, 
                                          parser->filepath_ref,
                                          parser->subject_ref,
                                          "java");
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
                                              "java");
    if (!attached)
        return FALSE;
    
    parser->statement_current = attached;
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

/**
 * parameter list
 */
static void parameter_list_push(JavaParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeRoundBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean parameter_list_pop(JavaParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

/**
 * subscript
 */
static void subscript_push(JavaParser* parser)
{
    statement_sub_attach(parser, kCEESourceFregmentTypeSquareBracketList);
    statement_sub_attach(parser, kCEESourceFregmentTypeStatement);
}

static cee_boolean subscript_pop(JavaParser* parser)
{
    if (statement_pop(parser))
        return statement_pop(parser);
    
    return FALSE;
}

static void java_declaration_translate_table_init(void)
{
    /**
     *                          declaration_specifier       builtin_type        identifier          assign_operator     .                       <                   annotation      ,           (               )                   [                   ]                   default     ;
     *  Init                    DeclarationSpecifier        BuiltinType         CustomType          Error               Error                   *TypeParameters     Annotation      Error       Error           Error               Error               Error               Error       Error
     *  DeclarationSpecifier    DeclarationSpecifier        BuiltinType         CustomType          Error               Error                   *TypeParameters     Annotation      Error       Error           Error               Error               Error               Error       Error
     *  BuiltinType             DeclarationSpecifier        BuiltinType         Identifier          Error               Error                   *TypeParameters     Annotation      Error       Error           Error               BuiltinType         BuiltinType         Error       Error
     *  CustomType              Error                       Error               Identifier          Error               CustomTypeSeparator     *TypeParameters     Annotation      Error       Error           Error               CustomType          CustomType          Error       Error
     *  CustomTypeSeparator     Error                       Error               CustomType          Error               Error                   Error               Error           Error       Error           Error               Error               Error               Error       Error
     *  Identifier              Error                       Error               Error               Confirm             Error                   Error               Error           Confirm     Error           Error               Identifier          Identifier          Error       Confirm
     *  Annotation              DeclarationSpecifier        BuiltinType         CustomType          Error               Error                   *TypeParameters     Annotation      Confirm     AnnotationList  Error               Annotation          Annotation          *Default    Confirm
     *  AnnotationList          Error                       Error               Error               Error               Error                   Error               Error           Error       Error           AnnotationListEnd   Error               Error               Error       Error
     *  AnnotationListEnd       DeclarationSpecifier        BuiltinType         CustomType          Error               Error                   *TypeParameters     Annotation      Confirm     Error           Error               AnnotationListEnd   AnnotationListEnd   *Default    Confirm
     *  Confirm                 Error                       Error               Identifier          Error               Error                   Error               Error           Confirm     Error           Error               Error               Error               Error       Confirm
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'  
     *  Default: skip to Terminate
     *  Throws: skip to Terminate
     *  
     */
    TRANSLATE_STATE_INI(java_declaration_translate_table,   kJavaDeclarationTranslateStateMax                   , kJavaDeclarationTranslateStateError                                                           );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateInit                  , token_id_is_declaration_specifier     , kJavaDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateInit                  , token_id_is_builtin_type              , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateInit                  , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateInit                  , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateInit                  , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateDeclarationSpecifier  , token_id_is_declaration_specifier     , kJavaDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateDeclarationSpecifier  , token_id_is_builtin_type              , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateDeclarationSpecifier  , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateBuiltinType           , token_id_is_declaration_specifier     , kJavaDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateBuiltinType           , token_id_is_builtin_type              , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateBuiltinType           , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateBuiltinType           , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateBuiltinType           , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateBuiltinType           , '['                                   , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateBuiltinType           , ']'                                   , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , '.'                                   , kJavaDeclarationTranslateStateCustomTypeSeparator     );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , '['                                   , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , ']'                                   , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomTypeSeparator   , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , token_id_is_assignment                , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , ','                                   , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , '['                                   , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , ']'                                   , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , ';'                                   , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , token_id_is_declaration_specifier     , kJavaDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , token_id_is_builtin_type              , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , ','                                   , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , '('                                   , kJavaDeclarationTranslateStateAnnotationList          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , '['                                   , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , ']'                                   , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , kCEETokenID_DEFAULT                   , kJavaDeclarationTranslateStateDefault                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , ';'                                   , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationList        , ')'                                   , kJavaDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , token_id_is_declaration_specifier     , kJavaDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , token_id_is_builtin_type              , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , ','                                   , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , '['                                   , kJavaDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , ']'                                   , kJavaDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_DEFAULT                   , kJavaDeclarationTranslateStateDefault                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , ';'                                   , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateConfirm               , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateConfirm               , ','                                   , kJavaDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateConfirm               , ';'                                   , kJavaDeclarationTranslateStateConfirm                 );
}

static cee_boolean java_declaration_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    CEEList* identifier = NULL;
    CEEList* prefix = NULL;
    CEEList* prefix_tail = NULL;
    cee_boolean prefix_parse = FALSE;
    JavaDeclarationTranslateState current = kJavaDeclarationTranslateStateInit;
    JavaDeclarationTranslateState prev = kJavaDeclarationTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_declaration_translate_table[current][token->identifier];
        
        if (current == kJavaDeclarationTranslateStateError) {
            break;
        }
        else if (current == kJavaDeclarationTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kJavaDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kJavaDeclarationTranslateStateDefault || 
                 current == kJavaDeclarationTranslateStateConfirm) {
            if (!prefix_parse) {
                prefix = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
                if (identifier)
                    prefix_tail = TOKEN_PREV(identifier);
                prefix_parse = TRUE;
            }
            
            if (identifier)
                declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                        fregment->subject_ref,
                                                                        identifier,
                                                                        identifier,
                                                                        kCEESourceSymbolTypeVariableDeclaration,
                                                                        "java",
                                                                        kCEETokenStringOptionCompact);
            
            if (declaration) {
                cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
                if (prefix && prefix_tail)
                    declaration->proto_descriptor = java_declaration_proto_descriptor_create(fregment,
                                                                                             prefix,
                                                                                             prefix_tail,
                                                                                             identifier);
                declarations = cee_list_prepend(declarations, declaration);
            }
            
            declaration = NULL;
            identifier = NULL;
            
            if (current == kJavaDeclarationTranslateStateDefault)
                break;
            
            if (!cee_token_is_identifier(p, ',') &&
                !cee_token_is_identifier(p, ';')) {
                p = skip_java_declaration_interval(p);
                if (!p)
                    break;
            }
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
    if (declarations)
        cee_source_symbols_print(declarations);
#endif
    
exit:
    
    return ret;
}

static CEEList* skip_java_declaration_interval(CEEList* p)
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

static void java_import_statement_translate_table_init(void)
{
    /**
     *                              import      static      Identifier      .                       *       ;
     *  Init                        Import      Error       Error           Error                   Error   Error
     *  Import                      Error       Import      Identifier      Error                   Error   Confirm
     *  Identifier                  Error       Error       Error           IdentifierSeparator     Error   Confirm
     *  IdentifierSeparator         Error       Error       Identifier      Error                   Demand  Error
     *  Demand                      Error       Error       Error           Error                   Error   Confirm
     */
    TRANSLATE_STATE_INI(java_import_statement_translate_table, kJavaImportStatementTranslateStateMax                    , kJavaImportStatementTranslateStateError                                               );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateInit                   , kCEETokenID_IMPORT            , kJavaImportStatementTranslateStateImport              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateImport                 , kCEETokenID_STATIC            , kJavaImportStatementTranslateStateImport              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateImport                 , kCEETokenID_IDENTIFIER        , kJavaImportStatementTranslateStateIdentifier          );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateImport                 , ';'                           , kJavaImportStatementTranslateStateConfirm             );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifier             , '.'                           , kJavaImportStatementTranslateStateIdentifierSeparator );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifier             , ';'                           , kJavaImportStatementTranslateStateConfirm             );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifierSeparator    , kCEETokenID_IDENTIFIER        , kJavaImportStatementTranslateStateIdentifier          );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifierSeparator    , '*'                           , kJavaImportStatementTranslateStateDemand              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateDemand                 , ';'                           , kJavaImportStatementTranslateStateConfirm             );
}

static cee_boolean java_import_statement_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    JavaImportStatementTranslateState current = kJavaImportStatementTranslateStateInit;
    JavaImportStatementTranslateState prev = kJavaImportStatementTranslateStateInit;
    CEEToken* token = NULL;
    CEESourceSymbol* import = NULL;
    CEEList* r = NULL;
    CEEList* s = NULL;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_import_statement_translate_table[current][token->identifier];
        
        if (current == kJavaImportStatementTranslateStateError) {
            break;
        }
        else if (current == kJavaImportStatementTranslateStateConfirm) {
            if (cee_token_is_identifier(p, ';'))
                s = TOKEN_PREV(p);
            break;
        }
        else if (current == kJavaImportStatementTranslateStateImport) {
            if (cee_token_is_identifier(p, kCEETokenID_IMPORT))
                r = p;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kJavaImportStatementTranslateStateConfirm)
        goto exit;
    
    import = cee_source_symbol_create_from_token_slice(fregment->filepath_ref, 
                                                       fregment->subject_ref, 
                                                       r, 
                                                       s, 
                                                       kCEESourceSymbolTypeImport, 
                                                       "java",
                                                       kCEETokenStringOptionIncompact);
    if (import) {
        cee_source_symbol_name_format(import->name);
        fregment->symbols = cee_list_prepend(fregment->symbols, import);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_IMPORT
    if (import)
        cee_source_symbol_print(import);
#endif
    
exit:
    return ret;
}

static void java_package_statement_translate_table_init(void)
{
    /**
     *                          package         annotation      identifier      .                       (               )                   ;
     *  Init                    Package         Annotation      Error           Error                   Error           Error               Error
     *  Package                 Error           Error           Identifier      Error                   Error           Error               Error
     *  Annotation              Package         Error           Error           Error                   AnnotationList  Error               Error
     *  AnnotationList          Error           Error           Error           Error                   Error           AnnotationListEnd   Error
     *  AnnotationListEnd       Package         Error           Error           Error                   Error           Error               Error  
     *  Identifier              Error           Error           Error           IdentifierSeparator     Error           Error               Confirm
     *  IdentifierSeparator     Error           Error           Identifier      Error                   Error           Error               Error
     */
    TRANSLATE_STATE_INI(java_package_statement_translate_table, kJavaPackageStatementTranslateStateMax                      , kJavaPackageStatementTranslateStateError                                                      );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateInit                     , kCEETokenID_PACKAGE               , kJavaPackageStatementTranslateStatePackage                );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateInit                     , kCEETokenID_ANNOTATION            , kJavaPackageStatementTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStatePackage                  , kCEETokenID_IDENTIFIER            , kJavaPackageStatementTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateAnnotation               , kCEETokenID_PACKAGE               , kJavaPackageStatementTranslateStatePackage                );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateAnnotation               , '('                               , kJavaPackageStatementTranslateStateAnnotationList         );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateAnnotationList           , ')'                               , kJavaPackageStatementTranslateStateAnnotationListEnd      );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateAnnotationListEnd        , kCEETokenID_PACKAGE               , kJavaPackageStatementTranslateStatePackage                );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateIdentifier               , '.'                               , kJavaPackageStatementTranslateStateIdentifierSeparator    );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateIdentifier               , ';'                               , kJavaPackageStatementTranslateStateConfirm                );
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateIdentifierSeparator      , kCEETokenID_IDENTIFIER            , kJavaPackageStatementTranslateStateIdentifier             );
}

static cee_boolean java_package_statement_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    CEEList* p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    JavaPackageStatementTranslateState current = kJavaPackageStatementTranslateStateInit;
    JavaPackageStatementTranslateState prev = kJavaPackageStatementTranslateStateInit;
    CEEToken* token = NULL;
    CEESourceSymbol* package = NULL;
    CEEList* r = NULL;
    CEEList* s = NULL;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_package_statement_translate_table[current][token->identifier];
        
        if (current == kJavaPackageStatementTranslateStateError) {
            break;
        }
        else if (current == kJavaPackageStatementTranslateStateConfirm) {
            if (cee_token_is_identifier(p, ';'))
                s = TOKEN_PREV(p);
            break;
        }
        else if (current == kJavaPackageStatementTranslateStatePackage) {
            if (cee_token_is_identifier(p, kCEETokenID_PACKAGE))
                r = p;
        }
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kJavaPackageStatementTranslateStateConfirm)
        goto exit;
    
    package = cee_source_symbol_create_from_token_slice(fregment->filepath_ref, 
                                                        fregment->subject_ref, 
                                                        r, 
                                                        s, 
                                                        kCEESourceSymbolTypePackage, 
                                                        "java",
                                                        kCEETokenStringOptionIncompact);
    if (package) {
        cee_source_symbol_name_format(package->name);
        fregment->symbols = cee_list_prepend(fregment->symbols, package);
        cee_source_fregment_type_set_exclusive(fregment, kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_IMPORT
    if (package)
        cee_source_symbol_print(package);
#endif
    
exit:
    return ret;
}

static void java_class_definition_translate_table_init(void)
{
    /**
     *                      class_specifier     identifier          <                   extends         annotation      .                       (                   )                   implements          ,           {
     *  Init                ClassSpecifier      Error               Error               Error           Error           Error                   Error               Error               Error               Error       Error
     *  ClassSpecifier      Error               Identifier          Error               Error           Error           Error                   Error               Error               Error               Error       Error
     *  Identifier          Error               Identifier          *TypeParameters     Superclass      Annotation      IdentifierSeparator     Error               Error               Superinterfaces     Comma       Confirm
     *  IdentifierSeparator Error               Identifier          Error               Error           Error           Error                   Error               Error               Error               Error       Error
     *  Comma               Error               Identifier          Error               Error           Annotation      Error                   Error               Error               Error               Comma       Error
     *  Superclass          Error               Identifier          Error               Ereror          Annotation      Error                   Error               Error               Error               Error       Error
     *  Annotation          Error               Identifier          Error               Error           Error           Error                   AnnotationList      Error               Error               Error       Error
     *  AnnotationList      Error               Error               Error               Error           Error           Error                   Error               AnnotationListEnd   Error               Error       Error
     *  AnnotationListEnd   Error               Identifier          Error               Error           Error           Error                   Error               Error               Error               Error       Error
     *  Superinterfaces     Error               Identifier          Error               Error           Annotation      Error                   Error               Error               Error               Error       Error
     *
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateMax                   , kJavaClassDefinitionTranslateStateError                                               );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateInit                  , kCEETokenID_CLASS         , kJavaClassDefinitionTranslateStateClassSpecifier          );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateClassSpecifier        , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , '<'                       , kJavaClassDefinitionTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , kCEETokenID_EXTENDS       , kJavaClassDefinitionTranslateStateSuperclass              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , kCEETokenID_ANNOTATION    , kJavaClassDefinitionTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , '.'                       , kJavaClassDefinitionTranslateStateIdentifierSeparator     );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifierSeparator   , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , kCEETokenID_IMPLEMENTS    , kJavaClassDefinitionTranslateStateSuperinterfaces         );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , ','                       , kJavaClassDefinitionTranslateStateComma                   );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , '{'                       , kJavaClassDefinitionTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateComma                 , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateComma                 , kCEETokenID_ANNOTATION    , kJavaClassDefinitionTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateComma                 , ','                       , kJavaClassDefinitionTranslateStateComma                   );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateSuperclass            , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateSuperclass            , kCEETokenID_ANNOTATION    , kJavaClassDefinitionTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateAnnotation            , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateAnnotation            , '('                       , kJavaClassDefinitionTranslateStateAnnotationList          );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateAnnotationList        , ')'                       , kJavaClassDefinitionTranslateStateAnnotationListEnd       );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateAnnotationListEnd     , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateSuperinterfaces       , kCEETokenID_IDENTIFIER    , kJavaClassDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateSuperinterfaces       , kCEETokenID_ANNOTATION    , kJavaClassDefinitionTranslateStateAnnotation              );
}

static cee_boolean java_class_definition_trap(CEESourceFregment* fregment, 
                                              CEEList** pp)
{
    enum _State {
        kStateClass = 0,
        kStateSuperclass,
        kStateSuperinterfaces,
    } state = kStateClass;
    
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    JavaClassDefinitionTranslateState current = kJavaClassDefinitionTranslateStateInit;
    JavaClassDefinitionTranslateState prev = kJavaClassDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    
    CEEList* identifier = NULL;
    
    CEEList* superclass = NULL;
    CEEList* superclasses = NULL;
    cee_char* superclasses_str = NULL;
    
    CEEList* superinterface = NULL;
    CEEList* superinterfaces = NULL;
    cee_char* superinterfaces_str = NULL;
    
    cee_char* derives_str = NULL;
    
    CEESourceSymbol* definition = NULL;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_class_definition_translate_table[current][token->identifier];
                
        if (current == kJavaClassDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kJavaClassDefinitionTranslateStateSuperclass) {
            state = kStateSuperclass;
        }
        else if (current == kJavaClassDefinitionTranslateStateSuperinterfaces) {
            state = kStateSuperinterfaces;
        }
        else if (current == kJavaClassDefinitionTranslateStateAnnotation) {
        }
        else if (current == kJavaClassDefinitionTranslateStateComma) {
            if (cee_token_is_identifier(p, ',')) {
                if (state == kStateSuperclass) {
                    if (superclass) {
                        superclasses = cee_list_prepend(superclasses, superclass);
                        superclass = NULL;
                    }
                }
                else if (state == kStateSuperinterfaces) {
                    if (superinterface) {
                        superinterfaces = cee_list_prepend(superinterfaces, superinterface);
                        superinterface = NULL;
                    }
                }
            }
        }
        else if (current == kJavaClassDefinitionTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (state == kStateClass)
                    TOKEN_APPEND(identifier, token);
                else if (state == kStateSuperclass)
                    TOKEN_APPEND(superclass, token);
                else if (state == kStateSuperinterfaces)
                    TOKEN_APPEND(superinterface, token);
            }
        }
        else if (current == kJavaClassDefinitionTranslateStateConfirm) {
            if (superclass) {
                superclasses = cee_list_prepend(superclasses, superclass);
                superclass = NULL;
            }
            
            if (superinterface) {
                superinterfaces = cee_list_prepend(superinterfaces, superinterface);
                superinterface = NULL;
            }
            ret = TRUE;
            break;
        }
        else if (current == kJavaClassDefinitionTranslateStateError) {
            break;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kJavaClassDefinitionTranslateStateConfirm)
        goto exit;
    
    
    p = TOKEN_LAST(identifier);
    definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                           fregment->subject_ref,
                                                           p,
                                                           p,
                                                           kCEESourceSymbolTypeClassDefinition,
                                                           "java",
                                                           kCEETokenStringOptionCompact);
    
    if (definition) {
        if (superclasses) {
            superclasses_str = java_name_scope_list_string_create(superclasses,
                                                                  fregment->subject_ref);
            derives_str = cee_strdup(superclasses_str);
        }
        
        if (superinterfaces) {
            superinterfaces_str = java_name_scope_list_string_create(superinterfaces,
                                                                     fregment->subject_ref);
            if (derives_str)
                cee_strconcat0(&derives_str, ",", superinterfaces_str, NULL);
            else
                derives_str = cee_strdup(superinterfaces_str);
        }
        
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        definition->derives = derives_str;
        definition->proto_descriptor = java_object_type_proto_descriptor_create(fregment,
                                                                                definition,
                                                                                definition->derives);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeClassDefinition);
    }
    
    *pp = NULL;
    
#ifdef DEBUG_CLASS
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
exit:
    
    if (identifier)
        cee_list_free(identifier);
    
    if (superclass)
        cee_list_free(superclass);
    
    if (superclasses)
        cee_list_free_full(superclasses, superclass_free);
    
    if (superclasses_str)
        cee_free(superclasses_str);
    
    if (superinterface)
        cee_list_free(superinterface);
            
    if (superinterfaces)
        cee_list_free_full(superinterfaces, superinterface_free);
    
    if (superinterfaces_str)
        cee_free(superinterfaces_str);
    
    return ret;
}

static void java_interface_definition_translate_table_init(void)
{
    /**
     *                          interface_specifier     identifier          <                   extends             annotation       .                      (                   )                   ,           {
     *  Init                    InterfaceSpecifier      Error               Error               Error               Error           Error                   Error               Error               Error       Error
     *  InterfaceSpecifier      Error                   Identifier          Error               Error               Error           Error                   Error               Error               Error       Error
     *  Identifier              Error                   Identifier          *TypeParameters     ExtendInterface     Annotation      IdentifierSeparator     Error               Error               Comma       Confirm
     *  IdentifierSeparator     Error                   Identifier          Error               Error               Error           Error                   Error               Error               Error       Error
     *  Comma                   Error                   Identifier          Error               Error               Annotation      Error                   Error               Error               Comma       Error
     *  ExtendInterface         Error                   Identifier          Error               Error               Annotation      Error                   Error               Error               Error       Error
     *  Annotation              Error                   Identifier          Error               Error               Error           Error                   AnnotationList      Error               Error       Error
     *  AnnotationList          Error                   Error               Error               Error               Error           Error                   Error               AnnotationListEnd   Error       Error
     *  AnnotationListEnd       Error                   Identifier          Error               Error               Error           Error                   Error               Error               Error       Error
     *  
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateMax                   , kJavaInterfaceDefinitionTranslateStateError                                               );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateInit                  , kCEETokenID_INTERFACE     , kJavaInterfaceDefinitionTranslateStateInterfaceSpecifier      );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateInterfaceSpecifier    , kCEETokenID_IDENTIFIER    , kJavaInterfaceDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , kCEETokenID_IDENTIFIER    , kJavaInterfaceDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , '<'                       , kJavaInterfaceDefinitionTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , kCEETokenID_EXTENDS       , kJavaInterfaceDefinitionTranslateStateExtendInterface         );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , kCEETokenID_ANNOTATION    , kJavaInterfaceDefinitionTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , '.'                       , kJavaInterfaceDefinitionTranslateStateIdentifierSeparator     );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , ','                       , kJavaInterfaceDefinitionTranslateStateComma                   );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , '{'                       , kJavaInterfaceDefinitionTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifierSeparator   , kCEETokenID_IDENTIFIER    , kJavaInterfaceDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateExtendInterface       , kCEETokenID_IDENTIFIER    , kJavaInterfaceDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateExtendInterface       , kCEETokenID_ANNOTATION    , kJavaInterfaceDefinitionTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateAnnotation            , kCEETokenID_IDENTIFIER    , kJavaInterfaceDefinitionTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateAnnotation            , '('                       , kJavaInterfaceDefinitionTranslateStateAnnotationList          );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateAnnotationList        , ')'                       , kJavaInterfaceDefinitionTranslateStateAnnotationListEnd       );
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateAnnotationListEnd     , kCEETokenID_IDENTIFIER    , kJavaInterfaceDefinitionTranslateStateIdentifier              );
}

static cee_boolean java_interface_definition_trap(CEESourceFregment* fregment,
                                                  CEEList** pp)
{
    enum _State {
        kStateInterface = 0,
        kStateExtendInterfaces,
    } state = kStateInterface;
    
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    JavaInterfaceDefinitionTranslateState current = kJavaInterfaceDefinitionTranslateStateInit;
    JavaInterfaceDefinitionTranslateState prev = kJavaInterfaceDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    
    CEEList* identifier = NULL;
    
    CEEList* extendinterface = NULL;
    CEEList* extendinterfaces = NULL;
    cee_char* derives_str = NULL;
    
    CEESourceSymbol* definition = NULL;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_interface_definition_translate_table[current][token->identifier];
                
        if (current == kJavaInterfaceDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kJavaInterfaceDefinitionTranslateStateExtendInterface) {
            state = kStateExtendInterfaces;
        }
        else if (current == kJavaInterfaceDefinitionTranslateStateAnnotation) {
        }
        else if (current == kJavaInterfaceDefinitionTranslateStateComma) {
            if (cee_token_is_identifier(p, ',')) {
                if (state == kStateExtendInterfaces) {
                    if (extendinterface) {
                        extendinterfaces = cee_list_prepend(extendinterfaces, extendinterface);
                        extendinterface = NULL;
                    }
                }
            }
        }
        else if (current == kJavaInterfaceDefinitionTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (state == kStateInterface)
                    TOKEN_APPEND(identifier, token);
                else if (state == kStateExtendInterfaces)
                    TOKEN_APPEND(extendinterface, token);
            }
        }
        else if (current == kJavaInterfaceDefinitionTranslateStateConfirm) {
            if (extendinterface) {
                extendinterfaces = cee_list_prepend(extendinterfaces, extendinterface);
                extendinterface = NULL;
            }
            ret = TRUE;
            break;
        }
        else if (current == kJavaInterfaceDefinitionTranslateStateError) {
            break;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kJavaInterfaceDefinitionTranslateStateConfirm)
        goto exit;
    
    p = TOKEN_LAST(identifier);
    definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                           fregment->subject_ref, 
                                                           p, 
                                                           p, 
                                                           kCEESourceSymbolTypeInterfaceDefinition,
                                                           "java",
                                                           kCEETokenStringOptionCompact);
    if (definition) {
        
        if (extendinterfaces)
            derives_str = java_name_scope_list_string_create(extendinterfaces,
                                                             fregment->subject_ref);
        
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        definition->derives = derives_str;
        definition->proto_descriptor = java_object_type_proto_descriptor_create(fregment,
                                                                                definition,
                                                                                definition->derives);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeInterfaceDefinition);
    }
        
    *pp = NULL;
        
#ifdef DEBUG_INTERFACE
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
    return TRUE;
    
exit:
        
    if (extendinterface)
        cee_list_free(extendinterface);
    
    if (extendinterfaces)
        cee_list_free_full(extendinterfaces, extendinterface_free);
    
    if (identifier)
        cee_list_free(identifier);
    
    return ret;
}

static void java_enum_definition_translate_table_init(void)
{
    /**
     *                      enum_specifier      identifier          <                   annotation      .                       (                   )                   implements          ,           {
     *  Init                EnumSpecifier       Error               Error               Error           Error                   Error               Error               Error               Error       Error
     *  EnumSpecifier       Error               Identifier          Error               Error           Error                   Error               Error               Error               Error       Error
     *  Identifier          Error               Identifier          *TypeParameters     Annotation      IdentifierSeparator     Error               Error               Superinterfaces     Comma       Confirm
     *  IdentifierSeparator Error               Identifier          Error               Error           Error                   Error               Error               Error               Error       Error
     *  Comma               Error               Identifier          Error               Annotation      Error                   Error               Error               Error               Comma       Error
     *  Annotation          Error               Identifier          Error               Error           Error                   AnnotationList      Error               Error               Error       Error
     *  AnnotationList      Error               Error               Error               Error           Error                   Error               AnnotationListEnd   Error               Error       Error
     *  AnnotationListEnd   Error               Identifier          Error               Error           Error                   Error               Error               Error               Error       Error
     *  Superinterfaces     Error               Identifier          Error               Annotation      Error                   Error               Error               Error               Error       Error
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     */
    TRANSLATE_STATE_INI(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateMax                  , kJavaEnumDefinitionTranslateStateError                                                );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateInit                 , kCEETokenID_ENUM          , kJavaEnumDefinitionTranslateStateEnumSpecifier            );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateEnumSpecifier        , kCEETokenID_IDENTIFIER    , kJavaEnumDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , kCEETokenID_IDENTIFIER    , kJavaEnumDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , '<'                       , kJavaEnumDefinitionTranslateStateTypeParameters           );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , kCEETokenID_ANNOTATION    , kJavaEnumDefinitionTranslateStateAnnotation               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , '.'                       , kJavaEnumDefinitionTranslateStateIdentifierSeparator      );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , kCEETokenID_IMPLEMENTS    , kJavaEnumDefinitionTranslateStateSuperinterfaces          );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , ','                       , kJavaEnumDefinitionTranslateStateComma                    );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , '{'                       , kJavaEnumDefinitionTranslateStateConfirm                  );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifierSeparator  , kCEETokenID_IDENTIFIER    , kJavaEnumDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateComma                , kCEETokenID_IDENTIFIER    , kJavaEnumDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateComma                , kCEETokenID_ANNOTATION    , kJavaEnumDefinitionTranslateStateAnnotation               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateComma                , ','                       , kJavaEnumDefinitionTranslateStateComma                    );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateAnnotation           , kCEETokenID_IDENTIFIER    , kJavaEnumDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateAnnotation           , '('                       , kJavaEnumDefinitionTranslateStateAnnotationList           );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateAnnotationList       , ')'                       , kJavaEnumDefinitionTranslateStateAnnotationListEnd        );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateAnnotationListEnd    , kCEETokenID_IDENTIFIER    , kJavaEnumDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateSuperinterfaces      , kCEETokenID_IDENTIFIER    , kJavaEnumDefinitionTranslateStateIdentifier               );
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateSuperinterfaces      , kCEETokenID_ANNOTATION    , kJavaEnumDefinitionTranslateStateAnnotation               );
}

static cee_boolean java_enum_definition_trap(CEESourceFregment* fregment,
                                             CEEList** pp)
{
    enum _State {
        kStateEnum = 0,
        kStateSuperInterfaces,
    } state = kStateEnum;
    
    cee_boolean ret = FALSE;
    CEEList* p = *pp;
    JavaEnumDefinitionTranslateState current = kJavaEnumDefinitionTranslateStateInit;
    JavaEnumDefinitionTranslateState prev = kJavaEnumDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    
    CEEList* identifier = NULL;
    
    CEEList* supperinterface = NULL;
    CEEList* supperinterfaces = NULL;
    cee_char* derives_str = NULL;
    
    CEESourceSymbol* definition = NULL;
    
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_enum_definition_translate_table[current][token->identifier];
                
        if (current == kJavaEnumDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kJavaEnumDefinitionTranslateStateSuperinterfaces) {
            state = kStateSuperInterfaces;
        }
        else if (current == kJavaEnumDefinitionTranslateStateAnnotation) {
        }
        else if (current == kJavaEnumDefinitionTranslateStateComma) {
            if (cee_token_is_identifier(p, ',')) {
                if (state == kStateSuperInterfaces) {
                    if (supperinterface) {
                        supperinterfaces = cee_list_prepend(supperinterfaces, supperinterface);
                        supperinterface = NULL;
                    }
                }
            }
        }
        else if (current == kJavaEnumDefinitionTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (state == kStateEnum)
                    TOKEN_APPEND(identifier, token);
                else if (state == kStateSuperInterfaces)
                    TOKEN_APPEND(supperinterface, token);
            }
        }
        else if (current == kJavaEnumDefinitionTranslateStateConfirm) {
            if (supperinterface) {
                supperinterfaces = cee_list_prepend(supperinterfaces, supperinterface);
                supperinterface = NULL;
            }
            ret = TRUE;
            break;
        }
        else if (current == kJavaEnumDefinitionTranslateStateError) {
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kJavaEnumDefinitionTranslateStateConfirm)
        goto exit;
        

    
    p = TOKEN_LAST(identifier);
    definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                           fregment->subject_ref, 
                                                           p, 
                                                           p, 
                                                           kCEESourceSymbolTypeEnumDefinition, 
                                                           "java",
                                                           kCEETokenStringOptionCompact);
    if (definition) {
        if (supperinterfaces)
            derives_str = java_name_scope_list_string_create(supperinterfaces,
                                                             fregment->subject_ref);
        
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        definition->derives = derives_str;
        definition->proto_descriptor = java_object_type_proto_descriptor_create(fregment,
                                                                                definition,
                                                                                definition->derives);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeEnumDefinition);
    }
    
    *pp = NULL;
    
#ifdef DEBUG_ENUM
    if (definition)
        cee_source_symbol_print(definition);
#endif
    
exit:
    if (supperinterface)
        cee_list_free(supperinterface);
    
    if (supperinterfaces)
        cee_list_free_full(supperinterfaces, superinterface_free);
    
    if (identifier)
        cee_list_free(identifier);
    
    return ret;
}

static cee_boolean java_enumerators_parse(CEESourceFregment* fregment)
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
                                                                       "java",
                                                                       kCEETokenStringOptionCompact);
                if (enumerator) {
                    enumerator->proto_descriptor =
                        java_enumerator_proto_descriptor_create(token->fregment_ref,
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

static cee_char* java_enumerator_proto_descriptor_create(CEESourceFregment* fregment,
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

static cee_char* java_declaration_proto_descriptor_create(CEESourceFregment* fregment,
                                                          CEEList* prefix,
                                                          CEEList* prefix_tail,
                                                          CEEList* identifier)
{
    if (!fregment || !identifier)
        return NULL;
    
    CEESourceFregment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* proto = NULL;
    cee_char* descriptor = NULL;
    const cee_char* subject = fregment->subject_ref;
    cee_char* identifier_str = NULL;
    
    access_level = java_access_level_search(fregment, SOURCE_FREGMENT_TOKENS_FIRST(fregment), NULL);
    if (!access_level) {
        grandfather = cee_source_fregment_grandfather_get(fregment);
        if (grandfather) {
            if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumDefinition))
                access_level = "private";
            else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumeratorBlock))
                access_level = "public";
        }
        if (!access_level)
            access_level = "public";
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    cee_strconcat0(&descriptor, "\"type\":", "\"declaration\"", NULL);
    
    if (access_level)
        cee_strconcat0(&descriptor, ", \"access_level\":", "\"", access_level, "\"", NULL);
    
    if (prefix && prefix_tail)
        proto = java_type_descriptor_from_token_slice(fregment, prefix, prefix_tail);
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

static cee_char* java_object_type_proto_descriptor_create(CEESourceFregment* fregment,
                                                          CEESourceSymbol* definition,
                                                          const cee_char* derives_str)
{
    if (!fregment || !definition)
        return NULL;
    
    CEESourceFregment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* descriptor = NULL;
    
    access_level = java_access_level_search(fregment,
                                            SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                            NULL);
        
    if (!access_level) {
        grandfather = cee_source_fregment_grandfather_get(fregment);
        if (grandfather) {
            if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
        
    cee_strconcat0(&descriptor, "{", NULL);
    
    if (definition->type == kCEESourceSymbolTypeClassDefinition)
        cee_strconcat0(&descriptor, "\"type\":", "\"class\"", NULL);
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

static cee_char* java_name_scope_list_string_create(CEEList* scopes,
                                                    const cee_char* subject)
{
    CEEList* p = NULL;
    CEEList* q = NULL;
    cee_char* str = NULL;
    cee_char* scopes_str = NULL;
    
    p = cee_list_last(scopes);
    while (p) {
        q = p->data;
        str = java_name_scope_create(q, subject);
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

static cee_char* java_name_scope_create(CEEList* tokens,
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

static void superclass_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_list_free((CEEList*)data);
}

static void superinterface_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_list_free((CEEList*)data);
}

static void extendinterface_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_list_free((CEEList*)data);
}

static void java_enumerator_block_translate_table_init(void)
{
    /**
     *              ,       identifier      {
     *  Init        Int     Identifier      Error
     *  Identifier  Error   Error           Confirm
     */
    TRANSLATE_STATE_INI(java_enumerator_block_translate_table, kJavaEnumeratorBlockTranslateStateMax           , kJavaEnumeratorBlockTranslateStateError                                             );
    TRANSLATE_STATE_SET(java_enumerator_block_translate_table, kJavaEnumeratorBlockTranslateStateInit          , kCEETokenID_IDENTIFIER            , kJavaEnumeratorBlockTranslateStateIdentifier    );
    TRANSLATE_STATE_SET(java_enumerator_block_translate_table, kJavaEnumeratorBlockTranslateStateInit          , ','                               , kJavaEnumeratorBlockTranslateStateInit          );
    TRANSLATE_STATE_SET(java_enumerator_block_translate_table, kJavaEnumeratorBlockTranslateStateIdentifier    , '{'                               , kJavaEnumeratorBlockTranslateStateConfirm       );
}

static cee_boolean java_enumerator_block_parse(CEESourceFregment* fregment)
{
    if (!cee_source_fregment_grandfather_type_is(fregment, kCEESourceFregmentTypeEnumDefinition))
        return FALSE;
    
    cee_boolean ret = FALSE;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* identifier = NULL;
    CEESourceFregment* grandfather = NULL;
    CEESourceSymbol* enum_symbol = NULL;
    CEEList* enum_symbols = NULL;
    CEESourceSymbol* enumerator = NULL;
    JavaEnumeratorBlockTranslateState current = kJavaEnumeratorBlockTranslateStateInit;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        current = java_enumerator_block_translate_table[current][token->identifier];
        
        if (current == kJavaEnumeratorBlockTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        else if (current == kJavaEnumeratorBlockTranslateStateConfirm) {
            ret = TRUE;
            break;
        }
        else if (current == kJavaEnumeratorBlockTranslateStateError) {
            break;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kJavaEnumeratorBlockTranslateStateConfirm)
        goto exit;
    
    enumerator = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                           fregment->subject_ref,
                                                           identifier,
                                                           identifier,
                                                           kCEESourceSymbolTypeEnumerator,
                                                           "java",
                                                           kCEETokenStringOptionCompact);
    if (enumerator) {
        cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
        cee_source_fregment_type_set_exclusive(fregment, kCEESourceFregmentTypeEnumeratorBlock);
        
        grandfather = cee_source_fregment_grandfather_get(fregment);
        enum_symbols = cee_source_fregment_symbols_search_by_type(grandfather,
                                                                  kCEESourceSymbolTypeEnumDefinition);
        if (enum_symbols)
            enum_symbol = cee_list_nth_data(enum_symbols, 0);
        
        enumerator->proto_descriptor = java_enumerator_proto_descriptor_create(fregment,
                                                                               enumerator,
                                                                               enum_symbol->name);
        fregment->symbols = cee_list_prepend(fregment->symbols, enumerator);
    }
    
#ifdef DEBUG_METHOD_DEFINITION
    if (enumerator)
        cee_source_symbol_print(enumerator);
#endif
    
    if (enum_symbols)
        cee_list_free(enum_symbols);
    
exit:
    return ret;
}

static void java_method_translate_table_init(void)
{
    /**
     *                          declaration_specifier       builtin_type        identifier          .                       <                   annotation          throws          ,           (               )                   [                       ]                   {               ;
     *  Init                    DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Error           Error       Error           Error               Error                   Error               Error           Error
     *  DeclarationSpecifier    DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Error           Error       Error           Error               Error                   Error               Error           Error
     *  BuiltinType             DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Error           Error       Error           Error               BuiltinType             BuiltinType         Error           Error
     *  Identifier              Error                       Error               Identifier          Identifier              *TypeParameters     Annotation          Error           Error       ParameterList   Error               Identifier              Identifier          Error           Error
     *  ParameterList           Error                       Error               Error               Error                   Error               Error               Error           Error       Error           ParameterListEnd    Error                   Error               Error           Error
     *  ParameterListEnd        Error                       Error               Error               Error                   Error               Annotation          Throws          Error       Error           Error               ParameterListEnd        ParameterListEnd    Definition      Declaration
     *  Annotation              DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Throws          Error       AnnotationList  Error               Annotation              Annotation          Definition      Declaration
     *  AnnotationList          Error                       Error               Error               Error                   Error               Error               Error           Error       Error           AnnotationListEnd   Error                   Error               Error           Error
     *  AnnotationListEnd       DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Throws          Error       Error           Error               AnnotationListEnd       AnnotationListEnd   Definition      Declaration
     *  Throws                  Error                       Error               Exception           Error                   Error               Error               Error           Error       Error           Error               Error                   Error               Error           Error
     *  Exception               Error                       Error               Exception           ExceptionSeparator      *TypeParameters     Error               Error           Exception   Error           Error               Error                   Error               Definition      Declaration
     *  ExceptionSeparator      Error                       Error               Exception           Error                   Error               Error               Error           Error       Error           Error               Error                   Error               Error           Error
     *  
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     *  
     */
    TRANSLATE_STATE_INI(java_method_translate_table, kJavaMethodTranslateStateMax                  , kJavaMethodTranslateStateError                                                      );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateInit                 , token_id_is_declaration_specifier , kJavaMethodTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateInit                 , token_id_is_builtin_type          , kJavaMethodTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateInit                 , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateInit                 , '<'                               , kJavaMethodTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateInit                 , kCEETokenID_ANNOTATION            , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateDeclarationSpecifier , token_id_is_declaration_specifier , kJavaMethodTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateDeclarationSpecifier , token_id_is_builtin_type          , kJavaMethodTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateDeclarationSpecifier , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateDeclarationSpecifier , '<'                               , kJavaMethodTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateDeclarationSpecifier , kCEETokenID_ANNOTATION            , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateBuiltinType          , token_id_is_declaration_specifier , kJavaMethodTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateBuiltinType          , token_id_is_builtin_type          , kJavaMethodTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateBuiltinType          , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateBuiltinType          , '<'                               , kJavaMethodTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateBuiltinType          , kCEETokenID_ANNOTATION            , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateBuiltinType          , '['                               , kJavaMethodTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateBuiltinType          , ']'                               , kJavaMethodTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateIdentifier           , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateIdentifier           , '.'                               , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateIdentifier           , '<'                               , kJavaMethodTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateIdentifier           , kCEETokenID_ANNOTATION            , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateIdentifier           , '('                               , kJavaMethodTranslateStateParameterList          );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateIdentifier           , '['                               , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateIdentifier           , ']'                               , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateParameterList        , ')'                               , kJavaMethodTranslateStateParameterListEnd       );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateParameterListEnd     , kCEETokenID_ANNOTATION            , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateParameterListEnd     , kCEETokenID_THROWS                , kJavaMethodTranslateStateThrows                 );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateParameterListEnd     , '['                               , kJavaMethodTranslateStateParameterListEnd       );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateParameterListEnd     , ']'                               , kJavaMethodTranslateStateParameterListEnd       );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateParameterListEnd     , '{'                               , kJavaMethodTranslateStateDefinition             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateParameterListEnd     , ';'                               , kJavaMethodTranslateStateDeclaration            );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , token_id_is_declaration_specifier , kJavaMethodTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , token_id_is_builtin_type          , kJavaMethodTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , '<'                               , kJavaMethodTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , kCEETokenID_ANNOTATION            , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , kCEETokenID_THROWS                , kJavaMethodTranslateStateThrows                 );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , '('                               , kJavaMethodTranslateStateAnnotationList         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , '['                               , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , ']'                               , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , '{'                               , kJavaMethodTranslateStateDefinition             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotation           , ';'                               , kJavaMethodTranslateStateDeclaration            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationList       , ')'                               , kJavaMethodTranslateStateAnnotationListEnd      );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , token_id_is_declaration_specifier , kJavaMethodTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , token_id_is_builtin_type          , kJavaMethodTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , '<'                               , kJavaMethodTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , kCEETokenID_ANNOTATION            , kJavaMethodTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , kCEETokenID_THROWS                , kJavaMethodTranslateStateThrows                 );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , '['                               , kJavaMethodTranslateStateAnnotationListEnd      );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , ']'                               , kJavaMethodTranslateStateAnnotationListEnd      );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , '{'                               , kJavaMethodTranslateStateDefinition             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateAnnotationListEnd    , ';'                               , kJavaMethodTranslateStateDeclaration            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateThrows               , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateException              );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateException            , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateException              );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateException            , '.'                               , kJavaMethodTranslateStateExceptionSeparator     );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateException            , '<'                               , kJavaMethodTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateException            , ','                               , kJavaMethodTranslateStateException              );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateException            , '{'                               , kJavaMethodTranslateStateDefinition             );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateException            , ';'                               , kJavaMethodTranslateStateDeclaration            );
    TRANSLATE_STATE_SET(java_method_translate_table, kJavaMethodTranslateStateExceptionSeparator   , kCEETokenID_IDENTIFIER            , kJavaMethodTranslateStateException              );
}

static cee_boolean java_method_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    cee_boolean is_class_member = FALSE;
    JavaMethodTranslateState current = kJavaMethodTranslateStateInit;
    JavaMethodTranslateState prev = kJavaMethodTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEESourceFregment* child = NULL;
    CEESourceFregment* grandfather = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEESourceSymbol* method = NULL;
    CEESourceSymbolType symbol_type = kCEESourceSymbolTypeUnknow;
        
    grandfather = cee_source_fregment_grandfather_get(fregment);
    if (grandfather) {
        if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition) ||
            cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDefinition) ||
            cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumDefinition) ||
            cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumeratorBlock))
            is_class_member = TRUE;
    }
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_method_translate_table[current][token->identifier];
        
        if (current == kJavaMethodTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kJavaMethodTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (!parameter_list)
                    identifier = p;
            }
        }
        else if (current == kJavaMethodTranslateStateParameterList) {
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kJavaMethodTranslateStateParameterListEnd) {
            if (!parameter_list_end)
                parameter_list_end = p;
        }
        else if (current == kJavaMethodTranslateStateDeclaration) {
            if (!is_class_member) {
                current = kJavaMethodTranslateStateError;
                ret = FALSE;
            }
            else {
                symbol_type = kCEESourceSymbolTypeFunctionDeclaration;
                ret = TRUE;
            }
            break;
        }
        else if (current == kJavaMethodTranslateStateDefinition) {
            symbol_type = kCEESourceSymbolTypeFunctionDefinition;
            ret = TRUE;
            break;
        }
        else if (current == kJavaMethodTranslateStateError) {
            break;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if ((current != kJavaMethodTranslateStateDeclaration &&
         current != kJavaMethodTranslateStateDefinition) ||
        !identifier || !parameter_list)
        goto exit;
    
    method = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                       fregment->subject_ref,
                                                       identifier,
                                                       identifier,
                                                       symbol_type,
                                                       "java",
                                                       kCEETokenStringOptionCompact);
    if (method) {
        cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
        
        if (symbol_type == kCEESourceSymbolTypeFunctionDefinition)
            cee_source_fregment_type_set_exclusive(fregment, kCEESourceFregmentTypeFunctionDefinition);
        else if (symbol_type == kCEESourceSymbolTypeFunctionDeclaration)
            cee_source_fregment_type_set_exclusive(fregment, kCEESourceFregmentTypeFunctionDeclaration);
        
        child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
        if (child) {
            child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
            java_method_parameters_parse(child);
        }
        method->proto_descriptor = java_method_proto_descriptor_create(fregment,
                                                                       SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                                       TOKEN_PREV(identifier),
                                                                       identifier,
                                                                       parameter_list,
                                                                       parameter_list_end);
        fregment->symbols = cee_list_prepend(fregment->symbols, method);
    }
    
#ifdef DEBUG_METHOD_DEFINITION
    if (method)
        cee_source_symbol_print(method);
#endif
    
exit:
    
    return ret;
}

static void java_method_parameters_declaration_translate_table_init(void)
{
    /**
     *                          declaration_specifier       builtin_type        identifier          <                   ...                 annotation      ,           .       this        (               )                   [               ]    
     *  Init                    DeclarationSpecifier        BuiltinType         Identifier          *TypeParameters     Error               Annotation      Error       Error   Error       Error           Error               Error           Error
     *  DeclarationSpecifier    DeclarationSpecifier        BuiltinType         Identifier          *TypeParameters     Error               Annotation      Error       Error   Error       Error           Error               Error           Error
     *  BuiltinType             DeclarationSpecifier        BuiltinType         Identifier          *TypeParameters     BuiltinType         Annotation      Error       Error   Error       Error           Error               BuiltinType     BuiltinType
     *  Identifier              Error                       Error               Identifier          *TypeParameters     Identifier          Annotation      Confirm     Dot     This        Error           Error               Identifier      Identifier
     *  Annotation              DeclarationSpecifier        BuiltinType         Identifier          *TypeParameters     Annotation          Annotation      Error       Error   Error       AnnotationList  Error               Error           Error
     *  AnnotationList          Error                       Error               Error               Error               Error               Error           Error       Error   Error       Error           AnnotationListEnd   Error           Error
     *  AnnotationListEnd       DeclarationSpecifier        BuiltinType         Identifier          *TypeParameters     AnnotationListEnd   Annotation      Error       Error   Error       Error           Error               Error           Error
     *  Dot                     Error                       Error               Identifier          Error               Error               Annotation      Error       Error   This        Error           Error               Error           Error
     *  This                    Error                       Error               Error               Error               Error               Error           Confirm     Error   Error       Error           Error               Error           Error
     *
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     *  
     */
    TRANSLATE_STATE_INI(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateMax                   , kJavaMethodParametersDeclarationTranslateStateError                                                       );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateInit                  , token_id_is_declaration_specifier , kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateInit                  , token_id_is_builtin_type          , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateInit                  , kCEETokenID_IDENTIFIER            , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateInit                  , '<'                               , kJavaMethodParametersDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateInit                  , kCEETokenID_ANNOTATION            , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier  , token_id_is_declaration_specifier , kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier  , token_id_is_builtin_type          , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_IDENTIFIER            , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier  , '<'                               , kJavaMethodParametersDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier  , kCEETokenID_ANNOTATION            , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , token_id_is_declaration_specifier , kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , token_id_is_builtin_type          , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , kCEETokenID_IDENTIFIER            , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , '<'                               , kJavaMethodParametersDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , kCEETokenID_ELLIPSIS              , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , kCEETokenID_ANNOTATION            , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , '['                               , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateBuiltinType           , ']'                               , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , kCEETokenID_IDENTIFIER            , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , '.'                               , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , '<'                               , kJavaMethodParametersDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , kCEETokenID_ELLIPSIS              , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , kCEETokenID_ANNOTATION            , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , ','                               , kJavaMethodParametersDeclarationTranslateStateConfirm                 );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , '.'                               , kJavaMethodParametersDeclarationTranslateStateDot                     );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , kCEETokenID_THIS                  , kJavaMethodParametersDeclarationTranslateStateThis                    );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , '['                               , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , ']'                               , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotation            , token_id_is_declaration_specifier , kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotation            , token_id_is_builtin_type          , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotation            , kCEETokenID_IDENTIFIER            , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotation            , '<'                               , kJavaMethodParametersDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotation            , kCEETokenID_ELLIPSIS              , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotation            , kCEETokenID_ANNOTATION            , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotation            , '('                               , kJavaMethodParametersDeclarationTranslateStateAnnotationList          );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotationList        , ')'                               , kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd     , token_id_is_declaration_specifier , kJavaMethodParametersDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd     , token_id_is_builtin_type          , kJavaMethodParametersDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_IDENTIFIER            , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd     , '<'                               , kJavaMethodParametersDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_ELLIPSIS              , kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_ANNOTATION            , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDot                   , kCEETokenID_IDENTIFIER            , kJavaMethodParametersDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDot                   , kCEETokenID_ANNOTATION            , kJavaMethodParametersDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateDot                   , kCEETokenID_THIS                  , kJavaMethodParametersDeclarationTranslateStateThis                    );
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateThis                  , ','                               , kJavaMethodParametersDeclarationTranslateStateConfirm                 );
}

static cee_boolean java_method_parameters_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = TRUE;
    JavaMethodParametersDeclarationTranslateState current = kJavaMethodParametersDeclarationTranslateStateInit;
    JavaMethodParametersDeclarationTranslateState prev = kJavaMethodParametersDeclarationTranslateStateInit;
    CEESourceSymbol* parameter = NULL;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* prefix = NULL;
    CEEList* identifier = NULL;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    prefix = p;
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_method_parameters_declaration_translate_table[current][token->identifier];
        
        if (current == kJavaMethodParametersDeclarationTranslateStateError) {
            ret = FALSE;
            break;
        }
        
        if (current == kJavaMethodParametersDeclarationTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        
        if (current == kJavaMethodParametersDeclarationTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER))
                identifier = p;
        }
        
        if (current == kJavaMethodParametersDeclarationTranslateStateThis) {
            if (cee_token_is_identifier(p, kCEETokenID_THIS))
                identifier = p;
        }
        
        if (current == kJavaMethodParametersDeclarationTranslateStateConfirm || !TOKEN_NEXT(p)) {
            if (identifier)
                parameter = java_method_parameter_create(fregment, prefix, identifier);
                        
            if (parameter)
                fregment->symbols = cee_list_append(fregment->symbols, parameter);
            
            parameter = NULL;
            identifier = NULL;
            
            if (!cee_token_is_identifier(p, ',')) {
                p = skip_java_declaration_interval(p);
                if (!p)
                    break;
            }
            
            prefix = TOKEN_NEXT(p);
            current = kJavaMethodParametersDeclarationTranslateStateInit;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (fregment->symbols)
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
    
#ifdef DEBUG_METHOD_DEFINITION
    if (fregment->symbols)
        cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static CEESourceSymbol* java_method_parameter_create(CEESourceFregment* fregment,
                                                     CEEList* prefix,
                                                     CEEList* identifier)
{
    CEESourceSymbol* parameter = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                           fregment->subject_ref,
                                                                           identifier,
                                                                           identifier,
                                                                           kCEESourceSymbolTypeFunctionParameter,
                                                                           "java",
                                                                           kCEETokenStringOptionCompact);
    
    if (!parameter)
        return NULL;
    
    cee_token_slice_state_mark(identifier, identifier, kCEETokenStateSymbolOccupied);
    if (prefix && TOKEN_PREV(identifier)) {
        parameter->proto_descriptor = java_method_parameter_proto_descriptor_create(fregment,
                                                                                    prefix,
                                                                                    TOKEN_PREV(identifier),
                                                                                    parameter);
    }
    return parameter;
}

static cee_char* java_method_parameter_proto_descriptor_create(CEESourceFregment* fregment,
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
        proto_str = java_type_descriptor_from_token_slice(fregment, prefix, prefix_tail);
    if (proto_str) {
        cee_strconcat0(&descriptor, ", \"proto\":", "\"", proto_str, "\"", NULL);
        cee_free(proto_str);
    }
        
    cee_strconcat0(&descriptor, "}", NULL);
        
    return descriptor;
}

static cee_char* java_type_descriptor_from_token_slice(CEESourceFregment* fregment,
                                                       CEEList* p,
                                                       CEEList* q)
{
    const cee_char* subject = fregment->subject_ref;
    cee_char* proto = NULL;
    CEEToken* token = NULL;
    cee_boolean found_type = FALSE;
    cee_char* str = NULL;
    
    while (p) {
        token = p->data;
        
        /** find type descriptor */
        if (token_id_is_builtin_type(token->identifier) ||
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

static cee_char* java_method_proto_descriptor_create(CEESourceFregment* fregment,
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
    CEESourceSymbol* parent_symbol = NULL;
    CEESourceFregment* grandfather = NULL;
    const cee_char* access_level = "public";
    cee_char* return_type_str = NULL;
    
    grandfather = cee_source_fregment_grandfather_get(fregment);
    if (grandfather) {
        if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition))
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather,
                                                                        kCEESourceSymbolTypeClassDefinition);
        else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDefinition))
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather,
                                                                        kCEESourceSymbolTypeInterfaceDefinition);
        else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumDefinition))
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather,
                                                                        kCEESourceSymbolTypeEnumDefinition);
        
        if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition) ||
            cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDefinition) ||
            cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumDefinition) ||
            cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumeratorBlock))
            is_class_member = TRUE;
    }
    
    access_level = java_access_level_search(fregment, SOURCE_FREGMENT_TOKENS_FIRST(fregment), NULL);
    if (!access_level) {
        if (grandfather) {
            if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeInterfaceDefinition) ||
                cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumDefinition))
                access_level = "private";
            else if (cee_source_fregment_type_is(grandfather, kCEESourceFregmentTypeEnumeratorBlock))
                access_level = "public";
        }
        if (!access_level)
            access_level = "public";
    }
    
    if (prefix && prefix_tail)
        return_type_str = java_type_descriptor_from_token_slice(fregment, prefix, prefix_tail);
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

static const cee_char* java_access_level_search(CEESourceFregment* fregment,
                                                CEEList* begin,
                                                CEEList* end)
{
    if (!begin || !end)
        return NULL;
    
    CEEList* p = begin;
    while (p) {
        if (cee_token_is_identifier(p, kCEETokenID_PUBLIC))
            return "public";
        else if (cee_token_is_identifier(p, kCEETokenID_PROTECTED))
            return "protected";
        else if (cee_token_is_identifier(p, kCEETokenID_PRIVATE))
            return "private";
        
        if (p == end)
            break;
        
        p = TOKEN_NEXT(p);
    }
    
    return NULL;
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
