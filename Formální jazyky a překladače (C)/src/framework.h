/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "ial.h"
#include "tstree.h"
#include "instructions.h"
#include "strings.h"


typedef struct sFrame {
	char *identifier; // pro vyhledavani?? jmeno fce 
	struct sFrame *top;
	struct sFrame *parent;
	struct sFrame *child;
	unsigned int size; // pro vyhledavani
	Data *data; //parametry a promenne
	Data *ret; //adresa na promennou kde se bude vkladat return
	tInstrListItem *lastActive; // adresa aktivni insturkce pred call
}StackFrame;

Data *findInFrame(char *name, StackFrame *sf);
StackFrame *newFrame(StackFrame *parent, TsTree root, char *name,Data *ret, tInstrListItem *lastActive);
StackFrame *deleteFrame(StackFrame *sf);
StackFrame *newTopFrame(TsTree root);



#endif // !FRAMEWORK

