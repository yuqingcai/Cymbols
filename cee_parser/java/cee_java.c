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
    kJavaClassDefinitionTranslateStateCommit,
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
    kJavaInterfaceDefinitionTranslateStateCommit,
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
    kJavaEnumDefinitionTranslateStateCommit, 
    kJavaEnumDefinitionTranslateStateError, 
    kJavaEnumDefinitionTranslateStateMax, 
} JavaEnumDefinitionTranslateState;

typedef enum _JavaMethodDefinitionTranslateState {
    kJavaMethodDefinitionTranslateStateInit = 0, 
    kJavaMethodDefinitionTranslateStateDeclarationSpecifier, 
    kJavaMethodDefinitionTranslateStateBuiltinType, 
    kJavaMethodDefinitionTranslateStateIdentifier, 
    kJavaMethodDefinitionTranslateStateParameterList, 
    kJavaMethodDefinitionTranslateStateParameterListEnd, 
    kJavaMethodDefinitionTranslateStateAnnotation, 
    kJavaMethodDefinitionTranslateStateAnnotationList, 
    kJavaMethodDefinitionTranslateStateAnnotationListEnd, 
    kJavaMethodDefinitionTranslateStateThrows, 
    kJavaMethodDefinitionTranslateStateException, 
    kJavaMethodDefinitionTranslateStateExceptionSeparator, 
    kJavaMethodDefinitionTranslateStateTypeParameters,
    kJavaMethodDefinitionTranslateStateCommit, 
    kJavaMethodDefinitionTranslateStateError, 
    kJavaMethodDefinitionTranslateStateMax, 
} JavaMethodDefinitionTranslateState;

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
    kJavaMethodParametersDeclarationTranslateStateCommit, 
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
    kJavaDeclarationTranslateStateCommit, 
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
    kJavaImportStatementTranslateStateCommit, 
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
    kJavaPackageStatementTranslateStateCommit,
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
static cee_int java_method_definition_translate_table[kJavaMethodDefinitionTranslateStateMax][CEETokenID_MAX];
static cee_int java_method_parameters_declaration_translate_table[kJavaMethodParametersDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int java_declaration_translate_table[kJavaDeclarationTranslateStateMax][CEETokenID_MAX];
static cee_int java_import_statement_translate_table[kJavaImportStatementTranslateStateMax][CEETokenID_MAX];
static cee_int java_package_statement_translate_table[kJavaPackageStatementTranslateStateMax][CEETokenID_MAX];

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
static cee_boolean references_in_source_fregment_parse(const cee_char* filepath,
                                                       CEESourceFregment* fregment,
                                                       const cee_char* subject,
                                                       CEESourceFregment* prep_directive,
                                                       CEESourceFregment* statement,
                                                       CEERange range,
                                                       CEEList** references);
static CEESourceFregment* java_referernce_fregment_convert(CEESourceFregment* fregment,
                                                           const cee_char* subject);
static void java_reference_fregment_parse(const cee_char* filepath,
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
static CEESourceSymbol* java_method_declaration_create(CEESourceFregment* fregment,
                                                       const cee_char* type_str,
                                                       CEEList* parameter_list,
                                                       CEEList* throws);
static CEESourceSymbol* java_identifier_declaration_create(CEESourceFregment* fregment,
                                                           const cee_char* type_str,
                                                           CEEList* identifier);
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
static CEEList* enumerators_extract(CEEList* tokens,
                                    const cee_char* filepath,
                                    const cee_char* subject);
static cee_char* java_variable_proto_descriptor_create(CEESourceFregment* fregment,
                                                       CEESourceSymbol* definition,
                                                       CEEList* identifier,
                                                       const cee_char* proto);
static cee_char* java_class_proto_descriptor_create(CEESourceFregment* fregment,
                                                    CEESourceSymbol* definition,
                                                    CEEList* identifier,
                                                    const cee_char* derives_str);
static cee_char* java_name_scope_list_string_create(CEEList* scopes,
                                                    const cee_char* subject);
static cee_char* java_name_scope_create(CEEList* tokens,
                                        const cee_char* subject);
static void superclass_free(cee_pointer data);
static void superinterface_free(cee_pointer data);
static void extendinterface_free(cee_pointer data);
static void java_method_definition_translate_table_init(void);
static cee_boolean java_method_definition_parse(CEESourceFregment* fregment);
static void java_method_parameters_declaration_translate_table_init(void);
static cee_boolean java_method_parameters_parse(CEESourceFregment* fregment);
static CEESourceSymbol* java_method_parameter_create(CEESourceFregment* fregment,
                                                     CEEList* head,
                                                     CEEList* begin,
                                                     CEEList* end);
static cee_char* java_type_descriptor_from_token_slice(CEESourceFregment* fregment,
                                                       CEEList* p,
                                                       CEEList* q);
static cee_char* java_method_definition_proto_descriptor_create(CEESourceFregment* fregment,
                                                                CEESourceSymbol* definition,
                                                                CEEList* parameters,
                                                                CEEList* method,
                                                                CEEList* throws,
                                                                CEEList* commit);
static cee_char* java_method_declaration_proto_descriptor_create(CEESourceFregment* fregment,
                                                                 CEESourceSymbol* definition,
                                                                 CEEList* parameters,
                                                                 CEEList* method,
                                                                 const cee_char* return_str,
                                                                 CEEList* throws);
static void exception_free(cee_pointer data);
static const cee_char* java_access_level_search(CEESourceFregment* fregment,
                                                CEEList* begin,
                                                CEEList* end);
/**
 * parser
 */
CEESourceParserRef cee_java_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->symbol_parse = symbol_parse;
    parser->reference_parse = reference_parse;
    parser->token_type_matcher = token_type_matcher;
    
    JavaParser* java = parser_create();
    java->super = parser;
    
    java_block_header_trap_init(java);
    java_token_type_map_init();
    
    java_class_definition_translate_table_init();
    java_interface_definition_translate_table_init();
    java_enum_definition_translate_table_init();
    java_method_definition_translate_table_init();
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
    
    reference_fregment = java_referernce_fregment_convert(fregment, subject);
    java_reference_fregment_parse(filepath,
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

static void java_reference_fregment_parse(const cee_char* filepath,
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
        java_reference_fregment_parse(filepath,
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
    
    if (java_method_definition_parse(current))
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
    CEESourceFregment* current = parser->statement_current;
    const cee_char* subject = parser->subject_ref;
    const cee_char* filepath = current->filepath_ref;
    
    if (!current || !current->tokens_ref)
        return;
    
    /** enumerators parse */
    if (cee_source_fregment_grandfather_type_is(current, kCEESourceFregmentTypeEnumDefinition)) {
        CEEList* enumerators = NULL;
        enumerators = enumerators_extract(current->tokens_ref, filepath, subject);
        if (enumerators) {
            current->symbols = cee_list_concat(current->symbols, enumerators);
            cee_source_fregment_type_set(current, kCEESourceFregmentTypeEnumurators);
        }
    }
}

static cee_boolean block_reduce(JavaParser* parser)
{
    if (!parser->statement_current)
        return FALSE;
    
    cee_source_fregment_symbols_fregment_range_mark(parser->statement_current);
    statement_attach(parser, kCEESourceFregmentTypeStatement);
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
    
    if (java_declaration_parse(current) ||
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
     *                          declaration_specifier       builtin_type        identifier          assign_operator         .                       <                   annotation          throws          ,           (               )                   [                   ]                   default     ;
     *  Init                    DeclarationSpecifier        BuiltinType         CustomType          Error                   Error                   *TypeParameters     Annotation          Error           Error       Error           Error               Error               Error               Error       Commit
     *  DeclarationSpecifier    DeclarationSpecifier        BuiltinType         CustomType          Error                   Error                   *TypeParameters     Annotation          Error           Error       Error           Error               Error               Error               Error       Error
     *  BuiltinType             DeclarationSpecifier        BuiltinType         Identifier          Error                   Error                   *TypeParameters     Annotation          Error           Error       Error           Error               BuiltinType         BuiltinType         Error       Error
     *  CustomType              Error                       Error               Identifier          Error                   CustomTypeSeparator     *TypeParameters     Annotation          Error           Error       Structor        Error               CustomType          CustomType          Error       Error
     *  Structor                Error                       Error               Error               Error                   Error                   Error               Error               Error           Error       Error           StructorEnd         Error               Error               Error       Error
     *  StructorEnd             Error                       Error               Error               Error                   Error                   *TypeParameters     Error               Throws          Commit      Error           Error               Error               Error               Error       Commit
     *  Identifier              Error                       Error               Error               Commit                  Error                   Error               Error               Error           Commit      ParameterList   Error               Identifier          Identifier          Error       Commit
     *  CustomTypeSeparator     Error                       Error               CustomType          Error                   Error                   Error               Error               Error           Error       Error           Error               Error               Error               Error       Error
     *  ParameterList           Error                       Error               Error               Error                   Error                   Error               Error               Error           Error       Error           ParameterListEnd    Error               Error               Error       Error
     *  ParameterListEnd        Error                       Error               Error               Error                   Error                   Error               Annotation          Throws          Commit      Error           Error               Error               Error               *Default    Commit
     *  Annotation              DeclarationSpecifier        BuiltinType         CustomType          Error                   Error                   *TypeParameters     Annotation          Throws          Commit      AnnotationList  Error               Annotation          Annotation          *Default    Commit
     *  AnnotationList          Error                       Error               Error               Error                   Error                   Error               Error               Error           Error       Error           AnnotationListEnd   Error               Error               Error       Error
     *  AnnotationListEnd       DeclarationSpecifier        BuiltinType         CustomType          Error                   Error                   *TypeParameters     Annotation          Throws          Commit      Error           Error               AnnotationListEnd   AnnotationListEnd   *Default    Commit
     *  TypeInit                Error                       Error               Identifier          Error                   Error                   Error               Error               Error           Error       Error           Error               Error               Error               Error       Error
     *  Throws                  Error                       Error               Exception           Error                   Error                   Error               Error               Error           Error       Error           Error               Error               Error               Error       Error
     *  Exception               Error                       Error               Exception           Error                   ExceptionSeparator      *TypeParameters     Error               Error           Exception   Error           Error               Error               Error               Error       Commit
     *  ExceptionSeparator      Error                       Error               Exception           Error                   Error                   Error               Error               Error           Error       Error           Error               Error               Error               Error       Error
     *  Commit                  Error                       Error               Identifier          Error                   Error                   Error               Error               Error           Commit      Error           Error               Error               Error               Error       Commit
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
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , '('                                   , kJavaDeclarationTranslateStateStructor                );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , '['                                   , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomType            , ']'                                   , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateStructor              , ')'                                   , kJavaDeclarationTranslateStateStructorEnd             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateStructorEnd           , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateStructorEnd           , kCEETokenID_THROWS                    , kJavaDeclarationTranslateStateThrows                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateStructorEnd           , ','                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateStructorEnd           , ';'                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , token_id_is_assignment                , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , ','                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , '('                                   , kJavaDeclarationTranslateStateParameterList           );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , '['                                   , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , ']'                                   , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateIdentifier            , ';'                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCustomTypeSeparator   , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateParameterList         , ')'                                   , kJavaDeclarationTranslateStateParameterListEnd        );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateParameterListEnd      , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateParameterListEnd      , kCEETokenID_THROWS                    , kJavaDeclarationTranslateStateThrows                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateParameterListEnd      , ','                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateParameterListEnd      , kCEETokenID_DEFAULT                   , kJavaDeclarationTranslateStateDefault                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateParameterListEnd      , ';'                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , token_id_is_declaration_specifier     , kJavaDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , token_id_is_builtin_type              , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , kCEETokenID_THROWS                    , kJavaDeclarationTranslateStateThrows                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , ','                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , '('                                   , kJavaDeclarationTranslateStateAnnotationList          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , '['                                   , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , ']'                                   , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , kCEETokenID_DEFAULT                   , kJavaDeclarationTranslateStateDefault                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotation            , ';'                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationList        , ')'                                   , kJavaDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , token_id_is_declaration_specifier     , kJavaDeclarationTranslateStateDeclarationSpecifier    );
    TRANSLATE_FUNCS_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , token_id_is_builtin_type              , kJavaDeclarationTranslateStateBuiltinType             );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateCustomType              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_ANNOTATION                , kJavaDeclarationTranslateStateAnnotation              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_THROWS                    , kJavaDeclarationTranslateStateThrows                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , ','                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , '['                                   , kJavaDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , ']'                                   , kJavaDeclarationTranslateStateAnnotationListEnd       );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , kCEETokenID_DEFAULT                   , kJavaDeclarationTranslateStateDefault                 );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateAnnotationListEnd     , ';'                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateTypeInit              , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateThrows                , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateException               );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateException             , '.'                                   , kJavaDeclarationTranslateStateExceptionSeparator      );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateException             , '<'                                   , kJavaDeclarationTranslateStateTypeParameters          );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateException             , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateException               );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateException             , ','                                   , kJavaDeclarationTranslateStateException               );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateException             , ';'                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateExceptionSeparator    , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateException               );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCommit                , kCEETokenID_IDENTIFIER                , kJavaDeclarationTranslateStateIdentifier              );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCommit                , ','                                   , kJavaDeclarationTranslateStateCommit                  );
    TRANSLATE_STATE_SET(java_declaration_translate_table,   kJavaDeclarationTranslateStateCommit                , ';'                                   , kJavaDeclarationTranslateStateCommit                  );
}

