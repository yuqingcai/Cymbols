//
//  cee_text_macos_attribute.c
//  Cymbols
//
//  Created by qing on 2019/11/21.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#include <cjson/cJSON.h>
#include <pcre.h>
#include "cee_regex.h"
#include "cee_text_macos_attribute.h"

#define OVECCOUNT 30

typedef struct _CEETextFallbackFontItem {
    cee_float size;
    CEEList* fonts;
} CEETextFallbackFontItem;

typedef struct _CEEMacOSPlatform {
    cee_char* name;
    CEEList* attributes;
    CEETextColorRef default_backgroundcolor;
    CEETextAttribute* attribute_map[kCEETagTypeMax];
    CEEList* fallback_font_names;
    CEEList* fallback_font_items;
    CGColorSpaceRef color_space;
} CEEMacOSPlatform;

static void font_attributes_from_descriptor(const cee_char* descriptor,
                                            cee_char** family,
                                            cee_char** style,
                                            cee_char** size);
static CGColorRef color_create_form_descriptor(CEEMacOSPlatform* platform, 
                                               const char* descriptor);
static cee_boolean color_component_from_hsb_descriptor(const char* descriptor, 
                                                       CGFloat components[4]);
static cee_boolean color_component_from_rgb_descriptor(const char* descriptor,
                                                       CGFloat components[4]);
static cee_boolean color_component_from_hex(const char* descriptor,
                                            CGFloat components[4]);
static void color_free(cee_pointer color);
static void platform_attributes_clean(CEEMacOSPlatform* platform);
static void platform_default_background_color_clean(CEEMacOSPlatform* platform);
static void default_plantext_attribute_create(CEEMacOSPlatform* platform);
static CEETextAttribute* text_attribute_create_from_rule(CEEMacOSPlatform* platform,
                                                         const cJSON* rule);
static void hsb2rgb(cee_float hue,
                    cee_float saturation,
                    cee_float brightness,
                    cee_float *red,
                    cee_float *green,
                    cee_float *blue);



static const cee_char* default_rule_descriptor =
"{\
    \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\", \
    \"forecolor\" : \"rgba(20, 20, 20, 100)\" \
}";



CTFontSymbolicTraits traits_make(const char* style)
{
    CTFontSymbolicTraits traits = 0;
    if (cee_strcontain(style, "italic", TRUE))
        traits |= kCTFontTraitItalic;
    if (cee_strcontain(style, "bold", TRUE))
        traits |= kCTFontTraitBold;
    return traits;
}

static CTFontRef font_create(const char* family,
                             const char* style,
                             const float size)
{
    
    CTFontSymbolicTraits traits = traits_make(style);
    CFStringRef traitsDictKeys[1] = {
        kCTFontSymbolicTrait
    };
    CFStringRef traitsDictValues[1] = {
        (CFStringRef)CFNumberCreate(NULL, kCFNumberSInt32Type, &traits)
    };
    
    CFDictionaryRef traitsDict =
        CFDictionaryCreate(NULL,
                           (const void**)traitsDictKeys,
                           (const void**)traitsDictValues,
                           1,
                           NULL,
                           NULL);
    
    CFStringRef fontDescriptorDictKeys[3] = {
        kCTFontFamilyNameAttribute,
        kCTFontSizeAttribute,
        kCTFontTraitsAttribute
    };
    CFStringRef fontDescriptorDictValues[3] = {
        CFStringCreateWithCString(NULL, family, kCFStringEncodingUTF8),
        (CFStringRef)CFNumberCreate(NULL, kCFNumberFloat32Type, &size),
        (CFStringRef)traitsDict
    };
        
    CFDictionaryRef fontDescriptorDict =
        CFDictionaryCreate(NULL,
                           (const void**)fontDescriptorDictKeys,
                           (const void**)fontDescriptorDictValues,
                           3,
                           NULL,
                           NULL);
    
    CTFontDescriptorRef fontDescriptor =
        CTFontDescriptorCreateWithAttributes(fontDescriptorDict);
    CTFontRef font = CTFontCreateWithFontDescriptor(fontDescriptor, size, NULL);
    
    CFRelease(fontDescriptor);
    CFRelease(fontDescriptorDict);
    CFRelease(fontDescriptorDictValues[0]);
    CFRelease(fontDescriptorDictValues[1]);
    
    CFRelease(traitsDict);
    CFRelease(traitsDictValues[0]);
    
    return font;
}

