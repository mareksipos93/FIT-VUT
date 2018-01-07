#include <stdio.h>
#include <assert.h>
#include "tmal.h"

const unsigned int MAX_BLOCKS = 10000;
const size_t THREAD_HEAP = 1L*1024*1024*1024; // 1GB

int main()
{
    assert(blks_table == NULL);

    // priprava pro 4 vlakna
    tal_alloc_blks_table(4);

    assert(blks_table != NULL);

    // kazdemu vlaknu priradit/alokovat jeho heap
    for (int tid = 0; tid < 4; tid++)
        tal_init_blks(tid, MAX_BLOCKS, THREAD_HEAP);

    for (int tid = 0; tid < 4; tid++)
    {
        assert(blks_table[tid].nblks == MAX_BLOCKS);
        assert(blks_table[tid].heap_size == THREAD_HEAP);
        struct blk_info_t *first = blks_table[tid].blk_info_arr;
        assert(first != NULL);
        assert(first->size == THREAD_HEAP);
        assert(!first->used);
        assert(first->prev_idx < 0 && first->next_idx < 0);
    }

    /**
     *   +----+----+----+----+----+-------------+
     *   | .................................... |
     *   +----+----+----+----+----+-------------+
     */

    // vlakno 2 by rado alokovalo par bajtu
    void *b1 = tal_alloc(2, sizeof(size_t));

    /**
     *   +----+----+----+----+----+-------------+
     *   | b1 | ............................... |
     *   +----+----+----+----+----+-------------+
     */

    assert(blks_table[2].blk_info_arr[0].size == sizeof(size_t));
    assert(blks_table[2].blk_info_arr[0].ptr == b1);
    assert(blks_table[2].blk_info_arr[0].prev_idx < 0);
    assert(blks_table[2].blk_info_arr[0].next_idx == 1);
    assert(blks_table[2].blk_info_arr[0].used);
    assert(blks_table[2].blk_info_arr[1].size == THREAD_HEAP - sizeof(size_t));
    assert(blks_table[2].blk_info_arr[1].prev_idx == 0);
    assert(blks_table[2].blk_info_arr[1].next_idx < 0);
    assert(!blks_table[2].blk_info_arr[1].used);

    // alokujeme vice polozek
    void *  a[4];
    for (int i = 0; i < 4; i++)
        a[i] = tal_alloc(2, sizeof(size_t));

    /**
     *   +----+----+----+----+----+-------------+
     *   | b1 | a0 | a1 | a2 | a3 | ........... |
     *   +----+----+----+----+----+-------------+
     */
    tal_free(2, a[0]);

    /**
     *   +----+----+----+----+----+-------------+
     *   | b1 | .. | a1 | a2 | a3 | ........... |
     *   +----+----+----+----+----+-------------+
     */

    tal_free(2, a[2]);

    /**
     *   +----+----+----+----+----+-------------+
     *   | b1 | .. | a1 | .. | a3 | ........... |
     *   +----+----+----+----+----+-------------+
     */

    void *c1 = tal_alloc(2, 1); // alokuj pouze 1 bajt, ale zarovnej

    /**
     *   +----+----+----+----+----+-------------+
     *   | b1 | c1 | a1 | .. | a3 | ........... |
     *   +----+----+----+----+----+-------------+
     */

    assert(blks_table[2].blk_info_arr[0].next_idx == 1);
    assert(blks_table[2].blk_info_arr[1].prev_idx == 0);
    assert(blks_table[2].blk_info_arr[1].ptr == c1);
    assert(blks_table[2].blk_info_arr[1].size == sizeof(size_t));

    tal_free(2, a[1]);

    /**
     *   +----+----+----+----+----+-------------+
     *   | b1 | c1 | ....... | a3 | ........... |
     *   +----+----+----+----+----+-------------+
     */

    unsigned b1_idx = 0;
    unsigned c1_idx = blks_table[2].blk_info_arr[b1_idx].next_idx;
    unsigned blank_idx = blks_table[2].blk_info_arr[c1_idx].next_idx;
    unsigned a3_idx = blks_table[2].blk_info_arr[blank_idx].next_idx;
    unsigned rest_idx = blks_table[2].blk_info_arr[a3_idx].next_idx;
    assert(blks_table[2].blk_info_arr[b1_idx].ptr == b1);
    assert(blks_table[2].blk_info_arr[c1_idx].ptr == c1);
    assert(blks_table[2].blk_info_arr[blank_idx].ptr == a[1]);
    assert(blks_table[2].blk_info_arr[a3_idx].ptr == a[3]);
    assert(blks_table[2].blk_info_arr[rest_idx].size ==
           THREAD_HEAP - 5*sizeof(size_t));

    tal_free(2, b1);

    /**
     *   +----+----+----+----+----+-------------+
     *   | .. | c1 | ....... | a3 | ........... |
     *   +----+----+----+----+----+-------------+
     */

    tal_free(2, c1);

    /**
     *   +----+----+----+----+----+-------------+
     *   | ................. | a3 | ........... |
     *   +----+----+----+----+----+-------------+
     */

    unsigned first_idx = 0;
    unsigned second_idx = blks_table[2].blk_info_arr[first_idx].next_idx;
    unsigned third_idx = blks_table[2].blk_info_arr[second_idx].next_idx;
    assert(! blks_table[2].blk_info_arr[first_idx].used);
    assert(blks_table[2].blk_info_arr[second_idx].used);
    assert(! blks_table[2].blk_info_arr[third_idx].used);

    assert(blks_table[2].blk_info_arr[first_idx].size == 4*sizeof(size_t));
    assert(blks_table[2].blk_info_arr[second_idx].size == sizeof(size_t));

    return 0;
}