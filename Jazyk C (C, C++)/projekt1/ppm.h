/**
** ppm.h
** Solution: IJC-DU1, task b), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#ifndef PPM_H
#define PPM_H

// Max bytes for storing color information
#define PPMBYTELIMIT 5000*5000*3

struct ppm {
    unsigned xsize; // Horizontal size
    unsigned ysize; // Vertical size
    char data[]; // RGB data 3*xsize*ysize
};

struct ppm * ppm_read(const char * filename);

int ppm_write(struct ppm *p, const char * filename);

#endif // ifndef PPM.H