static CTFontRef font_create_from_descriptor(const char* descriptor)
{
    cee_char* family = NULL;
    cee_char* style = NULL;
    cee_char* size = NULL;
    CTFontRef font = NULL;
    
    font_attributes_from_descriptor(descriptor, &family, &style, &size);
    font = font_create(family, style, atof(size));

    cee_free(size);
    cee_free(style);
    cee_free(family);
    
    return font;
}

static void font_free(cee_pointer font)
{
    if (!font)
        return;
    
    CFRelease(font);
}

static void fallback_font_item_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETextFallbackFontItem* item = (CEETextFallbackFontItem*)data;
    
    CEEList* p = item->fonts;
    while (p) {
        CTFontRef font = p->data;
        CFRelease(font);
        p = p->next;
    }
    cee_list_free(item->fonts);
    cee_free(item);
}

static CEETextFallbackFontItem* fallback_font_item_create(CEEMacOSPlatform* platform,
                                                          cee_float size)
{
    CEETextFallbackFontItem* item = 
        (CEETextFallbackFontItem*)cee_malloc0(sizeof(CEETextFallbackFontItem));
    item->size = size;
    
    CTFontRef font = NULL;
    CEEList* p = platform->fallback_font_names;
    while (p) {
        cee_char* name = p->data;
        font = font_create(name, "Regular", size);
        item->fonts = cee_list_append(item->fonts, (cee_pointer)font);
        p = p->next;
    }
    
    return item;
}

static CEETextFallbackFontItem* fallback_font_item_get(CEEMacOSPlatform* platform,
                                                       cee_float size)
{
    CEETextFallbackFontItem* item = NULL;
    CEEList* p = platform->fallback_font_items;
    while (p) {
        item = p->data;
        if (fabs(size - item->size) < FLT_EPSILON)
            return item;
        p = p->next;
    }
    
    item = fallback_font_item_create(platform, size);
    platform->fallback_font_items = 
        cee_list_append(platform->fallback_font_items, item);
    
    return item;
}

static CTFontRef font_get_from_fallback_item(CEETextFallbackFontItem* item,
                                             CEEUnicodePoint codepoint)
{
    CTFontRef font = NULL;
    CEEList* p = item->fonts;
    CGGlyph glyphs[2];
    UniChar characters[4] = { 0, 0, 0, 0};
    CFIndex length = 1;
    
    if (CFStringGetSurrogatePairForLongCharacter(codepoint, characters))
        length = 2;
    
    while (p) {
        font = p->data;
        if (CTFontGetGlyphsForCharacters(font, characters, glyphs, length))
            return font;
        p = p->next;
    }
    
    return NULL;
}

static CTFontRef fallback_font_get(CEEMacOSPlatform* platform,
                                  CEEUnicodePoint codepoint,
                                  CEETextAttribute* attribute)
{    
    cee_float size = attribute->font_size;
    CTFontRef font = NULL;
    CEETextFallbackFontItem* item = fallback_font_item_get(platform, size);
    font = font_get_from_fallback_item(item, codepoint);
    return font;
}

static CTFontRef font_get(CEEMacOSPlatform* platform,
                          CEEUnicodePoint codepoint,
                          CEETagType type)
{
    CEETextAttribute* attribute = platform->attribute_map[type];
    if (!attribute)
        attribute = platform->attribute_map[kCEETagTypePlainText];
    
    CTFontRef font = (CTFontRef)attribute->font;
    
    CGGlyph glyphs[2];
    UniChar characters[4] = { 0, 0, 0, 0};
    CFIndex length = 1;
    
    if (CFStringGetSurrogatePairForLongCharacter(codepoint, characters))
        length = 2;
    
    if (!CTFontGetGlyphsForCharacters(font, characters, glyphs, length))
        font = fallback_font_get(platform, codepoint, attribute);
    
    return font;
}

static CEETextAttribute* text_attribute_create(cee_char* name,
                                               CEETextFontRef font,
                                               cee_float font_size,
                                               cee_boolean underline,
                                               cee_boolean strikethrough,
                                               CGColorRef forecolor,
                                               CGColorRef backgroundcolor)
{
    CEETextAttribute* attribute = 
        (CEETextAttribute*)cee_malloc0(sizeof(CEETextAttribute));
    attribute->name = cee_strdup(name);
    attribute->font = font;
    attribute->font_size = font_size;
    attribute->underline = underline;
    attribute->strikethrough = strikethrough;
    attribute->forecolor = (CEETextColorRef)forecolor;
    attribute->backgroundcolor = (CEETextColorRef)backgroundcolor;
    return attribute;
}

