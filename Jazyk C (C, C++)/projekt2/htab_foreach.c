/**
** htab_foreach.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <stdio.h>

#include "htable.h"

/* Run function for each entry in table */
void htab_foreach(htab_t *t, void (*func)(const char *, unsigned int)) {

    for (unsigned int i = 0; i < t->htab_size; i++) { // Loop lists
        struct htab_listitem *actual = t->ptr[i]; // Actual looped item from list
        while (actual != NULL) { // Loop items from actual list
            func(actual->key, actual->data); // Call function
            actual = actual->next; // Do next one
        }
    }

    return;
}
