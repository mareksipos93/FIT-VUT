/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


#include <stdio.h>
#include <stdlib.h>

#define FOREACH_INSTR(INS) \
	INS(I_MOV) \
	INS(I_CALL) \
	INS(I_RET) \
	INS(I_PUSH) \
	INS(I_POP) \
	INS(I_JMP) \
	INS(I_JMPE) \
	INS(I_JMPNE) \
	INS(I_JMPL) \
	INS(I_JMPLE) \
	INS(I_JMPG) \
	INS(I_JMPGE) \
	INS(I_ADD) \
	INS(I_SUB) \
	INS(I_MUL) \
	INS(I_DIV) \
	INS(I_INC) \
	INS(I_DEC) \
	INS(I_READ) \
    INS(I_IREAD) \
    INS(I_DREAD) \
    INS(I_SREAD) \
	INS(I_WRITE) \
	INS(I_LEN)\
	INS(I_SUBS)\
	INS(I_CMP)\
	INS(I_FIND)\
	INS(I_SORT)\
	INS(I_LABEL)


#define GENERATE_ENUM_INSTR(ENUM) ENUM,
#define GENERATE_STRING_INSTR(STRING) #STRING,

static const char *INSTR_STRING[] = {
	FOREACH_INSTR(GENERATE_STRING_INSTR)
};

// Instrunkci sada
typedef enum {
	FOREACH_INSTR(GENERATE_ENUM_INSTR)
}Instructions;

typedef enum {
	c_int, // konstanta int
	c_double, // konstanta double 
	c_string, // konstatna string
	name // identifikator 
}O_Type;

typedef union {
	int v_int;
	double v_double;
	char *v_string;
	char *name;
}O_Value;

typedef struct {
	O_Type type; // typ operandu
	O_Value value; // hodnota pro jednotlivy typ
}Operand;


typedef struct {
	Instructions instr;
	Operand *addr1;
	Operand *addr2;
	Operand *addr3;
}tInstr;

typedef struct instrListItem {
	tInstr instr;
	struct instrListItem *next;
} tInstrListItem;

typedef struct {
	tInstrListItem *first;
	tInstrListItem *last;
	tInstrListItem *active;
} tInstrList;

void instrListInit(tInstrList *list);
void instrListFree(tInstrList *list);

tInstrListItem *instrListAddInstr(tInstrList *list, tInstr instr);

int instrListGetActiveInstr(tInstrList *list, tInstr *instr);
void instrListSetActive(tInstrList *list, tInstrListItem *active);
void instrListSetActiveFirst(tInstrList *list);
void instrListSetActiveNext(tInstrList *list);

//TODO pridat makra pro tvorbu operandu/instrukce


#endif // include guard