static void text_attribute_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETextAttribute* attribute = (CEETextAttribute*)data;
    
    if (attribute->name)
        cee_free(attribute->name);
    
    if (attribute->backgroundcolor)
        color_free(attribute->backgroundcolor);
    
    if (attribute->forecolor)
        color_free(attribute->forecolor);
        
    if (attribute->font)
        font_free(attribute->font);
    
    cee_free(data);
}

static CEEList* fallback_font_names_create(CEEMacOSPlatform* platform)
{
    CEETextAttribute* attribute = platform->attribute_map[kCEETagTypePlainText];
    CTFontRef font = (CTFontRef)attribute->font;
        
    CEEList* names = NULL;
    CFArrayRef descriptors = CTFontCopyDefaultCascadeListForLanguages(font, nil);
    CFIndex numberOfDescriptor = CFArrayGetCount(descriptors);
    for (CFIndex i = 0; i < numberOfDescriptor; i ++) {
        CTFontDescriptorRef descriptor = CFArrayGetValueAtIndex(descriptors, i);
        CFStringRef name = 
            CTFontDescriptorCopyAttribute(descriptor, kCTFontFamilyNameAttribute);
        
        CFIndex length = CFStringGetLength(name);
        CFIndex max_size = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
        char *buffer = (char *)cee_malloc0(max_size);
        if (CFStringGetCString(name, buffer, max_size, kCFStringEncodingUTF8))
            names = cee_list_append(names, buffer);
        else
            cee_free(buffer);
        
        CFRelease(name);
    }
    
    return names;
}

static CEETextGlyphRef unknow_glyph_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    CGGlyph glyphs[2];
    UniChar characters[4] = { 0, 0, 0, 0};
    CFIndex length = 1;
    
    if (CFStringGetSurrogatePairForLongCharacter(codepoint, characters))
        length = 2;
    
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlainText);
    CTFontGetGlyphsForCharacters(font, characters, glyphs, length);
    return (CEETextGlyphRef)glyphs[0];
}

static cee_float unknow_glyph_width_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    CGSize advances[2];
    CGGlyph glyphs[2];
    UniChar characters[4] = { 0, 0, 0, 0};
    CFIndex length = 1;
    
    if (CFStringGetSurrogatePairForLongCharacter(codepoint, characters))
        length = 2;
    
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlainText);
    CTFontGetGlyphsForCharacters(font, characters, glyphs, length);
    CTFontGetAdvancesForGlyphs(font, kCTFontOrientationHorizontal, glyphs, advances, 1);
    return advances[0].width;
}

static cee_float unknow_glyph_height_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlainText);
    return (CTFontGetDescent(font) + CTFontGetAscent(font));
}

static cee_float unknow_glyph_ascent_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlainText);
    return CTFontGetAscent(font);
}

static CEETextFontRef unknow_glyph_font_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    return (CEETextFontRef)font_get(platform, codepoint, kCEETagTypePlainText);
}

cee_pointer cee_text_platform_create(void)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)cee_malloc0(sizeof(CEEMacOSPlatform));
    platform->name = cee_strdup("MacOS");
    platform->color_space = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    cee_text_platform_configure(platform, NULL);
    return platform;
}

void cee_text_platform_free(cee_pointer data)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)data;
    if (platform->name)
        cee_free(platform->name);
        
    if (platform->attributes)
        cee_list_free_full(platform->attributes, text_attribute_free);
        
    if (platform->fallback_font_names)
        cee_list_free_full(platform->fallback_font_names, cee_free);
    
    if (platform->fallback_font_items)
        cee_list_free_full(platform->fallback_font_items,
                           fallback_font_item_free);
    if (platform->color_space)
        CGColorSpaceRelease(platform->color_space);
    
    if (platform->default_backgroundcolor)
        color_free(platform->default_backgroundcolor);
    
    cee_free(platform);
}

static void platform_default_background_color_clean(CEEMacOSPlatform* platform)
{
    if (platform->default_backgroundcolor)
        color_free(platform->default_backgroundcolor);
    platform->default_backgroundcolor = NULL;
}

const cee_char* cee_text_platform_name_get(cee_pointer platform_ref)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    return platform->name;
}

void cee_text_platform_attributes_create(cee_pointer platform_ref)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
}

