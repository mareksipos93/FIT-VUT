/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef IO_H
#define IO_H

/* ------------------------------HLAVICKOVE SOUBORY----------------------------------- */
#include "ial.h"
#include "instructions.h"
#include "lex_anal.h"
#include "error.h"
#include "framework.h"
#include "interpret.h"
/* ----------------------------------------------------------------------------------- */

#define STR_MAX 100000

char *readString();
bool readInt(int *out);
double readDouble(double *out);


#endif // include guard
