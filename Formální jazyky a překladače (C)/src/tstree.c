/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "tstree.h"
#include "testing.h"

void tsTreeInit(TsTree *root) {
	*root = NULL;
}

/*
	prida TS do stromu na zaklade jmena, 
	nekontroluje strapvnost jmen
	name = plny identifikator
	ht = predem vytvorena TS
*/
int tsAdd(TsTree *root, char *name, unsigned int varCount, void *addr, HashTable ht) {
	TsTree tmp;
	tmp = malloc(sizeof(struct tsTree));
	if (tmp == NULL) {
		fprintf(stderr, "Malloc Err\n");
		return 1;
	}
	else {
		tmp->name = makeString(name);
		tmp->addr = addr;
		tmp->varCount = varCount;
		tmp->ts = ht;
		tmp->next = NULL;
		tmp->child = NULL;
	}

	if (*root == NULL) { // prvni	
		*root = tmp;
	}
	else { //najdu kam
		//printf("find for global\n");
		if (findSubstring(tmp->name, ".") < 0) { //global
			for (TsTree x = *root; x != NULL; x = x->next) { //najdu konec
				if (x->next == NULL) {
					x->next = tmp;
					//printf("added");
					return 1;
				}
			}
		}
		else {//local
			// najdu rodice 
			//printf("find for local\n");
			for (TsTree x = *root; x != NULL; x = x->next) {
				if (isHisParent(name, x->name)) { // nasel rodice
					//najdu konec seznamu childs
					if (x->child == NULL) { //prvni child
						x->child = tmp;
					}
					else {
						for (TsTree t = x->child; t != NULL; t = t->next) {
							if (t->next == NULL) { //konec
								t->next = tmp;	// priradim na konec
								return 1;
							}
						}
					}
					break;
				}
			}
		}
	}
	return 1;
}



void tsDel(TsTree *root) {
	TsTree tmp;
	for (TsTree x = *root; x != NULL;) {
		for (TsTree t = x->child; t != NULL;) {
			debug(" [TSTREE] Deleting: %s\n",t->name);
			free(t->name);
			deleteHashTable(t->ts);
			tmp = t->next;
			free(t);
			t = tmp;
			debug(" [TSTREE] Deleted.\n");
		}
		debug(" [TSTREE] Deleting: %s\n", x->name);
		free(x->name);
		deleteHashTable(x->ts);
		tmp = x->next;
		free(x);
		x = tmp;
		debug(" [TSTREE] Deleted.\n");
	}

	*root = NULL;
}

TsTree tsFind(TsTree root, char *name) {
	for (TsTree x = root; x != NULL; x = x->next) {
		if (!strcmp(x->name, name)) return x;
		if (isHisParent(name, x->name)) {
			for (TsTree y = x->child; y != NULL; y = y->next) {
				if (!strcmp(y->name, name)) {
					return y;
				}
			}
		}
	}
	return NULL;
}

int isHisParent(char *name, char *parent) {
	if (findSubstring(name, parent) == 0 &&
		name[strlen(parent)] == '.'
		) return 1;
	return 0;
}