cee_boolean cee_text_platform_configure(cee_pointer platform_ref,
                                        const cee_uchar* descriptor)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    platform_attributes_clean(platform);    
    if (!descriptor) {
        default_plantext_attribute_create(platform);
        return TRUE;
    }
    platform_default_background_color_clean(platform);
    
    cJSON *descriptor_object = cJSON_Parse((const char*)descriptor);
    if (!descriptor_object) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            fprintf(stderr, "is_valid_text_platform_descriptor Error before: %s\n", error_ptr);
        return FALSE;
    }
        
    const cJSON *background_color_item = cJSON_GetObjectItemCaseSensitive(descriptor_object, "backgroundcolor");
    if (background_color_item) {
        cee_char* string = background_color_item->valuestring;
        platform->default_backgroundcolor = (CEETextColorRef)color_create_form_descriptor(platform, string);
    }
    
    CEEList* attributes = NULL;
    CEETextAttribute* attribute = NULL;
    CEETextAttribute* default_attribute = NULL;
    const cJSON *rules = cJSON_GetObjectItemCaseSensitive(descriptor_object, "rules");
    if (rules) {
        const cJSON *rule = NULL;
        
        struct SyntaxColorSchemeRule {
            CEETagType tag;
            cee_char* name;
        } rule_map[] = {
            { kCEETagTypePlainText,                        "plaintext"                             },
            { kCEETagTypeComment,                          "comment"                               },
            { kCEETagTypeKeyword,                          "keyword"                               },
            { kCEETagTypeConstant,                         "constant"                              },
            { kCEETagTypeCharacter,                        "character"                             },
            { kCEETagTypeLiteral,                          "literal"                               },
            { kCEETagTypePunctuation,                      "punctuation"                           },
            { kCEETagTypePrepDirective,                    "prep_directive"                        },
            { kCEETagTypePrepFilename,                     "filename"                              },
            { kCEETagTypePrepDirectiveDefine,              "prep_directive"                        },
            { kCEETagTypePrepDirectiveDefineParameter,     "parameter"                             },
            { kCEETagTypeFunctionDefinition,               "function"                              },
            { kCEETagTypeFunctionDeclaration,              "function"                              },
            { kCEETagTypeCatchBlock,                       "keyword"                               },
            { kCEETagTypeVariable,                         "variable"                              },
            { kCEETagTypeProperty,                         "variable"                              },
            { kCEETagTypeMessageDeclaration,               "function"                              },
            { kCEETagTypeMessageDefinition,                "function"                              },
            { kCEETagTypeFunctionParameter,                "parameter"                             },
            { kCEETagTypeMessageParameter,                 "parameter"                             },
            { kCEETagTypeOperatorDeclaration,              "function"                              },
            { kCEETagTypeOperatorDefinition,               "function"                              },
            { kCEETagTypeClassDefinition,                  "class"                                 },
            { kCEETagTypeEnumDefinition,                   "enum"                                  },
            { kCEETagTypeUnionDefinition,                  "union"                                 },
            { kCEETagTypeInterface,                        "interface"                             },
            { kCEETagTypeImplementation,                   "implementation"                        },
            { kCEETagTypeProtocol,                         "protocol"                              },
            { kCEETagTypeEnumerator,                       "enumerator"                            },
            { kCEETagTypeNamespaceDeclaration,             "namespace"                             },
            { kCEETagTypeTypeDeclaration,                  "type_declaration"                      },
            { kCEETagTypeNamespaceDefinition,              "namespace"                             },
            { kCEETagTypeTypeDefine,                       "type_declaration"                      },
            { kCEETagTypePrepDirectiveReference,           "prep_directive_reference"              },
            { kCEETagTypePrepDirectiveParameterReference,  "prep_directive_parameter_reference"    },
            { kCEETagTypeTypeReference,                    "type_reference"                        },
            { kCEETagTypeTypeDefineReference,              "type_reference"                        },
            { kCEETagTypeVariableReference,                "variable_reference"                    },
            { kCEETagTypeFunctionReference,                "function_reference"                    },
            { kCEETagTypeMessageReference,                 "function_reference"                    },
            { kCEETagTypeMemberReference,                  "member_reference"                      },
            { kCEETagTypeNamespaceReference,               "variable_reference"                    },
            { kCEETagTypeLabelReference,                   "variable_reference"                    },
            { kCEETagTypeEnumeratorReference,              "enumerator"                            },
            
        };
        
        int map_size = sizeof (rule_map) / sizeof(rule_map[0]);
        for (int i = 0; i < kCEETagTypeMax; i ++)
            platform->attribute_map[i] = NULL;
        
        cJSON_ArrayForEach(rule, rules) {
            cJSON* name_item = cJSON_GetObjectItemCaseSensitive(rule, "name");
            CEEList* tag_indexes = NULL;
            CEEList* p = NULL;
            cee_long index = -1;
            
            for (int i = 0; i < map_size; i ++) {
                if (!strcmp(name_item->valuestring, rule_map[i].name))
                    tag_indexes = cee_list_prepend(tag_indexes, CEE_INT_TO_POINTER(rule_map[i].tag));
            }
            
            attribute = text_attribute_create_from_rule(platform, rule);
            attributes = cee_list_prepend(attributes, attribute);
            
            p = tag_indexes;
            while (p) {
                index = CEE_POINTER_TO_INT(p->data);
                platform->attribute_map[index] = attribute;
                p = p->next;
            }
            cee_list_free(tag_indexes);

            if (!default_attribute) {
                if (!strcmp(name_item->valuestring, "plaintext"))
                    default_attribute = attribute;
            }
        }
    }
    else {
        attribute = text_attribute_create_from_rule(platform, descriptor_object);
        attributes = cee_list_append(attributes, attribute);
        default_attribute = attribute;
    }
    
    cJSON_Delete(descriptor_object);
    
    if (!platform->attribute_map[kCEETagTypePlainText])
        platform->attribute_map[kCEETagTypePlainText] = default_attribute;
    
    platform->attributes = attributes;
    platform->fallback_font_names = fallback_font_names_create(platform);
    
    return TRUE;
}

