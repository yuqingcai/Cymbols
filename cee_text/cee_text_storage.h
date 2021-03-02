#ifndef __CEE_TEXT_STORAGE_H__
#define __CEE_TEXT_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include "cee_lib.h"
#include "cee_tag.h"
#include "cee_text_modify.h"

typedef enum _CEETextStorageLineBreakType {
    kCEETextStorageLineBreakTypeUnknow,
    kCEETextStorageLineBreakTypeLF,
    kCEETextStorageLineBreakTypeCR,
    kCEETextStorageLineBreakTypeCRLF,
} CEETextStorageLineBreakType;

typedef enum _CEETextStorageEncodeType {
    kCEETextStorageEncodeTypeUTF8,
    kCEETextStorageEncodeTypeUNICODE,
} CEETextStorageEncodeType;

typedef struct _CEETextStorage* CEETextStorageRef;
CEETextStorageRef cee_text_storage_create(cee_pointer observer,
                                          const cee_uchar* string,
                                          void (*update_notify)(cee_pointer,
                                                                CEETextStorageRef,
                                                                CEERange,
                                                                CEERange)
                                          );
void cee_text_storage_free(CEETextStorageRef data);
void cee_text_storage_buffer_replace(CEETextStorageRef storage,
                                     CEERange range,
                                     const cee_uchar* in,
                                     cee_uchar** out,
                                     cee_ulong* length,
                                     cee_boolean enable_update_notify);
void cee_text_storage_buffer_set(CEETextStorageRef storage,
                                 const cee_uchar* string);
const cee_uchar* cee_text_storage_buffer_get(CEETextStorageRef storage);
cee_ulong cee_text_storage_size_get(CEETextStorageRef storage);
cee_ulong cee_text_storage_character_count_get(CEETextStorageRef storage);
cee_ulong cee_text_storage_max_paragraph_length_get(CEETextStorageRef storage);
cee_ulong cee_text_storage_paragraph_count_get(CEETextStorageRef storage);
cee_ulong cee_text_storage_line_break_length_get(CEETextStorageRef storage);
CEETextStorageLineBreakType cee_text_storage_line_break_type_get(CEETextStorageRef storage);
void cee_text_storage_buffer_character_next(CEETextStorageRef storage,
                                            cee_long current,
                                            cee_long* next,
                                            CEEUnicodePoint* codepoint,
                                            cee_ulong* length);
void cee_text_storage_buffer_character_prev(CEETextStorageRef storage,
                                            cee_long current,
                                            cee_long* prev,
                                            CEEUnicodePoint* codepoint,
                                            cee_ulong* length);
void cee_text_storage_buffer_word_next(CEETextStorageRef storage,
                                       cee_long current,
                                       cee_long* next);
void cee_text_storage_buffer_word_prev(CEETextStorageRef storage,
                                       cee_long current,
                                       cee_long* prev);
cee_long cee_text_storage_paragraph_index_get(CEETextStorageRef storage,
                                              cee_long buffer_offset);
cee_long cee_text_storage_buffer_offset_by_paragraph_index(CEETextStorageRef storage,
                                                           cee_long index);
cee_long cee_text_storage_paragraph_beginning_get(CEETextStorageRef storage,
                                                  cee_long buffer_offset);
cee_long cee_text_storage_paragraph_end_get(CEETextStorageRef storage,
                                            cee_long buffer_offset);
cee_long cee_text_storage_paragraph_prev_get(CEETextStorageRef storage,
                                             cee_long buffer_offset);
cee_long cee_text_storage_paragraph_next_get(CEETextStorageRef storage,
                                             cee_long buffer_offset);
cee_long cee_text_storage_character_index_in_paragraph(CEETextStorageRef storage,
                                                       cee_long paragraph,
                                                       cee_long buffer_offset);
cee_long cee_text_storage_character_count_in_paragraph(CEETextStorageRef storage,
                                                       cee_long paragraph);
cee_long cee_text_storage_buffer_offset_by_character_index(CEETextStorageRef storage,
                                                           cee_long paragraph,
                                                           cee_long character);
void cee_text_storage_modify_logger_create(CEETextStorageRef storage,
                                           void (*modify_free)(cee_pointer));
void cee_text_storage_modify_prepend(CEETextStorageRef storage,
                                     CEETextModifyRef modify);
CEETextModifyRef cee_text_storage_modify_backward(CEETextStorageRef storage);
CEETextModifyRef cee_text_storage_modify_forward(CEETextStorageRef storage);
void cee_text_storage_modify_clear(CEETextStorageRef storage);
CEERange cee_text_storage_word_get(CEETextStorageRef storage,
                                   cee_long buffer_offset);
void cee_text_storage_attribute_generate(CEETextStorageRef storage,
                                         CEERange range);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TEXT_STORAGE_H__ */
