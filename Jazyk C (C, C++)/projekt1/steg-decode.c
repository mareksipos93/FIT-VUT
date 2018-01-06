/**
** steg-decode.c
** Solution: IJC-DU1, task b), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#include <stdlib.h>
#include <ctype.h>
#include "bit_array.h"
#include "eratosthenes.h"
#include "error.h"
#include "ppm.h"

int main(int argc, char **argv) {

    // Missing program argument
    if(argc < 2) {
        fatal_error("Chybi nazev PPM (P6) souboru k dekodovani!\n");
    }

    // Prepares bitarray
    struct ppm *image = ppm_read(argv[1]);
    ba_create(bytes, PPMBYTELIMIT); // Static, otherwise image->xsize*image->ysize*3
    Eratosthenes(bytes);

    short tempchar[1]; // Bits of this are used for restructuring printable char
    tempchar[0] = 0; // Its array because DU1_SET_BIT works with array
    short temppos = 0; // Where are we saving (from 0 to CHAR_BIT-1)
    short ended = 0; // If message ended correctly with \0

    for (unsigned long i = 2; i < PPMBYTELIMIT; i++)
    {
        // Not prime number index
        if (ba_get_bit(bytes, i) == 1) {
            continue;
        }

        // Decode bits
        short bit = DU1_GET_BIT_(image->data, CHAR_BIT*i+7);
        DU1_SET_BIT_(tempchar, sizeof(bit)*CHAR_BIT-temppos-1, bit);
        temppos++;

        // Full char achieved
        if (temppos == CHAR_BIT) {
            fprintf(stdout, "%c", tempchar[0]);

            // Not printable
            if (!isprint(tempchar[0]) && tempchar[0] != '\0') {
                free(image);
                fatal_error("Zprava obsahuje netisknutelne znaky!\n");
            }

            // Last one, quit
            if (tempchar[0] == '\0'){
                fprintf(stdout, "\n");
                ended = 1;
                break;
            }

            // Reset for another char
            tempchar[0] = 0;
            temppos = 0;
        }
    }
    // Not ended with \0
    if (ended == 0) {
        free(image);
        fatal_error("Zprava nebyla ukoncena nulovym znakem!\n");
    }

    free(image);
    return 0;
}


