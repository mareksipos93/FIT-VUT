/**
 * Hlavickovy soubor pro Thread Memory Allocator.
 * Demonstracni priklad pro 1. ukol IPS/2016
 * Ales Smrcka
 */
#ifndef _TMAL_H
#define _TMAL_H

#include <stddef.h> // size_t
#include <stdbool.h> // bool

/**
 * The structure blk_info_t encapsulates data of a single memory block.
 */
struct blk_info_t {

    /// base pointer of the allocated space
    void *ptr;

    /// size of the block
    size_t size;

    /**
     * Index to blk_info_t which is to the left (resp. right) of the current
     * block (double-linked list). Negative value means there is no such block.
     */
    int prev_idx;
    int next_idx;

    /// true = block is allocated, false = block is free
    bool used;
};

/**
 * Extended block array: base pointer of array + array capacity.
 */
struct blk_array_info_t {

    /// pointer to the first block info. NULL = block array is not used
    struct blk_info_t *blk_info_arr;

    /// number of active blocks (allocated for array of blk_info_t)
    unsigned nblks;

    /// heap capacity
    size_t heap_size;
};

/**
 * Global base pointer to block tables. Thread index is the index to blk_table.
 */
extern struct blk_array_info_t *blks_table;

/**
 * Allocate sparse table of blocks for several threads.
 * @param  nthreads     number of threads/items in the table
 * @return              pointer to the first block array, NULL = failed
 */
struct blk_array_info_t *tal_alloc_blks_table(unsigned nthreads);

/**
 * Allocates and initialize array of blocks.
 * @param  tid      thread index.
 * @param  nblks    capacity in number of blocks in the array.
 * @param  theap    heap capacity for a given thread.
 * @return          pointer to the first block in an array.
 */
struct blk_info_t *tal_init_blks(unsigned tid, unsigned nblks, size_t theap);

/**
 * Splits one block into two.
 * @param tid       thread index
 * @param blk_idx  index of the block to be split
 * @param req_size  requested size of the block
 * @return          index of a new block created as remainder.
 */
int tal_blk_split(unsigned tid, int blk_idx, size_t req_size);

/**
 * Merge two blocks in the block list/array.
 * @param tid       thread index
 * @param left_idx  index of the left block
 * @param right_idx index of the right block
 */
void tal_blk_merge(unsigned tid, int left_idx, int right_idx);

/**
 * Allocate memory for a given thread. Note that allocated memory will be
 * aligned to sizeof(size_t) bytes.
 * @param  tid  thread index (in the blocks table)
 * @param  size requested allocated size
 * @return      pointer to allocated space, NULL = failed
 */
void *tal_alloc(unsigned tid, size_t size);

/**
 * Realloc memory for a given thread.
 * @param tid   thread index
 * @param ptr   pointer to allocated memory, NULL = allocate a new memory.
 * @param size  a new requested size (may be smaller than already allocated),
 *              0 = equivalent to free the allocated memory.
 * @return      pointer to reallocated space, NULL = failed.
 */
void *tal_realloc(unsigned tid, void *ptr, size_t size);

/**
 * Free memory for a given thread.
 * @param tid   thread index
 * @param ptr   pointer to memory allocated by tal_alloc or tal_realloc.
 *              NULL = do nothing.
 */
void tal_free(unsigned tid, void *ptr);

#endif