/**
 * Iterator function for generating combinations,
 * analoguous to Python's itertools.combinations.
 * Adapted from Knuth 4A, §7.2.1.3, algorithm T.
 * Freeware. No pull requests accepted.
 *
 * See combinations-test.c in this repo for an example program.
 *
 * Made by: E. Dronkert, Utrecht NL, May 2024.
 * https://github.com/ednl
 * https://mastodon.social/@ednl
 */

#include <stdlib.h>  // malloc, free
#include "combinations.h"

// Successive calls give different combinations of k indices
// from a set of n. Returns pointer to first element of array
// of length k.
//
// For example: three calls of `combinations(3,2)` give three
// (identical) pointers to arrays: [1,0], [2,0] and [2,1].
// The fourth call will return NULL (and memory is freed).
//
// To reset and free memory before all combinations have been
// generated, call with n<=0 or k<=0 or n<=k.
int *combinations(const int n, const int k)
{
    static int *index = NULL;
    static int j, x;

    // Wrong input, or reset.
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
