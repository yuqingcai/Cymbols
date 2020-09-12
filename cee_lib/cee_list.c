#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "cee_mem.h"
#include "cee_list.h"

CEEList* cee_list_create()
{
    CEEList* list = cee_malloc0(sizeof(CEEList));
    return list;
}

void cee_list_free(CEEList* list)
{
    while (list) {
        CEEList* next = list->next;
        cee_free(list);
        list = next;
    }
}

void cee_list_free_full(CEEList* list,
                        CEEDestroyNotify free_func)
{
    CEEList* p = list;
    while (p) {
        (*free_func)(p->data);
        p->data = NULL;
        p = p->next;
    }
    
    cee_list_free(list);
}

CEEList* cee_list_append(CEEList* list,
                         cee_pointer data)
{
    CEEList *new_list = NULL;
    CEEList *last = NULL;
    
    new_list = cee_list_create();
    new_list->data = data;
    new_list->prev = NULL;
    new_list->next = NULL;
    
    if (!list)
        return new_list;
    
    last = cee_list_last(list);
    last->next = new_list;
    new_list->prev = last;
    return list;
}

CEEList* cee_list_prepend(CEEList* list,
                          cee_pointer data)
{
    CEEList *new_list = NULL;
    
    new_list = cee_list_create();
    new_list->data = data;
    new_list->next = NULL;
    new_list->prev = NULL;
    
    if (list) {
        list->prev = new_list;
        new_list->next = list;
    }
    
    return new_list;
}

CEEList* cee_list_concat(CEEList *list1,
                         CEEList *list2)
{
    CEEList *tmp_list;
    
    if (list2) {
        
        tmp_list = cee_list_last(list1);
        if (tmp_list)
            tmp_list->next = list2;
        else
            list1 = list2;
        
        list2->prev = tmp_list;
    }
    return list1;
}

CEEList* cee_list_first(CEEList *list)
{
    if (list) {
        while (list->prev)
            list = list->prev;
    }
    
    return list;
}

CEEList* cee_list_last(CEEList* list)
{
    if (list) {
        while (list->next)
            list = list->next;
    }
    
    return list;
}

CEEList* cee_list_nth(CEEList *list,
                      cee_uint n)
{
    while ((n-- > 0) && list)
        list = list->next;
    
    return list;
}

CEEList * cee_list_nth_prev(CEEList* list,
                            cee_uint n)
{
    while((n-- > 0) && list)
        list = list->prev;
    
    return list;
}

cee_pointer cee_list_nth_data(CEEList *list,
                              cee_uint n)
{
    while ((n-- > 0) && list)
        list = list->next;
    
    return list ? list->data : NULL;
}

cee_uint cee_list_length(CEEList *list)
{
    if (!list)
        return 0;
    
    cee_uint length = 0;
    
    while (list) {
        length++;
        list = list->next;
    }
    
    return length;
}

void cee_list_foreach(CEEList* list,
                      CEEFunc func,
                      cee_pointer user_data)
{
    while (list) {
        CEEList *next = list->next;
        (*func)(list->data, user_data);
        list = next;
    }
}

CEEList* cee_list_insert(CEEList* list,
                         cee_pointer data,
                         cee_int position)
{
    CEEList *new_list;
    CEEList *tmp_list;
    
    if (position < 0)
        return cee_list_append(list, data);
    else if (position == 0)
        return cee_list_prepend(list, data);
    
    tmp_list = cee_list_nth(list, position);
    if (!tmp_list)
        return cee_list_append(list, data);
    
    new_list = cee_list_create();
    new_list->data = data;
    new_list->prev = tmp_list->prev;
    tmp_list->prev->next = new_list;
    new_list->next = tmp_list;
    tmp_list->prev = new_list;
    
    return list;
}

cee_int cee_list_position(CEEList *list,
                          CEEList *llink)
{
    cee_int i;
    
    i = 0;
    while (list) {
        if (list == llink)
            return i;
        i++;
        list = list->next;
    }
    
    return -1;
}

