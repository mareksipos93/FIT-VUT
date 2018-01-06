/**
** error.h
** Solution: IJC-DU1, task b), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#include <stdarg.h>

#ifndef ERROR_H
#define ERROR_H

void warning_msg(const char *fmt, ...);
void fatal_error(const char *fmt, ...);

#endif // ERROR_H
