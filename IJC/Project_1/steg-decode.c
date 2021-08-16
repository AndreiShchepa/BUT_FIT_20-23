//  steg-decode.c
//  Řešení IJC-DU1, příklad b), 20.3.2021
//  Autor: Andrei Shchapaniak, FIT
//  Přeloženo: gcc 10.2.0

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ppm.h"
#include "error.h"
#include "eratosthenes.h"

#define START_SIZE 1

#define ERROR_ARGUMENTS "%s: %s: You entered %d argument(s), but you should enter 2!\n"
#define ERROR_END_OF_STRING "%s: %s: String in the image doesnt contain \'\\0\' at tne end!\n"
#define ERROR_ALLOC "%s: %s: Cannot allocate memory for message!\n"

/*
 * @brief 				Print on stdout decoded message from name_file.ppm
 *
 * @param	argc		number of arguments
 * @param	argv[]		array of arguments
 *
 * @return				If success, 0, otherwise 1
 */
int main(int argc, const char *argv[])
{
	if(argc != 2)
	{
		error_exit(ERROR_ARGUMENTS, __FILE__, __func__, argc);
	}

	struct ppm *image = ppm_read(argv[1]);

	if(image == NULL) { return 1; }

	bitset_index_t image_size = image->xsize * image->ysize * 3;
	bitset_alloc(prime_array, image_size);
	Eratosthenes(prime_array);

	char *tmp = NULL;
	char *msg = (char*) malloc(START_SIZE);

	if(msg == NULL)
	{
		bitset_free(prime_array);
		ppm_free(image);
		error_exit(ERROR_ALLOC, __FILE__, __func__);
	}

	char shift = 0;
	unsigned idx = 0;
	char errflag = 1;
	msg[idx] = 0;

	for(bitset_index_t i = 23; i < image_size ; i += 2)
	{
		if(bitset_getbit(prime_array, i) == 0)
		{
			msg[idx] |= (image->data[i] & 1UL) << shift;
			shift++;
			if(shift == CHAR_BIT)
			{
				if(msg[idx] == '\0')
				{
					errflag = 0;
					break;
				}
				else
				{
					idx++;
					tmp = (char*) realloc(msg, (idx + START_SIZE));
					if(tmp == NULL)
					{
						bitset_free(prime_array);
						ppm_free(image);
						error_exit(ERROR_ALLOC, __FILE__, __func__);
					}
					msg = tmp;
				}
				msg[idx] = 0;
				shift = 0;
			}
		}
	}

	bitset_free(prime_array);
	ppm_free(image);

	if(errflag == 1)
	{
		free(msg);
		error_exit(ERROR_END_OF_STRING, __FILE__, __func__);
	}
	else
	{
		printf("%s\n", msg);
		free(msg);
	}

	return 0;
}
