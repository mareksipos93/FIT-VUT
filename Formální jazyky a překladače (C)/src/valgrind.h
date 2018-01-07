/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef VALGRIND_H
#define VALGRIND_H

#include <stdlib.h>
/*
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC 1

#include <Crtdbg.h>
#undef malloc
#define malloc(size) _malloc_dbg(size,_CLIENT_BLOCK,__FILE__,__LINE__)
#undef free
#define free(addr) _free_dbg(addr,_CLIENT_BLOCK)
#endif
*/
void valgrind();

#endif // !VALGRIND_H

