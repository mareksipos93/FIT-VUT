/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef IAL_H
#define IAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "valgrind.h"
#include "error.h"

//Boyer-Moore
int findSubstring(char *s, char *search);


#define HASH_TABLE_SIZE 101

//Hash table
typedef struct hItem {
	char *key;
	char *type; //1. VELKYM typ var/fce/navesti... 2. datovy typ 3. az n pro parametry F
	unsigned int data; // 0-undefined 1-defined 
	unsigned int index; // poradi promenne 0-n
	struct hItem *next; // kvuli konfliktum v omezenosti hashe
}*HashTable;

HashTable createHashTable(unsigned int size);
void deleteHashTable(HashTable hashTable);
int addToHashTable(HashTable hashTable, char *key, char *type, unsigned int data,unsigned int index);
HashTable searchInHashTable(HashTable hashTable, char *key);
unsigned int hash(char *key);

void listMergeSort(char *arr);

typedef struct listM{
	int index;
	struct listM *next;
}*neklPos; // list neklesajici posloupnosti

typedef struct {
	neklPos first;
	neklPos active;
	neklPos last; //dalsi zacatek
}listZacatku; //list zacaktku

listZacatku lzinit();
void copyFirst(listZacatku *L,int *index);
void deleteFirst(listZacatku *L);
void first(listZacatku *L);
int active(listZacatku *L);
void insertLast(listZacatku *L, int index);




#endif // include guard
