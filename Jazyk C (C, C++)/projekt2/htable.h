/**
** htable.h
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#ifndef HTABLE_H_INCLUDED
#define HTABLE_H_INCLUDED

#include "hash_function.h"

/* Main table structure with its own type called htab_t */
typedef struct htable {
    unsigned int htab_size;
    unsigned int (*hash_fun_ptr)(const char *, unsigned int);
    unsigned int n;
    struct htab_listitem *ptr[]; // Flexible array member
} htab_t;

/* One entry from htable */
struct htab_listitem {
    char *key;
    unsigned int data;
    struct htab_listitem *next;
};

/* Create and initialize table by size */
htab_t *htab_init(unsigned int size);
/* Create and initialize table by size with custom hash function */
htab_t *htab_init2(unsigned int size, unsigned int (*hash_fun_ptr)(const char *, unsigned int));
/* Find entry by key, if key cannot be found, create new one */
struct htab_listitem *htab_lookup_add(htab_t *t, const char *key);
/* Run function for each entry in table */
void htab_foreach(htab_t *t, void (*func)(const char *, unsigned int));
/* Remove key from table */
void htab_remove(htab_t *t, const char *key);
/* Remove all keys from table */
void htab_clear(htab_t *t);
/* Remove table from memory, calls clear */
void htab_free(htab_t *t);

#endif // HTABLE_H_INCLUDED
