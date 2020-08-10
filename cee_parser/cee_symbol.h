#ifndef __CEE_SYMBOL_H__
#define __CEE_SYMBOL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"
#include "cee_symbol_cache.h"

typedef enum _CEESourceSymbolType {
    kCEESourceSymbolTypeUnknow = 0,
    kCEESourceSymbolTypePrepDirectiveInclude,
    kCEESourceSymbolTypePrepDirectiveDefine,
    kCEESourceSymbolTypePrepDirectiveParameter,
    kCEESourceSymbolTypePrepDirectiveCommon,
    kCEESourceSymbolTypeFunctionDeclaration,
    kCEESourceSymbolTypeFunctionDefinition,
    kCEESourceSymbolTypeLabel,
    kCEESourceSymbolTypeVariableDeclaration,
    kCEESourceSymbolTypeCustomTypeDeclaration,
    kCEESourceSymbolTypeProperty,
    kCEESourceSymbolTypeMessageDeclaration,
    kCEESourceSymbolTypeMessageDefinition,
    kCEESourceSymbolTypeFunctionParameter,
    kCEESourceSymbolTypeMessageParameter,
    kCEESourceSymbolTypeTypedef,
    kCEESourceSymbolTypeClassDefinition,
    kCEESourceSymbolTypeEnumDefinition,
    kCEESourceSymbolTypeUnionDefinition,
    kCEESourceSymbolTypeEnumerator,
    kCEESourceSymbolTypeNamespaceDefinition,
    kCEESourceSymbolTypeInterfaceDeclaration,
    kCEESourceSymbolTypeImplementationDefinition,
    kCEESourceSymbolTypeProtocolDeclaration,
    kCEESourceSymbolTypeTemplateDeclaration,
    kCEESourceSymbolTypeExternBlock,
    kCEESourceSymbolTypeMax,
} CEESourceSymbolType;

typedef struct _CEESourceSymbol {
    CEESourceSymbolType type;
    cee_char* descriptor;
    cee_char* language;
    cee_char* filepath;
    cee_char* parent;
    cee_char* derives;
    cee_char* protos;
    cee_char* locations;
    cee_char* fregment_range;
} CEESourceSymbol;

void cee_symbols_clean(cee_pointer db);
cee_boolean cee_symbols_write(cee_pointer db,
                              CEEList* symbols);
CEEList* cee_symbols_search_by_descriptor(cee_pointer db,
                                          const cee_char* descriptor,
                                          CEESymbolCacheRef cache);
CEEList* cee_symbols_search_by_parent(cee_pointer db,
                                      const cee_char* parent);
CEEList* cee_symbols_search_by_type(cee_pointer db,
                                    const cee_char* type);
CEEList* cee_symbols_search_by_filepath(cee_pointer db,
                                        const cee_char* filepath);
cee_boolean cee_symbols_delete_by_filepath(cee_pointer db,
                                           const cee_char* filepath);
void cee_symbol_free(cee_pointer data);
CEESourceSymbol* cee_symbol_create(CEESourceSymbolType type,
                                   const cee_char* descriptor,
                                   const cee_char* parent,
                                   const cee_char* derived,
                                   const cee_char* data_type,
                                   const cee_char* language,
                                   const cee_char* filepath,
                                   const cee_char* locations,
                                   const cee_char* block_range);
CEESourceSymbol* cee_symbol_copy(CEESourceSymbol* symbol);
void cee_symbol_dump(CEESourceSymbol* symbol);
void cee_symbols_dump(CEEList* symbols);
CEESourceSymbolType cee_symbol_database_search(cee_pointer database, 
                                               const cee_uchar* subject, 
                                               cee_long offset, 
                                               cee_ulong length);
CEESourceSymbol* cee_symbol_from_token_slice(const cee_uchar* filepath,
                                             const cee_uchar* subject,
                                             CEEList* begin,
                                             CEEList* end,
                                             CEESourceSymbolType type,
                                             const cee_char* language);
CEESourceSymbol* cee_symbol_from_tokens(const cee_uchar* filepath,
                                        const cee_uchar* subject,
                                        CEEList* tokens,
                                        CEESourceSymbolType type,
                                        const cee_char* language);
cee_int cee_symbol_location_compare(const cee_pointer a,
                                    const cee_pointer b);

typedef enum _CEESourceReferenceType {
    kCEESourceReferenceTypeUnknow = 0,
    kCEESourceReferenceTypeReplacement,
    kCEESourceReferenceTypeFunction,
    kCEESourceReferenceTypeCustomTypeDeclaration,
    kCEESourceReferenceTypeVariable,
    kCEESourceReferenceTypeMember,
    kCEESourceReferenceTypeTypeDefine,
    kCEESourceReferenceTypeEnumerator,
    kCEESourceReferenceTypeNamespace,
    kCEESourceReferenceTypeLabel,
    kCEESourceReferenceTypeMax,
} CEESourceReferenceType;

typedef enum _CEESourceReferenceSearchOption {
    kCEESourceReferenceSearchOptionLocal = 0x1 << 0,
    kCEESourceReferenceSearchOptionGlobal = 0x1 << 1
} CEESourceReferenceSearchOption;

typedef struct _CEESourceReference {
    CEESourceReferenceType type;
    CEEList* tokens_ref;
    cee_char* descriptor;
    CEEList* locations;
} CEESourceReference;

CEESourceReference* cee_reference_create(const cee_uchar* subject,
                                         CEEList* tokens,
                                         CEESourceReferenceType type);
void cee_reference_free(cee_pointer data);
cee_boolean cee_symbol_is_block_type(CEESourceSymbol* symbol);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SYMBOL_H__ */
