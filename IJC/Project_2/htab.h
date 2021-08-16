// htab.h
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

// Licence: Public domain

// The following lines cant be changed
#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <string.h>     // size_t
#include <stdbool.h>    // bool

// Tabel:
struct htab;    // incomplete structure declaration - user doesnt see content
typedef struct htab htab_t;     // typedef as specified

// Types:
typedef const char* htab_key_t;        // type of key
typedef int htab_value_t;              // type of value

// A pair of data in a table:
typedef struct htab_pair
{
    htab_key_t    key;          // key
    htab_value_t  value;        // associated value
} htab_pair_t;                  // typedef as specified

// Hash functions (same for all tables in the program)
// If you define the same function in the program, yours will be used.
/*
 * @brief           Sdbm algorithm for hash table
 *
 * @param   str     String_key to find its hash
 *
 * @return          Hash of the string
 */
size_t htab_hash_function(htab_key_t str);


// Function for working with the table:
/*
 * @brief           Allocates hash table and set the structure values
 *
 * @param     n     Size of hash table
 *
 * @return          Initialized and allocated hash table
 */
htab_t *htab_init(size_t n);

/*
 * @brief           Initialize and allocate new table and move data fron old to new.
 *
 * @param     n     Size of the new hash table
 * @param   from    Pointer to table with data
 *
 * @return          Initialized and allocated new hash table
 */
htab_t *htab_move(size_t n, htab_t *from);

/*
 * @param     t     Const pointer to hash table
 *
 * @return          Number of items in the table
 */
size_t htab_size(const htab_t *t);

/*
 * @param     t     Const pointer to hash table
 *
 * @return          Size of the table
 */
size_t htab_bucket_count(const htab_t *t);

/*
 * @brief           In the table finds the item that matches the key_string
 *
 * @param     t     Pointer to hash table
 * @param    key    String for find item and its index
 *
 * @return          On success, poiner to item, otherwise NULL
 */
htab_pair_t *htab_find(htab_t *t, htab_key_t key);

/*
 * @brief           In the table finds the item that matches the key_string or create this item
 *
 * @param     t     Pointer to hash table
 * @param    key    String for find item and its index
 *
 * @return          Poiner to the found or created item
 */
htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key);

/*
 * @brief           In the table finds the item that matches the key_string and deletes it
 *
 * @param     t     Pointer to hash table
 * @param    key    String for find item and its index
 *
 * @return          On success true, otherwise false
 */
bool htab_erase(htab_t *t, htab_key_t key);

/*
 * @brief           Goes through all items and calls the function f on them
 *
 * @param     t     Const pointer to hash table
 * @param     f     Pointer to void function with 1 param
 */
void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data));

/*
 * @brief           Clear all items in the hash table
 *
 * @param     t     Pointer to hash table
 */
void htab_clear(htab_t *t);

/*
 * @brief           Table destructor. Call htab_clear() and free table
 * @see             htab_for_each()
 *
 * @param     t     Pointer to hash table
 */
void htab_free(htab_t *t);

#endif /* __HTAB_H__ */
