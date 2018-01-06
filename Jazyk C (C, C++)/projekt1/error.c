/**
** error.c
** Solution: IJC-DU1, task b), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"

// Error happened, but not serious, so only inform about it
void warning_msg(const char *fmt, ...) {
    va_list args; // List to hold arguments in
    va_start(args, fmt); // Fill with arguments
    fprintf(stderr, "CHYBA: "); // Pre-message
    vfprintf(stderr, fmt, args); // Post-message
    va_end(args); // Destroy list
}

// Same as warning_msg, but ends execution
void fatal_error(const char *fmt, ...) {
    va_list args; // List to hold arguments in
    va_start(args, fmt); // Fill with arguments
    fprintf(stderr, "CHYBA: "); // Pre-message
    vfprintf(stderr, fmt, args); // Post-message
    va_end(args); // Destroy list
    exit(1); // Ba-dum-tss
}
