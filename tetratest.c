// Compile:
// clang -std=gnu17 -Wall -Wextra -Ofast tetratest.c startstoptimer.c
// gcc   -std=gnu17 -Wall -Wextra -Ofast tetratest.c startstoptimer.c -lm

#include <stdio.h>
#include <stdlib.h>  // rand, srand
#include <math.h>    // cbrt, floor
#include <stdint.h>  // uint32_t
#include <time.h>    // time
#include "startstoptimer.h"

#define CBR6 1.8171205928321397  // cube root of 6
#define LOOP (1<<25)  // 33.6M

// Te(2952) = 4291795704
// Te(2953) will overflow in uint32_t
#define TE_COUNT 2953
static uint32_t Te[TE_COUNT];

int main(void)
{
    srand(time(NULL));

    Te[0] = 0;
    Te[1] = 1;
    for (int n = 2; n < TE_COUNT; ++n)
        Te[n] = Te[n - 1] - Te[n - 2] + Te[n - 1] + (uint32_t)n;  // no overflow

    volatile uint32_t r;
    volatile int hit;

    starttimer();
    for (int i = 0; i < LOOP; ++i)
        r = (uint32_t)(rand());
    double t0 = stoptimer_ms();
    printf("base     :%5.0f ms\n", t0);

    hit = 0;
    starttimer();
    for (int i = 0; i < LOOP; ++i) {
        uint32_t t = (uint32_t)(rand());
        int lo = 0, hi = TE_COUNT - 1;
        if (t == Te[lo] || t == Te[hi]) {
            ++hit;
            continue;
        }
        while (lo < hi) {
            int m = ((lo + 1) >> 1) + (hi >> 1);
            if (m == lo || m == hi)
                break;
            if      (Te[m] < t) lo = m;
            else if (Te[m] > t) hi = m;
            else {
                ++hit;
                break;
            }
        }
    }
    double t1 = stoptimer_ms();
    printf("bsearch  :%5.0f ms\n", t1 - t0);

    hit = 0;
    starttimer();
    for (int i = 0; i < LOOP; ++i) {
        uint32_t t = (uint32_t)(rand());
        int m = (int)(floor(cbrt(t) * CBR6));
        while (m < TE_COUNT && Te[m] < t)
            ++m;
        hit += m < TE_COUNT && Te[m] == t;
    }
    double t2 = stoptimer_ms();
    printf("cbrt(6t) :%5.0f ms\n", t2 - t0);

    printf("Speed-up : %.1fx\n", (t1 - t0) / (t2 - t0));
    return 0;
}
