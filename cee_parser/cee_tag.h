#ifndef __CEE_TAG_H__
#define __CEE_TAG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cee_lib.h"

typedef enum _CEETagType {
    kCEETagTypePlainText = 0,
    kCEETagTypeKeyword,
    kCEETagTypeComment,
    kCEETagTypeConstant,
    kCEETagTypeCharacter,
    kCEETagTypeLiteral,
    kCEETagTypePunctuation,
    kCEETagTypePrepDirective,
    kCEETagTypePrepFilename,
    kCEETagTypePrepDirectiveDefine,
    kCEETagTypePrepDirectiveDefineParameter,
    kCEETagTypeNamespaceDeclaration,
    kCEETagTypeNamespaceDefinition,
    kCEETagTypeFunctionDeclaration,
    kCEETagTypeFunctionDefinition,
    kCEETagTypeCatchBlock,
    kCEETagTypeVariable,
    kCEETagTypeTypeDeclaration,
    kCEETagTypeProperty,
    kCEETagTypeMessageDeclaration,
    kCEETagTypeMessageDefinition,
    kCEETagTypeFunctionParameter,
    kCEETagTypeMessageParameter,
    kCEETagTypeTypeDefine,
    kCEETagTypeClassDefinition,
    kCEETagTypeEnumDefinition,
    kCEETagTypeUnionDefinition,
    kCEETagTypeEnumerator,
    kCEETagTypeInterface,
    kCEETagTypeImplementation,
    kCEETagTypeProtocol,
    kCEETagTypeOperatorDeclaration,
    kCEETagTypeOperatorDefinition,
    kCEETagTypeTypeDefineReference,
    kCEETagTypeTypeReference,
    kCEETagTypePrepDirectiveReference,
    kCEETagTypePrepDirectiveParameterReference,
    kCEETagTypeVariableReference,
    kCEETagTypeFunctionReference,
    kCEETagTypeMessageReference,
    kCEETagTypeMemberReference,
    kCEETagTypeEnumeratorReference,
    kCEETagTypeNamespaceReference,
    kCEETagTypeLabelReference,
    
    /** HTML & XML */
    kCEETagTypeXMLTag,
    kCEETagTypeXMLAttribute,
    
    /** CSS */
    kCEETagTypeCSSSelector,
    kCEETagTypeCSSProperty,
    
    /** ... */
    kCEETagTypeIgnore,
    kCEETagTypeMax,
} CEETagType;

typedef struct _CEETag CEETag;
    
typedef struct _CEETag {
    CEETagType type;
    CEERange range;
} CEETag;

void cee_tag_free(cee_pointer data);
CEETag* cee_tag_create(CEETagType type,
                       CEERange range);
cee_int cee_tag_compare(const cee_pointer a,
                        const cee_pointer b);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TAG_H__ */