static cee_boolean java_declaration_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    cee_boolean is_class_member = FALSE;
    JavaDeclarationTranslateState current = kJavaDeclarationTranslateStateInit;
    JavaDeclarationTranslateState prev = kJavaDeclarationTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* parameter_list_end = NULL;
    CEEList* throws = NULL;
    CEEList* declarations = NULL;
    CEESourceSymbol* declaration = NULL;
    cee_char* type = NULL;
    CEEList* parent_symbols = NULL;
    CEESourceSymbol* parent_symbol = NULL;
    CEESourceFregment* grandfather_fregment = NULL;
    
    grandfather_fregment = cee_source_fregment_grandfather_get(fregment);
    if (grandfather_fregment) {
        if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeClassDefinition))
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather_fregment,
                                                                        kCEESourceSymbolTypeClassDefinition);
        else if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeInterfaceDefinition))
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather_fregment, 
                                                                        kCEESourceSymbolTypeInterfaceDefinition);
        else if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeEnumDefinition)) {
            parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather_fregment, 
                                                                        kCEESourceSymbolTypeEnumDefinition);
        }
        
        if (parent_symbols) {
            parent_symbol = cee_list_nth_data(parent_symbols, 0);
            is_class_member = TRUE;
        }
    }
        
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
        else if (current == kJavaDeclarationTranslateStateStructor) {
            if (!is_class_member)
                break;
            
            if (!parameter_list &&
                cee_token_is_identifier(p, '('))
                parameter_list = p;
        }
        else if (current == kJavaDeclarationTranslateStateStructorEnd) {
            if (!parameter_list_end &&
                cee_token_is_identifier(p, ')'))
                parameter_list_end = p;
        }
        else if (current == kJavaDeclarationTranslateStateParameterList) {
            if (!parameter_list &&
                cee_token_is_identifier(p, '('))
                parameter_list = p;
        }
        else if (current == kJavaDeclarationTranslateStateParameterListEnd) {
            if (!parameter_list_end &&
                cee_token_is_identifier(p, ')'))
                parameter_list_end = p;
        }
        else if (current == kJavaDeclarationTranslateStateThrows) {
            if (cee_token_is_identifier(p, kCEETokenID_THROWS))
                throws = p;
        }
        else if (current == kJavaDeclarationTranslateStateDefault || 
                 current == kJavaDeclarationTranslateStateCommit) {
            
            if (!type) {
                if (identifier) {
                    type = java_type_descriptor_from_token_slice(fregment, 
                                                                 SOURCE_FREGMENT_TOKENS_FIRST(fregment), 
                                                                 TOKEN_PREV(identifier));
                }
                else {
                    /** 
                     * Constructor doesn't has return type declaraton, 
                     * use its parent symbol type as return type
                     */
                    if (parent_symbol) 
                        type = cee_strdup(parent_symbol->name);
                } 
            }
            
            if (parameter_list && parameter_list_end)
                declaration = java_method_declaration_create(fregment, 
                                                             type, 
                                                             parameter_list, 
                                                             throws);
            else if (identifier)
                declaration = java_identifier_declaration_create(fregment,
                                                                 type,
                                                                 identifier);
            
            if (declaration) {
                /** Java enumerator can be a constructor, set these symbols to be an enumerator explicitly */
                if (cee_source_fregment_type_is(grandfather_fregment,
                                                kCEESourceFregmentTypeEnumDefinition))
                    declaration->type = kCEESourceSymbolTypeEnumerator;
                
                declarations = cee_list_prepend(declarations, declaration);
            }
            
            declaration = NULL;
            identifier = NULL;
            parameter_list = NULL;
            parameter_list_end = NULL;
            throws = NULL;
            
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
    cee_source_symbols_print(declarations);
#endif
    
exit:
    
    if (type)
        cee_free(type);
    
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

static CEESourceSymbol* java_method_declaration_create(CEESourceFregment* fregment,
                                                       const cee_char* type_str,
                                                       CEEList* parameter_list,
                                                       CEEList* throws)
{
    CEESourceFregment* child = NULL;
    CEESourceSymbol* declaration = NULL;
    CEEList* q = NULL;
    CEEList* identifier = NULL;
    
    q = cee_token_not_whitespace_newline_before(parameter_list);
    if (!cee_token_is_identifier(q, kCEETokenID_IDENTIFIER))
        return NULL;
    
    identifier = q;
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            identifier, 
                                                            identifier,
                                                            kCEESourceSymbolTypeFunctionDeclaration,
                                                            "java",
                                                            kCEETokenStringOptionCompact);
    if (declaration) {
        
        child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
        if (child) {
            child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
            java_method_parameters_parse(child);
        }
        
        declaration->proto = java_method_declaration_proto_descriptor_create(fregment,
                                                                             declaration,
                                                                             child->symbols,
                                                                             identifier,
                                                                             type_str,
                                                                             throws);
        cee_token_slice_state_mark(identifier,
                                   identifier,
                                   kCEETokenStateSymbolOccupied);
    }
    return declaration;
}

