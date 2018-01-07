/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/


#include "collections.h"

/////////////////////////////
/////////// STACK ///////////
/////////////////////////////

bool cStack_init(cStack *stack, unsigned size) {
    stack->cap = size;
    stack->used = 0;
    stack->items = malloc(sizeof(cItem)*size);
    return (stack->items != NULL);
}

bool cStack_push(cStack *stack, cItem item) {
    if (stack->used >= stack->cap) {
        cItem *resized = realloc(stack->items, sizeof(cItem) * 2 * stack->cap);
        if (resized == NULL) {
            return false;
        }
        else {
            stack->items = resized;
        }
        stack->cap *= 2;
    }
    stack->items[stack->used++] = item;
    return true;
}

bool cStack_pop(cStack *stack) {
    if (cStack_isempty(stack)) {
        return false;
    }
    else {
        stack->used--;
        return true;
    }
}

cItem cStack_top(cStack *stack) {
    if (cStack_isempty(stack)) {
        cItem item;
        item.type = IT_ERROR;
        item.content.error = 1;
        return item;
    }
    return stack->items[stack->used-1];
}

bool cStack_isempty(cStack *stack) {
    return (stack->used == 0);
}

void cStack_free(cStack *stack) {
    free(stack->items);
}

/////////////////////////////
/////////// QUEUE ///////////
/////////////////////////////

void cQueue_init(cQueue *queue) {
    queue->first = NULL;
    queue->size = 0;
}

bool cQueue_insert(cQueue *queue, cItem item) {
    cQueueElem *elem = (cQueueElem *) malloc(sizeof(cQueueElem));
    if (elem == NULL) {
        return false;
    }
    else {
        if (cQueue_isempty(queue)) {
            queue->first = elem;
        }
        else {
            cQueueElem *iterated = queue->first;
            while (iterated->next != NULL) {
                iterated = iterated->next;
            }
            iterated->next = elem;
        }
        elem->next = NULL;
        elem->item = item;
        queue->size++;
        return true;
    }
}

cItem cQueue_first(cQueue *queue) {
    if (cQueue_isempty(queue)) {
        cItem item;
        item.type = IT_ERROR;
        item.content.error = 1;
        return item;
    }
    return queue->first->item;
}

bool cQueue_pop(cQueue *queue) {
    if (cQueue_isempty(queue)) {
        return false;
    }
    else {
        cQueueElem *tofree = queue->first;
        queue->first = tofree->next;
        queue->size--;
        free(tofree);
        return true;
    }
}

bool cQueue_isempty(cQueue *queue) {
    return (queue->size == 0);
}

void cQueue_free(cQueue *queue) {
    if (!cQueue_isempty(queue)) {
        cQueueElem *tofree = queue->first;
        cQueueElem *next = queue->first;
        while (next != NULL) {
            next = tofree->next;
            free(tofree);
        }
    }
    queue->first = NULL;
    queue->size = 0;
}

