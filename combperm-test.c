// Testing permutation algorithms
// There are n-factorial possible permutations of an array with n elements.
//   10! =     3,628,800
//   11! =    39,916,800
//   12! =   479,001,600
//   13! = 6,227,020,800
//
// Times in milliseconds:
//
// Pi5 | count | lexic | plain |
// ----+-------+-------+-------+
//  10 |     9 |    30 |    23 |
//  11 |   100 |   325 |   263 |
//  12 |  1200 |  3927 |  3556 |
//  13 | 15594 | 50896 | 45340 |
//
//  i5 | count | lexic | plain | Core i5-8250U 1.8GHz
// ----+-------+-------+-------+
//  10 |     5 |    20 |    37 |
//  11 |    60 |   224 |   406 |
//  12 |   728 |  2693 |  4945 |
//  13 |  9507 | 35114 | 64357 |
//
//  M1 | count | lexic | plain |
// ----+-------+-------+-------+
//  10 |     7 |    15 |    10 |
//  11 |    83 |   164 |   103 |
//  12 |  1013 |  1977 |  1219 |
//  13 | 13300 | 25773 | 15771 |
//
//  M4 | count | lexic | plain |
// ----+-------+-------+-------+
//  10 |     1 |     8 |     6 |
//  11 |     9 |    96 |    73 |
//  12 |   109 |  1165 |   883 |
//  13 |  1580 | 15257 | 11676 |

#include <stdio.h>
#include <stdlib.h>    // atoi
#include <string.h>    // strlen
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include "combperm.h"
#include "startstoptimer.h"

static int errmsg(const char *const progname, const int errval)
{
    fprintf(stderr, "Test permutation algorithms\n");
    fprintf(stderr, "Usage : %s p|l|c <n>\n", progname);
    fprintf(stderr, "   p  : 'plain changes' (Steinhaus-Johnson-Trotter)\n");
    fprintf(stderr, "   l  : lexicographic order\n");
    fprintf(stderr, "   c  : only count to n factorial, no permutations\n");
    fprintf(stderr, "  <n> : integer number >=1 and <=20 (practically <=13)\n");
    return errval;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        return errmsg(argv[0], 1);
    if (strlen(argv[1]) != 1 || strlen(argv[2]) > 2)
        return errmsg(argv[0], 2);
    if (!strchr("plc", argv[1][0]))
        return errmsg(argv[0], 3);

    const int n = atoi(argv[2]);
    if (n < 1 || n > 20)
        return errmsg(argv[0], 4);

    uint64_t fac = 1;
    for (int i = 2; i <= n; ++i)
        fac *= i;

    const char alg = argv[1][0];
    volatile uint64_t count = 0;

    starttimer();
    switch (alg) {
        case 'c': for (; count != fac; ++count); break;
        case 'l': for (int *a; (a = permutations(n)); ++count); break;
        case 'p': for (int *a; (a = plainchanges(n)); ++count); break;
    }
    const double t = stoptimer_ms();
    printf("Count: %"PRIu64" (%d! = %"PRIu64")\n", count, n, fac);
    printf("Time: %.0f ms\n", t);
    plainchanges(0);  // free mem
}
