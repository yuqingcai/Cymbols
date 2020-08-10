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


typedef enum _CEETokenIdentifierType {
    kCEETokenIdentifierTypeComment                  = 0x1 << 0,
    kCEETokenIdentifierTypeConstant                 = 0x1 << 1,
    kCEETokenIdentifierTypeCharacter                = 0x1 << 2,
    kCEETokenIdentifierTypeLiteral                  = 0x1 << 3,
    kCEETokenIdentifierTypePrepDirective            = 0x1 << 4,
    kCEETokenIdentifierTypePunctuation              = 0x1 << 5,
    kCEETokenIdentifierTypeKeyword                  = 0x1 << 6,
    kCEETokenIdentifierTypeAssignment               = 0x1 << 7,
    kCEETokenIdentifierTypeBuildinType              = 0x1 << 8,
    kCEETokenIdentifierTypeDeclarationSpecifier     = 0x1 << 9,
    kCEETokenIdentifierTypeAccessSpecifier          = 0x1 << 10,
    kCEETokenIdentifierTypeObjectSpecifier          = 0x1 << 11,
    kCEETokenIdentifierTypePrepDirectiveCondition   = 0x1 << 12,
    kCEETokenIdentifierTypeVirtualSpecifier         = 0x1 << 13,
    kCEETokenIdentifierTypeAlignasSpecifier         = 0x1 << 14,
    kCEETokenIdentifierTypeOverloadOperator         = 0x1 << 15,
    
} CEETokenIdentifierType;

typedef enum _CEETokenID {
    kCEETokenID_UNKNOW = 512,
    kCEETokenID_NEW_LINE,
    kCEETokenID_WHITE_SPACE,
    
    kCEETokenID_C_COMMENT,
    kCEETokenID_CPP_COMMENT,
    kCEETokenID_IDENTIFIER,
    kCEETokenID_CONSTANT,
    kCEETokenID_CHARACTER,
    kCEETokenID_LITERAL,
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
    
    
    kCEEARMASMTokenID_CCOMMENT,
    kCEEARMASMTokenID_CPPCOMMENT,
    kCEEARMASMTokenID_IDENTIFIER,
    kCEEARMASMTokenID_CONSTANT,
    kCEEARMASMTokenID_CHARACTER,
    kCEEARMASMTokenID_LITERAL,
    
    
    kCEEGNUASMTokenID_CCOMMENT,
    kCEEGNUASMTokenID_CPPCOMMENT,
    kCEEGNUASMTokenID_IDENTIFIER,
    kCEEGNUASMTokenID_CONSTANT,
    kCEEGNUASMTokenID_CHARACTER,
    kCEEGNUASMTokenID_LITERAL,
    
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
} CEETokenState;

typedef enum _CEERangeListType {
    kCEERangeListTypeContinue,
    kCEERangeListTypeSeparate
} CEERangeListType;

typedef enum _CEETokenStringOption {
    kCEETokenStringOptionDefault = 0,
    kCEETokenStringOptionCompact = 0x1 << 0,
} CEETokenStringOption;

typedef struct _CEEToken {
    CEETokenID identifier;
    cee_long offset;
    cee_ulong length;
    CEETokenState state;
    cee_pointer fregment_ref;
} CEEToken;

void cee_token_free(cee_pointer data);
CEEToken* cee_token_create(CEETokenID token_id,
                           cee_long offset,
                           cee_ulong length);

cee_char* cee_string_from_token(const cee_uchar* subject,
                                CEEToken* token);
void cee_string_from_token_print(const cee_uchar* subject,
                                 CEEToken* token);
cee_char* cee_string_from_tokens(const cee_uchar* subject,
                                 CEEList* p,
                                 CEETokenStringOption option);
void cee_string_from_tokens_print(const cee_uchar* subject,
                                  CEEList* p,
                                  CEETokenStringOption option);
cee_char* cee_string_from_token_slice(const cee_uchar* subject,
                                      CEEList* p,
                                      CEEList* q,
                                      CEETokenStringOption option);
void cee_string_from_token_slice_print(const cee_uchar* subject,
                                       CEEList* p,
                                       CEEList* q,
                                       CEETokenStringOption option);
void cee_string_concat_with_token(cee_char** str,
                                  const cee_uchar* subject,
                                  CEEToken* token);
//cee_char* cee_formated_string_from_token_slice(const cee_uchar* subject,
//                                               CEEList* p,
//                                               CEEList* q);
//cee_char* cee_formated_string_from_token_slice_append(const cee_uchar* subject,
//                                                      CEEList* p,
//                                                      CEEList* q,
//                                                      const cee_char* append);
//cee_char* cee_formated_string_from_token_slice_prepend(const cee_uchar* subject,
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
void cee_token_slice_mark(CEEList* begin,
                          CEEList* end,
                          CEETokenState state);
void cee_tokens_mark(CEEList* tokens,
                     CEETokenState state);

CEEList* cee_token_is_identifier_before(CEEList* p,
                                        CEETokenID token_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TOKEN_H__ */
