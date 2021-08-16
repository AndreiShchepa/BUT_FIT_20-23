// htab_init.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

htab_t *htab_init(size_t n)
{
    if (n <= 0)
        return NULL;

    // alocation with flexible array member
    htab_t *table = malloc(sizeof(htab_t) + n * sizeof(struct htab_item*));

    if(table == NULL) { return NULL; }

    for(size_t i = 0; i < n; i++)
    {
        table->ptr[i] = NULL;
    }

    table->size = 0;
    table->arr_size = n;

    return table;
}
