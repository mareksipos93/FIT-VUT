/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdbool.h>
#include "lex_anal.h"
#include "synt_structures.h"

////////////////////////////
/////////// ITEM ///////////
////////////////////////////

typedef enum {
    IT_ERROR,
    IT_TERMINAL,
    IT_NTTYPE,
    IT_PSYMBOL,
    IT_TOKEN,
    IT_FEW
}cItemType;

typedef union {
    int error;
    Terminal terminal;
    NTType nttype;
    Psymbol psymbol;
    Ttoken *token;
    FEW few;
}cItemData;

typedef struct {
    // Type of stored data
    cItemType type;
    // Data (any type from cItemData union)
    cItemData content;
}cItem;

/////////////////////////////
/////////// STACK ///////////
/////////////////////////////

typedef struct {
    unsigned cap; // Capacity
    unsigned used; // Used capacity
    cItem *items; // Items
}cStack;

// Init stack with custom size
bool cStack_init(cStack *stack, unsigned size);
// Insert item into stack
bool cStack_push(cStack *stack, cItem item);
// Remove item from the stack top
bool cStack_pop(cStack *stack);
// Receive a copy of item from the stack top (when empty: item.type = T_ERROR)
cItem cStack_top(cStack *stack);
// Check whether the stack is empty
bool cStack_isempty(cStack *stack);
// Free items in stack
void cStack_free(cStack *stack);

/////////////////////////////
/////////// QUEUE ///////////
/////////////////////////////

typedef struct cQueueElem {
    struct cQueueElem *next; // Next element
    cItem item; // Data (item)
} cQueueElem;

typedef struct {
    cQueueElem *first; // First element
    unsigned size; // Amount of elements
} cQueue;

// Init Queue
void cQueue_init(cQueue *queue);
// Insert item into queue
bool cQueue_insert(cQueue *queue, cItem item);
// Receive a copy of the first item (when empty: item.type = T_ERROR)
cItem cQueue_first(cQueue *queue);
// Removes a first item from the Queue
bool cQueue_pop(cQueue *queue);
// Check whether the Queue is empty
bool cQueue_isempty(cQueue *queue);
// Free all elements of Queue
void cQueue_free(cQueue *queue);

#endif // include guard
