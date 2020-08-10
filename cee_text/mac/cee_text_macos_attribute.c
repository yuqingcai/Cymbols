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

static const cee_uchar* default_scheme = (const cee_uchar*)"{\
    \"name\" : \"cymbols\",\
    \"rules\" : [\
    {\
        \"identifier\" : \"plantext\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(20, 20, 20, 100)\",\
        \"backgroundcolor\" : \"rgba(0, 0, 0, 0)\"\
    },\
    {\
        \"identifier\" : \"c_comment\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"italic\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"true\",\
        \"forecolor\" : \"rgba(100, 100, 100, 100)\",\
        \"backgroundcolor\" : \"rgba(0, 0, 0, 0)\"\
    },\
    {\
        \"identifier\" : \"cpp_comment\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"italic\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"true\",\
        \"forecolor\" : \"rgba(100, 100, 100, 100)\",\
        \"backgroundcolor\" : \"rgba(0, 0, 0, 0)\"\
    },\
    {\
        \"identifier\" : \"keyword\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(0, 80, 100, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"literal\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(23, 100, 100, 100)\",\
        \"backgroundcolor\" : \"rgba(255, 255, 187, 0)\"\
    },\
    {\
        \"identifier\" : \"character\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(230, 80, 0, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"identifier\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 255, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"constant\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(255, 0, 0, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"attribute\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"class\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(230, 100, 30, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"member\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250.0, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"object\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250.0, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"function\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"bold\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(230, 100, 30, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"variable\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"parameter\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"italic\\\", 5.0)\",\
        \"underline\" : \"true\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(230, 100, 30, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"prep_directive\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"bold\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(0, 80, 100, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"prep_replacement\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(230, 100, 30, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"prep_condition\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(100, 100, 60, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"enum\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"union\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"namespace\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"template\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"declarator\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"rgba(0, 0, 250, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"punctuation\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(297, 70, 50, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    },\
    {\
        \"identifier\" : \"reference\",\
        \"font\" : \"font(\\\"Courier\\\", 12.0, \\\"regular\\\", 5.0)\",\
        \"underline\" : \"false\",\
        \"strikethrough\" : \"false\",\
        \"forecolor\" : \"hsba(100, 100, 60, 100)\",\
        \"backgroundcolor\" : \"rgba(20, 20, 20, 0)\"\
    }\
    ]\
}";

typedef struct _CEETextFallbackFontItem {
    cee_float size;
    CEEList* fonts;
} CEETextFallbackFontItem;

typedef struct _CEEMacOSPlatform {
    cee_char* name;
    CEEList* attributes;
    CEETextAttribute* attribute_map[kCEETagTypeMax];
    CEEList* fallback_font_names;
    CEEList* fallback_font_items;
} CEEMacOSPlatform;

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
    
    if (ret) {
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

static CGColorRef color_create_form_descriptor(const char* descriptor)
{
    CGColorRef color = NULL;
    
    cee_int ret;
    pcre *re = NULL;
    cee_int err;
    cee_int ovector[OVECCOUNT];
    const cee_char *err_msg;
    const cee_char* pattern = "(rgba|hsba)\\s*\\(\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*\\)";
    
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
    if (ret) {
        cee_char* type = cee_strndup(&descriptor[ovector[1*2]], 
                                     ovector[1*2+1] - ovector[1*2]);
        cee_char* str0 = cee_strndup(&descriptor[ovector[2*2]], 
                                     ovector[2*2+1] - ovector[2*2]);
        cee_char* str1 = cee_strndup(&descriptor[ovector[3*2]], 
                                     ovector[3*2+1] - ovector[3*2]);
        cee_char* str2 = cee_strndup(&descriptor[ovector[4*2]], 
                                     ovector[4*2+1] - ovector[4*2]);
        cee_char* str3 = cee_strndup(&descriptor[ovector[5*2]], 
                                     ovector[5*2+1] - ovector[5*2]);
        
        cee_float component0 = atof(str0);
        cee_float component1 = atof(str1);
        cee_float component2 = atof(str2);
        cee_float component3 = atof(str3);
        
        cee_float red = 0.0;
        cee_float green = 0.0;
        cee_float blue = 0.0;
        cee_float alpha = 0.0;

        alpha = component3 / 100.0;
        
        if (!strcmp(type, "rgba")) {
            red = component0 / 255.0;
            green = component1 / 255.0;
            blue = component2 / 255.0;
        }
        else if (!strcmp(type, "hsba")) {
            hsb2rgb(component0, 
                    component1, 
                    component2,
                    &red,
                    &green,
                    &blue);
        }

        color = CGColorCreateGenericRGB(red, 
                                        green, 
                                        blue, 
                                        alpha);

        cee_free(str3);
        cee_free(str2);
        cee_free(str1);
        cee_free(str0);
        cee_free(type);
    }
    
    if (re)
        pcre_free(re);
    
    return color;
}

static void color_free(cee_pointer color)
{
    CFRelease(color);
}

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
        attribute = platform->attribute_map[kCEETagTypePlanText];
    
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

static CEETextAttribute* text_attribute_create(CEETextFontRef font,
                                               cee_float font_size,
                                               cee_boolean underline,
                                               cee_boolean strikethrough,
                                               CGColorRef forecolor,
                                               CGColorRef backgroundcolor)
{
    CEETextAttribute* attribute = 
        (CEETextAttribute*)cee_malloc0(sizeof(CEETextAttribute));
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
    CEETextAttribute* attribute = platform->attribute_map[kCEETagTypePlanText];
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
    
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlanText);
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
    
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlanText);
    CTFontGetGlyphsForCharacters(font, characters, glyphs, length);
    CTFontGetAdvancesForGlyphs(font, kCTFontOrientationHorizontal, glyphs, advances, 1);
    return advances[0].width;
}

static cee_float unknow_glyph_height_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlanText);
    return (CTFontGetDescent(font) + CTFontGetAscent(font));
}

