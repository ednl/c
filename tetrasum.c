/******************************************************************************
 * Brady Haran of Numberphile asked: which 5 tetrahedral numbers sum to 343867?
 * This is my solution that finds 277 different combinations of 5 distinct
 * tetrahedral numbers. I implemented Donald Knuth's algorithm to find all
 * combinations of k elements from a set of n, from The Art of Computer
 * Programming - Volume 4A - Combinatorial Algorithms, §7.2.1.3, algorithm T.
 *
 * Compile: clang -std=gnu17 -Ofast -o tetrasum tetrasum.c combinations.c
 * Run: ./tetrasum 343867 5
 *
 * E. Dronkert
 * Utrecht, Netherlands
 * 2024-05-20
 * https://github.com/ednl/c/blob/master/tetrasum.c
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>  // atoll, malloc, free
#include <limits.h>  // INT_MAX
#include <math.h>    // cbrt, ceil
#include "combinations.h"

// Avoid overflow when determining tetrahedral number count to choose from.
#define TARGET_MAX (INT_MAX / 6)

// Arbitrary limit, but n!/(k!(n-k)!) grows fast.
#define PARTS_MAX 128

// Calculate the n'th tetrahedral number.
static int tetracalc(const int n)
{
    return ((n * (n + 1)) >> 1) * (n + 2) / 3;  // avoid early overflow by dividing in 2 steps
}

int main(int argc, char *argv[])
{
    // Must have two command line arguments: target (= sum) and parts (= how many parts make up the sum).
    if (argc != 3) {
        fprintf(stderr, "Useage: %s <target> <parts>\n", argv[0]);
        return 1;
    }

    // Check argument values.
    const long long a1 = atoll(argv[1]), a2 = atoll(argv[2]);
    if (a1 <= 0 || a1 > TARGET_MAX) {
        fprintf(stderr, "Argument 1 out of range: 0 < target <= %d\n", TARGET_MAX);
        return 2;
    }
    if (a2 <= 0 || a2 > PARTS_MAX) {
        fprintf(stderr, "Argument 2 out of range: 0 < parts <= %d\n", PARTS_MAX);
        return 3;
    }

    // Header.
    const int target = (int)a1, parts = (int)a2;
    printf("Find all combinations of %d distinct tetrahedral numbers that sum to %d:\n", parts, target);
    printf("------------------------------------------------------------------------------------------\n");

    // Precompute the first N tetrahedral numbers to choose from.
    const int N = (int)(ceil(cbrt(target * 6)));
    int *tetra = malloc((size_t)N * sizeof *tetra);
    for (int i = 0; i < N; ++i)
        tetra[i] = tetracalc(i);

    // Check all combinations.
    int *index = NULL, solutions = 0;
    while ((index = combinations(N, parts))) {
        // Skip first tetrahedral number T(0)=0 because a zero term is no term.
        if (!index[0])
            continue;
        // Sum of current combination.
        int sum = tetra[index[0]];
        for (int i = 1; i < parts; ++i)
            sum += tetra[index[i]];
        // Is this a solution?
        if (sum == target) {
            printf("%u: T(%d)", ++solutions, index[0]);
            for (int i = 1; i < parts; ++i)
                printf("+T(%d)", index[i]);
            printf(" = %d", tetra[index[0]]);
            for (int i = 1; i < parts; ++i)
                printf("+%d", tetra[index[i]]);
            printf(" = %d\n", target);
        }
    }

    // Footer.
    printf("------------------------------------------------------------------------------------------\n");
    printf("Found %d different combinations of %d distinct tetrahedral numbers that sum to %d.\n", solutions, parts, target);

    free(tetra);
    return 0;
}
