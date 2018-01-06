/*
 * Nazev: Proj2 - iteracni vypocty
 * Autor: Marek Sipos (187044 xsipos03)
 * Popis: Program implementuje vypocet
 *        logaritmu dvema zpusoby - pres
 *        tayloruv polynom a zretezenym
 *        zlomkem. Program take dokaze
 *        spravne vyhledat korektni pocet
 *        iteraci pro pozadovanou presnost
 *        logaritmu.
 */

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funkce pro vypsani napovedy --help
void napoveda()
{
    printf("* Nazev: Proj2 - iteracni vypocty\n");
    printf("* Autor: Marek Sipos (187044 xsipos03)\n");
    printf("* Popis: Program implementuje vypocet\n");
    printf("*        logaritmu dvema zpusoby - pres\n");
    printf("*        tayloruv polynom a zretezenym\n");
    printf("*        zlomkem. Program take dokaze\n");
    printf("*        spravne vyhledat korektni pocet\n");
    printf("*        iteraci pro pozadovanou presnost\n");
    printf("*        logaritmu.\n");
    printf("-----------------------------------------\n");
    printf("| Pouziti:\n");
    printf("|   --log X N\n");
    printf("|       X - cislo pro vypocet prirozeneho logaritmu\n");
    printf("|       N - pocet iteraci\n");
    printf("|   --iter MIN MAX EPS\n");
    printf("|       MIN - minimum intervalu\n");
    printf("|       MAX - maximum intervalu\n");
    printf("|       EPS - presnost vypoctu (EPS >= 1e-12)\n");
    return;
}

// Funkce pro implementaci absolutni hodnoty
double my_abs(double cislo)
{
    if (cislo < 0)
        return -cislo;
    else
        return cislo;
}

// Funkce pro implementaci umocneni cisla - pouze nezaporny cely exponent
double my_pow(double cislo, unsigned int exponent)
{
    double vysledek = 1;
    if (exponent == 0)
        vysledek = 1;
    else
    {
        for (unsigned int i = 1; i <= exponent; i++)
            vysledek *= cislo;
    }
    return vysledek;
}

// Logaritmus vypocteny taylorovym polynomem
double taylor_log(double x, unsigned int n)
{
    double vysledek = 0;
    if (x <= 1.0)
    {
        double tempx = 1-x; // Protoze vzorecek je pro log(1-x) tak se to musi ve vzorci prevratit
        double aktualniClen = tempx;
        for (unsigned int i = 1; i <= n; i++)
        {
            vysledek -= aktualniClen;
            aktualniClen = (aktualniClen*tempx*i)/(i+1);
        }
    }
    else
    {
        double aktualniClen = (x-1)/x;
        for (unsigned int i = 1; i <= n; i++)
        {
            vysledek += aktualniClen;
            aktualniClen = (aktualniClen*i*((x-1)/x))/(i+1);
        }
    }
    return vysledek;
}

// Logaritmus vypocteny zretezenym zlomkem
double cfrac_log(double x, unsigned int n)
{
    double vysledek;
    double tempx = -(1-x)/(1+x); // Prevracime podobne jako v taylor_log
    if (x == 1)
        vysledek = 0;
    else if (n == 1)
        vysledek = (2*tempx);
    else
    {
        vysledek = 0;
        for (unsigned int i = n; i > 0; i--)
        {
            if (i > 1)
                vysledek = (-1*my_pow(i-1, 2)*my_pow(tempx, 2))/((i*2)-1+vysledek);
            else
                vysledek = (2*tempx)/((i*2)-1+vysledek);
        }
    }
    return vysledek;
}

// Funkce pro zjisteni potrebnych poctu iteraci pro danou presnost - 0 = zretezeny zlomek, 1 = tayloruv polynom
int zjistiIteraci(double presnost, double min, double max, int typ)
{
    int vysledek = 0;
    // Ziskame presne matematicke hodnoty pres knihovnu
    double knihovnaMin = log(min);
    double knihovnaMax = log(max);
    // Vnejsi iterace - pridava upresnujici iterace jednu po druhe
    for (unsigned int i = 1; ; i++)
    {
        vysledek++; // Pridame upresnujici iteraci pro funkce na logaritmy
        double mojeMin;
        double mojeMax;

        // Ziskame vlastni matematicke hodnoty pres funkce
        if (typ == 1)
        {
            mojeMin = taylor_log(min, i);
            mojeMax = taylor_log(max, i);
        }
        else
        {
            mojeMin = cfrac_log(min, i);
            mojeMax = cfrac_log(max, i);
        }
        // Nyni porovname
        if (my_abs(knihovnaMin - mojeMin) <= presnost && my_abs(knihovnaMax - mojeMax) <= presnost)
            break;
    }
    return vysledek;
}