cee_int cee_list_index(CEEList* list,
                       const cee_pointer data)
{
    cee_int i;
    
    i = 0;
    while (list) {
        if (list->data == data)
            return i;
        i++;
        list = list->next;
    }
    
    return -1;
}

CEEList* cee_list_find(CEEList* list,
                       const cee_pointer data)
{
    while (list) {
        if (list->data == data)
            break;
        list = list->next;
    }
    
    return list;
}

CEEList* cee_list_copy(CEEList* list)
{
    return cee_list_copy_deep(list, NULL, NULL);
}

CEEList * cee_list_copy_deep(CEEList* list,
                             CEECopyFunc func,
                             cee_pointer user_data)
{
    CEEList *new_list = NULL;
    
    if (list) {
        CEEList *last;
        
        new_list = cee_list_create();
        if (func)
            new_list->data = func(list->data, user_data);
        else
            new_list->data = list->data;
        
        new_list->prev = NULL;
        last = new_list;
        list = list->next;
        while (list) {
            last->next = cee_list_create();
            last->next->prev = last;
            last = last->next;
            if (func)
                last->data = func(list->data, user_data);
            else
                last->data = list->data;
            list = list->next;
        }
        last->next = NULL;
    }
    
    return new_list;
}


static inline CEEList * _cee_list_remove_link(CEEList *list,
                                              CEEList *link)
{
    if (link == NULL)
        return list;
    
    if (link->prev) {
        if (link->prev->next == link)
            link->prev->next = link->next;
    }
    if (link->next) {
        if (link->next->prev == link)
            link->next->prev = link->prev;
    }
    
    if (link == list)
        list = list->next;
    
    link->next = NULL;
    link->prev = NULL;
    
    return list;
}

CEEList *cee_list_remove(CEEList *list,
                         const cee_pointer data)
{
    CEEList *tmp;
    
    tmp = list;
    while (tmp) {
        if (tmp->data != data)
            tmp = tmp->next;
        else {
            list = _cee_list_remove_link(list, tmp);
            cee_list_free(tmp);
            
            break;
        }
    }
    return list;
}

CEEList* cee_list_remove_all(CEEList* list,
                             const cee_pointer data)
{
    CEEList *tmp = list;
    
    while (tmp) {
        if (tmp->data != data)
            tmp = tmp->next;
        else {
            CEEList *next = tmp->next;
            
            if (tmp->prev)
                tmp->prev->next = next;
            else
                list = next;
            if (next)
                next->prev = tmp->prev;
            
            cee_list_free(tmp);
            tmp = next;
        }
    }
    return list;
}

CEEList* cee_list_remove_link(CEEList *list,
                              CEEList *llink)
{
    return _cee_list_remove_link(list, llink);
}

CEEList * cee_list_delete_link(CEEList *list,
                               CEEList *link)
{
    list = _cee_list_remove_link(list, link);
    cee_list_free(link);
    
    return list;
}

CEEList * cee_list_reverse(CEEList *list)
{
    CEEList *last;
    
    last = NULL;
    while (list) {
        last = list;
        list = last->next;
        last->next = last->prev;
        last->prev = list;
    }
    
    return last;
}

CEEList* cee_list_find_custom(CEEList* list,
                              const cee_pointer data,
                              CEECompareFunc func)
{
    if (!func)
        return list;
    
    while (list) {
        if (!func(list->data, data))
            return list;
        list = list->next;
    }
    
    return NULL;
}

CEEList* cee_list_insert_before(CEEList* list,
                                CEEList* sibling,
                                cee_pointer data)
{
    if (!list) {
        list = cee_list_create();
        list->data = data;
        return list;
    }
    
    if (sibling) {
        CEEList *node = cee_list_create();
        node->data = data;
        node->prev = sibling->prev;
        node->next = sibling;
        sibling->prev = node;
        if (node->prev) {
            node->prev->next = node;
            return list;
        }
        else {
            return node;
        }
    }
    else {
        CEEList *p = list;
        while (p->next)
            p = p->next;
        
        p->next = cee_list_create();
        p->next->data = data;
        p->next->prev = p;
        p->next->next = NULL;
        
        return list;
    }
}

