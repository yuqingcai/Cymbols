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
                                const cee_uchar* subject,
                                CEESourceFregment** statement,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                cee_pointer** buffer2tokens);

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
                                const cee_uchar* subject,
                                CEESourceFregment** statement,
                                CEESourceFregment** prep_directive,
                                CEESourceFregment** comment,
                                CEEList** tokens_ref,
                                cee_pointer** buffer2tokens)
{
    CEETokenID token_id = kCEETokenID_UNKNOW;
    CEESourceFregment* root = NULL;
    return root;
}