static CEESourceSymbol* java_identifier_declaration_create(CEESourceFregment* fregment,
                                                           const cee_char* type_str,
                                                           CEEList* identifier)
{
    CEESourceSymbol* declaration = NULL;
    declaration = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                            fregment->subject_ref,
                                                            identifier,
                                                            identifier,
                                                            kCEESourceSymbolTypeVariableDeclaration,
                                                            "java",
                                                            kCEETokenStringOptionCompact);
    if (!declaration)
        return NULL;
    
    declaration->proto = java_variable_proto_descriptor_create(fregment, 
                                                               declaration,
                                                               identifier,
                                                               type_str);
    cee_token_slice_state_mark(identifier,
                               identifier,
                               kCEETokenStateSymbolOccupied);
    return declaration;
}

static void java_import_statement_translate_table_init(void)
{
    /**
     *                              import      static      Identifier      .                       *       ;
     *  Init                        Import      Error       Error           Error                   Error   Error
     *  Import                      Error       Import      Identifier      Error                   Error   Commit
     *  Identifier                  Error       Error       Error           IdentifierSeparator     Error   Commit
     *  IdentifierSeparator         Error       Error       Identifier      Error                   Demand  Error
     *  Demand                      Error       Error       Error           Error                   Error   Commit
     */
    TRANSLATE_STATE_INI(java_import_statement_translate_table, kJavaImportStatementTranslateStateMax                    , kJavaImportStatementTranslateStateError                                               );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateInit                   , kCEETokenID_IMPORT            , kJavaImportStatementTranslateStateImport              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateImport                 , kCEETokenID_STATIC            , kJavaImportStatementTranslateStateImport              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateImport                 , kCEETokenID_IDENTIFIER        , kJavaImportStatementTranslateStateIdentifier          );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateImport                 , ';'                           , kJavaImportStatementTranslateStateCommit              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifier             , '.'                           , kJavaImportStatementTranslateStateIdentifierSeparator );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifier             , ';'                           , kJavaImportStatementTranslateStateCommit              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifierSeparator    , kCEETokenID_IDENTIFIER        , kJavaImportStatementTranslateStateIdentifier          );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateIdentifierSeparator    , '*'                           , kJavaImportStatementTranslateStateDemand              );
    TRANSLATE_STATE_SET(java_import_statement_translate_table, kJavaImportStatementTranslateStateDemand                 , ';'                           , kJavaImportStatementTranslateStateCommit              );
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
        else if (current == kJavaImportStatementTranslateStateCommit) {
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
    
    if (current != kJavaImportStatementTranslateStateCommit)
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
     *  Identifier              Error           Error           Error           IdentifierSeparator     Error           Error               Commit
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
    TRANSLATE_STATE_SET(java_package_statement_translate_table, kJavaPackageStatementTranslateStateIdentifier               , ';'                               , kJavaPackageStatementTranslateStateCommit                 );
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
        else if (current == kJavaPackageStatementTranslateStateCommit) {
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
    
    if (current != kJavaPackageStatementTranslateStateCommit)
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
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeDeclaration);
        ret = TRUE;
    }
    
