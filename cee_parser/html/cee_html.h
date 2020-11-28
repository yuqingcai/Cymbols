#ifndef __CEE_HTML_H__
#define __CEE_HTML_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"
#include "cee_source_parser.h"

CEESourceParserRef cee_html_parser_create(const cee_char* identifier);
void cee_html_parser_free(cee_pointer data);
void cee_lexer_html_buffer_create(const cee_char* subject);
void cee_lexer_html_buffer_free(void);
cee_int cee_lexer_html_token_get(CEEToken** token);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_HTML_H__ */
