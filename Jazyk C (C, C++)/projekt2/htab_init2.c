/**
** htable_init2.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <stdlib.h>

#include "htable.h"

/* Create and initialize table by size with custom hash function */
htab_t *htab_init2(unsigned int size, unsigned int (*hash_fun_ptr)(const char *, unsigned int)) {

    // Get memory
    htab_t *htab = malloc(sizeof(htab_t) + size * sizeof(struct htab_listitem *));

    // Failed to allocate memory
    if (htab == NULL) {
        return NULL;
    }

    // Initialize
    htab->htab_size = size;
    htab->hash_fun_ptr = hash_fun_ptr;
    htab->n = 0;
    for (unsigned int i = 0; i < size; i++) {
        htab->ptr[i] = NULL;
    }

    return htab;
}