CEEList* cee_list_insert_after(CEEList* list,
                               CEEList* sibling,
                               cee_pointer data)
{
    if (!list) {
        list = cee_list_create();
        list->data = data;
        return list;
    }
    
    if (sibling) {
        CEEList *node = cee_list_create();
        node->data = data;
        
        node->prev = sibling;
        node->next = sibling->next;
        if (node->next)
            node->next->prev = node;
        sibling->next = node;
        return list;
        
    }
    else {
        return cee_list_append(list, data);
    }
}

static CEEList* list_sort_merge(CEEList* l1,
                                CEEList* l2,
                                CEECompareFunc compare_func)
{
    CEEList list, *l, *lprev;
    cee_int cmp;
    
    l = &list;
    lprev = NULL;
    
    while (l1 && l2) {
        cmp = (compare_func)(l1->data, l2->data);
        
        if (cmp <= 0) {
            l->next = l1;
            l1 = l1->next;
        }
        else {
            l->next = l2;
            l2 = l2->next;
        }
        l = l->next;
        l->prev = lprev;
        lprev = l;
    }
    
    l->next = l1 ? l1 : l2;
    l->next->prev = l;
    
    return list.next;
}

static CEEList* list_sort(CEEList *list,
                          CEECompareFunc compare_func)
{
    CEEList *l1, *l2;
    
    if (!list)
        return NULL;
    
    if (!list->next)
        return list;
    
    l1 = list;
    l2 = list->next;
    
    while ((l2 = l2->next) != NULL) {
        if ((l2 = l2->next) == NULL)
            break;
        l1 = l1->next;
    }
    l2 = l1->next;
    l1->next = NULL;
    
    return list_sort_merge(list_sort(list, compare_func),
                           list_sort(l2, compare_func),
                           compare_func);
}

CEEList* cee_list_sort(CEEList* list,
                       CEECompareFunc compare_func)
{
    return list_sort(list, compare_func);
}

static CEEList* list_sort_merge_with_data(CEEList* l1,
                                          CEEList* l2,
                                          CEECompareDataFunc compare_func,
                                          cee_pointer user_data)
{
    CEEList list, *l, *lprev;
    cee_int cmp;
    
    l = &list;
    lprev = NULL;
    
    while (l1 && l2) {
        cmp = (compare_func)(l1->data, l2->data, user_data);
        
        if (cmp <= 0) {
            l->next = l1;
            l1 = l1->next;
        }
        else {
            l->next = l2;
            l2 = l2->next;
        }
        l = l->next;
        l->prev = lprev;
        lprev = l;
    }
    
    l->next = l1 ? l1 : l2;
    l->next->prev = l;
    
    return list.next;
}


static CEEList* list_sort_with_data(CEEList *list,
                                    CEECompareDataFunc compare_func,
                                    cee_pointer user_data)
{
    CEEList *l1, *l2;
    
    if (!list)
        return NULL;
    
    if (!list->next)
        return list;
    
    l1 = list;
    l2 = list->next;
    
    while ((l2 = l2->next) != NULL) {
        if ((l2 = l2->next) == NULL)
            break;
        l1 = l1->next;
    }
    l2 = l1->next;
    l1->next = NULL;
    
    return list_sort_merge_with_data(list_sort_with_data(list, compare_func, user_data),
                                     list_sort_with_data(l2, compare_func, user_data),
                                     compare_func,
                                     user_data);
}

CEEList* cee_list_sort_with_data(CEEList *list,
                                 CEECompareDataFunc compare_func,
                                 cee_pointer user_data)
{
    return list_sort_with_data (list, compare_func, user_data);
}
