%option yylineno
%option noyywrap
%option prefix="c_scan_"
%x Preprocesser
L	[a-zA-Z_]
H	[a-fA-F0-9]
D	[0-9]
O   [0-7]
B   [01]
WS	[ \t]
NL	(\r\n|\n|\r\v\f)
WN  ([ \t]|\r\n|\n|\r\v\f)
IS  (u|U|l|L)*
FS  (f|F|l|L)
DE  ([eE][+|-]?[0-9]+)
HE  ([pP][+|-]?[0-9]+)
WB  [^a-zA-Z0-9_"'<]
%{

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "cee_lib.h"
#include "cee_token.h"

static cee_ulong g_scan_offset = 0;
static YY_BUFFER_STATE buffer = NULL;

#define RETURN_TOKEN_ID(token_id) {\
    return token_id;\
}

static int c_comment(cee_long offset, 
                     cee_boolean* eof);
static int cpp_comment(cee_long offset, 
                       cee_boolean* eof);
static int literal(cee_long offset,
                   cee_boolean* eof);
static int character(cee_long offset, 
                    cee_boolean* eof);
%}

%%
"alignas"                                   RETURN_TOKEN_ID(kCEETokenID_ALIGNAS                    )
"alignof"                                   RETURN_TOKEN_ID(kCEETokenID_ALIGNOF                    )
"and"                                       RETURN_TOKEN_ID(kCEETokenID_AND                        )
"and_eq"                                    RETURN_TOKEN_ID(kCEETokenID_AND_EQ                     )
"asm"                                       RETURN_TOKEN_ID(kCEETokenID_ASM                        )
"atomic_cancel"                             RETURN_TOKEN_ID(kCEETokenID_ATOMIC_CANCEL              )
"atomic_commit"                             RETURN_TOKEN_ID(kCEETokenID_ATOMIC_COMMIT              )
"atomic_noexcept"                           RETURN_TOKEN_ID(kCEETokenID_ATOMIC_NOEXCEPT            )
"auto"                                      RETURN_TOKEN_ID(kCEETokenID_AUTO                       )
"bitand"                                    RETURN_TOKEN_ID(kCEETokenID_BITAND                     )
"bitor"                                     RETURN_TOKEN_ID(kCEETokenID_BITOR                      )
"bool"                                      RETURN_TOKEN_ID(kCEETokenID_BOOL                       )
"break"                                     RETURN_TOKEN_ID(kCEETokenID_BREAK                      )
"case"                                      RETURN_TOKEN_ID(kCEETokenID_CASE                       )
"catch"                                     RETURN_TOKEN_ID(kCEETokenID_CATCH                      )
"char"                                      RETURN_TOKEN_ID(kCEETokenID_CHAR                       )
"char16_t"                                  RETURN_TOKEN_ID(kCEETokenID_CHAR16_T                   )
"char32_t"                                  RETURN_TOKEN_ID(kCEETokenID_CHAR32_T                   )
"class"                                     RETURN_TOKEN_ID(kCEETokenID_CLASS                      )
"compl"                                     RETURN_TOKEN_ID(kCEETokenID_COMPL                      )
"concept"                                   RETURN_TOKEN_ID(kCEETokenID_CONCEPT                    )
"const"                                     RETURN_TOKEN_ID(kCEETokenID_CONST                      )
"constexpr"                                 RETURN_TOKEN_ID(kCEETokenID_CONSTEXPR                  )
"consteval"                                 RETURN_TOKEN_ID(kCEETokenID_CONSTEVAL                  )
"const_cast"                                RETURN_TOKEN_ID(kCEETokenID_CONST_CAST                 )
"continue"                                  RETURN_TOKEN_ID(kCEETokenID_CONTINUE                   )
"decltype"                                  RETURN_TOKEN_ID(kCEETokenID_DECLTYPE                   )
"default"                                   RETURN_TOKEN_ID(kCEETokenID_DEFAULT                    )
"delete"                                    RETURN_TOKEN_ID(kCEETokenID_DELETE                     )
"do"                                        RETURN_TOKEN_ID(kCEETokenID_DO                         )
"double"                                    RETURN_TOKEN_ID(kCEETokenID_DOUBLE                     )
"dynamic_cast"                              RETURN_TOKEN_ID(kCEETokenID_DYNAMIC_CAST               )
"else"                                      RETURN_TOKEN_ID(kCEETokenID_ELSE                       )
"enum"                                      RETURN_TOKEN_ID(kCEETokenID_ENUM                       )
"explicit"                                  RETURN_TOKEN_ID(kCEETokenID_EXPLICIT                   )
"export"                                    RETURN_TOKEN_ID(kCEETokenID_EXPORT                     )
"extern"                                    RETURN_TOKEN_ID(kCEETokenID_EXTERN                     )
"false"                                     RETURN_TOKEN_ID(kCEETokenID_FALSE                      )
"float"                                     RETURN_TOKEN_ID(kCEETokenID_FLOAT                      )
"for"                                       RETURN_TOKEN_ID(kCEETokenID_FOR                        )
"friend"                                    RETURN_TOKEN_ID(kCEETokenID_FRIEND                     )
"goto"                                      RETURN_TOKEN_ID(kCEETokenID_GOTO                       )
"if"                                        RETURN_TOKEN_ID(kCEETokenID_IF                         )
"import"                                    RETURN_TOKEN_ID(kCEETokenID_IMPORT                     )
"inline"                                    RETURN_TOKEN_ID(kCEETokenID_INLINE                     )
"int"                                       RETURN_TOKEN_ID(kCEETokenID_INT                        )
"long"                                      RETURN_TOKEN_ID(kCEETokenID_LONG                       )
"module"                                    RETURN_TOKEN_ID(kCEETokenID_MODULE                     )
"mutable"                                   RETURN_TOKEN_ID(kCEETokenID_MUTABLE                    )
"namespace"                                 RETURN_TOKEN_ID(kCEETokenID_NAMESPACE                  )
"new"                                       RETURN_TOKEN_ID(kCEETokenID_NEW                        )
"noexcept"                                  RETURN_TOKEN_ID(kCEETokenID_NOEXCEPT                   )
"not"                                       RETURN_TOKEN_ID(kCEETokenID_NOT                        )
"not_eq"                                    RETURN_TOKEN_ID(kCEETokenID_NOT_EQ                     )
"nullptr"                                   RETURN_TOKEN_ID(kCEETokenID_NULLPTR                    )
"operator"                                  RETURN_TOKEN_ID(kCEETokenID_OPERATOR                   )
"or"                                        RETURN_TOKEN_ID(kCEETokenID_OR                         )
"or_eq"                                     RETURN_TOKEN_ID(kCEETokenID_OR_EQ                      )
"private"                                   RETURN_TOKEN_ID(kCEETokenID_PRIVATE                    )
"protected"                                 RETURN_TOKEN_ID(kCEETokenID_PROTECTED                  )
"public"                                    RETURN_TOKEN_ID(kCEETokenID_PUBLIC                     )
"register"                                  RETURN_TOKEN_ID(kCEETokenID_REGISTER                   )
"reinterpret_cast"                          RETURN_TOKEN_ID(kCEETokenID_REINTERPRET_CAST           )
"requires"                                  RETURN_TOKEN_ID(kCEETokenID_REQUIRES                   )
"restrict"                                  RETURN_TOKEN_ID(kCEETokenID_RESTRICT                   )
"return"                                    RETURN_TOKEN_ID(kCEETokenID_RETURN                     )
"short"                                     RETURN_TOKEN_ID(kCEETokenID_SHORT                      )
"signed"                                    RETURN_TOKEN_ID(kCEETokenID_SIGNED                     )
"sizeof"                                    RETURN_TOKEN_ID(kCEETokenID_SIZEOF                     )
"static"                                    RETURN_TOKEN_ID(kCEETokenID_STATIC                     )
"static_assert"                             RETURN_TOKEN_ID(kCEETokenID_STATIC_ASSERT              )
"static_cast"                               RETURN_TOKEN_ID(kCEETokenID_STATIC_CAST                )
"struct"                                    RETURN_TOKEN_ID(kCEETokenID_STRUCT                     )
"switch"                                    RETURN_TOKEN_ID(kCEETokenID_SWITCH                     )
"synchronized"                              RETURN_TOKEN_ID(kCEETokenID_SYNCHRONIZED               )
"template"                                  RETURN_TOKEN_ID(kCEETokenID_TEMPLATE                   )
"this"                                      RETURN_TOKEN_ID(kCEETokenID_THIS                       )
"thread_local"                              RETURN_TOKEN_ID(kCEETokenID_THREAD_LOCAL               )
"throw"                                     RETURN_TOKEN_ID(kCEETokenID_THROW                      )
"true"                                      RETURN_TOKEN_ID(kCEETokenID_TRUE                       )
"try"                                       RETURN_TOKEN_ID(kCEETokenID_TRY                        )
"typedef"                                   RETURN_TOKEN_ID(kCEETokenID_TYPEDEF                    )
"typeid"                                    RETURN_TOKEN_ID(kCEETokenID_TYPEID                     )
"typename"                                  RETURN_TOKEN_ID(kCEETokenID_TYPENAME                   )
"union"                                     RETURN_TOKEN_ID(kCEETokenID_UNION                      )
"unsigned"                                  RETURN_TOKEN_ID(kCEETokenID_UNSIGNED                   )
"using"                                     RETURN_TOKEN_ID(kCEETokenID_USING                      )
"virtual"                                   RETURN_TOKEN_ID(kCEETokenID_VIRTUAL                    )
"volatile"                                  RETURN_TOKEN_ID(kCEETokenID_VOLATILE                   )
"wchar_t"                                   RETURN_TOKEN_ID(kCEETokenID_WCHAR_T                    )
"while"                                     RETURN_TOKEN_ID(kCEETokenID_WHILE                      )
"xor"                                       RETURN_TOKEN_ID(kCEETokenID_XOR                        )
"xor_eq"                                    RETURN_TOKEN_ID(kCEETokenID_XOR_EQ                     )
"override"                                  RETURN_TOKEN_ID(kCEETokenID_OVERRIDE                   )
"final"                                     RETURN_TOKEN_ID(kCEETokenID_FINAL                      )
"transaction_safe"                          RETURN_TOKEN_ID(kCEETokenID_TRANSACTION_SAFE           )
"transaction_safe_dynamic"                  RETURN_TOKEN_ID(kCEETokenID_TRANSACTION_SAFE_DYNAMIC   )
"_Alignas"                                  RETURN_TOKEN_ID(kCEETokenID__ALIGNAS                   )
"_Alignof"                                  RETURN_TOKEN_ID(kCEETokenID__ALIGNOF                   )
"_Atomic"                                   RETURN_TOKEN_ID(kCEETokenID__ATOMIC                    )
"_Bool"                                     RETURN_TOKEN_ID(kCEETokenID__BOOL                      )
"_Complex"                                  RETURN_TOKEN_ID(kCEETokenID__COMPLEX                   )
"_Generic"                                  RETURN_TOKEN_ID(kCEETokenID__GENERIC                   )
"_Imaginary"                                RETURN_TOKEN_ID(kCEETokenID__IMAGINARY                 )
"_Noreturn"                                 RETURN_TOKEN_ID(kCEETokenID__NORETURN                  )
"_Static_assert"                            RETURN_TOKEN_ID(kCEETokenID__STATIC_ASSERT             )
"_Thread_local"                             RETURN_TOKEN_ID(kCEETokenID__THREAD_LOCAL              )
"="                                         RETURN_TOKEN_ID('='                                    )
"+"                                         RETURN_TOKEN_ID('+'                                    )
"-"                                         RETURN_TOKEN_ID('-'                                    )
"*"                                         RETURN_TOKEN_ID('*'                                    )
"/"                                         RETURN_TOKEN_ID('/'                                    )
"\\"                                        RETURN_TOKEN_ID('\\'                                   )
"%"                                         RETURN_TOKEN_ID('%'                                    )
"~"                                         RETURN_TOKEN_ID('~'                                    )
"&"                                         RETURN_TOKEN_ID('&'                                    )
"|"                                         RETURN_TOKEN_ID('|'                                    )
"^"                                         RETURN_TOKEN_ID('^'                                    )
"!"                                         RETURN_TOKEN_ID('!'                                    )
"<"                                         RETURN_TOKEN_ID('<'                                    )
">"                                         RETURN_TOKEN_ID('>'                                    )
"."                                         RETURN_TOKEN_ID('.'                                    )
","                                         RETURN_TOKEN_ID(','                                    )
"?"                                         RETURN_TOKEN_ID('?'                                    )
":"                                         RETURN_TOKEN_ID(':'                                    )
"("                                         RETURN_TOKEN_ID('('                                    )
")"                                         RETURN_TOKEN_ID(')'                                    )
("{"|"<%")                                  RETURN_TOKEN_ID('{'                                    )
("}"|"%>")                                  RETURN_TOKEN_ID('}'                                    )
("["|"<:")                                  RETURN_TOKEN_ID('['                                    )
("]"|":>")                                  RETURN_TOKEN_ID(']'                                    )
("#"|"%:")                                  RETURN_TOKEN_ID('#'                                    )
";"                                         RETURN_TOKEN_ID(';'                                    )
"@"                                         RETURN_TOKEN_ID('@'                                    )
"+="                                        RETURN_TOKEN_ID(kCEETokenID_ADD_ASSIGNMENT             )
"-="                                        RETURN_TOKEN_ID(kCEETokenID_MINUS_ASSIGNMENT           )
"*="                                        RETURN_TOKEN_ID(kCEETokenID_MULTI_ASSIGNMENT           )
"/="                                        RETURN_TOKEN_ID(kCEETokenID_DIV_ASSIGNMENT             )
"%="                                        RETURN_TOKEN_ID(kCEETokenID_MOD_ASSIGNMENT             )
"&="                                        RETURN_TOKEN_ID(kCEETokenID_AND_ASSIGNMENT             )
"|="                                        RETURN_TOKEN_ID(kCEETokenID_OR_ASSIGNMENT              )
"^="                                        RETURN_TOKEN_ID(kCEETokenID_XOR_ASSIGNMENT             )
"<<="                                       RETURN_TOKEN_ID(kCEETokenID_LEFT_OFFSET_ASSIGNMENT     )
">>="                                       RETURN_TOKEN_ID(kCEETokenID_RIGHT_OFFSET_ASSIGNMENT    )
"++"                                        RETURN_TOKEN_ID(kCEETokenID_INCREMENT                  )
"--"                                        RETURN_TOKEN_ID(kCEETokenID_DECREMENT                  )
"&&"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_AND                  )
"||"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_OR                   )
"=="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_EQUAL                )
"!="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_UNEQUAL              )
"<="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LESS_EQUAL           )
">="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LARGE_EQUAL          )
"->"                                        RETURN_TOKEN_ID(kCEETokenID_MEMBER_POINTER             )
"..."                                       RETURN_TOKEN_ID(kCEETokenID_ELLIPSIS                   )
"::"                                        RETURN_TOKEN_ID(kCEETokenID_SCOPE                      )
"<=>"                                       RETURN_TOKEN_ID(kCEETokenID_COMPARISON                 )
"->*"                                       RETURN_TOKEN_ID(kCEETokenID_POINTER_MEMBER_POINTER     )
("##"|"%:%:")                               RETURN_TOKEN_ID(kCEETokenID_TOKEN_PASTING              )
"@"{WN}*"{"                                 RETURN_TOKEN_ID(kCEETokenID_AT_BRACE                   )
"^"{WN}*"{"                                 RETURN_TOKEN_ID(kCEETokenID_CARAT_BRACE                )
"@"{WN}*"["                                 RETURN_TOKEN_ID(kCEETokenID_AT_BRACKET                 )
"@"{WN}*"interface"                         RETURN_TOKEN_ID(kCEETokenID_AT_INTERFACE               )
"@"{WN}*"end"                               RETURN_TOKEN_ID(kCEETokenID_AT_END                     )
"@"{WN}*"implementation"                    RETURN_TOKEN_ID(kCEETokenID_AT_IMPLEMENTATION          )
"@"{WN}*"class"                             RETURN_TOKEN_ID(kCEETokenID_AT_CLASS                   )
"@"{WN}*"public"                            RETURN_TOKEN_ID(kCEETokenID_AT_PUBLIC                  )
"@"{WN}*"package"                           RETURN_TOKEN_ID(kCEETokenID_AT_PACKAGE                 )
"@"{WN}*"protected"                         RETURN_TOKEN_ID(kCEETokenID_AT_PROTECTED               )
"@"{WN}*"private"                           RETURN_TOKEN_ID(kCEETokenID_AT_PRIVATE                 )
"@"{WN}*"property"                          RETURN_TOKEN_ID(kCEETokenID_AT_PROPERTY                )
"@"{WN}*"synthesize"                        RETURN_TOKEN_ID(kCEETokenID_AT_SYNTHESIZE              )
"@"{WN}*"dynamic"                           RETURN_TOKEN_ID(kCEETokenID_AT_DYNAMIC                 )
"@"{WN}*"required"                          RETURN_TOKEN_ID(kCEETokenID_AT_REQUIRED                )
"@"{WN}*"optional"                          RETURN_TOKEN_ID(kCEETokenID_AT_OPTIONAL                )
"@"{WN}*"try"                               RETURN_TOKEN_ID(kCEETokenID_AT_TRY                     )
"@"{WN}*"catch"                             RETURN_TOKEN_ID(kCEETokenID_AT_CATCH                   )
"@"{WN}*"finally"                           RETURN_TOKEN_ID(kCEETokenID_AT_FINALLY                 )
"@"{WN}*"throw"                             RETURN_TOKEN_ID(kCEETokenID_AT_THROW                   )
"@"{WN}*"selector"                          RETURN_TOKEN_ID(kCEETokenID_AT_SELECTOR                )
"@"{WN}*"protocol"                          RETURN_TOKEN_ID(kCEETokenID_AT_PROTOCOL                )
"@"{WN}*"encode"                            RETURN_TOKEN_ID(kCEETokenID_AT_ENCODE                  )
"@"{WN}*"defs"                              RETURN_TOKEN_ID(kCEETokenID_AT_DEFS                    )
"@"{WN}*"autoreleasepool"                   RETURN_TOKEN_ID(kCEETokenID_AT_AUTORELEASEPOOL         )
"@"{WN}*"synchronized"                      RETURN_TOKEN_ID(kCEETokenID_AT_SYNCHRONIZED            )
"@"{WN}*"compatibility_alias"               RETURN_TOKEN_ID(kCEETokenID_AT_COMPATIBILITY_ALIAS     )
"id"                                        RETURN_TOKEN_ID(kCEETokenID_ID                         )
"instancetype"                              RETURN_TOKEN_ID(kCEETokenID_INSTANCETYPE               )
"IMP"                                       RETURN_TOKEN_ID(kCEETokenID_IMP                        )
"in"                                        RETURN_TOKEN_ID(kCEETokenID_IN                         )
"inout"                                     RETURN_TOKEN_ID(kCEETokenID_INOUT                      )
"nil"                                       RETURN_TOKEN_ID(kCEETokenID_NIL                        )
"oneway"                                    RETURN_TOKEN_ID(kCEETokenID_ONEWAY                     )
"out"                                       RETURN_TOKEN_ID(kCEETokenID_OUT                        )
"SEL"                                       RETURN_TOKEN_ID(kCEETokenID_SEL                        )
"self"                                      RETURN_TOKEN_ID(kCEETokenID_SELF                       )
"super"                                     RETURN_TOKEN_ID(kCEETokenID_SUPER                      )
"YES"                                       RETURN_TOKEN_ID(kCEETokenID_YES                        )
"NO"                                        RETURN_TOKEN_ID(kCEETokenID_NO                         )
"atomic"                                    RETURN_TOKEN_ID(kCEETokenID_ATOMIC                     )
"nonatomic"                                 RETURN_TOKEN_ID(kCEETokenID_NONATOMIC                  )
"strong"                                    RETURN_TOKEN_ID(kCEETokenID_STRONG                     )
"weak"                                      RETURN_TOKEN_ID(kCEETokenID_WEAK                       )
"assign"                                    RETURN_TOKEN_ID(kCEETokenID_ASSIGN                     )
"copy"                                      RETURN_TOKEN_ID(kCEETokenID_COPY                       )
"retain"                                    RETURN_TOKEN_ID(kCEETokenID_RETAIN                     )
"readonly"                                  RETURN_TOKEN_ID(kCEETokenID_READONLY                   )
"readwrite"                                 RETURN_TOKEN_ID(kCEETokenID_READWRITE                  )
"__bridge"                                  RETURN_TOKEN_ID(kCEETokenID___BRIDGE                   )
"__bridge_transfer"                         RETURN_TOKEN_ID(kCEETokenID___BRIDGE_TRANSFER          )
"__bridge_retained"                         RETURN_TOKEN_ID(kCEETokenID___BRIDGE_RETAINED          )
"__block"                                   RETURN_TOKEN_ID(kCEETokenID___BLOCK                    )
"__kindof"                                  RETURN_TOKEN_ID(kCEETokenID___KIND_OF                  )
"_Nonnull"                                  RETURN_TOKEN_ID(kCEETokenID__NONNULL                   )
"_Nullable"                                 RETURN_TOKEN_ID(kCEETokenID__NULLABLE                  )
"nonnull"                                   RETURN_TOKEN_ID(kCEETokenID_NONNULL                    )
"nullable"                                  RETURN_TOKEN_ID(kCEETokenID_NULLABLE                   )
"NS_ENUM"                                   RETURN_TOKEN_ID(kCEETokenID_NS_ENUM                    )
"NS_OPTIONS"                                RETURN_TOKEN_ID(kCEETokenID_NS_OPTIONS                 )
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_LINES_COMMENT              )
"//"                                        RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT               )
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                    )
"@"{WS}*"\""                                RETURN_TOKEN_ID(kCEETokenID_LITERAL                    )
"'"                                         RETURN_TOKEN_ID(kCEETokenID_CHARACTER                  )
"#"{WS}*"include"                           RETURN_TOKEN_ID(kCEETokenID_HASH_INCLUDE               )
"#"{WS}*"define"                            RETURN_TOKEN_ID(kCEETokenID_HASH_DEFINE                )
"#"{WS}*"if"                                RETURN_TOKEN_ID(kCEETokenID_HASH_IF                    )
"#"{WS}*"ifdef"                             RETURN_TOKEN_ID(kCEETokenID_HASH_IFDEF                 )
"#"{WS}*"ifndef"                            RETURN_TOKEN_ID(kCEETokenID_HASH_IFNDEF                )
"#"{WS}*"endif"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ENDIF                 )
"#"{WS}*"else"                              RETURN_TOKEN_ID(kCEETokenID_HASH_ELSE                  )
"#"{WS}*"elif"                              RETURN_TOKEN_ID(kCEETokenID_HASH_ELIF                  )
"#"{WS}*"undef"                             RETURN_TOKEN_ID(kCEETokenID_HASH_UNDEF                 )
"#"{WS}*"line"                              RETURN_TOKEN_ID(kCEETokenID_HASH_LINE                  )
"#"{WS}*"error"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ERROR                 )
"#"{WS}*"warning"                           RETURN_TOKEN_ID(kCEETokenID_HASH_WARNING               )
"#"{WS}*"pragma"                            RETURN_TOKEN_ID(kCEETokenID_HASH_PRAGMA                )
"#"{WS}*"import"                            RETURN_TOKEN_ID(kCEETokenID_HASH_IMPORT                )
"#"{WS}*{L}({L}|{D})*                       RETURN_TOKEN_ID(kCEETokenID_HASH_UNKNOW                )
"void"                                      RETURN_TOKEN_ID(kCEETokenID_VOID                       )
"long"{WN}+"double"                         RETURN_TOKEN_ID(kCEETokenID_LONG_DOUBLE                )
"signed"{WN}+"char"                         RETURN_TOKEN_ID(kCEETokenID_SIGNED_CHAR                )
"unsigned"{WN}+"char"                       RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_CHAR              )
"short"{WN}+"int"                           RETURN_TOKEN_ID(kCEETokenID_SHORT_INT                  )
"signed"{WN}+"short"                        RETURN_TOKEN_ID(kCEETokenID_SIGNED_SHORT               )
"signed"{WN}+"short"{WN}+"int"              RETURN_TOKEN_ID(kCEETokenID_SIGNED_SHORT_INT           )
"unsigned"{WN}+"short"                      RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_SHORT             )
"unsigned"{WN}+"short"{WN}+"int"            RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_SHORT_INT         )
"signed"{WN}+"int"                          RETURN_TOKEN_ID(kCEETokenID_SIGNED_INT                 )
"unsigned"{WN}+"int"                        RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_INT               )
"long"{WN}+"int"                            RETURN_TOKEN_ID(kCEETokenID_LONG_INT                   )
"signed"{WN}+"long"                         RETURN_TOKEN_ID(kCEETokenID_SIGNED_LONG                )
"signed"{WN}+"long"{WN}+"int"               RETURN_TOKEN_ID(kCEETokenID_SIGNED_LONG_INT            )
"unsigned"{WN}+"long"                       RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_LONG              )
"unsigned"{WN}+"long"{WN}+"int"             RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_LONG_INT          )
"long"{WN}+"long"                           RETURN_TOKEN_ID(kCEETokenID_LONG_LONG                  )
"long"{WN}+"long"{WN}+"int"                 RETURN_TOKEN_ID(kCEETokenID_LONG_LONG_INT              )
"signed"{WN}+"long"{WN}+"long"              RETURN_TOKEN_ID(kCEETokenID_SIGNED_LONG_LONG           )
"signed"{WN}+"long"{WN}+"long"{WN}+"int"    RETURN_TOKEN_ID(kCEETokenID_SIGNED_LONG_LONG_INT       )
"unsigned"{WN}+"long"{WN}+"long"            RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_LONG_LONG         )
"unsigned"{WN}+"long"{WN}+"long"{WN}+"int"  RETURN_TOKEN_ID(kCEETokenID_UNSIGNED_LONG_LONG_INT     )
"int8_t"                                    RETURN_TOKEN_ID(kCEETokenID_INT8_T                     )
"int16_t"                                   RETURN_TOKEN_ID(kCEETokenID_INT16_T                    )
"int32_t"                                   RETURN_TOKEN_ID(kCEETokenID_INT32_T                    )
"int64_t"                                   RETURN_TOKEN_ID(kCEETokenID_INT64_T                    )
"int_fast8_t"                               RETURN_TOKEN_ID(kCEETokenID_INT_FAST8_T                )
"int_fast16_t"                              RETURN_TOKEN_ID(kCEETokenID_INT_FAST16_T               )
"int_fast32_t"                              RETURN_TOKEN_ID(kCEETokenID_INT_FAST32_T               )
"int_fast64_t"                              RETURN_TOKEN_ID(kCEETokenID_INT_FAST64_T               )
"int_least8_t"                              RETURN_TOKEN_ID(kCEETokenID_INT_LEAST8_T               )
"int_least16_t"                             RETURN_TOKEN_ID(kCEETokenID_INT_LEAST16_T              )
"int_least32_t"                             RETURN_TOKEN_ID(kCEETokenID_INT_LEAST32_T              )
"int_least64_t"                             RETURN_TOKEN_ID(kCEETokenID_INT_LEAST64_T              )
"intmax_t"                                  RETURN_TOKEN_ID(kCEETokenID_INTMAX_T                   )
"intptr_t"                                  RETURN_TOKEN_ID(kCEETokenID_INTPTR_T                   )
"uint8_t"                                   RETURN_TOKEN_ID(kCEETokenID_UINT8_T                    )
"uint16_t"                                  RETURN_TOKEN_ID(kCEETokenID_UINT16_T                   )
"uint32_t"                                  RETURN_TOKEN_ID(kCEETokenID_UINT32_T                   )
"uint64_t"                                  RETURN_TOKEN_ID(kCEETokenID_UINT64_T                   )
"uint_fast8_t"                              RETURN_TOKEN_ID(kCEETokenID_UINT_FAST8_T               )
"uint_fast16_t"                             RETURN_TOKEN_ID(kCEETokenID_UINT_FAST16_T              )
"uint_fast32_t"                             RETURN_TOKEN_ID(kCEETokenID_UINT_FAST32_T              )
"uint_fast64_t"                             RETURN_TOKEN_ID(kCEETokenID_UINT_FAST64_T              )
"uint_least8_t"                             RETURN_TOKEN_ID(kCEETokenID_UINT_LEAST8_T              )
"uint_least16_t"                            RETURN_TOKEN_ID(kCEETokenID_UINT_LEAST16_T             )
"uint_least32_t"                            RETURN_TOKEN_ID(kCEETokenID_UINT_LEAST32_T             )
"uint_least64_t"                            RETURN_TOKEN_ID(kCEETokenID_UINT_LEAST64_T             )
"uintmax_t"                                 RETURN_TOKEN_ID(kCEETokenID_UINTMAX_T                  )
"uintptr_t"                                 RETURN_TOKEN_ID(kCEETokenID_UINTPTR_T                  )
0[xX]{H}+{IS}?                              RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
{D}+{IS}?                                   RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
0b{B}+{IS}?                                 RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
{D}+{DE}{FS}?                               RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
{D}*"."{D}+{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
{D}+"."{D}*{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
0[xX]{H}+{HE}{FS}?                          RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
0[xX]{H}*"."{H}+{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
0[xX]{H}+"."{H}*{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                   )
{L}({L}|{D})*                               RETURN_TOKEN_ID(kCEETokenID_IDENTIFIER                 )
{NL}                                        RETURN_TOKEN_ID(kCEETokenID_NEW_LINE                   )
{WS}                                        RETURN_TOKEN_ID(kCEETokenID_WHITE_SPACE                )
.                                           RETURN_TOKEN_ID(kCEETokenID_UNKNOW                     )
%%

static int c_comment(cee_long offset, 
                     cee_boolean* eof)
{
    int c0, c1;
    int length = 0;
    
    do {
        c0 = input();
        
        if (!c0) {
            *eof = TRUE;
            break;
        }
        
        length ++;
        
        if (c0 == '*') {
            c1 = input();
            
            if (!c1) {
                *eof = TRUE;
                break;
            }
            
            if (c1 == '/') {
                length ++;
                break;
            }
            
            unput(c1);
        }
        
    } while (1);
    
    return length;
}

static int cpp_comment(cee_long offset, 
                       cee_boolean* eof)
{
    int c0;
    int length = 0;
    
    do {
        c0 = input();
        
        if (!c0) {
            *eof = TRUE;
            break;
        }
        
        if (c0 == '\n' || c0 == '\r') {
            unput(c0);
            break;
        }
        
        length ++;
        
    } while (1);
    
    return length;
}


static int literal(cee_long offset,
                   cee_boolean* eof)
{
    int length = 0;
    int escape = 0;
    int c0;
    
    do {
        c0 = input();
        if (!c0) {
            *eof = TRUE;
            break;
        }
        
        length ++;
        
        if (c0 == '\\' && !escape)
            escape ++;
        else if (escape && c0 != '"')
            escape --;
        
        if (c0 == '"') {
            if (escape)
                escape --;
            else
                break;
        }
        
    } while (1);
    
    return length;
}

static int character(cee_long offset, 
                     cee_boolean* eof)
{
    int length = 0;
    int escape = 0;
    int c0;
    
    do {
        c0 = input();
        if (!c0) {
            *eof = TRUE;
            break;
        }
        
        length ++;
        
        if (c0 == '\\' && !escape)
            escape ++;
        else if (escape && c0 != '\'')
            escape --;
        
        if (c0 == '\'') {
            if (escape)
                escape --;
            else
                break;
        }
        
    } while (1);
    
    return length;
}

cee_int cee_lexer_c_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = c_scan_lex();
    cee_long offset = g_scan_offset;
    cee_ulong length = c_scan_get_leng();
    
    g_scan_offset += length;
    
    *token = cee_token_create(identifier, offset, length);
    
    if (!identifier) /* EOF */
        return 0;
    
    if (identifier == kCEETokenID_LINES_COMMENT) {
        tail_length = c_comment(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LINE_COMMENT) {
        tail_length = cpp_comment(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LITERAL) {
        tail_length = literal(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_CHARACTER) {
        tail_length = character(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    
    if (eof)
        return 0;
    
    return 1;
}

void cee_lexer_c_buffer_create(const cee_uchar* subject)
{
    buffer = c_scan__scan_bytes(subject, strlen(subject));
    g_scan_offset = 0;
}

void cee_lexer_c_buffer_free()
{
    yy_delete_buffer(buffer);
}
