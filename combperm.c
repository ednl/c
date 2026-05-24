/**
 * COMBINATIONS AND PERMUTATIONS
 * Freeware. No pull requests accepted.
 * Made by: E. Dronkert, Utrecht NL, May 2024.
 * https://github.com/ednl
 * https://mastodon.social/@ednl
 */

#include <stdlib.h>  // malloc, realloc, free
#include <string.h>  // memcpy
#include "combperm.h"

// Successive calls give combinations of k indices from a set of n.
// Adapted from Knuth 4A, §7.2.1.3, algorithm T.
// Returns pointer to array of int, index 0..k-1.
//
// For example: combinations(3,2) gives [0,1], [0,2], [1,2]
// The fourth call will return NULL (and memory is freed).
//
// To reset and free memory before all combinations have been
// generated, call with n<=0 or k<=0 or n<=k.
int *combinations(const int n, const int k)
{
    static int *index = NULL;
    static int j, x;

    // Wrong input or reset
    if (n <= 0 || k <= 0 || n <= k) {
        free(index);
        return index = NULL;
    }

    // Initialisation
    if (!index) {
        index = malloc((size_t)(k + 2) * sizeof *index);
        if (index == NULL)
            return NULL;
        for (int i = 0; i < k; ++i)
            index[i] = i;
        index[k    ] = n;
        index[k + 1] = 0;
        j = k - 1;
        return index;
    }

    // Construct new combination, return immediately when
    // the next one is complete.
    while (j >= 0) {
        index[j] = j + 1;
        j--;
        return index;
    }

    if (index[0] + 1 < index[1]) {
        index[0]++;
        return index;
    }

    j = 0;
    do {
        j++;
        index[j - 1] = j - 1;
    } while ((x = index[j] + 1) == index[j + 1]);

    if (j < k) {
        index[j--] = x;
        return index;
    }

    // No more new combinations
    free(index);
    return index = NULL;
}

// Local helper function: swap two ints via different pointers.
static inline void swap(int *const restrict a, int *const restrict b)
{
    const int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Successive calls give permutations in lexicographic order of 'count' index numbers.
// Ref.: https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// Returns pointer to first element of next permutation of 'count' index numbers.
// Returns NULL (and memory is freed) when all permutations have been visited.
//   Set count<=0 to free memory.
//   NB: not thread-safe because permutation and state
//       are stored in local static variables.
int *permutations(const int count)
{
    static int *index = NULL;
    static int len = 0;

    // Wrong input or manual reset: reset & return
    if (count < 1) {
        free(index);
        len = 0;
        return (index = NULL);
    }

    // (Re-) initialisation on first call or if count changes
    if (index == NULL || count != len) {
        int *tmp = realloc(index, count * sizeof *tmp);
        if (tmp) {
            index = tmp;
            len = count;
            for (int i = 0; i < count; ++i)
                index[i] = i;
        }
        return index;
    }

    // Find pivot_x: last element where next element is larger
    int px = len - 2;
    while (px >= 0 && index[px] >= index[px + 1])
        --px;

    // If not found, this was the final permutation: reset & return
    if (px < 0) {
        free(index);
        len = 0;
        return (index = NULL);
    }

    // Find pivot_y: last element larger than pivot_x and swap them
    int py = len - 1;
    while (index[px] >= index[py])
        --py;  // py will be at least px+1 because pivot_x < pivot_x+1
    swap(&index[px], &index[py]);

    // Reverse suffix = sequence after pivot_x
    for (int l = px + 1, r = len - 1; l < r; ++l, --r)
        swap(&index[l], &index[r]);

    // Return next permutation
    return index;
}

// Successive calls give permutations in "plain changes" order of 'count' index numbers.
// Permutations in the second half are lexically reversed from those in the first half.
// Ref.: https://en.wikipedia.org/wiki/Steinhaus–Johnson–Trotter_algorithm
//       with improvement by Shimon Even as implemented by:
//       https://github.com/steppi/adeft/blob/master/src/adeft/score/permutations.pyx
// Returns pointer to first element of next permutation of 'count' index numbers.
// Returns NULL (but memory NOT freed) when all permutations have been visited.
//   Set count<=0 to free memory.
//   NB: not thread-safe because permutation and state
//       are stored in local static variables.
int *plainchanges(const int count)
{
    static int *mem = NULL;   // memory arena for 4 arrays
    static int cap  = 0;      // maximum array capacity
    static int len  = 0;      // current array size
    static int last = 0;      // last = len - 1
    static int move = 0;      // value (not index) to be moved left or right
    // Derivative variables (mem divided into 4 chunks)
    static int *perm = NULL;  // permutation of index numbers 0..count-1
    static int *dir  = NULL;  // which way to move each value: -1=left, +1=right
    static int *pos  = NULL;  // inverse permutation (position of values)
    static int *next = NULL;  // state used to calc next move value

    // Wrong input or manual reset: free memory & return NULL
    if (count < 1)
        goto combperm_plainchanges_free;  // also reset

    // (Re-) initialisation on first call, or if count changes
    if (count != len) {
        if (count > cap) {  // new larger capacity, or init?
            int *tmp = realloc(mem, 4 * count * sizeof (int));  // 4x = perm+dir+pos+next
            if (tmp == NULL)  // memory allocation failed?
                goto combperm_plainchanges_free;  // also reset
            mem = tmp;
            cap = count;
        }
        // Divide mem into 4 chunks of size count
        perm = mem + count * 0;
        dir  = mem + count * 1;
        pos  = mem + count * 2;
        next = mem + count * 3;
        // Initialise arrays
        for (int i = 0; i < count; ++i) {
            perm[i] = i;
            dir[i] = -1;
        }
        // Copy init values: pos=perm, next=dir
        memcpy(pos, perm, 2 * count * sizeof (int));
        len = count;
        last = move = count - 1;
        return perm;
    }

    // End condition: if move is zero, next permutation would be
    // same as first permutation (= ordered array)
    // Separate check from count<1 to allow count==1 and save reallocation
    if (!move)
        goto combperm_plainchanges_reset;  // does not free memory

    // Step: look at neighbour left or right
    const int xi = pos[move];
    const int yi = xi + dir[move];
    const int zi = yi + dir[move];

    // Swap but remember replaced value for new pos index
    const int tmp = perm[yi];
    perm[yi] = move;
    perm[xi] = tmp;

    // Re-index inverse permutation
    pos[tmp ] = xi;
    pos[move] = yi;

    // Check boundary
    if (zi == -1 || zi == len || perm[zi] > move) {
        // took final step in current direction
        dir[move] = -dir[move];
        if (move == last) {
            // took final step and move is last
            if (next[last] < 0) {
                move = last - 1;
                if (-next[last] != last)
                    next[last - 1] = next[last];
            } else
                move = next[last] - 1;
        } else {
            // took final step and move is not last
            next[last] = -(move + 2);
            if (next[move] > 0)
                next[move + 1] = next[move];
            else {
                next[move + 1] = move;
                if (-next[move] != move)
                    next[move - 1] = next[move];
            }
            move = last;
        }
    } else if (move != last) {
        // not final step and move is not last
        next[last] = -(move + 1);
        move = last;
    }

    // Return next permutation
    return perm;

combperm_plainchanges_free:
    free(mem);
    mem = NULL;
    cap = 0;
combperm_plainchanges_reset:
    perm = dir = pos = next = NULL;
    len = last = move = 0;
    return NULL;
}
