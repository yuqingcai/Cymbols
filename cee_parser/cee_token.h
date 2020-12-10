#ifndef __CEE_TOKEN_H__
#define __CEE_TOKEN_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"

#define BRACKET_SIGN_DECLARATION() cee_long _round = 0, _angle = 0, _square = 0, _brace = 0;
#define CLEAR_BRACKETS() (_round) = (_angle) = (_square) = 0, _brace = 0;
#define SIGN_BRACKET(identifier) {\
    if ((identifier) == '(') _round ++;\
    else if ((identifier) == ')') (_round) --;\
    else if ((identifier) == '<') (_angle) ++;\
    else if ((identifier) == '>') (_angle) --;\
    else if ((identifier) == '[' || (identifier) == kCEETokenID_AT_BRACKET) (_square) ++;\
    else if ((identifier) == ']') (_square) --;\
    else if ((identifier) == '{' || (identifier) == kCEETokenID_AT_BRACE || (identifier) == kCEETokenID_CARAT_BRACE) (_brace) ++;\
    else if ((identifier) == '}') (_brace) --;\
}

#define BRACKETS_IS_CLEAN() (!(_round) && !(_angle) && !(_square))

#define TOKEN_FIRST(p) cee_list_last((p))
#define TOKEN_LAST(p) cee_list_first((p))
#define TOKEN_NEXT(p) (p)->prev
#define TOKEN_PREV(p) (p)->next
#define TOKEN_PREV_SET(p, node) (p)->next = (node)
#define TOKEN_NEXT_SET(p, node) (p)->prev = (node)
#define TOKEN_APPEND(tokens, token) (tokens) = cee_list_prepend((tokens), (token))

typedef enum _CEETokenType {
    kCEETokenTypeComment                  = 0x1 << 0,
    kCEETokenTypeIdentifier               = 0x1 << 1,
    kCEETokenTypeConstant                 = 0x1 << 2,
    kCEETokenTypeCharacter                = 0x1 << 3,
    kCEETokenTypeLiteral                  = 0x1 << 4,
    kCEETokenTypePrepDirective            = 0x1 << 5,
    kCEETokenTypePunctuation              = 0x1 << 6,
    kCEETokenTypeKeyword                  = 0x1 << 7,
    kCEETokenTypeAssignment               = 0x1 << 8,
    kCEETokenTypeBuildinType              = 0x1 << 9,
    kCEETokenTypeDeclarationSpecifier     = 0x1 << 10,
    kCEETokenTypeAccessSpecifier          = 0x1 << 11,
    kCEETokenTypeObjectSpecifier          = 0x1 << 12,
    kCEETokenTypePrepDirectiveCondition   = 0x1 << 13,
    kCEETokenTypeVirtualSpecifier         = 0x1 << 14,
    kCEETokenTypeAlignasSpecifier         = 0x1 << 15,
    kCEETokenTypeOverloadOperator         = 0x1 << 16,
    kCEETokenTypeASMDirective             = 0x1 << 17,
    kCEETokenTypeBuildinReference         = 0x1 << 18,
    kCEETokenTypeParameterSpecifier       = 0x1 << 19,
} CEETokenType;

