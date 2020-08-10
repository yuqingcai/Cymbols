#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include "cee_text_layout.h"

#define CEE_TEXT_LAYOUT_DEBUG 1

typedef struct _CEETextUnit {
    struct _CEETextLine* line_ref;
    cee_long index;
    CEEUnicodePoint codepoint;
    cee_long buffer_offset;
    cee_ulong buffer_length;
    CEETextGlyphRef glyph_ref;
    CEETextColorRef fore_color_ref;
    CEETextColorRef background_color_ref;
    CEETextFontRef font_ref;
    cee_float ascent;
    CEERect bounds;
    cee_boolean under_line;
} CEETextUnit;

typedef struct _CEETextLine {
    struct _CEETextLayout* layout_ref;
    cee_long paragraph_index;
    cee_float margin;
    CEEList* units;
    CEERect bounds;
    cee_float base;
    cee_ulong nb_unit;
    cee_float anchor;
    cee_float asent;
} CEETextLine;

typedef struct _CEETextLayout {
    CEERect bounds;
    cee_ulong tab_size;
    cee_float line_space;
    cee_pointer platform_ref;
    CEETextStorageRef storage_ref;
    cee_long paragraph_index;
    cee_ulong nb_paragraph;
    CEEList* lines;
    cee_float max_line_width;
    cee_float horizontal_offset;
    
    cee_float left_margin;
    cee_float right_margin;
    cee_float top_margin;
    cee_float bottom_margin;
    
    CEETextLayoutAlignment aligment;
    
    cee_float wrap_indent;
    
    cee_float anchor;
    cee_boolean terminate;
    cee_boolean line_feed;
    cee_boolean line_wrapped;
    
    cee_pointer attribute_generator_ref;
    void (*attribute_generate)(cee_pointer,
                               CEETextLayoutRef,
                               CEERange);
    CEEBST* tags_bst_ref;
} CEETextLayout;

static cee_boolean unit_node_get(CEETextLayoutRef layout,
                                 cee_long buffer_offset,
                                 CEEList** line_node,
                                 CEEList** unit_node);
static cee_float paragraph_margin(CEETextLine* line);
static void line_wrap(CEETextLine* line, 
                      cee_long* adjusted);
static void layout_carrage_return(CEETextLayout* layout);
static void line_vertical_position(CEETextLine* line);
static void line_vertical_align(CEETextLine* line);
static void layout_append_line(CEETextLine* line);
static void line_free(cee_pointer data);
static CEETextLine* line_create(CEETextLayout* layout);
static void line_paragraph_index_set(CEETextLine* line,
                                     cee_long index);
static void line_prepend_unit(CEETextLine* line,
                              CEETextUnit* unit);
static void line_units_reverse(CEETextLine* line);
static cee_boolean line_clipped_horizontal(CEETextLine* line);
static cee_boolean line_clipped_vertical(CEETextLine* line);
static cee_float white_space_width_get(CEETextLayout* layout);
static cee_float tab_placeholder_width_get(CEETextLine* line);
static void unit_free(cee_pointer data);
static CEETextUnit* unit_create(CEETextLine* line,
                                CEEUnicodePoint codepoint,
                                cee_long buffer_offset,
                                cee_ulong buffer_length,
                                cee_boolean tinted);
static cee_boolean layout_line_contain_offset(CEETextLine* line,
                                              cee_long offset);
static void layout_init(CEETextLayout* layout);
static void text_layout(CEETextLayoutRef layout,
                        cee_boolean tinted);

static cee_boolean unit_node_get(CEETextLayoutRef layout,
                                 cee_long buffer_offset,
                                 CEEList** line_node,
                                 CEEList** unit_node)
{
    cee_long offset = -1;
    cee_ulong length = 0;
    CEETextLineRef line = NULL;
    CEETextUnitRef unit = NULL;
    CEEList* q = NULL;
    CEEList* p = NULL;
    
    if (line_node)
        *line_node = NULL;

    if (unit_node)
        *unit_node = NULL;
    
    p = cee_text_layout_lines_get(layout);
    while (p) {
        line = p->data;
        q = cee_text_line_units_get(line);
        
        while (q) {
            
            unit = q->data;
            offset = cee_text_unit_buffer_offset_get(unit);
            length = cee_text_unit_buffer_length_get(unit);
            
            if (buffer_offset >= offset && 
                buffer_offset < offset + length) {
                
                if (line_node)
                    *line_node = p;
                if (unit_node)
                    *unit_node = q;
                
                return TRUE;
            }
            
            q = q->next;
        }
        
        p = p->next;
    }
    
    return FALSE;
}

