#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>

#include "hash_table.h"
#include "doubly_linked_list.h"

/* working with unsorted lists enables faster access to the last
inserted / last accessed elements (in-list caching).
that means the elements in hash table are grouped but not sorted */

#define ENTRIES hash->arr_entries

struct hash_table
{
	d_list_t** arr_entries;
	size_t num_of_entries;
	hash_map_func_t map_func;
	hash_is_equal_func_t is_equal_func;
	void *param;
};

size_t Max(size_t a, size_t b);


/*******************************************************************/

hash_table_t *HashTableCreate(size_t num_of_entries,
							  hash_map_func_t map_func,
							  hash_is_equal_func_t is_equal_func,
							  void *param)
{
	hash_table_t *hash = NULL;
	d_list_t **arr_entries = NULL;
	size_t i = 0, j = 0;

	assert(num_of_entries > 0);
	assert(map_func != NULL);
	assert(is_equal_func != NULL);
	
	hash = malloc(sizeof(hash_table_t));
	if (NULL == hash)
	{
		return (NULL);
	}

	arr_entries = malloc(sizeof(d_list_t *) * num_of_entries);
	if (NULL == arr_entries)
	{
		free(hash);
		hash = NULL;
		return (NULL);
	}

	for (i = 0; i < num_of_entries; ++i)
	{
		arr_entries[i] = DListCreate();
		/* if malloc failed - free everything that has been allocated so far*/
		if (NULL == arr_entries[i])
		{
			for (j = 0; j < i; ++j)
			{
				DListDestroy(arr_entries[i]);
				arr_entries[i] = NULL;
			}

			free(arr_entries);
			arr_entries = NULL;
			free(hash);
			hash = NULL;

			return (NULL);
		}
	}

	hash->arr_entries = arr_entries;
	hash->num_of_entries = num_of_entries;
	hash->map_func = map_func;
	hash->is_equal_func = (dlist_is_equal_func_t)is_equal_func;
	hash->param = param;

	return (hash);
}

/*************/
void HashTableDestroy(hash_table_t *hash)
{
	size_t i = 0;
	size_t num_of_entries = 0;

	assert(hash != NULL);

	num_of_entries = hash->num_of_entries;

	for (i = 0; i < num_of_entries; ++i)
	{
		DListDestroy(ENTRIES[i]);
		ENTRIES[i] = NULL;
	}

	free(ENTRIES);
	ENTRIES = NULL;

	free(hash);
	hash = NULL;
}

/*************/
int HashTableInsert(hash_table_t *hash, void *data)
{
	size_t map_result = 0;
	dlist_iter_t new_iter = NULL;
	dlist_iter_t end_iter = NULL;

	assert(hash != NULL);
	assert(data != NULL);

	map_result = hash->map_func(data);

	end_iter = DListEnd(ENTRIES[map_result]);
	new_iter = DListPushFront(ENTRIES[map_result], data);

	/* if inserted iter is different from end iter - insert succeeded */
	return (DListIsSameIter(new_iter, end_iter));
}

/*************/
size_t HashTableCount(const hash_table_t *hash)
{
	size_t counter = 0;
	size_t i = 0;
	size_t num_of_entries = 0;

	assert(hash != NULL);

	num_of_entries = hash->num_of_entries;

	for (i = 0; i < num_of_entries; ++i)
	{
		counter += DListCount(ENTRIES[i]);
	}

	return (counter);
}

/*************/
int HashTableIsEmpty (const hash_table_t *hash)
{
	int is_empty = 1;
	size_t i = 0;
	size_t num_of_entries = 0;

	assert(hash != NULL);

	num_of_entries = hash->num_of_entries;

	for (i = 0; (i < num_of_entries) && (is_empty); ++i)
	{
		is_empty = DListIsEmpty(ENTRIES[i]);
	}

	return (is_empty);
}

