/**
** eratosthenes.c
** Solution: IJC-DU1, task a), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#include <math.h>
#include "eratosthenes.h"
#include "bit_array.h"

// Implementation of simple Eratosthenes algorithm
void Eratosthenes(bit_array_t name) {
    for (unsigned long i = 2; i < sqrt(ba_size(name)); i++)
    {
        // Got prime number
        if (ba_get_bit(name, i) == 0) {
            for (unsigned long j = i*2; j < ba_size(name); j+=i)
                ba_set_bit(name, j, 1);
        }
    }
}
