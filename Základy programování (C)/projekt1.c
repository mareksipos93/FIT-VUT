/*
 * Nazev: PROJ #1 - Prace s textem
 * Autor: Marek Sipos (187044 xsipos03)
 * Popis: Program prijima a zpracovava vstupni text ze
 *        standartniho vstupu stdin. Je schopen rozeznat
 *        nezaporne cele cislo, datum a dalsi vstupy.
 *        Mezi sekundarni funkce patri rozpoznani prvocisla,
 *        zjisteni dne v tydnu a detekce palindromu.
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Funkce pro vypsani napovedy
void napoveda()
{
    printf("  * O PROGRAMU:\n");
    printf("  * Nazev: PROJ #1 - Prace s textem\n");
    printf("  * Autor: Marek Sipos (187044 xsipos03)\n");
    printf("  * Popis: Program prijima a zpracovava vstupni text ze\n");
    printf("  *        standartniho vstupu stdin. Je schopen rozeznat\n");
    printf("  *        nezaporne cele cislo, datum a dalsi vstupy.\n");
    printf("  *        Mezi sekundarni funkce patri rozpoznani prvocisla,\n");
    printf("  *        zjisteni dne v tydnu a detekce palindromu.\n");
}

// Funkce pro zjisteni poctu tisknutelnych znaku v poli (rozsah 33-126)
int delkaPole (char retezec[])
{
    int vysledek = 0;
    for (int i = 0; retezec[i] != '\0'; i++) // Dulezite aby byla iterace spravna
    {
        if ((int)retezec[i] >= 33 && (int)retezec[i] <= 126)
            vysledek++;
    }
    return vysledek;
}

// Funkce prijme pole znaku, da pryc pocatecni nuly, a pote jej ulozi do jineho pole znaku na ktery ukazuje vstupni ukazatel
void bezNulNaZacatku (char retezec[], char *vystup)
{
    int index = 0;
    int delka = delkaPole(retezec);
    bool uklada = false; // Jak prochazi napr 000000055 tak jestli uz narazil na ty petky
    for (int i = 0; i < delka; i++)
    {
        if ((int)retezec[i] != 48)
            uklada = true;
        if (uklada == true)
        {
            vystup[index] = retezec[i];
            index++;
        }
    }
    // Ukoncovaci znak
    vystup[index] = '\0'; // Neni tam index+1 protoze se pri minulem ulozeni sam zvysil
    return;
}

// Funkce vezme retezec ve tvaru XXXX-XX-XX a vrati den v tydnu
int vratDenVTydnu (char retezec[], char *vystup)
{
    // Extrakce roku
    char rokPole[] = {retezec[0], retezec[1], retezec[2], retezec[3], '\0'};
    int rok;
    sscanf(rokPole, "%d", &rok);
    // Extrakce mesice
    char mesicPole[] = {retezec[5], retezec[6], '\0'};
    int mesic;
    sscanf(mesicPole, "%d", &mesic);
    // Extrakce dne
    char denPole[] = {retezec[8], retezec[9], '\0'};
    int den;
    sscanf(denPole, "%d", &den);
    // Prace se strukturou
    struct tm * datum; // Vychozi struktura pro mktime()
    time_t tempDatum;
    time(&tempDatum); // DULEZITE: Ziska lokalni cas pro spravne nastaveni
    datum = localtime(&tempDatum);
    datum->tm_year = rok - 1900;
    datum->tm_mon = mesic - 1;
    datum->tm_mday = den;
    // Zpracovani
    if (mktime(datum) == -1)
        return 1;
    else
    {
        strftime(vystup, sizeof(vystup), "%a", datum);
        return 0;
    }
}

// Funkce pro verifikaci korektniho vstupu tisknutelnych znaku
bool jeKorektni (char retezec[])
{
    bool vysledek = true;
    int delka = delkaPole(retezec);
    for (int i = 0; i < delka; i++)
    {
        if ((int)retezec[i] < 32 || (int)retezec[i] > 126)
        {
            vysledek = false;
            break;
        }
    }
    return vysledek;
}

// Funkce pro overeni, zda je retezec slozen z cislic (rozsah 48-57)
bool jeCislo (char retezec[])
{
    bool vysledek = true;
    int delka = delkaPole(retezec);
    for (int i = 0; i < delka; i++)
    {
        if (!isdigit(retezec[i]))
        {
            vysledek = false;
            break;
        }
    }
    return vysledek;
}

// Funkce pro zjisteni prvocisla
bool jePrvocislo (long cislo)
{
    bool vysledek;
    if (cislo > 1 && cislo <= INT_MAX)
        {
            vysledek = true;
            for (int test = 2; test < (cislo/2)+1; test++)
            {
                if (cislo % test == 0)
                {
                    vysledek = false;
                    break;
                }
            }
        }
    else
        vysledek = false;
    return vysledek;
}

// Funkce pro zjisteni palindromu
bool jePalindrom (char retezec[])
{
    bool vysledek = true;
    int delka = delkaPole(retezec);
    for (int i = 0; i < delka; i++)
    {
        if ((int)retezec[i] != (int)retezec[delka-1-i])
        {
            vysledek = false;
            break;
        }
    }
    return vysledek;
}

// Funkce pro detekci formatu data XXXX-XX-XX
bool jeDatum (char retezec[])
{
    bool vysledek = true;
    int delka = delkaPole(retezec);
    if (delka != 10)
        vysledek = false;
    else
    {
        for (int i = 0; i < delka; i++)
        {
            if (i == 4 || i == 7)
            {
                if (retezec[i] != '-')
                {
                    vysledek = false;
                    break;
                }
            }
            else if (retezec[i] < '0' || retezec[i] > '9')
            {
                vysledek = false;
                break;
            }
        }
    }
    // Kontrola rozsahu dne a mesice
    if (vysledek == true)
    {
        // Kontrola dne 01-31
        if ((retezec[8] > '3') || (retezec[8] == '3' && retezec[9] > '1') || (retezec[8] == '0' && retezec[9] == '0'))
            vysledek = false;
        // Kontrola mesice 01-12
        else if ((retezec[5] > '1') || (retezec[5] == '1' && retezec[6] > '2') || (retezec[5] == '0' && retezec[6] == '0'))
            vysledek = false;
    }
    return vysledek;
}

int main(int argc, char *argv[])
{
    (void) argv; // Proti varovani o unused variable

    // Popis programu pri predani argumentu
    if (argc > 1)
    {
        napoveda();
        return 0;
    }

    char vstupSlovo[101]; // Zde se uklada vstup (max 100, +1 pro nulovy znak)

    // Hlavni iteracni vstup
    while (scanf("%100s", vstupSlovo) != EOF)
    {
        if (!jeKorektni(vstupSlovo))
        {
            fprintf(stderr, "Neplatny vstup\n");
            return 1;
        }
        // Zpracovani cisla (prvocislo)
        if (jeCislo(vstupSlovo))
        {
            long vstupCislo; // Zde se uklada cislo
            vstupCislo = strtol(vstupSlovo, NULL, 10);
            if (errno == ERANGE) // Pokud cislo preteklo
            {
                /* Cislo preteklo, vypise ho jako text, netreba konverze na cislo, je mimo rozsah
                 * I tak se potrebujeme zbavit pocatecnich nul na vystupu. */
                char vstupBezNul[delkaPole(vstupSlovo)+1];
                bezNulNaZacatku(vstupSlovo, vstupBezNul);
                printf("number: %s\n", vstupBezNul);
            }
            else if (jePrvocislo(vstupCislo))
                printf("number: %ld (prime)\n", vstupCislo);
            else
                printf("number: %ld\n", vstupCislo);
        }
        else if (jeDatum(vstupSlovo)) // Zpracovani data (den v tydnu)
        {
            char den[4];
            if (vratDenVTydnu(vstupSlovo, den) == 1)
            {
                fprintf(stderr, "Neznama chyba v datu\n");
                return 1;
            }
            else
                printf("date: %s %s\n", den, vstupSlovo);
        }
        else // Zpracovani retezce (palindrom)
        {
            if (jePalindrom(vstupSlovo))
              printf("word: %s (palindrome)\n", vstupSlovo);
            else
              printf("word: %s\n", vstupSlovo);
        }
    }

    return 0;
}
