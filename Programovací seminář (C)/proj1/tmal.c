/* ========================================================================== */
/* Autor:   Marek Šipoš (xsipos03)                                            */
/* Predmet: IPS (soubor tmal.c)                                               */
/* Datum:   14.10.2016                                                        */
/* ========================================================================== */

#include <stdio.h>  // fprintf
#include <stdlib.h> // malloc
#include "tmal.h";

/* Priprava globalniho pole blks_table, ktere bude ukazovat na jednotlive tabulky bloku */
struct blk_array_info_t *blks_table = NULL;

/**
 * Vyctovy typ reprezentujici chybove stavy
 */
typedef enum {
        UNKNOWN
    ,   MALLOC_FAILED
    ,   BLKS_TABLE_NOT_READY
    ,   BLOCK_INDEX_INVALID
    ,   BLOCK_SIZE_INVALID
    ,   BLOCK_NOT_FOUND
    ,   MAXIMUM_BLOCKS_REACHED
} ERROR_CODES;

/**
 * Funkce pro zachytavani chybovych stavu
 * @param code Chybovy kod
 */
void error(ERROR_CODES code) {
    switch (code) {
        case MALLOC_FAILED:
            fprintf(stderr, "ERROR - allocation failed (malloc)\n");
            break;
        case UNKNOWN:
            fprintf(stderr, "ERROR - unknown error\n");
            break;
        case BLKS_TABLE_NOT_READY:
            fprintf(stderr, "ERROR - block tables not ready\n");
            break;
        case BLOCK_INDEX_INVALID:
            fprintf(stderr, "ERROR - requested block number is invalid\n");
            break;
        case BLOCK_SIZE_INVALID:
            fprintf(stderr, "ERROR - requested block size is invalid\n");
            break;
        case BLOCK_NOT_FOUND:
            fprintf(stderr, "ERROR - memory block for given address not found\n");
            break;
        case MAXIMUM_BLOCKS_REACHED:
            fprintf(stderr, "ERROR - all blocks for given thread are already in use\n");
            break;
    }
}

/**
 * Zarovna danou velikost na nejblizsi vyssi nasobek
 * @param input Velikost na zarovnani
 * @param multiplier Pozadovany nasobek
 * @return Zarovnana velikost na dany nasobek
 */
size_t align_size(size_t input, size_t multiplier) {
    return ((input+(multiplier-1))/multiplier)*multiplier;
}

/**
 * Nalezne nepouzivanou strukturu blk_info_t a vrati jeji ID
 * @param tid Identifikator vlakna
 * @return ID nepouzivaneho blk_info_t, -1 pokud se vsechny pouzivaji
 */
int find_unused_block(unsigned tid) {
    int i = 0;
    while (blks_table[tid].blk_info_arr[i].used) {
        i++;
        if (i == blks_table[tid].nblks) {
            return -1;
        }
    }
    return i;
}

/* Alokace ridkych tabulek pro nthreads vlaken */
struct blk_array_info_t *tal_alloc_blks_table(unsigned nthreads){

    /* Alokace pameti pro vsechny tabulky bloku jednotlivych vlaken */
    if ((blks_table = malloc(nthreads * sizeof(struct blk_array_info_t))) == NULL)
    {
        error(MALLOC_FAILED);
        return NULL;
    }

    return &blks_table[0]; // prvni tabulka bloku - ale jeji reference (proto ampersand)
}

/* Priprava tabulky bloku pro dane vlakno */
struct blk_info_t *tal_init_blks(unsigned tid, unsigned nblks, size_t theap)
{
    /* Ridke tabulky nejsou alokovany */
    if (blks_table == NULL) {
        error(BLKS_TABLE_NOT_READY);
        return NULL;
    }

    /* Alokace pameti pro vsechny blk_info_t daneho vlakna */
    if ((blks_table[tid].blk_info_arr = malloc(nblks * sizeof(struct blk_info_t))) == NULL)
    {
        error(MALLOC_FAILED);
        return NULL;
    }

