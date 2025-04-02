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
    while ((a = permutations(n))) {
        ++count;
        // printf("%3d:", ++count);
        // for (int i = 0; i < n; ++i)
        //     printf(" %d", a[i]);
        // printf("\n");
    }
    printf("%llu\n", count);
    printf("Time: %.1f s\n", stoptimer_s());  // n=13 => t=24.7 s on Apple M1 3.2 GHz
    return 0;
}
