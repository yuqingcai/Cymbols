%option yylineno
%option noyywrap
%option prefix="csharp_scan_"
%x Preprocesser
L   [a-zA-Z_]
H   [a-fA-F0-9]
D   [0-9]
O   [0-7]
B   [01_]
WS  [ \t]
NL  (\r\n|\n|\r\v\f)
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

static cee_ulong scan_offset = 0;
static cee_int line_no = 0;
static YY_BUFFER_STATE buffer = NULL;

#define RETURN_TOKEN_ID(token_id) {\
    return token_id;\
}

static int lines_comment(cee_long offset, 
                     cee_boolean* eof);
static int line_comment(cee_long offset, 
                       cee_boolean* eof);
static int literal(cee_long offset,
                   cee_boolean* eof);
static int character(cee_long offset, 
                    cee_boolean* eof);
                    
%}

%%
"abstract"                                  RETURN_TOKEN_ID(kCEETokenID_ABSTRACT                    )
"as"                                        RETURN_TOKEN_ID(kCEETokenID_AS                          )
"base"                                      RETURN_TOKEN_ID(kCEETokenID_BASE                        )
"bool"                                      RETURN_TOKEN_ID(kCEETokenID_BOOL                        )
"break"                                     RETURN_TOKEN_ID(kCEETokenID_BREAK                       )
"byte"                                      RETURN_TOKEN_ID(kCEETokenID_BYTE                        )
"case"                                      RETURN_TOKEN_ID(kCEETokenID_CASE                        )
"catch"                                     RETURN_TOKEN_ID(kCEETokenID_CATCH                       )
"char"                                      RETURN_TOKEN_ID(kCEETokenID_CHAR                        )
"checked"                                   RETURN_TOKEN_ID(kCEETokenID_CHECKED                     )
"class"                                     RETURN_TOKEN_ID(kCEETokenID_CLASS                       )
"const"                                     RETURN_TOKEN_ID(kCEETokenID_CONST                       )
"continue"                                  RETURN_TOKEN_ID(kCEETokenID_CONTINUE                    )
"decimal"                                   RETURN_TOKEN_ID(kCEETokenID_DECIMAL                     )
"default"                                   RETURN_TOKEN_ID(kCEETokenID_DEFAULT                     )
"delegate"                                  RETURN_TOKEN_ID(kCEETokenID_DELEGATE                    )
"do"                                        RETURN_TOKEN_ID(kCEETokenID_DO                          )
"double"                                    RETURN_TOKEN_ID(kCEETokenID_DOUBLE                      )
"else"                                      RETURN_TOKEN_ID(kCEETokenID_ELSE                        )
"enum"                                      RETURN_TOKEN_ID(kCEETokenID_ENUM                        )
"event"                                     RETURN_TOKEN_ID(kCEETokenID_EVENT                       )
"explicit"                                  RETURN_TOKEN_ID(kCEETokenID_EXPLICIT                    )
"extern"                                    RETURN_TOKEN_ID(kCEETokenID_EXTERN                      )
"false"                                     RETURN_TOKEN_ID(kCEETokenID_FALSE                       )
"finally"                                   RETURN_TOKEN_ID(kCEETokenID_FINALLY                     )
"fixed"                                     RETURN_TOKEN_ID(kCEETokenID_FIXED                       )
"float"                                     RETURN_TOKEN_ID(kCEETokenID_FLOAT                       )
"for"                                       RETURN_TOKEN_ID(kCEETokenID_FOR                         )
"foreach"                                   RETURN_TOKEN_ID(kCEETokenID_FOREACH                     )
"goto"                                      RETURN_TOKEN_ID(kCEETokenID_GOTO                        )
"if"                                        RETURN_TOKEN_ID(kCEETokenID_IF                          )
"implicit"                                  RETURN_TOKEN_ID(kCEETokenID_IMPLICIT                    )
"in"                                        RETURN_TOKEN_ID(kCEETokenID_IN                          )
"int"                                       RETURN_TOKEN_ID(kCEETokenID_INT                         )
"interface"                                 RETURN_TOKEN_ID(kCEETokenID_INTERFACE                   )
"internal"                                  RETURN_TOKEN_ID(kCEETokenID_INTERNAL                    )
"is"                                        RETURN_TOKEN_ID(kCEETokenID_IS                          )
"lock"                                      RETURN_TOKEN_ID(kCEETokenID_LOCK                        )
"long"                                      RETURN_TOKEN_ID(kCEETokenID_LONG                        )
"namespace"                                 RETURN_TOKEN_ID(kCEETokenID_NAMESPACE                   )
"new"                                       RETURN_TOKEN_ID(kCEETokenID_NEW                         )
"null"                                      RETURN_TOKEN_ID(kCEETokenID_NULL                        )
"object"                                    RETURN_TOKEN_ID(kCEETokenID_OBJECT                      )
"operator"                                  RETURN_TOKEN_ID(kCEETokenID_OPERATOR                    )
"out"                                       RETURN_TOKEN_ID(kCEETokenID_OUT                         )
"override"                                  RETURN_TOKEN_ID(kCEETokenID_OVERRIDE                    )
"params"                                    RETURN_TOKEN_ID(kCEETokenID_PARAMS                      )
"private"                                   RETURN_TOKEN_ID(kCEETokenID_PRIVATE                     )
"protected"                                 RETURN_TOKEN_ID(kCEETokenID_PROTECTED                   )
"public"                                    RETURN_TOKEN_ID(kCEETokenID_PUBLIC                      )
"readonly"                                  RETURN_TOKEN_ID(kCEETokenID_READONLY                    )
"ref"                                       RETURN_TOKEN_ID(kCEETokenID_REF                         )
"return"                                    RETURN_TOKEN_ID(kCEETokenID_RETURN                      )
"sbyte"                                     RETURN_TOKEN_ID(kCEETokenID_SBYTE                       )
"sealed"                                    RETURN_TOKEN_ID(kCEETokenID_SEALED                      )
"short"                                     RETURN_TOKEN_ID(kCEETokenID_SHORT                       )
"sizeof"                                    RETURN_TOKEN_ID(kCEETokenID_SIZEOF                      )
"stackalloc"                                RETURN_TOKEN_ID(kCEETokenID_STACKALLOC                  )
"static"                                    RETURN_TOKEN_ID(kCEETokenID_STATIC                      )
"string"                                    RETURN_TOKEN_ID(kCEETokenID_STRING                      )
"struct"                                    RETURN_TOKEN_ID(kCEETokenID_STRUCT                      )
"switch"                                    RETURN_TOKEN_ID(kCEETokenID_SWITCH                      )
"this"                                      RETURN_TOKEN_ID(kCEETokenID_THIS                        )
"throw"                                     RETURN_TOKEN_ID(kCEETokenID_THROW                       )
"true"                                      RETURN_TOKEN_ID(kCEETokenID_TRUE                        )
"try"                                       RETURN_TOKEN_ID(kCEETokenID_TRY                         )
"typeof"                                    RETURN_TOKEN_ID(kCEETokenID_TYPEOF                      )
"uint"                                      RETURN_TOKEN_ID(kCEETokenID_UINT                        )
"ulong"                                     RETURN_TOKEN_ID(kCEETokenID_ULONG                       )
"unchecked"                                 RETURN_TOKEN_ID(kCEETokenID_UNCHECKED                   )
"unsafe"                                    RETURN_TOKEN_ID(kCEETokenID_UNSAFE                      )
"ushort"                                    RETURN_TOKEN_ID(kCEETokenID_USHORT                      )
"using"                                     RETURN_TOKEN_ID(kCEETokenID_USING                       )
"virtual"                                   RETURN_TOKEN_ID(kCEETokenID_VIRTUAL                     )
"void"                                      RETURN_TOKEN_ID(kCEETokenID_VOID                        )
"volatile"                                  RETURN_TOKEN_ID(kCEETokenID_VOLATILE                    )
"while"                                     RETURN_TOKEN_ID(kCEETokenID_WHILE                       )
"add"                                       RETURN_TOKEN_ID(kCEETokenID_ADD                         )
"alias"                                     RETURN_TOKEN_ID(kCEETokenID_ALIAS                       )
"ascending"                                 RETURN_TOKEN_ID(kCEETokenID_ASCENDING                   )
"async"                                     RETURN_TOKEN_ID(kCEETokenID_ASYNC                       )
"await"                                     RETURN_TOKEN_ID(kCEETokenID_AWAIT                       )
"by"                                        RETURN_TOKEN_ID(kCEETokenID_BY                          )
"descending"                                RETURN_TOKEN_ID(kCEETokenID_DESCENDING                  )
"dynamic"                                   RETURN_TOKEN_ID(kCEETokenID_DYNAMIC                     )
"equals"                                    RETURN_TOKEN_ID(kCEETokenID_EQUALS                      )
"from"                                      RETURN_TOKEN_ID(kCEETokenID_FROM                        )
"get"                                       RETURN_TOKEN_ID(kCEETokenID_GET                         )
"global"                                    RETURN_TOKEN_ID(kCEETokenID_GLOBAL                      )
"group"                                     RETURN_TOKEN_ID(kCEETokenID_GROUP                       )
"into"                                      RETURN_TOKEN_ID(kCEETokenID_INTO                        )
"join"                                      RETURN_TOKEN_ID(kCEETokenID_JOIN                        )
"let"                                       RETURN_TOKEN_ID(kCEETokenID_LET                         )
"nameof"                                    RETURN_TOKEN_ID(kCEETokenID_NAMEOF                      )
"notnull"                                   RETURN_TOKEN_ID(kCEETokenID_NOTNULL                     )
"on"                                        RETURN_TOKEN_ID(kCEETokenID_ON                          )
"orderby"                                   RETURN_TOKEN_ID(kCEETokenID_ORDERBY                     )
"partial"                                   RETURN_TOKEN_ID(kCEETokenID_PARTIAL                     )
"remove"                                    RETURN_TOKEN_ID(kCEETokenID_REMOVE                      )
"select"                                    RETURN_TOKEN_ID(kCEETokenID_SELECT                      )
"set"                                       RETURN_TOKEN_ID(kCEETokenID_SET                         )
"unmanaged"                                 RETURN_TOKEN_ID(kCEETokenID_UNMANAGED                   )
"value"                                     RETURN_TOKEN_ID(kCEETokenID_VALUE                       )
"var"                                       RETURN_TOKEN_ID(kCEETokenID_VAR                         )
"when"                                      RETURN_TOKEN_ID(kCEETokenID_WHEN                        )
"where"                                     RETURN_TOKEN_ID(kCEETokenID_WHERE                       )
"with"                                      RETURN_TOKEN_ID(kCEETokenID_WITH                        )
"yield"                                     RETURN_TOKEN_ID(kCEETokenID_YIELD                       )
"{"                                         RETURN_TOKEN_ID('{'                                     )
"}"                                         RETURN_TOKEN_ID('}'                                     )
"["                                         RETURN_TOKEN_ID('['                                     )
"]"                                         RETURN_TOKEN_ID(']'                                     )
"("                                         RETURN_TOKEN_ID('('                                     )
")"                                         RETURN_TOKEN_ID(')'                                     )
"."                                         RETURN_TOKEN_ID('.'                                     )
","                                         RETURN_TOKEN_ID(','                                     )
":"                                         RETURN_TOKEN_ID(':'                                     )
";"                                         RETURN_TOKEN_ID(';'                                     )
"+"                                         RETURN_TOKEN_ID('+'                                     )
"-"                                         RETURN_TOKEN_ID('-'                                     )
"*"                                         RETURN_TOKEN_ID('*'                                     )
"/"                                         RETURN_TOKEN_ID('/'                                     )
"%"                                         RETURN_TOKEN_ID('%'                                     )
"&"                                         RETURN_TOKEN_ID('&'                                     )
"|"                                         RETURN_TOKEN_ID('|'                                     )
"^"                                         RETURN_TOKEN_ID('^'                                     )
"!"                                         RETURN_TOKEN_ID('!'                                     )
"~"                                         RETURN_TOKEN_ID('~'                                     )
"="                                         RETURN_TOKEN_ID('='                                     )
"<"                                         RETURN_TOKEN_ID('<'                                     )
">"                                         RETURN_TOKEN_ID('>'                                     )
"?"                                         RETURN_TOKEN_ID('?'                                     )
"??"                                        RETURN_TOKEN_ID(kCEETokenID_NULL_COALESCING             )
"::"                                        RETURN_TOKEN_ID(kCEETokenID_SCOPE                       )
"++"                                        RETURN_TOKEN_ID(kCEETokenID_INCREMENT                   )
"--"                                        RETURN_TOKEN_ID(kCEETokenID_DECREMENT                   )
"&&"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_AND                   )
"||"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_OR                    )
"->"                                        RETURN_TOKEN_ID(kCEETokenID_MEMBER_POINTER              )
"=="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_EQUAL                 )
"!="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_UNEQUAL               )
"<="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LESS_EQUAL            )
">="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LARGE_EQUAL           )
"+="                                        RETURN_TOKEN_ID(kCEETokenID_ADD_ASSIGNMENT              )
"-="                                        RETURN_TOKEN_ID(kCEETokenID_MINUS_ASSIGNMENT            )
"*="                                        RETURN_TOKEN_ID(kCEETokenID_MULTI_ASSIGNMENT            )
"/="                                        RETURN_TOKEN_ID(kCEETokenID_DIV_ASSIGNMENT              )
"%="                                        RETURN_TOKEN_ID(kCEETokenID_MOD_ASSIGNMENT              )
"&="                                        RETURN_TOKEN_ID(kCEETokenID_AND_ASSIGNMENT              )
"|="                                        RETURN_TOKEN_ID(kCEETokenID_OR_ASSIGNMENT               )
"^="                                        RETURN_TOKEN_ID(kCEETokenID_XOR_ASSIGNMENT              )
"<<="                                       RETURN_TOKEN_ID(kCEETokenID_LEFT_OFFSET_ASSIGNMENT      )
"=>"                                        RETURN_TOKEN_ID(kCEETokenID_LAMBDA                      )
">>="                                       RETURN_TOKEN_ID(kCEETokenID_RIGHT_OFFSET_ASSIGNMENT     )
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_LINES_COMMENT               )
"//"                                        RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                )
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                     )
"'"                                         RETURN_TOKEN_ID(kCEETokenID_CHARACTER                   )
"#"{WS}*"define"                            RETURN_TOKEN_ID(kCEETokenID_HASH_DEFINE                 )
"#"{WS}*"if"                                RETURN_TOKEN_ID(kCEETokenID_HASH_IF                     )
"#"{WS}*"endif"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ENDIF                  )
"#"{WS}*"else"                              RETURN_TOKEN_ID(kCEETokenID_HASH_ELSE                   )
"#"{WS}*"elif"                              RETURN_TOKEN_ID(kCEETokenID_HASH_ELIF                   )
"#"{WS}*"undef"                             RETURN_TOKEN_ID(kCEETokenID_HASH_UNDEF                  )
"#"{WS}*"line"                              RETURN_TOKEN_ID(kCEETokenID_HASH_LINE                   )
"#"{WS}*"error"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ERROR                  )
"#"{WS}*"warning"                           RETURN_TOKEN_ID(kCEETokenID_HASH_WARNING                )
"#"{WS}*"pragma"                            RETURN_TOKEN_ID(kCEETokenID_HASH_PRAGMA                 )
"#"{WS}*"region"                            RETURN_TOKEN_ID(kCEETokenID_HASH_REGION                 )
"#"{WS}*"endregion"                         RETURN_TOKEN_ID(kCEETokenID_HASH_ENDREGION              )
0[xX]{H}+{IS}?                              RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+{IS}?                                   RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0b{B}+{IS}?                                 RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+{DE}{FS}?                               RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}*"."{D}+{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+"."{D}*{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}+{HE}{FS}?                          RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}*"."{H}+{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}+"."{H}*{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{L}({L}|{D})*                               RETURN_TOKEN_ID(kCEETokenID_IDENTIFIER                  )
{NL}                                        RETURN_TOKEN_ID(kCEETokenID_NEW_LINE                    )
{WS}                                        RETURN_TOKEN_ID(kCEETokenID_WHITE_SPACE                 )
.                                           RETURN_TOKEN_ID(kCEETokenID_UNKNOW                      )
%%

static int lines_comment(cee_long offset, 
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
        
        if (c0 == '\n') {
            line_no ++;
        }
        else if (c0 == '\r') {
            c1 = input();
            if (!c1) {
                *eof = TRUE;
                break;
            }
            else if (c1 != '\n') {
                line_no ++;
            }
            unput(c1);
        }
        
        
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

static int line_comment(cee_long offset, 
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
    int c0, c1;;
    
    do {
        c0 = input();
        if (!c0) {
            *eof = TRUE;
            break;
        }
        
        length ++;
        
        if (c0 == '\n') {
            line_no ++;
        }
        else if (c0 == '\r') {
            c1 = input();
            if (!c1) {
                *eof = TRUE;
                break;
            }
            else if (c1 != '\n') {
                line_no ++;
            }
            unput(c1);
        }
        
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
    int c0, c1;
    
    do {
        c0 = input();
        if (!c0) {
            *eof = TRUE;
            break;
        }
        
        length ++;
        
        if (c0 == '\n') {
            line_no ++;
        }
        else if (c0 == '\r') {
            c1 = input();
            if (!c1) {
                *eof = TRUE;
                break;
            }
            else if (c1 != '\n') {
                line_no ++;
            }
            unput(c1);
        }
        
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

cee_int cee_lexer_csharp_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = csharp_scan_lex();
    cee_long offset = scan_offset;
    cee_ulong length = csharp_scan_get_leng();
    
    scan_offset += length;
    
    *token = cee_token_create(identifier, offset, length, line_no);
    
    if (identifier == kCEETokenID_NEW_LINE)
        line_no ++;
        
    if (!identifier) /* EOF */
        return 0;
    
    if (identifier == kCEETokenID_LINES_COMMENT) {
        tail_length = lines_comment(offset, &eof);
        (*token)->length += tail_length;
        scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LINE_COMMENT) {
        tail_length = line_comment(offset, &eof);
        (*token)->length += tail_length;
        scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LITERAL) {
        tail_length = literal(offset, &eof);
        (*token)->length += tail_length;
        scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_CHARACTER) {
        tail_length = character(offset, &eof);
        (*token)->length += tail_length;
        scan_offset += tail_length;
    }
    
    if (eof)
        return 0;
    
    return 1;
}

void cee_lexer_csharp_buffer_create(const cee_char* subject)
{
    buffer = csharp_scan__scan_bytes(subject, strlen(subject));
    scan_offset = 0;
    line_no = 0;
}

void cee_lexer_csharp_buffer_free()
{
    yy_delete_buffer(buffer);
}

