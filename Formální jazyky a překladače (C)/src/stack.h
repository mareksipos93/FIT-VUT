/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
	char *name;
	enum {
		t_int,
		t_double,
		t_string,
	}type;
	union {
		int v_int;
		double v_double;
		char *v_string;
	}value;
	bool defined;
}Data;

typedef struct {
	unsigned int size;
	unsigned int SP; // index vrcholu
	unsigned int BP; // base pointer
	Data *data;
}Stack;


Stack stackInit(unsigned int size);
void stackFree(Stack *stack);
int stackPush(Stack *stack, Data data);
int stackPop(Stack *stack, Data *data);
int stackResize(Stack stack, unsigned int newSize);


#endif // !STACK
