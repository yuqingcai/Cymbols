#ifndef __CEE_SYMBOL_H__
#define __CEE_SYMBOL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"

typedef enum _CEESourceSymbolType {
    kCEESourceSymbolTypeUnknow = 0,
    kCEESourceSymbolTypePrepDirectiveInclude,
    kCEESourceSymbolTypePrepDirectiveIncludePath,
    kCEESourceSymbolTypePrepDirectiveDefine,
    kCEESourceSymbolTypePrepDirectiveParameter,
    kCEESourceSymbolTypePrepDirectiveCommon,
    kCEESourceSymbolTypeFunctionDeclaration,
    kCEESourceSymbolTypeFunctionDefinition,
    kCEESourceSymbolTypeLabel,
    kCEESourceSymbolTypeVariableDeclaration,
    kCEESourceSymbolTypeCustomTypeDeclaration,
    kCEESourceSymbolTypePropertyDeclaration,
    kCEESourceSymbolTypeMessageDeclaration,
    kCEESourceSymbolTypeMessageDefinition,
    kCEESourceSymbolTypeFunctionParameter,
    kCEESourceSymbolTypeMessageParameter,
    kCEESourceSymbolTypeTypeDefine,
    kCEESourceSymbolTypeStructDefinition,
    kCEESourceSymbolTypeClassDefinition,
    kCEESourceSymbolTypeEnumDefinition,
    kCEESourceSymbolTypeUnionDefinition,
    kCEESourceSymbolTypeEnumerator,
    kCEESourceSymbolTypeNamespaceDefinition,
    kCEESourceSymbolTypeInterfaceDeclaration,
    kCEESourceSymbolTypeInterfaceDefinition,
    kCEESourceSymbolTypeImplementationDefinition,
    kCEESourceSymbolTypeProtocolDeclaration,
    kCEESourceSymbolTypeTemplateDeclaration,
    kCEESourceSymbolTypeExternBlock,
    kCEESourceSymbolTypeXMLTagStart,
    kCEESourceSymbolTypeXMLTagEnd,
    kCEESourceSymbolTypeXMLTagName,
    kCEESourceSymbolTypeXMLTagAttribute,
    kCEESourceSymbolTypeCSSSelector,
    kCEESourceSymbolTypeImport,
    kCEESourceSymbolTypeUsingDeclaration,
    kCEESourceSymbolTypePackage,
    kCEESourceSymbolTypeASMDirective,
    kCEESourceSymbolTypeOperatorOverloadDefinition,
    kCEESourceSymbolTypeOperatorOverloadDeclaration,
    
    /** ... */
    kCEESourceSymbolTypeMax,
} CEESourceSymbolType;

typedef struct _CEESourceSymbol {
    CEESourceSymbolType type;
    cee_char* name;
    cee_char* alias;
    cee_char* language;
    cee_char* filepath;
    cee_char* parent;
    cee_char* derives;
    cee_char* proto;
    cee_char* locations;
    cee_char* fregment_range;
} CEESourceSymbol;

CEESourceSymbol* cee_source_symbol_create(CEESourceSymbolType type,
                                          const cee_char* name,
                                          const cee_char* alias,
                                          const cee_char* parent,
                                          const cee_char* derived,
                                          const cee_char* data_type,
                                          const cee_char* language,
                                          const cee_char* filepath,
                                          const cee_char* locations,
                                          const cee_char* block_range);
void cee_source_symbol_free(cee_pointer data);
cee_int cee_source_symbol_count_get(void);
CEESourceSymbol* cee_source_symbol_copy(CEESourceSymbol* symbol);
CEESourceSymbol* cee_source_symbol_create_from_token_slice(const cee_char* filepath,
                                                           const cee_char* subject,
                                                           CEEList* begin,
                                                           CEEList* end,
                                                           CEESourceSymbolType type,
                                                           const cee_char* language);
CEESourceSymbol* cee_source_symbol_create_from_tokens(const cee_char* filepath,
                                                      const cee_char* subject,
                                                      CEEList* tokens,
                                                      CEESourceSymbolType type,
                                                      const cee_char* language);
void cee_source_symbol_name_format(cee_char* name);
void cee_source_symbol_print(CEESourceSymbol* symbol);
void cee_source_symbols_print(CEEList* symbols);
cee_boolean cee_source_symbol_is_block_type(CEESourceSymbol* symbol);
cee_int cee_source_symbol_location_compare(const cee_pointer a,
                                           const cee_pointer b);
cee_boolean cee_source_symbols_are_equal(CEESourceSymbol* symbol0,
                                         CEESourceSymbol* symbol1);
typedef cee_boolean (*CEESymbolMatcher)(cee_pointer data,
                                        cee_pointer user_data);
CEESourceSymbol* cee_source_symbol_tree_search(CEETree* tree,
                                               CEESymbolMatcher searcher,
                                               cee_pointer user_data);
cee_boolean cee_source_symbol_matcher_by_buffer_offset(cee_pointer data,
                                                       cee_pointer user_data);
cee_boolean cee_source_symbol_matcher_by_name(cee_pointer data,
                                              cee_pointer user_data);
cee_boolean cee_source_symbol_is_definition(CEESourceSymbol* symbol);

typedef struct _CEESourceSymbolWrapper {
    CEESourceSymbol* symbol_ref;
    cee_int level;
} CEESourceSymbolWrapper;

CEESourceSymbolWrapper* cee_source_symbol_wrapper_create(CEESourceSymbol* symbol,
                                                         cee_uint level);
void cee_source_symbol_wrapper_free(cee_pointer data);
void cee_source_symbol_tree_dump_to_wrappers(CEETree* tree, 
                                             CEEList** list);
cee_int cee_source_symbol_wrapper_location_compare(const cee_pointer a,
                                                   const cee_pointer b);
CEEList* cee_source_symbol_wrappers_copy_with_condition(CEEList* wrappers,
                                                        const cee_char* conditoin);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SYMBOL_H__ */
