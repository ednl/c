// Times in milliseconds

// Pi5 | count | lexic | plain |
// ----+-------+-------+-------+
//  10 |       |       |       |
//  11 |       |       |       |
//  12 |       |       |       |
//  13 | 15600 | 51200 | 45600 |

//  M1 | count | lexic | plain |
// ----+-------+-------+-------+
//  10 |     7 |    15 |    10 |
//  11 |    83 |   164 |   103 |
//  12 |  1013 |  1977 |  1219 |
//  13 | 13300 | 25773 | 15771 |

//  M4 | count | lexic | plain |
// ----+-------+-------+-------+
//  10 |       |       |       |
//  11 |       |       |       |
//  12 |       |       |       |
//  13 |  1500 | 14300 | 11100 |

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

    volatile uint64_t count = 0;
    starttimer();
    switch (argv[1][0]) {
        case 'c': for (; count != fac; ++count); break;
        case 'l': for (int *a; (a = permutations(n)); ++count); break;
        case 'p': for (int *a; (a = plainchanges(n)); ++count); break;
    }
    const double t = stoptimer_ms();
    printf("Count: %"PRIu64" (%d! = %"PRIu64")\n", count, n, fac);
    printf("Time: %.0f ms\n", t);
}
