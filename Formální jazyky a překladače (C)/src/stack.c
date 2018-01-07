/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "stack.h"

Stack stackInit(unsigned int size) {
	Stack stack;
	stack.size = size;
	stack.SP = 0;
	stack.BP = 0;
	stack.data = malloc(sizeof(Data)*size);
	return stack;
}

void stackFree(Stack *stack) {
	free(stack->data);
}

int stackPush(Stack *stack, Data data) {
	if (stack->SP < stack->size) {
		stack->data[stack->SP++] = data;
		return 0;
	}	
	else return 1;
}

int stackPop(Stack *stack, Data *data) {
	if (stack->SP > stack->BP) {
		/* 
		musi byt hard copy? ano nebo ne ?
		data->type = stack.data[stack.SP].type;
		data->value = stack.data[stack.SP].value;*/
		*data = stack->data[--stack->SP];
		return 0;
	}
	else return 1; 
}

int stackResize(Stack stack, unsigned int newSize) {
	//TODO implementace stackREsize
	return 0;
}