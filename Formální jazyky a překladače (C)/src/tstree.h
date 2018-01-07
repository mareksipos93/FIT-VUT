/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef TSTREE_H
#define TSTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ial.h"
#include "valgrind.h"
#include "strings.h"
#include "ial.h"
#include "instructions.h"

typedef struct tsTree{
	char *name; //identifikator
	unsigned int varCount; // pocet promennych
	tInstrListItem *addr; //adresa instrukce
	HashTable ts; //tabulka symbolu
	struct tsTree *child; // seznam lokalnich ts
	struct tsTree *next; // list TS akt. urovne
}*TsTree;

void tsTreeInit(TsTree *root);
int tsAdd(TsTree *root, char *name,unsigned int varCount,void *addr, HashTable ht);
void tsDel(TsTree *root);
TsTree tsFind(TsTree root,char *name);
int isHisParent(char *name,char *parent);

#endif