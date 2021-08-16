//  primes.c
//  Řešení IJC-DU1, příklad a), 20.3.2021
//  Autor: Andrei Shchapaniak, FIT
//  Přeloženo: gcc 10.2.0

#include <stdio.h>
#include <stdlib.h>
#include "eratosthenes.h"
#include "bitset.h"
#include <time.h>

#define SIZE 200000000L
#define MAX_PRIMES 10

/*
 * @brief 	Print last 10 prime numbers from range 0 to SIZE
 *
 * @return	0 (SUCCESS)
 */
int main()
{
	clock_t start = clock();
	static bitset_create(bit_arr, SIZE);
	int counter_primes = 0;
	unsigned long index = SIZE - 1;
	unsigned long primes[MAX_PRIMES] = {0};

	Eratosthenes(bit_arr);

	while(counter_primes < MAX_PRIMES)
	{
		if(bitset_getbit(bit_arr, index) == 0)
		{
   			primes[counter_primes] = index;
   			counter_primes++;
   	 	}
		index--;
	}

	for(index = MAX_PRIMES; index > 0; index--)
	{
		printf("%lu\n", primes[index - 1]);
	}

	fprintf(stderr, "Time=%.3g\n", (double)(clock() - start)/CLOCKS_PER_SEC);
	return 0;
}
