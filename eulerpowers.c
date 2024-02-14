// https://en.wikipedia.org/wiki/Euler%27s_sum_of_powers_conjecture

#include <stdio.h>
#include <stdint.h>

#define N 256u
#define K 5u
#define LEN 5u  // 4 or 5

static uint64_t p[N] = {0, 1};

int main(void)
{
    for (uint64_t i = 2; i < N; ++i) {
        uint64_t j = i;
        for (unsigned k = 1; k < K; ++k)
            j *= i;
        p[i] = j;
    }

    for (unsigned i = 1; i < N - 5; ++i)
        for (unsigned j = i + 1; j < N - 4; ++j) {
            uint64_t s2 = p[i] + p[j];
            if (s2 > p[N - 1]) break;
            for (unsigned k = j + 1; k < N - 3; ++k) {
                uint64_t s3 = s2 + p[k];
                if (s3 > p[N - 1]) break;
                for (unsigned l = k + 1; l < N - 2; ++l) {
                    uint64_t s4 = s3 + p[l];
                    if (s4 > p[N - 1]) break;
                #if LEN == 4
                    for (unsigned m = l + 1; m < N; ++m) {
                        if (p[m] < s4) continue;
                        if (p[m] > s4) break;
                        printf("%u^5 + %u^5 + %u^5 + %u^5 = %u^5\n", i, j, k, l, m);
                        // 27^5 + 84^5 + 110^5 + 133^5 = 144^5
                    }
                #elif LEN == 5
                    for (unsigned m = l + 1; m < N - 1; ++m) {
                        uint64_t s5 = s4 + p[m];
                        if (s5 > p[N - 1]) break;
                        for (unsigned n = m + 1; n < N; ++n) {
                            if (p[n] < s5) continue;
                            if (p[n] > s5) break;
                            printf("%u^5 + %u^5 + %u^5 + %u^5 + %u^5 = %u^5\n", i, j, k, l, m, n);
                            // 7^5 + 43^5 + 57^5 + 80^5 + 100^5 = 107^5
                            // 14^5 + 86^5 + 114^5 + 160^5 + 200^5 = 214^5 (common factor 2)
                            // 19^5 + 43^5 + 46^5 + 47^5 + 67^5 = 72^5
                            // 21^5 + 23^5 + 37^5 + 79^5 + 84^5 = 94^5
                            // 38^5 + 86^5 + 92^5 + 94^5 + 134^5 = 144^5 (common factor 2)
                            // 42^5 + 46^5 + 74^5 + 158^5 + 168^5 = 188^5 (common factor 2)
                            // 57^5 + 129^5 + 138^5 + 141^5 + 201^5 = 216^5 (common factor 3)
                        }
                    }
                #endif
                }
            }
        }

    return 0;
}
