#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include "cee_binary_layout.h"

typedef struct _CEEBinaryUnit {
    struct _CEEBinaryLine* line_ref;
    cee_long index;
    CEEUnicodePoint codepoint;
    CEETextGlyphRef glyph_ref;
    CEETextColorRef fore_color_ref;
    CEETextColorRef background_color_ref;
    CEETextFontRef font_ref;
    cee_float ascent;
    CEERect bounds;
    cee_boolean under_line;
} CEEBinaryUnit;

typedef struct _CEEBinaryLine {
    struct _CEEBinaryLayout* layout_ref;
    cee_long paragraph_index;
    cee_float margin;
    CEEList* units;
    CEERect bounds;
    cee_float base;
    cee_ulong nb_unit;
    cee_float anchor;
    cee_float asent;
} CEEBinaryLine;

typedef struct _CEEBinaryLayout {
    CEERect bounds;
    cee_float line_space;
    cee_pointer platform_ref;
    CEEBinaryStorageRef storage_ref;
    cee_long paragraph_index;
    cee_ulong nb_paragraph;
    cee_ulong nb_paragraph_max;
    CEEList* lines;
    cee_float max_line_width;
    cee_float horizontal_offset;
    cee_float left_margin;
    cee_float right_margin;
    cee_float top_margin;
    cee_float bottom_margin;
    cee_uint character_per_line;
    cee_float anchor;
} CEEBinaryLayout;



static void binary_layout(CEEBinaryLayoutRef layout);
static void line_free(cee_pointer data);
static CEEBinaryLine* line_create(CEEBinaryLayout* layout);
static void line_prepend_unit(CEEBinaryLine* line,
                              CEEBinaryUnit* unit);
static void line_units_reverse(CEEBinaryLine* line);
static cee_boolean line_clipped_vertical(CEEBinaryLine* line);
static void line_paragraph_index_set(CEEBinaryLine* line,
                                     cee_long index);
static void line_unit_interval_increase(CEEBinaryLine* line);
static void line_vertical_position(CEEBinaryLine* line);
static void line_vertical_align(CEEBinaryLine* line);
static void layout_append_line(CEEBinaryLine* line);
static void unit_free(cee_pointer data);
static CEEBinaryUnit* unit_create(CEEBinaryLine* line,
                                  CEEUnicodePoint codepoint);


void cee_binary_layout_free(CEEBinaryLayoutRef layout)
{
    if (!layout)
        return;
        
    if (layout->lines)
        cee_list_free_full(layout->lines, line_free);
            
    cee_free(layout);
}

CEEBinaryLayoutRef cee_binary_layout_create(CEEBinaryStorageRef storage,
                                            cee_pointer platform,
                                            cee_uint character_per_line,
                                            cee_float line_space,
                                            cee_float left_margin,
                                            cee_float right_margin,
                                            cee_float top_margin,
                                            cee_float bottom_margin,
                                            CEERect bounds)
{
    CEEBinaryLayout* layout = (CEEBinaryLayout*)cee_malloc0(sizeof(CEEBinaryLayout));
    
    layout->storage_ref = storage;
    layout->platform_ref = platform;
    layout->character_per_line = character_per_line;
    layout->line_space = line_space;
    layout->bounds = bounds;
    layout->left_margin = left_margin;
    layout->right_margin = right_margin;
    layout->top_margin = top_margin;
    layout->bottom_margin = bottom_margin;
    return layout;
}

void cee_binary_layout_run(CEEBinaryLayoutRef layout)
{
    binary_layout(layout);
}

void cee_binary_layout_bounds_set(CEEBinaryLayoutRef layout,
                                  CEERect bounds)
{
    layout->bounds = bounds;
    layout->nb_paragraph_max = 0;
}

void cee_binary_layout_storage_set(CEEBinaryLayoutRef layout,
                                   CEEBinaryStorageRef storage)
{
    layout->storage_ref = storage;
}

