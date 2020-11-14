#ifndef __CEE_SOURCE_PARSERS_H__
#define __CEE_SOURCE_PARSERS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_source_parser.h"

void cee_parsers_create(void);
void cee_parsers_free(void);
CEESourceParserRef cee_source_parser_get(const cee_char* filepath);
void cee_source_parser_asm_name_set(const cee_char* name);
const cee_char* cee_source_parser_asm_name_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_SOURCE_ANALYSERS_H__ */
