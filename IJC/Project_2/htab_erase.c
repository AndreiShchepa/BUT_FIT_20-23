// htab_erase.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

bool htab_erase(htab_t *t, htab_key_t key)
{
    if (t == NULL) { return NULL; }

    size_t index = htab_hash_function(key) % t->arr_size;
    struct htab_item **p = &(t->ptr[index]);
    struct htab_item *item = t->ptr[index];

    if(item != NULL)
    {
        size_t len = strlen(item->pair.key);
        while(p != NULL)
        {
            if(len == strlen(key) && strncmp(key, item->pair.key, len) == 0)
            {
                *p = item->next;
                free((char*)item->pair.key);
                free(item);
                (t->size)--;
                return true;
            }
            p = &(*p)->next;
        }
    }

    return false;
}
