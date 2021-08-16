// bitset.h
// Řešení IJC-DU1, příklad a), 20.3.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include "error.h"

#ifndef BITSET_H
#define BITSET_H

typedef unsigned long* bitset_t;
typedef unsigned long bitset_index_t;

#define MAX_SIZE 200000000L
#define UL_BITS (sizeof(bitset_index_t) * CHAR_BIT)

/*
 * @brief 			shift for 1. It is used in functions get_bit and set_bit.
 *
 * @param 	index	This describes position of bit
 */
#define SHIFT(index) (1UL << (index)%UL_BITS)

/*
 * @brief 			It is used to find bit_index in array
 *
 * @param 	index	This describes position of bit
 */
#define BIT_INDEX(index) (1 + (index)/UL_BITS)

/*
 * @brief 				It is used to find size of unsigned long for array
 *
 * @param 	velikost	This describes size of bits
 */
#define SIZE_INDEX(velikost) ((velikost)/UL_BITS + 1 + (((velikost) % UL_BITS) ? 1 : 0))

/*
 * @brief 				It is used to check given arguments
 *
 * @param 	velikost	This describes position of bit
 * @param 	jmeno_pole	This describes name of array
 */
#define CONDITION_INDEX(jmeno_pole, index) ((index) >= bitset_size((jmeno_pole)))

/*
 * @brief				Initialize of bit_array
 *
 * @param	jmeno_pole	This describes name of created array
 * @param	velikost	This describes size of bits
 */
#define bitset_create(jmeno_pole, velikost)\
			bitset_index_t (jmeno_pole)[SIZE_INDEX((velikost))] = {(velikost), };\
			_Static_assert(((velikost) > 0), "Size of array must be greater than 0.\n")

/*
 * @brief				Dynamic initialize of bit_array
 *
 * @param	jmeno_pole	This describes name of created array
 * @param	velikost	This describes size of bits
 */
#define bitset_alloc(jmeno_pole, velikost)\
		assert((velikost) > 0 && (velikost) < MAX_SIZE);\
		bitset_t (jmeno_pole) = (bitset_t)calloc(SIZE_INDEX((velikost)), sizeof(bitset_index_t));\
		if((jmeno_pole) == NULL)\
		{\
			error_exit("bitset_alloc: Chyba alokace paměti\n");\
		}\
		(jmeno_pole)[0] = (velikost)

#ifdef USE_INLINE

/*
 * @brief				Get size of array in bits
 *
 * @param	jmeno_pole 	This describes name of the array.
 *
 * @returns 			Size of the array. It is on the first position of the array.
 */
inline bitset_index_t bitset_size(bitset_t jmeno_pole)
{
	return jmeno_pole[0];
}

/*
 * @brief				Clear allocated array
 *
 * @param	jmeno_pole 	This describes name of the array.
 *
 * returns 				Nothing
 */
inline void bitset_free(bitset_t jmeno_pole)
{
	free(jmeno_pole);
}

/*
 * @brief				Set vyraz at the index in the array
 *
 * @param 	jmeno_pole 	This describes name of the array.
 * @param 	index 		This describes on which position the function set vyraz.
 * @param 	vyraz 		This describes what expression (0 or 1) the function set to the array.
 *
 * returns 				Nothing
 */
inline void bitset_setbit(bitset_t jmeno_pole, const bitset_index_t index, const bitset_index_t vyraz)
{
	if(CONDITION_INDEX(jmeno_pole, index))
	{
		error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu",
					 (bitset_index_t)(index), (bitset_index_t)(bitset_size(jmeno_pole) - 1));
	}
	else
	{
		if(vyraz == 0)
		{
			jmeno_pole[BIT_INDEX(index)] &= ~(SHIFT(index));
		}
		else
		{
			jmeno_pole[BIT_INDEX(index)] |= (SHIFT(index));
		}
	}
}

/*
 * @brief				Get bit at the index from the array
 *
 * @param 	jmeno_pole 	This describes name of the array.
 *
 * returns 				Value from index in the array
 */
inline bitset_index_t bitset_getbit(bitset_t jmeno_pole, const bitset_index_t index)
{
	if(CONDITION_INDEX(jmeno_pole, index))
	{
		error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu",
					 (bitset_index_t)(index), (bitset_index_t)(bitset_size(jmeno_pole) - 1));
	}

	return jmeno_pole[BIT_INDEX(index)] & (SHIFT(index));
}

#else

// @see bitset_size()
#define bitset_size(jmeno_pole) (jmeno_pole)[0]

// @see bitset_free()
#define bitset_free(jmeno_pole) free((jmeno_pole))

// @see bitset_setbit()
#define bitset_setbit(jmeno_pole, index, vyraz)\
	(\
		(CONDITION_INDEX((jmeno_pole), (index)))\
		?\
			(error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu\n",\
						(bitset_index_t)(index), (bitset_index_t)(bitset_size((jmeno_pole)) - 1)), 1)\
		:\
			(((vyraz) == 0)\
			?\
				((jmeno_pole)[BIT_INDEX((index))] &= ~(SHIFT((index))))\
			:\
				((jmeno_pole)[BIT_INDEX((index))] |= (SHIFT((index))))\
			)\
	)

// @see bitset_getbit()
#define bitset_getbit(jmeno_pole, index)\
	(\
		(CONDITION_INDEX((jmeno_pole), (index)))\
		?\
			(error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n",\
						(bitset_index_t)(index), (bitset_index_t)(bitset_size((jmeno_pole)) - 1)), 1)\
		:\
			((jmeno_pole)[BIT_INDEX((index))] & (SHIFT((index))))\
	)

#endif /* USE_INLINE */
#endif /* BITSET_H */