static void platform_attributes_clean(CEEMacOSPlatform* platform)
{
    if (platform->attributes)
        cee_list_free_full(platform->attributes, text_attribute_free);
    platform->attributes = NULL;
    
    for (int i = 0; i < kCEETagTypeMax; i ++)
        platform->attribute_map[i] = NULL;
}

static void default_plantext_attribute_create(CEEMacOSPlatform* platform)
{
    CEETextAttribute* attribute = NULL;
    CEEList* attributes = NULL;
    cJSON *rule = cJSON_Parse(default_rule_descriptor);
    attribute = text_attribute_create_from_rule(platform, rule);
    attributes = cee_list_append(attributes, attribute);
    platform->attribute_map[kCEETagTypePlainText] = attribute;
    platform->attributes = attributes;
    platform->fallback_font_names = fallback_font_names_create(platform);
}

static CEETextAttribute* text_attribute_create_from_rule(CEEMacOSPlatform* platform, 
                                                         const cJSON* rule)
{
    cJSON* name_item = NULL;
    cJSON* font_item = NULL;
    cJSON* underline_item = NULL;
    cJSON* strikethrough_item = NULL;
    cJSON* forecolor_item = NULL;
    cJSON* backgroundcolor_item = NULL;
    
    cee_char* name = NULL;
    CTFontRef font = NULL;
    CGColorRef forecolor = NULL;
    CGColorRef backgroundcolor = NULL;
    cee_boolean underline = FALSE;
    cee_boolean strikethrough = FALSE;
    cee_char* font_size = NULL;
    CEETextAttribute* attribute = NULL;
    
    name_item = cJSON_GetObjectItemCaseSensitive(rule, "name");
    font_item = cJSON_GetObjectItemCaseSensitive(rule, "font");
    underline_item = cJSON_GetObjectItemCaseSensitive(rule, "underline");
    strikethrough_item = cJSON_GetObjectItemCaseSensitive(rule, "strikethrough");
    forecolor_item = cJSON_GetObjectItemCaseSensitive(rule, "forecolor");
    backgroundcolor_item = cJSON_GetObjectItemCaseSensitive(rule, "backgroundcolor");
    
    if (name_item)
        name = name_item->valuestring;
    
    if (font_item) {
        font_attributes_from_descriptor(font_item->valuestring, NULL, NULL, &font_size);
        font = font_create_from_descriptor(font_item->valuestring);
    }
    if (forecolor_item)
        forecolor = color_create_form_descriptor(platform, 
                                                 forecolor_item->valuestring);
    
    if (backgroundcolor_item)
        backgroundcolor = color_create_form_descriptor(platform, 
                                                       backgroundcolor_item->valuestring);
    
    if (underline_item && !strcmp(underline_item->valuestring, "true"))
        underline = TRUE;
    if (strikethrough && !strcmp(strikethrough_item->valuestring, "true"))
        strikethrough = TRUE;

    attribute = text_attribute_create(name,
                                      (CEETextFontRef)font,
                                      atof(font_size),
                                      underline, 
                                      strikethrough, 
                                      forecolor,
                                      backgroundcolor);
    
    if (font_size)
        cee_free(font_size);
    
    return attribute;
}