#ifdef DEBUG_IMPORT
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
     *  Identifier          Error               Identifier          *TypeParameters     Superclass      Annotation      IdentifierSeparator     Error               Error               Superinterfaces     Comma       Commit
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
    TRANSLATE_STATE_SET(java_class_definition_translate_table,  kJavaClassDefinitionTranslateStateIdentifier            , '{'                       , kJavaClassDefinitionTranslateStateCommit                  );
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
        else if (current == kJavaClassDefinitionTranslateStateCommit) {
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
    
    if (current != kJavaClassDefinitionTranslateStateCommit)
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
                cee_strconcat0(&derives_str, " ", superinterfaces_str, NULL);
            else
                derives_str = cee_strdup(superinterfaces_str);
        }
        
        cee_token_slice_state_mark(p, p, kCEETokenStateSymbolOccupied);
        definition->derives = derives_str;
        definition->proto = java_class_proto_descriptor_create(fregment,
                                                               definition,
                                                               p,
                                                               derives_str);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeClassDefinition);
    }
    
    *pp = NULL;
    
#ifdef DEBUG_CLASS
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
     *  Identifier              Error                   Identifier          *TypeParameters     ExtendInterface     Annotation      IdentifierSeparator     Error               Error               Comma       Commit
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
    TRANSLATE_STATE_SET(java_interface_definition_translate_table,  kJavaInterfaceDefinitionTranslateStateIdentifier            , '{'                       , kJavaInterfaceDefinitionTranslateStateCommit                  );
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
        else if (current == kJavaInterfaceDefinitionTranslateStateCommit) {
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
    
    if (current != kJavaInterfaceDefinitionTranslateStateCommit)
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
        definition->proto = java_class_proto_descriptor_create(fregment,
                                                               definition,
                                                               p,
                                                               derives_str);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeInterfaceDefinition);
    }
        
    *pp = NULL;
        
#ifdef DEBUG_INTERFACE
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
     *  Identifier          Error               Identifier          *TypeParameters     Annotation      IdentifierSeparator     Error               Error               Superinterfaces     Comma       Commit
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
    TRANSLATE_STATE_SET(java_enum_definition_translate_table, kJavaEnumDefinitionTranslateStateIdentifier           , '{'                       , kJavaEnumDefinitionTranslateStateCommit                   );
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
        else if (current == kJavaEnumDefinitionTranslateStateCommit) {
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
    
    if (current != kJavaEnumDefinitionTranslateStateCommit)
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
        definition->proto = java_class_proto_descriptor_create(fregment,
                                                               definition,
                                                               p,
                                                               derives_str);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeEnumDefinition);
    }
    
    *pp = NULL;
    
#ifdef DEBUG_ENUM
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

