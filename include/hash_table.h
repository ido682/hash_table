#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <stddef.h> /* size_t */

typedef struct hash_table hash_table_t;
typedef struct hash_stats hash_stats_t;
typedef size_t (*hash_map_func_t)(const void *data);
typedef int (*hash_action_func_t)(void *data, void *param);
typedef int (*hash_is_equal_func_t)(const void *external_data,
                        			const void *internal_data,
									void *param);

struct hash_stats
{
    size_t max_length;
    double average_length;
    double sd;
    size_t num_of_empty_cells;
};


/*******************************************************************/

hash_table_t *HashTableCreate(size_t num_of_entries, hash_map_func_t map_func,
							  hash_is_equal_func_t is_equal_func, void *param);

void HashTableDestroy(hash_table_t *hash_table);

int HashTableInsert(hash_table_t *hash_table, void *data_to_insert);
/*
Returns 0 on success, 1 on failure.
*/

size_t HashTableCount(const hash_table_t *hash_table);

int HashTableIsEmpty(const hash_table_t *hash_table);

void *HashTableFind(const hash_table_t *hash_table, void *data_to_match);
/*                
Returns a reference to the data, NULL if not found.
*/

int HashTableForEach(hash_table_t *hash_table,
					 hash_action_func_t action_func, void *param);   
/*
"action" is invoked for each element, until a non-zero value is returned.
the order of invoking is same as the order of entries (in-entry, order
starts with last element inserted/accessed).        
Returnes the first non-zero value returned, or 0 if all action funcs
returned 0. 
*/

void HashTableRemove(hash_table_t *hash_table, void *data_to_remove);
/*                   
If data wasn't found - nothing is committed.
*/

hash_stats_t HashTableStatitics(const hash_table_t *hash_table);
/*                
Provides statstics about the hash table enteries: maximum length,
average length, standard deviation and number of empty cells.
*/


#endif /*__HASH_TABLE_H__*/

