#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include "cee_text_edit.h"
#include "cee_text_autocomplete.h"
#include "cee_tag.h"
#include "cee_regex.h"

typedef struct _CEETextCaret {
    cee_long buffer_offset;
    cee_long character_index;
} CEETextCaret;

typedef struct _CEETextCursor {
    cee_long buffer_offset;
    cee_long character_index;
} CEETextCursor;

typedef struct _CEETextSelection {
    CEERange range;
    cee_long anchor;
} CEETextSelection;

typedef struct _CEETextSearch {
    cee_char* subject;
    CEEList* ranges;
    cee_ulong index;
} CEETextSearch;

typedef struct _CEETextMarked {
    CEERange range;
    CEERange selected;
} CEETextMarked;

typedef struct _CEETextHighlight {
    CEEList* ranges;
} CEETextHighlight;

typedef struct _CEETextEditState {
    CEEList* carets;
    CEEList* selections;
} CEETextEditState;

typedef struct _CEETextFregment {
    CEERange range;
    cee_uchar* subject;
} CEETextFregment;

typedef struct _CEETextReplacement {
    CEETextFregment* lval;
    CEETextFregment* rval;
} CEETextReplacement;

typedef struct _CEETextModify {
    CEEList* replacements;
    CEETextEditState* state0;
    CEETextEditState* state1;
} CEETextModify;

typedef struct _CEETextEdit {
    cee_pointer host_ref;
    CEETextStorageRef storage_ref;
    CEETextLayoutRef layout;
    cee_pointer platform;
    void (*pasteboard_string_set)(cee_pointer, const cee_uchar*);
    void (*pasteboard_string_create)(cee_pointer, cee_uchar**);
    CEETextCaret caret;
    CEETextCursor cursor;
    cee_boolean wrap;
    CEERect viewport;
    CEETextSelection selection;
    CEETextMarked marked;
    CEETextSearch search;
    CEETextHighlight higthlight;
    cee_float horizontal_scroll_margin;
    cee_long paragraph_vertical_margin;
    cee_ulong search_timeout_ms;
} CEETextEdit;

typedef struct _CEETextEditCommand {
    cee_char* name;
    void (*func)(CEETextEdit* edit,
                 cee_pointer input,
                 cee_pointer* output);
} CEETextEditCommand;

static cee_boolean has_selection(CEETextEdit* edit);
static void selection_free(cee_pointer data);
static CEETextSelection* selection_copy(CEETextSelection* src);
static void selection_discard(CEETextEdit* edit);
static void selection_anchor_set(CEETextEdit* edit,
                                 cee_long buffer_offset);
static void selection_content_delete(CEETextEdit* edit);
static void selection_complete(CEETextEditRef edit,
                               cee_long buffer_offset);
static cee_boolean has_marked(CEETextEdit* edit);
static void marked_discard(CEETextEdit* edit);
static void marked_text_replace(CEETextEdit* edit, 
                                const cee_uchar* str);
static void caret_free(cee_pointer data);
static CEETextCaret* caret_copy(CEETextCaret* src);
static void text_fregment_free(CEETextFregment* fregment);
static CEETextFregment* text_fregment_create(CEETextStorageRef storage,
                                             CEERange range);
static void text_replacement_free(cee_pointer data);
static CEETextReplacement* text_replacement_create(CEETextFregment* lval,
                                                   CEETextFregment* rval);
static void text_edit_state_free(cee_pointer data);
static CEETextEditState* text_edit_state_create(CEETextEdit* edit);
static void text_modify_free(cee_pointer data);
static CEETextModifyRef text_modify_create(CEEList* replacements,
                                           CEETextEditState* state0,
                                           CEETextEditState* state1);
static cee_boolean buffer_offset_is_outof_layout_vertical(CEETextEdit* edit,
                                                          cee_long buffer_offset);
static void layout_vertical_adjust_to_pragraph_index(CEETextEdit* edit,
                                                     cee_long index,
                                                     cee_long margin);
static void layout_vertical_adjust_to_buffer_offset(CEETextEdit* edit,
                                                    cee_long buffer_offset);
static cee_boolean buffer_offset_is_outof_layout_horizontal(CEETextEdit* edit,
                                                            cee_long buffer_offset);
static void layout_horizontal_adjust_to_buffer_offset(CEETextEdit* edit,
                                                      cee_long buffer_offset);
static void layout_adjust_to_buffer_offset_if_need(CEETextEdit* edit,
                                                   cee_long buffer_offset);
static void caret_buffer_offset_set(CEETextEdit* edit,
                                    cee_long buffer_offset,
                                    cee_boolean character_index);

static void search_discard(CEETextEdit* edit);
static cee_boolean has_highlight(CEETextEdit* edit);
static void highlight_discard(CEETextEdit* edit);
static void text_edit_reset(CEETextEdit* edit);
static void caret_discard(CEETextEdit* edit);
static void cursor_discard(CEETextEdit* edit);

static cee_boolean has_selection(CEETextEdit* edit)
{
    return edit->selection.range.length ? TRUE : FALSE;
}

static void selection_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETextSelection* selection = (CEETextSelection*)data;
    cee_free(selection);
}

static CEETextSelection* selection_copy(CEETextSelection* src)
{
    CEETextSelection* dst = (CEETextSelection*)cee_malloc(sizeof(CEETextSelection));
    memcpy(dst, src, sizeof(CEETextSelection));
    return dst;
}

static void selection_discard(CEETextEdit* edit)
{
    edit->selection.anchor = edit->caret.buffer_offset;
    edit->selection.range = cee_range_make(0, 0);
}

static void selection_anchor_set(CEETextEdit* edit,
                                 cee_long buffer_offset)
{
    if (buffer_offset == -1)
        return ;
    
    edit->selection.anchor = buffer_offset;
}

static void selection_content_delete(CEETextEdit* edit)
{
    CEETextStorageRef storage = edit->storage_ref;
    
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    CEERange range0;
    CEERange range1;
    
    state0 = text_edit_state_create(edit);
    range0 = edit->selection.range;
    fregment0 = text_fregment_create(storage, range0);
    
    layout_adjust_to_buffer_offset_if_need(edit, range0.location);
    cee_text_storage_buffer_replace(storage, 
                                    range0, 
                                    (const cee_uchar*)"",
                                    NULL,
                                    NULL);
    
    edit->caret.buffer_offset = range0.location;
    selection_discard(edit);
    edit->selection.anchor = edit->caret.buffer_offset;
    state1 = text_edit_state_create(edit);
    range1 = cee_range_make(range0.location, 0);
    fregment1 = text_fregment_create(storage, range1);
        
    replacement = text_replacement_create(fregment0, fregment1);
    replacements = cee_list_append(replacements, replacement);
    modify = text_modify_create(replacements, state0, state1);
    cee_text_storage_modify_prepend(storage, modify);
    
    cee_text_layout_run(edit->layout);
}

static void selection_complete(CEETextEditRef edit,
                               cee_long buffer_offset)
{
    CEETextStorageRef storage = edit->storage_ref;
    const cee_uchar* buffer = cee_text_storage_buffer_get(storage);
    cee_float start_offset = 0;
    cee_float end_offset = 0;
    cee_ulong length = 0;
    
    if (buffer_offset == edit->selection.anchor) {
        edit->selection.range.length = 0;
        return ;
    }
    
    if (edit->selection.anchor < buffer_offset) {
        start_offset = edit->selection.anchor;
        end_offset = cee_codec_utf8_decode_prev(buffer,
                                                buffer_offset,
                                                NULL,
                                                NULL);
    }
    else if (edit->selection.anchor > buffer_offset) {
        start_offset = buffer_offset;
        end_offset = cee_codec_utf8_decode_prev(buffer,
                                                edit->selection.anchor,
                                                NULL,
                                                NULL);
    }
    
    length = cee_codec_utf8_decode_length(buffer, end_offset);
    length = (end_offset + length) - start_offset;
    edit->selection.range = cee_range_make(start_offset, length);
}
static void marked_text_replace(CEETextEdit* edit, 
                                const cee_uchar* str)
{
    CEETextLayoutRef layout = edit->layout;
    CEETextStorageRef storage = edit->storage_ref;
    cee_ulong length = 0;
    cee_long paragraph = -1;
    
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    CEERange range0;
    CEERange range1;
    
    edit->caret.buffer_offset = edit->marked.range.location;
    state0 = text_edit_state_create(edit);
    
    range0 = cee_range_make(edit->marked.range.location, 0);
    
    CEETextCaret* caret = cee_list_first(state0->carets)->data;
    caret->buffer_offset = range0.location;
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         caret->buffer_offset);
    caret->character_index = 
        cee_text_storage_character_index_in_paragraph(storage, 
                                                      paragraph,                  
                                                      edit->caret.buffer_offset);
    
    fregment0 = text_fregment_create(storage, range0);
    cee_text_storage_buffer_replace(storage, 
                                    edit->marked.range, 
                                    str,
                                    NULL,
                                    &length);
    
    range1 = cee_range_make(edit->marked.range.location, length);
    fregment1 = text_fregment_create(storage, range1);
    replacement = text_replacement_create(fregment0, fregment1);
    
    edit->caret.buffer_offset = range0.location + length;
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         edit->caret.buffer_offset);
    edit->caret.character_index = 
        cee_text_storage_character_index_in_paragraph(storage, 
                                                      paragraph,                  
                                                      edit->caret.buffer_offset);
    edit->selection.anchor = edit->caret.buffer_offset;
    
    state1 = text_edit_state_create(edit);
    replacements = cee_list_append(replacements, replacement);
    
    modify = text_modify_create(replacements, state0, state1);
    cee_text_storage_modify_prepend(storage, modify);
    
    cee_text_layout_run(layout);
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

