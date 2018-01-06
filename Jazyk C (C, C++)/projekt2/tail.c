/**
** tail.c
** Solution: IJC-DU2, task a), 10.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, w/o optimization
*/

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Row size limit (without \n and \0)
#define LINESIZE 510

// Convert string to long int
bool strtolong(char *string, long *result) {
    char *temp;
    errno = 0;
    *result = strtol(string, &temp, 10);
    if (*temp != '\0' || errno == ERANGE)
        return false;
    else
        return true;
}

// Handle file input
void printtail(FILE *f, long printlines) {

    // Nothing to print
    if (printlines == 0) {
        return;
    }

    // Initialization
    long numoflines = 0;
    char **array = NULL;
    bool toolonginfo = false; // To print info about line too long only once

    // Get line by line input and save to array of pointers (each pointer = line)
    bool breakit = false;
    while (breakit == false) {
        // Extend size of array of lines
        char **newarray = realloc(array, (numoflines+1) * sizeof(char *));
        if (newarray == NULL) {
            fprintf(stderr, "Chyba pri alokaci pameti");
            exit(1);
        }
        else {
            array = newarray;
        }
        // Prepare space for reading line
        char *buffer = malloc(LINESIZE+2 * sizeof(char)); // +2 because \n and \0
        if (buffer == NULL) {
            fprintf(stderr, "Chyba pri alokaci pameti");
            exit(1);
        }
        // Read line
        clearerr(f);
/* */   char *test = fgets(buffer, LINESIZE+2, f);
        if (test == NULL) {
            if (ferror(f)) { // Error? Okay....
                fprintf(stderr, "Neznama chyba pri cteni dat");
                exit(1);
            }
            else // Empty line? Okay...
                buffer[0] = '\0';

        }
        // If line exceeds size limit
        if (strlen(buffer) == LINESIZE+1 && buffer[LINESIZE] != '\n') {
            // Print info (only once)
            if (toolonginfo == false) {
                fprintf(stderr, "Nektere radky jsou delsi nez %d znaku, budou orezany...\n", LINESIZE);
                toolonginfo = true;
            }
            // Skip rest
            char ch;
            do {
                ch = getc(f);
                if (ch == EOF) {
                    breakit = true;
                }
            }
            while (ch != '\n' && ch != EOF);
        }

        // Adds newline character to the end
        long len = strlen(buffer);
        if (len > 0) {
            if (len == LINESIZE+1) {
                if (buffer[len-1] != '\n') {
                    buffer[len-1] = '\n';
                }
            }
            else if (len < LINESIZE+1) {
                if (buffer[len-1] != '\n') {
                    buffer[len] = '\n';
                    buffer[len+1] = '\0';
                }
            }
        }

        // Change memory size for that line to exact size of line
        char *newbuffer = realloc(buffer, (strlen(buffer)+1) * sizeof(char));
        if (newbuffer == NULL) {
            fprintf(stderr, "Chyba pri alokaci pameti");
            exit(1);
        }
        else {
            buffer = newbuffer;
        }

        array[numoflines] = buffer; // Pass loaded line to line array
        if (feof(f)) // Check for break of infinite loop (EOF reached)
            breakit = true;
        else
            numoflines++; // else Continue reading
    }

    // We can output only lines which are present in file
    if (printlines > numoflines)
        printlines = numoflines;

    // Skip last empty line
    if (strcmp(array[numoflines], "\n") == 0) {
        free(array[numoflines]);
        numoflines--;
    }
    // Print results
    for (int i = numoflines-printlines; i < numoflines ; i++) {
        printf("%s", array[i]);
    }
    // Clean
    for (int i = 0; i <= numoflines; i++) {
        free(array[i]);
        array[i] = NULL;
    }
    free(array);
    array = NULL;
    return;
}

int main(int argc, char *argv[]) {

    long lines = 10; // Amount of lines to print
    int argindex = 0; // Last file argument index to handle
    FILE *f = NULL; // File to be opened
    if (argc > 1) {
        // Handle arguments
        for (int i = 1; i < argc; i++) {
            // '-' = direct input
            if (strcmp(argv[i], "-") == 0) { // Direct input
                argindex = i;
            }
            // '-n' = number of lines to print
            else if (strcmp(argv[i], "-n") == 0) {
                if (argv[i+1] != NULL) { // Argument i+1 with number
                    if (strtolong(argv[i+1], &lines)) { // Is valid number
                        if (lines < 0) // Negative = Positive (like POSIX)
                            lines = -lines;
                        i++; // argument i+1 was handled
                    }
                    else {
                        fprintf(stderr, "tail: '%s': Chybny pocet radku\n", argv[i+1]);
                        exit(1);
                    }
                }
                else {
                    fprintf(stderr, "tail: prepinac vyzaduje argument - 'n'\n");
                    exit(1);
                }
            }
            // Unknown argument starting with '-'
            else if (argv[i][0] == '-') {
                    fprintf(stderr, "tail: neplatny prepinac %s\n", argv[i]);
                    exit(1);
            }
            // This should be filename to open
            else {
                argindex = i;
            }
        }
    }
    // Direct input
    if (argindex == 0 || strcmp(argv[argindex], "-") == 0)
        printtail(stdin, lines);
    // File input
    else {
        f = fopen(argv[argindex], "r");
        if (f == NULL) {
            fprintf(stderr, "tail: '%s' nelze otevrit pro cteni\n", argv[argindex]);
            exit(1);
        }
        printtail(f, lines);
        fclose(f);
    }

    return 0;
}
