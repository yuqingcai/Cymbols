#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "cee_mem.h"
#include "cee_tree.h"

cee_int cee_tree_count = 0;

CEETree* cee_tree_create(void)
{
    CEETree* tree = cee_malloc0(sizeof(CEETree));
    cee_tree_count ++;
    return tree;
}

void cee_tree_free(CEETree* tree)
{
    CEEList* p = CEE_TREE_CHILDREN_FIRST(tree);
    while (p) {
        CEETree* child = p->data;
        cee_tree_free(child);
        p = CEE_TREE_NODE_NEXT(p);
    }
    cee_list_free(tree->children);
    
    cee_free(tree);

    cee_tree_count --;
}

void cee_tree_free_full(CEETree* tree,
                        CEEDestroyNotify free_func)
{
    CEEList* p = CEE_TREE_CHILDREN_FIRST(tree);
    while (p) {
        CEETree* child = p->data;
        cee_tree_free(child);
        p = CEE_TREE_NODE_NEXT(p);
    }
    cee_list_free(tree->children);
    
    (*free_func)(tree->data);
    cee_free(tree);
    
    cee_tree_count --;
}

cee_int cee_tree_count_get(void)
{
    return cee_tree_count;
}

CEETree* cee_tree_insert(CEETree* tree,
                         cee_pointer data)
{
    CEETree* new_tree = cee_tree_create();
    new_tree->data = data;
    new_tree->parent = tree;
    
    if (tree)
        tree->children = cee_list_prepend(tree->children, new_tree);
    
    return new_tree;
}

void cee_tree_foreach(CEETree* tree,
                      CEEFunc func,
                      cee_pointer user_data)
{
    CEEList* p = NULL;
    
    (*func)(tree->data, user_data);
    
    if (tree->children) {
        p = CEE_TREE_CHILDREN_FIRST(tree);
        while (p) {
            tree = p->data;
            cee_tree_foreach(tree, func, user_data);
            p = CEE_TREE_NODE_NEXT(p);
        }
    }
}

void cee_tree_sort(CEETree* tree, 
                   CEECompareFunc compare_func)
{
    CEEList* p = CEE_TREE_CHILDREN_FIRST(tree);
    while (p) {
        CEETree* child = p->data;
        cee_tree_sort(child, compare_func);
        p = CEE_TREE_NODE_NEXT(p);
    }
    tree->children = cee_list_sort(tree->children, compare_func);
}