static CEEList* enumerators_extract(CEEList* tokens,
                                    const cee_char* filepath,
                                    const cee_char* subject)
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
        
        if (identifier == kCEETokenID_IDENTIFIER && enumurating)
            q = p;
        
        if (identifier == ',' || !TOKEN_NEXT(p)) {
            if (q) {
                enumerator = cee_source_symbol_create_from_token_slice(filepath,
                                                                       subject, 
                                                                       q, 
                                                                       q, 
                                                                       kCEESourceSymbolTypeEnumerator, 
                                                                       "java",
                                                                       kCEETokenStringOptionCompact);
                if (enumerator) {
                    enumerator->proto = java_variable_proto_descriptor_create(token->fregment_ref,
                                                                              enumerator,
                                                                              q,
                                                                              enum_symbol->name);
                    cee_token_slice_state_mark(q, q, kCEETokenStateSymbolOccupied);
                    enumerators = cee_list_prepend(enumerators, enumerator);
                }
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

static cee_char* java_variable_proto_descriptor_create(CEESourceFregment* fregment,
                                                       CEESourceSymbol* definition,
                                                       CEEList* identifier,
                                                       const cee_char* proto)
{
    if (!fregment || !definition)
        return NULL;
    
    CEESourceFregment* grandfather_fregment = NULL;
    const cee_char* access_level = "public";
    cee_char* descriptor = NULL;
    
    if (identifier)
        access_level = java_access_level_search(fregment,
                                                SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                TOKEN_PREV(identifier));
    if (!access_level) {
        grandfather_fregment = cee_source_fregment_grandfather_get(fregment);
        if (grandfather_fregment) {
            if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeInterfaceDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeEnumDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    
    cee_strconcat0(&descriptor, "{", NULL);
    
    if (definition->type == kCEESourceSymbolTypeLabel)
        cee_strconcat0(&descriptor, "type:", "label", ",", NULL);
    else if (definition->type == kCEESourceSymbolTypeEnumerator)
        cee_strconcat0(&descriptor, "type:", "enumerator", ",", NULL);
    else
        cee_strconcat0(&descriptor, "type:", "variable", ",", NULL);
    
    cee_strconcat0(&descriptor, "name:", definition->name, ",", NULL);
    cee_strconcat0(&descriptor, "access_level:", access_level, ",", NULL);
    cee_strconcat0(&descriptor, "proto:", proto, NULL);
    cee_strconcat0(&descriptor, "}", NULL);

    return descriptor;
}

static cee_char* java_class_proto_descriptor_create(CEESourceFregment* fregment,
                                                    CEESourceSymbol* definition,
                                                    CEEList* identifier,
                                                    const cee_char* derives_str)
{
    if (!fregment || !definition)
        return NULL;
    
    CEESourceFregment* grandfather_fregment = NULL;
    const cee_char* access_level = "public";
    cee_char* descriptor = NULL;
    
    if (identifier)
        access_level = java_access_level_search(fregment,
                                                SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                TOKEN_PREV(identifier));
    if (!access_level) {
        grandfather_fregment = cee_source_fregment_grandfather_get(fregment);
        if (grandfather_fregment) {
            if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeInterfaceDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeEnumDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
        
    cee_strconcat0(&descriptor, "{", NULL);
    
    if (definition->type == kCEESourceSymbolTypeClassDefinition)
        cee_strconcat0(&descriptor, "type:", "class_definition", ",", NULL);
    else if (definition->type == kCEESourceSymbolTypeInterfaceDefinition)
        cee_strconcat0(&descriptor, "type:", "interface_definition", ",", NULL);
    else if (definition->type == kCEESourceSymbolTypeEnumDefinition)
        cee_strconcat0(&descriptor, "type:", "enum_definition", ",", NULL);
    else
        cee_strconcat0(&descriptor, "type:", "class_declaration", ",", NULL);
    
    cee_strconcat0(&descriptor, "name:", definition->name, ",", NULL);
    cee_strconcat0(&descriptor, "access_level:", access_level, ",", NULL);
    cee_strconcat0(&descriptor, "derivers:", derives_str, NULL);
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
                cee_strconcat0(&scopes_str, " ", NULL);
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

static void java_method_definition_translate_table_init(void)
{
    /**
     *                          declaration_specifier       builtin_type        identifier          .                       <                   annotation          throws          ,           (               )                   [                       ]                   {
     *  Init                    DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Error           Error       Error           Error               Error                   Error               Error
     *  DeclarationSpecifier    DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Error           Error       Error           Error               Error                   Error               Error
     *  BuiltinType             DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Error           Error       Error           Error               BuiltinType             BuiltinType         Error
     *  Identifier              Error                       Error               Identifier          Identifier              *TypeParameters     Annotation          Error           Error       ParameterList   Error               Identifier              Identifier          Error
     *  ParameterList           Error                       Error               Error               Error                   Error               Error               Error           Error       Error           ParameterListEnd    Error                   Error               Error
     *  ParameterListEnd        Error                       Error               Error               Error                   Error               Annotation          Throws          Error       Error           Error               ParameterListEnd        ParameterListEnd    Commit
     *  Annotation              DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Throws          Error       AnnotationList  Error               Annotation              Annotation          Commit
     *  AnnotationList          Error                       Error               Error               Error                   Error               Error               Error           Error       Error           AnnotationListEnd   Error                   Error               Error  
     *  AnnotationListEnd       DeclarationSpecifier        BuiltinType         Identifier          Error                   *TypeParameters     Annotation          Throws          Error       Error           Error               AnnotationListEnd       AnnotationListEnd   Commit
     *  Throws                  Error                       Error               Exception           Error                   Error               Error               Error           Error       Error           Error               Error                   Error               Error
     *  Exception               Error                       Error               Exception           ExceptionSeparator      *TypeParameters     Error               Error           Exception   Error           Error               Error                   Error               Commit
     *  ExceptionSeparator      Error                       Error               Exception           Error                   Error               Error               Error           Error       Error           Error               Error                   Error               Error
     *  
     *  TypeParameters: save 'current state', skip TypeParameterList then restore 'current state'
     *  
     */
    TRANSLATE_STATE_INI(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateMax                  , kJavaMethodDefinitionTranslateStateError                                                      );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateInit                 , token_id_is_declaration_specifier , kJavaMethodDefinitionTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateInit                 , token_id_is_builtin_type          , kJavaMethodDefinitionTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateInit                 , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateInit                 , '<'                               , kJavaMethodDefinitionTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateInit                 , kCEETokenID_ANNOTATION            , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateDeclarationSpecifier , token_id_is_declaration_specifier , kJavaMethodDefinitionTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateDeclarationSpecifier , token_id_is_builtin_type          , kJavaMethodDefinitionTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateDeclarationSpecifier , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateDeclarationSpecifier , '<'                               , kJavaMethodDefinitionTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateDeclarationSpecifier , kCEETokenID_ANNOTATION            , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateBuiltinType          , token_id_is_declaration_specifier , kJavaMethodDefinitionTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateBuiltinType          , token_id_is_builtin_type          , kJavaMethodDefinitionTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateBuiltinType          , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateBuiltinType          , '<'                               , kJavaMethodDefinitionTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateBuiltinType          , kCEETokenID_ANNOTATION            , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateBuiltinType          , '['                               , kJavaMethodDefinitionTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateBuiltinType          , ']'                               , kJavaMethodDefinitionTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateIdentifier           , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateIdentifier           , '.'                               , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateIdentifier           , '<'                               , kJavaMethodDefinitionTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateIdentifier           , kCEETokenID_ANNOTATION            , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateIdentifier           , '('                               , kJavaMethodDefinitionTranslateStateParameterList          );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateIdentifier           , '['                               , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateIdentifier           , ']'                               , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateParameterList        , ')'                               , kJavaMethodDefinitionTranslateStateParameterListEnd       );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateParameterListEnd     , kCEETokenID_ANNOTATION            , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateParameterListEnd     , kCEETokenID_THROWS                , kJavaMethodDefinitionTranslateStateThrows                 );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateParameterListEnd     , '['                               , kJavaMethodDefinitionTranslateStateParameterListEnd       );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateParameterListEnd     , ']'                               , kJavaMethodDefinitionTranslateStateParameterListEnd       );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateParameterListEnd     , '{'                               , kJavaMethodDefinitionTranslateStateCommit                 );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , token_id_is_declaration_specifier , kJavaMethodDefinitionTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , token_id_is_builtin_type          , kJavaMethodDefinitionTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , '<'                               , kJavaMethodDefinitionTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , kCEETokenID_ANNOTATION            , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , kCEETokenID_THROWS                , kJavaMethodDefinitionTranslateStateThrows                 );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , '('                               , kJavaMethodDefinitionTranslateStateAnnotationList         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , '['                               , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , ']'                               , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotation           , '{'                               , kJavaMethodDefinitionTranslateStateCommit                 );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationList       , ')'                               , kJavaMethodDefinitionTranslateStateAnnotationListEnd      );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , token_id_is_declaration_specifier , kJavaMethodDefinitionTranslateStateDeclarationSpecifier   );
    TRANSLATE_FUNCS_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , token_id_is_builtin_type          , kJavaMethodDefinitionTranslateStateBuiltinType            );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateIdentifier             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , '<'                               , kJavaMethodDefinitionTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , kCEETokenID_ANNOTATION            , kJavaMethodDefinitionTranslateStateAnnotation             );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , kCEETokenID_THROWS                , kJavaMethodDefinitionTranslateStateThrows                 );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , '['                               , kJavaMethodDefinitionTranslateStateAnnotationListEnd      );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , ']'                               , kJavaMethodDefinitionTranslateStateAnnotationListEnd      );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateAnnotationListEnd    , '{'                               , kJavaMethodDefinitionTranslateStateCommit                 );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateThrows               , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateException              );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateException            , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateException              );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateException            , '.'                               , kJavaMethodDefinitionTranslateStateExceptionSeparator     );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateException            , '<'                               , kJavaMethodDefinitionTranslateStateTypeParameters         );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateException            , ','                               , kJavaMethodDefinitionTranslateStateException              );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateException            , '{'                               , kJavaMethodDefinitionTranslateStateCommit                 );
    TRANSLATE_STATE_SET(java_method_definition_translate_table, kJavaMethodDefinitionTranslateStateExceptionSeparator   , kCEETokenID_IDENTIFIER            , kJavaMethodDefinitionTranslateStateException              );
}