typedef enum _CEETokenID {
    kCEETokenID_UNKNOW = 512,
    kCEETokenID_IGNORE,
    kCEETokenID_NEW_LINE,
    kCEETokenID_WHITE_SPACE,
    
    kCEETokenID_LINES_COMMENT,
    kCEETokenID_LINE_COMMENT,
    kCEETokenID_IDENTIFIER,
    kCEETokenID_CONSTANT,
    kCEETokenID_CHARACTER,
    kCEETokenID_LITERAL,
    kCEETokenID_MULTI_LITERAL,
    kCEETokenID_HASH_INCLUDE,
    kCEETokenID_HASH_IMPORT,
    kCEETokenID_HASH_DEFINE,
    kCEETokenID_HASH_UNDEF,
    kCEETokenID_HASH_IF,
    kCEETokenID_HASH_IFDEF,
    kCEETokenID_HASH_IFNDEF,
    kCEETokenID_HASH_ENDIF,
    kCEETokenID_HASH_ELSE,
    kCEETokenID_HASH_ELIF,
    kCEETokenID_HASH_LINE,
    kCEETokenID_HASH_ERROR,
    kCEETokenID_HASH_WARNING,
    kCEETokenID_HASH_PRAGMA,
    kCEETokenID_HASH_UNKNOW,
    kCEETokenID_ADD_ASSIGNMENT,
    kCEETokenID_MINUS_ASSIGNMENT,
    kCEETokenID_MULTI_ASSIGNMENT,
    kCEETokenID_DIV_ASSIGNMENT,
    kCEETokenID_MOD_ASSIGNMENT,
    kCEETokenID_AND_ASSIGNMENT,
    kCEETokenID_OR_ASSIGNMENT,
    kCEETokenID_XOR_ASSIGNMENT,
    kCEETokenID_LEFT_OFFSET_ASSIGNMENT,
    kCEETokenID_RIGHT_OFFSET_ASSIGNMENT,
    kCEETokenID_INCREMENT,
    kCEETokenID_DECREMENT,
    kCEETokenID_LOGIC_AND,
    kCEETokenID_LOGIC_OR,
    kCEETokenID_LOGIC_EQUAL,
    kCEETokenID_LOGIC_UNEQUAL,
    kCEETokenID_LOGIC_LESS_EQUAL,
    kCEETokenID_LOGIC_LARGE_EQUAL,
    kCEETokenID_MEMBER_POINTER,
    kCEETokenID_COMPARISON,
    kCEETokenID_POINTER_MEMBER_POINTER,
    kCEETokenID_TOKEN_PASTING,
    kCEETokenID_ELLIPSIS,
    kCEETokenID_SCOPE,
    kCEETokenID_AT_BRACE,
    kCEETokenID_AT_BRACKET,
    kCEETokenID_CARAT_BRACE,
    kCEETokenID_ALIGNAS,
    kCEETokenID_ALIGNOF,
    kCEETokenID_AND,
    kCEETokenID_AND_EQ,
    kCEETokenID_ASM,
    kCEETokenID_ATOMIC_CANCEL,
    kCEETokenID_ATOMIC_COMMIT,
    kCEETokenID_ATOMIC_NOEXCEPT,
    kCEETokenID_AUTO,
    kCEETokenID_BITAND,
    kCEETokenID_BITOR,
    kCEETokenID_BOOL,
    kCEETokenID_BREAK,
    kCEETokenID_CASE,
    kCEETokenID_CATCH,
    kCEETokenID_CHAR,
    kCEETokenID_CHAR16_T,
    kCEETokenID_CHAR32_T,
    kCEETokenID_CLASS,
    kCEETokenID_COMPL,
    kCEETokenID_CONCEPT,
    kCEETokenID_CONST,
    kCEETokenID_CONSTEVAL,
    kCEETokenID_CONSTEXPR,
    kCEETokenID_CONST_CAST,
    kCEETokenID_CONTINUE,
    kCEETokenID_DECLTYPE,
    kCEETokenID_DEFAULT,
    kCEETokenID_DELETE,
    kCEETokenID_DO,
    kCEETokenID_DOUBLE,
    kCEETokenID_DYNAMIC_CAST,
    kCEETokenID_ELSE,
    kCEETokenID_ENUM,
    kCEETokenID_EXPLICIT,
    kCEETokenID_EXPORT,
    kCEETokenID_EXTERN,
    kCEETokenID_FALSE,
    kCEETokenID_FLOAT,
    kCEETokenID_FOR,
    kCEETokenID_FRIEND,
    kCEETokenID_GOTO,
    kCEETokenID_IF,
    kCEETokenID_IMPORT,
    kCEETokenID_INLINE,
    kCEETokenID_INT,
    kCEETokenID_LONG,
    kCEETokenID_MODULE,
    kCEETokenID_MUTABLE,
    kCEETokenID_NAMESPACE,
    kCEETokenID_NEW,
    kCEETokenID_NOEXCEPT,
    kCEETokenID_NOT,
    kCEETokenID_NOT_EQ,
    kCEETokenID_NULLPTR,
    kCEETokenID_OPERATOR,
    kCEETokenID_OR,
    kCEETokenID_OR_EQ,
    kCEETokenID_PRIVATE,
    kCEETokenID_PROTECTED,
    kCEETokenID_PUBLIC,
    kCEETokenID_REGISTER,
    kCEETokenID_REINTERPRET_CAST,
    kCEETokenID_REQUIRES,
    kCEETokenID_RESTRICT,
    kCEETokenID_RETURN,
    kCEETokenID_SHORT,
    kCEETokenID_SIGNED,
    kCEETokenID_SIZEOF,
    kCEETokenID_STATIC,
    kCEETokenID_STATIC_ASSERT,
    kCEETokenID_STATIC_CAST,
    kCEETokenID_STRUCT,
    kCEETokenID_SWITCH,
    kCEETokenID_SYNCHRONIZED,
    kCEETokenID_TEMPLATE,
    kCEETokenID_THIS,
    kCEETokenID_THREAD_LOCAL,
    kCEETokenID_THROW,
    kCEETokenID_TRUE,
    kCEETokenID_TRY,
    kCEETokenID_TYPEDEF,
    kCEETokenID_TYPEID,
    kCEETokenID_TYPENAME,
    kCEETokenID_UNION,
    kCEETokenID_UNSIGNED,
    kCEETokenID_USING,
    kCEETokenID_VIRTUAL,
    kCEETokenID_VOLATILE,
    kCEETokenID_WCHAR_T,
    kCEETokenID_WHILE,
    kCEETokenID_XOR,
    kCEETokenID_XOR_EQ,
    kCEETokenID_OVERRIDE,
    kCEETokenID_FINAL,
    kCEETokenID_TRANSACTION_SAFE,
    kCEETokenID_TRANSACTION_SAFE_DYNAMIC,
    kCEETokenID__ALIGNAS,
    kCEETokenID__ALIGNOF,
    kCEETokenID__ATOMIC,
    kCEETokenID__BOOL,
    kCEETokenID__COMPLEX,
    kCEETokenID__GENERIC,
    kCEETokenID__IMAGINARY,
    kCEETokenID__NORETURN,
    kCEETokenID__STATIC_ASSERT,
    kCEETokenID__THREAD_LOCAL,
    kCEETokenID_VOID,
    kCEETokenID_LONG_DOUBLE,
    kCEETokenID_SIGNED_CHAR,
    kCEETokenID_UNSIGNED_CHAR,
    kCEETokenID_SHORT_INT,
    kCEETokenID_SIGNED_SHORT,
    kCEETokenID_SIGNED_SHORT_INT,
    kCEETokenID_UNSIGNED_SHORT,
    kCEETokenID_UNSIGNED_SHORT_INT,
    kCEETokenID_SIGNED_INT,
    kCEETokenID_UNSIGNED_INT,
    kCEETokenID_LONG_INT,
    kCEETokenID_SIGNED_LONG,
    kCEETokenID_SIGNED_LONG_INT,
    kCEETokenID_UNSIGNED_LONG,
    kCEETokenID_UNSIGNED_LONG_INT,
    kCEETokenID_LONG_LONG,
    kCEETokenID_LONG_LONG_INT,
    kCEETokenID_SIGNED_LONG_LONG,
    kCEETokenID_SIGNED_LONG_LONG_INT,
    kCEETokenID_UNSIGNED_LONG_LONG,
    kCEETokenID_UNSIGNED_LONG_LONG_INT,
    kCEETokenID_INT8_T,
    kCEETokenID_INT16_T,
    kCEETokenID_INT32_T,
    kCEETokenID_INT64_T,
    kCEETokenID_INT_FAST8_T,
    kCEETokenID_INT_FAST16_T,
    kCEETokenID_INT_FAST32_T,
    kCEETokenID_INT_FAST64_T,
    kCEETokenID_INT_LEAST8_T,
    kCEETokenID_INT_LEAST16_T,
    kCEETokenID_INT_LEAST32_T,
    kCEETokenID_INT_LEAST64_T,
    kCEETokenID_INTMAX_T,
    kCEETokenID_INTPTR_T,
    kCEETokenID_UINT8_T,
    kCEETokenID_UINT16_T,
    kCEETokenID_UINT32_T,
    kCEETokenID_UINT64_T,
    kCEETokenID_UINT_FAST8_T,
    kCEETokenID_UINT_FAST16_T,
    kCEETokenID_UINT_FAST32_T,
    kCEETokenID_UINT_FAST64_T,
    kCEETokenID_UINT_LEAST8_T,
    kCEETokenID_UINT_LEAST16_T,
    kCEETokenID_UINT_LEAST32_T,
    kCEETokenID_UINT_LEAST64_T,
    kCEETokenID_UINTMAX_T,
    kCEETokenID_UINTPTR_T,
    kCEETokenID_AT_INTERFACE,
    kCEETokenID_AT_IMPLEMENTATION,
    kCEETokenID_AT_PROTOCOL,
    kCEETokenID_AT_END,
    kCEETokenID_AT_CLASS,
    kCEETokenID_AT_PUBLIC,
    kCEETokenID_AT_PACKAGE,
    kCEETokenID_AT_PROTECTED,
    kCEETokenID_AT_PRIVATE,
    kCEETokenID_AT_PROPERTY,
    kCEETokenID_AT_SYNTHESIZE,
    kCEETokenID_AT_DYNAMIC,
    kCEETokenID_AT_REQUIRED,
    kCEETokenID_AT_OPTIONAL,
    kCEETokenID_AT_TRY,
    kCEETokenID_AT_CATCH,
    kCEETokenID_AT_FINALLY,
    kCEETokenID_AT_THROW,
    kCEETokenID_AT_SELECTOR,
    kCEETokenID_AT_ENCODE,
    kCEETokenID_AT_DEFS,
    kCEETokenID_AT_AUTORELEASEPOOL,
    kCEETokenID_AT_SYNCHRONIZED,
    kCEETokenID_AT_COMPATIBILITY_ALIAS,
    kCEETokenID_ID,
    kCEETokenID_INSTANCETYPE,
    kCEETokenID_IMP,
    kCEETokenID_IN,
    kCEETokenID_INOUT,
    kCEETokenID_NIL,
    kCEETokenID_ONEWAY,
    kCEETokenID_OUT,
    kCEETokenID_SEL,
    kCEETokenID_SELF,
    kCEETokenID_SUPER,
    kCEETokenID_YES,
    kCEETokenID_NO,
    kCEETokenID_ATOMIC,
    kCEETokenID_NONATOMIC,
    kCEETokenID_STRONG,
    kCEETokenID_WEAK,
    kCEETokenID_ASSIGN,
    kCEETokenID_COPY,
    kCEETokenID_RETAIN,
    kCEETokenID_READONLY,
    kCEETokenID_READWRITE,
    kCEETokenID___BRIDGE,
    kCEETokenID___BRIDGE_TRANSFER,
    kCEETokenID___BRIDGE_RETAINED,
    kCEETokenID___BLOCK,
    kCEETokenID__NONNULL,
    kCEETokenID__NULLABLE,
    kCEETokenID_NONNULL,
    kCEETokenID_NULLABLE,
    kCEETokenID___KIND_OF,
    kCEETokenID_NS_ENUM,
    kCEETokenID_NS_OPTIONS,
    kCEETokenID_HTML_COMMENT,
    kCEETokenID_HTML_DOCTYPE,
    kCEETokenID_CSS_RULE,
    kCEETokenID_CSS_PROPERTY,
    kCEETokenID_CSS_FUNCTION,
    kCEETokenID_ASSOCIATEDTYPE,
    kCEETokenID_DEINIT,
    kCEETokenID_EXTENSION,
    kCEETokenID_FILEPRIVATE,
    kCEETokenID_FUNC,
    kCEETokenID_INIT,
    kCEETokenID_INTERNAL,
    kCEETokenID_LET,
    kCEETokenID_OPEN,
    kCEETokenID_PROTOCOL,
    kCEETokenID_RETHROWS,
    kCEETokenID_SUBSCRIPT,
    kCEETokenID_TYPEALIAS,
    kCEETokenID_VAR,
    kCEETokenID_DEFER,
    kCEETokenID_FALLTHROUGH,
    kCEETokenID_GUARD,
    kCEETokenID_REPEAT,
    kCEETokenID_WHERE,
    kCEETokenID_AS,
    kCEETokenID_ANY,
    kCEETokenID_IS,
    kCEETokenID_CAPITAL_SELF,
    kCEETokenID_THROWS,
    kCEETokenID_UNDERLINE_DOT,
    kCEETokenID_HASH_AVAILABLE,
    kCEETokenID_HASH_COLORLITERAL,
    kCEETokenID_HASH_COLUMN,
    kCEETokenID_HASH_ELSEIF,
    kCEETokenID_HASH_FILE,
    kCEETokenID_HASH_FILEID,
    kCEETokenID_HASH_FILELITERAL,
    kCEETokenID_HASH_FILEPATH,
    kCEETokenID_HASH_FUNCTION,
    kCEETokenID_HASH_IMAGELITERAL,
    kCEETokenID_HASH_SELECTOR,
    kCEETokenID_HASH_SOURCELOCATION,
    kCEETokenID_ASSOCIATIVITY,
    kCEETokenID_CONVENIENCE,
    kCEETokenID_DYNAMIC,
    kCEETokenID_DIDSET,
    kCEETokenID_GET,
    kCEETokenID_INFIX,
    kCEETokenID_INDIRECT,
    kCEETokenID_LAZY,
    kCEETokenID_LEFT,
    kCEETokenID_MUTATING,
    kCEETokenID_NONE,
    kCEETokenID_NONMUTATING,
    kCEETokenID_OPTIONAL,
    kCEETokenID_POSTFIX,
    kCEETokenID_PRECEDENCE,
    kCEETokenID_PREFIX,
    kCEETokenID_CAPITAL_PROTOCOL,
    kCEETokenID_REQUIRED,
    kCEETokenID_RIGHT,
    kCEETokenID_SET,
    kCEETokenID_CAPITAL_TYPE,
    kCEETokenID_UNOWNED,
    kCEETokenID_WILLSET,
    kCEETokenID_AT_IDENTIFIER,
    kCEETokenID_AND_MULTI,
    kCEETokenID_AND_DIV,
    kCEETokenID_AND_MOD,
    kCEETokenID_AND_ADD,
    kCEETokenID_AND_MINUS,
    kCEETokenID_HALF_OPEN_RANGE,
    kCEETokenID_LOGIC_IDENTICALLY_EQUAL,
    kCEETokenID_LOGIC_IDENTICALLY_UNEQUAL,
    kCEETokenID_PATTERN_MATCH,
    kCEETokenID_NIL_COALESCING,
    kCEETokenID_LEFT_SHIFT_ASSIGNMENT,
    kCEETokenID_RIGHT_SHIFT_ASSIGNMENT,
    
    kCEETokenID_ABSTRACT,
    kCEETokenID_ASSERT,
    kCEETokenID_BOOLEAN,
    kCEETokenID_BYTE,
    kCEETokenID_EXPORTS,
    kCEETokenID_EXTENDS,
    kCEETokenID_FINALLY,
    kCEETokenID_IMPLEMENTS,
    kCEETokenID_INSTANCEOF,
    kCEETokenID_INTERFACE,
    kCEETokenID_NATIVE,
    kCEETokenID_PACKAGE,
    kCEETokenID_STRICTFP,
    kCEETokenID_TRANSIENT,
    kCEETokenID_ANNOTATION,
    kCEETokenID_LABEL,
    
    kCEETokenID_BASE,
    kCEETokenID_CHECKED,
    kCEETokenID_DECIMAL,
    kCEETokenID_DELEGATE,
    kCEETokenID_EVENT,
    kCEETokenID_FIXED,
    kCEETokenID_FOREACH,
    kCEETokenID_IMPLICIT,
    kCEETokenID_LOCK,
    kCEETokenID_NULL,
    kCEETokenID_OBJECT,
    kCEETokenID_PARAMS,
    kCEETokenID_REF,
    kCEETokenID_SBYTE,
    kCEETokenID_SEALED,
    kCEETokenID_STACKALLOC,
    kCEETokenID_STRING,
    kCEETokenID_TYPEOF,
    kCEETokenID_UINT,
    kCEETokenID_ULONG,
    kCEETokenID_UNCHECKED,
    kCEETokenID_UNSAFE,
    kCEETokenID_USHORT,
    kCEETokenID_LAMBDA,
    kCEETokenID_NULL_COALESCING,
    kCEETokenID_HASH_REGION,
    kCEETokenID_HASH_ENDREGION,
    
    kCEETokenID_ADD,
    kCEETokenID_ALIAS,
    kCEETokenID_ASCENDING,
    kCEETokenID_ASYNC,
    kCEETokenID_AWAIT,
    kCEETokenID_BY,
    kCEETokenID_DESCENDING,
    kCEETokenID_EQUALS,
    kCEETokenID_FROM,
    kCEETokenID_GLOBAL,
    kCEETokenID_GROUP,
    kCEETokenID_INTO,
    kCEETokenID_JOIN,
    kCEETokenID_NAMEOF,
    kCEETokenID_NOTNULL,
    kCEETokenID_ON,
    kCEETokenID_ORDERBY,
    kCEETokenID_PARTIAL,
    kCEETokenID_REMOVE,
    kCEETokenID_SELECT,
    kCEETokenID_UNMANAGED,
    kCEETokenID_VALUE,
    kCEETokenID_WHEN,
    kCEETokenID_WITH,
    kCEETokenID_YIELD,
    
    kCEETokenID_XML_COMMENT,
    
    kCEETokenID_END
} CEETokenID;

