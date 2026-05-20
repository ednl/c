#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <inttypes.h>  // PRIu64
#include "combperm.h"
#include "startstoptimer.h"

#define N 13
#define PERM_PLAIN
// #define PERM_LEXIC

int main(void)
{
    starttimer();
    volatile uint64_t count = 0;
#if defined PERM_PLAIN
    for (int *a; (a = plainchanges(N)); ++count);  // M4: 11.2 s, M1: 15.7 s, Pi5: 44.5 s
#elif defined PERM_LEXIC
    for (int *a; (a = permutations(N)); ++count);  // M4: 14.4 s, M1: 25.4 s, Pi5: 51.2 s
#else
    for (; count < 6227020800U; ++count);          // M4: ? s, M1: 13.3 s, Pi5: ? s
#endif
    printf("%"PRIu64"\n", count);
    printf("Time: %.1f s\n", stoptimer_s());
}
