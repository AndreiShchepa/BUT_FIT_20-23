// htab_find.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

htab_pair_t *htab_find(htab_t *t, htab_key_t key)
{
    if (t == NULL) { return NULL; }

    size_t index = htab_hash_function(key) % t->arr_size;
    struct htab_item *item = t->ptr[index];

    if(item != NULL)
    {
        size_t len = strlen(item->pair.key);
        while(1)
        {
            if(len == strlen(key) && strncmp(key, item->pair.key, len) == 0)
                 return &item->pair;

            if(item->next == NULL) { break; }
            else                   { item = item->next; }
        }
    }
    return NULL;
}
