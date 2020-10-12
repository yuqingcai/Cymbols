#ifndef __CEE_REFERENCE_H__
#define __CEE_REFERENCE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"

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

typedef struct _CEESourceSymbolReference {
    CEESourceReferenceType type;
    cee_char* filepath;
    CEEList* tokens_ref;
    cee_char* name;
    cee_char* locations;
} CEESourceSymbolReference;

CEESourceSymbolReference* cee_source_symbol_reference_create(const cee_char* filepath,
                                                             const cee_uchar* subject,
                                                             CEEList* tokens,
                                                             CEESourceReferenceType type);
void cee_source_symbol_reference_free(cee_pointer data);
void cee_source_symbol_reference_print(CEESourceSymbolReference* reference);
void cee_source_symbol_references_print(CEEList* references);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_REFERENCE_H__ */
