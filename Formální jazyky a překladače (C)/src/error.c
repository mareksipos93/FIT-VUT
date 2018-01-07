/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/


#include "error.h"


void error(int chyba)
{
	switch(chyba)
	{
		case ERR_OK:
			break;
		case ERR_LEX:
			fprintf(stderr,"%d - Chyba v programe v ramci lexikalnej analyzy (chybna struktura aktualneho lexemu).\n",chyba);
			break;
		case ERR_SYNT:
			fprintf(stderr,"%d - Chyba v programe v ramci syntaktickej analyzy (chybna syntax struktury programu).\n", chyba);
			break;
		case ERR_SEM_DEF:
			fprintf(stderr,"%d - Semanticka chyba - nedefinovana funkcia/premenna, pokus o redefinovanie funkcie/premennej. \n", chyba);
			break;
		case ERR_SEM_TYPE:
			fprintf(stderr,"%d - Semanticka chyba typovej kompatibility v aritmeticktych, retazcovych a relacnych vyrazoch, pripadne nespravny pocet ci typ parametrov pri volani funkcie.\n", chyba);
			break;
		case ERR_SEM_OTHER:
			fprintf(stderr,"%d - Ostatne semanticke chyby.\n", chyba);
			break;
		case ERR_RUN_NUM:
			fprintf(stderr,"%d - Behova chyba pri nacitani ciselnej hodnoty zo vstupu.\n", chyba);
			break;
		case ERR_RUN_INIT:
			fprintf(stderr,"%d - Behova chyba pri praci s neinicializovanou premennou.\n", chyba);
			break;
		case ERR_RUN_DEV:
			fprintf(stderr,"%d - Behova chyba delenie nulou.\n", chyba);
			break;
		case ERR_RUN_OTHER:
			fprintf(stderr,"%d - Ostatne behove chyby.\n", chyba);
			break;
		case ERR_INTER:
			fprintf(stderr,"%d - Interna chyba interpretu tj. neovplyvnitelna vstupnym programom (napr. chyba alokacie pamati, chyba pri otvarani suboru s riadiacim programom, nespravne parametre prikazoveho riadku).\n", chyba);
			break;
	}
	if (chyba != ERR_OK)
		exit(chyba);
}
