#include <stdio.h>
#include <stdint.h>
#include "combperm.h"
#include "startstoptimer.h"

int main(void)
{
    starttimer();
    const int n = 13;
    int *a = NULL;
    uint64_t count = 0;
    // while ((a = permutations(n))) {  // M4: 14.4 s, M1: 24.7 s
    while ((a = plainchanges(n))) {  // M4: 11.2 s, M1: ? s
        ++count;
        // printf("%3d:", ++count);
        // for (int i = 0; i < n; ++i)
        //     printf(" %d", a[i]);
        // printf("\n");
    }
    printf("%llu\n", count);
    printf("Time: %.1f s\n", stoptimer_s());
    return 0;
}
