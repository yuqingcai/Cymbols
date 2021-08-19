#ifndef __CEE_ASCII_LAYOUT_H__
#define __CEE_ASCII_LAYOUT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"
#include "cee_ascii_storage.h"

#ifdef __APPLE__
#include "cee_text_macos_attribute.h"
#elif define Q_OS_LINUX || Q_OS_WIN32
#endif

typedef struct _CEEASCIILayout* CEEASCIILayoutRef;
typedef struct _CEEASCIILine* CEEASCIILineRef;
typedef struct _CEEASCIIUnit* CEEASCIIUnitRef;

void cee_ascii_layout_free(CEEASCIILayoutRef layout);
CEEASCIILayoutRef cee_ascii_layout_create(CEEASCIIStorageRef storage,
                                          cee_pointer platform,
                                          cee_uint character_per_line,
                                          cee_float line_space,
                                          cee_float left_margin,
                                          cee_float right_margin,
                                          cee_float top_margin,
                                          cee_float bottom_margin,
                                          CEERect bounds);
void cee_ascii_layout_run(CEEASCIILayoutRef layout);
void cee_ascii_layout_bounds_set(CEEASCIILayoutRef layout,
                                 CEERect bounds);
void cee_ascii_layout_storage_set(CEEASCIILayoutRef layout,
                                  CEEASCIIStorageRef storage);
void cee_ascii_layout_reset(CEEASCIILayoutRef layout);
cee_pointer cee_ascii_layout_platform_get(CEEASCIILayoutRef layout);
CEEList* cee_ascii_layout_lines_get(CEEASCIILayoutRef layout);
CEEList* cee_ascii_line_units_get(CEEASCIILineRef line);
cee_float cee_ascii_line_base_get(CEEASCIILineRef line);
cee_float cee_ascii_line_asent_get(CEEASCIILineRef line);
cee_ulong cee_ascii_line_unit_count_get(CEEASCIILineRef line);
cee_float cee_ascii_line_margin_get(CEEASCIILineRef line);
CEERect cee_ascii_line_bounds_get(CEEASCIILineRef line);
CEETextGlyphRef cee_ascii_unit_glyph_get(CEEASCIIUnitRef unit);
CEETextFontRef cee_ascii_unit_font_get(CEEASCIIUnitRef unit);
CEETextColorRef cee_ascii_unit_fore_color_get(CEEASCIIUnitRef unit);
CEETextColorRef cee_ascii_unit_background_color_get(CEEASCIIUnitRef unit);
CEERect cee_ascii_unit_bounds_get(CEEASCIIUnitRef unit);
cee_float cee_ascii_unit_ascent_get(CEEASCIIUnitRef unit);
cee_boolean cee_ascii_unit_under_line_get(CEEASCIIUnitRef unit);
cee_long cee_ascii_unit_buffer_offset_get(CEEASCIIUnitRef unit);
cee_long cee_ascii_layout_paragraph_index_get(CEEASCIILayoutRef layout);
cee_long cee_ascii_layout_paragraph_count_get(CEEASCIILayoutRef layout);
cee_ulong cee_ascii_layout_paragraph_count_max_get(CEEASCIILayoutRef layout);
cee_float cee_ascii_layout_max_line_width_get(CEEASCIILayoutRef layout);
cee_float cee_ascii_layout_horizontal_offset_get(CEEASCIILayoutRef layout);
void cee_ascii_layout_paragraph_index_set(CEEASCIILayoutRef layout,
                                           cee_long index);
void cee_ascii_layout_horizontal_offset_set(CEEASCIILayoutRef layout,
                                            cee_float offset);
cee_long cee_ascii_layout_buffer_offset_by_paragraph_index(CEEASCIILayoutRef layout,
                                                           cee_long index);
cee_long cee_ascii_layout_paragraph_index_by_buffer_offset(CEEASCIILayoutRef layout,
                                                           cee_long offset);
cee_uint cee_ascii_layout_characters_per_line(CEEASCIILayoutRef layout);
cee_long cee_ascii_line_paragraph_index_get(CEEASCIILineRef line);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_ASCII_LAYOUT_H__ */
