/**
** io.h
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include <stdio.h>

/* Get one word from file, returns length of word or EOF */
int get_word(char *s, int max, FILE *f);

#endif // IO_H_INCLUDED