CEETextGlyphRef cee_text_platform_glyph_get(cee_pointer platform_ref,
                                            CEEUnicodePoint codepoint,
                                            CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    CGGlyph glyphs[2];
    UniChar characters[4] = { 0, 0, 0, 0};
    CFIndex length = 1;
    
    if (CFStringGetSurrogatePairForLongCharacter(codepoint, characters))
        length = 2;
    
    CTFontRef font = font_get(platform, codepoint, type);
    if (!font)
        return unknow_glyph_get(platform);
    
    CTFontGetGlyphsForCharacters(font, characters, glyphs, length);
    return (CEETextGlyphRef)glyphs[0];
}

cee_float cee_text_platform_width_get(cee_pointer platform_ref,
                                      CEEUnicodePoint codepoint,
                                      CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    CGSize advances[2];
    CGGlyph glyphs[2];
    UniChar characters[4] = { 0, 0, 0, 0};
    CFIndex length = 1;
    
    if (CFStringGetSurrogatePairForLongCharacter(codepoint, characters))
        length = 2;
    
    CTFontRef font = font_get(platform, codepoint, type);
    if (!font)
        return unknow_glyph_width_get(platform);
    
    CTFontGetGlyphsForCharacters(font, characters, glyphs, length);
    CTFontGetAdvancesForGlyphs(font, kCTFontOrientationHorizontal, glyphs, advances, 1);
    return advances[0].width;
}

cee_float cee_text_platform_height_get(cee_pointer platform_ref,
                                       CEEUnicodePoint codepoint,
                                       CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    CTFontRef font = font_get(platform, codepoint, type);
    if (!font)
        return unknow_glyph_height_get(platform);
    
    return (CTFontGetDescent(font) + CTFontGetAscent(font));
}

cee_float cee_text_platform_ascent_get(cee_pointer platform_ref,
                                       CEEUnicodePoint codepoint,
                                       CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    CTFontRef font = font_get(platform, codepoint, type);
    if (!font)
        return unknow_glyph_ascent_get(platform);
    return CTFontGetAscent(font);
}

cee_boolean cee_text_platform_under_line_get(cee_pointer platform_ref,
                                             CEEUnicodePoint codepoint,
                                             CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    CEETextAttribute* attribute = platform->attribute_map[type];
    if (!attribute)
        attribute = platform->attribute_map[kCEETagTypePlainText];
    return attribute->underline;
}

CEETextFontRef cee_text_platform_font_get(cee_pointer platform_ref,
                                          CEEUnicodePoint codepoint,
                                          CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    CTFontRef font = font_get(platform, codepoint, type);
    if (!font)
        font = (CTFontRef)unknow_glyph_font_get(platform);
    return (CEETextFontRef)font;
}

CEETextColorRef cee_text_platform_fore_color_get(cee_pointer platform_ref,
                                                CEEUnicodePoint codepoint,
                                                CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    if (codepoint == -1 || type == kCEETagTypeIgnore) {
        CEETextAttribute* attribute = platform->attribute_map[kCEETagTypePlainText];
        return attribute->forecolor;
    }
    
    CEETextAttribute* attribute = platform->attribute_map[type];
    if (!attribute)
        attribute = platform->attribute_map[kCEETagTypePlainText];
    return (CEETextColorRef)attribute->forecolor;
}

CEETextColorRef cee_text_platform_background_color_get(cee_pointer platform_ref,
                                                      CEEUnicodePoint codepoint,
                                                      CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    if (codepoint == -1 || type == kCEETagTypeIgnore)
        return platform->default_backgroundcolor;
    
    CEETextAttribute* attribute = platform->attribute_map[type];
    if (!attribute)
        attribute = platform->attribute_map[kCEETagTypePlainText];
    return (CEETextColorRef)attribute->backgroundcolor;
}

