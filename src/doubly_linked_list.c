#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */

#include "doubly_linked_list.h"

typedef struct d_node d_node_t;
struct d_node
{
    void *data;
    d_node_t *next;
    d_node_t *prev;
};

struct d_list
{
    d_node_t head;
    d_node_t tail;
};
                        
static d_node_t *CreateNode(void *data);
                
                
/*******************************************************************/

d_list_t *DListCreate()
{
    d_list_t *d_list = malloc(sizeof(d_list_t));
    
    if (NULL == d_list)
    {
        return (NULL);
    }
    
    d_list->head.data = NULL;
    d_list->head.next = &d_list->tail;
    d_list->head.prev = NULL;
    
    d_list->tail.data = NULL;
    d_list->tail.next = NULL;
    d_list->tail.prev = &d_list->head;
    
    return (d_list);
}

/*****************/
void DListDestroy(d_list_t *d_list)
{
    dlist_iter_t current = DListBegin(d_list);
    dlist_iter_t end = DListEnd(d_list);
        
    while (!DListIsSameIter(current, end))
    {
        dlist_iter_t next = DListNext(current);
        free(current); 
        current = next;
    }
    
    free(d_list);
    d_list = NULL;
}

/*****************/
int DListIsEmpty(const d_list_t *d_list)
{
    return (DListIsSameIter(DListBegin((d_list_t *)d_list),
                            DListEnd((d_list_t *)d_list)));
}

/*****************/
size_t DListCount(const d_list_t *d_list)
{
    size_t elements_ctr = 0;
    dlist_iter_t current = DListBegin((d_list_t *)d_list);
    dlist_iter_t end = DListEnd((d_list_t *)d_list);
    
    while (!DListIsSameIter(current, end))
    {
        ++elements_ctr;
        current = DListNext(current);
    }
    
    return (elements_ctr);
}

/*****************/
dlist_iter_t DListBegin(d_list_t *d_list)
{
    return ((dlist_iter_t)d_list->head.next);
}

/*****************/
dlist_iter_t DListEnd(d_list_t *d_list)
{
    return ((dlist_iter_t)(&d_list->tail));
}

/*****************/
int DListIsSameIter(dlist_iter_t iter1, dlist_iter_t iter2)
{
    return (iter1 == iter2);
}

/*****************/
dlist_iter_t DListNext(dlist_iter_t iter)
{
    assert(iter != NULL);

    return ((dlist_iter_t)(((d_node_t *)iter)->next)); 
}

/*****************/
dlist_iter_t DListPrev(dlist_iter_t iter)
{
    assert(iter != NULL);

    return ((dlist_iter_t)(((d_node_t *)iter)->prev)); 
}

/*****************/
void *DListGetData(dlist_iter_t iter)
{
    assert(iter != NULL);

    return (((d_node_t *)iter)->data);  
}

/*****************/
dlist_iter_t DListInsert(d_list_t *d_list, void *data, dlist_iter_t iter)
{
    d_node_t *after_new = (d_node_t *)iter;
    d_node_t *before_new = (d_node_t *)DListPrev(iter);
    
    d_node_t *new = CreateNode(data); 
    if (NULL == new)
    {
        return (DListEnd(d_list));
    }
    
    new->next = after_new;
    new->prev = before_new;
    before_new->next = new;
    after_new->prev = new;
    
    return ((dlist_iter_t)new);
}

/*****************/
dlist_iter_t DListPushFront(d_list_t *d_list, void *data)
{
    return (DListInsert(d_list, data, DListBegin(d_list)));
}

/*****************/
dlist_iter_t DListPushBack(d_list_t *d_list, void *data)
{
    return (DListInsert(d_list, data, DListEnd(d_list)));
}

/*****************/
dlist_iter_t DListErase(dlist_iter_t iter)
{
    dlist_iter_t next = DListNext(iter);

    ((d_node_t *)DListPrev(iter))->next = (d_node_t *)DListNext(iter);
    ((d_node_t *)DListNext(iter))->prev = (d_node_t *)DListPrev(iter);
    free(iter);
    iter = NULL;  
    
    return (next);  
}

/*****************/
void DListPopFront(d_list_t *d_list)
{
    DListErase(DListBegin(d_list));    
}

/*****************/
void DListPopBack(d_list_t *d_list)
{
    DListErase(DListPrev(DListEnd(d_list)));    
}

/*****************/
int DListForEach(dlist_iter_t from, dlist_iter_t to,
                 dlist_action_func_t action_func, void *param)
{
    int result = 0;
    dlist_iter_t current = from;
    
    while ((!DListIsSameIter(current, to)) && (0 == result))
    {
        result = action_func(DListGetData(current), param);
        current = DListNext(current);
    }
    
    return (result);
}         

/*****************/
dlist_iter_t DListFind(dlist_iter_t from, dlist_iter_t to,
                       dlist_is_equal_func_t is_equal_func,
                       const void *data, void *param)
{
    dlist_iter_t current = from;
    int is_equal = 0;
    
    while ((!DListIsSameIter(current, to)) && (!is_equal))
    {
        is_equal = (is_equal_func(DListGetData(current), data, param));
        
        current = DListNext(current);
    }

    if (is_equal)
    {
        current = DListPrev(current);
    }
    
    return (current); 
}

/*****************/
void DListSpliceBefore(dlist_iter_t dest_insert, dlist_iter_t src_from, dlist_iter_t src_to)
{
    d_node_t *original_dest_insert_prev = (((d_node_t *)dest_insert)->prev);
    d_node_t *original_src_from_prev = (((d_node_t *)src_from)->prev);
    
    ((d_node_t *)DListPrev(dest_insert))->next = (d_node_t *)src_from;
    ((d_node_t *)dest_insert)->prev = (d_node_t *)DListPrev(src_to);
    
    ((d_node_t *)src_from)->prev = original_dest_insert_prev;
    ((d_node_t *)DListPrev(src_to))->next = (d_node_t *)dest_insert;
    
    ((d_node_t *)original_src_from_prev)->next = (d_node_t *)src_to;
    ((d_node_t *)src_to)->prev = original_src_from_prev;
}

/*****************/
static d_node_t *CreateNode(void *data)
{
    d_node_t *new_node = malloc(sizeof(d_node_t));
    if (NULL == new_node)
    {
        return (NULL);
    }
    
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;

    return (new_node);
}
