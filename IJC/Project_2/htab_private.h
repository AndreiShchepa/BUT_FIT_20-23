// private_htab.h
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#ifndef __HTAB_PRIVATE_H__
#define __HTAB_PRIVATE_H__

#include "htab.h"
#include <stdlib.h> // malloc
#include <stdint.h> // uint_32

// Struct of the table:
struct htab
{
    size_t           size;      // number of items
    size_t           arr_size;  // size of the table
    struct htab_item *ptr[];    // table
};

// Struct of the item
struct htab_item
{
    htab_pair_t       pair;     // item (key, value)
    struct htab_item *next;     // pointer to the next item with same index
};

/*
 * @brief           Inicialized item and allocated its memory
 *
 * @param   key     String_key to find its length
 *
 * @return          Pointer to allocated item
 */
struct htab_item *htab_item_init(htab_key_t key);

#endif /* __HTAB_PRIVATE_H__ */