void cee_binary_layout_reset(CEEBinaryLayout* layout)
{
    layout->max_line_width = 0.0;
    layout->horizontal_offset = 0.0;
    layout->paragraph_index = 0;
}

cee_pointer cee_binary_layout_platform_get(CEEBinaryLayoutRef layout)
{
    return layout->platform_ref;
}

CEEList* cee_binary_layout_lines_get(CEEBinaryLayoutRef layout)
{
    return layout->lines;
}

CEEList* cee_binary_line_units_get(CEEBinaryLineRef line)
{
    return line->units;
}

cee_float cee_binary_line_base_get(CEEBinaryLineRef line)
{
    return line->base;
}

cee_float cee_binary_line_asent_get(CEEBinaryLineRef line)
{
    return line->asent;
}

cee_ulong cee_binary_line_unit_count_get(CEEBinaryLineRef line)
{
    return line->nb_unit;
}

cee_float cee_binary_line_margin_get(CEEBinaryLineRef line)
{
    return line->margin;
}

CEERect cee_binary_line_bounds_get(CEEBinaryLineRef line)
{
    return line->bounds;
}

CEETextGlyphRef cee_binary_unit_glyph_get(CEEBinaryUnitRef unit)
{
    return unit->glyph_ref;
}

CEETextFontRef cee_binary_unit_font_get(CEEBinaryUnitRef unit)
{
    return unit->font_ref;
}

CEETextColorRef cee_binary_unit_fore_color_get(CEEBinaryUnitRef unit)
{
    return unit->fore_color_ref;
}

CEETextColorRef cee_binary_unit_background_color_get(CEEBinaryUnitRef unit)
{
    return unit->background_color_ref;
}

CEERect cee_binary_unit_bounds_get(CEEBinaryUnitRef unit)
{
    return unit->bounds;
}

cee_float cee_binary_unit_ascent_get(CEEBinaryUnitRef unit)
{
    return unit->ascent;
}

cee_boolean cee_binary_unit_under_line_get(CEEBinaryUnitRef unit)
{
    return unit->under_line;
}


static cee_uchar value_alphabet(cee_uchar value)
{
    cee_char ch = '?';
    if (value >= 0 && value <= 9)
        ch = value + 0x30;
    else if (value >= 0xA && value <= 0xF)
        ch = value - 0xA + 0x41;
    
    return ch;
}

static void binary_layout(CEEBinaryLayoutRef layout)
{
    cee_ulong nb_paragraph = 0;
    CEEBinaryStorageRef storage = layout->storage_ref;
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    cee_uchar byte = -1;
    cee_long paragraph_index = layout->paragraph_index;
    cee_long current =
        cee_binary_storage_buffer_offset_by_paragraph_index(layout->storage_ref,
                                                            layout->character_per_line,
                                                            paragraph_index);
    cee_long next = -1;
    CEEBinaryLine* line = NULL;
    CEEBinaryUnit* unit = NULL;
    cee_long n = 0;
    
    if (layout->lines) {
        cee_list_free_full(layout->lines, line_free);
        layout->lines = NULL;
    }
    
    if (current == -1 || !cee_binary_storage_buffer_get(storage))
        return;
    
    layout->lines = NULL;
    layout->anchor = layout->top_margin;
    
    while (TRUE) {
        
        if (!line) {
            line = line_create(layout);
            n = 0;
        }
               
        cee_binary_storage_buffer_byte_next(storage,
                                            current,
                                            &next,
                                            &byte);
        
        
        /** terminate */
        if (next == -1) {
            if (!line->units) {
                line_free(line);
            }
            else {
                line_units_reverse(line);
                layout_append_line(line);
                line_paragraph_index_set(line, paragraph_index ++);
                nb_paragraph ++;
            }
            
            line = NULL;
            break;
        }
        
        n ++;
        
        codepoint = value_alphabet(byte >> 4);
        unit = unit_create(line, codepoint);
        line_prepend_unit(line, unit);
        
        codepoint = value_alphabet(byte & 0x0F);
        unit = unit_create(line, codepoint);
        line_prepend_unit(line, unit);
        
        line_unit_interval_increase(line);
                
        current = next;
                        
        if (n == layout->character_per_line) {
            line_units_reverse(line);
            layout_append_line(line);
            line_paragraph_index_set(line, paragraph_index ++);
            nb_paragraph ++;
            
            if (line_clipped_vertical(line))
                break;
            
            line = NULL;
            continue;
        }
    }
    
    layout->nb_paragraph = nb_paragraph;
    if (nb_paragraph > layout->nb_paragraph_max)
        layout->nb_paragraph_max = nb_paragraph;
}