static cee_float paragraph_margin(CEETextLine* line)
{
    CEETextLayout* layout = line->layout_ref;
    cee_float width = 0.0;
    CEEList* p = line->units;
    while (p) {
        CEETextUnit* unit = p->data;
        
        if (unit->codepoint == CEE_UNICODE_POINT_HT)
            width += white_space_width_get(layout) * layout->tab_size;
        else if (unit->codepoint == CEE_UNICODE_POINT_SP)
            width += white_space_width_get(layout);
        else
            break;
        
        p = p->next;
    }
    
    return width;
}

static void line_wrap(CEETextLine* line, 
                      cee_long* adjusted)
{
    CEETextLayout* layout = line->layout_ref;
    if (!layout->line_wrapped) {
        layout->line_wrapped = TRUE;
        layout->wrap_indent = paragraph_margin(line) + 
            (white_space_width_get(layout) * layout->tab_size);
    }
    
    if (cee_list_length(line->units) == 1)
        return;
    
    CEEList* p = cee_list_last(line->units);
    
    CEETextUnit* unit = p->data;
    *adjusted = unit->buffer_offset;
    
    p->prev->next = NULL;
    p->prev = NULL;
    
    line->nb_unit --;
    line->anchor -= unit->bounds.size.width;
    cee_list_free_full(p, unit_free);
    
}

static void layout_carrage_return(CEETextLayout* layout)
{
    if (layout->line_wrapped) {
        layout->line_wrapped = FALSE;
        layout->wrap_indent = 0.0;
    }
    layout->line_feed = FALSE;
}

static void line_vertical_position(CEETextLine* line)
{
    if (!line->units)
        return ;
    
    CEETextLayout* layout = line->layout_ref;
    cee_float max_asent = 0.0;
    cee_float max_height = 0.0;
    CEETextUnit* unit = NULL;
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

static void line_vertical_align(CEETextLine* line)
{
    if (!line->units)
        return ;
    
    CEETextUnit* unit = NULL;
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

static void layout_append_line(CEETextLine* line)
{
    CEETextLayout* layout = line->layout_ref;    
    line_vertical_position(line);
    line_vertical_align(line);
    
    layout->lines = cee_list_append(layout->lines, line);
    layout->anchor += (line->bounds.size.height + layout->line_space);
    
    if (line->anchor > layout->max_line_width)
        layout->max_line_width = line->anchor;
    
    if (layout->bounds.size.width < FLT_MAX) {
        if (layout->aligment == kCEETextLayoutAlignmentCenter)
            line->bounds.origin.x = 
                (layout->bounds.size.width - line->bounds.size.width) / 2.0;
        else if (layout->aligment == kCEETextLayoutAlignmentRight)
            line->bounds.origin.x = 
                layout->bounds.size.width - line->bounds.size.width;
    }
    else {
        line->bounds.origin.x -= layout->horizontal_offset;
    }
}

static void line_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETextLine* line = (CEETextLine*)data;
    if (line->units)
        cee_list_free_full(line->units, unit_free);
    
    cee_free(data);
}

static CEETextLine* line_create(CEETextLayout* layout)
{
    CEETextLine* line = cee_malloc0(sizeof(CEETextLine));
    line->layout_ref = layout;
    line->anchor = 0.0;
    line->margin = layout->left_margin + layout->wrap_indent;
    line->paragraph_index = -1;
    line->nb_unit = 0;
    return line;
}

static void line_paragraph_index_set(CEETextLine* line,
                                     cee_long index)
{
    line->paragraph_index = index;
}

static void line_prepend_unit(CEETextLine* line,
                              CEETextUnit* unit)
{
    line->anchor += unit->bounds.size.width;
    line->units = cee_list_prepend(line->units, unit);
    unit->index = line->nb_unit;
    line->nb_unit ++;
}

static void line_units_reverse(CEETextLine* line)
{
    line->units = cee_list_reverse(line->units);
}

static cee_boolean line_clipped_horizontal(CEETextLine* line)
{
    CEETextLayout* layout = line->layout_ref;
    return line->anchor > (layout->bounds.size.width - 
                           (line->margin + layout->right_margin));
}

static cee_boolean line_clipped_vertical(CEETextLine* line)
{
    CEETextLayout* layout = line->layout_ref;
    return line->base > layout->bounds.size.height - layout->bottom_margin;
}

static cee_float white_space_width_get(CEETextLayout* layout)
{
    return cee_text_platform_width_get(layout->platform_ref,
                                       CEE_UNICODE_POINT_SP,
                                       kCEETagTypePlanText);
}

static cee_float tab_placeholder_width_get(CEETextLine* line)
{
    CEETextLayout* layout = line->layout_ref;
    cee_float space_width = white_space_width_get(layout);
    cee_float placeholder_width = 0.0;
    cee_float tab_stop = 0.0;
    cee_int i = 1;
    
    while (TRUE) {
        tab_stop = space_width * layout->tab_size * i;
        if ((cee_long)tab_stop > (cee_long)line->anchor)
            break;
        i ++;
    }
    placeholder_width = tab_stop - line->anchor;
    return placeholder_width;
}

static void unit_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_free(data);
}