static cee_boolean java_method_definition_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = FALSE;
    JavaMethodDefinitionTranslateState current = kJavaMethodDefinitionTranslateStateInit;
    JavaMethodDefinitionTranslateState prev = kJavaMethodDefinitionTranslateStateInit;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEESourceFregment* child = NULL;
    
    CEEList* identifier = NULL;
    CEEList* parameter_list = NULL;
    CEEList* throws = NULL;
    CEEList* commit = NULL;
        
    CEESourceSymbol* definition = NULL;
    
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_method_definition_translate_table[current][token->identifier];
        
        if (current == kJavaMethodDefinitionTranslateStateTypeParameters) {
            p = skip_type_parameter_list(p, FALSE);
            if (!p)
                break;
            current = prev;
        }
        else if (current == kJavaMethodDefinitionTranslateStateIdentifier) {
            if (cee_token_is_identifier(p, kCEETokenID_IDENTIFIER)) {
                if (!parameter_list)
                    identifier = p;
            }
        }
        else if (current == kJavaMethodDefinitionTranslateStateParameterList) {
            if (!parameter_list)
                parameter_list = p;
        }
        else if (current == kJavaMethodDefinitionTranslateStateThrows) {
            if (!throws)
                throws = p;
        }
        else if (current == kJavaMethodDefinitionTranslateStateCommit) {
            commit = p;
            ret = TRUE;
            break;
        }
        else if (current == kJavaMethodDefinitionTranslateStateError) {
            break;
        }
        
next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (current != kJavaMethodDefinitionTranslateStateCommit ||
        !identifier || !parameter_list)
        goto exit;
    
    definition = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                           fregment->subject_ref,
                                                           identifier, 
                                                           identifier,
                                                           kCEESourceSymbolTypeFunctionDefinition,
                                                           "java",
                                                           kCEETokenStringOptionCompact);
    
    if (definition) {
        cee_token_slice_state_mark(identifier,
                                   identifier,
                                   kCEETokenStateSymbolOccupied);
        cee_source_fregment_type_set_exclusive(fregment,
                                               kCEESourceFregmentTypeFunctionDefinition);
        
        child = cee_source_fregment_child_index_by_leaf(fregment, parameter_list->data);
        if (child) {
            child = SOURCE_FREGMENT_CHILDREN_FIRST(child)->data;
            java_method_parameters_parse(child);
        }
        
        definition->proto = java_method_definition_proto_descriptor_create(fregment,
                                                                           definition,
                                                                           child->symbols,
                                                                           identifier,
                                                                           throws,
                                                                           commit);
        fregment->symbols = cee_list_prepend(fregment->symbols, definition);
    }
    
