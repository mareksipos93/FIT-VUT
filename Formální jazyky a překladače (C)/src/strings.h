/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef STRINGS_H
#define STRINGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "valgrind.h"
#include "stack.h"
#include <stdbool.h>

// prace s retezci

char *makeString(char *str);
char *cat(char *s1, char *s2);
char *varToString(Data *var);
char *intToString(int x);
char *doubleToString(double x);
char *getSubString(char *str, int startindex, int size);
int compare(char *s1, char *s2);

/* Extract part from full identifier
   For example:
       - explodeFullIdentifier("class.ident", false) -> "class"
       - explodeFullIdentifier("class.ident", true)  -> "ident" */
char *explodeFullIdentifier(char *id, bool left);

#endif // include guard