static void line_vertical_position(CEEBinaryLine* line)
{
    if (!line->units)
        return ;
    
    CEEBinaryLayout* layout = line->layout_ref;
    cee_float max_asent = 0.0;
    cee_float max_height = 0.0;
    CEEBinaryUnit* unit = NULL;
    CEEList* p = line->units;
    
    while (p) {
        unit = p->data;
        
        if (unit->ascent > max_asent)
            max_asent = unit->ascent;
        
        if (unit->bounds.size.height > max_height)
            max_height = unit->bounds.size.height;
        
        p = p->next;
    }
    
    unit = cee_list_first(line->units)->data;
    line->bounds.origin.x = line->margin;
    line->bounds.origin.y = layout->anchor;
    line->bounds.size.height = max_height;
    line->bounds.size.width = line->anchor;
    line->base = layout->anchor + max_asent;
    line->asent = max_asent;
}

static void line_vertical_align(CEEBinaryLine* line)
{
    if (!line->units)
        return ;
    
    CEEBinaryUnit* unit = NULL;
    CEEList* p = line->units;
    cee_float unit_ascent = 0.0;
    cee_float line_ascent = line->asent;
    
    while (p) {
        unit = p->data;
        unit_ascent = unit->ascent;
        
        if (unit_ascent < line_ascent)
            unit->bounds.origin.y += (line_ascent - unit_ascent);
        
        p = p->next;
    }
}

static void layout_append_line(CEEBinaryLine* line)
{
    CEEBinaryLayout* layout = line->layout_ref;
    line_vertical_position(line);
    line_vertical_align(line);
    
    layout->lines = cee_list_append(layout->lines, line);
    layout->anchor += (line->bounds.size.height + layout->line_space);
    
    if (line->anchor > layout->max_line_width)
        layout->max_line_width = line->anchor;
    
    if (layout->bounds.size.width < FLT_MAX) {
    }
    else {
        line->bounds.origin.x -= layout->horizontal_offset;
    }
}

static void line_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEEBinaryLine* line = (CEEBinaryLine*)data;
    
    if (line->units)
        cee_list_free_full(line->units, unit_free);
    
    cee_free(data);
}

static CEEBinaryLine* line_create(CEEBinaryLayout* layout)
{
    CEEBinaryLine* line = cee_malloc0(sizeof(CEEBinaryLine));
    line->layout_ref = layout;
    line->anchor = 0.0;
    line->margin = layout->left_margin;
    line->paragraph_index = -1;
    line->nb_unit = 0;
    return line;
}

static void line_prepend_unit(CEEBinaryLine* line,
                              CEEBinaryUnit* unit)
{
    line->anchor += unit->bounds.size.width;
    line->units = cee_list_prepend(line->units, unit);
    unit->index = line->nb_unit;
    line->nb_unit ++;
}

static void line_units_reverse(CEEBinaryLine* line)
{
    line->units = cee_list_reverse(line->units);
}

static cee_boolean line_clipped_vertical(CEEBinaryLine* line)
{
    CEEBinaryLayout* layout = line->layout_ref;
    return line->base > layout->bounds.size.height - layout->bottom_margin;
}

