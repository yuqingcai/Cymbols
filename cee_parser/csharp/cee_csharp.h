#ifndef __CEE_CSHARP_H__
#define __CEE_CSHARP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"
#include "cee_source_parser.h"

CEESourceParserRef cee_csharp_parser_create(const cee_char* identifier);
void cee_csharp_parser_free(cee_pointer data);
void cee_lexer_csharp_buffer_create(const cee_uchar* subject);
void cee_lexer_csharp_buffer_free(void);
cee_int cee_lexer_csharp_token_get(CEEToken** token);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_CSHARP_H__ */
