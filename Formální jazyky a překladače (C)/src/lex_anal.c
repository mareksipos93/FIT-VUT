/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "lex_anal.h"

/**
 * Klucove slova (tabulka klicovych slov)
 */
const char *klucoveSlova[17] =
{
	"boolean\0", "break\0", "class\0", "continue\0", "do\0", "if\0", "double\0",
	"else\0", "false\0", "for\0", "int\0", "true\0", "while\0", "return\0",
	"String\0", "static\0", "void\0",
};

/**
 * Rezervovane slova (tabulka rezervovanych slov)
 */
const char *rezervovaneSlova[36] =
{
	"abstract\0", "new\0", "switch\0", "assert\0", "default\0", "package\0",
	"synchronized\0", "goto\0", "private\0", "this\0", "implements\0",
	"protected\0", "throw\0", "byte\0", "import\0", "public\0", "throws\0",
	"case\0", "enum\0", "instanceof\0", "transient\0", "catch\0", "extends\0",
	"short\0", "try\0", "char\0", "final\0", "interface\0", "finally\0", "long\0",
	"strictfp\0", "volatile\0", "const\0", "float\0", "native\0", "super\0",
};

// zdrojovy soubor
FILE* subor = NULL;
unsigned int riadok;

// funkce pro otevreni zdrojoveho souboru
void loadSource(char* path) {
	// pokud uz je soubor nacten, radsi nic neudelame
	if (subor != NULL) {
		return;
	}
	// otevreme zdrojovy soubor, pokud se to nepovede -> chyba
	subor = fopen(path, "r");
	if (subor == NULL) {
		error(ERR_INTER);
	}
}

