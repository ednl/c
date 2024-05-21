/******************************************************************************
 * Brady Haran of Numberphile asked: which 5 tetrahedral numbers sum to 343867?
 * This is my solution that finds 277 different combinations of 5 distinct
 * tetrahedral numbers. I implemented Donald Knuth's algorithm to find all
 * combinations of k elements from a set of n, from The Art of Computer
 * Programming - Volume 4A - Combinatorial Algorithms, §7.2.1.3, algorithm T.
 *
 * Compile: clang -std=gnu17 -Ofast -o tetrasum tetrasum.c combinations.c
 * Run: ./tetrasum 343867 5     : show tetrahedral numbers and no. of solutions
 *  or: ./tetrasum -v 343867 5  : show everything (verbose)
 *  or: ./tetrasum -q 343867 5  : only show no. of solutions (quiet)
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
#include <string.h>  // strcmp

// My own "n choose k" combinations function.
#include "combinations.h"

// Avoid overflow when determining tetrahedral numbers to choose from.
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
    // Verbosity level.
    int argoffset = 0;
    int verbose = 1;
    if (argc > 1) {
        if (!strcmp(argv[1], "-q") || !strcmp(argv[1], "--quiet")) {
            argoffset = 1;
            verbose = 0;
        } else if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--verbose")) {
            argoffset = 1;
            verbose = 2;
        }
    }

    // Must have 2 or 3 command line arguments:
    //   (optional) quiet/verbose option
    //   target (= sum)
    //   parts (= how many parts make up the sum)
    if (argc - argoffset != 3) {
        if (verbose)
            fprintf(stderr, "Useage: %s [-q|-v] <target> <parts>\n", argv[0]);
        return 1;
    }

    // Check argument values.
    const long long a1 = atoll(argv[argoffset + 1]), a2 = atoll(argv[argoffset + 2]);
    if (a1 <= 0 || a1 > TARGET_MAX) {
        if (verbose)
            fprintf(stderr, "Argument 1 out of range: 0 < target <= %d.\n", TARGET_MAX);
        return 2;
    }
    if (a2 <= 0 || a2 > PARTS_MAX) {
        if (verbose)
            fprintf(stderr, "Argument 2 out of range: 0 < parts <= %d.\n", PARTS_MAX);
        return 3;
    }
    if (a1 <= a2) {
        if (verbose)
            fprintf(stderr, "Target (arg. 1) must be greater than parts (arg. 2).\n");
        return 4;
    }

    // Search parameters.
    const int target = (int)a1, parts = (int)a2, N = (int)(ceil(cbrt(target * 6)));

    // Final consistency check.
    if (N <= parts) {
        if (verbose)
            fprintf(stderr, "Cuberoot(target x 6) must be greater than parts. Increase target or decrease parts.\n");
        return 5;
    }

    // Precompute the first N tetrahedral numbers to choose from.
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
            ++solutions;
            if (verbose == 1) {
                printf("%d", tetra[index[parts - 1]]);
                for (int i = parts - 1; i > 0; )
                    printf(" %d", tetra[index[--i]]);
                printf("\n");
            } else if (verbose == 2) {
                printf("%u: T(%d)", solutions, index[parts - 1]);
                for (int i = parts - 1; i > 0; )
                    printf("+T(%d)", index[--i]);
                printf(" = %d", tetra[index[parts - 1]]);
                for (int i = parts - 1; i > 0; )
                    printf("+%d", tetra[index[--i]]);
                printf(" = %d\n", target);
            }
        }
    }

    // Summary.
    if (!verbose) {
        if (solutions)
            printf("%d\n", solutions);
    } else if (verbose == 1) {
        printf("Solutions: %d\n", solutions);
    } else if (verbose == 2) {
        if (solutions)
            printf("------------------------------------------------------------------------------------------\n");
        printf("Found %d different combinations of %d distinct tetrahedral numbers that sum to %d.\n", solutions, parts, target);
    }

    free(tetra);
    return 0;
}
