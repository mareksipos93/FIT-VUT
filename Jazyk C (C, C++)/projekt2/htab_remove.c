/**
** htab_remove.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htable.h"

/* Remove key from table */
void htab_remove(htab_t *t, const char *key) {

    char str[strlen(key)];
    strcpy(str, key); // Makes copy of key, just to be safe
    unsigned int hash = t->hash_fun_ptr(str, t->htab_size); // Get hash of given key

    // Loop list items
    struct htab_listitem *actual = t->ptr[hash];
    struct htab_listitem *last = NULL;
    while (actual != NULL) {
        if (strcmp(actual->key, str) == 0) { // Found it

            // Found item was first in the list
            if (last == NULL) {
                t->ptr[hash] = actual->next;
            }
            // Found item was in the middle of the list
            else if (last != NULL && actual->next != NULL) {
                last->next = actual->next;
            }
            // Found item was last in the list
            else if (last != NULL && actual->next == NULL) {
                last->next = NULL;
            }

            // Free allocated memory of this item
            free(actual->key);
            actual->key = NULL;
            free(actual);
            actual = NULL;

            return;
        }
        last = actual;
        actual = actual->next; // Do next one
    }

    return;
}
