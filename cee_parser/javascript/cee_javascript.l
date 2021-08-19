%option yylineno
%option noyywrap
%option prefix="js_scan_"
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

static cee_boolean in_tag = FALSE;
static cee_ulong scan_offset = 0;
static cee_int line_no = 0;
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
"abstract"                                  RETURN_TOKEN_ID(kCEETokenID_ABSTRACT                        )
"arguments"                                 RETURN_TOKEN_ID(kCEETokenID_ARGUMENTS                       )
"await"                                     RETURN_TOKEN_ID(kCEETokenID_AWAIT                           )
"boolean"                                   RETURN_TOKEN_ID(kCEETokenID_BOOLEAN                         )
"break"                                     RETURN_TOKEN_ID(kCEETokenID_BREAK                           )
"byte"                                      RETURN_TOKEN_ID(kCEETokenID_BYTE                            )
"case"                                      RETURN_TOKEN_ID(kCEETokenID_CASE                            )
"catch"                                     RETURN_TOKEN_ID(kCEETokenID_CATCH                           )
"char"                                      RETURN_TOKEN_ID(kCEETokenID_CHAR                            )
"class"                                     RETURN_TOKEN_ID(kCEETokenID_CLASS                           )
"const"                                     RETURN_TOKEN_ID(kCEETokenID_CONST                           )
"continue"                                  RETURN_TOKEN_ID(kCEETokenID_CONTINUE                        )
"debugger"                                  RETURN_TOKEN_ID(kCEETokenID_DEBUGGER                        )
"default"                                   RETURN_TOKEN_ID(kCEETokenID_DEFAULT                         )
"delete"                                    RETURN_TOKEN_ID(kCEETokenID_DELETE                          )
"do"                                        RETURN_TOKEN_ID(kCEETokenID_DO                              )
"double"                                    RETURN_TOKEN_ID(kCEETokenID_DOUBLE                          )
"else"                                      RETURN_TOKEN_ID(kCEETokenID_ELSE                            )
"enum"                                      RETURN_TOKEN_ID(kCEETokenID_ENUM                            )
"eval"                                      RETURN_TOKEN_ID(kCEETokenID_EVAL                            )
"export"                                    RETURN_TOKEN_ID(kCEETokenID_EXPORT                          )
"extends"                                   RETURN_TOKEN_ID(kCEETokenID_EXTENDS                         )
"false"                                     RETURN_TOKEN_ID(kCEETokenID_FALSE                           )
"final"                                     RETURN_TOKEN_ID(kCEETokenID_FINAL                           )
"finally"                                   RETURN_TOKEN_ID(kCEETokenID_FINALLY                         )
"float"                                     RETURN_TOKEN_ID(kCEETokenID_FLOAT                           )
"for"                                       RETURN_TOKEN_ID(kCEETokenID_FOR                             )
"function"                                  RETURN_TOKEN_ID(kCEETokenID_FUNCTION                        )
"goto"                                      RETURN_TOKEN_ID(kCEETokenID_GOTO                            )
"if"                                        RETURN_TOKEN_ID(kCEETokenID_IF                              )
"implements"                                RETURN_TOKEN_ID(kCEETokenID_IMPLEMENTS                      )
"import"                                    RETURN_TOKEN_ID(kCEETokenID_IMPORT                          )
"in"                                        RETURN_TOKEN_ID(kCEETokenID_IN                              )
"instanceof"                                RETURN_TOKEN_ID(kCEETokenID_INSTANCEOF                      )
"int"                                       RETURN_TOKEN_ID(kCEETokenID_INT                             )
"interface"                                 RETURN_TOKEN_ID(kCEETokenID_INTERFACE                       )
"let"                                       RETURN_TOKEN_ID(kCEETokenID_LET                             )
"long"                                      RETURN_TOKEN_ID(kCEETokenID_LONG                            )
"native"                                    RETURN_TOKEN_ID(kCEETokenID_NATIVE                          )
"new"                                       RETURN_TOKEN_ID(kCEETokenID_NEW                             )
"null"                                      RETURN_TOKEN_ID(kCEETokenID_NULL                            )
"package"                                   RETURN_TOKEN_ID(kCEETokenID_PACKAGE                         )
"private"                                   RETURN_TOKEN_ID(kCEETokenID_PRIVATE                         )
"protected"                                 RETURN_TOKEN_ID(kCEETokenID_PROTECTED                       )
"public"                                    RETURN_TOKEN_ID(kCEETokenID_PUBLIC                          )
"return"                                    RETURN_TOKEN_ID(kCEETokenID_RETURN                          )
"short"                                     RETURN_TOKEN_ID(kCEETokenID_SHORT                           )
"static"                                    RETURN_TOKEN_ID(kCEETokenID_STATIC                          )
"super"                                     RETURN_TOKEN_ID(kCEETokenID_SUPER                           )
"switch"                                    RETURN_TOKEN_ID(kCEETokenID_SWITCH                          )
"synchronized"                              RETURN_TOKEN_ID(kCEETokenID_SYNCHRONIZED                    )
"this"                                      RETURN_TOKEN_ID(kCEETokenID_THIS                            )
"throw"                                     RETURN_TOKEN_ID(kCEETokenID_THROW                           )
"throws"                                    RETURN_TOKEN_ID(kCEETokenID_THROWS                          )
"transient"                                 RETURN_TOKEN_ID(kCEETokenID_TRANSIENT                       )
"true"                                      RETURN_TOKEN_ID(kCEETokenID_TRUE                            )
"try"                                       RETURN_TOKEN_ID(kCEETokenID_TRY                             )
"typeof"                                    RETURN_TOKEN_ID(kCEETokenID_TYPEOF                          )
"var"                                       RETURN_TOKEN_ID(kCEETokenID_VAR                             )
"void"                                      RETURN_TOKEN_ID(kCEETokenID_VOID                            )
"volatile"                                  RETURN_TOKEN_ID(kCEETokenID_VOLATILE                        )
"while"                                     RETURN_TOKEN_ID(kCEETokenID_WHILE                           )
"with"                                      RETURN_TOKEN_ID(kCEETokenID_WITH                            )
"yield"                                     RETURN_TOKEN_ID(kCEETokenID_YIELD                           )
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_LINES_COMMENT                   )
"//"                                        RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                    )
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                         )
"'"                                         RETURN_TOKEN_ID(kCEETokenID_CHARACTER                       )
"+"                                         RETURN_TOKEN_ID('+'                                         )
"-"                                         RETURN_TOKEN_ID('-'                                         )
"*"                                         RETURN_TOKEN_ID('*'                                         )
"**"                                        RETURN_TOKEN_ID(kCEETokenID_EXP                             )
"/"                                         RETURN_TOKEN_ID('/'                                         )
"%"                                         RETURN_TOKEN_ID('%'                                         )
"++"                                        RETURN_TOKEN_ID(kCEETokenID_INCREMENT                       )
"--"                                        RETURN_TOKEN_ID(kCEETokenID_DECREMENT                       )
"="                                         RETURN_TOKEN_ID('='                                         )
"+="                                        RETURN_TOKEN_ID(kCEETokenID_ADD_ASSIGNMENT                  )
"-="                                        RETURN_TOKEN_ID(kCEETokenID_MINUS_ASSIGNMENT                )
"*="                                        RETURN_TOKEN_ID(kCEETokenID_MULTI_ASSIGNMENT                )
"/="                                        RETURN_TOKEN_ID(kCEETokenID_DIV_ASSIGNMENT                  )
"%="                                        RETURN_TOKEN_ID(kCEETokenID_MOD_ASSIGNMENT                  )
"**="                                       RETURN_TOKEN_ID(kCEETokenID_EXP_ASSIGNMENT                  )
"&"                                         RETURN_TOKEN_ID('&'                                         )
"|"                                         RETURN_TOKEN_ID('|'                                         )
"^"                                         RETURN_TOKEN_ID('^'                                         )
"~"                                         RETURN_TOKEN_ID('~'                                         )
"<<"                                        RETURN_TOKEN_ID(kCEETokenID_ZERO_FILL_LEFT_SHIFT            )
">>"                                        RETURN_TOKEN_ID(kCEETokenID_SIGNED_RIGHT_SHIFT              )
">>>"                                       RETURN_TOKEN_ID(kCEETokenID_ZERO_FILL_RIGHT_SHIFT           )
"<<="                                       RETURN_TOKEN_ID(kCEETokenID_ZERO_FILL_LEFT_SHIFT_ASSIGNMENT )
">>="                                       RETURN_TOKEN_ID(kCEETokenID_SIGNED_RIGHT_SHIFT_ASSIGNMENT   )
">>>="                                      RETURN_TOKEN_ID(kCEETokenID_ZERO_FILL_RIGHT_SHIFT_ASSIGNMENT)
"&="                                        RETURN_TOKEN_ID(kCEETokenID_AND_ASSIGNMENT                  )
"^="                                        RETURN_TOKEN_ID(kCEETokenID_XOR_ASSIGNMENT                  )
"|="                                        RETURN_TOKEN_ID(kCEETokenID_OR_ASSIGNMENT                   )
"=="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_EQUAL                     )
"==="                                       RETURN_TOKEN_ID(kCEETokenID_EQUAL_VALUE_AND_TYPE            )
"!="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_UNEQUAL                   )
"!=="                                       RETURN_TOKEN_ID(kCEETokenID_NOT_EQUAL_VALUE_AND_TYPE        )
">"                                         RETURN_TOKEN_ID('<'                                         )
"<"                                         RETURN_TOKEN_ID('>'                                         )
">="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LARGE_EQUAL               )
"<="                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_LESS_EQUAL                )
"&&"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_AND                       )
"||"                                        RETURN_TOKEN_ID(kCEETokenID_LOGIC_OR                        )
"!"                                         RETURN_TOKEN_ID('!'                                         )
0[xX]{H}+{IS}?                              RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
{D}+{IS}?                                   RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
0b{B}+{IS}?                                 RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
{D}+{DE}{FS}?                               RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
{D}*"."{D}+{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
{D}+"."{D}*{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
0[xX]{H}+{HE}{FS}?                          RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
0[xX]{H}*"."{H}+{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
0[xX]{H}+"."{H}*{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                        )
{L}({L}|{D})*                               RETURN_TOKEN_ID(kCEETokenID_IDENTIFIER                      )
{NL}                                        RETURN_TOKEN_ID(kCEETokenID_NEW_LINE                        )
{WS}                                        RETURN_TOKEN_ID(kCEETokenID_WHITE_SPACE                     )
.                                           RETURN_TOKEN_ID(kCEETokenID_UNKNOW                          )
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

cee_int cee_lexer_js_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = js_scan_lex();
    cee_long offset = scan_offset;
    cee_ulong length = js_scan_get_leng();
    
    scan_offset += length;
    
    *token = cee_token_create(identifier, offset, length, line_no);
    
    if (identifier == kCEETokenID_NEW_LINE)
        line_no ++;
    
    if (!identifier) /* EOF */
        return 0;
    
    if (identifier == kCEETokenID_LINES_COMMENT) {
        tail_length = c_comment(offset, &eof);
        (*token)->length += tail_length;
        scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LINE_COMMENT) {
        tail_length = cpp_comment(offset, &eof);
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

void cee_lexer_js_buffer_create(const cee_char* subject)
{
    buffer = js_scan__scan_bytes(subject, strlen(subject));
    scan_offset = 0;
    line_no = 0;
}

void cee_lexer_js_buffer_free()
{
    yy_delete_buffer(buffer);
}

