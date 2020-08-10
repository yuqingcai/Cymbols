#ifndef __CEE_TAG_H__
#define __CEE_TAG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cee_lib.h"

typedef enum _CEETagType {
    kCEETagTypePlanText = 0,
    
    /** C Family Language Tags  */
    kCEETagTypeCKeyword,
    kCEETagTypeCComment,
    kCEETagTypeCPPComment,
    kCEETagTypeCConstant,
    kCEETagTypeCCharacter,
    kCEETagTypeCLiteral,
    kCEETagTypeCPunctuation,
    kCEETagTypeCPrepDirective,
    kCEETagTypeCPrepFilename,
    kCEETagTypeCPrepDirectiveDefine,
    kCEETagTypeCPrepDirectiveDefineParameter,
    kCEETagTypeCNamespaceDeclaration,
    kCEETagTypeCNamespaceDefinition,
    kCEETagTypeCFunctionDeclaration,
    kCEETagTypeCFunctionDefinition,
    kCEETagTypeCCatchBlock,
    kCEETagTypeCVariable,
    kCEETagTypeCTypeDeclaration,
    kCEETagTypeOCProperty,
    kCEETagTypeCMessageDeclaration,
    kCEETagTypeCMessageDefinition,
    kCEETagTypeCFunctionParameter,
    kCEETagTypeCMessageParameter,
    kCEETagTypeCTypeDef,
    kCEETagTypeCClassDefinition,
    kCEETagTypeCEnumDefinition,
    kCEETagTypeCUnionDefinition,
    kCEETagTypeCEnumerator,
    kCEETagTypeOCInterface,
    kCEETagTypeOCImplementation,
    kCEETagTypeOCProtocol,
    kCEETagTypeCOperatorDeclaration,
    kCEETagTypeCOperatorDefinition,
    
    kCEETagTypeCTypedefReference,
    kCEETagTypeCTypeReference,
    kCEETagTypeCPrepDirectiveReference,
    kCEETagTypeCPrepDirectiveParameterReference,
    kCEETagTypeCVariableReference,
    kCEETagTypeCFunctionReference,
    kCEETagTypeCMessageReference,
    kCEETagTypeCMemberReference,
    
    
    /** xxx language tags  */
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