#define CEETokenID_MAX kCEETokenID_END

#define TRANSLATE_STATE_SET(T, S0, I, S1) T[S0][I] = S1

#define TRANSLATE_STATE_ANY(T, S0, S1) {\
    for (cee_int j = 0; j < CEETokenID_MAX; j ++)\
    T[S0][j] = S1;\
}

#define TRANSLATE_FUNCS_SET(T, S0, func, S1) {\
    for (cee_int i = 0; i < CEETokenID_MAX; i ++) {\
        if (func(i))\
        T[S0][i] = S1;\
    }\
}

#define TRANSLATE_STATE_INI(T, C, S) {\
    for (cee_int i = 0; i < C; i ++) {\
        for (cee_int j = 0; j < CEETokenID_MAX; j ++) {\
            T[i][j] = S;\
        }\
        T[i][kCEETokenID_NEW_LINE] = i;\
        T[i][kCEETokenID_WHITE_SPACE] = i;\
    }\
}

typedef enum _CEETokenState {
    kCEETokenStateInit = 0,
    kCEETokenStateSymbolOccupied = 0x1 << 0,
    kCEETokenStateIgnore = 0x1 << 1,
} CEETokenState;

typedef enum _CEERangeListType {
    kCEERangeListTypeContinue,
    kCEERangeListTypeSeparate
} CEERangeListType;

