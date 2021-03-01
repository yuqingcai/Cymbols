#ifndef __CEE_TEXT_EDIT_H__
#define __CEE_TEXT_EDIT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_text_layout.h"
#include "cee_text_storage.h"

typedef struct _CEETextEdit* CEETextEditRef;

CEETextEditRef cee_text_edit_create(cee_pointer platform,
                                    CEETextStorageRef storage,
                                    CEETextLayoutAlignment layout_aligment,
                                    void (*pasteboard_string_set)(cee_pointer, const cee_uchar*),
                                    void (*pasteboard_string_create)(cee_pointer, cee_uchar**));
void cee_text_edit_storage_set(CEETextEditRef edit, 
                               CEETextStorageRef storage);
CEETextStorageRef cee_text_edit_storage_get(CEETextEditRef edit);
void cee_text_edit_wrap_set(CEETextEditRef edit,
                            cee_boolean wrap);
cee_boolean cee_text_edit_wrap_get(CEETextEditRef edit);
void cee_text_edit_wrap_with_indent_set(CEETextEditRef edit,
                                        cee_boolean wrap_with_indent);
void cee_text_edit_aligment_set(CEETextEditRef edit,
                                CEETextLayoutAlignment aligment);
void cee_text_edit_free(CEETextEditRef edit);
void cee_text_edit_container_size_set(CEETextEditRef edit,
                                      CEESize size);
CEETextLayoutRef cee_text_edit_layout(CEETextEditRef edit);
void cee_text_modify_free(cee_pointer data);
void cee_text_edit_caret_move_right(CEETextEditRef edit);
void cee_text_edit_caret_move_left(CEETextEditRef edit);
void cee_text_edit_caret_move_up(CEETextEditRef edit);
void cee_text_edit_caret_move_down(CEETextEditRef edit);
void cee_text_edit_caret_move_word_right(CEETextEditRef edit);
void cee_text_edit_caret_move_word_left(CEETextEditRef edit);
void cee_text_edit_caret_move_paragraph_beginning(CEETextEditRef edit);
void cee_text_edit_caret_move_paragraph_end(CEETextEditRef edit);
void cee_text_edit_caret_move_document_beginning(CEETextEditRef edit);
void cee_text_edit_caret_move_document_end(CEETextEditRef edit);
void cee_text_edit_caret_move_right_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_left_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_down_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_up_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_word_right_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_word_left_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_paragraph_beginning_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_paragraph_end_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_document_beginning_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_document_end_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_paragraph_forward_selection(CEETextEditRef edit);
void cee_text_edit_caret_move_paragraph_backward_selection(CEETextEditRef edit);
void cee_text_edit_page_down(CEETextEditRef edit);
void cee_text_edit_page_up(CEETextEditRef edit);
void cee_text_edit_page_down_selection(CEETextEditRef edit);
void cee_text_edit_page_up_selection(CEETextEditRef edit);
cee_boolean cee_text_edit_scroll_line_up(CEETextEditRef edit);
cee_boolean cee_text_edit_scroll_line_down(CEETextEditRef edit);
void cee_text_edit_select_all(CEETextEditRef edit);
void cee_text_edit_select_paragraph(CEETextEditRef edit);
void cee_text_edit_select_word(CEETextEditRef edit);
void cee_text_edit_delete_forward(CEETextEditRef edit);
void cee_text_edit_delete_backward(CEETextEditRef edit);
void cee_text_edit_delete_word_forward(CEETextEditRef edit);
void cee_text_edit_delete_word_backward(CEETextEditRef edit);
void cee_text_edit_delete_to_paragraph_beginning(CEETextEditRef edit);
void cee_text_edit_delete_to_paragraph_end(CEETextEditRef edit);
void cee_text_edit_copy(CEETextEditRef edit);
void cee_text_edit_cut(CEETextEditRef edit);
void cee_text_edit_paste(CEETextEditRef edit);
void cee_text_edit_paste(CEETextEditRef edit);
void cee_text_edit_undo(CEETextEditRef edit);
void cee_text_edit_redo(CEETextEditRef edit);
void cee_text_edit_insert(CEETextEditRef edit,
                          const cee_uchar* str);
