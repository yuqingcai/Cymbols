%option yylineno
%option noyywrap
%option prefix="xml_scan_"
%x Preprocesser
L   [a-zA-Z_-]
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

static int xml_comment(cee_long offset,
                       cee_boolean* eof);

%}

%%
"="                                         RETURN_TOKEN_ID('='                                     )
"/"                                         RETURN_TOKEN_ID('/'                                     )
"<"                                         RETURN_TOKEN_ID('<'                                     )
">"                                         RETURN_TOKEN_ID('>'                                     )
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                     )
"'"                                         RETURN_TOKEN_ID(kCEETokenID_CHARACTER                   )
"<!--"                                      RETURN_TOKEN_ID(kCEETokenID_XML_COMMENT                 )
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


static int xml_comment(cee_long offset,
                       cee_boolean* eof)
{
    int c0, c1, c2;
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
        
        if (c0 == '-') {
            c1 = input();
            
            if (!c1) {
                *eof = TRUE;
                break;
            }
            
            if (c1 == '-') {
                length ++;
                
                c2 = input();
                
                if (!c2) {
                    *eof = TRUE;
                    break;
                }
                
                if (c2 == '>') {
                    length ++;
                    break;
                }
                
                unput(c2);
                
                length --;
            }
            
            unput(c1);
        }
        
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

cee_int cee_lexer_xml_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = xml_scan_lex();
    cee_long offset = scan_offset;
    cee_ulong length = xml_scan_get_leng();
    
    scan_offset += length;
    
    *token = cee_token_create(identifier, offset, length, line_no);
    
    if (identifier == kCEETokenID_NEW_LINE)
        line_no ++;
    
    if (!identifier) /* EOF */
        return 0;
    
    if (identifier == '<') {
        in_tag = TRUE;
    }
    else if (identifier == '>') {
        if (in_tag)
            in_tag = FALSE;
    }
    else if (identifier == kCEETokenID_XML_COMMENT) {
        tail_length = xml_comment(offset, &eof);
        (*token)->length += tail_length;
        scan_offset += tail_length;
    }
    else if (identifier == kCEETokenID_LITERAL) {
        if (in_tag) {
            tail_length = literal(offset, &eof);
            (*token)->length += tail_length;
            scan_offset += tail_length;
        }
    }
    else if (identifier == kCEETokenID_CHARACTER) {
        if (in_tag) {
            tail_length = character(offset, &eof);
            (*token)->length += tail_length;
            scan_offset += tail_length;
        }
    }
        
    if (eof)
        return 0;
    
    return 1;
}
void cee_lexer_xml_buffer_create(const cee_char* subject)
{
    buffer = xml_scan__scan_bytes(subject, strlen(subject));
    scan_offset = 0;
    line_no = 0;
}

void cee_lexer_xml_buffer_free()
{
    yy_delete_buffer(buffer);
}