    size_t theap_aligned = align_size(theap, sizeof(size_t));

    /* Alokace heapu */
    void *heap;
    if ((heap = malloc(theap_aligned)) == NULL) {
        error(MALLOC_FAILED);
        return NULL;
    }

    /* Prvni blok bude nejvetsi = cely heap */
    blks_table[tid].blk_info_arr[0].ptr = heap;
    blks_table[tid].blk_info_arr[0].size = theap_aligned;
    blks_table[tid].blk_info_arr[0].used = false;
    blks_table[tid].blk_info_arr[0].next_idx = -1;
    blks_table[tid].blk_info_arr[0].prev_idx = -1;

    /* Priprava vsech dalsich nepouzivanych blk_info_t */
    for (unsigned i = 1; i < nblks; i++) {
        blks_table[tid].blk_info_arr[i].ptr = NULL; // Blok nema prirazenu pamet
        blks_table[tid].blk_info_arr[i].size = 0; // Blok nereprezentuje zadnou cast pameti
        blks_table[tid].blk_info_arr[i].used = false; // Blok neni pouzivan, protoze ani nema pamet
        /* Blok nema prirazenu pamet, tudiz nema souseda */
        blks_table[tid].blk_info_arr[i].next_idx = -1;
        blks_table[tid].blk_info_arr[i].prev_idx = -1;
    }

    /* Prirazeni informaci do tabulky bloku tohoto vlakna */
    blks_table[tid].nblks = nblks; // Celkovy pocet bloku (pripravenych struktur blk_info_t)
    blks_table[tid].heap_size = theap; // Celkova velikost heapu daneho vlakna

    return blks_table[tid].blk_info_arr; // Vraci ukazatel na prvni blk_info_t toho vlakna
}

/* Rozdeleni jednoho bloku na dva */
int tal_blk_split(unsigned tid, int blk_idx, size_t req_size) {

    /* Kontrola, zda takovy blok vubec existuje */
    if (blk_idx >= blks_table[tid].nblks) {
        error(BLOCK_INDEX_INVALID);
        return -1;
    }

    size_t req_size_aligned = align_size(req_size, sizeof(size_t)); // Zarovname pozadovanou velikost

    /* Kontrola, zda tento blok muze danou pamet rozdelit tak, aby mu zbylo aspon 1*sizeof(size_t) pameti */
    if (blks_table[tid].blk_info_arr[blk_idx].size-req_size_aligned < sizeof(size_t)) {
        error(BLOCK_SIZE_INVALID);
        return -1;
    }

    /* Nalezeni vhodneho prazdneho blk_info_t a osetreni, kdy se jiz vsechny bloky pouzivaji */
    int blk_idx_new = find_unused_block(tid);
    if (blk_idx_new == -1) {
        error(MAXIMUM_BLOCKS_REACHED);
        return -1;
    }

    /* Uprava puvodniho a noveho bloku (nutno zachovat poradi) */
    blks_table[tid].blk_info_arr[blk_idx].size -= req_size_aligned;
    blks_table[tid].blk_info_arr[blk_idx_new].ptr = (void *)(((char *)blks_table[tid].blk_info_arr[blk_idx].ptr) + blks_table[tid].blk_info_arr[blk_idx].size);
    blks_table[tid].blk_info_arr[blk_idx_new].size = req_size_aligned;
    blks_table[tid].blk_info_arr[blk_idx_new].used = false;
    blks_table[tid].blk_info_arr[blk_idx_new].next_idx = blks_table[tid].blk_info_arr[blk_idx].next_idx;
    blks_table[tid].blk_info_arr[blk_idx_new].prev_idx = blk_idx;
    blks_table[tid].blk_info_arr[blk_idx].next_idx = blk_idx_new;

    return blk_idx_new;
}

