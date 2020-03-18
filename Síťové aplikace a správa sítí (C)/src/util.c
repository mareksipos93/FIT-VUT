/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

util.c - modul obsahující pomocné funkce

**/

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#include <stdio.h>

char *malloc_string(int num, ... ) {
    va_list valist;
    va_start(valist, num);

    char *result = NULL;
    bool first = true;


    for (int i = 0; i < num; i++) {
        char *param = va_arg(valist, char*);

        if (param == NULL)
            continue;

        if (first) {
            first = false;
            result = (char*) malloc((strlen(param)+1) * sizeof(char));
            strcpy(result, param);
        } else {
            result = (char*) realloc(result, (strlen(result)+strlen(param)+1) * sizeof(char));
            strcat(result, param);
        }
    }
    
    va_end(valist);;

    return result;
}
