/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "ial.h"
#include "testing.h"

int	findSubstring(char *s, char *search) {
	if (s == NULL || search == NULL 
		|| s[0] == '\0' || search[0] == '\0') return 0;

	int sLen = strlen(s);
	int searchLen = strlen(search);
	int i,j,k=0;

	if (sLen < searchLen) return 0; // neni mozne aby byl v retezci delsi podretezec

	// pozici kurzoru nastavim na searchLen az do sLen
	for (i = searchLen - 1, j = 0; i < sLen && j < searchLen;) {
		// kontroluju jestli je znak c z S je v search
		// pokud ne 
			// posunu se o pozici c nalezenem v search -> plna delka pokud znak neni
			// jinak o pozici znaku c zprava
		//opakuju do shody nebo konce S

		if (s[i-j] == search[searchLen-j-1]) {//znak odpovida
			j++; 
		}
		else { // neshoduje se, posouvam se..
			for (k = searchLen; k > 0 && s[i-j] != search[k-1]; k--);
			j = 0;
			i += searchLen - k; 
		}
	}

	if (j == searchLen) return i - searchLen + 1;
	else return -1;
}

HashTable createHashTable(unsigned int size) {
	HashTable htab;
	htab = malloc(sizeof(struct hItem)*size);
    if (htab == NULL)
        error(ERR_INTER);

	memset(htab, 0, sizeof(struct hItem)*size);

	return htab;
}

// Odstraneni tabulky
void deleteHashTable(HashTable hashTable) {

	if (hashTable == NULL) return;

	debug(" [HASHTABLE] Deleting table:");
	hashWriteOut(hashTable);

	for (unsigned int i = 0; i < HASH_TABLE_SIZE; i++) {
		if (hashTable[i].next != NULL) {
			for (HashTable item = hashTable[i].next; item != NULL; ) {
				HashTable tmp = item;
				item = item->next;
				free(tmp->key);
				free(tmp->type);
				free(tmp);
			}
			hashTable[i].next = NULL;
		}
	}
	for (unsigned int i = 0; i < HASH_TABLE_SIZE; i++) {
		if (hashTable[i].key) {
			free(hashTable[i].key);
			free(hashTable[i].type);
		}
	}
	
	free(hashTable);
}

// Pridavani zaznamu do tabulky
int addToHashTable(HashTable hashTable, char *key, char *type, unsigned int data, unsigned int varIndex) {

	if (searchInHashTable(hashTable, key) != NULL) //prvek uz existuje!
		return 1;

	unsigned int index = hash(key);

	HashTable item = &(hashTable[index]);

	if (item->key != NULL) {//pokud neco jiz existuje
		for (; item->next != NULL; item = item->next); //najdu konec
													   // vytvorim novou polozku
		HashTable tmp = malloc(sizeof(struct hItem));
		//pridam do seznamu
		item->next = tmp;
		//zmenim odkaz na polozku
		item = tmp;
	}

	item->key = malloc(sizeof(char)*strlen(key) + 1);
	strcpy(item->key, key);

	item->type = malloc(sizeof(char)*strlen(type) + 1);
	strcpy(item->type, type);

	item->data = data;
	item->index = varIndex;
	item->next = NULL;


	return 0;
}


// Vyhledavani v tabulce
HashTable searchInHashTable(HashTable hashTable, char *key) {
	//unsigned int index = hash(key);

	// prochazim zaznam, pokud je key = 0 neexistuje v tabulce, pokud je zaznam nullovy = neexistuje v seznamu
	for (HashTable item = &(hashTable[hash(key)]); item != NULL && item->key != NULL; item = item->next) {
		if (!strcmp(key, item->key))
			return item; //vracim odkaz na zaznam
	}

	// nenalezen
	return NULL;
}

// Hashovaci fce
unsigned int hash(char *key) {
	unsigned int h = 0;
	const unsigned char *p;
	for (p = (const unsigned char*)key; *p != '\0'; p++)
		h = 65599 * h + *p;
	return h % HASH_TABLE_SIZE;
}