/*************/
void *HashTableFind(const hash_table_t *hash, void *data)
{
	size_t map_result = 0;
	dlist_iter_t found_iter = NULL;
	dlist_iter_t end_iter = NULL;
	dlist_iter_t begin_iter = NULL;
	void *found_data = NULL;
	d_list_t *list = NULL;

	assert(hash != NULL);
	assert(data != NULL);

	map_result = hash->map_func(data);
	list = ENTRIES[map_result];

	end_iter = DListEnd(list);
	begin_iter = DListBegin(list);

	found_iter = DListFind(begin_iter, end_iter,
						   hash->is_equal_func, data, hash->param);

	/* if data wasn't found, NULL is returned (found_data is set to NULL) */
	if (!DListIsSameIter(found_iter, end_iter))
	{
		found_data = DListGetData(found_iter);
		/* caching */
		DListErase(found_iter);
		DListPushFront(list, found_data);
	}

	return (found_data);
}

/*************/
int HashTableForEach(hash_table_t *hash,
					 hash_action_func_t action_func, void *param)
{
	size_t i = 0;
	int ret_val = 0;
	dlist_iter_t end_iter = NULL;
	dlist_iter_t begin_iter = NULL;
	d_list_t *list = NULL;
	size_t num_of_entries = 0;

	assert(hash != NULL);
	assert(action_func != NULL);

	num_of_entries = hash->num_of_entries;

	for (i = 0; (i < num_of_entries) && (0 == ret_val); ++i)
	{
		list = ENTRIES[i];
		end_iter = DListEnd(list);
		begin_iter = DListBegin(list);
		ret_val = DListForEach(begin_iter, end_iter, action_func, param);
	}

	return (ret_val);
}                               

/*************/
void HashTableRemove (hash_table_t *hash, void *data)
{
	size_t map_result = 0;
	dlist_iter_t found_iter = NULL;
	dlist_iter_t end_iter = NULL;
	dlist_iter_t begin_iter = NULL;
	d_list_t *list = NULL;

	assert(hash != NULL);
	assert(data != NULL);

	map_result = hash->map_func(data);
	list = ENTRIES[map_result];

	end_iter = DListEnd(list);
	begin_iter = DListBegin(list);

	found_iter = DListFind(begin_iter, end_iter,
						   hash->is_equal_func, data, hash->param);

	/* DListErase is invoked only if data was found */
	if (!DListIsSameIter(found_iter, end_iter))
	{
		DListErase(found_iter);
	}
}

/*************/
hash_stats_t HashTableStatitics(const hash_table_t *hash)
{
	size_t total_size = 0;
	double average_size = 0;
	size_t empty_entries_ctr = 0;
	size_t total_sd = 0;
	double average_sd = 0;
	size_t cur_list_sd = 0;
	size_t cur_list_size = 0;
	size_t max_size = 0;
	size_t i = 0;
	hash_stats_t stats = {0};
	size_t num_of_entries = 0;

	assert(hash != NULL);

	num_of_entries = hash->num_of_entries;

	for (i = 0; i < num_of_entries; ++i)
	{
		cur_list_size = DListCount(ENTRIES[i]);
		
		total_size += cur_list_size;
		empty_entries_ctr += (0 == cur_list_size);
		max_size = Max(max_size, cur_list_size);
	}

	average_size = (double)total_size / (double)num_of_entries;

	for (i = 0; i < num_of_entries; ++i)
	{
		/* size of each list (calculated in previous traverse) may be saved
		in an allocated array to avoid recounting, but requirements don't
		allow a failure of this function */
		cur_list_size = DListCount(ENTRIES[i]);
		cur_list_sd = abs((ssize_t)cur_list_size - (ssize_t)average_size);
		total_sd += cur_list_sd;
	}

	average_sd = (double)total_sd / (double)num_of_entries;

	stats.max_length = max_size;
	stats.average_length = average_size;
	stats.sd = average_sd;
	stats.num_of_empty_cells = empty_entries_ctr;

	return (stats);
}

/*************/
size_t Max(size_t a, size_t b)
{
	return ((a > b) ? (a) : (b));
}

