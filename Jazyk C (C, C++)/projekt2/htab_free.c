/**
** htab_free.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <stdlib.h>

#include "htable.h"

/* Remove table from memory, calls clear */
void htab_free(htab_t *t) {

    htab_clear(t);
    free(t);
    t = NULL;
}