static void caret_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETextCaret* caret = (CEETextCaret*)data;
    cee_free(caret);
}

static CEETextCaret* caret_copy(CEETextCaret* src)
{
    CEETextCaret* dst = (CEETextCaret*)cee_malloc(sizeof(CEETextCaret));
    memcpy(dst, src, sizeof(CEETextCaret));
    return dst;
}

static void text_fregment_free(CEETextFregment* fregment)
{
    if (!fregment)
        return;
        
    if (fregment->subject)
        cee_free(fregment->subject);
    
    cee_free(fregment);
}

static CEETextFregment* text_fregment_create(CEETextStorageRef storage,
                                             CEERange range)
{
    const cee_uchar* buffer = cee_text_storage_buffer_get(storage);
    CEETextFregment* fregment = (CEETextFregment*)cee_malloc0(sizeof(CEETextFregment));
    cee_long location = range.location;
    cee_ulong length = range.length;
    fregment->subject = (cee_uchar*)cee_strndup((cee_char*)&buffer[location], length);
    fregment->range = range;
    return fregment;
}

static void text_replacement_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETextReplacement* replacement = (CEETextReplacement*)data;
    
    if (replacement->lval)
        text_fregment_free(replacement->lval);
    
    if (replacement->rval)
        text_fregment_free(replacement->rval);
    
    cee_free(replacement);
}

static CEETextReplacement* text_replacement_create(CEETextFregment* lval,
                                                   CEETextFregment* rval)
{
    CEETextReplacement* replacement = 
        (CEETextReplacement*)cee_malloc0(sizeof(CEETextReplacement));
    replacement->lval = lval;
    replacement->rval = rval;
    return replacement;
}

static void text_edit_state_free(cee_pointer data)
{
    if (!data)
        return ;
    
    CEETextEditState* state = (CEETextEditState*)data;
    
    if (state->carets)
        cee_list_free_full(state->carets, caret_free);
    
    if (state->selections)
        cee_list_free_full(state->selections, selection_free);
    
    cee_free(state);
}

static CEETextEditState* text_edit_state_create(CEETextEdit* edit)
{
    CEETextEditState* state = (CEETextEditState*)cee_malloc0(sizeof(CEETextEditState));
    CEETextCaret* caret = caret_copy(&edit->caret);
    CEETextSelection* selection = selection_copy(&edit->selection);
    state->carets = cee_list_append(state->carets, caret);
    state->selections = cee_list_append(state->selections, selection);
    
    return state;
}

static void text_modify_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETextModify* modify = (CEETextModify*)data;
    
    if (modify->replacements)
        cee_list_free_full(modify->replacements, text_replacement_free);
    
    if (modify->state0)
        text_edit_state_free(modify->state0);
    
    if (modify->state1)
        text_edit_state_free(modify->state1);
    
    cee_free(modify);
}

CEETextModifyRef text_modify_create(CEEList* replacements,
                                    CEETextEditState* state0,
                                    CEETextEditState* state1)
{
    CEETextModify* modify = (CEETextModify*)cee_malloc0(sizeof(CEETextModify));
    modify->replacements = replacements;
    modify->state0 = state0;
    modify->state1 = state1;
    return modify;
}

static cee_boolean buffer_offset_is_outof_layout_vertical(CEETextEdit* edit,
                                                          cee_long buffer_offset)
{
    CEETextLayoutRef layout = edit->layout;
    CEETextUnitRef head_unit = cee_text_layout_head_unit_get(layout);
    CEETextUnitRef tail_unit = cee_text_layout_tail_unit_get(layout);
    cee_long head_offset = cee_text_unit_buffer_offset_get(head_unit);
    cee_long tail_offset = cee_text_unit_buffer_offset_get(tail_unit);
    cee_ulong tail_length = cee_text_unit_buffer_length_get(tail_unit);
    
    if (buffer_offset < head_offset || buffer_offset >= tail_offset + tail_length)
        return TRUE;
    
    return FALSE;
}

static void layout_vertical_adjust_to_pragraph_index(CEETextEdit* edit,
                                                     cee_long index,
                                                     cee_long margin)
{
    CEETextLayoutRef layout = edit->layout;    
    
    if (margin == -1) {
        if (index >= edit->paragraph_vertical_margin)
            index -= edit->paragraph_vertical_margin;
        else
            index = 0;
    }
    else {
        if (index < margin)
            index = 0;
        else
            index -= margin;
    }
    
    cee_text_layout_paragraph_index_set(layout, index);
    cee_text_layout_run(layout);
}

static void layout_vertical_adjust_to_buffer_offset(CEETextEdit* edit,
                                                    cee_long buffer_offset)
{
    CEETextStorageRef storage = edit->storage_ref;
    cee_long index = cee_text_storage_paragraph_index_get(storage, buffer_offset);
    layout_vertical_adjust_to_pragraph_index(edit, index, -1);
}

static cee_boolean buffer_offset_is_outof_layout_horizontal(CEETextEdit* edit,
                                                            cee_long buffer_offset)
{
    CEETextLayoutRef layout = edit->layout;
    cee_float horizontal_offset = cee_text_layout_horizontal_offset_get(layout);
    cee_float x0 = horizontal_offset;
    cee_float x1 = x0 + edit->viewport.size.width - edit->horizontal_scroll_margin;
    CEERect bounds;
    CEETextUnitRef unit = cee_text_unit_get_by_offset(layout, buffer_offset);
    
    if (!unit)
        return FALSE;
    
    bounds = cee_text_unit_bounds_get(unit);
    
    if ((x0 >= bounds.origin.x + bounds.size.width) || 
        (x0 > bounds.origin.x && x0 < bounds.origin.x + bounds.size.width) || 
        (x1 < bounds.origin.x + bounds.size.width))
        return TRUE;
    
    return FALSE;
}

static void layout_horizontal_adjust_to_buffer_offset(CEETextEdit* edit,
                                                      cee_long buffer_offset)
{
    CEETextLayoutRef layout = edit->layout;
    cee_float horizontal_offset = cee_text_layout_horizontal_offset_get(layout);
    cee_float x0 = horizontal_offset;
    cee_float x1 = x0 + edit->viewport.size.width - edit->horizontal_scroll_margin;
    CEERect bounds;
    CEETextUnitRef unit = cee_text_unit_get_by_offset(layout, buffer_offset);
    if (!unit)
        return;
    
    bounds = cee_text_unit_bounds_get(unit);
    
    if ((x0 >= bounds.origin.x + bounds.size.width) || 
        (x0 > bounds.origin.x && x0 < bounds.origin.x + bounds.size.width))
        horizontal_offset = bounds.origin.x;
    else if (x1 < bounds.origin.x + bounds.size.width)
        horizontal_offset += (bounds.origin.x + bounds.size.width - x1);
    
    cee_text_layout_horizontal_offset_set(layout, horizontal_offset);
    cee_text_layout_run(layout);
}

