// bitset.c
// Řešení IJC-DU1, příklad a), 20.3.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "bitset.h"

#ifdef USE_INLINE
extern inline bitset_index_t bitset_size(bitset_t jmeno_pole);

extern inline void bitset_free(bitset_t jmeno_pole);

extern inline void bitset_setbit(bitset_t jmeno_pole, const bitset_index_t index, const bitset_index_t vyraz);

extern inline bitset_index_t bitset_getbit(bitset_t jmeno_pole, const bitset_index_t index);
#endif /* USE_INLINE */
