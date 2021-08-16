// error.c
// Řešení IJC-DU1, příklad b), 20.3.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include "error.h"

/*
 * @brief           Print warning on stderr
 *
 * @param   fmt     String for print
 * @param   ...		Variadic list of values
 */
void warning_msg(const char *fmt, ...)
{
    va_list var_l;
    va_start(var_l, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, var_l);
    va_end(var_l);
}

/*
 * @brief           Print error on stderr
 *
 * @param	fmt		String for print
 * @param	...		Variadic list of values
 */
void error_exit(const char *fmt, ...)
{
    va_list var_l;
    va_start(var_l, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, var_l);
    va_end(var_l);
    exit(1);
}