static cee_boolean buffer_offset_is_clipped_layout_bounds_vertical(CEETextEdit* edit,
                                                                   cee_long buffer_offset)
{
    CEETextLayoutRef layout = edit->layout;
    CEETextLineRef line = NULL;
    CEERect line_bounds;
    CEERect viewport = edit->viewport;
        
    line = cee_text_line_get_by_offset(layout, buffer_offset);
    if (!line)
        return FALSE;
    
    line_bounds = cee_text_line_bounds_get(line);
    
    if (line_bounds.origin.y + line_bounds.size.height > 
        viewport.origin.y + viewport.size.height)
        return TRUE;
    
    return FALSE;
}

static void layout_bounds_vertical_adjust_to_buffer_offset(CEETextEdit* edit,
                                                           cee_long buffer_offset)
{
    if (cee_text_edit_scroll_line_down(edit))
        cee_text_layout_run(edit->layout);
}

static void layout_adjust_to_buffer_offset_if_need(CEETextEdit* edit,
                                                   cee_long buffer_offset)
{
    if (buffer_offset_is_clipped_layout_bounds_vertical(edit, buffer_offset))
        layout_bounds_vertical_adjust_to_buffer_offset(edit, buffer_offset);
    
    if (buffer_offset_is_outof_layout_vertical(edit, buffer_offset))
        layout_vertical_adjust_to_buffer_offset(edit, buffer_offset);
    
    if (buffer_offset_is_outof_layout_horizontal(edit, buffer_offset))
        layout_horizontal_adjust_to_buffer_offset(edit, buffer_offset);
}


static void caret_buffer_offset_set(CEETextEdit* edit,
                                    cee_long buffer_offset,
                                    cee_boolean character_index)
{
    if (buffer_offset == -1)
        return ;
    
    CEETextStorageRef storage = edit->storage_ref;
    cee_long paragraph = -1;
    edit->caret.buffer_offset = buffer_offset;
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         buffer_offset);
    if (character_index) {
        edit->caret.character_index =
            cee_text_storage_character_index_in_paragraph(storage,
                                                          paragraph,
                                                          buffer_offset);
    }
}

void cee_text_edit_free(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    text_edit_reset(edit);
    
    if (edit->layout)
        cee_text_layout_free(edit->layout);
    
    if (edit->platform)
        cee_text_platform_free(edit->platform);
        
    cee_free(edit);
}

void cee_text_edit_storage_set(CEETextEditRef edit, 
                               CEETextStorageRef storage)
{
    text_edit_reset(edit);
    cee_text_storage_modify_logger_create(storage, text_modify_free);
    edit->storage_ref = storage;
    if (edit->layout) {
        cee_text_layout_storage_set(edit->layout, edit->storage_ref);
        cee_text_layout_run(edit->layout);
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
    }
}

static void text_edit_reset(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    if (edit->layout)
        cee_text_layout_reset(edit->layout);
    
    caret_discard(edit);
    cursor_discard(edit);
    selection_discard(edit);
    marked_discard(edit);
    search_discard(edit);
    highlight_discard(edit);
}

CEETextEditRef cee_text_edit_create(cee_pointer host_ref,
                                    CEETextStorageRef storage,
                                    CEETextLayoutAlignment layout_aligment,
                                    void (*pasteboard_string_set)(cee_pointer, const cee_uchar*),
                                    void (*pasteboard_string_create)(cee_pointer, cee_uchar**))
{    
    CEETextEdit* edit = (CEETextEdit*)cee_malloc0(sizeof(CEETextEdit));
    edit->wrap = TRUE;
    edit->host_ref = host_ref;
    edit->platform = cee_text_platform_create();
    edit->layout = cee_text_layout_create(storage,
                                          edit->platform,
                                          layout_aligment,
                                          4,
                                          3.0,
                                          6.0, 6.0, 6.0, 6.0,
                                          cee_rect_make(0.0, 0.0, 0.0, 0.0));
    edit->horizontal_scroll_margin = 0.0;
    edit->paragraph_vertical_margin = 4;
    edit->pasteboard_string_set = pasteboard_string_set;
    edit->pasteboard_string_create = pasteboard_string_create;
    edit->search_timeout_ms = 10000;
    
    cee_text_edit_storage_set(edit, storage);
    text_edit_reset(edit);
    
    return edit;
}

void cee_text_edit_aligment_set(CEETextEditRef edit,
                                CEETextLayoutAlignment aligment)
{
    cee_text_layout_aligment_set(edit->layout, 
                                 aligment);
    cee_text_layout_run(edit->layout);
}

