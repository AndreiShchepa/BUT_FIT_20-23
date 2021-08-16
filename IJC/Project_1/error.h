// error.h
// Řešení IJC-DU1, příklad b), 20.3.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef ERROR_H
#define ERROR_H

void warning_msg(const char *fmt, ...);
void error_exit(const char *fmt, ...);

#endif //ERROR_H
