#ifndef __CEE_BST_H__
#define __CEE_BST_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_list.h"

typedef struct _CEEBST CEEBST;

typedef struct _CEEBST {
    cee_pointer data;
    CEEBST* left;
    CEEBST* right;
} CEEBST;

CEEBST* cee_bst_create(CEEList* list);
void cee_bst_free(CEEBST* bst);
cee_int cee_bst_count_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_BST_H__ */
