#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include "cee_ascii_edit.h"

typedef struct _CEEASCIIEdit {
    cee_pointer host_ref;
    CEEASCIIStorageRef storage;
    cee_boolean own_storage;
    CEEASCIILayoutRef layout;
    cee_pointer platform;
    CEERect viewport;
} CEEASCIIEdit;

static void ascii_edit_reset(CEEASCIIEditRef edit);

CEEASCIIEditRef cee_ascii_edit_create(cee_pointer host_ref)
{
    CEEASCIIEdit* edit = (CEEASCIIEdit*)cee_malloc0(sizeof(CEEASCIIEdit));
    edit->host_ref = host_ref;
    edit->platform = cee_text_platform_create();
    edit->storage = cee_ascii_storage_create(NULL, 0);
    edit->own_storage = TRUE;
    edit->layout = cee_ascii_layout_create(edit->storage,
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

void cee_ascii_edit_free(CEEASCIIEditRef edit)
{
    if (!edit)
        return;
        
    if (edit->layout)
        cee_ascii_layout_free(edit->layout);
    
    if (edit->platform)
        cee_text_platform_free(edit->platform);
    
    if (edit->own_storage && edit->storage)
        cee_ascii_storage_free(edit->storage);
    
    cee_free(edit);
}

void cee_ascii_edit_attributes_configure(CEEASCIIEditRef edit,
                                         const cee_uchar* descriptor)
{
    cee_text_platform_configure(edit->platform, descriptor);
    cee_ascii_layout_run(edit->layout);
}

void cee_ascii_edit_container_size_set(CEEASCIIEditRef edit,
                                       CEESize size)
{
    if (!edit)
        return;
        
    CEEASCIILayoutRef layout = edit->layout;
    CEERect bounds = cee_rect_make(0.0, 0.0, 0.0, 0.0);
    bounds = cee_rect_make(0.0, 0.0, FLT_MAX, size.height);
    edit->viewport = cee_rect_make(0.0, 0.0, size.width, size.height);
    
    cee_ascii_layout_bounds_set(layout, bounds);
    cee_ascii_layout_run(layout);
}


void cee_ascii_edit_storage_set(CEEASCIIEditRef edit,
                                CEEASCIIStorageRef storage)
{
    ascii_edit_reset(edit);
    
    if (edit->own_storage && edit->storage) {
        cee_ascii_storage_free(edit->storage);
        edit->storage = NULL;
        edit->own_storage = FALSE;
    }
    
    edit->storage = storage;
    if (edit->layout) {
        cee_ascii_layout_storage_set(edit->layout, edit->storage);
        cee_ascii_layout_run(edit->layout);
    }
}

static void ascii_edit_reset(CEEASCIIEditRef edit)
{
    if (!edit)
        return;
    
    if (edit->layout)
        cee_ascii_layout_reset(edit->layout);
}

CEEASCIILayoutRef cee_ascii_edit_layout_get(CEEASCIIEditRef edit)
{
    return edit->layout;
}

cee_float cee_ascii_edit_vertical_scroller_offset_get(CEEASCIIEditRef edit)
{
    if (!edit)
        return 0.0;
    
    cee_ulong nb_paragraph = cee_ascii_layout_paragraph_count_get(edit->layout);
    cee_ulong index = cee_ascii_layout_paragraph_index_get(edit->layout);
    if (!nb_paragraph)
        return 0;
    else
        return (cee_float)(index + 1) / (cee_float)nb_paragraph;
}

cee_float cee_ascii_edit_vertical_scroller_proportion_get(CEEASCIIEditRef edit)
{
    if (!edit)
        return 1.0;
    
    cee_ulong nb_paragraph = cee_ascii_layout_paragraph_count_get(edit->layout);
    cee_ulong nb_present_line = cee_ascii_layout_paragraph_count_max_get(edit->layout);
    if (!nb_paragraph)
        return 1.0;
    else
        return ((cee_float)nb_present_line / (cee_float)nb_paragraph);
}

cee_float cee_ascii_edit_horizontal_scroller_offset_get(CEEASCIIEditRef edit)
{
    if (!edit)
        return 0.0;
    
    cee_float max_line_width = cee_ascii_layout_max_line_width_get(edit->layout);
    cee_float horizontal_offset = cee_ascii_layout_horizontal_offset_get(edit->layout);
    return horizontal_offset / max_line_width;
}

cee_float cee_ascii_edit_horizontal_scroller_proportion_get(CEEASCIIEditRef edit)
{
    if (!edit)
        return 1.0;
    
    cee_float value = edit->viewport.size.width /
            cee_ascii_layout_max_line_width_get(edit->layout);
    return value;
}

void cee_ascii_edit_scroll_vertical_to(CEEASCIIEditRef edit,
                                       cee_float proportion)
{
    if (!edit || proportion < 0)
        return;
    
    CEEASCIILayoutRef layout = edit->layout;
    cee_ulong nb_paragraph = cee_ascii_layout_paragraph_count_get(layout);
    cee_long index = (nb_paragraph * proportion) + 0.5;
    index = index > 0 ? index - 1 : index;
    cee_ascii_layout_paragraph_index_set(layout, index);
    cee_ascii_layout_run(layout);
}

void cee_ascii_edit_scroll_horizontal_to(CEEASCIIEditRef edit,
                                         cee_float proportion)
{
    if (!edit || proportion < 0)
        return;
    
    CEEASCIILayoutRef layout = edit->layout;
    cee_float max_line_width = cee_ascii_layout_max_line_width_get(layout);
    cee_ascii_layout_horizontal_offset_set(edit->layout,
                                           proportion * max_line_width);
    cee_ascii_layout_run(edit->layout);
}

cee_boolean cee_ascii_edit_scroll_line_up(CEEASCIIEditRef edit)
{
    if (!edit)
        return FALSE;
        
    CEEASCIILayoutRef layout = edit->layout;
    cee_long index = cee_ascii_layout_paragraph_index_get(layout);
    
    if (index <= 0)
        return FALSE;
    
    index --;
    cee_ascii_layout_paragraph_index_set(layout, index);
    cee_ascii_layout_run(layout);
    return TRUE;
}

cee_boolean cee_ascii_edit_scroll_line_down(CEEASCIIEditRef edit)
{
    if (!edit)
        return FALSE;
    
    CEEASCIILayoutRef layout = edit->layout;
    cee_ulong nb_paragraph = cee_ascii_layout_paragraph_count_get(layout);
    cee_long index = cee_ascii_layout_paragraph_index_get(layout);
    
    if (index >= nb_paragraph - 1)
        return FALSE;
    
    index ++;
    cee_ascii_layout_paragraph_index_set(layout, index);
    cee_ascii_layout_run(layout);
    return TRUE;
}

CEEASCIIStorageRef cee_ascii_edit_storage_get(CEEASCIIEditRef edit)
{
    return edit->storage;
}
