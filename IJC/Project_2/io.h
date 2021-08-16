// io.h
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#ifndef __IO_H__
#define __IO_H__

#include <ctype.h>
#include <stdio.h>

/*
 * @brief               Read one word from the file
 *
 * @param       s       String for word from the file
 * @param       max     Max length of string
 * @param       f       Pointer to file
 *
 * @return              Length of the word
 */
int read_word(char *s, int max, FILE *f);

#endif /* __IO_H__ */
