#ifndef __CEE_GNU_ASM_H__
#define __CEE_GNU_ASM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_token.h"
#include "cee_source_parser.h"

CEESourceParserRef cee_gnu_asm_parser_create(const cee_char* identifier);
void cee_gnu_asm_parser_free(cee_pointer data);
void cee_lexer_gnu_asm_buffer_create(const cee_uchar* subject);
void cee_lexer_gnu_asm_buffer_free(void);
cee_int cee_lexer_gnu_asm_token_get(CEEToken** token);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_GNU_ASM_PARSER_H__ */
