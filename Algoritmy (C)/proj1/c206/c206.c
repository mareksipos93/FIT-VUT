	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
    /* Mazat prvky musime jen pokud tam nejake jsou */
    if (L->Last != NULL) {
        /* Iterujeme prvky a postupne je uvolnujeme */
        tDLElemPtr processed = L->Last;
        do {
            tDLElemPtr prev = processed->lptr;
            free(processed);
            processed = prev;
        } while(processed != NULL);
    }
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    /* Vytvorime novy ukazatel na strukturu tDLElem s dostatkem mista */
    tDLElemPtr new = malloc(sizeof(struct tDLElem));
    if (new == NULL) {
        DLError();
        return;
    }
    /* Seznam byl doted prazdny */
    if (L->First == NULL) {
        new->lptr = NULL; // Nove vlozeny prvek je nejvic vlevo
        new->rptr = NULL; // Za nove vlozenym prvkem nic neni
        L->Last = new; // Seznam byl prazdny, takze novy prvek je i posledni
    /* Seznam nebyl prazdny */
    } else {
        L->First->lptr = new; // Nyni druhy prvek ukazuje na nove vlozeny prvni
        new->lptr = NULL; // Nove vlozeny prvek je nejvic vlevo, nic nalevo uz neni
        new->rptr = L->First; // Nove vlozeny prvek ukazuje na sveho nasledovnika
    }
    new->data = val; // Priradime informaci o hodnote novemu prvku
    L->First = new; // Urcime nas nove vytvoreny prvek jako prvni v seznamu
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    /* Vytvorime novy ukazatel na strukturu tDLElem s dostatkem mista */
    tDLElemPtr new = malloc(sizeof(struct tDLElem));
    if (new == NULL) {
        DLError();
        return;
    }
    /* Seznam byl doted prazdny */
    if (L->First == NULL) {
        new->lptr = NULL; // Nove vlozeny prvek je nejvic vlevo
        new->rptr = NULL; // Za nove vlozenym prvkem nic neni
        L->First = new; // Seznam byl prazdny, takze novy prvek je tedy prvni
    /* Seznam nebyl prazdny */
    } else {
        L->Last->rptr = new; // Nyni predposledni prvek ukazuje na nove vlozeny posledni
        new->rptr = NULL; // Nove vlozeny prvek je nejvic vpravo, nic napravo uz neni
        new->lptr = L->Last; // Nove vlozeny prvek ukazuje na sveho predchudce
    }
    new->data = val; // Priradime informaci o hodnote novemu prvku
    L->Last = new; // Urcime nas nove vytvoreny prvek jako posledni v seznamu
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First; // Nastavit aktivni na prvni
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last; // Nastavit aktivni na posledni
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    /* Prazdny seznam = chyba */
    if (L->First == NULL) {
        DLError();
        return;
    } else {
        *val = L->First->data; // Pres ukazatel ulozime datovou hodnotu */
    }
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    /* Prazdny seznam = chyba */
    if (L->First == NULL) {
        DLError();
        return;
    } else {
        *val = L->Last->data; // Pres ukazatel ulozime datovou hodnotu */
    }
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if (L->First != NULL) {
        tDLElemPtr next = L->First->rptr; // Ziskame druhy prvek, klidne to muze byt NULL
        /* Pokud byl prvni prvek aktivni, tak se aktivita ztrati */
        if (L->Act == L->First) {
            L->Act = NULL;
        }
        free(L->First); // Uvolnime prvni prvek
        /* Za prvnim prvkem uz nic nebylo, nyni je seznam prazdny */
        if (next == NULL) {
            L->First = NULL;
            L->Last = NULL;
        }
        /* Druhy prvek bude nyni prvni */
        else {
            next->lptr = NULL; // Tento prvek je nyni nejvic vlevo
            L->First = next;
        }
    }
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if (L->First != NULL) {
        tDLElemPtr prev = L->Last->lptr; // Ziskame predposledni prvek, klidne to muze byt NULL
        /* Pokud byl posledni prvek aktivni, tak se aktivita ztrati */
        if (L->Act == L->Last) {
            L->Act = NULL;
        }
        free(L->Last); // Uvolnime posledni prvek
        /* Pred poslednim prvkem uz nic nebylo, nyni je seznam prazdny */
        if (prev == NULL) {
            L->First = NULL;
            L->Last = NULL;
        }
        /* Predposledni prvek nyni bude posledni */
        else {
            prev->rptr = NULL; // Tento prvek je nyni nejvic vpravo
            L->Last = prev;
        }
    }
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if (L->Act != NULL && L->Act->rptr != NULL) { // Jednoduche zpracovani definovanych podminek
        tDLElemPtr next = L->Act->rptr; // Ziskame prvek za aktivnim
        /* Za mazanym prvkem jeste neco je */
        if (next->rptr != NULL) {
            L->Act->rptr = next->rptr; // Propojime aktivni s tim za mazanym
            next->rptr->lptr = L->Act; // Propojime prvek za mazanym s aktivnim
        }
        /* Za mazanym prvkem uz nic neni */
        else {
            L->Act->rptr = NULL; // Aktivni je nyni nejvice vpravo
            L->Last = L->Act; // Aktivni je tedy nyni poslednim prvkem
        }
        free(next); // Muzeme bez obav uvolnit
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    if (L->Act != NULL && L->Act->lptr != NULL) { // Jednoduche zpracovani definovanych podminek
        tDLElemPtr prev = L->Act->lptr; // Ziskame prvek pred aktivnim
        /* Pred mazanym prvkem jeste neco je */
        if (prev->lptr != NULL) {
            L->Act->lptr = prev->lptr; // Propojime aktivni s tim pred mazanym
            prev->lptr->rptr = L->Act; // Propojime prvek pred mazanym s aktivnim
        }
        /* Pred mazanym prvkem uz nic neni */
        else {
            L->Act->lptr = NULL; // Aktivni je nyni nejvice vlevo
            L->First = L->Act; // Aktivni je tedy nyni prvnim prvkem
        }
        free(prev); // Muzeme bez obav uvolnit
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act != NULL) { // Ignorujeme neaktivni seznam
        tDLElemPtr new = malloc(sizeof(struct tDLElem)); // Misto pro novy prvek
        /* Malloc se nezdaril */
        if (new == NULL) {
            DLError();
            return;
        /* Vse je pripraveno */
        } else {
            new->data = val; // Priradime datovou hodnotu
            new->lptr = L->Act; // Novy prvek zleva ukazuje na aktivni
            /* Za aktivnim nic nebylo, tedy byl aktivni posledni */
            if (L->Act->rptr == NULL) {
                new->rptr = NULL; // Novy je uplne vpravo
                L->Act->rptr = new; // Aktivni zprava ukazuje na novy
                L->Last = new; // Novy je tedy uplne posledni
            }
            /* Za aktivnim neco bylo */
            else {
                new->rptr = L->Act->rptr; // Novy ukazuje zprava na ten, na ktery ukazoval aktivni
                new->rptr->lptr = new; // Ten prvek vpravo ukazuje zleva na nove vlozeny
                L->Act->rptr = new; // Aktivni zprava ukazuje na novy
            }
        }
    }
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act != NULL) { // Ignorujeme neaktivni seznam
        tDLElemPtr new = malloc(sizeof(struct tDLElem)); // Misto pro novy prvek
        /* Malloc se nezdaril */
        if (new == NULL) {
            DLError();
            return;
            /* Vse je pripraveno */
        } else {
            new->data = val; // Priradime datovou hodnotu
            new->rptr = L->Act; // Novy prvek zprava ukazuje na aktivni
            /* Pred aktivnim nic nebylo, tedy byl aktivni prvni */
            if (L->Act->lptr == NULL) {
                new->lptr = NULL; // Novy je uplne vlevo
                L->Act->lptr = new; // Aktivni zleva ukazuje na novy
                L->First = new; // Novy je tedy uplne prvni
            }
            /* Pred aktivnim neco bylo */
            else {
                new->lptr = L->Act->lptr; // Novy ukazuje zleva na ten, na ktery ukazoval aktivni
                new->lptr->rptr = new; // Ten prvek vlevo ukazuje zprava na nove vlozeny
                L->Act->lptr = new; // Aktivni zleva ukazuje na novy
            }
        }
    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    /* Prazdny seznam = chyba */
    if (L->Act == NULL) {
        DLError();
        return;
    } else {
        *val = L->Act->data; // Pres ukazatel ulozime datovou hodnotu
    }
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if (L->Act != NULL) { // Ignorujeme neaktivni seznam
        L->Act->data = val; // Prepiseme datovou hodnotu aktivniho prvku
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    if (L->Act != NULL) { // Ignorujeme neaktivni seznam
        L->Act = L->Act->rptr; // Posuneme aktivitu vpravo (klidne na NULL)
    }
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
    if (L->Act != NULL) { // Ignorujeme neaktivni seznam
        L->Act = L->Act->lptr; // Posuneme aktivitu vlevo (klidne na NULL)
    }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
    return (L->Act != NULL);
}

/* Konec c206.c*/
