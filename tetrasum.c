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
#include <stdlib.h>  // atoll, malloc
#include <limits.h>  // INT_MAX
#include <math.h>    // cbrt, ceil
#include "combinations.h"

// Avoid overflow when determining tetrahedral number count.
#define SUM_MAX (INT_MAX / 6)

// Arbitrary limit, but n!/(k!(n-k)!) grows fast.
#define TAKE_MAX 128

// Calculate the n'th tetrahedral number.
static int tetracalc(const int n)
{
    return ((n * (n + 1)) >> 1) * (n + 2) / 3;  // avoid early overflow by dividing in 2 steps
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Useage: %s <sum> <summands>\n", argv[0]);  // summands = how many parts in the sum
        return 1;
    }
    const long long t1 = atoll(argv[1]), t2 = atoll(argv[2]);
    if (t1 <= 0 || t1 > SUM_MAX) {
        fprintf(stderr, "Argument 1 out of range: 0 < sum <= %d\n", SUM_MAX);
        return 2;
    }
    if (t2 <= 0 || t2 > TAKE_MAX) {
        fprintf(stderr, "Argument 2 out of range: 0 < summands <= %d\n", TAKE_MAX);
        return 3;
    }
    const int sum = (int)t1, take = (int)t2;
    printf("Find all combinations of %d distinct tetrahedral numbers that sum to %d:\n", take, sum);
    printf("------------------------------------------------------------------------------------------\n");

    // Precompute the first N tetrahedral numbers to choose from.
    const int N = (int)(ceil(cbrt(sum * 6)));
    int *tetra = malloc((size_t)N * sizeof *tetra);
    for (int i = 0; i < N; ++i)
        tetra[i] = tetracalc(i);

    int *index = NULL, solutions = 0;
    while ((index = combinations(N, take))) {
        // Skip first tetrahedral number T(0)=0 because a zero term is no term.
        if (!index[0])
            continue;

        // Sum of current combination.
        int combosum = 0;
        for (int i = 0; i < take; ++i)
            combosum += tetra[index[i]];

        // Is this a solution?
        if (combosum == sum) {
            printf("%u: T(%d)", ++solutions, index[0]);
            for (int i = 1; i < take; ++i)
                printf("+T(%u)", index[i]);
            printf(" = %u", tetra[index[0]]);
            for (int i = 1; i < take; ++i)
                printf("+%u", tetra[index[i]]);
            printf(" = %d\n", sum);
        }
    }
    printf("------------------------------------------------------------------------------------------\n");
    printf("Found %d different combinations of %d distinct tetrahedral numbers that sum to %d.\n", solutions, take, sum);
    free(tetra);
    return 0;
}
