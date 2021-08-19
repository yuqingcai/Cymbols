#ifndef __CEE_ASCII_EDIT_H__
#define __CEE_ASCII_EDIT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"
#include "cee_ascii_layout.h"
#include "cee_ascii_storage.h"

typedef struct _CEEASCIIEdit* CEEASCIIEditRef;

CEEASCIIEditRef cee_ascii_edit_create(cee_pointer host_ref);
void cee_ascii_edit_free(CEEASCIIEditRef edit);
void cee_ascii_edit_attributes_configure(CEEASCIIEditRef edit,
                                         const cee_uchar* descriptor);
void cee_ascii_edit_container_size_set(CEEASCIIEditRef edit,
                                       CEESize size);
void cee_ascii_edit_storage_set(CEEASCIIEditRef edit,
                                CEEASCIIStorageRef storage);
CEEASCIILayoutRef cee_ascii_edit_layout_get(CEEASCIIEditRef edit);
cee_float cee_ascii_edit_vertical_scroller_offset_get(CEEASCIIEditRef edit);
cee_float cee_ascii_edit_vertical_scroller_proportion_get(CEEASCIIEditRef edit);
cee_float cee_ascii_edit_horizontal_scroller_offset_get(CEEASCIIEditRef edit);
cee_float cee_ascii_edit_horizontal_scroller_proportion_get(CEEASCIIEditRef edit);
void cee_ascii_edit_scroll_vertical_to(CEEASCIIEditRef edit,
                                       cee_float proportion);
void cee_ascii_edit_scroll_horizontal_to(CEEASCIIEditRef edit,
                                         cee_float proportion);
cee_boolean cee_ascii_edit_scroll_line_up(CEEASCIIEditRef edit);
cee_boolean cee_ascii_edit_scroll_line_down(CEEASCIIEditRef edit);
CEEASCIIStorageRef cee_ascii_edit_storage_get(CEEASCIIEditRef edit);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_ASCII_EDIT_H__ */
