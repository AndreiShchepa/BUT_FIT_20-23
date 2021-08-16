// eratosthenes.c
// Řešení IJC-DU1, příklad a), 20.3.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include <stdio.h>
#include "bitset.h"
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>

#define FIRST_NON_PRIME_ODD_NUMBER 3

/*
 * @brief 				Set 0 on primary bits using sieve of Eratosthenes
 *
 * @param	arr_bit		This describes name of array
 */
void Eratosthenes(bitset_t arr_bit)
{
	unsigned long size = bitset_size(arr_bit);
	unsigned long sqrt_size = sqrt(size);
	unsigned long i;
	unsigned long j;

	// Memset fills the array with ones in all even indexes, because they are not 100 percent prime numbers.
	// Since the function is working with bytes, we will fill it with 0x55 (0b01010101).
	memset(arr_bit+1, 0x55, (SIZE_INDEX(size) - 1) * CHAR_BIT);
	bitset_setbit(arr_bit, 0, 1);
	bitset_setbit(arr_bit, 1, 1);
	bitset_setbit(arr_bit, 2, 0);

	// I use i+=2, because we neednt check even numbers
	// If we find prime number A, then n*A (n > 1) wont be prime numbers
    for(i = FIRST_NON_PRIME_ODD_NUMBER; i < sqrt_size; i+=2)
	{
		if(bitset_getbit(arr_bit, i) == 0)
		{
            for(j = i*i; j < size; j+=2*i)
			{
                bitset_setbit(arr_bit, j, 1);
            }
        }
    }
}
