// io.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "io.h"

int read_word(char *s, int max, FILE *f)
{
    int c;
    int i;
    int last_i;

    for(i = 0; (c = fgetc(f)) != EOF; i++)
    {
        if (isspace(c) != 0) { break; }

        if (i < max - 1)
        {
            // While length < max_length - 1, write word to string.
            // Then we dont write word to string,
            // but length of word will be incremented each letter by one.
            s[i] = c;
            last_i = i + 1;
        }
    }

    // If it isnt EOF, end string by symbol '\0'
    if(c == EOF) { return EOF; }
    else         { s[last_i] = '\0'; }

    return i;
}