void cee_text_edit_replace_ranges(CEETextEditRef edit,
                                  const cee_uchar* str,
                                  CEEList* ranges);
void cee_text_edit_scroll_left(CEETextEditRef edit);
void cee_text_edit_scroll_right(CEETextEditRef edit);
void cee_text_edit_scroll_vertical_to(CEETextEditRef edit,
                                      cee_float proportion);
void cee_text_edit_scroll_horizontal_to(CEETextEditRef edit,
                                        cee_float proportion);
cee_float cee_text_edit_vertical_scroller_offset_get(CEETextEditRef edit);
cee_float cee_text_edit_vertical_scroller_proportion_get(CEETextEditRef edit);
cee_float cee_text_edit_horizontal_scroller_offset_get(CEETextEditRef edit);
cee_float cee_text_edit_horizontal_scroller_proportion_get(CEETextEditRef edit);
void cee_text_edit_selection_anchor_position_set(CEETextEditRef edit,
                                                 CEEPoint position);
void cee_text_edit_selection_complete_position_set(CEETextEditRef edit,
                                                   CEEPoint position);
cee_boolean cee_text_edit_search(CEETextEditRef edit,
                                 cee_char* subject,
                                 cee_boolean regex,
                                 cee_boolean sensitive,
                                 CEEStringSearchMode mode,
                                 cee_boolean* timeout);
void cee_text_edit_searched_index_set(CEETextEditRef edit,
                                      cee_long index);
cee_long cee_text_edit_searched_index_get(CEETextEditRef edit);
CEEList* cee_text_edit_searched_ranges_get(CEETextEditRef edit);
CEEList* cee_text_edit_searched_rects_create(CEETextEditRef edit);
CEEList* cee_text_edit_searched_highlight_rects_create(CEETextEditRef edit);
void cee_text_edit_mark_text(CEETextEditRef edit,
                             const cee_uchar* str);
CEERange cee_text_edit_marked_range_get(CEETextEditRef edit);
void cee_text_edit_marked_selection_set(CEETextEditRef edit,
                                        CEERange range);
CEERange cee_text_edit_marked_selection_get(CEETextEditRef edit);
void cee_text_edit_marked_discard(CEETextEditRef edit);
CEEList* cee_text_edit_marked_rects_create(CEETextEditRef edit);
CEEList* cee_text_edit_marked_selection_rects_create(CEETextEditRef edit);
cee_ulong cee_text_edit_marked_character_count_get(CEETextEditRef edit);
void cee_text_edit_modified_update(CEETextEditRef edit);
cee_ulong cee_text_edit_paragraph_count_get(CEETextEditRef edit);
CEERange cee_text_edit_selection_range_get(CEETextEditRef edit);
void cee_text_edit_selection_range_set(CEETextEditRef edit, 
                                       CEERange range);
CEEList* cee_text_edit_selection_rects_create(CEETextEditRef edit);
void cee_text_edit_attributes_configure(CEETextEditRef edit,
                                        const cee_uchar* descriptor);
void cee_text_edit_highlight_clear(CEETextEditRef edit);
void cee_text_edit_highlight_set(CEETextEditRef edit,
                                 CEEList* ranges);
CEEList* cee_text_edit_highlight_rects_create(CEETextEditRef edit);
void cee_text_edit_caret_position_set(CEETextEditRef edit,
                                      CEEPoint position);
void cee_text_edit_caret_buffer_offset_set(CEETextEditRef edit,
                                           cee_long buffer_offset);
cee_long cee_text_edit_caret_buffer_offset_get(CEETextEditRef edit);
void cee_text_edit_cursor_position_set(CEETextEditRef edit,
                                       CEEPoint position);
cee_long cee_text_edit_cursor_buffer_offset_get(CEETextEditRef edit);
void cee_text_edit_scroll_to_pragraph(CEETextEditRef edit,
                                      cee_long index);
cee_long cee_text_edit_character_index_in_paragraph_by_buffer_offset_get(CEETextEditRef edit,
                                                                         cee_long buffer_offset);
cee_long cee_text_edit_paragraph_index_by_buffer_offset_get(CEETextEditRef edit,
                                                            cee_long buffer_offset);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TEXT_EDIT_H__ */