static CEETextUnit* unit_create(CEETextLine* line,
                                CEEUnicodePoint codepoint,
                                cee_long buffer_offset,
                                cee_ulong buffer_length,
                                cee_boolean tinted)
{
    CEETextLayout* layout = line->layout_ref;
    CEETextStorageRef storage = layout->storage_ref;
    cee_pointer platform = layout->platform_ref;
    CEETagType type = kCEETagTypePlanText;
    CEETag* tag = NULL;
    CEETextUnit* unit = cee_malloc0(sizeof(CEETextUnit));
    CEEUnicodePointType codepoint_type = cee_codec_unicode_point_type(codepoint);
    
    unit->line_ref = line;
    unit->buffer_offset = buffer_offset;
    unit->buffer_length = buffer_length;
    unit->index = -1;
    unit->codepoint = codepoint;
    unit->bounds.origin.x = line->anchor;
    unit->bounds.origin.y = 0.0;
    unit->glyph_ref = NULL;
    
    assert(platform);
    
    type = kCEETagTypePlanText;
    if (tinted) {
        tag = cee_text_layout_tag_get(layout, buffer_offset);
        if (tag)
            type = tag->type;
        else
            type = kCEETagTypePlanText;
    }
    
    if (codepoint_type == kCEEUnicodePointTypeControl) {
        unit->fore_color_ref = cee_text_platform_fore_color_get(platform, codepoint, type);
        unit->background_color_ref = cee_text_platform_background_color_get(platform, codepoint, type);
        if (codepoint == CEE_UNICODE_POINT_HT)
            unit->bounds.size.width = tab_placeholder_width_get(line);
        else
            unit->bounds.size.width = white_space_width_get(layout);
        unit->bounds.size.height = cee_text_platform_height_get(platform, CEE_UNICODE_POINT_SP, type);
        unit->ascent = cee_text_platform_ascent_get(platform, CEE_UNICODE_POINT_SP, type);
        unit->under_line = cee_text_platform_under_line_get(platform, CEE_UNICODE_POINT_SP, type);
    }
    else {
        unit->glyph_ref = cee_text_platform_glyph_get(platform, codepoint, type);
        unit->font_ref = cee_text_platform_font_get(platform, codepoint, type);
        unit->fore_color_ref = cee_text_platform_fore_color_get(platform, codepoint, type);
        unit->background_color_ref = cee_text_platform_background_color_get(platform, codepoint, type);
        unit->bounds.size.width = cee_text_platform_width_get(platform, codepoint, type);
        unit->bounds.size.height = cee_text_platform_height_get(platform, codepoint, type);
        unit->ascent = cee_text_platform_ascent_get(platform, codepoint, type);
        unit->under_line = cee_text_platform_under_line_get(platform, codepoint, type);
    }
    
    return unit;
}



static cee_boolean layout_line_contain_offset(CEETextLine* line,
                                              cee_long offset)
{
    CEEList* p = line->units;
    while (p) {
        CEETextUnitRef unit = p->data;
        if (offset >= unit->buffer_offset && 
            offset < unit->buffer_offset + unit->buffer_length)
            return TRUE;
        
        p = p->next;
    }
    return FALSE;
}

