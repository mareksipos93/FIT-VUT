/**
** bit_array.h
** Solution: IJC-DU1, task a), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#include <stdio.h>
#include <limits.h>
#include "error.h"

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

typedef unsigned long bit_array_t[];

// Amount of bits in unsigned long
// CHAR_BIT represents amount of bits in byte, see limits.h
#define ULONG_BITSIZE (CHAR_BIT * sizeof(unsigned long))
// Amount of bits in any integer type - based on first element of array
// CHAR_BIT represents amount of bits in byte, see limits.h
#define NUMBER_BITSIZE(name) (CHAR_BIT * sizeof(name[0]))

// Auxiliary macros, without range checking
#define BIT_SHIFT(name, index) ((NUMBER_BITSIZE(name)-(index % NUMBER_BITSIZE(name)))-1)
#define DU1_GET_BIT_(name, index) ((((name)[(index) / NUMBER_BITSIZE(name)]) & (1UL << (BIT_SHIFT(name, (index))))) >> (BIT_SHIFT(name, (index))))
#define DU1_SET_BIT_(name, index, expression) (expression) != 0 ? (name[(index) / NUMBER_BITSIZE(name)] |= (1UL << (BIT_SHIFT(name, (index)))))\
                                                                : (name[(index) / NUMBER_BITSIZE(name)] &= ~(1UL << (BIT_SHIFT(name, (index)))))

// Calculates needed size for bit array (needed for integer division)
#define needed_size(size) ((size) % ULONG_BITSIZE == 0 ? (size)/ULONG_BITSIZE : ((size)/ULONG_BITSIZE)+1)
// Creates new bit array with specified name and size (size saved in first u-long)
// Rest of u-longs are filled with zeros (works both as global and local variable)
// IMPORTANT - cannot be made as in-line function due to dynamic declaration
//             thus this one is always parsed as macro even with -USE_INLINE argument
#define ba_create(name, size) unsigned long name[needed_size(size)+1] = {(size), 0}

// MACRO VARIATIONS OF FUNCTIONS
#ifndef USE_INLINE

// Amount of bits available in bit array
#define ba_size(name) (name[0])

// Get bit of bit array name on index index
#define ba_get_bit(name, index) ( (index) >= ba_size(name) ? fatal_error("Index %ld mimo rozsah 0..%ld", (index), ba_size(name)-1), 0 /* , 0 Ternary operator trick to force evaluating */ \
                                                         : DU1_GET_BIT_(name, index+ULONG_BITSIZE) )

// Set bit of bit array name on index index to expression
#define ba_set_bit(name, index, expression) ( (index) >= ba_size(name) ? fatal_error("Index %ld mimo rozsah 0..%ld", (index), ba_size(name)-1), 0 /* , 0 Ternary operator trick to force evaluating */ \
                                                         : DU1_SET_BIT_(name, index+ULONG_BITSIZE, expression) )

// IN-LINE VARIATIONS OF FUNCTIONS
#else

// Amount of bits available in bit array
static inline unsigned long ba_size(bit_array_t name) {
    return name[0];
}

// Get bit of bit array name on index index
static inline unsigned long ba_get_bit(bit_array_t name, unsigned long index) {
    // Bit overflow
    if (index >= ba_size(name))
        fatal_error("Index %ld mimo rozsah 0..%ld", index, ba_size(name)-1);

    // Gets bit, << >> stands for bit shift, & bitwise AND
    return DU1_GET_BIT_(name, index+ULONG_BITSIZE);
}

// Set bit of bit array name on index index to expression
static inline void ba_set_bit(bit_array_t name, unsigned long index, short expression) {
    // Bit overflow
    if (index >= ba_size(name))
        fatal_error("Index %ld mimo rozsah 0..%ld", index, ba_size(name)-1);

    // Sets bit, ~ stands for negation, << bit shift, | & bitwise OR ANDULONG_BITSIZE
    DU1_SET_BIT_(name, index+ULONG_BITSIZE, expression);
    return;
}

#endif // ifndef USE_INLINE

#endif // ifndef BIT_ARRAY_H
