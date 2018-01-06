/**
** primes.c
** Solution: IJC-DU1, task b), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#include "bit_array.h"
#include "eratosthenes.h"

// Maximum size of bit array, need to extend memstack !!!
#define PR_SEARCH_CAP 202000000
// Print last X primes
#define PR_PRINT_LIMIT 10

int main() {

    ba_create(array, PR_SEARCH_CAP);
    unsigned long printlimit = 0;
    unsigned long results[10];
    Eratosthenes(array);

    // Fetch results in array (to be able to print in ascending order)
    for (unsigned long i = PR_SEARCH_CAP-1; i > 2; i--)
    {
        if ((ba_get_bit(array, i) == 0) && (printlimit < PR_PRINT_LIMIT)) {
            results[printlimit] = i;
            printlimit++;
        }
    }

    // Print results
    for (short i = 10; i > 0; i--)
        printf("%lu\n", results[i-1]);

    return 0;
}


