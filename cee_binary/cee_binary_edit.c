#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include "cee_binary_edit.h"

typedef struct _CEEBinaryEdit {
    cee_pointer host_ref;
    CEEBinaryStorageRef storage;
    cee_boolean own_storage;
    CEEBinaryLayoutRef layout;
    cee_pointer platform;
    CEERect viewport;
} CEEBinaryEdit;

static void binary_edit_reset(CEEBinaryEditRef edit);

CEEBinaryEditRef cee_binary_edit_create(cee_pointer host_ref)
{
    CEEBinaryEdit* edit = (CEEBinaryEdit*)cee_malloc0(sizeof(CEEBinaryEdit));
    edit->host_ref = host_ref;
    edit->platform = cee_text_platform_create();
    edit->storage = cee_binary_storage_create(NULL, 0);
    edit->own_storage = TRUE;
    edit->layout = cee_binary_layout_create(edit->storage,
                                            edit->platform,
                                            32, 
                                            3.0,
                                            6.0,
                                            6.0,
                                            6.0,
                                            6.0,
                                            cee_rect_make(0.0, 0.0, 0.0, 0.0));
    return edit;
}

void cee_binary_edit_free(CEEBinaryEditRef edit)
{
    if (!edit)
        return;
        
    if (edit->layout)
        cee_binary_layout_free(edit->layout);
    
    if (edit->platform)
        cee_text_platform_free(edit->platform);
    
    if (edit->own_storage && edit->storage)
        cee_binary_storage_free(edit->storage);
    
    cee_free(edit);
}

void cee_binary_edit_attributes_configure(CEEBinaryEditRef edit,
                                          const cee_uchar* descriptor)
{
    cee_text_platform_configure(edit->platform, descriptor);
    cee_binary_layout_run(edit->layout);
}

void cee_binary_edit_container_size_set(CEEBinaryEditRef edit,
                                        CEESize size)
{
    if (!edit)
        return;
        
    CEEBinaryLayoutRef layout = edit->layout;
    CEERect bounds = cee_rect_make(0.0, 0.0, 0.0, 0.0);
    bounds = cee_rect_make(0.0, 0.0, FLT_MAX, size.height);
    edit->viewport = cee_rect_make(0.0, 0.0, size.width, size.height);
    
    cee_binary_layout_bounds_set(layout, bounds);
    cee_binary_layout_run(layout);
}


void cee_binary_edit_storage_set(CEEBinaryEditRef edit,
                                 CEEBinaryStorageRef storage)
{
    binary_edit_reset(edit);
    
    if (edit->own_storage && edit->storage) {
        cee_binary_storage_free(edit->storage);
        edit->storage = NULL;
        edit->own_storage = FALSE;
    }
    
    edit->storage = storage;
    if (edit->layout) {
        cee_binary_layout_storage_set(edit->layout, edit->storage);
        cee_binary_layout_run(edit->layout);
    }
}

static void binary_edit_reset(CEEBinaryEditRef edit)
{
    if (!edit)
        return;
    
    if (edit->layout)
        cee_binary_layout_reset(edit->layout);
}

CEEBinaryLayoutRef cee_binary_edit_layout_get(CEEBinaryEditRef edit)
{
    return edit->layout;
}

cee_float cee_binary_edit_vertical_scroller_offset_get(CEEBinaryEditRef edit)
{
    if (!edit)
        return 0.0;
    
    cee_ulong nb_paragraph = cee_binary_layout_paragraph_count_get(edit->layout);
    cee_ulong index = cee_binary_layout_paragraph_index_get(edit->layout);
    if (!nb_paragraph)
        return 0;
    else
        return (cee_float)(index + 1) / (cee_float)nb_paragraph;
}

cee_float cee_binary_edit_vertical_scroller_proportion_get(CEEBinaryEditRef edit)
{
    if (!edit)
        return 1.0;
    
    cee_ulong nb_paragraph = cee_binary_layout_paragraph_count_get(edit->layout);
    cee_ulong nb_present_line = cee_binary_layout_paragraph_count_max_get(edit->layout);
    if (!nb_paragraph)
        return 1.0;
    else
        return ((cee_float)nb_present_line / (cee_float)nb_paragraph);
}

cee_float cee_binary_edit_horizontal_scroller_offset_get(CEEBinaryEditRef edit)
{
    if (!edit)
        return 0.0;
    
    cee_float max_line_width = cee_binary_layout_max_line_width_get(edit->layout);
    cee_float horizontal_offset = cee_binary_layout_horizontal_offset_get(edit->layout);
    return horizontal_offset / max_line_width;
}

cee_float cee_binary_edit_horizontal_scroller_proportion_get(CEEBinaryEditRef edit)
{
    if (!edit)
        return 1.0;
    
    cee_float value = edit->viewport.size.width /
            cee_binary_layout_max_line_width_get(edit->layout);
    return value;
}

void cee_binary_edit_scroll_vertical_to(CEEBinaryEditRef edit,
                                        cee_float proportion)
{
    if (!edit || proportion < 0)
        return;
    
    CEEBinaryLayoutRef layout = edit->layout;
    cee_ulong nb_paragraph = cee_binary_layout_paragraph_count_get(layout);
    cee_long index = (nb_paragraph * proportion) + 0.5;
    index = index > 0 ? index - 1 : index;
    cee_binary_layout_paragraph_index_set(layout, index);
    cee_binary_layout_run(layout);
}

void cee_binary_edit_scroll_horizontal_to(CEEBinaryEditRef edit,
                                          cee_float proportion)
{
    if (!edit || proportion < 0)
        return;
    
    CEEBinaryLayoutRef layout = edit->layout;
    cee_float max_line_width = cee_binary_layout_max_line_width_get(layout);
    cee_binary_layout_horizontal_offset_set(edit->layout,
                                            proportion * max_line_width);
    cee_binary_layout_run(edit->layout);
}

cee_boolean cee_binary_edit_scroll_line_up(CEEBinaryEditRef edit)
{
    if (!edit)
        return FALSE;
        
    CEEBinaryLayoutRef layout = edit->layout;
    cee_long index = cee_binary_layout_paragraph_index_get(layout);
    
    if (index <= 0)
        return FALSE;
    
    index --;
    cee_binary_layout_paragraph_index_set(layout, index);
    cee_binary_layout_run(layout);
    return TRUE;
}

cee_boolean cee_binary_edit_scroll_line_down(CEEBinaryEditRef edit)
{
    if (!edit)
        return FALSE;
    
    CEEBinaryLayoutRef layout = edit->layout;
    cee_ulong nb_paragraph = cee_binary_layout_paragraph_count_get(layout);
    cee_long index = cee_binary_layout_paragraph_index_get(layout);
    
    if (index >= nb_paragraph - 1)
        return FALSE;
    
    index ++;
    cee_binary_layout_paragraph_index_set(layout, index);
    cee_binary_layout_run(layout);
    return TRUE;
}

CEEBinaryStorageRef cee_binary_edit_storage_get(CEEBinaryEditRef edit)
{
    return edit->storage;
}
