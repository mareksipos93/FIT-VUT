/**
** ppm.c
** Solution: IJC-DU1, task b), 25.03 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, optimization: -O2
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ppm.h"
#include "error.h"

// Loads the PPM file to dynamically allocated PPM structure
struct ppm * ppm_read(const char * filename){

    // File opening
    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL) {
        warning_msg("Soubor %s se nepodarilo otevrit pro cteni!\n", filename);
        return NULL;
    }

    // Header (text)
    char magicnumber[3];
    unsigned xsize, ysize;
    int colordepth;

    // Less than 4 values from header read
    if (fscanf(fp, "%2s %u %u %d", magicnumber, &xsize, &ysize, &colordepth) < 1) {
        warning_msg("Hlavicka souboru %s nesla nacist!\n", filename);
        fclose(fp);
        return NULL;
    }

    // File not begins with P6 magic number
    if (strcmp("P6", magicnumber) != 0) {
        warning_msg("Soubor %s je nespravneho formatu! Nacteny format: %s\n", filename, magicnumber);
        fclose(fp);
        return NULL;
    }

    unsigned binsize = xsize*ysize*3;

    // File contains more color bytes that we can handle
    if (binsize > PPMBYTELIMIT) {
        warning_msg("Obrazek souboru %s je prilis velky!\n", filename);
        fclose(fp);
        return NULL;
    }

    // Unsupported color format
    if (colordepth != 255) {
        warning_msg("Rozsah barev souboru %s neni 255!\n", filename);
        fclose(fp);
        return NULL;
    }

    // Color pixels (binary)
    fscanf(fp,"%*c"); // Skips one whitespace before binary data

    struct ppm *image = NULL;

    // Dynamic structure allocation
    if ((image = malloc(sizeof(struct ppm) + binsize)) == NULL) {
        warning_msg("Chyba pri alokaci dat pro soubor %s!\n", filename);
        fclose(fp);
        return NULL;
    }

    image->xsize = xsize;
    image->ysize = ysize;

    // Reads binary data
    if (fread(image->data, 1, binsize, fp) != binsize) {
        warning_msg("Chyba pri cteni obrazovych dat v souboru %s!\n", filename);
        free(image);
        fclose(fp);
        return NULL;
    }

    // PPM must end with EOF
    if(fgetc(fp) != EOF) {
        warning_msg("Nespravne zakonceni souboru %s!\n", filename);
        free(image);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return image;
}

// Saves content of PPM structure into PPM file
int ppm_write(struct ppm *p, const char * filename) {

    // File opening
    FILE *fp;
    if ((fp = fopen(filename, "wb")) == NULL) {
        warning_msg("Soubor %s se nepodarilo otevrit pro zapis!\n", filename);
        return -1;
    }

    // Write header
    if (fprintf(fp, "P6\n%u %u\n255\n", p->xsize, p->ysize) < 0) {
        warning_msg("Chyba pri zapisu hlavicky do souboru %s!\n", filename);
        fclose(fp);
        return -2;
    }

    unsigned binsize = p->xsize*p->ysize*3;

    // Write binary data
    if (fwrite(p->data, 1, binsize, fp) != binsize) {
        warning_msg("Chyba pri zapisu obrazovych dat do souboru %s!\n", filename);
        fclose(fp);
        return -3;
    }

    fclose(fp);
    return 0;
}
