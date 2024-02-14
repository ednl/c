// https://en.wikipedia.org/wiki/Euler%27s_sum_of_powers_conjecture
// Counterexample: find N Kth powers whose sum is equal to a Kth power where N<K.
// For N=4, K=5 should find: 144^5 = 27^5 + 84^5 + 110^5 + 133^5
// Code slightly optimised from https://www.reddit.com/r/math/comments/1apnbf5/has_there_even_been_a_famous_and_longstanding/kq92guo/

#include <stdio.h>   // printf
#include <stdint.h>  // int64_t
#include <math.h>    // exp, log
// #include "startstoptimer.h"

static int64_t p[256];

static int64_t fourth(int64_t x)
{
    x *= x;
    x *= x;
    return x;
}

static int64_t fifth(int64_t x)
{
    return fourth(x) * x;
}

int main(void)
{
    // starttimer();
    for (int64_t i = 0; i < 256; ++i)
        p[i] = fifth(i);

    for (int64_t n = 5; n < 256; ++n) {
        const int64_t target = p[n];
        for (int64_t a = 1;; ++a) {
            const int64_t a5 = p[a];
            const int64_t res_a = target - a5;
            if (res_a <= a5) break;
            for (int64_t b = a + 1;; ++b) {
                const int64_t b5 = p[b];
                const int64_t res_b = res_a - b5;
                if (res_b <= b5) break;
                for (int64_t c = b + 1;; ++c) {
                    const int64_t c5 = p[c];
                    const int64_t res_c = res_b - c5;
                    if (res_c <= c5) break;
                    const int64_t d = (int64_t)(round(exp(0.2 * log(res_c))));
                    if (p[d] == res_c) {
                        printf("%lld^5 = %lld^5 + %lld^5 + %lld^5 + %lld^5\n", n, a, b, c, d);
                        // printf("Time: %.0f ms\n", stoptimer_ms());
                        return 0;
                    }
                }
            }
        }
    }
}