static void font_attributes_from_descriptor(const cee_char* descriptor,
                                            cee_char** family,
                                            cee_char** style,
                                            cee_char** size)
{
    const cee_char* pattern = "font\\s*\\(\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*,\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*\\)";
    
    cee_int ret;
    pcre *re = NULL;
    cee_int err;
    cee_int ovector[OVECCOUNT];
    const cee_char *err_msg;
    
    re = pcre_compile((char*)pattern,
                      PCRE_NEWLINE_ANY | PCRE_UTF8,
                      &err_msg,
                      &err,
                      NULL);
    
    memset(ovector, 0, sizeof(int)*OVECCOUNT);
    
    ret = pcre_exec(re,
                    NULL,
                    (char*)descriptor,
                    (int)strlen((char*)descriptor),
                    ovector[1],
                    0,
                    ovector,
                    OVECCOUNT);
    
    if (ret > 0) {
        if (family)
            *family = cee_strndup(&descriptor[ovector[1*2]], 
                                  ovector[1*2+1] - ovector[1*2]);
        if (size)
            *size = cee_strndup(&descriptor[ovector[2*2]], 
                                ovector[2*2+1] - ovector[2*2]);
        if (style)
            *style = cee_strndup(&descriptor[ovector[3*2]], 
                                 ovector[3*2+1] - ovector[3*2]);
    }
    
    if (re)
        pcre_free(re);
}

static CGColorRef color_create_form_descriptor(CEEMacOSPlatform* platform,
                                               const char* descriptor)
{
    CGColorRef color = NULL;
    CGFloat components[4];
    
    if (color_component_from_rgb_descriptor(descriptor, components) ||
        color_component_from_hsb_descriptor(descriptor, components) ||
        color_component_from_hex(descriptor, components)) {
        if (platform->color_space)
            color = CGColorCreate(platform->color_space, components);
    }
    
    if (!color)
        color = CGColorCreateGenericRGB(0.0, 0.0, 0.0, 1.0);
    
    return color;
    
}

static cee_boolean color_component_from_rgb_descriptor(const char* descriptor,
                                                       CGFloat components[4])
{
    cee_int ret = 0;
    pcre *re = NULL;
    cee_int err;
    cee_int ovector[OVECCOUNT];
    const cee_char* err_msg;
    const cee_char* pattern = "(rgba)\\s*\\(\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*\\)";
    
    re = pcre_compile((char*)pattern,
                      PCRE_NEWLINE_ANY | PCRE_UTF8,
                      &err_msg,
                      &err,
                      NULL);
    
    memset(ovector, 0, sizeof(int)*OVECCOUNT);
    
    ret = pcre_exec(re,
                    NULL,
                    (char*)descriptor,
                    (int)strlen((char*)descriptor),
                    ovector[1],
                    0,
                    ovector,
                    OVECCOUNT);
    if (ret < 0) {
        if (re)
            pcre_free(re);
        return FALSE;
    }
    
    cee_char* type = cee_strndup(&descriptor[ovector[1*2]], ovector[1*2+1] - ovector[1*2]);
    cee_char* str0 = cee_strndup(&descriptor[ovector[2*2]], ovector[2*2+1] - ovector[2*2]);
    cee_char* str1 = cee_strndup(&descriptor[ovector[3*2]], ovector[3*2+1] - ovector[3*2]);
    cee_char* str2 = cee_strndup(&descriptor[ovector[4*2]], ovector[4*2+1] - ovector[4*2]);
    cee_char* str3 = cee_strndup(&descriptor[ovector[5*2]], ovector[5*2+1] - ovector[5*2]);
    
    components[0] = atof(str0) / 256.0;
    components[1] = atof(str1) / 256.0;
    components[2] = atof(str2) / 256.0;
    components[3] = atof(str3) / 100.0;
        
    cee_free(str3);
    cee_free(str2);
    cee_free(str1);
    cee_free(str0);
    cee_free(type);
    
    if (re)
        pcre_free(re);
    
    return TRUE;
}

