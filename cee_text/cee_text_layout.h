#ifndef __CEE_TEXT_LAYOUT_H__
#define __CEE_TEXT_LAYOUT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"
#include "cee_text_storage.h"

#ifdef __APPLE__
#include "cee_text_macos_attribute.h"
#elif define Q_OS_LINUX || Q_OS_WIN32
#endif

typedef enum _CEETextLayoutAlignment {
    kCEETextLayoutAlignmentLeft,
    kCEETextLayoutAlignmentCenter,
    kCEETextLayoutAlignmentRight    
} CEETextLayoutAlignment;

typedef struct _CEETextLayout* CEETextLayoutRef;
typedef struct _CEETextLine* CEETextLineRef;
typedef struct _CEETextUnit* CEETextUnitRef;

CEETextLayoutRef cee_text_layout_create(CEETextStorageRef storage,
                                        cee_pointer platform,
                                        CEETextLayoutAlignment aligment,
                                        cee_ulong tab_size,
                                        cee_float line_space,
                                        cee_float left_margin,
                                        cee_float right_margin,
                                        cee_float top_margin,
                                        cee_float bottom_margin,
                                        CEERect bounds);
cee_float cee_text_layout_line_space_get(CEETextLayoutRef layout);
void cee_text_layout_storage_set(CEETextLayoutRef layout,
                                 CEETextStorageRef storage);
void cee_text_layout_aligment_set(CEETextLayoutRef layout,
                                  CEETextLayoutAlignment aligment);
void cee_text_layout_platform_set(CEETextLayoutRef layout,
                                  cee_pointer platform);
cee_pointer cee_text_layout_platform_get(CEETextLayoutRef layout);
CEETextColorRef cee_text_layout_platform_background_color_get(cee_pointer platform);
void cee_text_layout_run(CEETextLayoutRef layout);
cee_boolean cee_text_layout_paragraph_index_is_invalid(CEETextLayoutRef layout);
void cee_text_layout_free(CEETextLayoutRef layout);
void cee_text_layout_bounds_set(CEETextLayoutRef layout,
                                CEERect bounds);
void cee_text_layout_horizontal_offset_set(CEETextLayoutRef layout,
                                           cee_float offset);
cee_float cee_text_layout_horizontal_offset_get(CEETextLayoutRef layout);

CEERect cee_text_layout_bounds_get(CEETextLayoutRef layout);
void cee_text_layout_paragraph_index_set(CEETextLayoutRef layout,
                                         cee_long paragraph_index);
cee_long cee_text_layout_paragraph_index_get(CEETextLayoutRef layout);
CEEList* cee_text_layout_lines_get(CEETextLayoutRef layout);
cee_ulong cee_text_layout_paragraph_count_get(CEETextLayoutRef layout);
cee_float cee_text_layout_max_line_width_get(CEETextLayoutRef layout);
CEEList* cee_text_layout_line_boxes_create(CEETextLayoutRef layout,
                                           cee_long from,
                                           cee_long to);
cee_float cee_text_line_base_get(CEETextLineRef line);
cee_float cee_text_line_asent_get(CEETextLineRef line);
cee_boolean cee_text_unit_under_line_get(CEETextUnitRef unit);
CEEList* cee_text_line_units_get(CEETextLineRef line);
cee_ulong cee_text_line_unit_count_get(CEETextLineRef line);
cee_float cee_text_line_margin_get(CEETextLineRef line);
CEERect cee_text_line_bounds_get(CEETextLineRef line);
CEERect cee_text_line_box_get(CEETextLineRef line);
cee_long cee_text_line_paragraph_index_get(CEETextLineRef line);
CEEUnicodePoint cee_text_unit_codepoint_get(CEETextUnitRef unit);
CEETextGlyphRef cee_text_unit_glyph_get(CEETextUnitRef unit);
CEETextFontRef cee_text_unit_font_get(CEETextUnitRef unit);
CEETextColorRef cee_text_unit_fore_color_get(CEETextUnitRef unit);
CEETextColorRef cee_text_unit_background_color_get(CEETextUnitRef unit);
CEERect cee_text_unit_bounds_get(CEETextUnitRef unit);
CEERect cee_text_unit_box_get(CEETextUnitRef unit);
cee_long cee_text_unit_buffer_offset_get(CEETextUnitRef unit);
cee_ulong cee_text_unit_buffer_length_get(CEETextUnitRef unit);
cee_ulong cee_text_unit_index_get(CEETextUnitRef unit);
CEETextStorageRef cee_text_layout_storage_get(CEETextLayoutRef layout);
CEETextUnitRef cee_text_unit_get_by_location(CEETextLayoutRef layout,
                                             CEEPoint location);
cee_float cee_text_unit_ascent_get(CEETextUnitRef unit);
CEETextLineRef cee_text_layout_head_line_get(CEETextLayoutRef layout);
CEETextUnitRef cee_text_layout_head_unit_get(CEETextLayoutRef layout);
CEETextLineRef cee_text_layout_tail_line_get(CEETextLayoutRef layout);
CEETextUnitRef cee_text_layout_tail_unit_get(CEETextLayoutRef layout);
CEEList* cee_text_layout_rects_create(CEETextLayoutRef layout,
                                      CEERange range);
CEETextUnitRef cee_text_unit_get_by_offset(CEETextLayoutRef layout,
                                           cee_long buffer_offset);
CEETextLineRef cee_text_line_get_by_offset(CEETextLayoutRef layout,
                                           cee_long buffer_offset);
void cee_text_layout_attribute_generator_set(CEETextLayoutRef layout,
                                             cee_pointer generator);
void cee_text_layout_attribute_generate_set(CEETextLayoutRef layout,
                                            CEEList* (*generate)(cee_pointer,
                                                                 CEERange));
CEETag* cee_text_layout_tag_get(CEETextLayoutRef layout,
                                cee_long buffer_offset);
void cee_text_layout_reset(CEETextLayoutRef layout);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TEXT_LAYOUT_H__ */
