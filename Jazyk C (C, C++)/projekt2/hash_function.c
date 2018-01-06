/**
** hash_function.c
** Solution: IJC-DU2, task b), 23.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2
*/

#include "hash_function.h"

/* Default hash_function */
unsigned int hash_function(const char *str, unsigned int htab_size) {
  unsigned int h=0;
  const unsigned char *p;
  for(p=(const unsigned char*)str; *p!='\0'; p++)
      h = 65599*h + *p;
  return h % htab_size;
}
