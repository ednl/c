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