// funkce vracejici token (podle stavu konecneho automatu)
Ttoken* getNextToken()
{
	// Ziskat token pri neotevrenem souboru nelze
	if (subor == NULL) {
		return NULL;
	}

	int c;
	// vytvori a inicializuje token
	Ttoken* token = vytvorToken();

	// dokud neni konec souboru
	while (c = fgetc(subor))
	{
		token->cisloRiadku = riadok;
		// stavy, ktere nasleduji ihned po pocatecnim stave
		switch (token->type)
		{
			// pociatocny stav
			case POC_STAV:

				// znak noveho radku
				if (c == '\n')
				{
					riadok++;
					token->cisloRiadku = riadok;
					token->type = POC_STAV;
				}

				// ignoruje biele znaky
				else if (isspace(c))
				{
					token->type = POC_STAV;
				}

				// hned narazi na koniec suboru
				else if (c == EOF)
				{
					token->type = KONEC_SOUBORU;
				}

				// =, alebo ==
				else if (c == '=')
				{
					token->type = PRIRAZENI;
				}

				// scitani
				else if (c == '+')
				{
					token->type = SCITANI;
					return token;
				}

				// odecitani
				else if (c == '-')
				{
					token->type = ODECITANI;
					return token;
				}

				// nasobeni
				else if (c == '*')
				{
					token->type = NASOBENI;
					return token;
				}

				// kdyz je prvnim znakem '/', nebo deleni
				else if (c == '/')
				{
					token->type = DELENI;
				}

				// <, alebo <=
				else if (c == '<')
				{
					token->type = MENSI;
				}

				// >, >> alebo >=
				else if (c == '>')
				{
					token->type = VETSI;
				}

				// !=
				else if (c == '!')
				{
					token->type = NEROVNO;
				}

				// strednik
				else if (c == ';')
				{
					token->type = STREDNIK;
					return token;
				}

				// carka
				else if (c == ',')
				{
					token->type = CARKA;
					return token;
				}

				// leva kulata zavorka
				else if (c == '(')
				{
					token->type = LEVA_KULATA_ZAVORKA;
					return token;
				}

				// prava kulata zavorka
				else if (c == ')')
				{
					token->type = PRAVA_KULATA_ZAVORKA;
					return token;
				}

				// leva hranata zavorka
				else if (c == '[')
				{
					token->type = LEVA_HRANATA_ZAVORKA;
					return token;
				}

				// prava hranata zavorka
				else if (c == ']')
				{
					token->type = PRAVA_HRANATA_ZAVORKA;
					return token;
				}

				// leva slozena zavorka
				else if (c == '{')
				{
					token->type = LEVA_SLOZENA_ZAVORKA;
					return token;
				}

				// prava slozena zavorka
				else if (c == '}')
				{
					token->type = PRAVA_SLOZENA_ZAVORKA;
					return token;
				}

				//  dvojite uvodzovky - "
				else if (c == 34)
				{
					token->type = RETEZEC;
				}

				// identifikator, alebo klucove slovo
				else if (isalpha(c) || (c == '_') || (c == '$'))
				{
					addChar(token->attr, c);
					token->type = IDENTIFIKATOR;
				}

				// INT, double
				else if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = CELOCISELNY_LITERAL;
				}

				// lexikalni chyba
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

			case PRIRAZENI:

				// kombinace ==
				if (c == '=')
				{
					token->type = ROVNO;
					return token;
				}

				// kombinace = (prirazeni)
				else
				{
					// posunie sa v subore o 1 znak dozadu a vrati =
					fseek(subor, -1, SEEK_CUR);
					token->type = PRIRAZENI;
					return token;
				}

				break;

			case DELENI:

				// kombinace // (jednoradkovy komentar)
				if (c == '/')
				{
					token->type = JEDNORADKOVY_KOMENTAR;
				}

				// kombinace /* (viceradkovy komentar)
				else if (c == '*')
				{
					token->type = VICERADKOVY_KOMENTAR;
				}

				// v pripade jine kombinace vrati chybu (stav LEXIKALNI_CHYBA)
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = DELENI;
					return token;
				}

				break;

			case JEDNORADKOVY_KOMENTAR:

				// koniec komentu
				if (c == '\n')
				{
					riadok++;
					token->cisloRiadku = riadok;
					token->type = POC_STAV;
				}

				// koniec suboru => vrati EOF token
				else if (c == EOF)
				{
					token->type = KONEC_SOUBORU;
				}

				// vsechno ostatni (jakekoliv znaky v komentari)
				else
				{
					token->type = JEDNORADKOVY_KOMENTAR;
				}

				break;

			case VICERADKOVY_KOMENTAR:

				// kombinace /* [neco] *
				if (c == '*')
				{
					token->type = VICERADKOVY_KOMENTAR_2;
				}

				// koniec suboru => neukonceny koment, vrati chybu
				else if (c == EOF)
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				// vsechno ostatni (jakekoliv znaky v komentari)
				else
				{
					if (c == '\n')
					{
						riadok++;
						token->cisloRiadku = riadok;
					}

					token->type = VICERADKOVY_KOMENTAR;
				}

				break;

			case VICERADKOVY_KOMENTAR_2:

				// koniec komentara (kombinace /* [neco] */)
				if (c == '/')
				{
					token->type = POC_STAV;
				}

				// koniec suboru => neukonceny koment, vrati chybu
				else if (c == EOF)
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				// vsechno ostatni (jakekoliv znaky v komentari)
				else
				{
					if (c == '\n')
					{
						riadok++;
						token->cisloRiadku = riadok;
					}

					token->type = VICERADKOVY_KOMENTAR;
				}

				break;

			case MENSI:

				// <=
				if (c == '=')
				{
					token->type = MENSI_NEBO_ROVNO;
					return token;
				}

				// posunie sa v subore o 1 znak dozadu a vrati <
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = MENSI;
					return token;
				}

				break;

			case VETSI:

				// >=
				if (c == '=')
				{
					token->type = VETSI_NEBO_ROVNO;
					return token;
				}

				// posunie sa v subore o 1 znak dozadu a vrati >
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = VETSI;
					return token;
				}

				break;

			case NEROVNO:

				// !=
				if (c == '=')
				{
					token->type = NEROVNO;
					return token;
				}

				// ziaden iny token sa nemoze zacinat ! => chyba
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

				// jednoduchy identifikator
			case IDENTIFIKATOR:

				// identifikator pokracuje
				if (isalpha(c) || (c == '_') || (c == '$') || isdigit(c))
				{
					addChar(token->attr, c);
					token->type = IDENTIFIKATOR;
				}

				// kdyz je znak tecka nasleduje plne kvalifikovany identifikator
				else if (c == '.')
				{
					addChar(token->attr, c);
					token->type = PLNE_KVALIFIKOVANY_IDENTIFIKATOR;
				}

				// chybny znak -> posun v souboru o jeden zank zpet a vrati token
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = najdiKlucoveSlovo(token->attr);
					return token;
				}

				break;

				// plne kvalifikovany identifikator
			case PLNE_KVALIFIKOVANY_IDENTIFIKATOR:

				if (isalpha(c) || (c == '_') || (c == '$') || isdigit(c))
				{
					addChar(token->attr, c);
					token->type = PLNE_KVALIFIKOVANY_IDENTIFIKATOR_2;
				}

				// kombinace retezec.[cokoli krome identifikatoru] je chybna a vrati chybu
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

				// plne kvalifikovany identifikator pokracovani
			case PLNE_KVALIFIKOVANY_IDENTIFIKATOR_2:

				if (isalpha(c) || (c == '_') || (c == '$') || isdigit(c))
				{
					addChar(token->attr, c);
					token->type = PLNE_KVALIFIKOVANY_IDENTIFIKATOR_2;
				}

				// chybny znak -> posun v souboru o jeden znak zpet a vrati token
				else
				{
					fseek(subor, -1, SEEK_CUR);
					// projde tabulku klicovych slov a tabulku rezervovanych slov
					token->type = najdiKlucoveSlovo2(token->attr);

					// kdyz vyhodnoti token jako klicove, nebo rezervovane slovo vyhlasi chybu
					if ((token->type == KLICOVE_SLOVO) || (token->type == REZERVOVANE_SLOVO))
					{
						fseek(subor, -1, SEEK_CUR);
						token->type = LEXIKALNI_CHYBA;
					}

					// jinak vrati token PLNE_KVALIFIKOVANY_IDENTIFIKATOR
					else
					{
						token->type = PLNE_KVALIFIKOVANY_IDENTIFIKATOR;
						return token;
					}
				}

				break;

				// retezec
			case RETEZEC:

                if ((c > 31) && (c != 34) && (c != 92))
				{
					addChar(token->attr, c);
					token->type = RETEZEC;
				}

				// prijde / -> escape sekvence
				else if(c == 92)
                {
                    addChar(token->attr, c);
					token->type = RETEZEC_2;
                }

				// koniec retazca
				else if (c == 34)
				{
					token->type = RETEZEC;
					return token;
				}
				// jiny nepovoleny znak vrati chybu (nebo neukonceni retezce)
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

            case RETEZEC_2:

                /* znaky " n t \ */
                if ((c == 34) || (c == 110) || (c == 116) || (c == 92))
                {
                    addChar(token->attr, c);
					token->type = RETEZEC;
                }

                // oktalova escape sekvence (001-377)
                else if ((c >= '0') && (c <= '3'))
                {
                    // tedy oktalove cislo zacina 3
                    if (c == '3')
                    {
                        addChar(token->attr, c);

                        // escape sekvence (30? - 37?)
                        if ((c >= '0') && (c <= '7'))
                        {
                            // escape sekvence (37?-37?)
                            if (c == '7')
                            {
                                addChar(token->attr, c);

                                //escape sekvence (370-377)
                                if ((c >= '0') && (c <= '7'))
                                {
                                    addChar(token->attr, c);

                                    // escape sekvence (370-377 + dalsi znak neni cislo)
                                    if (!isdigit(c))
                                    {
                                        token->type = RETEZEC;
                                    }

                                    // escape sekvence (370-377 + dalsi znak je cislo) vrati chybu
                                    else
                                    {
                                        fseek(subor, -1, SEEK_CUR);
                                        token->type = LEXIKALNI_CHYBA;
                                    }
                                }

                                //escape sekvence (377-379) vrati chybu
                                else
                                {
                                    fseek(subor, -1, SEEK_CUR);
                                    token->type = LEXIKALNI_CHYBA;
                                }
                            }

                            // escape sekvence (30?-36?)
                            else
                            {
                                 addChar(token->attr, c);

                                 // sekvence pokracuje cislem
                                 if (isdigit(c))
                                 {
                                     addChar(token->attr, c);

                                     // sekvence nepokracuje cislem
                                     if (!isdigit(c))
                                     {
                                          token->type = RETEZEC;
                                     }

                                     // sekvence nepokracuje cislem (4. cislice neni povolena)
                                     else
                                     {
                                         fseek(subor, -1, SEEK_CUR);
                                         token->type = LEXIKALNI_CHYBA;
                                     }
                                 }

                                 // sekvence nepokracuje cislem - vrati chybu
                                 else
                                 {
                                     fseek(subor, -1, SEEK_CUR);
                                     token->type = LEXIKALNI_CHYBA;
                                 }
                            }
                        }

                        // vraci chybu (nepovolena sekvence)
                        else
                        {
                            fseek(subor, -1, SEEK_CUR);
                            token->type = LEXIKALNI_CHYBA;
                        }
                    }

                    // oktalove cislo nezacina 3
                    else
                    {
                        addChar(token->attr, c);

                        // sekvence pokracuje cislem
                        if (isdigit(c))
                        {
                            addChar(token->attr, c);

                            // sekvence pokracuje cislem
                            if (isdigit(c))
                            {
                                addChar(token->attr, c);

                                // sekvence nepokracuje cislem (4. cislice neni povolena)
                                if (!isdigit(c))
                                {
                                    token->type = RETEZEC;
                                }

                                // sekvence pokracuje cislem - vraci chybu
                                else
                                {
                                    fseek(subor, -1, SEEK_CUR);
                                    token->type = LEXIKALNI_CHYBA;
                                }
                            }

                            // sekvence nepokracuje cislem - vraci chybu
                            else
                            {
                                fseek(subor, -1, SEEK_CUR);
                                token->type = LEXIKALNI_CHYBA;
                            }
                        }

                        // sekvence nepokracuje cislem - vraci chybu
                        else
                        {
                            fseek(subor, -1, SEEK_CUR);
                            token->type = LEXIKALNI_CHYBA;
                        }
                    }
                }

                // jina escape sekvence
                else
                {
                    fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
                }

                break;

			case CELOCISELNY_LITERAL:

				// pokracuje cislo
				if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = CELOCISELNY_LITERAL;
				}

				// nasleduje desetinna cast
				else if (c == '.')
				{
					addChar(token->attr, c);
					token->type = DESETINNY_LITERAL;
				}

				// nasleduje exponent
				else if ((c == 'e') || (c == 'E'))
				{
					addChar(token->attr, c);
					token->type = CELOCISELNY_LITERAL_EXPONENT;
				}

				// konec cisla
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = CELOCISELNY_LITERAL;
					return token;
				}

				break;

			case DESETINNY_LITERAL:

				// desatinna cast cisla
				if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = DESETINNY_LITERAL_2;
				}

				// kombinace cislo.[cokoliv krome cisla] vrati chybu
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

			case DESETINNY_LITERAL_2:

				// v pripade cisla pokracujeme jako double (popø. double s kombinací)
				if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = DESETINNY_LITERAL_2;
				}

				// v pripade kombinace cislo.cisloe(E)
				else if ((c == 'e') || (c == 'E'))
				{
					addChar(token->attr, c);
					token->type = DESETINNY_LITERAL_EXPONENT;
				}

				// v pripade jine kombinace cislo.cislo vrati token DBL
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = DESETINNY_LITERAL;
					return token;
				}

				break;

			// kombinace cislo.cisloe(E)
			case DESETINNY_LITERAL_EXPONENT:

				// kombinace cislo.cisloe(E)+(-)
				// plus a minus je nepovinny znak, muze nasledovat cislo
				if ((c == '+') || (c == '-') || isdigit(c))
				{
					addChar(token->attr, c);
					token->type = DESETINNY_LITERAL_EXPONENT_2;
				}

				// jina kombinace vede k chybe (kombinace cislo.cisloe(E)[cokoli krome cisla])
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

			// kombinace cislo.cisloe(E)+(-,cislo)
			case DESETINNY_LITERAL_EXPONENT_2:

				// nasleduje cislo
				if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = DESETINNY_LITERAL_EXPONENT_3;
				}

				// vrati chybu, nepovolena kombinace
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

			// kombinace cislo.cisloe(E)+(-,cislo)cislo
			case DESETINNY_LITERAL_EXPONENT_3:

				// nasleduje cislo
				if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = DESETINNY_LITERAL_EXPONENT_3;
				}

				// jina kombinace
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = DESETINNY_LITERAL_EXPONENT;
					return token;
				}

				break;

			// kombinace cisloe(E)
			case CELOCISELNY_LITERAL_EXPONENT:

				// nasleduje +,-,cislo
				// plus a minus je nepovinny znak, muze nasledovat cislo
				if ((c == '+') || (c == '-') || isdigit(c))
				{
					addChar(token->attr, c);
					token->type = CELOCISELNY_LITERAL_EXPONENT_2;
				}

				// jina kombinace vede k chybe
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

			// kombinace cisloe(E)+(-,cislo)
			case CELOCISELNY_LITERAL_EXPONENT_2:

				// nasleduje cislo
				if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = CELOCISELNY_LITERAL_EXPONENT_3;
				}

				// jina kombinace vrati chybu
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = LEXIKALNI_CHYBA;
				}

				break;

			// kombinace cisloe(E)+(-,cislo)cislo
			case CELOCISELNY_LITERAL_EXPONENT_3:

				// nasleduje cislo
				if (isdigit(c))
				{
					addChar(token->attr, c);
					token->type = CELOCISELNY_LITERAL_EXPONENT_3;
				}

				// jina kombinace
				else
				{
					fseek(subor, -1, SEEK_CUR);
					token->type = CELOCISELNY_LITERAL_EXPONENT;
					return token;
				}

				break;

			// v pripade konce souboru ukonci funkci
			case KONEC_SOUBORU:

				return token;

				break;

			// v pripade chyby se chyba vytiskne a ukonci funkce
			case LEXIKALNI_CHYBA:

				//fprintf(stderr, "Chyba lexikalniho analyzatoru na radku %d\n", token->cisloRiadku);
				error(ERR_LEX);
				return NULL;

				break;
		}
	}
    return token; // Aby nebyl warning, ze to ne ve vsech pripadech vraci token
}

