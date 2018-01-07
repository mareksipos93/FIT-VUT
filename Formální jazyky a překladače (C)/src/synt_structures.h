/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef SYNT_STRUCTURES_H
#define SYNT_STRUCTURES_H

// INFO:
// This file exists to break Circular dependency within collections.h and synt_anal.h
// Refer to https://en.wikipedia.org/wiki/Circular_dependency for more details

#include <stdbool.h>
#include "lex_anal.h"

#define FOREACH_NTTYPE(NTTYPE)          \
    NTTYPE(NT_PROGRAM)                  \
    NTTYPE(NT_TRIDA)                    \
    NTTYPE(NT_SEZNAM_DEFINIC_STATIC)    \
    NTTYPE(NT_DEFINICE_STATIC)          \
    NTTYPE(NT_DEFINICE_PROMENNA)        \
    NTTYPE(NT_DEF_PROM_KONEC)           \
    NTTYPE(NT_DEFINICE_FUNKCE)          \
    NTTYPE(NT_SEZNAM_PARAMETRU)         \
    NTTYPE(NT_PARAMETR_PRVNI)           \
    NTTYPE(NT_PARAMETR_DALSI)           \
    NTTYPE(NT_SEZNAM_VSTUPU)            \
    NTTYPE(NT_VSTUP_DALSI)              \
    NTTYPE(NT_VSTUP_KONEC)              \
    NTTYPE(NT_SLOZENY_PRIKAZ)           \
    NTTYPE(NT_BLOK_PRIKAZU)             \
    NTTYPE(NT_PRIKAZ)                   \
    NTTYPE(NT_POUZITI)                  \
    NTTYPE(NT_VOLANI_FUNKCE)            \
    NTTYPE(NT_NAVRAT_KONEC)             \
    NTTYPE(NT_PRIRAZENI)                \
    NTTYPE(NT_PRAVA_STRANA)             \
    NTTYPE(NT_DATOVY_TYP)               \
    NTTYPE(NT_DOLLAR)

#define FOREACH_TTYPE(TTYPE)                                                                                                    \
    TTYPE(T_IDENT)      /* identificator */                                                                                     \
    TTYPE(T_FIDENT)     /* function identificator */                                                                            \
    TTYPE(T_CLASS)      /* keyword CLASS */                                                                                     \
    TTYPE(T_STATIC)     /* keyword STATIC */                                                                                    \
    TTYPE(T_RETURN)     /* keyword RETURN */                                                                                    \
    TTYPE(T_IF)         /* keyword IF */                                                                                        \
    TTYPE(T_ELSE)       /* keyword ELSE */                                                                                      \
    TTYPE(T_WHILE)      /* keyword WHILE */                                                                                     \
    TTYPE(T_VOID)       /* keyword VOID */                                                                                      \
    TTYPE(T_TYPE)       /* int, double, String */                                                                               \
    TTYPE(T_EXPRESSION) /* any kind of expr - at least 2 tokens (num, var id, string, parenthesis, aritm + rel operators) */    \
    TTYPE(T_LCB)        /* { */                                                                                                 \
    TTYPE(T_RCB)        /* } */                                                                                                 \
    TTYPE(T_LRB)        /* ( */                                                                                                 \
    TTYPE(T_RRB)        /* ) */                                                                                                 \
    TTYPE(T_SC)         /* ; */                                                                                                 \
    TTYPE(T_COMMA)      /* , */                                                                                                 \
    TTYPE(T_ASSIGN)     /* = */                                                                                                 \
    TTYPE(T_EOF)        /* EOF */                                                                                               \
    TTYPE(T_UNKNOWN)    /* some other not known terminal */

#define FOREACH_PTYPE(PTYPE)        \
    PTYPE(PS_VALUE)      /* i */    \
    PTYPE(PS_ESYS)       /* ESYS */ \
    PTYPE(PS_LRB)        /* ( */    \
    PTYPE(PS_RRB)        /* ) */    \
    PTYPE(PS_PLUS)       /* + */    \
    PTYPE(PS_MINUS)      /* - */    \
    PTYPE(PS_STAR)       /* * */    \
    PTYPE(PS_SLASH)      /* / */    \
    PTYPE(PS_LTHAN)      /* < */    \
    PTYPE(PS_RTHAN)      /* > */    \
    PTYPE(PS_LTHANEQ)    /* <= */   \
    PTYPE(PS_RTHANEQ)    /* >= */   \
    PTYPE(PS_EQ)         /* == */   \
    PTYPE(PS_NEQ)        /* != */   \
    PTYPE(PS_LSYS)       /* LSYS */ \
    PTYPE(PS_RSYS)       /* RSYS */ \
    PTYPE(PS_DOLLAR)     /* $ */

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

/* List of possible non-terminals */
typedef enum {
    FOREACH_NTTYPE(GENERATE_ENUM)
}NTType;

/* Get text representation of enum NTType */
static char *NTType_string[] = {
    FOREACH_NTTYPE(GENERATE_STRING)
};

/* List of possible terminals (excluding lex_error and expression symbols) */
typedef enum {
    FOREACH_TTYPE(GENERATE_ENUM)
}TType;

/* Get text representation of enum TType */
static char *TType_string[] = {
    FOREACH_TTYPE(GENERATE_STRING)
};

/* List of possible precedence symbols */
typedef enum {
    FOREACH_PTYPE(GENERATE_ENUM)
}PType;

/* Get text representation of enum PType */
static char *PType_string[] = {
    FOREACH_PTYPE(GENERATE_STRING)
};

/* Terminal structure */
typedef struct {
    TType type; // Type of this terminal
    Ttoken *token; // Associated token (can be NULL if it was made artificially)
    char data; // I/D/S/B/-
}Terminal;

/* Precedence symbol structure */
typedef struct {
    PType type;
    char data; // I/D/S/B/-
    Ttoken *token; // Associated token (can be NULL if it was made artificially)
    char *temp_var; // Temp var where this token is stored
}Psymbol;

/* FEW FRAME */
typedef struct {
    char type; // F / E / W
    int value;
}FEW;

#endif // include guard
