// htab_clear.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

void htab_clear(htab_t *t)
{
    if (t == NULL)
        return;
        
    struct htab_item* item_for_free;
    struct htab_item* item;
    size_t pos = 0;

	for(size_t i = 0; pos != t->size; i++)
	{
        item = t->ptr[i];
        while (item != NULL)
        {
            item_for_free = item;
            item = item->next;
            free((char*)item_for_free->pair.key);
            free(item_for_free);
            pos++;
        }
	}
    t->size = 0;
}