typedef enum _CEETokenStringOption {
    kCEETokenStringOptionDefault = 0,
    kCEETokenStringOptionCompact = 0x1 << 0,
    kCEETokenStringOptionIncompact = 0x1 << 1,
} CEETokenStringOption;

typedef struct _CEEToken {
    CEETokenID identifier;
    cee_long offset;
    cee_ulong length;
    CEETokenState state;
    cee_pointer fregment_ref;
} CEEToken;

typedef struct _CEESourceTokenMap {
    cee_pointer* map;
    cee_ulong length;
} CEESourceTokenMap;

typedef enum _CEETokenClusterType {
    kCEETokenClusterTypeSymbol,
    kCEETokenClusterTypeReference,
} CEETokenClusterType;

typedef struct _CEETokenCluster {
    CEETokenClusterType type;
    cee_pointer content_ref;
} CEETokenCluster;

void cee_token_free(cee_pointer data);
CEEToken* cee_token_create(CEETokenID token_id,
                           cee_long offset,
                           cee_ulong length);

cee_char* cee_string_from_token(const cee_char* subject,
                                CEEToken* token);
void cee_string_from_token_print(const cee_char* subject,
                                 CEEToken* token);
cee_char* cee_string_from_tokens(const cee_char* subject,
                                 CEEList* p,
                                 CEETokenStringOption option);
