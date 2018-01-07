/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "strings.h"
#include "testing.h"

//vytvori kopii str na nove adrese, tu vraci
char *makeString(char *str) {
	char *s = malloc(strlen(str) + 1);
	strcpy(s, str);
	return s;
}

//spojeni dvou retezcu/znaku
char *cat(char *s1, char *s2) {
	char *str = malloc(strlen(s1) + strlen(s2) + 1);

	strcpy(str, s1);
	strcat(str, s2);
	return str;
}

//TODO Vrati string reprezentaci non-string promenne
char *varToString(Data *var) {
	//char *s;
	switch (var->type) {
		case t_int: 
		//	s = intToString(var->value.v_int);
			break;
		case t_double: 
		//	s = intToString(var->value.v_double);
			break;
		default: //?
			break;
	}

	//printf("conv to %s\n", s);
	return "tohle vubec nefunguje";
}

char *intToString(int x) {
	debug(" [INTTOSTRING] converting\n");
	char *tmp = malloc(50);
	snprintf(tmp,50, "%d", x);
	
	return tmp;
}
char *doubleToString(double x) {
	debug(" [DOUBLETOSTRING] converting %g\n",x);
	char *tmp = malloc(50);
	snprintf(tmp,50,"%g",x);
	return tmp;
}

//vytvori pod retezec od indexu startindex o velikosti size
char *getSubString(char *str, int startindex, int size) {
	return NULL;
}

/* ----------------------------------------------------------------------------------- */
/* ------------------------------FCE COMPARE------------------------------------------ */
// porovna 2 zadane retezce s1 a s2 a vrati
// 0    ,kdyz se rovnaji
// 1    ,kdyz je s1 > s2
// -1   ,kdyz je s1 < s2

int compare(char *s1, char *s2)
{
	return (strcmp(s1, s2));
}
/* ----------------------------------------------------------------------------------- */

char *explodeFullIdentifier(char *id, bool left) {
    char *pos;
    if ((pos = strchr(id, '.')) == NULL) // No dot, no fun :(
        return makeString(id);
    else {
        char *exploded; int length;
        if (left) {
            length = pos - id;
            exploded = (char *) malloc(length * sizeof(char) + 1);
            strncpy(exploded, id, length);
            exploded[length] = '\0';
        }
        else {
            length = strlen(id) - (pos - id) - 1;
            exploded = (char *) malloc(length * sizeof(char) + 1);
            strncpy(exploded, pos+1, length);
            exploded[length] = '\0';
        }
        return exploded;
    }
}