int main(int argc, char *argv[])
{
    // Nezadal zadne argumenty
    if (argc < 2)
    {
        fprintf(stderr, "Neplatne nebo chybejici argumenty, pouzij --help pro napovedu\n");
        return 1;
    }

    // Volani napovedy
    if (!strcmp(argv[1], "--help"))
        napoveda();

    // Vypocet prirozeneho logaritmu
    else if (!strcmp(argv[1], "--log"))
    {
        if (argc < 4)
        {
            fprintf(stderr, "Neplatne nebo chybejici argumenty, pouzij --help pro napovedu\n");
            return 1;
        }

        double cislo = strtod(argv[2], NULL);
        if (errno == ERANGE)
        {
            fprintf(stderr, "Zadany argument je prilis velky\n");
            return 2;
        }
        unsigned int iterace = strtol(argv[3], NULL, 10);
        if (errno == ERANGE)
        {
            fprintf(stderr, "Zadany argument je prilis velky\n");
            return 2;
        }
        if (cislo <= 0)
        {
            fprintf(stderr, "Cislo pro vyhledani logaritmu je neplatne\n");
            return 3;
        }
        if (iterace < 1)
        {
            fprintf(stderr, "Iterace nemuze byt mensi nez 1\n");
            return 4;
        }
        printf("       log(%.4g) = %.12g\n", cislo, log(cislo));
        printf("    cf_log(%.4g) = %.12g\n", cislo, cfrac_log(cislo, iterace));
        printf("taylor_log(%.4g) = %.12g\n", cislo, taylor_log(cislo, iterace));
    }

    // Hledani pozadovaneho poctu iteraci
    else if (!strcmp(argv[1], "--iter"))
    {
        // Osetreni chybovych stavu
        if (argc < 5)
        {
            fprintf(stderr, "Neplatne nebo chybejici argumenty, pouzij --help pro napovedu\n");
            return 1;
        }
        double min = strtod(argv[2], NULL);
        if (errno == ERANGE)
        {
            fprintf(stderr, "Zadany argument je prilis velky\n");
            return 2;
        }
        double max = strtod(argv[3], NULL);
        if (errno == ERANGE)
        {
            fprintf(stderr, "Zadany argument je prilis velky\n");
            return 2;
        }
        double eps = strtod(argv[4], NULL);
        if (errno == ERANGE)
        {
            fprintf(stderr, "Zadany argument je prilis velky\n");
            return 2;
        }
        if (min <= 0 || max <= 0)
        {
            fprintf(stderr, "Cislo pro vyhledani logaritmu je neplatne\n");
            return 3;
        }
        if (eps < 1e-12*0.5) // *0.5 kvuli presnosti vypoctu u double
        {
            fprintf(stderr, "Neplatna presnost EPS nebo mimo rozsah\n");
            return 5;
        }
        // Zjistime pozadovanou hodnoty
        int cfracIteraci = zjistiIteraci(eps, min, max, 0);
        int taylorIteraci = zjistiIteraci(eps, min, max, 1);
        // Vypise vysledky dle specifikace
        printf("log(%.4g) = %.12g\n", min, log(min));
        printf("log(%.4g) = %.12g\n", max, log(max));
        printf("continued fraction iterations = %d\n", cfracIteraci);
        printf("cf_log(%.4g) = %.12g\n", min, cfrac_log(min, cfracIteraci));
        printf("cf_log(%.4g) = %.12g\n", max, cfrac_log(max, cfracIteraci));
        printf("taylor polynomial iterations = %d\n", taylorIteraci);
        printf("taylor_log(%.4g) = %.12g\n", min, taylor_log(min, taylorIteraci));
        printf("taylor_log(%.4g) = %.12g\n", max, taylor_log(max, taylorIteraci));
    }

    // Neodpovida zadani
    else
        {
            fprintf(stderr, "Neplatne nebo chybejici argumenty, pouzij --help pro napovedu\n");
            return 1;
        }

    return 0;
}
