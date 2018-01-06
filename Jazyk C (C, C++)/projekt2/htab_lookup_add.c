/**
** htab_lookup_add.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htable.h"

/* Find entry by key, if key cannot be found, create new one */
struct htab_listitem *htab_lookup_add(htab_t *t, const char *key) {

    char str[strlen(key)];
    strcpy(str, key); // Makes copy of key, just to be safe
    unsigned int hash = t->hash_fun_ptr(str, t->htab_size); // Get hash of given key
    int createNew = 0; // If new list item will be made, 0 = no, 1 = yes first, 2 = yes last
    struct htab_listitem *last = NULL; // Last used item of the list (used on the end)

    // Table does not have any list on this index
    if (t->ptr[hash] == NULL) {
        createNew = 1; // We will create new one (first on this index)
    }
    // Table already has list on this index, loop through it
    else {
        struct htab_listitem *checked = t->ptr[hash]; // Actual checked item
        do {
            if (strcmp(checked->key, str) == 0) { // Found it
                checked->data += 1;
                return checked;
            }
            else { // Not found, try next one
                last = checked;
                checked = checked->next;
            }
        } while (checked != NULL);

        createNew = 2; // Not found, we will create new one (last on this index)
    }

    // Create new list and return it
    struct htab_listitem *result = malloc(sizeof(struct htab_listitem)); // Memory for list item
    if (result == NULL) { // Malloc failed
        return NULL;
    }
    result->key = malloc(strlen(str)+1 * sizeof(char)); // Memory for string 'key'
    if (result->key == NULL) { // Malloc failed
        free(result); // Important - free already malloced memory for list item
        return NULL;
    }
    strcpy(result->key, str); // Save given key
    result->data = 1; // First occurrence
    result->next = NULL; // This item is last
    t->n += 1; // New item
    if (createNew == 1) {
        t->ptr[hash] = result; // Assign list to table as first one on this index
    }
    else {
        last->next = result; // Assign new item as next of last one
    }

    return result;
}
