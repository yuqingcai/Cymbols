%option yylineno
%option noyywrap
%option prefix="swift_scan_"
%x Preprocesser
L   [a-zA-Z_]
H   [a-fA-F0-9]
D   [0-9]
O   [0-7]
B   [01]
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

static cee_ulong g_scan_offset = 0;
static YY_BUFFER_STATE buffer = NULL;

#define FORWARD_SCAN_CHARACTER_MAX 1024

#define RETURN_TOKEN_ID(token_id) {\
    return token_id;\
}

static int forward_buffer[FORWARD_SCAN_CHARACTER_MAX];

static int lines_comment(cee_long offset, 
                        cee_boolean* eof);
static int line_comment(cee_long offset, 
                        cee_boolean* eof);
static int literal(cee_long offset, 
                   cee_boolean* eof);
static int multi_literal(cee_long offset, 
                         cee_boolean* eof);
                        
%}

%%
"associatedtype"                            RETURN_TOKEN_ID(kCEETokenID_ASSOCIATEDTYPE              )
"class"                                     RETURN_TOKEN_ID(kCEETokenID_CLASS                       )
"deinit"                                    RETURN_TOKEN_ID(kCEETokenID_DEINIT                      )
"enum"                                      RETURN_TOKEN_ID(kCEETokenID_ENUM                        )
"extension"                                 RETURN_TOKEN_ID(kCEETokenID_EXTENSION                   )
"fileprivate"                               RETURN_TOKEN_ID(kCEETokenID_FILEPRIVATE                 )
"func"                                      RETURN_TOKEN_ID(kCEETokenID_FUNC                        )
"import"                                    RETURN_TOKEN_ID(kCEETokenID_IMPORT                      )
"init"                                      RETURN_TOKEN_ID(kCEETokenID_INIT                        )
"inout"                                     RETURN_TOKEN_ID(kCEETokenID_INOUT                       )
"internal"                                  RETURN_TOKEN_ID(kCEETokenID_INTERNAL                    )
"let"                                       RETURN_TOKEN_ID(kCEETokenID_LET                         )
"open"                                      RETURN_TOKEN_ID(kCEETokenID_OPEN                        )
"operator"                                  RETURN_TOKEN_ID(kCEETokenID_OPERATOR                    )
"private"                                   RETURN_TOKEN_ID(kCEETokenID_PRIVATE                     )
"protocol"                                  RETURN_TOKEN_ID(kCEETokenID_PROTOCOL                    )
"public"                                    RETURN_TOKEN_ID(kCEETokenID_PUBLIC                      )
"rethrows"                                  RETURN_TOKEN_ID(kCEETokenID_RETHROWS                    )
"static"                                    RETURN_TOKEN_ID(kCEETokenID_STATIC                      )
"struct"                                    RETURN_TOKEN_ID(kCEETokenID_STRUCT                      )
"subscript"                                 RETURN_TOKEN_ID(kCEETokenID_SUBSCRIPT                   )
"typealias"                                 RETURN_TOKEN_ID(kCEETokenID_TYPEALIAS                   )
"var"                                       RETURN_TOKEN_ID(kCEETokenID_VAR                         )
"break"                                     RETURN_TOKEN_ID(kCEETokenID_BREAK                       )
"case"                                      RETURN_TOKEN_ID(kCEETokenID_CASE                        )
"continue"                                  RETURN_TOKEN_ID(kCEETokenID_CONTINUE                    )
"default"                                   RETURN_TOKEN_ID(kCEETokenID_DEFAULT                     )
"defer"                                     RETURN_TOKEN_ID(kCEETokenID_DEFER                       )
"do"                                        RETURN_TOKEN_ID(kCEETokenID_DO                          )
"else"                                      RETURN_TOKEN_ID(kCEETokenID_ELSE                        )
"fallthrough"                               RETURN_TOKEN_ID(kCEETokenID_FALLTHROUGH                 )
"for"                                       RETURN_TOKEN_ID(kCEETokenID_FOR                         )
"guard"                                     RETURN_TOKEN_ID(kCEETokenID_GUARD                       )
"if"                                        RETURN_TOKEN_ID(kCEETokenID_IF                          )
"in"                                        RETURN_TOKEN_ID(kCEETokenID_IN                          )
"repeat"                                    RETURN_TOKEN_ID(kCEETokenID_REPEAT                      )
"return"                                    RETURN_TOKEN_ID(kCEETokenID_RETURN                      )
"switch"                                    RETURN_TOKEN_ID(kCEETokenID_SWITCH                      )
"where"                                     RETURN_TOKEN_ID(kCEETokenID_WHERE                       )
"while"                                     RETURN_TOKEN_ID(kCEETokenID_WHILE                       )
"as"                                        RETURN_TOKEN_ID(kCEETokenID_AS                          )
"Any"                                       RETURN_TOKEN_ID(kCEETokenID_ANY                         )
"catch"                                     RETURN_TOKEN_ID(kCEETokenID_CATCH                       )
"false"                                     RETURN_TOKEN_ID(kCEETokenID_FALSE                       )
"is"                                        RETURN_TOKEN_ID(kCEETokenID_IS                          )
"nil"                                       RETURN_TOKEN_ID(kCEETokenID_NIL                         )
"super"                                     RETURN_TOKEN_ID(kCEETokenID_SUPER                       )
"self"                                      RETURN_TOKEN_ID(kCEETokenID_SELF                        )
"Self"                                      RETURN_TOKEN_ID(kCEETokenID_CAPITAL_SELF                )
"throw"                                     RETURN_TOKEN_ID(kCEETokenID_THROW                       )
"throws"                                    RETURN_TOKEN_ID(kCEETokenID_THROWS                      )
"true"                                      RETURN_TOKEN_ID(kCEETokenID_TRUE                        )
"try"                                       RETURN_TOKEN_ID(kCEETokenID_TRY                         )
"_."                                        RETURN_TOKEN_ID(kCEETokenID_UNDERLINE_DOT               )
"@"{WN}*{L}({L}|{D})*                       RETURN_TOKEN_ID(kCEETokenID_AT_IDENTIFIER               )
"#"{WS}*"available"                         RETURN_TOKEN_ID(kCEETokenID_HASH_AVAILABLE              )
"#"{WS}*"colorLiteral"                      RETURN_TOKEN_ID(kCEETokenID_HASH_COLORLITERAL           )
"#"{WS}*"column"                            RETURN_TOKEN_ID(kCEETokenID_HASH_COLUMN                 )
"#"{WS}*"else"                              RETURN_TOKEN_ID(kCEETokenID_HASH_ELSE                   )
"#"{WS}*"elseif"                            RETURN_TOKEN_ID(kCEETokenID_HASH_ELSEIF                 )
"#"{WS}*"endif"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ENDIF                  )
"#"{WS}*"error"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ERROR                  )
"#"{WS}*"file"                              RETURN_TOKEN_ID(kCEETokenID_HASH_FILE                   )
"#"{WS}*"fileID"                            RETURN_TOKEN_ID(kCEETokenID_HASH_FILEID                 )
"#"{WS}*"fileLiteral"                       RETURN_TOKEN_ID(kCEETokenID_HASH_FILELITERAL            )
"#"{WS}*"filePath"                          RETURN_TOKEN_ID(kCEETokenID_HASH_FILEPATH               )
"#"{WS}*"function"                          RETURN_TOKEN_ID(kCEETokenID_HASH_FUNCTION               )
"#"{WS}*"if"                                RETURN_TOKEN_ID(kCEETokenID_HASH_IF                     )
"#"{WS}*"imageLiteral"                      RETURN_TOKEN_ID(kCEETokenID_HASH_IMAGELITERAL           )
"#"{WS}*"line"                              RETURN_TOKEN_ID(kCEETokenID_HASH_LINE                   )
"#"{WS}*"selector"                          RETURN_TOKEN_ID(kCEETokenID_HASH_SELECTOR               )
"#"{WS}*"sourceLocation"                    RETURN_TOKEN_ID(kCEETokenID_HASH_SOURCELOCATION         )
"#"{WS}*"warning"                           RETURN_TOKEN_ID(kCEETokenID_HASH_WARNING                )
"associativity"                             RETURN_TOKEN_ID(kCEETokenID_ASSOCIATIVITY               )
"convenience"                               RETURN_TOKEN_ID(kCEETokenID_CONVENIENCE                 )
"dynamic"                                   RETURN_TOKEN_ID(kCEETokenID_DYNAMIC                     )
"didSet"                                    RETURN_TOKEN_ID(kCEETokenID_DIDSET                      )
"final"                                     RETURN_TOKEN_ID(kCEETokenID_FINAL                       )
"get"                                       RETURN_TOKEN_ID(kCEETokenID_GET                         )
"infix"                                     RETURN_TOKEN_ID(kCEETokenID_INFIX                       )
"indirect"                                  RETURN_TOKEN_ID(kCEETokenID_INDIRECT                    )
"lazy"                                      RETURN_TOKEN_ID(kCEETokenID_LAZY                        )
"left"                                      RETURN_TOKEN_ID(kCEETokenID_LEFT                        )
"mutating"                                  RETURN_TOKEN_ID(kCEETokenID_MUTATING                    )
"none"                                      RETURN_TOKEN_ID(kCEETokenID_NONE                        )
"nonmutating"                               RETURN_TOKEN_ID(kCEETokenID_NONMUTATING                 )
"optional"                                  RETURN_TOKEN_ID(kCEETokenID_OPTIONAL                    )
"override"                                  RETURN_TOKEN_ID(kCEETokenID_OVERRIDE                    )
"postfix"                                   RETURN_TOKEN_ID(kCEETokenID_POSTFIX                     )
"precedence"                                RETURN_TOKEN_ID(kCEETokenID_PRECEDENCE                  )
"prefix"                                    RETURN_TOKEN_ID(kCEETokenID_PREFIX                      )
"Protocol"                                  RETURN_TOKEN_ID(kCEETokenID_CAPITAL_PROTOCOL            )
"required"                                  RETURN_TOKEN_ID(kCEETokenID_REQUIRED                    )
"right"                                     RETURN_TOKEN_ID(kCEETokenID_RIGHT                       )
"set"                                       RETURN_TOKEN_ID(kCEETokenID_SET                         )
"Type"                                      RETURN_TOKEN_ID(kCEETokenID_CAPITAL_TYPE                )
"unowned"                                   RETURN_TOKEN_ID(kCEETokenID_UNOWNED                     )
"weak"                                      RETURN_TOKEN_ID(kCEETokenID_WEAK                        )
"willSet"                                   RETURN_TOKEN_ID(kCEETokenID_WILLSET                     )
"++"                                        RETURN_TOKEN_ID(kCEETokenID_INCREMENT                   )
"--"                                        RETURN_TOKEN_ID(kCEETokenID_DECREMENT                   )
"+"                                         RETURN_TOKEN_ID('+'                                     )
"-"                                         RETURN_TOKEN_ID('-'                                     )
"!"                                         RETURN_TOKEN_ID('!'                                     )
"~"                                         RETURN_TOKEN_ID('~'                                     )
"*"                                         RETURN_TOKEN_ID('*'                                     )
"/"                                         RETURN_TOKEN_ID('/'                                     )
"%"                                         RETURN_TOKEN_ID('%'                                     )
"&*"                                        RETURN_TOKEN_ID(kCEETokenID_AND_MULTI                   )
"&/"                                        RETURN_TOKEN_ID(kCEETokenID_AND_DIV                     )
"&%"                                        RETURN_TOKEN_ID(kCEETokenID_AND_MOD                     )
"&"                                         RETURN_TOKEN_ID('&'                                     )
"&+"                                        RETURN_TOKEN_ID(kCEETokenID_AND_ADD                     )
"&-"                                        RETURN_TOKEN_ID(kCEETokenID_AND_MINUS                   )
"|"                                         RETURN_TOKEN_ID('|'                                     )
"^"                                         RETURN_TOKEN_ID('^'                                     )
"..<"                                       RETURN_TOKEN_ID(kCEETokenID_HALF_OPEN_RANGE             )
"..."                                       RETURN_TOKEN_ID(kCEETokenID_ELLIPSIS                    )
"<"                                         RETURN_TOKEN_ID('<'                                     )
"<="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LESS_EQUAL            )
">"                                         RETURN_TOKEN_ID('>'                                     )
">="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LARGE_EQUAL           )
"=="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_EQUAL                 )
"!="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_UNEQUAL               )
"==="                                       RETURN_TOKEN_ID(kCEETokenID_LOGIC_IDENTICALLY_EQUAL     )
"!=="                                       RETURN_TOKEN_ID(kCEETokenID_LOGIC_IDENTICALLY_UNEQUAL   )
"~="                                        RETURN_TOKEN_ID(kCEETokenID_PATTERN_MATCH               )
"&&"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_AND                   )
"||"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_OR                    )
"??"                                        RETURN_TOKEN_ID(kCEETokenID_NIL_COALESCING              )
"="                                         RETURN_TOKEN_ID('='                                     )
"*="                                        RETURN_TOKEN_ID(kCEETokenID_MULTI_ASSIGNMENT            )
"/="                                        RETURN_TOKEN_ID(kCEETokenID_DIV_ASSIGNMENT              )
"%="                                        RETURN_TOKEN_ID(kCEETokenID_MOD_ASSIGNMENT              )
"+="                                        RETURN_TOKEN_ID(kCEETokenID_ADD_ASSIGNMENT              )
"-="                                        RETURN_TOKEN_ID(kCEETokenID_MINUS_ASSIGNMENT            )
"<<="                                       RETURN_TOKEN_ID(kCEETokenID_LEFT_SHIFT_ASSIGNMENT       )
">>="                                       RETURN_TOKEN_ID(kCEETokenID_RIGHT_SHIFT_ASSIGNMENT      )
"&="                                        RETURN_TOKEN_ID(kCEETokenID_AND_ASSIGNMENT              )
"^="                                        RETURN_TOKEN_ID(kCEETokenID_XOR_ASSIGNMENT              )
"|="                                        RETURN_TOKEN_ID(kCEETokenID_OR_ASSIGNMENT               )
"."                                         RETURN_TOKEN_ID('.'                                     )
"["                                         RETURN_TOKEN_ID('['                                     )
"]"                                         RETURN_TOKEN_ID(']'                                     )
"("                                         RETURN_TOKEN_ID('('                                     )
")"                                         RETURN_TOKEN_ID(')'                                     )
"{"                                         RETURN_TOKEN_ID('{'                                     )
"}"                                         RETURN_TOKEN_ID('}'                                     )
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_LINES_COMMENT               )
"//"                                        RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                )
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                     )
"\"\"\""                                    RETURN_TOKEN_ID(kCEETokenID_MULTI_LITERAL               )
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
    int comments = 1;
    
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
                
                comments --;
                if (!comments)
                    break;
            }
            else
                unput(c1);
        }
        else if (c0 == '/') {
            c1 = input();
            
            if (!c1) {
                *eof = TRUE;
                break;
            }
            
            if (c1 == '*') {
                length ++;
                comments ++;
            }
            else 
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

static int forward_input(int count)
{
    int c;
    int i;
    
    i = 0;
    while (count) {
        c = input();
        
        if (!c) 
            break;
        
        forward_buffer[i] = c;
        i ++;
        count --;
    }
        
    return i;
}

static void forward_unput(int count)
{
    do {
        count --;
        unput(forward_buffer[count]);
    } while (count);
}

static int multi_literal(cee_long offset,
                         cee_boolean* eof)
{
    int length = 0;
    int c;
    
    do {
        c = input();
        if (!c) {
            *eof = TRUE;
            break;
        }
        
        length ++;
        
        /** detect """ */
        if (c == '\"') {
            
            /** check if next 2 character are "\"\"" */
            int k = forward_input(2);
            length += k;
            
            if (k < 2) {
                *eof = TRUE;
                break;
            }
            
            if (forward_buffer[0] == '\"' &&
                forward_buffer[1] == '\"') {
                break;
            }
            else {
                forward_unput(2);
                length -= k;
            }
        }
        
    } while (1);
    
    return length;
}

cee_int cee_lexer_swift_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = swift_scan_lex();
    cee_long offset = g_scan_offset;
    cee_ulong length = swift_scan_get_leng();
    
    g_scan_offset += length;
    
    *token = cee_token_create(identifier, offset, length);
    
    if (!identifier) /* EOF */
        return 0;

    if (identifier == kCEETokenID_LINES_COMMENT) {
        tail_length = lines_comment(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LINE_COMMENT) {
        tail_length = line_comment(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LITERAL) {
        tail_length = literal(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_MULTI_LITERAL) {
        tail_length = multi_literal(offset, &eof);
        (*token)->length += tail_length;
        g_scan_offset += tail_length;
    }
    
    if (eof)
        return 0;
        
    return 1;
}
void cee_lexer_swift_buffer_create(const cee_uchar* subject)
{
    buffer = swift_scan__scan_bytes(subject, strlen(subject));
    g_scan_offset = 0;
}

void cee_lexer_swift_buffer_free()
{
    yy_delete_buffer(buffer);
}

