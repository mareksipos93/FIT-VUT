/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef TESTWRITEOUT_H
#define TESTWRITEOUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "valgrind.h"
#include "ial.h"
#include "instructions.h"
#include "lex_anal.h"
#include "synt_anal.h"
#include "error.h"
#include "framework.h"
#include "strings.h"
#include "tstree.h"
#include "instructions.h"
#include "interpret.h"


#define debugFile stdout

//#define debug(...) do{ fprintf( debugFile, __VA_ARGS__ ); } while( false )

#define debug(...) do{ } while ( false )



//#define printf(...) do{ \
//fprintf( debugFile, __VA_ARGS__ ); \
///*fprintf( stdout, __VA_ARGS__ );*/ \
//} while( false )




//ial
void hashWriteOut(HashTable hashTable);
void hashTest();

//tstree
void tsWriteOutTree(TsTree root);
void tsWriteOutTreeTS(TsTree root);
void tsTest();

//interpret
void testInterpret();

//ramce
void testWriteOutFrame(StackFrame *sf);

void testWriteOutInstr(tInstrList list);
void testWriteOutI(tInstr ins);

#endif // ! TESTWRITEOUT_H

