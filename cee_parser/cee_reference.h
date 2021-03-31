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
    kCEESourceReferenceTypeIgnore ,
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

typedef struct _CEESourceSymbolReference {
    CEESourceReferenceType type;
    cee_char* file_path;
    CEEList* tokens_ref;
    cee_char* name;
    CEEList* ranges;
    cee_pointer parser_ref;
} CEESourceSymbolReference;

CEESourceSymbolReference* cee_source_symbol_reference_create(cee_pointer parser_ref,
                                                             const cee_char* file_path,
                                                             const cee_char* subject,
                                                             CEEList* tokens,
                                                             CEESourceReferenceType type);
void cee_source_symbol_reference_free(cee_pointer data);
void cee_source_symbol_reference_print(CEESourceSymbolReference* reference);
void cee_source_symbol_references_print(CEEList* references);
cee_int cee_source_symbol_reference_location_compare(const cee_pointer a,
                                                     const cee_pointer b);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_REFERENCE_H__ */
