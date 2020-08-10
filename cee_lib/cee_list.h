#ifndef __CEE_LIST_H__
#define __CEE_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_types.h"

typedef struct _CEEList {
    cee_pointer data;
    struct _CEEList* next;
    struct _CEEList* prev;
} CEEList;

void cee_list_free(CEEList* list);
void cee_list_free_full(CEEList* list,
                        CEEDestroyNotify free_func);
CEEList* cee_list_create(void);
CEEList* cee_list_append(CEEList* list,
                         cee_pointer data);
CEEList* cee_list_prepend(CEEList* list,
                          cee_pointer data);
CEEList* cee_list_concat(CEEList *list1,
                         CEEList *list2);
CEEList* cee_list_first(CEEList *list);
CEEList* cee_list_last(CEEList* list);
CEEList* cee_list_nth(CEEList *list,
                      cee_uint n);
CEEList * cee_list_nth_prev(CEEList* list,
                            cee_uint n);
cee_pointer cee_list_nth_data(CEEList *list,
                              cee_uint n);
cee_uint cee_list_length(CEEList *list);
void cee_list_foreach(CEEList* list,
                      CEEFunc func,
                      cee_pointer user_data);
CEEList* cee_list_insert(CEEList* list,
                         cee_pointer data,
                         cee_int position);
cee_int cee_list_position(CEEList *list,
                          CEEList *llink);
cee_int cee_list_index(CEEList* list,
                       const cee_pointer data);
CEEList* cee_list_find(CEEList* list,
                       const cee_pointer data);
CEEList * cee_list_copy(CEEList *list);
CEEList * cee_list_copy_deep(CEEList* list,
                             CEECopyFunc func,
                             cee_pointer user_data);
CEEList *cee_list_remove(CEEList *list,
                         const cee_pointer data);
CEEList* cee_list_remove_all(CEEList* list,
                             const cee_pointer data);
CEEList* cee_list_remove_link(CEEList *list,
                              CEEList *llink);
CEEList * cee_list_delete_link(CEEList *list,
                               CEEList *link);
CEEList * cee_list_reverse(CEEList *list);
CEEList* cee_list_find_custom(CEEList* list,
                              const cee_pointer data,
                              CEECompareFunc func);
CEEList* cee_list_insert_before(CEEList* list,
                                CEEList* sibling,
                                cee_pointer data);
CEEList* cee_list_insert_after(CEEList* list,
                               CEEList* sibling,
                               cee_pointer data);
CEEList* cee_list_sort(CEEList* list,
                       CEECompareFunc compare_func);
CEEList* cee_list_sort_with_data(CEEList *list,
                                 CEECompareDataFunc compare_func,
                                 cee_pointer user_data);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_LIST_H__ */