/*=========================IMPLEMENTACE FUNKCI=========================*/
// zkontroluje podle tabulky klicovych a rezervovanych slov, zda neodpovida tokenu
// identifikator
TokenType najdiKlucoveSlovo(string* s)
{
	int j;

	// prejdeme klucove slova
	for (j = 0; j < 17; j++)
		if (!(strcmp(s->str, klucoveSlova[j])))
			return KLICOVE_SLOVO;

	// prejdeme prezervovane slova
	for (j = 0; j < 36; j++)
		if (!(strcmp(s->str, rezervovaneSlova[j])))
			return REZERVOVANE_SLOVO;

	return IDENTIFIKATOR;
}

// zkontroluje podle tabulky klicovych a rezervovanych slov, zda neodpovida tokenu
// plne kvalifikovany identifikator
TokenType najdiKlucoveSlovo2(string* s)
{
	// pomocne promenne
	int j;
	char *str, *str2;

	char vysledek[100000];
	strcpy(vysledek, s->str);

	str = strtok(s->str, ".");
	str2 = strtok(NULL, " ");

	// prejdeme klucove slova
	for (j = 0; j < 17; j++)
		// kdyz se prvni nebo druhy identifikator rovna klicovemu slovu vrati chybu
		if ((strcmp(str, klucoveSlova[j]) == 0) || (strcmp(str2, klucoveSlova[j]) == 0))
			return KLICOVE_SLOVO;

	// prejdeme prezervovane slova
	for (j = 0; j < 36; j++)
		// kdyz se prvni nebo druhy identifikator rovna rezervovanemu slovu vrati chybu
		if ((strcmp(str, rezervovaneSlova[j]) == 0) || ((strcmp(str2, rezervovaneSlova[j])) == 0))
			return REZERVOVANE_SLOVO;

	s->str = vysledek;
	return IDENTIFIKATOR;
}


