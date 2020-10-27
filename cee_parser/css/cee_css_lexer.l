%option yylineno
%option noyywrap
%option prefix="css_scan_"
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
UT  ("cm"|"CM"|"mm"|"MM"|"in"|"IN"|"px"|"PX"|"pt"|"PT"|"pc"|"PC"|"em"|"EM"|"ex"|"EX"|"ch"|"CH"|"rem"|"REM"|"vw"|"VW"|"vh"|"VH"|"vmin"|"VMIN"|"vmax"|"VMAX"|"%")



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

static int css_comment(cee_long offset, 
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
"!"                                         RETURN_TOKEN_ID('!'                                     )
"<"                                         RETURN_TOKEN_ID('<'                                     )
">"                                         RETURN_TOKEN_ID('>'                                     )
"."                                         RETURN_TOKEN_ID('.'                                     )
","                                         RETURN_TOKEN_ID(','                                     )
"?"                                         RETURN_TOKEN_ID('?'                                     )
":"                                         RETURN_TOKEN_ID(':'                                     )
"("                                         RETURN_TOKEN_ID('('                                     )
")"                                         RETURN_TOKEN_ID(')'                                     )
"{"                                         RETURN_TOKEN_ID('{'                                     )
"}"                                         RETURN_TOKEN_ID('}'                                     )
"["                                         RETURN_TOKEN_ID('['                                     )
"]"                                         RETURN_TOKEN_ID(']'                                     )
"#"                                         RETURN_TOKEN_ID('#'                                     )
";"                                         RETURN_TOKEN_ID(';'                                     )
"@"{WN}*{L}({L}|{D})*                       RETURN_TOKEN_ID(kCEETokenID_CSS_RULE                    )
"/*"                                        RETURN_TOKEN_ID(kCEETokenID_C_COMMENT                   )
"\""                                        RETURN_TOKEN_ID(kCEETokenID_LITERAL                     )
"align-content"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"align-items"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"align-self"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-delay"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-direction"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-duration"                        RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-fill-mode"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-iteration-count"                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-name    Specifies"               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-play-state"                      RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"animation-timing-function"                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"backface-visibility"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-attachment"                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-clip"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-color"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-image"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-origin"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-position"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-repeat"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"background-size"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border"                                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-bottom"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-bottom-color"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-bottom-left-radius"                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-bottom-right-radius"                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-bottom-style"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-bottom-width"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-collapse"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-color"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-image"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-image-outset"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-image-repeat"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-image-slice"                        RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-image-source"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-image-width"                        RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-left"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-left-color"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-left-style"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-left-width"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-radius"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-right"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-right-color"                        RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-right-style"                        RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-right-width"                        RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-spacing"                            RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-style"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-top"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-top-color"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-top-left-radius"                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-top-right-radius"                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-top-style"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-top-width"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"border-width"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"bottom"                                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"box-shadow"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"box-sizing"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"caption-side"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"clear"                                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"clip"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"color"                                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-count"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-fill"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-gap"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-rule"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-rule-color"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-rule-style"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-rule-width"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-span"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"column-width"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"columns"                                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"content"                                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"counter-increment"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"counter-reset"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"cursor"                                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"direction"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"display"                                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"empty-cells"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"flex"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"flex-basis"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"flex-direction"                            RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"flex-flow"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"flex-grow"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"flex-shrink"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"flex-wrap"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"float"                                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font-family"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font-size"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font-size-adjust"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font-stretch"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font-style"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font-variant"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"font-weight"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"height"                                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"justify-content"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"left"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"letter-spacing"                            RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"line-height"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"list-style"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"list-style-image"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"list-style-position"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"list-style-type"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"margin"                                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"margin-bottom"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"margin-left"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"margin-right"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"margin-top"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"max-height"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"max-width"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"min-height"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"min-width"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"opacity"                                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"order"                                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"outline"                                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"outline-color"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"outline-offset"                            RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"outline-style"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"outline-width"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"overflow"                                  RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"overflow-x"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"overflow-y"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"padding"                                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"padding-bottom"                            RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"padding-left"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"padding-right"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"padding-top"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"page-break-after"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"page-break-before"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"page-break-inside"                         RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"perspective"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"perspective-origin"                        RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"position"                                  RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"quotes"                                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"resize"                                    RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"right"                                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"tab-size"                                  RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"table-layout"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-align"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-align-last"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-decoration"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-decoration-color"                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-decoration-line"                      RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-decoration-style"                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-indent"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-justify"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-overflow"                             RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-shadow"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"text-transform"                            RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"top"                                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transform"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transform-origin"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transform-style"                           RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transition"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transition-delay"                          RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transition-duration"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transition-property"                       RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"transition-timing-function"                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"vertical-align"                            RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"visibility"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"white-space"                               RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"width"                                     RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"word-break"                                RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"word-spacing"                              RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"word-wrap"                                 RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"z-index"                                   RETURN_TOKEN_ID(kCEETokenID_CSS_PROPERTY                )
"attr"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"calc"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"cubic-bezier"                              RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"hsl"                                       RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"hsla"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"linear-gradient"                           RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"radial-gradient"                           RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"repeating-linear-gradient"                 RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"repeating-radial-gradient"                 RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"rgb"                                       RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"rgba"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"var"                                       RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"step"                                      RETURN_TOKEN_ID(kCEETokenID_CSS_FUNCTION                )
"'"                                         RETURN_TOKEN_ID(kCEETokenID_CHARACTER                   )
0[xX]{H}+{IS}?{UT}?                         RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
"#"{H}+{IS}?{UT}?                           RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+{IS}?{UT}?                              RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0b{B}+{IS}?{UT}?                            RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+{DE}{FS}?{UT}?                          RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}*"."{D}+{DE}?{FS}?{UT}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{D}+"."{D}*{DE}?{FS}?{UT}?                  RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}+{HE}{FS}?{UT}?                     RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}*"."{H}+{HE}?{FS}?{UT}?             RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
0[xX]{H}+"."{H}*{HE}?{FS}?{UT}?             RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
"#"{H}+{HE}{FS}?{UT}?                       RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
"#"{H}*"."{H}+{HE}?{FS}?{UT}?               RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
"#"{H}+"."{H}*{HE}?{FS}?{UT}?               RETURN_TOKEN_ID(kCEETokenID_CONSTANT                    )
{L}({L}|{D})*                               RETURN_TOKEN_ID(kCEETokenID_IDENTIFIER                  )
{NL}                                        RETURN_TOKEN_ID(kCEETokenID_NEW_LINE                    )
{WS}                                        RETURN_TOKEN_ID(kCEETokenID_WHITE_SPACE                 )
.                                           RETURN_TOKEN_ID(kCEETokenID_UNKNOW                      )
%%

static int css_comment(cee_long offset, 
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

cee_int cee_lexer_css_token_get(CEEToken** token)
{
    cee_boolean eof = FALSE;
    cee_ulong tail_length = 0;
    
    CEETokenID identifier = css_scan_lex();
    cee_long offset = g_scan_offset;
    cee_ulong length = css_scan_get_leng();
    
    g_scan_offset += length;
    
    *token = cee_token_create(identifier, offset, length);
    
    if (!identifier) /* EOF */
        return 0;
        
    if (identifier == kCEETokenID_C_COMMENT) {
        tail_length = css_comment(offset, &eof);
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
void cee_lexer_css_buffer_create(const cee_uchar* subject)
{
    buffer = css_scan__scan_bytes(subject, strlen(subject));
    g_scan_offset = 0;
}

void cee_lexer_css_buffer_free()
{
    yy_delete_buffer(buffer);
}