#ifdef DEBUG_METHOD_DEFINITION
    cee_source_symbol_print(definition);
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
     *  Identifier              Error                       Error               Identifier          *TypeParameters     Identifier          Annotation      Commit      Dot     This        Error           Error               Identifier      Identifier
     *  Annotation              DeclarationSpecifier        BuiltinType         Identifier          *TypeParameters     Annotation          Annotation      Error       Error   Error       AnnotationList  Error               Error           Error
     *  AnnotationList          Error                       Error               Error               Error               Error               Error           Error       Error   Error       Error           AnnotationListEnd   Error           Error
     *  AnnotationListEnd       DeclarationSpecifier        BuiltinType         Identifier          *TypeParameters     AnnotationListEnd   Annotation      Error       Error   Error       Error           Error               Error           Error
     *  Dot                     Error                       Error               Identifier          Error               Error               Annotation      Error       Error   This        Error           Error               Error           Error
     *  This                    Error                       Error               Error               Error               Error               Error           Commit      Error   Error       Error           Error               Error           Error
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
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateIdentifier            , ','                               , kJavaMethodParametersDeclarationTranslateStateCommit                  );
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
    TRANSLATE_STATE_SET(java_method_parameters_declaration_translate_table, kJavaMethodParametersDeclarationTranslateStateThis                  , ','                               , kJavaMethodParametersDeclarationTranslateStateCommit                  );
}

