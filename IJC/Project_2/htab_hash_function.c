// htab_hash_function.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "htab.h"
#include "htab_private.h"

size_t htab_hash_function(htab_key_t str)
{
    uint32_t h = 0;     // must be 32 bits
    const unsigned char *p;
    for(p = (const unsigned char*)str; *p!='\0'; p++)
    {
        h = 65599*h + *p;
    }
    return h;
}
