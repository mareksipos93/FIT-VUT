/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/

#ifndef LEX_ANAL_H
#define LEX_ANAL_H

// hlavickove soubory 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "error.h"

// vyctovy typ obsahujici seznam tokenu
typedef enum {
  
  // pocatecni stav
  POC_STAV,
  
  // komentare
  JEDNORADKOVY_KOMENTAR,
  VICERADKOVY_KOMENTAR,
  VICERADKOVY_KOMENTAR_2,
  
  // operandy (operace)
  SCITANI,
  DELENI,
  ODECITANI,
  NASOBENI,

  // relacni operatory
  MENSI,
  VETSI,
  ROVNO,
  NEROVNO,
  MENSI_NEBO_ROVNO,
  VETSI_NEBO_ROVNO,
  
  PRIRAZENI,
  STREDNIK,
  CARKA,
  
  // zavorky
  LEVA_KULATA_ZAVORKA,
  PRAVA_KULATA_ZAVORKA,
  LEVA_HRANATA_ZAVORKA,
  PRAVA_HRANATA_ZAVORKA,
  LEVA_SLOZENA_ZAVORKA,
  PRAVA_SLOZENA_ZAVORKA,
  
  
  // identifikatory
  IDENTIFIKATOR,
  PLNE_KVALIFIKOVANY_IDENTIFIKATOR,
  PLNE_KVALIFIKOVANY_IDENTIFIKATOR_2,
  KLICOVE_SLOVO,
  REZERVOVANE_SLOVO,
  
  // retezec
  RETEZEC,
  RETEZEC_2,
  
  // cisla
  CELOCISELNY_LITERAL,
  DESETINNY_LITERAL,
  DESETINNY_LITERAL_2,
  DESETINNY_LITERAL_EXPONENT,
  DESETINNY_LITERAL_EXPONENT_2,
  DESETINNY_LITERAL_EXPONENT_3,
  CELOCISELNY_LITERAL_EXPONENT,
  CELOCISELNY_LITERAL_EXPONENT_2,
  CELOCISELNY_LITERAL_EXPONENT_3,
  
  // konec souboru (EOF)
  KONEC_SOUBORU,
  // lexikalni chyba (lex_error) !! Musi byt vzdy posledni v ENUM !!
  LEXIKALNI_CHYBA // vzdy posledni v ENUM

}TokenType;

// struktury reprezentujici token
typedef struct{
	char* str;
	int length;
}string;

typedef struct token{
	string* attr;
	TokenType type;
	int cisloRiadku;
    struct token *next; // Pouziva syntakticka analyza, lex s tim nic nedela
}Ttoken;

// funkce vytvarejici token 
Ttoken* vytvorToken();
// funkce pro inicializaci retezce a inicializaci tokenu
int stringInit(string* s);
// funkce pro rozsireni daneho retezce o znak
int addChar(string* s,char c);
// funkce pro otevreni zdrojoveho souboru
void loadSource(char* path);

// funkce vracejici token (podle stavu konecneho automatu)
Ttoken* getNextToken();
// funkce pro pruchod tabulkou klicovych a rezervovanych slov pro identifikator
TokenType najdiKlucoveSlovo(string* s);
// funkce pro pruchod tabulkou klicovych a rezervovanych slov pro plne_kvalifikovany_identifikator
TokenType najdiKlucoveSlovo2(string* s);

// funkce pro ziskani textove reprezentace typu tokenu
char *getTokenName(TokenType type);

#endif // include guard
