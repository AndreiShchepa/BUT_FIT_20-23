//  steg-encode.c
//  Řešení IJC-DU1, příklad (volitelne), 20.3.2021
//  Autor: Andrei Shchapaniak, FIT
//  Přeloženo: gcc 10.2.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include "error.h"
#include "eratosthenes.h"

#define ERROR_ARGUMENTS "%s:%s: You entered %d argument(s), but you should enter 3!\n"
#define ERROR_READ_FILE "%s:%s: There was an error in the function \"ppm_read()\"!\n"

/*
 * @brief               Encode message to name_file.ppm
 *
 * @param	argc        number of arguments
 * @param	argv[]	    array of arguments
 *
 * @return	            If success, 0, otherwise 1
 */
int main(int argc, const char *argv[])
{
    if(argc != 3)
	{
		error_exit(ERROR_ARGUMENTS, __func__, __FILE__, argc);
	}

	struct ppm *image = ppm_read(argv[1]);
	if(image == NULL) { return 1; }

	bitset_index_t image_size = image->xsize * image->ysize * 3;
	bitset_alloc(prime_array, image_size);
	Eratosthenes(prime_array);

	char symbol = argv[2][0];
	char number_of_bits = 0;
    int size_word = strlen(argv[2]);
    int number_of_symbols = 0;
    int bit_from_string;
    int bit_from_image;

	for(bitset_index_t i = 23; i < image_size ; i += 2)
	{
		if(bitset_getbit( prime_array, i ) == 0)
		{
            bit_from_string = symbol & 1UL;
            bit_from_image = (unsigned)(image->data[i]) & 1UL;

            if (bit_from_image == 0 || (bit_from_image == 1 && bit_from_string == 1))
            {
                image->data[i] |= bit_from_string;
            }
            else
            {
                image->data[i] = ~(1UL | (~(image->data[i])));
            }

            symbol >>= 1;
			number_of_bits++;
			if(number_of_bits == CHAR_BIT)
			{
				if(number_of_symbols == size_word - 1)      { symbol = '\0'; }
                else if (number_of_symbols == size_word)    { break; }
                else                { symbol = argv[2][number_of_symbols+1]; }

                number_of_symbols++;
                number_of_bits = 0;
			}
		}
	}

    int err_code = 1;
    if (ppm_write(image, argv[1]) == 0) { err_code = 0; }

    bitset_free(prime_array);
	ppm_free(image);

	return err_code;
}
