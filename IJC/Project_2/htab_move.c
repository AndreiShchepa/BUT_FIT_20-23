// htab_move.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

htab_t *htab_move(size_t n, htab_t *from)
{
    if (from == NULL || n <= 0) { return NULL; }

    htab_t *new_tab = htab_init(n);
    if(new_tab == NULL) { return NULL; }

    htab_pair_t* pair;
    struct htab_item* tmp_item;
    size_t pos = 0;

    for (size_t i = 0; pos != from->size; i++)
	{
        tmp_item = from->ptr[i];
		while(tmp_item != NULL)
		{
            pair = htab_lookup_add(new_tab, tmp_item->pair.key);
            if (pair == NULL) { return NULL; }

            pair->value = tmp_item->pair.value;
			tmp_item = tmp_item->next;
            pos++;
		}
	}
    htab_clear(from);

    return new_tab;
}
