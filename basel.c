#include <stdio.h>    // printf
#include <stdlib.h>   // arc4random, NULL
#include <stdint.h>   // uint64_t, UINT64_MAX
#include <stdbool.h>  // bool, true, false
#include <math.h>     // M_PI

static uint64_t rnd(void)
{
    uint64_t n;
    arc4random_buf(&n, sizeof n);
    return n;
}

static bool coprime(uint64_t a, uint64_t b)
{
    uint64_t r;

    if (b > a) {
        r = a;
        a = b;
        b = r;
    }
    while (b) {
        r = a % b;
        a = b;
        b = r;
    }
    return a == 1;
}

int main(void)
{
    uint64_t i, pairs = 0, coprimes = 0;
    long double pi, err, prev = 0;

    while (true) {
        for (i = 0; i < 1 << 8; ++i) {
            ++pairs;
            if (coprime(rnd(), rnd())) {
                ++coprimes;
            }
        }
        pi = sqrtl((long double)pairs / coprimes * 6);
        err = pi - M_PI;
        if (signbit(err) ^ signbit(prev)) {
            printf("%.10Lf %+.10Lf\n", pi, err);
        }
        prev = err;
    }
    return 0;
}
