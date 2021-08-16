// tail.c
// Řešení IJC-DU2, příklad 1), 19.04.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

typedef long long ll;
typedef char* string;

#define COUNT_SYMBOLS 511 // 511 default
#define DEFAULT_LINES 10
#define WRONG_NUM_ARGC  "%s: %s: You have entered %d argument(s). Write ./tail -h for help!\n"
#define WRONG_NANE_FILE "%s: %s: File with name \"%s\" is wrong!\n"
#define WRONG_ARG_LINE  "%s: %s: You have entered wrong argument for number of writed lines!\n"
#define ERROR_ALLOC_MEM "%s: %s: Allocation of memory has be wrong!\n"
#define ERROR_MUCH_SYM_ON_LINE "On line %d in the file is more than 511 symbols!\n\n"

/*
 * @brief                 Frees memory
 *
 * @param	lines         Dynamic arr
 * @param	num_lines     Number of allocated lines in arr
 */
void free_str(string* lines, ll num_lines)
{
    for (ll i = 0; i < num_lines; i++) { free(lines[i]); }

    free(lines);
}

/*
 * @brief           Prints error on stderr
 *
 * @param	fmt		String for print
 * @param	...		Variadic list of values
 */
void error_exit(const char *fmt, ...)
{
    va_list var_1;
    va_start(var_1, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, var_1);
    va_end(var_1);
    exit(1);
}

/*
 * @brief       Creates dynamic array of size 1 line
 *
 * @return      On success, poiner to created array, otherwise NULL
 */
string *line_ctor()
{
	string *lines = (string*)malloc(sizeof(string));

    if (lines == NULL) { return NULL; }
    lines[0] = (string)malloc(COUNT_SYMBOLS + 2);  // 511 def + '\n' + '\0'

    if (lines[0] == NULL)
    {
        free(lines);
        return NULL;
    }
    return lines;
}

/*
 * @brief           Realocates dynamic array of (before_size + 1)
 *
 * @param	lines   Pointer to array
 * @param	size	Size fo reallocating
 *
 * @return          On success, poiner to reallocated array, otherwise NULL
 */
string* realloc_arr(string* lines, ll size)
{
    string *tmp = (string*)realloc(lines, sizeof(string) * (size + 1));
    if(tmp == NULL)
    {
        free_str(lines, size);
        return NULL;
    }

    lines = tmp;

    lines[size] = (string)malloc(COUNT_SYMBOLS + 2);   // 511 def + '\n' + '\0'
    if (lines[size] == NULL)
    {
        free_str(lines, size);
        return NULL;
    }
    return lines;
}

/*
 * @brief           Reads lines from file to string array
 *
 * @param	f       Pointer to file
 * @param	r_l	    Pointer to number of read lines from file
 *
 * @return          On success, poiner to array, otherwise NULL
 */
string* write_file_to_struct(FILE *f, ll *r_l)
{
    string *lines = line_ctor();
    if (lines == NULL) { return NULL; }

    bool warning = false;
    int c;
    int pos = 0;

    while ((c = fgetc(f)) != EOF)
    {
        // while number of symbols < limit of symbols, fill str_arr
        if(pos < COUNT_SYMBOLS)
        {
            lines[(*r_l)][pos++] = c;

            // if in str_arr is limit of symbols, set '\n' to the end and skip other symbols on the line
            if(pos == COUNT_SYMBOLS && c != 10) { lines[(*r_l)][pos++] = '\n'; }
        }
        else if (warning == false && c != '\n')
        {   // if its the first overflow of the string, prints warning_msg
            fprintf(stderr, "%s", ERROR_MUCH_SYM_ON_LINE);
            warning = true;
        }

        // if is end of program, set '\0' and realloc array for new string
        if (c == 10)
        {
            lines[(*r_l)++][pos] = '\0';
            lines = realloc_arr(lines, *r_l);
            if (lines == NULL) { return NULL; }
            pos = 0;
        }
    }

    return lines;
}

/*
 * @brief           Writes lines from arr to stdout
 *
 * @param	lines   Pointer to array with lines
 * @param	r_l	    Number of read lines from file
 * @param   w_l     Number of lines, which must be writed
 */
void write_str(string* lines, ll r_l, ll w_l)
{
    if (w_l <= 0) { return; }

    w_l = r_l < w_l ? 0 : r_l - w_l;

    while(w_l < r_l) { printf("%s", lines[w_l++]); }
}

int main(int argc, char const *argv[])
{
    FILE *fp = stdin;
    string *lines;
    ll num_of_read_lines = 0;
    ll num_of_write_lines = DEFAULT_LINES;

    if(argc < 1 || argc > 4)
    {
        // if was entered wrong number of arguments
        error_exit(WRONG_NUM_ARGC, __FILE__, __func__, argc);
    }
    else
    {
        // case, if user has entered file
        if (argc == 2 || argc == 4)
        {
            fp = fopen(argv[argc - 1], "r");
            if(fp == NULL)
            {
                error_exit(WRONG_NANE_FILE, __FILE__, __func__, argv[1]);
            }
        }
    }

    // call function for filling array of strings
    lines = write_file_to_struct(fp, &num_of_read_lines);
    if(lines == NULL)
    {
        fclose(fp);
        error_exit(ERROR_ALLOC_MEM, __FILE__, __func__);
    }

    // if user has entered number of lines fot printing
    if (argc == 3 || argc == 4)
    {
        if(strcmp(argv[1], "-n") != 0 || atoi(argv[2]) < 1)
        {
            error_exit(WRONG_ARG_LINE, __FILE__, __func__);
        }

        /*
         * if user has entered +n, n > 0, program prints last $(all_lines - n + 1)     lines from file/input
         * if user has entered  n, n > 0, program prints last $(0 + n)                 lines from file/input
         * if user has entered -n,        program ends with error, because number of lines musnt be negative
        */
        if(argv[2][0] == '+') { num_of_write_lines = num_of_read_lines + 1 - atoi(argv[2]); }
        else                  { num_of_write_lines = atoi(argv[2]); }
    }

    write_str(lines, num_of_read_lines, num_of_write_lines);

    free_str(lines, num_of_read_lines + 1);
    fclose(fp);

    return 0;
}