void listMergeSort(char *arr) {
	//printf("%s\n",arr);
	int size = strlen(arr);
	if (size == 1) return;
	int *pom = malloc(sizeof(int)*size);
	
	
	//for (int i = 0; i < size; i++)
	//	printf("%3d| ", i);
	//printf("\n");

	//for (int i = 0; i < size; i++)
	//	printf("%3c| ", arr[i]);
	//printf("\n");

	//zretezeni posloupnosti
	for (int i = 0; i < size-1; i++) {
		if (arr[i] < arr[i + 1]) {
			pom[i] = i+1;
		}
		else {
			pom[i] = -1; //konec
		}
		//printf("%3d| ", pom[i]);
	}

	pom[size-1] = -1; //konec


	//seznam zacatku
	listZacatku L = lzinit();
	insertLast(&L, 0);

	for (int i = 1; i < size; i++) {
		if (pom[i - 1] == -1) {
			insertLast(&L, i);
		}
	}

	//for (neklPos tmp = L.first; tmp != NULL; tmp = tmp->next)
		//printf("zacatek %d\n", tmp->index);

	//sort
	int zac1=0, zac2=0;
	int index=0;
	int tmp1,tmp2;
	do {
		for (neklPos tmp = L.first; tmp != NULL; tmp = tmp->next)
			//printf("zacatek %d\n", tmp->index);

		copyFirst(&L, &zac1);
		tmp1 = zac1;
		deleteFirst(&L); first(&L);
		if (active(&L)) {
			copyFirst(&L,&zac2);
			tmp2 = zac2;
			deleteFirst(&L);
			if (arr[zac1] < arr[zac2]) { 
				index = zac1;
				tmp1 = pom[zac1];
				insertLast(&L, zac1); 
			}
			else {
				index = zac2;
				tmp2 = pom[zac2];
				insertLast(&L, zac2);
			}
			

			//seradim 
			while(tmp1!=-1 && tmp2!=-1){
				//printf("\ncompare %c %c %d %d %d", arr[tmp1], arr[tmp2],index,tmp1,tmp2);
				if (arr[tmp1] < arr[tmp2]) {		
					pom[index] = tmp1;
					index = tmp1;
					tmp1 = pom[tmp1];
					if (tmp1 == -1) {
						pom[index] = tmp2;
						break;
					}
					
				}
				else {
					pom[index] = tmp2;
					index = tmp2;
					tmp2 = pom[tmp2];
					if (tmp2 == -1) {
						pom[index] = tmp1;
						break;
					}
				}		
			}

			if (tmp1 == -1) {
				pom[index] = tmp2;
			}
			else if(tmp2 == -1)
					pom[index] = tmp1;
		}
	} while (L.first != L.last);

	char *tmp = malloc(size);
	//printf("END:\n"); 
	index = 0;
	for (int i = L.first->index; i != -1; i = pom[i],index++) {
		tmp[index] = arr[i];
	}
	
	for (int i = 0; i < size; i++)
		arr[i] = tmp[i];

	free(tmp);
}


listZacatku lzinit() {
	listZacatku L;
	L.active = NULL;
	L.first = NULL;
	L.last = NULL;
	return L;
}

void copyFirst(listZacatku *L, int *index) {
	*index = L->first->index;
}
void deleteFirst(listZacatku *L) {
	neklPos tmp = L->first->next;
	if (L->first == L->last) {
		L->last = NULL;
	}
	free(L->first);
	L->first = tmp;
}
void first(listZacatku *L) {
	L->active = L->first;
}
int active(listZacatku *L) {
	return (int) L->active;
}

void insertLast(listZacatku *L, int index) {
	//printf("adding %d\n", index);
	if (L->last == NULL) {
		neklPos tmp = malloc(sizeof(struct listM));
		tmp->index = index;
		tmp->next = NULL;
		//printf("adding first %d\n", tmp->index);
		L->first = tmp;
		L->last = tmp;
	}
	else {
		neklPos tmp = malloc(sizeof(struct listM));
		tmp->index = index;
		tmp->next = NULL;
		//printf("adding next %d\n", tmp->index);
		L->last->next = tmp;
		L->last = tmp;
	}
}