// htab_bucket_count.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

size_t htab_bucket_count(const htab_t *t)
{    
    return t->arr_size;
}
