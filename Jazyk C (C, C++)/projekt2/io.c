/**
** io.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <ctype.h>

#include "io.h"

/* Get one word from file, returns length of word or EOF */
int get_word(char *s, int max, FILE *f) {

    // If stream is already at end or parameter 'max' is wrong
    if (feof(f) || max <= 0) {
        return EOF;
    }

    // User wanted only one character (= '\0')
    if (max == 1) {
        s[0] = '\0';
        return 0;
    }

    int c = EOF; // We will save read character here

    // Skip opening white-spaces
    while (1) {
        c = fgetc(f);
        if (c == EOF) {
            return EOF;
        }
        else if (!isspace(c)) {
            break;
        }
    }

    // We have one character already read
    s[0] = c;

    if (max == 2) { // Aren't we done yet?
        s[1] = '\0';
        return 1;
    }
    else {
        for (int i = 1; i < max-1; i++) {
            c = fgetc(f);
            if (c == EOF) {
                s[i] = '\0';
                return EOF;
            }
            else if (isspace(c)) {
                s[i] = '\0';
                return i;
            }
            else {
                s[i] = c;
            }
        }
        // Here we have reached max chars
        s[max] = '\0';
        // Skip rest of the word
        while (1) {
            c = fgetc(f);
            if (c == EOF || isspace(c)) {
                break;
            }
        }

        /* When we skip rest of the word,
           we return 'max' (with '\0') to indicate this */
        return max;
    }
}
