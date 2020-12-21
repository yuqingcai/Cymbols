#include <stdlib.h>
#include <stdio.h>
#include "cee_mem.h"
#include "cee_bst.h"

cee_int cee_bst_count = 0;

static CEEBST* sorted_list_to_bst(CEEList **head, cee_ulong n) 
{ 
    if (n <= 0) 
        return NULL; 
  
    CEEBST* left = sorted_list_to_bst(head, (n/2));
  
    CEEBST* root = (CEEBST*)cee_malloc0(sizeof(CEEBST));
    cee_bst_count ++;
    
    root->data = *head;
    
    root->left = left;
    
    *head = (*head)->next;
    
    root->right = sorted_list_to_bst(head, n - (n/2) - 1);
  
    return root; 
}

CEEBST* cee_bst_create(CEEList* list) 
{
    cee_ulong n = cee_list_length(list);
    return sorted_list_to_bst(&list, n); 
}

void cee_bst_free(CEEBST* bst)
{
    if (!bst)
        return;
    
    CEEBST* left = bst->left;
    cee_bst_free(left);
    
    CEEBST* right = bst->right;
    cee_bst_free(right);
    
    cee_free(bst);
    
    cee_bst_count --;
}

cee_int cee_bst_count_get(void)
{
    return cee_bst_count;
}