void cee_text_edit_wrap_set(CEETextEditRef edit,
                            cee_boolean wrap) {
    edit->wrap = wrap;
    CEETextLayoutRef layout = edit->layout;
    
    CEERect bounds = cee_rect_make(0.0, 0.0, 0.0, 0.0);
    if (edit->wrap)
        bounds = cee_rect_make(0.0, 0.0, edit->viewport.size.width, 
                               edit->viewport.size.height);
    else
        bounds = cee_rect_make(0.0, 0.0, FLT_MAX, edit->viewport.size.height);
    
    cee_text_layout_bounds_set(layout, bounds);
    cee_text_layout_run(layout);
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

cee_boolean cee_text_edit_wrap_get(CEETextEditRef edit) {
    return edit->wrap;
}

void cee_text_edit_container_size_set(CEETextEditRef edit,
                                      CEESize size)
{
    if (!edit)
        return;
        
    CEETextLayoutRef layout = edit->layout;
    CEERect bounds = cee_rect_make(0.0, 0.0, 0.0, 0.0);
    if (edit->wrap)
        bounds = cee_rect_make(0.0, 0.0, size.width, size.height);
    else
        bounds = cee_rect_make(0.0, 0.0, FLT_MAX, size.height);
    
    edit->viewport = cee_rect_make(0.0, 0.0, size.width, size.height);
    
    cee_text_layout_bounds_set(layout, bounds);
    cee_text_layout_run(layout);
}

void cee_text_edit_pasteboard_string_set(CEETextEditRef edit,
                                         void (*callback)(cee_pointer, const cee_uchar*))
{
    if (!edit)
        return;
    edit->pasteboard_string_set = callback;
}

void cee_text_edit_pasteboard_string_create(CEETextEditRef edit,
                                            void (*callback)(cee_pointer, cee_uchar**))
{
    if (!edit)
        return;
    edit->pasteboard_string_create = callback;
}

CEETextLayoutRef cee_text_edit_layout(CEETextEditRef edit)
{
    if (!edit)
        return NULL;
    
    return edit->layout;
}

void cee_text_edit_caret_move_right(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (has_selection(edit)) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = cee_text_storage_buffer_character_next(storage,
                                                           buffer_offset,
                                                           NULL,
                                                           NULL);
    if (buffer_offset == -1)
        return;
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_left(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (has_selection(edit)) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = cee_text_storage_buffer_character_prev(storage,
                                                           buffer_offset,
                                                           NULL,
                                                           NULL);
    if (buffer_offset == -1)
        return;
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_up(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_long buffer_offset = edit->caret.buffer_offset;
    cee_long character_index = edit->caret.character_index;
    CEETextStorageRef storage = edit->storage_ref;
    cee_long paragraph = -1;
    cee_ulong nb_character = 0;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (has_selection(edit)) {
        selection_discard(edit);
        return;
    }
    
    paragraph = cee_text_storage_paragraph_prev_get(storage,
                                                    buffer_offset);
    if (paragraph == -1)
        return;
    
    nb_character = cee_text_storage_character_count_in_paragraph(storage,
                                                                 paragraph);
    
    if (character_index > nb_character - 1)
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              nb_character - 1);
    else
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              character_index);
    
    caret_buffer_offset_set(edit, buffer_offset, FALSE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_down(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_long buffer_offset = edit->caret.buffer_offset;
    cee_long character_index = edit->caret.character_index;
    CEETextStorageRef storage = edit->storage_ref;
    cee_long paragraph = -1;
    cee_ulong nb_character = 0;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (edit->selection.range.length) {
        selection_discard(edit);
        return;
    }
    
    
    paragraph = cee_text_storage_paragraph_next_get(storage,
                                                    buffer_offset);
    if (paragraph == -1)
        return;
    
    nb_character = cee_text_storage_character_count_in_paragraph(storage,
                                                                 paragraph);
    
    if (character_index > nb_character - 1)
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              nb_character - 1);
    else
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              character_index);
    caret_buffer_offset_set(edit, buffer_offset, FALSE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_word_right(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (edit->selection.range.length) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = cee_text_storage_buffer_word_next(storage,
                                                      buffer_offset);
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_word_left(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (edit->selection.range.length) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = cee_text_storage_buffer_word_prev(storage,
                                                      buffer_offset);
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_paragraph_beginning(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (edit->selection.range.length) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = cee_text_storage_paragraph_beginning_get(storage,
                                                             buffer_offset);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_paragraph_end(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (edit->selection.range.length) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = cee_text_storage_paragraph_end_get(storage,
                                                       buffer_offset);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_document_beginning(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (edit->selection.range.length) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = 0;
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_document_end(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    cee_long nb_paragraph = cee_text_storage_paragraph_count_get(storage);
    cee_long last_paragraph = cee_text_storage_paragraph_by_index(storage,
                                                                  nb_paragraph - 1);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    if (edit->selection.range.length) {
        selection_discard(edit);
        return ;
    }
    
    buffer_offset = cee_text_storage_paragraph_end_get(storage,
                                                       last_paragraph);
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_anchor_set(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_right_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = cee_text_storage_buffer_character_next(storage,
                                                           buffer_offset,
                                                           NULL,
                                                           NULL);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_left_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = cee_text_storage_buffer_character_prev(storage,
                                                           buffer_offset,
                                                           NULL,
                                                           NULL);

    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_up_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_long buffer_offset = edit->caret.buffer_offset;
    cee_long character_index = edit->caret.character_index;
    CEETextStorageRef storage = edit->storage_ref;
    cee_long paragraph = -1;
    cee_ulong nb_character = 0;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    paragraph = cee_text_storage_paragraph_prev_get(storage,
                                                    buffer_offset);
    if (paragraph == -1)
        return;
    
    nb_character = cee_text_storage_character_count_in_paragraph(storage,
                                                                 paragraph);
    
    if (character_index > nb_character - 1)
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              nb_character - 1);
    else
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              character_index);
    
    caret_buffer_offset_set(edit, buffer_offset, FALSE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_down_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_long buffer_offset = edit->caret.buffer_offset;
    cee_long character_index = edit->caret.character_index;
    CEETextStorageRef storage = edit->storage_ref;
    cee_long paragraph = -1;
    cee_ulong nb_character = 0;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    paragraph = cee_text_storage_paragraph_next_get(storage,
                                                    buffer_offset);
    if (paragraph == -1)
        return;
    
    nb_character = cee_text_storage_character_count_in_paragraph(storage,
                                                                 paragraph);
    
    if (character_index > nb_character - 1)
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              nb_character - 1);
    else
        buffer_offset = 
            cee_text_storage_buffer_offset_by_character_index(storage,
                                                              paragraph,
                                                              character_index);
    
    caret_buffer_offset_set(edit, buffer_offset, FALSE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_word_right_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = cee_text_storage_buffer_word_next(storage,
                                                      buffer_offset);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_word_left_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = cee_text_storage_buffer_word_prev(storage,
                                                      buffer_offset);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_paragraph_beginning_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = cee_text_storage_paragraph_beginning_get(storage,
                                                             buffer_offset);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_paragraph_end_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = cee_text_storage_paragraph_end_get(storage,
                                                       buffer_offset);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_document_beginning_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = 0;
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_document_end_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_float buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    cee_long nb_paragraph = cee_text_storage_paragraph_count_get(storage);
    cee_long last_paragraph = cee_text_storage_paragraph_by_index(storage,
                                                                  nb_paragraph - 1);
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    buffer_offset = cee_text_storage_paragraph_end_get(storage,
                                                       last_paragraph);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_paragraph_forward_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_long paragraph = -1;
    cee_ulong buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    paragraph = cee_text_storage_paragraph_next_get(storage,
                                                    buffer_offset);
    if (paragraph == -1)
        return;
    
    buffer_offset = cee_text_storage_paragraph_end_get(storage, paragraph);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_caret_move_paragraph_backward_selection(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    cee_long paragraph = -1;
    cee_ulong buffer_offset = edit->caret.buffer_offset;
    CEETextStorageRef storage = edit->storage_ref;
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
    
    
    paragraph = cee_text_storage_paragraph_prev_get(storage,
                                                    buffer_offset);
    if (paragraph == -1)
        return;
    
    buffer_offset = cee_text_storage_paragraph_beginning_get(storage, paragraph);
    
    caret_buffer_offset_set(edit, buffer_offset, TRUE);
    selection_complete(edit, buffer_offset);
    
    layout_adjust_to_buffer_offset_if_need(edit, buffer_offset);
}

void cee_text_edit_page_down(CEETextEditRef edit)
{
    
}

void cee_text_edit_page_up(CEETextEditRef edit)
{
    
}

void cee_text_edit_page_down_selection(CEETextEditRef edit)
{
    
}

void cee_text_edit_page_up_selection(CEETextEditRef edit)
{
    
}

cee_boolean cee_text_edit_scroll_line_up(CEETextEditRef edit)
{
    if (!edit)
        return FALSE;
    
    CEETextLayoutRef layout = edit->layout;
    cee_long index = cee_text_layout_paragraph_index_get(layout);
    
    if (index <= 0)
        return FALSE;
    
    index --;
    cee_text_layout_paragraph_index_set(layout, index);
    cee_text_layout_run(layout);
    return TRUE;
}

cee_boolean cee_text_edit_scroll_line_down(CEETextEditRef edit)
{
    if (!edit)
        return FALSE;
    
    CEETextLayoutRef layout = edit->layout;
    CEETextStorageRef storage = edit->storage_ref;
    cee_ulong nb_paragraph = cee_text_storage_paragraph_count_get(storage);
    cee_long index = cee_text_layout_paragraph_index_get(layout);
    
    if (index >= nb_paragraph - 1)
        return FALSE;
    
    index ++;
    cee_text_layout_paragraph_index_set(layout, index);
    cee_text_layout_run(layout);
    return TRUE;
}

void cee_text_edit_select_all(CEETextEditRef edit)
{
    CEETextStorageRef storage = edit->storage_ref;
    edit->selection.anchor = 0;
    edit->selection.range.location = 0;
    edit->selection.range.length = cee_text_storage_size_get(storage);
}

void cee_text_edit_select_paragraph(CEETextEditRef edit)
{
    CEETextStorageRef storage = edit->storage_ref;
    cee_long location0 = 
        cee_text_storage_paragraph_beginning_get(storage,
                                                edit->selection.anchor);
    cee_long location1 = 
        cee_text_storage_paragraph_end_get(storage,
                                           edit->selection.anchor);
    CEERange range = cee_range_make(location0, location1 - location0);
    
    edit->selection.anchor = range.location;
    edit->selection.range = range;
    edit->caret.buffer_offset = range.location + range.length;
    edit->caret.character_index =
        cee_text_storage_character_index_in_paragraph(storage,
                                                      location0,
                                                      edit->caret.buffer_offset);
}

void cee_text_edit_select_word(CEETextEditRef edit)
{
    CEETextStorageRef storage = edit->storage_ref;
    cee_long buffer_offset = edit->selection.anchor;
    CEERange range = cee_text_storage_word_get(storage, buffer_offset);
    cee_long paragraph = -1;
    
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         range.location);
    edit->selection.anchor = range.location;
    edit->selection.range = range;
    edit->caret.buffer_offset = range.location + range.length;
    edit->caret.character_index = 
        cee_text_storage_character_index_in_paragraph(storage,
                                                      paragraph,
                                                      edit->caret.buffer_offset);
}

void cee_text_edit_delete_forward(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    cee_ulong length = 0;
    CEERange range0;
    CEERange range1;
    cee_long next = -1;
    
    if (has_selection(edit)) {
        selection_content_delete(edit);
    }
    else {
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
        next = cee_text_storage_buffer_character_next(storage,
                                                      edit->caret.buffer_offset,
                                                      NULL,
                                                      &length);
        if (next == -1)
            return;
        
        state0 = text_edit_state_create(edit);
        range0 = cee_range_make(edit->caret.buffer_offset, length);
        fregment0 = text_fregment_create(storage, range0);
        
        cee_text_storage_buffer_replace(storage,
                                        range0,
                                        (const cee_uchar*)"",
                                        NULL,
                                        NULL);
        
        state1 = text_edit_state_create(edit);
        range1 = cee_range_make(edit->caret.buffer_offset, 0);
        fregment1 = text_fregment_create(storage, range1);
        
        replacement = text_replacement_create(fregment0, fregment1);
        replacements = cee_list_append(replacements, replacement);
        modify = text_modify_create(replacements, state0, state1);
        cee_text_storage_modify_prepend(storage, modify);
        
        cee_text_layout_run(layout);
    }
}

void cee_text_edit_delete_backward(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    cee_ulong length = 0;
    CEERange range0;
    CEERange range1;
    cee_long paragraph = -1;
    cee_long prev = -1;
    
    if (has_selection(edit)) {
        selection_content_delete(edit);
    }
    else {
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
        
        prev = cee_text_storage_buffer_character_prev(storage,
                                                      edit->caret.buffer_offset,
                                                      NULL,
                                                      &length);
        if (prev == -1)
            return;
        
        state0 = text_edit_state_create(edit);
        range0 = cee_range_make(prev, length);
        fregment0 = text_fregment_create(storage, range0);
        
        cee_text_storage_buffer_replace(storage,
                                        range0,
                                        (const cee_uchar*)"",
                                        NULL,
                                        NULL);
        
        edit->caret.buffer_offset = prev;
        paragraph = cee_text_storage_paragraph_beginning_get(storage, prev);
        edit->caret.character_index =
            cee_text_storage_character_index_in_paragraph(storage,
                                                          paragraph,
                                                          prev);
        edit->selection.anchor = edit->caret.buffer_offset;
        
        state1 = text_edit_state_create(edit);
        range1 = cee_range_make(prev, 0);
        fregment1 = text_fregment_create(storage, range1);
        
        replacement = text_replacement_create(fregment0, fregment1);
        replacements = cee_list_append(replacements, replacement);
        modify = text_modify_create(replacements, state0, state1);
        cee_text_storage_modify_prepend(storage, modify);
        
        cee_text_layout_run(layout);
        
        if (buffer_offset_is_outof_layout_vertical(edit, edit->caret.buffer_offset)) {
            if (cee_text_edit_scroll_line_up(edit))
                cee_text_layout_run(layout);
        }
        
        if (buffer_offset_is_outof_layout_horizontal(edit, edit->caret.buffer_offset))
            layout_horizontal_adjust_to_buffer_offset(edit, edit->caret.buffer_offset);
    }
}

void cee_text_edit_delete_word_forward(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    cee_ulong length = 0;
    CEERange range0;
    CEERange range1;
    cee_long next = -1;
    
    if (has_selection(edit)) {
        selection_content_delete(edit);
    }
    else {
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
        next = cee_text_storage_buffer_word_next(storage,
                                                 edit->caret.buffer_offset);
        if (next == -1)
            return;
        
        length = next - edit->caret.buffer_offset;
        if (!length)
            return;
        
        state0 = text_edit_state_create(edit);
        range0 = cee_range_make(edit->caret.buffer_offset, length);
        fregment0 = text_fregment_create(storage, range0);
        
        cee_text_storage_buffer_replace(storage,
                                        range0,
                                        (const cee_uchar*)"",
                                        NULL,
                                        NULL);
        
        state1 = text_edit_state_create(edit);
        range1 = cee_range_make(edit->caret.buffer_offset, 0);
        fregment1 = text_fregment_create(storage, range1);
        
        replacement = text_replacement_create(fregment0, fregment1);
        replacements = cee_list_append(replacements, replacement);
        modify = text_modify_create(replacements, state0, state1);
        cee_text_storage_modify_prepend(storage, modify);
        
        cee_text_layout_run(layout);
    }
}

void cee_text_edit_delete_word_backward(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    cee_ulong length = 0;
    CEERange range0;
    CEERange range1;
    cee_long paragraph = -1;
    cee_long prev = -1;
    
    if (has_selection(edit)) {
        selection_content_delete(edit);
    }
    else {
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
        
        prev = cee_text_storage_buffer_word_prev(storage,
                                                 edit->caret.buffer_offset);
        if (prev == -1)
            return;
        
        length = edit->caret.buffer_offset - prev;
        if (!length)
            return;
        
        state0 = text_edit_state_create(edit);
        range0 = cee_range_make(prev, length);
        fregment0 = text_fregment_create(storage, range0);
        
        cee_text_storage_buffer_replace(storage,
                                        range0,
                                        (const cee_uchar*)"",
                                        NULL,
                                        NULL);
        
        edit->caret.buffer_offset = prev;
        paragraph = cee_text_storage_paragraph_beginning_get(storage, prev);
        edit->caret.character_index =
            cee_text_storage_character_index_in_paragraph(storage,
                                                          paragraph,
                                                          prev);
        edit->selection.anchor = edit->caret.buffer_offset;
        
        state1 = text_edit_state_create(edit);
        range1 = cee_range_make(prev, 0);
        fregment1 = text_fregment_create(storage, range1);
        
        replacement = text_replacement_create(fregment0, fregment1);
        replacements = cee_list_append(replacements, replacement);
        modify = text_modify_create(replacements, state0, state1);
        cee_text_storage_modify_prepend(storage, modify);
        
        cee_text_layout_run(layout);
        
        if (buffer_offset_is_outof_layout_vertical(edit, edit->caret.buffer_offset)) {
            if (cee_text_edit_scroll_line_up(edit))
                cee_text_layout_run(layout);
        }
        
        if (buffer_offset_is_outof_layout_horizontal(edit, edit->caret.buffer_offset))
            layout_horizontal_adjust_to_buffer_offset(edit, edit->caret.buffer_offset);
    }
}

void cee_text_edit_delete_to_paragraph_beginning(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    cee_ulong length = 0;
    CEERange range0;
    CEERange range1;
    cee_long paragraph = -1;
    cee_long prev = -1;
    
    if (has_selection(edit)) {
        selection_content_delete(edit);
    }
    else {
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
        
        prev = cee_text_storage_paragraph_beginning_get(storage,
                                                        edit->caret.buffer_offset);
        if (prev == -1)
            return;
        
        if (prev == edit->caret.buffer_offset) {
            prev = cee_text_storage_buffer_character_prev(storage,
                                                          edit->caret.buffer_offset,
                                                          NULL,
                                                          &length);
        }
        else {
            length = edit->caret.buffer_offset - prev;
        }
        
        state0 = text_edit_state_create(edit);
        range0 = cee_range_make(prev, length);
        fregment0 = text_fregment_create(storage, range0);
        
        cee_text_storage_buffer_replace(storage,
                                        range0,
                                        (const cee_uchar*)"",
                                        NULL,
                                        NULL);
        
        edit->caret.buffer_offset = prev;
        paragraph = cee_text_storage_paragraph_beginning_get(storage, prev);
        edit->caret.character_index =
            cee_text_storage_character_index_in_paragraph(storage,
                                                          paragraph,
                                                          prev);
        edit->selection.anchor = edit->caret.buffer_offset;
        
        state1 = text_edit_state_create(edit);
        range1 = cee_range_make(prev, 0);
        fregment1 = text_fregment_create(storage, range1);
        
        replacement = text_replacement_create(fregment0, fregment1);
        replacements = cee_list_append(replacements, replacement);
        modify = text_modify_create(replacements, state0, state1);
        cee_text_storage_modify_prepend(storage, modify);
        
        cee_text_layout_run(layout);
        
        if (buffer_offset_is_outof_layout_vertical(edit, edit->caret.buffer_offset)) {
            if (cee_text_edit_scroll_line_up(edit))
                cee_text_layout_run(layout);
        }
        
        if (buffer_offset_is_outof_layout_horizontal(edit, edit->caret.buffer_offset))
            layout_horizontal_adjust_to_buffer_offset(edit, edit->caret.buffer_offset);
    }
}

void cee_text_edit_delete_to_paragraph_end(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    cee_ulong length = 0;
    CEERange range0;
    CEERange range1;
    cee_long next = -1;
    
    if (has_selection(edit)) {
        selection_content_delete(edit);
    }
    else {
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
        next = cee_text_storage_paragraph_end_get(storage,
                                                  edit->caret.buffer_offset);
        if (next == -1)
            return;
        
        if (next == edit->caret.buffer_offset) {
            next = cee_text_storage_buffer_character_next(storage,
                                                          edit->caret.buffer_offset,
                                                          NULL,
                                                          &length);
        }
        else {
            length = next - edit->caret.buffer_offset;
        }
        
        state0 = text_edit_state_create(edit);
        range0 = cee_range_make(edit->caret.buffer_offset, length);
        fregment0 = text_fregment_create(storage, range0);
        
        cee_text_storage_buffer_replace(storage,
                                        range0,
                                        (const cee_uchar*)"",
                                        NULL,
                                        NULL);
        
        state1 = text_edit_state_create(edit);
        range1 = cee_range_make(edit->caret.buffer_offset, 0);
        fregment1 = text_fregment_create(storage, range1);
        
        replacement = text_replacement_create(fregment0, fregment1);
        replacements = cee_list_append(replacements, replacement);
        modify = text_modify_create(replacements, state0, state1);
        cee_text_storage_modify_prepend(storage, modify);
        
        cee_text_layout_run(layout);
    }
}

void cee_text_edit_copy(CEETextEditRef edit)
{
    if (!edit || !has_selection(edit))
        return;
    
    const cee_uchar* buffer = cee_text_storage_buffer_get(edit->storage_ref);
    cee_char* str = cee_strndup((cee_char*)&buffer[edit->selection.range.location],
                                edit->selection.range.length);
    edit->pasteboard_string_set(edit->host_ref, (const cee_uchar*)str);
    cee_free(str);
}

void cee_text_edit_cut(CEETextEditRef edit)
{
    if (!edit || !has_selection(edit))
        return;
    
    const cee_uchar* buffer = cee_text_storage_buffer_get(edit->storage_ref);
    CEERange range = edit->selection.range;
    cee_char* str = cee_strndup((cee_char*)&buffer[range.location], range.length);
    edit->pasteboard_string_set(edit->host_ref, (const cee_uchar*)str);
    cee_free(str);
    selection_content_delete(edit);
}

void cee_text_edit_paste(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    cee_long paragraph = -1;
    cee_ulong length = 0;
    cee_uchar* pasted_str = NULL;
    
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    CEERange range0;
    CEERange range1;
    
    cee_uchar* str = NULL;
    edit->pasteboard_string_create(edit->host_ref, &str);
    if (!str)
        return;
    
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
    
    state0 = text_edit_state_create(edit);
    
    if (has_selection(edit)) {
        range0 = edit->selection.range;
        selection_discard(edit);
    }
    else {
        range0 = cee_range_make(edit->caret.buffer_offset, 0);
    }
    
    fregment0 = text_fregment_create(storage, range0);
    
    cee_text_storage_buffer_replace(storage,
                                    range0,
                                    str,
                                    &pasted_str,
                                    &length);
    
    range1 = cee_range_make(range0.location, length);
    fregment1 = text_fregment_create(storage, range1);
    replacement = text_replacement_create(fregment0, fregment1);
    
    cee_free(str);
    cee_free(pasted_str);
    
    if (range0.length)
        edit->caret.buffer_offset = range0.location + length;
    else
        edit->caret.buffer_offset += length;
    
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         edit->caret.buffer_offset);
    edit->caret.character_index =
        cee_text_storage_character_index_in_paragraph(storage,
                                                      paragraph,
                                                      edit->caret.buffer_offset);
    edit->selection.anchor = edit->caret.buffer_offset;
    
    state1 = text_edit_state_create(edit);
    replacements = cee_list_append(replacements, replacement);
    modify = text_modify_create(replacements, state0, state1);
    cee_text_storage_modify_prepend(storage, modify);
    
    cee_text_layout_run(layout);
    
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

void cee_text_edit_undo(CEETextEditRef edit)
{
    if (!edit || has_marked(edit))
        return;
    
    CEETextLayoutRef layout = edit->layout;
    CEETextStorageRef storage = edit->storage_ref;
    CEETextModify* modify = NULL;
    CEEList* p = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextEditState* state0 = NULL;
    CEETextCaret* caret = NULL;
    CEETextSelection* selection = NULL;
    
    modify = cee_text_storage_modify_backward(storage);
    if (!modify)
        return;
    
    p = cee_list_last(modify->replacements);
    while (p) {
        replacement = p->data;
        fregment0 = replacement->lval;
        fregment1 = replacement->rval;
        cee_text_storage_buffer_replace(storage,
                                        fregment1->range,
                                        fregment0->subject,
                                        NULL,
                                        NULL);
        
        p = p->prev;
    }
    
    if (cee_text_layout_paragraph_index_is_invalid(layout))
        cee_text_layout_paragraph_index_set(layout, 0);
    
    cee_text_layout_run(layout);
    
    state0 = modify->state0;
    caret = cee_list_first(state0->carets)->data;
    selection = cee_list_first(state0->selections)->data;
    edit->caret = *caret;
    edit->selection = *selection;
    
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

void cee_text_edit_redo(CEETextEditRef edit)
{
    if (!edit || has_marked(edit))
        return;
    
    CEETextLayoutRef layout = edit->layout;
    CEETextStorageRef storage = edit->storage_ref;
    CEETextModify* modify = NULL;
    CEEList* p = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextCaret* caret = NULL;
    CEETextSelection* selection = NULL;
    
    modify = cee_text_storage_modify_forward(storage);
    if (!modify)
        return;
    
    p = modify->replacements;
    while (p) {
        replacement = p->data;
        fregment0 = replacement->lval;
        fregment1 = replacement->rval;
        cee_text_storage_buffer_replace(storage,
                                        fregment0->range,
                                        fregment1->subject,
                                        NULL,
                                        NULL);
        p = p->next;
    }
    
    if (cee_text_layout_paragraph_index_is_invalid(layout))
        cee_text_layout_paragraph_index_set(layout, 0);
    
    cee_text_layout_run(layout);
    
    state1 = modify->state1;
    caret = cee_list_first(state1->carets)->data;
    selection = cee_list_first(state1->selections)->data;
    edit->caret = *caret;
    edit->selection = *selection;
    
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

void cee_text_edit_insert(CEETextEditRef edit,
                          const cee_uchar* str)
{
    if (!edit)
        return;
    
    CEETextLayoutRef layout = edit->layout;
    CEETextStorageRef storage = edit->storage_ref;
    cee_ulong length = 0;
    cee_long paragraph = -1;
    
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    CEERange range0;
    CEERange range1;
    
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
    
    if (has_marked(edit)) {
        marked_text_replace(edit, str);
        marked_discard(edit);
        return;
    }
    
    state0 = text_edit_state_create(edit);
    
    if (has_selection(edit)) {
        range0 = edit->selection.range;
        selection_discard(edit);
    }
    else {
        range0 = cee_range_make(edit->caret.buffer_offset, 0);
    }
    
    fregment0 = text_fregment_create(storage, range0);
    
    cee_text_storage_buffer_replace(storage,
                                    range0,
                                    str,
                                    NULL,
                                    &length);
    
    range1 = cee_range_make(range0.location, length);
    fregment1 = text_fregment_create(storage, range1);
    replacement = text_replacement_create(fregment0, fregment1);
    
    if (range0.length)
        edit->caret.buffer_offset = range0.location + length;
    else
        edit->caret.buffer_offset += length;
    
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         edit->caret.buffer_offset);
    edit->caret.character_index =
        cee_text_storage_character_index_in_paragraph(storage,
                                                      paragraph,
                                                      edit->caret.buffer_offset);
    edit->selection.anchor = edit->caret.buffer_offset;
    
    state1 = text_edit_state_create(edit);
    replacements = cee_list_append(replacements, replacement);
    
    modify = text_modify_create(replacements, state0, state1);
    cee_text_storage_modify_prepend(storage, modify);
    
    cee_text_layout_run(layout);
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

void cee_text_edit_replace_ranges(CEETextEditRef edit,
                                  const cee_uchar* str,
                                  CEEList* ranges)
{    
    if (!edit)
        return;
    
    if (has_marked(edit) || has_selection(edit) || !ranges)
        return;
    
    CEETextLayoutRef layout = edit->layout;
    CEETextStorageRef storage = edit->storage_ref;
    cee_ulong length = 0;
    CEEList* p = NULL;
    CEETextEditState* state0 = NULL;
    CEETextEditState* state1 = NULL;
    CEETextFregment* fregment0 = NULL;
    CEETextFregment* fregment1 = NULL;
    CEETextReplacement* replacement = NULL;
    CEETextModify* modify = NULL;
    CEEList* replacements = NULL;
    CEERange range0;
    CEERange range1;
    
    CEERange* range = NULL;
    
    range = cee_list_first(ranges)->data;
    caret_buffer_offset_set(edit, range->location, TRUE);
    state0 = text_edit_state_create(edit);
    state1 = text_edit_state_create(edit);
    
    p = cee_list_last(ranges);
    while (p) {
        range = p->data;
        
        range0 = cee_range_make(range->location, range->length);
        fregment0 = text_fregment_create(storage, range0);
        
        cee_text_storage_buffer_replace(storage,
                                        range0,
                                        str,
                                        NULL,
                                        &length);

        range1 = cee_range_make(range0.location, length);
        fregment1 = text_fregment_create(storage, range1);
        
        replacement = text_replacement_create(fregment0, fregment1);
        replacements = cee_list_append(replacements, replacement);
        
        p = p->prev;
    }
    
    modify = text_modify_create(replacements, state0, state1);
    cee_text_storage_modify_prepend(storage, modify);
    
    cee_text_layout_run(layout);
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

void cee_text_edit_caret_position_set(CEETextEditRef edit,
                                      CEEPoint position)
{
    if (!edit)
        return;
    
    cee_long paragraph = -1;
    CEETextStorageRef storage = edit->storage_ref;
    CEETextUnitRef unit = cee_text_unit_get_by_location(edit->layout, position);
    if (!unit)
        return;
    
    edit->caret.buffer_offset = cee_text_unit_buffer_offset_get(unit);
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         edit->caret.buffer_offset);
    edit->caret.character_index =
        cee_text_storage_character_index_in_paragraph(storage, 
                                                      paragraph, 
                                                      edit->caret.buffer_offset);
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

cee_long cee_text_edit_caret_buffer_offset_get(CEETextEditRef edit)
{
    if (!edit)
        return -1;
    
    return edit->caret.buffer_offset;
}

void cee_text_edit_scroll_left(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    layout_horizontal_adjust_to_buffer_offset(edit, edit->caret.buffer_offset);
}

void cee_text_edit_scroll_right(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    layout_horizontal_adjust_to_buffer_offset(edit, edit->caret.buffer_offset);
}

void cee_text_edit_scroll_vertical_to(CEETextEditRef edit,
                                      cee_float proportion)
{
    if (!edit || proportion < 0)
        return;
    
    CEETextLayoutRef layout = edit->layout;
    CEETextStorageRef storage = edit->storage_ref;
    cee_ulong nb_paragraph = cee_text_storage_paragraph_count_get(storage);
    cee_long index = (nb_paragraph * proportion);
    index = index > 0 ? index - 1 : index;
    cee_text_layout_paragraph_index_set(layout, index);
    cee_text_layout_run(layout);
}

void cee_text_edit_scroll_horizontal_to(CEETextEditRef edit,
                                        cee_float proportion)
{
    if (!edit || proportion < 0)
        return;
    
    CEETextLayoutRef layout = edit->layout;
    cee_float max_line_width = cee_text_layout_max_line_width_get(layout);
    cee_text_layout_horizontal_offset_set(edit->layout,
                                          proportion * max_line_width);
    cee_text_layout_run(edit->layout);
}

cee_float cee_text_edit_vertical_scroller_offset_get(CEETextEditRef edit)
{
    if (!edit)
        return 0.0;
    
    CEETextStorageRef storage = edit->storage_ref;
    cee_ulong nb_paragraph = cee_text_storage_paragraph_count_get(storage);
    cee_ulong index = cee_text_layout_paragraph_index_get(edit->layout);
    if (!nb_paragraph)
        return 0;
    else
        return (cee_float)(index + 1) / (cee_float)nb_paragraph;
}


cee_float cee_text_edit_vertical_scroller_proportion_get(CEETextEditRef edit)
{
    if (!edit)
        return 1.0;
    
    CEETextStorageRef storage = edit->storage_ref;
    cee_ulong nb_paragraph = cee_text_storage_paragraph_count_get(storage);
    cee_ulong nb_present_line = cee_text_layout_paragraph_count_get(edit->layout);
    if (!nb_paragraph)
        return 1.0;
    else
        return ((cee_float)nb_present_line / (cee_float)nb_paragraph);
}

cee_float cee_text_edit_horizontal_scroller_offset_get(CEETextEditRef edit)
{
    if (!edit)
        return 0.0;
    
    CEETextLayoutRef layout = edit->layout;
    cee_float max_line_width = cee_text_layout_max_line_width_get(layout);
    cee_float horizontal_offset = cee_text_layout_horizontal_offset_get(layout);
    return horizontal_offset / max_line_width;
}

cee_float cee_text_edit_horizontal_scroller_proportion_get(CEETextEditRef edit)
{
    if (!edit)
        return 1.0;
    
    CEETextLayoutRef layout = edit->layout;
    cee_float value = 1.0;
    if (!edit->wrap)
        value = edit->viewport.size.width /
            cee_text_layout_max_line_width_get(layout);
    return value;
}

void cee_text_edit_selection_anchor_position_set(CEETextEditRef edit,
                                                 CEEPoint position)
{
    if (!edit)
        return;
    
    selection_discard(edit);
    CEETextUnitRef unit = cee_text_unit_get_by_location(edit->layout, position);
    if (!unit)
        return;
    
    edit->selection.anchor = cee_text_unit_buffer_offset_get(unit);
}

void cee_text_edit_selection_complete_position_set(CEETextEditRef edit,
                                                   CEEPoint position)
{
    if (!edit)
        return;
    
    CEETextUnitRef unit = cee_text_unit_get_by_location(edit->layout, position);
    if (!unit)
        return;
    
    cee_long offset = cee_text_unit_buffer_offset_get(unit);
    selection_complete(edit, offset);
}


static void caret_discard(CEETextEdit* edit)
{
    memset(&edit->caret, 0, sizeof(CEETextCaret));
}

static void cursor_discard(CEETextEdit* edit)
{
    memset(&edit->cursor, 0, sizeof(CEETextCursor));
}

static cee_boolean has_marked(CEETextEdit* edit)
{
    return edit->marked.range.length ? TRUE : FALSE;
}

static void marked_discard(CEETextEdit* edit)
{
    edit->marked.range.location = -1;
    edit->marked.range.length = 0;
    edit->marked.selected.location = -1;
    edit->marked.selected.length = 0;
}

static void search_discard(CEETextEdit* edit)
{
    CEETextSearch* search = &edit->search;
    if (search->subject)
        cee_free(search->subject);
    search->subject = NULL;
    
    if (search->ranges)
        cee_list_free_full(search->ranges, cee_range_free);
    search->ranges = NULL;
    search->index = -1;
}

static void highlight_discard(CEETextEdit* edit)
{
    if (edit->higthlight.ranges)
        cee_list_free_full(edit->higthlight.ranges, cee_range_free);
    edit->higthlight.ranges = NULL;
}

cee_boolean cee_text_edit_search(CEETextEditRef edit,
                                 cee_char* subject,
                                 cee_boolean regex,
                                 cee_boolean insensitive,
                                 CEEStringSearchMode mode,
                                 cee_boolean* timeout)
{
    search_discard(edit);
    
    CEETextStorageRef storage = edit->storage_ref;
    const cee_uchar* buffer = cee_text_storage_buffer_get(storage);
    CEETextSearch* search = &edit->search;
    search->subject = cee_strdup(subject);
    
    if (regex) {
        search->ranges = cee_regex_search((const cee_char*)buffer, 
                                          search->subject, 
                                          TRUE,
                                          0,
                                          edit->search_timeout_ms,
                                          timeout);
    }
    else {
        search->ranges = cee_str_search((const cee_char*)buffer, 
                                        search->subject, 
                                        insensitive, 
                                        mode);
    }
    
    if (search->ranges)
        search->index = 0;
    
    return search->ranges ? TRUE : FALSE;
}

void cee_text_edit_searched_index_set(CEETextEditRef edit,
                                      cee_long index)
{    
    CEETextSearch* search = &edit->search;
    CEERange* range = NULL;
    CEEList* ranges = search->ranges;
    cee_ulong nb_ranges = cee_list_length(ranges);
    
    if (!ranges)
        return;
    
    if (index < 0)
        index = nb_ranges - 1;
    else if (index >= nb_ranges)
        index = 0;
    
    search->index = index;
    range = cee_list_nth_data(edit->search.ranges, (cee_int)index);
    layout_adjust_to_buffer_offset_if_need(edit, range->location);
}

cee_long cee_text_edit_searched_index_get(CEETextEditRef edit)
{
    return edit->search.index;
}

CEEList* cee_text_edit_searched_ranges_get(CEETextEditRef edit)
{
    return edit->search.ranges;
}

CEEList* cee_text_edit_searched_rects_create(CEETextEditRef edit)
{
    CEEList* rects = NULL;
    
    if (!edit || !edit->search.ranges)
        return NULL;
    
    CEEList* p = edit->search.ranges;
    while (p) {
        CEERange* range = p->data;
        CEEList* q = cee_text_layout_rects_create(edit->layout, *range);
        rects = cee_list_concat(rects, q);
        p = p->next;
    }
    
    return rects;
}

CEEList* cee_text_edit_searched_highlight_rects_create(CEETextEditRef edit)
{
    CEETextSearch* search = &edit->search;
    if (!search->ranges ||
        search->index >= cee_list_length(search->ranges))
        return NULL;
    
    CEERange* range = cee_list_nth_data(search->ranges, 
                                        (cee_uint)search->index);
    return cee_text_layout_rects_create(edit->layout, *range);
}

void cee_text_edit_mark_text(CEETextEditRef edit,
                             const cee_uchar* str)
{
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    cee_ulong length = 0;
    CEERange range;
    cee_long paragraph = -1;
    
    if (has_selection(edit))
        selection_content_delete(edit);
    
    if (!has_marked(edit))
        range = cee_range_make(edit->caret.buffer_offset, 0);
    else
        range = edit->marked.range;
    
    cee_text_storage_buffer_replace(storage, range, str, NULL, &length);
    edit->marked.range = cee_range_make(range.location, length);
    
    edit->caret.buffer_offset = edit->marked.range.location +
        edit->marked.range.length;
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         edit->caret.buffer_offset);
    edit->caret.character_index =
        cee_text_storage_character_index_in_paragraph(storage,
                                                      paragraph,
                                                      edit->caret.buffer_offset);
    edit->selection.anchor = edit->caret.buffer_offset;
    
    cee_text_layout_run(layout);
    layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
}

CEERange cee_text_edit_marked_range_get(CEETextEditRef edit)
{
    if (!edit || !has_marked(edit))
        return cee_range_make(0, 0);
    
    return edit->marked.range;
}

void cee_text_edit_marked_selection_set(CEETextEditRef edit,
                                        CEERange range)
{
    if (!edit || !has_marked(edit))
        return;
    
    edit->marked.selected = range;
}

CEERange cee_text_edit_marked_selection_get(CEETextEditRef edit)
{
    if (!edit || !has_marked(edit))
        return cee_range_make(0, 0);
    
    return edit->marked.selected;
}

void cee_text_edit_marked_discard(CEETextEditRef edit)
{
    marked_discard(edit);
}

CEEList* cee_text_edit_marked_rects_create(CEETextEditRef edit)
{
    if (!edit || !has_marked(edit))
        return NULL;
    
    return cee_text_layout_rects_create(edit->layout, edit->marked.range);
}

CEEList* cee_text_edit_marked_selection_rects_create(CEETextEditRef edit)
{
    if (!edit || !has_marked(edit))
        return NULL;
    
    return cee_text_layout_rects_create(edit->layout, edit->marked.selected);
}

cee_ulong cee_text_edit_marked_character_count_get(CEETextEditRef edit)
{
    if (!edit)
        return 0;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEERange range = edit->marked.range;
    cee_ulong count = 0;
    cee_long current = range.location;
    
    if (has_marked(edit)) {
        while (TRUE) {
            current = cee_text_storage_buffer_character_next(storage,
                                                             current,
                                                             NULL,
                                                             NULL);
            count ++;
            
            if (current >= range.location + range.length)
                break;
        }
    }
    
    return count;
}

void cee_text_edit_modified_update(CEETextEditRef edit)
{
    if (!edit)
        return;
    
    CEETextStorageRef storage = edit->storage_ref;
    CEETextLayoutRef layout = edit->layout;
    cee_long index = cee_text_layout_paragraph_index_get(layout);
    cee_ulong nb_paragraph = cee_text_storage_paragraph_count_get(storage);
    
    selection_discard(edit);
    marked_discard(edit);
    
    if (index >= nb_paragraph) {
        index = 0;
        cee_text_layout_paragraph_index_set(layout, index);
        memset(&edit->caret, 0, sizeof(CEETextCaret));
        cee_text_layout_run(layout);
        layout_adjust_to_buffer_offset_if_need(edit, edit->caret.buffer_offset);
    }
    
    cee_text_layout_run(layout);
    
    return;
}

cee_ulong cee_text_edit_paragraph_count_get(CEETextEditRef edit)
{
    if (!edit)
        return 0;
    
    return cee_text_storage_paragraph_count_get(edit->storage_ref);
}

CEERange cee_text_edit_selection_range_get(CEETextEditRef edit)
{
    if (!edit)
        return cee_range_make(0, 0);
    return edit->selection.range;
}

void cee_text_edit_selection_range_set(CEETextEditRef edit, 
                                       CEERange range)
{
    if (!edit)
        return;
    
    selection_discard(edit);
    edit->selection.range = range;
}

CEEList* cee_text_edit_selection_rects_create(CEETextEditRef edit)
{
    if (!edit || !has_selection(edit))
        return NULL;
    
    return cee_text_layout_rects_create(edit->layout,
                                        edit->selection.range);
}

void cee_text_edit_attributes_configure(CEETextEditRef edit,
                                        const cee_uchar* descriptor)
{
    cee_text_platform_configure(edit->platform, descriptor);
    cee_text_layout_run(edit->layout);
}

static cee_boolean has_highlight(CEETextEdit* edit)
{
    return edit->higthlight.ranges != NULL;
}

void cee_text_edit_highlight_clear(CEETextEditRef edit)
{
    if (edit->higthlight.ranges)
        cee_list_free_full(edit->higthlight.ranges, cee_range_free);
    edit->higthlight.ranges = NULL;
}

cee_pointer range_copy(const cee_pointer src,
                       cee_pointer data)
{
    CEERange* range = (CEERange*)src;
    return cee_range_create(range->location, range->length);
}

void cee_text_edit_highlight_set(CEETextEditRef edit,
                                 CEEList* ranges)
{
    if (!edit)
        return;
    
    edit->higthlight.ranges = cee_list_copy_deep(ranges, range_copy, NULL);
}

CEEList* cee_text_edit_highlight_rects_create(CEETextEditRef edit)
{
    if (!edit || !has_highlight(edit))
        return NULL;
    
    CEEList* rects = NULL;
    CEERange* range = NULL;
    cee_ulong nb_ranges = cee_list_length(edit->higthlight.ranges);
    for (cee_ulong i = 0; i < nb_ranges; i ++) {
        range = cee_list_nth_data(edit->higthlight.ranges, (cee_int)i);
        rects = cee_list_concat(rects, cee_text_layout_rects_create(edit->layout, *range));
    }
    return rects;
}

void cee_text_edit_cursor_position_set(CEETextEditRef edit,
                                       CEEPoint position)
{
    if (!edit)
        return;
        
    cee_long paragraph = -1;
    CEETextStorageRef storage = edit->storage_ref;
    CEETextUnitRef unit = cee_text_unit_get_by_location(edit->layout, position);
    if (!unit)
        return;
    
    edit->cursor.buffer_offset = cee_text_unit_buffer_offset_get(unit);
    paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                         edit->cursor.buffer_offset);
    edit->cursor.character_index =
        cee_text_storage_character_index_in_paragraph(storage, 
                                                      paragraph, 
                                                      edit->cursor.buffer_offset);
}

cee_long cee_text_edit_cursor_buffer_offset_get(CEETextEditRef edit)
{
    if (!edit)
        return -1;
    
    return edit->cursor.buffer_offset;
}

void cee_text_edit_scroll_to_pragraph(CEETextEditRef edit,
                                      cee_long index)
{
    layout_vertical_adjust_to_pragraph_index(edit, index, -1);
}
