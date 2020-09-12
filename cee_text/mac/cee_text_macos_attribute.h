//
//  cee_text_macos_attribute.h
//  Cymbols
//
//  Created by qing on 2019/11/21.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#ifndef __CEE_TEXT_MACOS_ATTRIBUTE_H__
#define __CEE_TEXT_MACOS_ATTRIBUTE_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_tag.h"
#include <CoreText/CoreText.h>
#include <CoreGraphics/CoreGraphics.h>

typedef struct _CEETextFont* CEETextFontRef;
typedef struct _CEETextColor* CEETextColorRef;
typedef struct _CEETextGlyph* CEETextGlyphRef;

typedef struct _CEETextAttribute {
    cee_char* name;
    CEETextFontRef font;
    cee_float font_size;
    CEETextColorRef forecolor;
    CEETextColorRef backgroundcolor;
    cee_boolean underline;
    cee_boolean strikethrough;
} CEETextAttribute;

typedef struct _CEETextAttribute* CEETextAttributeRef;

cee_pointer cee_text_platform_create(void);
void cee_text_platform_free(cee_pointer platform_ref);
const cee_char* cee_text_platform_name_get(cee_pointer platform_ref);
void cee_text_platform_attributes_create(cee_pointer platform_ref);
cee_boolean cee_text_platform_configure(cee_pointer platform_ref,
                                        const cee_uchar* descriptor);
CEETextGlyphRef cee_text_platform_glyph_get(cee_pointer platform_ref,
                                            CEEUnicodePoint codepoint,
                                            CEETagType type);
cee_float cee_text_platform_width_get(cee_pointer platform_ref,
                                      CEEUnicodePoint codepoint,
                                      CEETagType type);
cee_float cee_text_platform_height_get(cee_pointer platform_ref,
                                       CEEUnicodePoint codepoint,
                                       CEETagType type);
cee_float cee_text_platform_ascent_get(cee_pointer platform_ref,
                                       CEEUnicodePoint codepoint,
                                       CEETagType type);
cee_boolean cee_text_platform_under_line_get(cee_pointer platform_ref,
                                             CEEUnicodePoint codepoint,
                                             CEETagType type);
CEETextFontRef cee_text_platform_font_get(cee_pointer platform_ref,
                                          CEEUnicodePoint codepoint,
                                          CEETagType type);
CEETextColorRef cee_text_platform_fore_color_get(cee_pointer platform_ref,
                                                 CEEUnicodePoint codepoint,
                                                 CEETagType type);
CEETextColorRef cee_text_platform_background_color_get(cee_pointer platform_ref,
                                                       CEEUnicodePoint codepoint,
                                                       CEETagType type);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __CEE_TEXT_MACOS_ATTRIBUTE_H__ */
