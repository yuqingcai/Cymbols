%option yylineno
%option noyywrap
%option prefix="java_scan_"
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

static cee_ulong scan_offset = 0;
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
"abstract"                                  RETURN_TOKEN_ID(kCEETokenID_ABSTRACT                )
"assert"                                    RETURN_TOKEN_ID(kCEETokenID_ASSERT                  )
"boolean"                                   RETURN_TOKEN_ID(kCEETokenID_BOOLEAN                 )
"break"                                     RETURN_TOKEN_ID(kCEETokenID_BREAK                   )
"byte"                                      RETURN_TOKEN_ID(kCEETokenID_BYTE                    )
"case"                                      RETURN_TOKEN_ID(kCEETokenID_CASE                    )
"catch"                                     RETURN_TOKEN_ID(kCEETokenID_CATCH                   )
"char"                                      RETURN_TOKEN_ID(kCEETokenID_CHAR                    )
"class"                                     RETURN_TOKEN_ID(kCEETokenID_CLASS                   )
"continue"                                  RETURN_TOKEN_ID(kCEETokenID_CONTINUE                )
"const"                                     RETURN_TOKEN_ID(kCEETokenID_CONST                   )
"default"                                   RETURN_TOKEN_ID(kCEETokenID_DEFAULT                 )
"do"                                        RETURN_TOKEN_ID(kCEETokenID_DO                      )
"double"                                    RETURN_TOKEN_ID(kCEETokenID_DOUBLE                  )
"else"                                      RETURN_TOKEN_ID(kCEETokenID_ELSE                    )
"enum"                                      RETURN_TOKEN_ID(kCEETokenID_ENUM                    )
"exports"                                   RETURN_TOKEN_ID(kCEETokenID_EXPORTS                 )
"extends"                                   RETURN_TOKEN_ID(kCEETokenID_EXTENDS                 )
"final"                                     RETURN_TOKEN_ID(kCEETokenID_FINAL                   )
"finally"                                   RETURN_TOKEN_ID(kCEETokenID_FINALLY                 )
"float"                                     RETURN_TOKEN_ID(kCEETokenID_FLOAT                   )
"for"                                       RETURN_TOKEN_ID(kCEETokenID_FOR                     )
"goto"                                      RETURN_TOKEN_ID(kCEETokenID_GOTO                    )
"if"                                        RETURN_TOKEN_ID(kCEETokenID_IF                      )
"implements"                                RETURN_TOKEN_ID(kCEETokenID_IMPLEMENTS              )
"import"                                    RETURN_TOKEN_ID(kCEETokenID_IMPORT                  )
"instanceof"                                RETURN_TOKEN_ID(kCEETokenID_INSTANCEOF              )
"int"                                       RETURN_TOKEN_ID(kCEETokenID_INT                     )
"interface"                                 RETURN_TOKEN_ID(kCEETokenID_INTERFACE               )
"@"{WN}*"interface"                         RETURN_TOKEN_ID(kCEETokenID_INTERFACE               )
"long"                                      RETURN_TOKEN_ID(kCEETokenID_LONG                    )
"module"                                    RETURN_TOKEN_ID(kCEETokenID_MODULE                  )
"native"                                    RETURN_TOKEN_ID(kCEETokenID_NATIVE                  )
"new"                                       RETURN_TOKEN_ID(kCEETokenID_NEW                     )
"package"                                   RETURN_TOKEN_ID(kCEETokenID_PACKAGE                 )
"private"                                   RETURN_TOKEN_ID(kCEETokenID_PRIVATE                 )
"protected"                                 RETURN_TOKEN_ID(kCEETokenID_PROTECTED               )
"public"                                    RETURN_TOKEN_ID(kCEETokenID_PUBLIC                  )
"requires"                                  RETURN_TOKEN_ID(kCEETokenID_REQUIRES                )
"return"                                    RETURN_TOKEN_ID(kCEETokenID_RETURN                  )
"short"                                     RETURN_TOKEN_ID(kCEETokenID_SHORT                   )
"static"                                    RETURN_TOKEN_ID(kCEETokenID_STATIC                  )
"strictfp"                                  RETURN_TOKEN_ID(kCEETokenID_STRICTFP                )
"super"                                     RETURN_TOKEN_ID(kCEETokenID_SUPER                   )
"switch"                                    RETURN_TOKEN_ID(kCEETokenID_SWITCH                  )
"synchronized"                              RETURN_TOKEN_ID(kCEETokenID_SYNCHRONIZED            )
"this"                                      RETURN_TOKEN_ID(kCEETokenID_THIS                    )
"throw"                                     RETURN_TOKEN_ID(kCEETokenID_THROW                   )
"throws"                                    RETURN_TOKEN_ID(kCEETokenID_THROWS                  )
"transient"                                 RETURN_TOKEN_ID(kCEETokenID_TRANSIENT               )
"try"                                       RETURN_TOKEN_ID(kCEETokenID_TRY                     )
"var"                                       RETURN_TOKEN_ID(kCEETokenID_VAR                     )
"void"                                      RETURN_TOKEN_ID(kCEETokenID_VOID                    )
"volatile"                                  RETURN_TOKEN_ID(kCEETokenID_VOLATILE                )
"while"                                     RETURN_TOKEN_ID(kCEETokenID_WHILE                   )
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_LINES_COMMENT           )
"//"                                        RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT            )
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                 )
"'"                                         RETURN_TOKEN_ID(kCEETokenID_CHARACTER               )
"="                                         RETURN_TOKEN_ID('='                                 )
"+"                                         RETURN_TOKEN_ID('+'                                 )
"-"                                         RETURN_TOKEN_ID('-'                                 )
"*"                                         RETURN_TOKEN_ID('*'                                 )
"/"                                         RETURN_TOKEN_ID('/'                                 )
"\\"                                        RETURN_TOKEN_ID('\\'                                )
"%"                                         RETURN_TOKEN_ID('%'                                 )
"~"                                         RETURN_TOKEN_ID('~'                                 )
"&"                                         RETURN_TOKEN_ID('&'                                 )
"|"                                         RETURN_TOKEN_ID('|'                                 )
"^"                                         RETURN_TOKEN_ID('^'                                 )
"!"                                         RETURN_TOKEN_ID('!'                                 )
"<"                                         RETURN_TOKEN_ID('<'                                 )
">"                                         RETURN_TOKEN_ID('>'                                 )
"."                                         RETURN_TOKEN_ID('.'                                 )
","                                         RETURN_TOKEN_ID(','                                 )
"?"                                         RETURN_TOKEN_ID('?'                                 )
":"                                         RETURN_TOKEN_ID(':'                                 )
"("                                         RETURN_TOKEN_ID('('                                 )
")"                                         RETURN_TOKEN_ID(')'                                 )
"{"                                         RETURN_TOKEN_ID('{'                                 )
"}"                                         RETURN_TOKEN_ID('}'                                 )
"["                                         RETURN_TOKEN_ID('['                                 )
"]"                                         RETURN_TOKEN_ID(']'                                 )
";"                                         RETURN_TOKEN_ID(';'                                 )
"@"                                         RETURN_TOKEN_ID('@'                                 )
"."                                         RETURN_TOKEN_ID('.'                                 )
"..."                                       RETURN_TOKEN_ID(kCEETokenID_ELLIPSIS                )
"+="                                        RETURN_TOKEN_ID(kCEETokenID_ADD_ASSIGNMENT          )
"-="                                        RETURN_TOKEN_ID(kCEETokenID_MINUS_ASSIGNMENT        )
"*="                                        RETURN_TOKEN_ID(kCEETokenID_MULTI_ASSIGNMENT        )
"/="                                        RETURN_TOKEN_ID(kCEETokenID_DIV_ASSIGNMENT          )
"%="                                        RETURN_TOKEN_ID(kCEETokenID_MOD_ASSIGNMENT          )
"&="                                        RETURN_TOKEN_ID(kCEETokenID_AND_ASSIGNMENT          )
"|="                                        RETURN_TOKEN_ID(kCEETokenID_OR_ASSIGNMENT           )
"^="                                        RETURN_TOKEN_ID(kCEETokenID_XOR_ASSIGNMENT          )
"<<="                                       RETURN_TOKEN_ID(kCEETokenID_LEFT_OFFSET_ASSIGNMENT  )
">>="                                       RETURN_TOKEN_ID(kCEETokenID_RIGHT_OFFSET_ASSIGNMENT )
"++"                                        RETURN_TOKEN_ID(kCEETokenID_INCREMENT               )
"--"                                        RETURN_TOKEN_ID(kCEETokenID_DECREMENT               )
"&&"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_AND               )
"||"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_OR                )
"=="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_EQUAL             )
"!="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_UNEQUAL           )
"<="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LESS_EQUAL        )
">="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LARGE_EQUAL       )
"@"{WN}*{L}({L}|{D})*                       RETURN_TOKEN_ID(kCEETokenID_ANNOTATION              )
0[xX]{H}+{IS}?                              RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
{D}+{IS}?                                   RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
0b{B}+{IS}?                                 RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
{D}+{DE}{FS}?                               RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
{D}*"."{D}+{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
{D}+"."{D}*{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
0[xX]{H}+{HE}{FS}?                          RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
0[xX]{H}*"."{H}+{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
0[xX]{H}+"."{H}*{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                )
{L}({L}|{D})*                               RETURN_TOKEN_ID(kCEETokenID_IDENTIFIER              )
{NL}                                        RETURN_TOKEN_ID(kCEETokenID_NEW_LINE                )
{WS}                                        RETURN_TOKEN_ID(kCEETokenID_WHITE_SPACE             )
.                                           RETURN_TOKEN_ID(kCEETokenID_UNKNOW                  )
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

cee_int cee_lexer_java_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = java_scan_lex();
    cee_long offset = scan_offset;
    cee_ulong length = java_scan_get_leng();
    
    scan_offset += length;
    
    *token = cee_token_create(identifier, offset, length);
    
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
void cee_lexer_java_buffer_create(const cee_char* subject)
{
    buffer = java_scan__scan_bytes(subject, strlen(subject));
    scan_offset = 0;
}

void cee_lexer_java_buffer_free()
{
    yy_delete_buffer(buffer);
}

