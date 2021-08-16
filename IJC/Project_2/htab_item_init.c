// htab_item_init.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab_private.h"
#include "htab.h"

struct htab_item *htab_item_init(htab_key_t key)
{
    struct htab_item *item = (struct htab_item*)malloc(sizeof(struct htab_item));
    if(item == NULL) { return NULL; }

    item->pair.key = malloc(strlen(key) + 1);
    if (item->pair.key == NULL) { free(item); return NULL; }

    strcpy((char*)item->pair.key, key);
    item->next = NULL;
    item->pair.value = 0;

    return item;
}
