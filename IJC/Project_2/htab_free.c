// htab_free.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

void htab_free(htab_t *t)
{
    if (t == NULL)
        return;

    htab_clear(t);
    free(t);
}