static cee_boolean java_method_parameters_parse(CEESourceFregment* fregment)
{
    cee_boolean ret = TRUE;
    JavaMethodParametersDeclarationTranslateState current = kJavaMethodParametersDeclarationTranslateStateInit;
    JavaMethodParametersDeclarationTranslateState prev = kJavaMethodParametersDeclarationTranslateStateInit;
    CEESourceSymbol* declaration = NULL;
    CEEToken* token = NULL;
    CEEList* p = NULL;
    CEEList* head = NULL;
    CEEList* identifier = NULL;
        
    p = SOURCE_FREGMENT_TOKENS_FIRST(fregment);
    head = p;
    while (p) {
        token = p->data;
        
        if (cee_token_id_is_newline(token->identifier) ||
            cee_token_id_is_whitespace(token->identifier))
            goto next_token;
        
        prev = current;
        current = java_method_parameters_declaration_translate_table[current][token->identifier];
        
        if (current == kJavaMethodParametersDeclarationTranslateStateAnnotation){
            
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
        
        if (current == kJavaMethodParametersDeclarationTranslateStateCommit || !TOKEN_NEXT(p)) {

            if (identifier) {
                declaration = java_method_parameter_create(fregment,
                                                           head,
                                                           identifier,
                                                           identifier);
            }
            
            if (declaration)
                fregment->symbols = cee_list_append(fregment->symbols, declaration);
            
            declaration = NULL;
            identifier = NULL;
            
            head = TOKEN_NEXT(p);
            current = kJavaMethodParametersDeclarationTranslateStateInit;
        }
        
        if (current == kJavaMethodParametersDeclarationTranslateStateError) {
            ret = FALSE;
            break;
        }

next_token:
        p = TOKEN_NEXT(p);
    }
    
    if (fregment->symbols)
        cee_source_fregment_type_set(fregment, kCEESourceFregmentTypeDeclaration);
    
#ifdef DEBUG_METHOD_DEFINITION
    cee_source_symbols_print(fregment->symbols);
#endif
    
    return ret;
}

static CEESourceSymbol* java_method_parameter_create(CEESourceFregment* fregment,
                                                     CEEList* head,
                                                     CEEList* begin,
                                                     CEEList* end)
{
    CEESourceSymbol* parameter = cee_source_symbol_create_from_token_slice(fregment->filepath_ref,
                                                                           fregment->subject_ref,
                                                                           begin, 
                                                                           end,
                                                                           kCEESourceSymbolTypeFunctionParameter,
                                                                           "java",
                                                                           kCEETokenStringOptionCompact);
    
    if (!parameter)
        return NULL;
    
    cee_token_slice_state_mark(begin, end, kCEETokenStateSymbolOccupied);
    if (head && TOKEN_PREV(begin)) {
        cee_char* type = java_type_descriptor_from_token_slice(fregment,
                                                               head,
                                                               TOKEN_PREV(begin));
        parameter->proto = java_variable_proto_descriptor_create(fregment, 
                                                                 parameter,
                                                                 NULL,
                                                                 type);
        if (type)
            cee_free(type);
    }
    return parameter;
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

static cee_char* java_method_definition_proto_descriptor_create(CEESourceFregment* fregment,
                                                                CEESourceSymbol* definition,
                                                                CEEList* parameters,
                                                                CEEList* method,
                                                                CEEList* throws,
                                                                CEEList* commit)
{
    if (!fregment || !definition)
        return NULL;
    
    cee_boolean is_class_member = FALSE;
    CEEList* exception = NULL;
    CEEList* exceptions = NULL;
    cee_char* return_str = NULL;
    cee_char* exceptions_str = NULL;
    cee_char* descriptor = NULL;
    CEEList* p = NULL;
    CEEList* parent_symbols = NULL;
    CEESourceSymbol* parent_symbol = NULL;
    CEESourceFregment* grandfather_fregment = NULL;
    const cee_char* access_level = "public";
    const cee_char* subject = fregment->subject_ref;
    
    grandfather_fregment = cee_source_fregment_grandfather_get(fregment);
    if (method) {
        return_str = java_type_descriptor_from_token_slice(fregment, 
                                                           SOURCE_FREGMENT_TOKENS_FIRST(fregment), 
                                                           TOKEN_PREV(method));
        /** 
         * Constructor doesn't has return type declaraton, 
         * use its parent symbol type as return type
         */
        if (!return_str) {
            if (grandfather_fregment) {
                if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeClassDefinition))
                    parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather_fregment,
                                                                                kCEESourceSymbolTypeClassDefinition);
                else if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeInterfaceDefinition))
                    parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather_fregment, 
                                                                                kCEESourceSymbolTypeInterfaceDefinition);
                else if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeEnumDefinition)) {
                    parent_symbols = cee_source_fregment_symbols_search_by_type(grandfather_fregment, 
                                                                                kCEESourceSymbolTypeEnumDefinition);
                }
                
                if (parent_symbols) {
                    parent_symbol = cee_list_nth_data(parent_symbols, 0);
                    is_class_member = TRUE;
                }
            }
            
            if (parent_symbol)
                return_str = cee_strdup(parent_symbol->name);
        }
    }
    
    if (method)
        access_level = java_access_level_search(fregment,
                                                SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                TOKEN_PREV(method));
    
    if (!access_level) {
        if (grandfather_fregment) {
            if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeInterfaceDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeEnumDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
        
    if (throws && commit) {
        p = TOKEN_NEXT(throws);
        while (p) {
            CEEToken* token = p->data;
            if (token->identifier == kCEETokenID_IDENTIFIER ||
                token->identifier == '.') {
                TOKEN_APPEND(exception, token);
            }
            else if (token->identifier == '<') {
                p = skip_type_parameter_list(p, FALSE);
                if (!p)
                    break;
            }
            else if (token->identifier == ',') {
                if (exception) {
                    exceptions = cee_list_prepend(exceptions, exception);
                    exception = NULL;
                }
            }
            else if (p == commit) {
                if (exception) {
                    exceptions = cee_list_prepend(exceptions, exception);
                    exception = NULL;
                }
                break;
            }
            p = TOKEN_NEXT(p);
        }
    }
    if (exceptions)
        exceptions = cee_list_reverse(exceptions);
    
    p = exceptions;
    while (p) {
        cee_char* str = cee_string_from_tokens(subject, 
                                               TOKEN_FIRST(p->data), 
                                               kCEETokenStringOptionCompact);
        if (str) {
            cee_strconcat0(&exceptions_str, " ", str, NULL);
            cee_free(str);
        }
        
        p = p->next;
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    cee_strconcat0(&descriptor, "type:", "function_declaration", ",", NULL);
    cee_strconcat0(&descriptor, "return_type:", return_str, ",", NULL);
    cee_strconcat0(&descriptor, "name:", definition->name, ",", NULL);
    cee_strconcat0(&descriptor, "access_level:", access_level, ",", NULL);
    cee_strconcat0(&descriptor, "parameters:[", NULL);
    p = parameters;
    while (p) {
        CEESourceSymbol* parameter = p->data;
        cee_strconcat0(&descriptor, parameter->proto, NULL);
        
        if (p->next)
            cee_strconcat0(&descriptor, ",", NULL);
            
        p = p->next;
    }
    cee_strconcat0(&descriptor, "]", ",", NULL);
    cee_strconcat0(&descriptor, "exceptions:", exceptions_str, NULL);
    cee_strconcat0(&descriptor, "}", NULL);
    
    if (return_str)
        free(return_str);
    
    if (exceptions_str)
        free(exceptions_str);
        
    if (exception)
        cee_list_free(exception);
    
    if (exceptions)
        cee_list_free_full(exceptions, exception_free);
    
    return descriptor;
}

static cee_char* java_method_declaration_proto_descriptor_create(CEESourceFregment* fregment,
                                                                 CEESourceSymbol* definition,
                                                                 CEEList* parameters,
                                                                 CEEList* method,
                                                                 const cee_char* return_str,
                                                                 CEEList* throws)
{
    if (!fregment || !definition)
        return NULL;

    CEEList* exception = NULL;
    CEEList* exceptions = NULL;
    cee_char* exceptions_str = NULL;
    cee_char* descriptor = NULL;
    CEEList* p = NULL;
    CEESourceFregment* grandfather_fregment = NULL;
    const cee_char* subject = fregment->subject_ref;
    const cee_char* access_level = "public";
    
    if (method)
        access_level = java_access_level_search(fregment,
                                                SOURCE_FREGMENT_TOKENS_FIRST(fregment),
                                                TOKEN_PREV(method));
    
    if (!access_level) {
        grandfather_fregment = cee_source_fregment_grandfather_get(fregment);
        if (grandfather_fregment) {
            if (cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeClassDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeInterfaceDefinition) ||
                cee_source_fregment_type_is(grandfather_fregment, kCEESourceFregmentTypeEnumDefinition))
                access_level = "private";
        }
        if (!access_level)
            access_level = "public";
    }
    
    
    if (throws) {
        p = TOKEN_NEXT(throws);
        while (p) {
            CEEToken* token = p->data;
            
            if (token->identifier == kCEETokenID_IDENTIFIER ||
                token->identifier == '.') {
                TOKEN_APPEND(exception, token);
            }
            else if (token->identifier == ',' ||
                token->identifier == ';') {
                if (exception) {
                    exceptions = cee_list_prepend(exceptions, exception);
                    exception = NULL;
                }
            }
            else if (token->identifier == '<') {
                p = skip_type_parameter_list(p, FALSE);
                if (!p)
                    break;
            }
            
            p = TOKEN_NEXT(p);
        }
    }
    if (exceptions)
        exceptions = cee_list_reverse(exceptions);
    
    p = exceptions;
    while (p) {
        cee_char* str = cee_string_from_tokens(subject, 
                                               TOKEN_FIRST(p->data), 
                                               kCEETokenStringOptionCompact);
        if (str) {
            cee_strconcat0(&exceptions_str, " ", str, NULL);
            cee_free(str);
        }
        
        p = p->next;
    }
    
    cee_strconcat0(&descriptor, "{", NULL);
    cee_strconcat0(&descriptor, "type:", "function_declaration", ",", NULL);
    cee_strconcat0(&descriptor, "return_type:", return_str, ",", NULL);
    cee_strconcat0(&descriptor, "name:", definition->name, ",", NULL);
    cee_strconcat0(&descriptor, "access_level:", access_level, ",", NULL);
    cee_strconcat0(&descriptor, "parameters:[", NULL);
    p = parameters;
    while (p) {
        CEESourceSymbol* parameter = p->data;
        cee_strconcat0(&descriptor, parameter->proto, NULL);
        
        if (p->next)
            cee_strconcat0(&descriptor, ",", NULL);
        
        p = p->next;
    }
    cee_strconcat0(&descriptor, "]", ",", NULL);
    cee_strconcat0(&descriptor, "exceptions:", exceptions_str, NULL);
    cee_strconcat0(&descriptor, "}", NULL);
    
    if (exceptions_str)
        free(exceptions_str);
    
    if (exception)
        cee_list_free(exception);
    
    if (exceptions)
        cee_list_free_full(exceptions, exception_free);
    
    return descriptor;
}

static void exception_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_list_free((CEEList*)data);
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
