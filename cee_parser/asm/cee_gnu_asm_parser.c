#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_gnu_asm_parser.h"

typedef struct _CEEGNUASMParser {
} CEEGNUASMParser;

static CEEGNUASMParser* parser_create(void);
static void parser_free(cee_pointer data);
static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_uchar* filepath,
                                const cee_uchar* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map);

/**
 * parser
 */
CEESourceParserRef cee_gnu_asm_parser_create(const cee_char* identifier)
{
    CEESourceParserRef parser = cee_parser_create(identifier);
    parser->imp = parser_create();
    parser->symbol_parse = symbol_parse;
    return (CEESourceParserRef)parser;
}

void cee_gnu_asm_parser_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEESourceParserRef parser = (CEESourceParserRef)data;
    if (parser->imp)
        parser_free(parser->imp);
    cee_parser_free(parser);
}

static CEEGNUASMParser* parser_create(void)
{
    return cee_malloc0(sizeof(CEEGNUASMParser));
}

static void parser_free(cee_pointer data)
{
    if (!data)
        return ;
    CEEGNUASMParser* parser = (CEEGNUASMParser*)data;
    cee_free(parser);
}

static cee_boolean symbol_parse(CEESourceParserRef parser_ref,
                                const cee_uchar* filepath,
                                const cee_uchar* subject,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** statement,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                CEESourceTokenMap** source_token_map)
{
    return TRUE;
}
