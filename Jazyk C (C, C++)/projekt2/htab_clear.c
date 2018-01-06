/**
** htab_clear.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <stdlib.h>

#include "htable.h"

/* Remove all keys from table */
void htab_clear(htab_t *t) {

    for (unsigned int i = 0; i < t->htab_size; i++) { // Loop lists
        struct htab_listitem *actual = t->ptr[i]; // Actual looped item from list
        struct htab_listitem *next = NULL; // Item after looped one
        while (actual != NULL) { // Loop items from actual list
            next = actual->next;
            free(actual->key);
            actual->key = NULL;
            free(actual);
            actual = next; // Do next one
        }
        t->ptr[i] = NULL;
    }
}
