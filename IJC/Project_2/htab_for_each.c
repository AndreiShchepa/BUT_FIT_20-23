// htab_for_each.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data))
{
	if (t == NULL)
		return;
		
	for(size_t i = 0; i != t->arr_size; i++)
	{
		for (struct htab_item* item = t->ptr[i]; item != NULL; item = item->next)
		{
			f(&item->pair);
		}
	}
}