static cee_boolean color_component_from_hsb_descriptor(const char* descriptor,
                                                       CGFloat components[4])
{
    cee_int ret;
    pcre *re = NULL;
    cee_int err;
    cee_int ovector[OVECCOUNT];
    const cee_char* err_msg;
    const cee_char* pattern = "(hsba)\\s*\\(\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*\\)";
    
    re = pcre_compile((char*)pattern,
                      PCRE_NEWLINE_ANY | PCRE_UTF8,
                      &err_msg,
                      &err,
                      NULL);
    
    memset(ovector, 0, sizeof(int)*OVECCOUNT);
    
    ret = pcre_exec(re,
                    NULL,
                    (char*)descriptor,
                    (int)strlen((char*)descriptor),
                    ovector[1],
                    0,
                    ovector,
                    OVECCOUNT);
    if (ret < 0) {
        if (re)
            pcre_free(re);
        return FALSE;
    }
    
    cee_char* type = cee_strndup(&descriptor[ovector[1*2]], ovector[1*2+1] - ovector[1*2]);
    cee_char* str0 = cee_strndup(&descriptor[ovector[2*2]], ovector[2*2+1] - ovector[2*2]);
    cee_char* str1 = cee_strndup(&descriptor[ovector[3*2]], ovector[3*2+1] - ovector[3*2]);
    cee_char* str2 = cee_strndup(&descriptor[ovector[4*2]], ovector[4*2+1] - ovector[4*2]);
    cee_char* str3 = cee_strndup(&descriptor[ovector[5*2]], ovector[5*2+1] - ovector[5*2]);
    
    float component0 = atof(str0);
    float component1 = atof(str1);
    float component2 = atof(str2);
    float component3 = atof(str3);

    cee_float red = 0.0;
    cee_float green = 0.0;
    cee_float blue = 0.0;
    cee_float alpha = 0.0;
    
    hsb2rgb(component0, component1, component2, &red, &green, &blue);
    alpha = component3 / 100.0;
    
    components[0] = red;
    components[1] = green;
    components[2] = blue;
    components[3] = alpha;
    
    cee_free(str3);
    cee_free(str2);
    cee_free(str1);
    cee_free(str0);
    cee_free(type);
    
    if (re)
        pcre_free(re);
    
    return TRUE;
}


static cee_boolean color_component_from_hex(const char* descriptor,
                                            CGFloat components[4])
{
    cee_int ret;
    pcre *re = NULL;
    cee_int err;
    cee_int ovector[OVECCOUNT];
    const cee_char* err_msg;
    const cee_char* pattern = "#[0-9a-fA-F]+";
    
    re = pcre_compile((char*)pattern,
                      PCRE_NEWLINE_ANY | PCRE_UTF8,
                      &err_msg,
                      &err,
                      NULL);
    
    memset(ovector, 0, sizeof(int)*OVECCOUNT);
    
    ret = pcre_exec(re,
                    NULL,
                    (char*)descriptor,
                    (int)strlen((char*)descriptor),
                    ovector[1],
                    0,
                    ovector,
                    OVECCOUNT);
    if (ret < 0) {
        if (re)
            pcre_free(re);
        return FALSE;
    }
    
    long int hexValue = strtol(&descriptor[ovector[0] + 1], NULL, 16);
    
    components[0] = (CGFloat)(((unsigned char)(hexValue >> 16)) / 256.0);
    components[1] = (CGFloat)(((unsigned char)(hexValue >> 8)) / 256.0);
    components[2] = (CGFloat)(((unsigned char)(hexValue)) / 256.0);
    components[3] = 1.0;
    
    
    if (re)
        pcre_free(re);
    
    return TRUE;
}

static void hsb2rgb(cee_float hue,
                     cee_float saturation,
                     cee_float brightness,
                     cee_float *red,
                     cee_float *green,
                     cee_float *blue)
{
    int i;
    float f, p, q, t;
    
    saturation /= 100.0;
    brightness /= 100.0;
        
    if(saturation < FLT_EPSILON) {
        // achromatic (grey)
        *red = *green = *blue = brightness;
        return;
    }
    
    hue /= 60; // sector 0 to 5
    i = (int)floorf(hue);
    f = hue - i; // factorial part of h
    p = brightness * (1 - saturation);
    q = brightness * (1 - (saturation * f));
    t = brightness * (1 - (saturation * (1 - f)));
    
    switch (i) {
        case 0:
            *red = brightness;
            *green = t;
            *blue = p;
            break;
            
        case 1:
            *red = q;
            *green = brightness;
            *blue = p;
            break;
            
        case 2:
            *red = p;
            *green = brightness;
            *blue = t;
            break;
            
        case 3:
            *red = p;
            *green = q;
            *blue = brightness;
            break;
            
        case 4:
            *red = t;
            *green = p;
            *blue = brightness;
            break;
            
        default: // case 5:
            *red = brightness;
            *green = p;
            *blue = q;
            break;
    }
    return ;
}

static void color_free(cee_pointer color)
{
    CGColorRelease(color);
}
