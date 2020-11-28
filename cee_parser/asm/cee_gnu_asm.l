%option yylineno
%option noyywrap
%option prefix="gnu_asm_scan_"
%x Preprocesser
L   [a-zA-Z_.$]
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
"="                                         RETURN_TOKEN_ID('='                                     )
"+"                                         RETURN_TOKEN_ID('+'                                     )
"-"                                         RETURN_TOKEN_ID('-'                                     )
"*"                                         RETURN_TOKEN_ID('*'                                     )
"/"                                         RETURN_TOKEN_ID('/'                                     )
"\\"                                        RETURN_TOKEN_ID('\\'                                    )
"%"                                         RETURN_TOKEN_ID('%'                                     )
"~"                                         RETURN_TOKEN_ID('~'                                     )
"&"                                         RETURN_TOKEN_ID('&'                                     )
"|"                                         RETURN_TOKEN_ID('|'                                     )
"^"                                         RETURN_TOKEN_ID('^'                                     )
"<"                                         RETURN_TOKEN_ID('<'                                     )
">"                                         RETURN_TOKEN_ID('>'                                     )
"."                                         RETURN_TOKEN_ID('.'                                     )
","                                         RETURN_TOKEN_ID(','                                     )
"?"                                         RETURN_TOKEN_ID('?'                                     )
":"                                         RETURN_TOKEN_ID(':'                                     )
"("                                         RETURN_TOKEN_ID('('                                     )
")"                                         RETURN_TOKEN_ID(')'                                     )
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_LINES_COMMENT               )
"#"                                         RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                ) /** ‘#’ comment for i386, x86-64, i960, Motorola PowerPC, IBM S/390, Sunplus SCORE, ip2k, m32c, m32r, 68HC11 and 68HC12, Vax, V850, Xtensa systems */
";"                                         RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                ) /** ‘;’ comment for ARC, H8/300 family, HPPA, PDP-11, picoJava, Z80 */
"@"                                         RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                ) /** ‘@’ comment for ARM */
"!"                                         RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                ) /** ‘!’ comment for Renesas / SuperH SH, SPARC, 680x0, Z8000 */
"--"                                        RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                ) /** ‘!’ comment for V850 */
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                     )
"'"                                         RETURN_TOKEN_ID(kCEETokenID_CHARACTER                   )
"#"{WS}*"include"                           RETURN_TOKEN_ID(kCEETokenID_HASH_INCLUDE                )
"#"{WS}*"define"                            RETURN_TOKEN_ID(kCEETokenID_HASH_DEFINE                 )
"#"{WS}*"if"                                RETURN_TOKEN_ID(kCEETokenID_HASH_IF                     )
"#"{WS}*"ifdef"                             RETURN_TOKEN_ID(kCEETokenID_HASH_IFDEF                  )
"#"{WS}*"ifndef"                            RETURN_TOKEN_ID(kCEETokenID_HASH_IFNDEF                 )
"#"{WS}*"endif"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ENDIF                  )
"#"{WS}*"else"                              RETURN_TOKEN_ID(kCEETokenID_HASH_ELSE                   )
"#"{WS}*"elif"                              RETURN_TOKEN_ID(kCEETokenID_HASH_ELIF                   )
"#"{WS}*"undef"                             RETURN_TOKEN_ID(kCEETokenID_HASH_UNDEF                  )
"#"{WS}*"line"                              RETURN_TOKEN_ID(kCEETokenID_HASH_LINE                   )
"#"{WS}*"error"                             RETURN_TOKEN_ID(kCEETokenID_HASH_ERROR                  )
"#"{WS}*"warning"                           RETURN_TOKEN_ID(kCEETokenID_HASH_WARNING                )
"#"{WS}*"pragma"                            RETURN_TOKEN_ID(kCEETokenID_HASH_PRAGMA                 )
"#"{WS}*"import"                            RETURN_TOKEN_ID(kCEETokenID_HASH_IMPORT                 )
0[xX]{H}+{IS}?                              RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+{IS}?                                   RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0b{B}+{IS}?                                 RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+{DE}{FS}?                               RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}*"."{D}+{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+"."{D}*{DE}?{FS}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}+{HE}{FS}?                          RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}*"."{H}+{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}+"."{H}*{HE}?{FS}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
[a-zA-Z0-9_.$]+":"                           RETURN_TOKEN_ID(kCEETokenID_LABEL                      )
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

cee_int cee_lexer_gnu_asm_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = gnu_asm_scan_lex();
    cee_long offset = scan_offset;
    cee_ulong length = gnu_asm_scan_get_leng();
    
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

void cee_lexer_gnu_asm_buffer_create(const cee_char* subject)
{
    buffer = gnu_asm_scan__scan_bytes(subject, strlen(subject));
    scan_offset = 0;
}

void cee_lexer_gnu_asm_buffer_free()
{
    yy_delete_buffer(buffer);
}

