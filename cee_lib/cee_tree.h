#ifndef __CEE_TREE_H__
#define __CEE_TREE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_types.h"
#include "cee_list.h"

typedef struct _CEETree {
    cee_pointer data;
    struct _CEETree* parent;
    CEEList* children;
} CEETree;

#define CEE_TREE_CHILDREN_FIRST(tree) cee_list_last(((CEETree*)tree)->children)
#define CEE_TREE_NODE_NEXT(p) (p)->prev

typedef void (*CEEDestroyNotify)(cee_pointer data);

void cee_tree_free(CEETree* tree);
void cee_tree_free_full(CEETree* tree,
                        CEEDestroyNotify free_func);
CEETree* cee_tree_create(void);
cee_int cee_tree_count_get(void);
CEETree* cee_tree_insert(CEETree* tree,
                         cee_pointer data);
void cee_tree_foreach(CEETree* tree,
                      CEEFunc func,
                      cee_pointer user_data);
void cee_tree_sort(CEETree* tree, 
                   CEECompareFunc compare_func);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TREE_H__ */