static void layout_init(CEETextLayout* layout)
{
    if (layout->lines)
        cee_list_free_full(layout->lines, line_free);
    layout->lines = NULL;
    layout->anchor = layout->top_margin;
    layout->terminate = FALSE;
    layout->line_feed = FALSE;
    layout->line_wrapped = FALSE;
    layout->wrap_indent = 0.0;
}

void cee_text_layout_free(CEETextLayoutRef layout)
{
    if (!layout)
        return;
        
    if (layout->lines)
        cee_list_free_full(layout->lines, line_free);
    
    cee_free(layout);
}

CEETextLayoutRef cee_text_layout_create(CEETextStorageRef storage,
                                        cee_pointer platform,
                                        CEETextLayoutAlignment aligment,
                                        cee_ulong tab_size,
                                        cee_float line_space,
                                        cee_float left_margin,
                                        cee_float right_margin,
                                        cee_float top_margin,
                                        cee_float bottom_margin,
                                        CEERect bounds)
{
    CEETextLayout* layout = (CEETextLayout*)cee_malloc0(sizeof(CEETextLayout));
    
    layout->storage_ref = storage;
    layout->platform_ref = platform;
    layout->tab_size = tab_size;
    layout->line_space = line_space;
    layout->bounds = bounds;
    layout->paragraph_index = 0;
    layout->left_margin = left_margin;
    layout->right_margin = right_margin;
    layout->top_margin = top_margin;
    layout->bottom_margin = bottom_margin;
    layout->max_line_width = 0.0;
    layout->horizontal_offset = 0.0;
    layout->aligment = aligment;
    
    return layout;
}

cee_float cee_text_layout_line_space_get(CEETextLayoutRef layout)
{
    return layout->line_space;
}

void cee_text_layout_storage_set(CEETextLayoutRef layout,
                                 CEETextStorageRef storage)
{
    layout->storage_ref = storage;
}

void cee_text_layout_aligment_set(CEETextLayoutRef layout,
                                  CEETextLayoutAlignment aligment)
{
    layout->aligment = aligment;
}

void cee_text_layout_platform_set(CEETextLayoutRef layout,
                                  cee_pointer platform)
{
    layout->platform_ref = platform;
}

static void text_layout(CEETextLayoutRef layout,
                        cee_boolean tinted)
{
    cee_ulong nb_paragraph = 0;
    CEETextStorageRef storage = layout->storage_ref;
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    cee_size length = 0;
    cee_long paragraph_index = layout->paragraph_index;
    cee_long current = cee_text_storage_paragraph_by_index(layout->storage_ref,
                                                           paragraph_index);
    cee_long next = -1;
    CEETextLine* line = NULL;
    CEETextUnit* unit = NULL;
    
    if (current == -1 || !cee_text_storage_buffer_get(storage))
        return;
    
    layout_init(layout);
    
    while (TRUE) {
        
        if (!line) {
            line = line_create(layout);
            if (!layout->line_wrapped)
                line_paragraph_index_set(line, paragraph_index ++);
        }
        
        next = cee_text_storage_buffer_character_next(storage,
                                                      current,
                                                      &codepoint,
                                                      &length);
        
        if (codepoint == CEE_UNICODE_POINT_NUL)
            layout->terminate = TRUE;
        else if (codepoint == CEE_UNICODE_POINT_LF)
            layout->line_feed = TRUE;
        
        unit = unit_create(line, codepoint, current, length, tinted);
        line_prepend_unit(line, unit);
        current = next;
        
        if (line_clipped_horizontal(line)) {
            line_units_reverse(line);
            line_wrap(line, &current);
            layout_append_line(line);
            
            if (line_clipped_vertical(line) || layout->terminate)
                break;
            
            line = NULL;
            continue;
        }
                
        /** linefeed */
        if (layout->line_feed) {
            nb_paragraph ++;
            line_units_reverse(line);
            layout_append_line(line);
            
            if (line_clipped_vertical(line))
                break;
            
            layout_carrage_return(layout);
            line = NULL;
            
            continue;
        }
        
        /** terminate */
        if (layout->terminate) {
            nb_paragraph ++;
            line_units_reverse(line);
            layout_append_line(line);
            
            line = NULL;
            break;
        }
    }
    
    if (nb_paragraph > layout->nb_paragraph)
        layout->nb_paragraph = nb_paragraph;
}