void cee_string_from_tokens_print(const cee_char* subject,
                                  CEEList* p,
                                  CEETokenStringOption option);
cee_char* cee_string_from_token_slice(const cee_char* subject,
                                      CEEList* p,
                                      CEEList* q,
                                      CEETokenStringOption option);
void cee_string_from_token_slice_print(const cee_char* subject,
                                       CEEList* p,
                                       CEEList* q,
                                       CEETokenStringOption option);
void cee_string_concat_with_token(cee_char** str,
                                  const cee_char* subject,
                                  CEEToken* token);
//cee_char* cee_formated_string_from_token_slice(const cee_char* subject,
//                                               CEEList* p,
//                                               CEEList* q);
//cee_char* cee_formated_string_from_token_slice_append(const cee_char* subject,
//                                                      CEEList* p,
//                                                      CEEList* q,
//                                                      const cee_char* append);
//cee_char* cee_formated_string_from_token_slice_prepend(const cee_char* subject,
//                                                       CEEList* p,
//                                                       CEEList* q,
//                                                       const cee_char* prepend);
CEEList* cee_range_from_token(CEEToken* token);
CEEList* cee_ranges_from_tokens(CEEList* tokens,
                                CEERangeListType type);
CEEList* cee_ranges_from_token_slice(CEEList* p,
                                     CEEList* q,
                                     CEERangeListType type);