static cee_float unknow_glyph_ascent_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    CTFontRef font = font_get(platform, codepoint, kCEETagTypePlanText);
    return CTFontGetAscent(font);
}

static CEETextFontRef unknow_glyph_font_get(CEEMacOSPlatform* platform)
{
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_UNKONW;
    return (CEETextFontRef)font_get(platform, codepoint, kCEETagTypePlanText);
}

cee_pointer cee_text_platform_create(void)
{
    CEEMacOSPlatform* platform = 
        (CEEMacOSPlatform*)cee_malloc0(sizeof(CEEMacOSPlatform));
    platform->name = cee_strdup("MacOS");
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
    
    cee_free(platform);
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
    cee_boolean ret = TRUE;
    const cee_uchar* json_string = NULL;
    if (!descriptor)
        json_string = default_scheme;
    else
        json_string = descriptor;
    
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    if (platform->attributes)
        cee_list_free_full(platform->attributes, text_attribute_free);
    platform->attributes = NULL;
    
    for (int i = 0; i < kCEETagTypeMax; i ++)
        platform->attribute_map[i] = NULL;
    
    cJSON *attribute_items = cJSON_Parse((const char*)json_string);
    if (!attribute_items) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            fprintf(stderr, "Error before: %s\n", error_ptr);
        ret = FALSE;
        goto exit;
    }
    
    CEEList* attributes = NULL;
    const cJSON *rule = NULL;
    cJSON* identifier_item = NULL;
    cJSON* font_item = NULL;
    cJSON* underline_item = NULL;
    cJSON* strikethrough_item = NULL;
    cJSON* forecolor_item = NULL;
    cJSON* backgroundcolor_item = NULL;
    CEETextAttribute* attribute = NULL;
    CTFontRef font = NULL;
    CGColorRef forecolor = NULL;
    CGColorRef backgroundcolor = NULL;
    cee_boolean underline = FALSE;
    cee_boolean strikethrough = FALSE;
    cee_char* font_size = NULL;
    CEETextAttribute* default_attribute = NULL;
    
    const cJSON *rules = cJSON_GetObjectItemCaseSensitive(attribute_items, "rules");
    if (rules) {
        struct SyntaxColorSchemeRule {
            CEETagType tag;
            cee_char* identifier;
        } rule_map[] = {
            { kCEETagTypePlanText,                          "plantext"                              },
            { kCEETagTypeCKeyword,                          "keyword"                               },
            { kCEETagTypeCComment,                          "c_comment"                             },
            { kCEETagTypeCPPComment,                        "cpp_comment"                           },
            { kCEETagTypeCConstant,                         "constant"                              },
            { kCEETagTypeCCharacter,                        "character"                             },
            { kCEETagTypeCLiteral,                          "literal"                               },
            { kCEETagTypeCPunctuation,                      "punctuation"                           },
            { kCEETagTypeCPrepDirective,                    "prep_directive"                        },
            //{ kCEETagTypeCPrepFilename,                     "literal"                               },
            { kCEETagTypeCPrepDirectiveDefine,              "prep_replacement"                      },
            { kCEETagTypeCPrepDirectiveDefineParameter,     "parameter"                             },
            { kCEETagTypeCNamespaceDeclaration,             "namespace"                             },
            { kCEETagTypeCNamespaceDefinition,              "namespace"                             },
            { kCEETagTypeCFunctionDefinition,               "function"                              },
            { kCEETagTypeCFunctionDeclaration,              "function"                              },
            { kCEETagTypeCCatchBlock,                       "keyword"                               },
            { kCEETagTypeCVariable,                         "variable"                              },
            { kCEETagTypeCTypeDeclaration,                  "type_declaration"                      },
            { kCEETagTypeOCProperty,                        "variable"                              },
            { kCEETagTypeCMessageDeclaration,               "function"                              },
            { kCEETagTypeCMessageDefinition,                "function"                              },
            { kCEETagTypeCFunctionParameter,                "parameter"                             },
            { kCEETagTypeCMessageParameter,                 "parameter"                             },
            { kCEETagTypeCTypeDef,                          "type_declaration"                      },
            { kCEETagTypeCClassDefinition,                  "class"                                 },
            { kCEETagTypeCEnumDefinition,                   "enum"                                  },
            { kCEETagTypeCUnionDefinition,                  "union"                                 },
            { kCEETagTypeCEnumerator,                       "enumerator"                            },
            { kCEETagTypeOCInterface,                       "class"                                 },
            { kCEETagTypeOCImplementation,                  "class"                                 },
            { kCEETagTypeOCProtocol,                        "class"                                 },
            { kCEETagTypeCOperatorDeclaration,              "keyword"                               },
            { kCEETagTypeCOperatorDefinition,               "function"                              },
            { kCEETagTypeCTypedefReference,                 "type_define_reference"                 },
            { kCEETagTypeCTypeReference,                    "type_reference"                        },
            { kCEETagTypeCPrepDirectiveReference,           "prep_directive_reference"              },
            { kCEETagTypeCPrepDirectiveParameterReference,  "prep_directive_parameter_reference"    },
            { kCEETagTypeCVariableReference,                "variable_reference"                    },
            { kCEETagTypeCFunctionReference,                "function_reference"                    },
            { kCEETagTypeCMessageReference,                 "message_reference"                     },
            { kCEETagTypeCMemberReference,                  "member_reference"                      },
            
        };
        
        int map_size = sizeof (rule_map) / sizeof(rule_map[0]);
        for (int i = 0; i < kCEETagTypeMax; i ++)
            platform->attribute_map[i] = NULL;
        
        cJSON_ArrayForEach(rule, rules) {
            identifier_item = cJSON_GetObjectItemCaseSensitive(rule, "identifier");
            font_item = cJSON_GetObjectItemCaseSensitive(rule, "font");
            underline_item = cJSON_GetObjectItemCaseSensitive(rule, "underline");
            strikethrough_item = cJSON_GetObjectItemCaseSensitive(rule, "strikethrough");
            forecolor_item = cJSON_GetObjectItemCaseSensitive(rule, "forecolor");
            backgroundcolor_item = cJSON_GetObjectItemCaseSensitive(rule, "backgroundcolor");
            underline = FALSE;
            strikethrough = FALSE;
            
            CEEList* tag_indexes = NULL;
            CEEList* p = NULL;
            cee_long index = -1;
            
            for (int i = 0; i < map_size; i ++) {
                if (!strcmp(identifier_item->valuestring, rule_map[i].identifier))
                    tag_indexes = cee_list_prepend(tag_indexes, CEE_INT_TO_POINTER(rule_map[i].tag));
            }
            
            font_attributes_from_descriptor(font_item->valuestring, NULL, NULL, &font_size);
            font = font_create_from_descriptor(font_item->valuestring);
            forecolor = color_create_form_descriptor(forecolor_item->valuestring);
            backgroundcolor = color_create_form_descriptor(backgroundcolor_item->valuestring);
            
            if (!strcmp(underline_item->valuestring, "true"))
                underline = TRUE;
            if (!strcmp(strikethrough_item->valuestring, "true"))
                strikethrough = TRUE;
            
            attribute = text_attribute_create((CEETextFontRef)font,
                                              atof(font_size),
                                              underline, 
                                              strikethrough, 
                                              forecolor,
                                              backgroundcolor);
            attributes = cee_list_prepend(attributes, attribute);
            
            p = tag_indexes;
            while (p) {
                index = CEE_POINTER_TO_INT(p->data);
                platform->attribute_map[index] = attribute;
                p = p->next;
            }
            cee_free(font_size);
            cee_list_free(tag_indexes);

            if (!default_attribute) {
                if (!strcmp(identifier_item->valuestring, "plantext"))
                    default_attribute = attribute;
            }
        }
    }
    else {
        const cJSON *attributes_item = cJSON_GetObjectItemCaseSensitive(attribute_items, "attributes");
        font_item = cJSON_GetObjectItemCaseSensitive(attributes_item, "font");
        forecolor_item = cJSON_GetObjectItemCaseSensitive(attributes_item, "forecolor");
        
        font_attributes_from_descriptor(font_item->valuestring, NULL, NULL, &font_size);
        font = font_create_from_descriptor(font_item->valuestring);
        forecolor = color_create_form_descriptor(forecolor_item->valuestring);
        attribute = text_attribute_create((CEETextFontRef)font,
                                          atof(font_size),
                                          FALSE, 
                                          FALSE, 
                                          forecolor,
                                          NULL);
        attributes = cee_list_append(attributes, attribute);
        default_attribute = attribute;
    }
    
    cJSON_Delete(attribute_items);
    
    if (!platform->attribute_map[kCEETagTypePlanText])
        platform->attribute_map[kCEETagTypePlanText] = default_attribute;
    
    platform->attributes = attributes;
    platform->fallback_font_names = fallback_font_names_create(platform);
    
exit:
    
    return ret;
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
        attribute = platform->attribute_map[kCEETagTypePlanText];
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
    CEETextAttribute* attribute = platform->attribute_map[type];
    if (!attribute)
        attribute = platform->attribute_map[kCEETagTypePlanText];
    return (CEETextColorRef)attribute->forecolor;
}

CEETextColorRef cee_text_platform_background_color_get(cee_pointer platform_ref,
                                                      CEEUnicodePoint codepoint,
                                                      CEETagType type)
{
    CEEMacOSPlatform* platform = (CEEMacOSPlatform*)platform_ref;
    CEETextAttribute* attribute = platform->attribute_map[type];
    if (!attribute)
        attribute = platform->attribute_map[kCEETagTypePlanText];
    return (CEETextColorRef)attribute->backgroundcolor;
}