static void line_paragraph_index_set(CEEBinaryLine* line,
                                     cee_long index)
{
    line->paragraph_index = index;
}

static void line_unit_interval_increase(CEEBinaryLine* line)
{
    if (!line->units)
        return;
    
    CEEBinaryUnit* unit = cee_list_nth_data(line->units, 0);
    line->anchor += unit->bounds.size.width;
}

static void unit_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_free(data);
}

static CEEBinaryUnit* unit_create(CEEBinaryLine* line,
                                  CEEUnicodePoint codepoint)
{
    CEEBinaryLayout* layout = line->layout_ref;
    cee_pointer platform = layout->platform_ref;
    CEETagType type = kCEETagTypePlainText;
    CEEBinaryUnit* unit = cee_malloc0(sizeof(CEEBinaryUnit));
    
    unit->line_ref = line;
    unit->index = -1;
    unit->codepoint = codepoint;
    unit->bounds.origin.x = line->anchor;
    unit->bounds.origin.y = 0.0;
    unit->glyph_ref = NULL;
    
    assert(platform);
    
    type = kCEETagTypePlainText;
    unit->glyph_ref = cee_text_platform_glyph_get(platform, codepoint, type);
    unit->font_ref = cee_text_platform_font_get(platform, codepoint, type);
    unit->fore_color_ref = cee_text_platform_fore_color_get(platform, codepoint, type);
    unit->background_color_ref = cee_text_platform_background_color_get(platform, codepoint, type);
    unit->bounds.size.width = cee_text_platform_width_get(platform, codepoint, type);
    unit->bounds.size.height = cee_text_platform_height_get(platform, codepoint, type);
    unit->ascent = cee_text_platform_ascent_get(platform, codepoint, type);
    unit->under_line = cee_text_platform_under_line_get(platform, codepoint, type);
    
    return unit;
}

cee_long cee_binary_layout_paragraph_index_get(CEEBinaryLayoutRef layout)
{
    return layout->paragraph_index;
}

cee_long cee_binary_layout_paragraph_count_get(CEEBinaryLayoutRef layout)
{
    cee_ulong size = cee_binary_storage_size_get(layout->storage_ref);
    return (size + layout->character_per_line) / layout->character_per_line;
}

cee_ulong cee_binary_layout_paragraph_count_max_get(CEEBinaryLayoutRef layout)
{
    return layout->nb_paragraph_max;
}

cee_float cee_binary_layout_max_line_width_get(CEEBinaryLayoutRef layout)
{
    return layout->max_line_width;
}

cee_float cee_binary_layout_horizontal_offset_get(CEEBinaryLayoutRef layout)
{
    return layout->horizontal_offset;
}

void cee_binary_layout_paragraph_index_set(CEEBinaryLayoutRef layout,
                                           cee_long index)
{
    cee_ulong nb_paragraph = cee_binary_layout_paragraph_count_get(layout);
    if (index >= nb_paragraph)
        index = nb_paragraph - 1;
    layout->paragraph_index = index;
}

void cee_binary_layout_horizontal_offset_set(CEEBinaryLayoutRef layout,
                                             cee_float offset)
{
    layout->horizontal_offset = offset;
}

cee_long cee_binary_layout_buffer_offset_by_paragraph_index(CEEBinaryLayoutRef layout,
                                                            cee_long index)
{
    return layout->character_per_line * index;
}

cee_long cee_binary_layout_paragraph_index_by_buffer_offset(CEEBinaryLayoutRef layout,
                                                            cee_long offset)
{
    return offset / layout->character_per_line;
}

cee_uint cee_binary_layout_characters_per_line(CEEBinaryLayoutRef layout)
{
    return layout->character_per_line;
}

cee_long cee_binary_line_paragraph_index_get(CEEBinaryLineRef line)
{
    return line->paragraph_index;
}