void cee_text_layout_run(CEETextLayoutRef layout)
{
    CEETextUnitRef head_unit = 0;
    CEETextUnitRef tail_unit = 0;
    cee_long offset = 0;
    cee_ulong length = 0;
    CEERange layout_range;
    
    text_layout(layout, FALSE);
    
    head_unit = cee_text_layout_head_unit_get(layout);
    tail_unit = cee_text_layout_tail_unit_get(layout);
    offset = cee_text_unit_buffer_offset_get(head_unit);
    length = cee_text_unit_buffer_offset_get(tail_unit) +
                        cee_text_unit_buffer_length_get(tail_unit) -
                        cee_text_unit_buffer_offset_get(head_unit);
    layout_range = cee_range_make(offset, length);
        
    if (layout->attribute_generate) {
        layout->attribute_generate(layout->attribute_generator_ref,
                                   layout,
                                   layout_range);
        text_layout(layout, TRUE);
    }
}

cee_boolean cee_text_layout_paragraph_index_is_invalid(CEETextLayoutRef layout)
{
    CEETextStorageRef storage = layout->storage_ref;
    if (cee_text_storage_paragraph_by_index(storage, layout->paragraph_index) == -1)
        return TRUE;
    
    return FALSE;
}

void cee_text_layout_bounds_set(CEETextLayoutRef layout,
                                CEERect bounds)
{
    layout->bounds = bounds;
    layout->nb_paragraph = 0;
}

void cee_text_layout_horizontal_offset_set(CEETextLayoutRef layout,
                                           cee_float offset)
{
    layout->horizontal_offset = offset;
}

cee_float cee_text_layout_horizontal_offset_get(CEETextLayoutRef layout)
{
    return layout->horizontal_offset;
}

CEERect cee_text_layout_bounds_get(CEETextLayoutRef layout)
{
    if (!layout)
        return cee_rect_make(0.0, 0.0, 0.0, 0.0);
    
    return layout->bounds;
}

void cee_text_layout_paragraph_index_set(CEETextLayoutRef layout,
                                         cee_long index)
{
    layout->paragraph_index = index;
}

cee_long cee_text_layout_paragraph_index_get(CEETextLayoutRef layout)
{
    return layout->paragraph_index;
}

CEEList* cee_text_layout_lines_get(CEETextLayoutRef layout)
{
    return layout->lines;
}

cee_ulong cee_text_layout_paragraph_count_get(CEETextLayoutRef layout)
{
    return layout->nb_paragraph;
}

cee_float cee_text_layout_max_line_width_get(CEETextLayoutRef layout)
{
    return layout->max_line_width;
}

CEEList* cee_text_layout_line_boxes_create(CEETextLayoutRef layout,
                                           cee_long from,
                                           cee_long to)
{
    CEEList* boxes = NULL;
    CEERect* box = NULL;
    CEEList* l0 = NULL;
    CEEList* l1 = NULL;
    CEETextLineRef line = NULL;
    CEERect line_bounds;
    CEEList* p = cee_text_layout_lines_get(layout);
    while (p) {
        line = p->data;
        if (layout_line_contain_offset(line, from))
            l0 = p;
        if (layout_line_contain_offset(line, to))
            l1 = p;
        p = p->next;
    }
    
    if (!l0 || !l1)
        return NULL;
        
    p = l0;
    while (p) {
        
        line = p->data;
        line_bounds = cee_text_line_bounds_get(line);
        box = cee_rect_create(line_bounds.origin.x, 
                              line_bounds.origin.y - layout->line_space, 
                              line_bounds.size.width, 
                              line_bounds.size.height + layout->line_space);
        boxes = cee_list_append(boxes, box);
        
        if (p == l1)
            break;
        
        p = p->next;
    }
    
    return boxes;
}

cee_float cee_text_line_base_get(CEETextLineRef line)
{
    return line->base;
}

cee_float cee_text_line_asent_get(CEETextLineRef line)
{
    return line->asent;
}

cee_long cee_text_line_paragraph_index_get(CEETextLineRef line)
{
    return line->paragraph_index;
}

CEEList* cee_text_line_units_get(CEETextLineRef line)
{
    return line->units;
}