cee_char* cee_range_string_from_token(CEEToken* token);
cee_char* cee_ranges_string_from_tokens(CEEList* tokens,
                                        CEERangeListType type);
cee_char* cee_ranges_string_from_token_slice(CEEList* p,
                                             CEEList* q,
                                             CEERangeListType type);

cee_boolean cee_token_id_is_whitespace(CEETokenID identifier);
cee_boolean cee_token_id_is_newline(CEETokenID identifier);
cee_boolean cee_token_is_identifier(CEEList* p,
                                    CEETokenID identifier);
cee_boolean cee_token_is_ignore(CEEList* p);
CEEList* cee_token_not_whitespace_newline_before(CEEList* p);
CEEList* cee_token_not_whitespace_newline_after(CEEList* p);
CEEList* cee_token_not_whitespace_newline_first(CEEList* p);
CEEList* cee_token_not_whitespace_newline_last(CEEList* p);
cee_boolean cee_token_rear_is_round_brackets(CEEList* p);
CEEList* cee_token_skip_round_brackets(CEEList* p,
                                       cee_int n);
CEEList* cee_token_c_parameter_list_slice(CEEList* from,
                                          CEEList* to,
                                          CEETokenID separator);
CEEList* cee_token_until_identifier(CEEList* p,
                                    CEETokenID identifier,
                                    cee_boolean bracket_clear);
cee_boolean cee_token_is_round_bracket_pair(CEEList* p);
CEEList* cee_token_after_round_bracket_pair(CEEList* p);
CEEList* cee_token_identifier_first_equal(CEEList* p,
                                          CEETokenID token_id);
CEEList* cee_tokens_from_slice(CEEList* begin,
                               CEEList* end);
void cee_token_slice_state_mark(CEEList* begin,
                                CEEList* end,
                                CEETokenState state);
void cee_tokens_state_mark(CEEList* tokens,
                           CEETokenState state);

CEEList* cee_token_is_identifier_before(CEEList* p,
                                        CEETokenID token_id);
CEESourceTokenMap* cee_source_token_map_create(const cee_char* subject);
void cee_source_token_map_free(CEESourceTokenMap* token_map);
void cee_source_token_map(CEESourceTokenMap* token_map,
                          CEEList* p);
CEETokenCluster* cee_token_cluster_create(CEETokenClusterType type,
                                          cee_pointer content_ref);
void cee_token_cluster_free(cee_pointer data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TOKEN_H__ */
