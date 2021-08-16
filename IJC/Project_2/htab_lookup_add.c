// htab_lookup_add.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key)
{
    if (t == NULL) { return NULL; }
    size_t index = htab_hash_function(key) % t->arr_size;
    struct htab_item *item = t->ptr[index];

    if(item == NULL)
    {
        item = htab_item_init(key);
        if(item == NULL) { return NULL; }

        (t->size)++;
        t->ptr[index] = item;
        return &item->pair;
    }

    size_t len = strlen(item->pair.key);
    while(1)
    {
        if(len == strlen(key) && strncmp(key, item->pair.key, len) == 0)
        {
            return &item->pair;
        }

        if(item->next == NULL) { break; }
        else                   { item = item->next; }

    }

    struct htab_item *tmp = htab_item_init(key);
    if(tmp == NULL) { return NULL; }

    item->next = tmp;
    (t->size)++;
    return &tmp->pair;
}
