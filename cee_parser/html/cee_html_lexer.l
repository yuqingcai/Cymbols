%option yylineno
%option noyywrap
%option prefix="html_scan_"
%x Preprocesser
L	[a-zA-Z_]
H	[a-fA-F0-9]
D	[0-9]
O   [0-7]
B   [01]
WS	[ \t]
NL	(\r\n|\n|\r\v\f)
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

%}

%%
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


cee_int cee_lexer_html_token_get(CEETokenID* token_id,
                                 cee_long* offset,
                                 cee_ulong* length)
{
    cee_boolean terminate = FALSE;
    cee_ulong tail_length = 0;
    
    *token_id = html_scan_lex();
    *offset = g_scan_offset;
    *length = html_scan_get_leng();
    
    g_scan_offset += html_scan_get_leng();
    
    if (!*token_id) /* EOF */
        return 0;
    
    return 1;
}

void cee_lexer_html_subject_init(const cee_uchar* subject)
{
    buffer = html_scan__scan_bytes(subject, strlen(subject));
    g_scan_offset = 0;
}

void cee_lexer_html_buffer_free()
{
    yy_delete_buffer(buffer);
}

