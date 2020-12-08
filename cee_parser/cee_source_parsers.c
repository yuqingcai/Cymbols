#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cee_source_parsers.h"
#include "cee_c.h"
#include "cee_html.h"
#include "cee_css.h"
#include "cee_swift.h"
#include "cee_java.h"
#include "cee_gnu_asm.h"
#include "cee_csharp.h"

static CEESourceParserRef asm_parser_get(const cee_char* extension);
static cee_boolean using_gnu_asm(void);
static cee_boolean using_arm_asm(void);


static CEESourceParserRef c_parser = NULL;
static CEESourceParserRef html_parser = NULL;
static CEESourceParserRef css_parser = NULL;
static CEESourceParserRef swift_parser = NULL;
static CEESourceParserRef java_parser = NULL;
static CEESourceParserRef gnu_asm_parser = NULL;
static CEESourceParserRef arm_asm_parser = NULL;
static CEESourceParserRef csharp_parser = NULL;

/*
static CEESourceParserRef java_script_parser = NULL;
*/

static cee_char* asm_parser_name = NULL;

void cee_parsers_create()
{
    cee_parsers_init();
    asm_parser_name = cee_strdup("gnu");
    c_parser = cee_c_parser_create("c");
    html_parser = cee_html_parser_create("html");
    css_parser = cee_css_parser_create("css");
    swift_parser = cee_swift_parser_create("swift");
    java_parser = cee_java_parser_create("java");
    gnu_asm_parser = cee_gnu_asm_parser_create("gnu_asm");
    csharp_parser = cee_csharp_parser_create("csharp");
    
    /**
     arm_asm_parser = cee_arm_asm_parser_create("ARM_ASM");
     java_script_parser = cee_swift_parser_create("java_script");
     */
}

void cee_parsers_free()
{
    cee_c_parser_free(c_parser);
    cee_html_parser_free(html_parser);
    cee_css_parser_free(css_parser);
    cee_swift_parser_free(swift_parser);
    cee_java_parser_free(java_parser);
    cee_gnu_asm_parser_free(gnu_asm_parser);
    cee_csharp_parser_free(gnu_asm_parser);
    
    /**
        cee_java_script_parser_free(swift_parser);
        cee_arm_asm_parser_free(arm_asm_parser);
     */
    
    if (asm_parser_name)
        cee_free(asm_parser_name);
}

static cee_boolean is_c_source_extension(const cee_char* extension)
{
    return !cee_strcmp(extension, "c", FALSE) ||
            !cee_strcmp(extension, "cpp", FALSE) ||
            !cee_strcmp(extension, "cxx", FALSE) ||
            !cee_strcmp(extension, "cc", FALSE) ||
            !cee_strcmp(extension, "m", FALSE) ||
            !cee_strcmp(extension, "mm", FALSE) ||
            !cee_strcmp(extension, "h", FALSE) ||
            !cee_strcmp(extension, "hh", FALSE) ||
            !cee_strcmp(extension, "hpp", FALSE) ||
            !cee_strcmp(extension, "hxx", FALSE);
}

static cee_boolean is_swift_source_extension(const cee_char* extension)
{
    return !cee_strcmp(extension, "swift", FALSE);
}

static cee_boolean is_html_source_extension(const cee_char* extension)
{
    return !cee_strcmp(extension, "html", FALSE);
}

static cee_boolean is_css_source_extension(const cee_char* extension)
{
    return !cee_strcmp(extension, "css", FALSE);
}


static cee_boolean is_java_source_extension(const cee_char* extension)
{
    return !cee_strcmp(extension, "java", FALSE);
}

static cee_boolean is_asm_source_extension(const cee_char* extension)
{
    return (!cee_strcmp(extension, "asm", FALSE) ||
            !cee_strcmp(extension, "s", FALSE));
}

static cee_boolean is_csharp_source_extension(const cee_char* extension)
{
    return !cee_strcmp(extension, "cs", FALSE);
}

/*
static cee_boolean is_java_script_source_extension(const cee_char* extension)
{
    return !cee_strcmp(extension, "js", FALSE);
}
*/

CEESourceParserRef cee_source_parser_get(const cee_char* filepath)
{
    CEESourceParserRef parser = NULL;
    cee_char* extension = cee_path_extension((const cee_char*)filepath);
    
    if (!extension)
        return parser;
    
    if (is_c_source_extension(extension))
        parser = c_parser;
    else if (is_html_source_extension(extension))
        parser = html_parser;
    else if (is_css_source_extension(extension))
        parser = css_parser;
    else if (is_swift_source_extension(extension))
        parser = swift_parser;
    else if (is_java_source_extension(extension))
        parser = java_parser;
    else if (is_asm_source_extension(extension))
        parser = asm_parser_get(extension);
    else if (is_csharp_source_extension(extension))
        parser = csharp_parser;
    /*
     else if (is_java_script_source_extension(extension))
        parser = java_script_parser;
     */
    
    if (extension)
        cee_free(extension);
    
    return parser;
}

static CEESourceParserRef asm_parser_get(const cee_char* extension)
{
    CEESourceParserRef parser = gnu_asm_parser;
    
    if (using_arm_asm())
        parser = arm_asm_parser;
    else if (using_gnu_asm())
        parser = gnu_asm_parser;
    
    return parser;
}

static cee_boolean using_gnu_asm(void)
{
    return !cee_strcmp(asm_parser_name, "gnu", FALSE);
}

static cee_boolean using_arm_asm(void)
{
    return !cee_strcmp(asm_parser_name, "arm", FALSE);
}

void cee_source_parser_asm_name_set(const cee_char* name)
{
    if (asm_parser_name)
        cee_free(asm_parser_name);
    asm_parser_name = cee_strdup(name);
}

const cee_char* cee_source_parser_asm_name_get(void)
{
    return asm_parser_name;
}




