/**
 * Freeware. No pull requests accepted.
 * See combinations-test.c and permutations-test.c
 * in this repo for an example program.
 *
 * Made by: E. Dronkert, Utrecht NL, May 2024.
 * https://github.com/ednl
 * https://mastodon.social/@ednl
 */

#include <stdlib.h>  // malloc, free
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

// Successive calls give permutations in lexicographic order of len index numbers.
// https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// NB: not thread-safe because permutations are stored in local static variable.
// Returns pointer to first element of next permutation of n index numbers.
// Call as permutations(0) to reset and free memory.
int *permutations(const int len)
{
    static int *index = NULL;
    static int n = 0;

    // Wrong input or manual reset: reset & return
    if (len < 1) {
        free(index);
        n = 0;
        return (index = NULL);
    }

    // (Re-) initialisation on first call or if len changes
    if (index == NULL || len != n) {
        int *tmp = realloc(index, len * sizeof *tmp);
        if (tmp) {
            index = tmp;
            n = len;
            for (int i = 0; i < len; ++i)
                index[i] = i;
        }
        return index;
    }

    // Find pivot_x: last element where next element is larger
    int px = n - 2;
    while (px >= 0 && index[px] >= index[px + 1])
        --px;

    // If not found, this was the final permutation: reset & return
    if (px < 0) {
        free(index);
        n = 0;
        return (index = NULL);
    }

    // Find pivot_y: last element larger than pivot_x and swap them
    int py = n - 1;
    while (index[px] >= index[py])
        --py;  // py will be at least px+1 because pivot_x < pivot_x+1
    swap(&index[px], &index[py]);

    // Reverse suffix = sequence after pivot_x
    for (int l = px + 1, r = n - 1; l < r; ++l, --r)
        swap(&index[l], &index[r]);

    // Return next permutation
    return index;
}

// Successive calls give permutations in "plain changes" order of len index numbers.
// https://en.wikipedia.org/wiki/Steinhaus–Johnson–Trotter_algorithm
// with improvement by Shimon Even, as implemented by:
// https://github.com/steppi/adeft/blob/master/src/adeft/score/permutations.pyx
// Second half of permutations are lexically reversed from first half:
//   perm(i) = reverse(perm(n!/2 + i)) for i=0..(n!/2)-1
// NB: not thread-safe because permutation and state are stored in local static variables.
// Returns pointer to first element of next permutation of n index numbers.
// Returns NULL (and memory is freed) when all permutations have been visited.
// Call as plainchanges(0) to reset and free memory.
int *plainchanges(const int len)
{
    static int *perm = NULL;  // permutation of index numbers 0..len-1
    static int *pos = NULL;   // inverse permutation (position of values)
    static int *dir = NULL;   // which way to move each value: -1=left, +1=right
    static int *next = NULL;  // state used to calc next move value
    static int curlen = 0;    // remember len for consistency/reset
    static int max = 0;       // max = len - 1
    static int move = 0;      // value to be moved

    // Wrong input or manual reset: reset & return NULL
    if (len < 1)
        goto combperm_plainchanges_reset;

    // (Re-) initialisation on first call, or if len changes
    if (len != curlen) {
        perm = realloc(perm, len * sizeof (int));
        pos  = realloc(pos , len * sizeof (int));
        dir  = realloc(dir , len * sizeof (int));
        next = realloc(next, len * sizeof (int));
        if (!perm || !pos || !dir || !next)
            goto combperm_plainchanges_reset;
        for (int i = 0; i < len; ++i) {
            perm[i] = pos[i] = i;
            dir[i] = next[i] = -1;
        }
        curlen = len;
        move = max = len - 1;
        return perm;
    }

    // End condition: if move is zero, next permutation
    // would be ordered array, same as first permutation
    if (!move)
        goto combperm_plainchanges_reset;

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
    if (zi == -1 || zi == curlen || perm[zi] > move) {
        // final step in current direction
        dir[move] = -dir[move];
        if (move == max) {
            // final step and move is max
            if (next[max] < 0) {
                move = max - 1;
                if (-next[max] != max)
                    next[max - 1] = next[max];
            } else
                move = next[max] - 1;
        } else {
            // final step and move is not max
            next[max] = -(move + 2);
            if (next[move] > 0)
                next[move + 1] = next[move];
            else {
                next[move + 1] = move;
                if (-next[move] != move)
                    next[move - 1] = next[move];
            }
            move = max;
        }
    } else if (move != max) {
        // not final step and move is not max
        next[max] = -(move + 1);
        move = max;
    }

    // Return next permutation
    return perm;

combperm_plainchanges_reset:
    free(perm); perm = NULL;
    free(pos ); pos  = NULL;
    free(dir ); dir  = NULL;
    free(next); next = NULL;
    curlen = move = max = 0;
    return NULL;
}