cee_ulong cee_text_line_unit_count_get(CEETextLineRef line)
{
    return line->nb_unit;
}

cee_float cee_text_line_margin_get(CEETextLineRef line)
{
    return line->margin;
}

CEERect cee_text_line_bounds_get(CEETextLineRef line)
{
    return line->bounds;
}

CEERect cee_text_line_box_get(CEETextLineRef line)
{
    CEETextLayout* layout = line->layout_ref;
    cee_float line_space = cee_text_layout_line_space_get(layout);
    CEERect line_bounds = cee_text_line_bounds_get(line);
    CEERect rect = cee_rect_make(line_bounds.origin.x, 
                                 line_bounds.origin.y - line_space, 
                                 line_bounds.size.width,
                                 line_bounds.size.height + line_space);
    return rect;
}

CEEUnicodePoint cee_text_unit_codepoint_get(CEETextUnitRef unit)
{
    return unit->codepoint;
}

CEETextGlyphRef cee_text_unit_glyph_get(CEETextUnitRef unit)
{
    return unit->glyph_ref;
}

CEETextFontRef cee_text_unit_font_get(CEETextUnitRef unit)
{
    return unit->font_ref;
}

CEETextColorRef cee_text_unit_fore_color_get(CEETextUnitRef unit)
{
    return unit->fore_color_ref;
}

CEETextColorRef cee_text_unit_background_color_get(CEETextUnitRef unit)
{
    return unit->background_color_ref;
}

CEERect cee_text_unit_bounds_get(CEETextUnitRef unit)
{
    return unit->bounds;
}

CEERect cee_text_unit_box_get(CEETextUnitRef unit)
{
    CEETextLine* line = unit->line_ref;
    CEETextLayout* layout = line->layout_ref;
    cee_float line_space = cee_text_layout_line_space_get(layout);
    CEERect unit_bounds = cee_text_unit_bounds_get(unit);
    CEERect rect = cee_rect_make(unit_bounds.origin.x, 
                                 unit_bounds.origin.y - line_space, 
                                 unit_bounds.size.width, 
                                 unit_bounds.size.height + line_space);
    return rect;
}

cee_long cee_text_unit_buffer_offset_get(CEETextUnitRef unit)
{
    return unit->buffer_offset;
}

cee_ulong cee_text_unit_buffer_length_get(CEETextUnitRef unit)
{
    return unit->buffer_length;
}

cee_ulong cee_text_unit_index_get(CEETextUnitRef unit)
{
    return unit->index;
}

CEETextStorageRef cee_text_layout_storage_get(CEETextLayoutRef layout)
{
    return layout->storage_ref;
}

CEETextUnitRef cee_text_unit_get_by_location(CEETextLayoutRef layout,
                                             CEEPoint location)
{
    CEETextLineRef line = NULL;
    CEETextLineRef target_line = NULL;
    CEETextUnitRef unit = NULL;
    CEETextUnitRef target_unit = NULL;
    CEERect line_bounds;
    CEERect rect;
    
    CEEList* p = cee_text_layout_lines_get(layout);
    line = cee_list_first(p)->data;
    rect = cee_text_line_box_get(line);
    if (location.y < rect.origin.y)
        target_line = line;
    
    if (!target_line) {
        line = cee_list_last(p)->data;
        rect = cee_text_line_box_get(line);
        if (location.y > rect.origin.y + rect.size.height)
            target_line = line;
    }
    
    if (!target_line) {
        while (p) {
            line = p->data;
            rect = cee_text_line_box_get(line);
            
            if (location.y >= rect.origin.y && 
                location.y < rect.origin.y + rect.size.height) {
                target_line = line;
                break;
            }
            p = p->next;
        }
    }
    
    if (!target_line)
        return NULL;
    
    line_bounds = cee_text_line_bounds_get(target_line);
    
    p = cee_text_line_units_get(target_line);
    unit = cee_list_last(p)->data;
    rect = cee_text_unit_box_get(unit);
    rect.origin.x += line_bounds.origin.x;
    if (location.x > rect.origin.x + rect.size.width)
        target_unit = unit;
    
    if (!target_unit) {
        unit = cee_list_first(p)->data;
        rect = cee_text_unit_box_get(unit);
        rect.origin.x += line_bounds.origin.x;
        if (location.x < rect.origin.x)
            target_unit = unit;
    }
    
    if (!target_unit) {
        while (p) {
            unit = p->data;
            rect = cee_text_unit_box_get(unit);
            rect.origin.x += line_bounds.origin.x;
            
            if (location.x > rect.origin.x && location.x < 
                rect.origin.x + rect.size.width / 2.0) {
                target_unit = unit;
                break;
            }
            else if (location.x >= rect.origin.x + rect.size.width / 2.0 && 
                     location.x <= rect.origin.x + rect.size.width ) {
                if (p->next)
                    target_unit = p->next->data;
                else
                    target_unit = p->data;
                
                break;
            }
            
            p = p->next;
        }
    }
    
    return target_unit;
}

