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

#define RETURN_TOKEN_ID(token_id) {\
    return token_id;\
}

static int lines_comment(cee_long offset, 
                        cee_boolean* eof);
static int line_comment(cee_long offset, 
                        cee_boolean* eof);
%}

%%
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_LINES_COMMENT               )
"//"                                        RETURN_TOKEN_ID(kCEETokenID_LINE_COMMENT                )
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

