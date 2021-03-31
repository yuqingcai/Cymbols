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
    cee_char* file_path;
    cee_char* parent;
    cee_char* derives;
    CEEList* ranges;
    cee_int line_no;
    CEERange fregment_range;
    cee_char* proto_descriptor;
} CEESourceSymbol;

CEESourceSymbol* cee_source_symbol_create(CEESourceSymbolType type,
                                          const cee_char* name,
                                          const cee_char* alias,
                                          const cee_char* parent,
                                          const cee_char* derived,
                                          const cee_char* proto_descriptor,
                                          const cee_char* language,
                                          const cee_char* file_path,
                                          cee_int line_no,
                                          CEEList* ranges,
                                          CEERange fregment_range);
void cee_source_symbol_free(cee_pointer data);
cee_int cee_source_symbol_count_get(void);
CEESourceSymbol* cee_source_symbol_copy(CEESourceSymbol* symbol);
cee_pointer cee_source_symbol_copy_func(const cee_pointer src,
                                        cee_pointer data);
CEESourceSymbol* cee_source_symbol_create_from_token_slice(const cee_char* file_path,
                                                           const cee_char* subject,
                                                           CEEList* begin,
                                                           CEEList* end,
                                                           CEESourceSymbolType type,
                                                           const cee_char* language,
                                                           CEETokenStringOption option);
CEESourceSymbol* cee_source_symbol_create_from_tokens(const cee_char* file_path,
                                                      const cee_char* subject,
                                                      CEEList* tokens,
                                                      CEESourceSymbolType type,
                                                      const cee_char* language,
                                                      CEETokenStringOption option);
void cee_source_symbol_name_format(cee_char* name);
void cee_source_symbol_print(CEESourceSymbol* symbol);
void cee_source_symbols_print(CEEList* symbols);
cee_boolean cee_source_symbol_is_block_type(CEESourceSymbol* symbol);
cee_int cee_source_symbol_location_compare(const cee_pointer a,
                                           const cee_pointer b);
cee_boolean cee_source_symbols_are_equal(CEESourceSymbol* symbol0,
                                         CEESourceSymbol* symbol1);
CEESourceSymbol* cee_source_symbol_tree_search(CEETree* tree,
                                               CEECompareFunc searcher,
                                               cee_pointer user_data);
cee_int cee_source_symbol_matcher_by_buffer_offset(cee_pointer data,
                                                   cee_pointer user_data);
cee_int cee_source_symbol_matcher_by_name(cee_pointer data,
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

typedef struct _CEESourceSymbolCache {
    CEESourceSymbol* symbol;
    CEEList* referened_locations;
} CEESourceSymbolCache;

CEESourceSymbolCache* cee_source_symbol_cache_create(CEESourceSymbol* symbol,
                                                     cee_long referenced_location);
void cee_source_symbol_cache_free(cee_pointer);
CEESourceSymbolCache* cee_source_symbol_cached(CEEList* caches,
                                               CEESourceSymbol* symbol);
void cee_source_symbol_cache_location(CEESourceSymbolCache* cache,
                                      cee_long referenced_location);

cee_boolean cee_source_symbol_cache_name_hit(CEESourceSymbolCache* cache,
                                             const cee_char* name);
cee_boolean cee_source_symbol_cache_location_hit(CEESourceSymbolCache* cache,
                                                 cee_long location);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SYMBOL_H__ */
