/**

ISA projekt - Čtečka novinek ve formátu Atom s podporou TLS
Autor: Marek Šipoš (xsipos03), 26.9. 2018

util.h - modul obsahující pomocné funkce

**/

#ifndef UTIL_H_
#define UTIL_H_

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#include <stdarg.h>

/******** OPTIONS ********/

#define MAX_CERTFILES		10  // min 1
#define MAX_CERTADDRESSES	10  // min 1
#define FEEDFILE_BUFFERSIZE	50  // min 2
#define DEBUG				0

// Allocates string in memory and returns pointer to it, concatenating all string params into single string
char *malloc_string(int num, ... );

#define print_debug(...) do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)

#endif // UTIL_H_