cee_float cee_text_unit_ascent_get(CEETextUnitRef unit)
{
    return unit->ascent;
}

cee_boolean cee_text_unit_under_line_get(CEETextUnitRef unit)
{
    return unit->under_line;
}

CEETextLineRef cee_text_layout_head_line_get(CEETextLayoutRef layout)
{
    CEEList* lines = cee_text_layout_lines_get(layout);
    return cee_list_first(lines)->data;
}

CEETextUnitRef cee_text_layout_head_unit_get(CEETextLayoutRef layout)
{
    CEETextLineRef line = cee_text_layout_head_line_get(layout);
    CEEList* units = cee_text_line_units_get(line);
    return cee_list_first(units)->data;
}

CEETextLineRef cee_text_layout_tail_line_get(CEETextLayoutRef layout)
{
    CEEList* lines = cee_text_layout_lines_get(layout);
    return cee_list_last(lines)->data;
}

CEETextUnitRef cee_text_layout_tail_unit_get(CEETextLayoutRef layout)
{
    CEETextLineRef line = cee_text_layout_tail_line_get(layout);
    CEEList* units = cee_text_line_units_get(line);
    return cee_list_last(units)->data;
}

CEEList* cee_text_layout_rects_create(CEETextLayoutRef layout,
                                      CEERange range)
{
    CEETextStorageRef storage = layout->storage_ref;
    CEETextUnitRef head_unit = cee_text_layout_head_unit_get(layout);
    CEETextUnitRef tail_unit = cee_text_layout_tail_unit_get(layout);
    cee_long head_offset = cee_text_unit_buffer_offset_get(head_unit);
    cee_long tail_offset = cee_text_unit_buffer_offset_get(tail_unit);
    cee_ulong tail_length = cee_text_unit_buffer_length_get(tail_unit);
    CEEList* line_boxes = NULL;
    CEERect* rect = NULL;
    
    CEEList* unit_node = NULL;
    CEETextUnitRef unit0 = NULL;
    CEETextUnitRef unit1 = NULL;
    CEERect unit0_bounds;
    CEERect unit1_bounds;
    
    const cee_uchar* buffer = cee_text_storage_buffer_get(storage);
    cee_long start_offset = -1;
    cee_long end_offset = -1;
    
    if (!range.length) {
        start_offset = range.location;
        if (start_offset >= tail_offset + tail_length || start_offset < head_offset)
            return NULL;
        
        line_boxes = cee_text_layout_line_boxes_create(layout, start_offset, start_offset);
        rect = cee_list_first(line_boxes)->data;
        unit_node_get(layout, start_offset, NULL, &unit_node);
        unit0 = unit_node->data;
        unit0_bounds = cee_text_unit_bounds_get(unit0);
        rect->origin.x = rect->origin.x + unit0_bounds.origin.x;
        rect->size.width = 0.0;
    }
    else {
        start_offset = range.location;
        end_offset = cee_codec_utf8_encoded_byte0_get(buffer, range.location + range.length - 1);
        
        if (start_offset >= tail_offset + tail_length || end_offset < head_offset) {
            return NULL;
        }
        else if (start_offset < head_offset && end_offset >= tail_offset + tail_length) {
            line_boxes = cee_text_layout_line_boxes_create(layout, head_offset, tail_offset);
        }
        else if (start_offset < head_offset && 
                 end_offset >= head_offset && end_offset < tail_offset + tail_length) {
            line_boxes = cee_text_layout_line_boxes_create(layout, head_offset, end_offset);
            rect = cee_list_last(line_boxes)->data;
            unit_node_get(layout, end_offset, NULL, &unit_node);
            unit0 = unit_node->data;
            unit0_bounds = cee_text_unit_bounds_get(unit0);
            rect->size.width = unit0_bounds.origin.x + unit0_bounds.size.width;
        }
        else if (start_offset >= head_offset && start_offset < tail_offset + tail_length &&
                 end_offset >= tail_offset + tail_length) {
            line_boxes = cee_text_layout_line_boxes_create(layout, start_offset, tail_offset);
            rect = cee_list_first(line_boxes)->data;
            unit_node_get(layout, start_offset, NULL, &unit_node);
            unit0 = unit_node->data;
            unit0_bounds = cee_text_unit_bounds_get(unit0);
            rect->origin.x = rect->origin.x + unit0_bounds.origin.x;
            rect->size.width = rect->size.width - unit0_bounds.origin.x;
        }
        else if (start_offset >= head_offset && start_offset < tail_offset + tail_length &&
                 end_offset < tail_offset + tail_length) {
            
            line_boxes = cee_text_layout_line_boxes_create(layout, start_offset, end_offset);
            
            if (cee_list_length(line_boxes) == 1) {
                rect = cee_list_first(line_boxes)->data;
                unit_node_get(layout, start_offset, NULL, &unit_node);
                unit0 = unit_node->data;
                unit0_bounds = cee_text_unit_bounds_get(unit0);
                rect->origin.x = rect->origin.x + unit0_bounds.origin.x;
                
                unit_node_get(layout, end_offset, NULL, &unit_node);
                unit1 = unit_node->data;
                unit1_bounds = cee_text_unit_bounds_get(unit1);
                rect->size.width = unit1_bounds.origin.x + unit1_bounds.size.width - 
                    unit0_bounds.origin.x;
            }
            else {
                rect = cee_list_first(line_boxes)->data;
                unit_node_get(layout, start_offset, NULL, &unit_node);
                unit0 = unit_node->data;
                unit0_bounds = cee_text_unit_bounds_get(unit0);
                rect->origin.x = rect->origin.x + unit0_bounds.origin.x;
                rect->size.width = rect->size.width - unit0_bounds.origin.x;
                
                rect = cee_list_last(line_boxes)->data;
                unit_node_get(layout, end_offset, NULL, &unit_node);
                unit1 = unit_node->data;
                unit1_bounds = cee_text_unit_bounds_get(unit1);
                rect->size.width = unit1_bounds.origin.x + unit1_bounds.size.width;
            }
        }
    }
    
    return line_boxes;
}

