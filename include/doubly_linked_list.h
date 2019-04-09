#ifndef __DLIST_H__
#define __DLIST_H__

#include <stddef.h> /* size_t */

typedef struct dlist_info *dlist_iter_t;
typedef struct d_list d_list_t;
typedef int (*dlist_is_equal_func_t)(const void *internal_data,
									 const void *external_data,
									 void *param);
typedef int (*dlist_action_func_t)(void *data, void *param);
/* 
Returnes 0 on success, non-zero on failure.
*/


/*******************************************************************/

d_list_t *DListCreate(void);

void DListDestroy(d_list_t *d_list);

int DListIsEmpty(const d_list_t *d_list);

size_t DListCount(const d_list_t *d_list);

dlist_iter_t DListBegin(d_list_t *d_list);

dlist_iter_t DListEnd(d_list_t *d_list);

dlist_iter_t DListNext(dlist_iter_t iter);

dlist_iter_t DListPrev(dlist_iter_t iter);

dlist_iter_t DListInsert(d_list_t *d_list, void *data, dlist_iter_t iter);
/*
The new data is inserted before iter.
Returnes the new iterator on success, end iterator on failure.
*/

dlist_iter_t DListErase(dlist_iter_t iter);
/*
Returnes the iterator to the next element.
*/

dlist_iter_t DListPushFront(d_list_t *d_list, void *data);
/*
Inserts the new data to the beginning of the list.
Returnes the new iterator on success, end iterator on failure.
*/

dlist_iter_t DListPushBack(d_list_t *d_list, void *data);
/*
Inserts the new data to the end of the list.
Returnes the new iterator on success, end iterator on failure.
*/

void DListPopFront(d_list_t *d_list);
/*
Erases the first element in list.
The list mustn't be empty, otherwise - undefined behavior.
*/

void DListPopBack(d_list_t *d_list);
/*
Erases the last element in list.
The list mustn't be empty, otherwise - undefined behavior.
*/

int DListIsSameIter(dlist_iter_t iter1, dlist_iter_t iter2);
 
void *DListGetData(dlist_iter_t iter);

int DListForEach(dlist_iter_t from, dlist_iter_t to,
				 dlist_action_func_t action, void *param);  	 
/*  	 
"action" is invoked with each element in the range of "from" to "to"
(by order, not including "to"), until a non-zero value is returned.
Returnes the first non-zero value returned, or 0 if all action funcs
returned 0.
*/

dlist_iter_t DListFind(dlist_iter_t from, dlist_iter_t to,
					   dlist_is_equal_func_t is_equal_func,
					   const void *data, void *param);
/*
Searches for the data in the given range, returnes iterator to
the found data, or the "to" iterator if wasn't found.      	 
*/

void DListSpliceBefore(dlist_iter_t where, dlist_iter_t from, dlist_iter_t to);
/*       	 
Inserts a slice of a list (the range of "from" to "to") to another list
(just before "where").
*/


#endif /*__DLIST_H__*/

