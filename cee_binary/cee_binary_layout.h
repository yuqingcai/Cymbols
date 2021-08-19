#ifndef __CEE_BINARY_LAYOUT_H__
#define __CEE_BINARY_LAYOUT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"
#include "cee_binary_storage.h"

#ifdef __APPLE__
#include "cee_text_macos_attribute.h"
#elif define Q_OS_LINUX || Q_OS_WIN32
#endif

typedef struct _CEEBinaryLayout* CEEBinaryLayoutRef;
typedef struct _CEEBinaryLine* CEEBinaryLineRef;
typedef struct _CEEBinaryUnit* CEEBinaryUnitRef;

void cee_binary_layout_free(CEEBinaryLayoutRef layout);
CEEBinaryLayoutRef cee_binary_layout_create(CEEBinaryStorageRef storage,
                                            cee_pointer platform,
                                            cee_uint character_per_line,
                                            cee_float line_space,
                                            cee_float left_margin,
                                            cee_float right_margin,
                                            cee_float top_margin,
                                            cee_float bottom_margin,
                                            CEERect bounds);
void cee_binary_layout_run(CEEBinaryLayoutRef layout);
void cee_binary_layout_bounds_set(CEEBinaryLayoutRef layout,
                                  CEERect bounds);
void cee_binary_layout_storage_set(CEEBinaryLayoutRef layout,
                                   CEEBinaryStorageRef storage);
void cee_binary_layout_reset(CEEBinaryLayoutRef layout);
cee_pointer cee_binary_layout_platform_get(CEEBinaryLayoutRef layout);
CEEList* cee_binary_layout_lines_get(CEEBinaryLayoutRef layout);
CEEList* cee_binary_line_units_get(CEEBinaryLineRef line);
cee_float cee_binary_line_base_get(CEEBinaryLineRef line);
cee_float cee_binary_line_asent_get(CEEBinaryLineRef line);
cee_ulong cee_binary_line_unit_count_get(CEEBinaryLineRef line);
cee_float cee_binary_line_margin_get(CEEBinaryLineRef line);
CEERect cee_binary_line_bounds_get(CEEBinaryLineRef line);
CEETextGlyphRef cee_binary_unit_glyph_get(CEEBinaryUnitRef unit);
CEETextFontRef cee_binary_unit_font_get(CEEBinaryUnitRef unit);
CEETextColorRef cee_binary_unit_fore_color_get(CEEBinaryUnitRef unit);
CEETextColorRef cee_binary_unit_background_color_get(CEEBinaryUnitRef unit);
CEERect cee_binary_unit_bounds_get(CEEBinaryUnitRef unit);
cee_float cee_binary_unit_ascent_get(CEEBinaryUnitRef unit);
cee_boolean cee_binary_unit_under_line_get(CEEBinaryUnitRef unit);
cee_long cee_binary_unit_buffer_offset_get(CEEBinaryUnitRef unit);
cee_long cee_binary_layout_paragraph_index_get(CEEBinaryLayoutRef layout);
cee_long cee_binary_layout_paragraph_count_get(CEEBinaryLayoutRef layout);
cee_ulong cee_binary_layout_paragraph_count_max_get(CEEBinaryLayoutRef layout);
cee_float cee_binary_layout_max_line_width_get(CEEBinaryLayoutRef layout);
cee_float cee_binary_layout_horizontal_offset_get(CEEBinaryLayoutRef layout);
void cee_binary_layout_paragraph_index_set(CEEBinaryLayoutRef layout,
                                           cee_long index);
void cee_binary_layout_horizontal_offset_set(CEEBinaryLayoutRef layout,
                                             cee_float offset);
cee_long cee_binary_layout_buffer_offset_by_paragraph_index(CEEBinaryLayoutRef layout,
                                                            cee_long index);
cee_long cee_binary_layout_paragraph_index_by_buffer_offset(CEEBinaryLayoutRef layout,
                                                            cee_long offset);
cee_uint cee_binary_layout_characters_per_line(CEEBinaryLayoutRef layout);
cee_long cee_binary_line_paragraph_index_get(CEEBinaryLineRef line);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BINARY_LAYOUT_H__ */
