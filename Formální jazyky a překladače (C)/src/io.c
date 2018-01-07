/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "io.h"

/* ------------------------------READ STRING------------------------------------------ */
// ze standartiho vstupu nacte retezec ukonceny koncem radkem, nebo koncem
// vstupu, kdy symbol konce radku, nebo vstupu jiz do nacteneho retezce nepatri
char *readString()
{
    char slovo[STR_MAX];

    // nacitani ze stdin (resp. chyba)
    if (fgets(slovo, STR_MAX, stdin) == NULL)
    {
        fprintf(stderr, "stderr - fgets\n");
        return false;
    }

    // ukazatel na char -> fce vraci ukazatel na char
    char * slovo1;
    // oddeli konec radku od nactene posloupnosti znaku ze stdin
    slovo1 = strtok(slovo, "\n");
    // spocita pocet znaku pro dynamickou alokaci
    int pocet_znaku = strlen(slovo1);

    // ukazatel na char -> fce vraci ukazatel na char
    char *novy_vstup = NULL;
    novy_vstup = (char *) malloc(pocet_znaku*sizeof(char));

    if (novy_vstup == NULL)
    {
        fprintf(stderr, "stderr - malloc\n");
        return false;
    }

    novy_vstup = slovo1;

    // vraci ukazatel na nacteny retezec ze stdin
    return novy_vstup;
}
/* ----------------------------------------------------------------------------------- */
/* ------------------------------READ INT--------------------------------------------- */
// nacte retezec, ktery prevede na cele cislo, nebo vrati chybu, pokud retezec neodpovida
bool readInt(int *out)
{
    // nacteni ze stdin
    char *cislo = readString();

    // nacteni ze stdin
    if (cislo != NULL)
    {
        // zjisteni poctu znaku v retezci
        int pocet_znaku;
        pocet_znaku = strlen(cislo);

        // cyklus pro pruchod retezce
        for (int i = 0;i < pocet_znaku; i++)
        {
            // v pripade chyby retezce vraci false
            if (!isdigit(cislo[i]))
            {
                fprintf(stderr, "stderr int - string is not a int\n");
                return false;
            }

            // v pripade posledniho znaku rovnu cislu
            else if (i == (pocet_znaku - 1))
            {
                // prevod na cislo
                *out = (int) strtol(cislo, NULL, 10);
                return true;
            }
        }

        return false;
    }

    else
    {
        fprintf(stderr, "stderr int - fgets\n");
        return false;
    }
}
/* ----------------------------------------------------------------------------------- */
/* ------------------------------READ DOUBLE------------------------------------------ */
// nacte retezec, ktery prevede na realne cislo, nebo vrati chybu, pokud retezec neodpovida
double readDouble(double *out)
{
    // nacteni ze stdin
    char *cislo = readString();
    int stav = 0;

    int pocet_znaku = strlen(cislo);

    // nacteni ze stdin
    if (cislo != NULL)
    {
        for (int i = 0; i <= pocet_znaku; i++)
        {
            switch (stav)
            {
                case 0:

                    if (isdigit(cislo[i]))
                    {
                        stav = 01;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 01:

                    if (isdigit(cislo[i]))
                    {
                        stav = 01;
                    }

                    else if (cislo[i] == '.')
                    {
                        stav = 20;
                    }

                    else if (cislo[i] == 'e' || cislo[i] == 'E')
                    {
                        stav = 10;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 10:

                    if (cislo[i] == '+' || cislo[i] == '-')
                    {
                        stav = 11;
                    }

                    else if (isdigit(cislo[i]))
                    {
                        stav = 17;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 17:

                    if (isdigit(cislo[i]))
                    {
                        stav = 17;
                    }

                    else if (cislo[i] == '\0')
                    {
						*out = strtod(cislo, NULL);
                        return true;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;


                case 11:

                    if (isdigit(cislo[i]))
                    {
                        stav = 12;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 12:

                    if (isdigit(cislo[i]))
                    {
                        stav = 12;
                    }

                    else if (cislo[i] == '\0')
                    {
						*out = (double) strtod(cislo, NULL);
                        return true;
                    }

                    else
                    {
                        stav = 99;
                    }

                case 20:

                    if (isdigit(cislo[i]))
                    {
                        stav = 22;
                    }

                    else
                    {
                       stav = 99;
                    }

                    break;

                case 22:

                    if (isdigit(cislo[i]))
                    {
                        stav = 22;
                    }

                    else if (cislo[i] == 'e' || cislo[i] == 'E')
                    {
                        stav = 23;
                    }

                    else if (cislo[i] == '\0')
                    {
						*out = (double) strtod(cislo, NULL);
                        return true;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 23:

                    if (cislo[i] == '+' || cislo[i] == '-')
                    {
                        stav = 24;
                    }

                    else if (isdigit(cislo[i]))
                    {
                        stav = 25;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 24:

                    if (isdigit(cislo[i]))
                    {
                        stav = 25;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 25:

                    if (isdigit(cislo[i]))
                    {
                        stav = 25;
                    }

                    else if (cislo[i] == '\0')
                    {
                        *out = (double) strtod(cislo, NULL);
                        return true;
                    }

                    else
                    {
                        stav = 99;
                    }

                    break;

                case 99:

                    return false;
            }
        }
    

	}

    else
    {
        fprintf(stderr, "stderr int - fgets\n");
        return false;
    }
	return false;
}


