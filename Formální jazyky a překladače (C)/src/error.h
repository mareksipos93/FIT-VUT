/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

/* ========================================================================== */
/* Jmena:   .......                                                           */
/* Soubor:  error.c                                                           */
/* Faze:    Chybove stavy                                                     */
/* Datum:   24.10.2016                                                        */
/* ========================================================================== */
enum chyboveStavy
{
	ERR_OK = 0,			/* iadna chyba. */
	ERR_LEX = 1,			/* 1 - Chyba v programe v rámci lexikálnej analızy (chybná štruktúra aktuálneho lexému). */
 	ERR_SYNT = 2,			/* 2 - Chyba v programe v rámci syntaktickej analızy (chybná syntax štruktúry programu). */
 	ERR_SEM_DEF = 3,	            /* 3 - Sémantická chyba - nedefinovaná funkcia/premenná, pokus o redefinovanie funkcie/premennej. */
 	ERR_SEM_TYPE = 4,	            /* 4 - Sémantická chyba typovej kompatibility v aritmetickıch, reazcovıch a relaènıch vırazoch,
                                    prípadne nesprávny poèet èi typ parametrov pri volaní funkcie. */
 	ERR_SEM_OTHER = 6,		    /* 6 - Ostatné sémantické chyby. */
 	ERR_RUN_NUM = 7,			/* 7 - Behová chyba pri naèítaní èíselnej hodnoty zo vstupu. */
 	ERR_RUN_INIT = 8,		/* 8 - Behová chyba pri práci s neinicializovanou premennou. */
 	ERR_RUN_DEV = 9,		/* 9 - Behová chyba delenie nulou. */
 	ERR_RUN_OTHER = 10,			/* 10 - Ostatné behové chyby. */
 	ERR_INTER = 99	        /* 99 - Interná chyba interpretu tj. neovplyvnite¾ná vstupnım programom (napr. chyba alokácie
                                    pamäti, chyba pri otváraní súboru s riadiacim programom, nesprávne parametre príkazového
                                    riadku atï.). */
};

// Funkce pro zpusobeni chyby, zastavi beh programu (pokud to neni USPES_PREKL)
void error(int chyba);

#endif // include guard
