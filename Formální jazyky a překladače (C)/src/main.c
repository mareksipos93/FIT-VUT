/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "valgrind.h"
#include "testing.h"
#include "synt_anal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define PATRIK_TO_NECHCE 0
#define LEX_TEST 0       //Lexical analysis test
#define VALGRIND_TEST 0  //Valgrind test
#define OUTPUT_TIME 0     //Output execution time 


int main(int argc, char* argv[]) {

#if PATRIK_TO_NECHCE != 1
#if OUTPUT_TIME == 1
        clock_t tStart = clock();
    #endif

	// Missing parameter
	if (argc < 2) {
		error(ERR_INTER);
	}
	// Load source file
	loadSource(argv[1]);

    #if LEX_TEST == 1
        /* Lexical analysis test (print all found tokens) */
	    fprintf(stdout, "=== TEST LEXIKALNI ANALYZY ===\n");
	    Ttoken *token = NULL;
	    int num = 0;
	    do {
		    token = getNextToken();
		    fprintf(stdout, "#%-5d Typ: %-11s Radek: %-3d Data: %-18s\n", num, getTokenName(token->type), token->cisloRiadku, token->attr->str);
		    num++;
	    } while (token != NULL && token->type != KONEC_SOUBORU && token->type != LEXIKALNI_CHYBA);
    #elif VALGRIND_TEST == 1
        /* Valgrind test */
        tsTest();
        valgrind();
        printf("\n\nhit enter to end\n");
        getchar();
    #else

        /* Regular syntax analysis */
        execute();


    #endif

    #if OUTPUT_TIME == 1
        printf("OUTPUT_TIME is active, turn off all debugs for accurate results !\n");
        printf("  -> Time taken: %.3fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
    #endif

#else
	// patrikuv pisecek
	/*char *c = "asdcv";
	listMergeSort(c);
	*/


	//testInterpret();
	//valgrind();
	//getchar();

#endif


    return 0;

	
}
