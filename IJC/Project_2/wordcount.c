// wordcount.c
// Řešení IJC-DU2, příklad 2), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "io.h"
#include "htab.h"
#include "htab_private.h"

#define WORD_LIMIT 128
#define ALLOC_ERROR "Allocation memory has been with error!\n"
#define WRONG_WORD_LENGTH "Word is longer than 126 symbols!\n"
#define ERROR_SET_ITEM_TO_MAP "Some error occurred while inserting an item into the map!\n"
#define ERROR_ALLOC_NEW_MAP "Some error was occured while creating new hash table!\n"

// I have chosen this size of the table as the average value of common words in english books.
// I have taken the information from this site http://www.tylervigen.com/literature-statistics.
// I also had to take the prime number, because it is better and faster when dividing
#define HTAB_SIZE 45503

// #define TEST

// printf item of unordered map
void print_items(htab_pair_t* data)
{
    printf("%s\t%d\n", data->key, data->value);
}

int main()
{
    FILE *fp = stdin;

    char* word = malloc(WORD_LIMIT * sizeof(char));
    if(word == NULL)
    {
        fprintf(stderr, "%s", ALLOC_ERROR);
        return 1;
    }

    htab_t *unordered_map = htab_init(HTAB_SIZE);
    if(unordered_map == NULL)
    {
        free(word);
        fprintf(stderr, "%s", ALLOC_ERROR);
        return 1;
    }

    bool warning = false;
    int length;
    htab_pair_t *el_of_map;

    while((length = read_word(word, WORD_LIMIT, fp)) != EOF)
    {
        if(length >= WORD_LIMIT - 1 && warning == false)
        {
            fprintf(stderr, "%s", WRONG_WORD_LENGTH);
            warning = true;
        }

        el_of_map = htab_lookup_add(unordered_map, word);

        if(el_of_map == NULL)
        {
            fprintf(stderr, "%s", ERROR_SET_ITEM_TO_MAP);
            htab_free(unordered_map);
            free(word);
            return 1;
        }

        // increment number of the same words
	    (el_of_map->value)++;
    }

    htab_for_each(unordered_map, &print_items);

    #ifdef TEST
        htab_t *new_map = htab_move(10000, unordered_map);
        printf("\n###############TEST_MOVE###############\n\n");

        if (new_map != NULL) { htab_for_each(new_map, &print_items); }
        else                 { fprintf(stderr, "%s", ERROR_ALLOC_NEW_MAP); }

        htab_free(new_map);
    #endif

    htab_free(unordered_map);
    free(word);

    return 0;
}
