#ifndef __CEE_BINARY_EDIT_H__
#define __CEE_BINARY_EDIT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_binary_layout.h"
#include "cee_binary_storage.h"

typedef struct _CEEBinaryEdit* CEEBinaryEditRef;

CEEBinaryEditRef cee_binary_edit_create(cee_pointer host_ref);
void cee_binary_edit_free(CEEBinaryEditRef edit);
void cee_binary_edit_attributes_configure(CEEBinaryEditRef edit,
                                          const cee_uchar* descriptor);
void cee_binary_edit_container_size_set(CEEBinaryEditRef edit,
                                        CEESize size);
void cee_binary_edit_storage_set(CEEBinaryEditRef edit,
                                 CEEBinaryStorageRef storage);
CEEBinaryLayoutRef cee_binary_edit_layout_get(CEEBinaryEditRef edit);
cee_float cee_binary_edit_vertical_scroller_offset_get(CEEBinaryEditRef edit);
cee_float cee_binary_edit_vertical_scroller_proportion_get(CEEBinaryEditRef edit);
cee_float cee_binary_edit_horizontal_scroller_offset_get(CEEBinaryEditRef edit);
cee_float cee_binary_edit_horizontal_scroller_proportion_get(CEEBinaryEditRef edit);
void cee_binary_edit_scroll_vertical_to(CEEBinaryEditRef edit,
                                        cee_float proportion);
void cee_binary_edit_scroll_horizontal_to(CEEBinaryEditRef edit,
                                          cee_float proportion);
cee_boolean cee_binary_edit_scroll_line_up(CEEBinaryEditRef edit);
cee_boolean cee_binary_edit_scroll_line_down(CEEBinaryEditRef edit);
CEEBinaryStorageRef cee_binary_edit_storage_get(CEEBinaryEditRef edit);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BINARY_EDIT_H__ */