/* Slouceni dvou bloku do jednoho */
void tal_blk_merge(unsigned tid, int left_idx, int right_idx) {

    /* Kontrola, zda bloky existuji */
    if (left_idx >= blks_table[tid].nblks || right_idx >= blks_table[tid].nblks ) {
        error(BLOCK_INDEX_INVALID);
    }

    /* Uprava bloku - levy je rozsireny, pravy bude prazdny a nevyuzivany */
    blks_table[tid].blk_info_arr[left_idx].size += blks_table[tid].blk_info_arr[right_idx].size;
    blks_table[tid].blk_info_arr[left_idx].next_idx = blks_table[tid].blk_info_arr[right_idx].next_idx;
    if (blks_table[tid].blk_info_arr[left_idx].used == true && blks_table[tid].blk_info_arr[left_idx].used == false) {
        blks_table[tid].blk_info_arr[left_idx].used = true;
    } else {
        blks_table[tid].blk_info_arr[left_idx].used = blks_table[tid].blk_info_arr[right_idx].used ;
    }
    blks_table[tid].blk_info_arr[right_idx].ptr = NULL;
    blks_table[tid].blk_info_arr[right_idx].size = 0;
    blks_table[tid].blk_info_arr[right_idx].used = false;
    blks_table[tid].blk_info_arr[right_idx].next_idx = -1;
    blks_table[tid].blk_info_arr[right_idx].prev_idx = -1;
}

/**
 * Allocate memory for a given thread. Note that allocated memory will be
 * aligned to sizeof(size_t) bytes.
 * @param  tid  thread index (in the blocks table)
 * @param  size requested allocated size
 * @return      pointer to allocated space, NULL = failed
 */
void *tal_alloc(unsigned tid, size_t size) {

    size_t size_aligned = align_size(size, sizeof(size_t)); // Zarovname pozadovanou velikost
    int i = 0; // Iteracni idx bloku
    size_t size_continuous = 0;
    int blocks_continuous = 0;

    /* Projdeme vsemi bloky a zkusime najit misto pro alokovanou pamet */
    while (i >= 0) {
        if (blks_table[tid].blk_info_arr[i].used == false) { // Blok nesmi byt pouzivan
            if (blks_table[tid].blk_info_arr[i].size >= size_aligned) {
                /* Nalezeny blok je o neco vetsi nez potrebujeme, tak ho rozdelime na dva */
                if (blks_table[tid].blk_info_arr[i].size > size_aligned) {
                    tal_blk_split(tid, i, size_aligned);
                }
                blks_table[tid].blk_info_arr[i].used = true; // Nyni bude blok pouzivan
                return blks_table[tid].blk_info_arr[i].ptr; // Vratime adresu
            } else { // Velikost nestaci, ale mozna mame vice volnych bloku za sebou
                size_continuous += blks_table[tid].blk_info_arr[i].size;
                blocks_continuous++;
                // TODO - uz mozna mame size_continuous velkou jako size_aligned, takze merge vsech bloku
            }
        } else {
            /* Pokud bylo vic volnych bloku za sebou, tak je to jedno, protoze tento se pouzival */
            size_continuous = 0;
            blocks_continuous = 0;
        }

        i = blks_table[tid].blk_info_arr[i].next_idx; // Pokud jsme misto nenasli, pokracujeme dalsim prvkem
    }

    return NULL;
}

/**
 * Realloc memory for a given thread.
 * @param tid   thread index
 * @param ptr   pointer to allocated memory, NULL = allocate a new memory.
 * @param size  a new requested size (may be smaller than already allocated),
 *              0 = equivalent to free the allocated memory.
 * @return      pointer to reallocated space, NULL = failed.
 */
void *tal_realloc(unsigned tid, void *ptr, size_t size) {
    // TODO
}

/* Uvolneni bloku podle zadane adresy */
void tal_free(unsigned tid, void *ptr) {
    int i = 0;
    while (i >= 0) {
        if (ptr == blks_table[tid].blk_info_arr[i].ptr) { // Pamet se musi presne schodovat
            blks_table[tid].blk_info_arr[i].used = false; // Nastavime, ze je blok volny
            return;
        }

        i = blks_table[tid].blk_info_arr[i].next_idx; // Nenasli jsme spravny blok, pokracujeme
    }
}