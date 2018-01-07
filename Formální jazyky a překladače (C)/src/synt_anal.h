/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#ifndef SYNT_ANAL_H
#define SYNT_ANAL_H

#define TOKEN_DEBUG 0   // Debug messages for loaded tokens
#define SYNT_DEBUG 0    // Debug messages for syntax analysis
#define PREC_DEBUG 0    // Debug messages for precedence analysis
#define SEM_DEBUG 0     // Debug messsges for semantic analysis
#define GEN_DEBUG 0     // Debug messages for generator

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "collections.h"
#include "error.h"
#include "tstree.h"
#include "ial.h"
#include "strings.h"
#include "testing.h"
#include "instructions.h"
#include "interpret.h"

extern tInstrList instr_list; // Main instruction List
extern labelAdress *lbladdr; // Label table
extern cQueue token_archive; // Queue to store pre-used tokens
extern cStack stack; // Main stack for syntax analysis to hold terminals, non-terminals & prec. symbols
extern Ttoken *token_list; // Token list, used for second pass
extern bool first_analysis; // Whether this is first pass (used for static declarations)
extern char *current_class; // Actual class
extern char *current_func; // Actual function
extern char *expr_temp_last; // In which temp var is expression from last prec. analysis saved
extern char *expr_left_bool; // Last left boolean operand
extern Instructions expr_last_bool; // Last boolean operation
extern Instructions expr_last_bool_neg; // Last boolean operation negated
extern char *expr_right_bool; // Last right boolean operand

/* Get appropriate rule number from TT table - based on non-terminal and terminal */
int getRuleNumber(NTType nt, TType t);

/* Apply rule identified by number to stack (= expand non-terminal) */
void applyRule(int rule, cStack *stack);

/* Get next terminal (from getNextToken() OR token archive) */
Terminal getNextTerminal();

/* Get appropriate rule number from TT table - based on non-terminal and terminal */
char getPrecedenceOperation(PType top, PType input);

/* Get next precedence symbol (from getNextToken() OR token archive) */
Psymbol getNextPrecSymbol();

/* Auxiliary function to push terminal on stack (token will be NULL) */
void push_cstack_terminal(TType type, cStack *stack_p, char data);

/* Auxiliary function to push non-terminal on stack */
void push_cstack_nonterminal(NTType type, cStack *stack_p);

/* Auxiliary function to push precedence symbol on stack */
void push_cstack_psymbol(PType type, cStack *stack_p, char data, char *temp_var, Ttoken *token);

/* Auxiliary function to push FEW on stack */
void push_cstack_few(char type, cStack *stack_p, int value);

/* Auxiliary function to insert token into queue */
void insert_cqueue_token(Ttoken *token, cQueue *queue);

/* Run precedence analysis (returns type of last E symbol) */
char prec_analysis(Ttoken *token);

/* Load next token (source is determined by first_analysis variable) */
Ttoken *load_next_token();

/* Run syntax analysis */
void execute();


////////////////////
///// SEMANTIC /////
////////////////////

/* Gets declared class */
TsTree get_declared_class(char *name);
/* Gets declared function
   Name can be full (p_class ignored) or short (p_class needed) */
TsTree get_declared_function(char *name, char *p_class);
/* Gets declared function (HashTable version)
   Name can be full (p_class ignored) or short (p_class needed) */
HashTable get_declared_function_ht(char *name, char *p_class);
/* Gets declared variable
   Name can be full (args ignored) or short (class or both needed)
   In case of short name, function (short or long, if provided) is checked before class */
HashTable get_declared_variable(char *name, char *p_class, char *p_function);

/* Gets resulting type of operation with two operators
   Accepts types I/D/S/B
   Returns types I/D/S/B/E (error) */
char get_result_type(char first, char second, PType op);

/* Checks, whether types of left & right sides are compatible */
bool are_type_compatible(char left, char right);

/////////////////////
///// GENERATOR /////
/////////////////////

/* Creates and adds instruction on instruction tape (list) */
tInstrListItem *add_instruction(Instructions instr, char type1, char *value1, char type2, char *value2, char type3, char *value3);

/* Get free or free used temp variable */
char *manage_temp_var(char get, char *free);

#endif  // include guard