// rozsiri retezec o dany znak (prida char na koniec stringu)
int addChar(string* s, char c)
{
	char* new;

	if (!(new = (char *)realloc(s->str, s->length + 2)))
	{
		return 1;
	}

	else
	{
		s->str = new;
	}

	s->str[s->length] = c;
	s->str[s->length + 1] = '\0';
	s->length++;

	return 0;
}

// inicializace retezce reprezentujici cast tokenu, pouzita ve funkci vytvorToken()
int stringInit(string* s)
{
	s->str = (char *)malloc(sizeof(char));

	if (!s->str)
	{
		return 1;
	}
	else
	{
		s->str[0] = '\0';
		s->length = 0;
		return 0;
	}
}

// funkce vytvarejici token (alokuje tokenu a jeho nasledna inicializace)
Ttoken* vytvorToken()
{
	Ttoken* token;

	token = (Ttoken *)malloc(sizeof(Ttoken));
	token->attr = (string *)malloc(sizeof(string));
	token->type = POC_STAV;
    token->next = NULL;

	// funkce implementovana vyse
	stringInit(token->attr);

	return token;
}

// funkce pro ziskani textove reprezentace typu tokenu
char *getTokenName(TokenType type) {
    switch (type) {
    case POC_STAV:
        return "POC_STAV";
        break;
    case JEDNORADKOVY_KOMENTAR:
        return "//";
        break;
    case VICERADKOVY_KOMENTAR:
        return "/*";
        break;
    case VICERADKOVY_KOMENTAR_2:
        return "*/";
        break;
    case SCITANI:
        return "+";
        break;
    case DELENI:
        return "/";
        break;
    case ODECITANI:
        return "-";
        break;
    case NASOBENI:
        return "*";
        break;
    case MENSI:
        return "<";
        break;
    case VETSI:
        return "*";
        break;
    case ROVNO:
        return "==";
        break;
    case NEROVNO:
        return "!=";
        break;
    case MENSI_NEBO_ROVNO:
        return "<=";
        break;
    case VETSI_NEBO_ROVNO:
        return ">=";
        break;
    case PRIRAZENI:
        return "=";
        break;
    case STREDNIK:
        return ";";
        break;
    case CARKA:
        return ",";
        break;
    case LEVA_KULATA_ZAVORKA:
        return "(";
        break;
    case PRAVA_KULATA_ZAVORKA:
        return ")";
        break;
    case LEVA_HRANATA_ZAVORKA:
        return "[";
        break;
    case PRAVA_HRANATA_ZAVORKA:
        return "]";
        break;
    case LEVA_SLOZENA_ZAVORKA:
        return "{";
        break;
    case PRAVA_SLOZENA_ZAVORKA:
        return "}";
        break;
    case IDENTIFIKATOR:
        return "IDENT";
        break;
    case PLNE_KVALIFIKOVANY_IDENTIFIKATOR:
        return "PLNY_IDENT";
        break;
    case PLNE_KVALIFIKOVANY_IDENTIFIKATOR_2:
        return "PLNY_IDENT2";
        break;
    case KLICOVE_SLOVO:
        return "KEYWORD";
        break;
    case REZERVOVANE_SLOVO:
        return "REZWORD";
        break;
    case RETEZEC:
        return "String";
        break;
    case RETEZEC_2:
        return "String2";
        break; 
    case CELOCISELNY_LITERAL:
        return "celeCislo";
        break;
    case DESETINNY_LITERAL:
        return "desCislo";
        break;
    case DESETINNY_LITERAL_2:
        return "desCislo2";
        break;
    case DESETINNY_LITERAL_EXPONENT:
        return "desCisloExp";
        break;
    case DESETINNY_LITERAL_EXPONENT_2:
        return "desCisloExp2";
        break;
    case DESETINNY_LITERAL_EXPONENT_3:
        return "desCisloExp3";
        break;
    case CELOCISELNY_LITERAL_EXPONENT:
        return "celeCisloExp";
        break;
    case CELOCISELNY_LITERAL_EXPONENT_2:
        return "celeCisloExp2";
        break;
    case CELOCISELNY_LITERAL_EXPONENT_3:
        return "celeCisloExp3";
        break;
    case KONEC_SOUBORU:
        return "EOF";
        break;
    case LEXIKALNI_CHYBA:
        return "LERROR";
        break;
    default:
        return "???";
        break;
    }
}

/*=========================KONEC SOUBORU LEX_ANAl.c=========================*/
