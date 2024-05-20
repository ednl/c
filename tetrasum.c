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

#define SUM_MAX (INT_MAX / 6)
#define TAKE_MAX 128

// Calculate the n'th tetrahedral number
static int tetracalc(const int n)
{
    return ((n * (n + 1)) >> 1) * (n + 2) / 3;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Useage: %s <sum> <summands>\n", argv[0]);
        return 1;
    }

    int sum = 0, take = 0;
    long long t = atoll(argv[1]);
    if (t > 0 && t <= SUM_MAX)
        sum = (int)t;
    else {
        fprintf(stderr, "Argument 1 out of range: 0 < sum <= %d\n", SUM_MAX);
        return 2;
    }
    t = atoll(argv[2]);
    if (t > 0 && t <= TAKE_MAX)
        take = (int)t;
    else {
        fprintf(stderr, "Argument 2 out of range: 0 < summands <= %d\n", TAKE_MAX);
        return 3;
    }
    printf("Find all combinations of %d distinct tetrahedral numbers that sum to %d\n", take, sum);
    printf("------------------------------------------------------------------------------------------\n");

    // Precompute the first N tetrahedral numbers
    const int N = (int)(ceil(cbrt(sum * 6)));
    int *tetra = malloc((size_t)N * sizeof *tetra);
    for (int i = 0; i < N; ++i)
        tetra[i] = tetracalc(i + 1);  // do not use T(0)=0

    int *index = NULL, solutions = 0;
    while ((index = combinations(N, take))) {
        int s = 0;
        for (int i = 0; i < take; ++i)
            s += tetra[index[i]];
        if (s == sum) {
            printf("%u: T(%d)", ++solutions, index[0] + 1);  // adjust index for skipping T(0)
            for (int i = 1; i < take; ++i)
                printf("+T(%u)", index[i] + 1);  // adjust index for skipping T(0)
            printf(" = %u", tetra[index[0]]);
            for (int i = 1; i < take; ++i)
                printf("+%u", tetra[index[i]]);
            printf(" = %d\n", sum);
        }
    }
    printf("------------------------------------------------------------------------------------------\n");
    printf("Found %d different combinations of %d distinct tetrahedral numbers that sum to %d\n", solutions, take, sum);
    free(tetra);
    return 0;
}
