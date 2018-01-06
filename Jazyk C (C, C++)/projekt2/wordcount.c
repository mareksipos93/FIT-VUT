/**
** wordcount.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include <stdio.h>
#include <stdlib.h>

#include "htable.h"
#include "io.h"

#define WORD_SIZE 127 // Maximum size of word (without null terminator)
#define HTAB_SIZE 49157 // Hash table size

/* Komentar k vyberu HTAB_SIZE

Dle teorie je nejlepsi zvolene cislo prvocislo, co nejvic
vzdalene mocnine dvou. Takove cislo dokaze spolehlive odolavat
clusteringu (prilisnem shlukovani - nerovnomerne rozlozeni) a tudiz
i minimalizovat mnozstvi kolizi (zalezi vsak i na zvolene hash funkci).

Cislo 49157 je dle meho idealni kompromis - nema prehnane pametove a
vykonnostni naroky a poskytuje uspokojujici nahodnost rozmisteni.

Cislo jsem si vybral z teto webove stranky:
http://planetmath.org/goodhashtableprimes

*/

void print_helper(const char *str, unsigned int n) {
    printf("%s\t%u\n", str, n);
}

int main()
{
    // Initialize
    char word[WORD_SIZE+1];
    htab_t *htable = htab_init(HTAB_SIZE);
    if (htable == NULL) {
        fprintf(stderr, "Failed to initialize htable\n");
        return 1;
    }

    // Save words
    int len;
    int cut = 0;
    while ((len = get_word(word, WORD_SIZE+1, stdin)) != EOF) {
        if (len == WORD_SIZE+1 && cut == 0) {
            printf("Varovani: jedno nebo vice slov muselo byt orezano!\n");
            cut = 1;
        }
        if (htab_lookup_add(htable, word) == NULL) {
            fprintf(stderr, "Failed to add key to htable\n");
            htab_free(htable);
            return 1;
        }
    }

    // Print results
    htab_foreach(htable, print_helper);

    htab_free(htable);
    return 0;
}