CEETextUnitRef cee_text_unit_get_by_offset(CEETextLayoutRef layout,
                                           cee_long buffer_offset)
{
    CEEList* unit_node = NULL;
    unit_node_get(layout, buffer_offset, NULL, &unit_node);
    
    if (unit_node)
        return unit_node->data;
    
    return NULL;
}

CEETextLineRef cee_text_line_get_by_offset(CEETextLayoutRef layout,
                                           cee_long buffer_offset)
{
    CEEList* line_node = NULL;
    unit_node_get(layout, buffer_offset, &line_node, NULL);
    
    if (line_node)
        return line_node->data;
    
    return NULL;
}


void cee_text_layout_attribute_generator_set(CEETextLayoutRef layout,
                                             cee_pointer generator)
{
    layout->attribute_generator_ref = generator;
}
void cee_text_layout_attribute_generate_set(CEETextLayoutRef layout,
                                            void (*generate)(cee_pointer,
                                                             CEETextLayoutRef,
                                                             CEERange))
{
    layout->attribute_generate = generate;
}


void cee_text_layout_tags_bst_set(CEETextLayoutRef layout,
                                  CEEBST* tags_bst)
{
    layout->tags_bst_ref = tags_bst;
}

CEETag* cee_text_layout_tag_get(CEETextLayoutRef layout,
                                cee_long buffer_offset)
{
    CEEBST* p = layout->tags_bst_ref;
    while (p) {
        CEEList* tag_node = p->data;
        CEETag* tag = tag_node->data;
        
        if (buffer_offset < tag->range.location)
            p = p->left;
        else {
            if (buffer_offset >= tag->range.location &&
                buffer_offset <= tag->range.location + tag->range.length - 1)
                return tag;
            else
                p = p->right;
        }
    }
    return NULL;
}
