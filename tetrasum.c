#include <stdio.h>
#include <stdint.h>

#define N 128
static uint32_t n[N];
static const uint32_t sum = 343867;

static uint32_t tetra(const uint32_t i)
{
    return ((i * (i + 1)) >> 1) * (i + 2) / 3;
}

int main(void)
{
    for (unsigned i = 0; i < N; ++i)
        n[i] = tetra(i);

    unsigned count = 0;
    for (unsigned i = 0; i < N - 4; ++i) {
        unsigned a = n[i];
        if (a > sum) break;
        for (unsigned j = i + 1; j < N - 3; ++j) {
            unsigned b = a + n[j];
            if (b > sum) break;
            for (unsigned k = j + 1; k < N - 2; ++k) {
                unsigned c = b + n[k];
                if (c > sum) break;
                for (unsigned l = k + 1; l < N - 1; ++l) {
                    unsigned d = c + n[l];
                    if (d > sum) break;
                    for (unsigned m = l + 1; m < N; ++m) {
                        unsigned e = d + n[m];
                        if (e > sum) break;
                        if (e == sum)
                            printf("%3u: t(%2u) + t(%2u) + t(%2u) + t(%2u) + t(%3u) = %6u + %6u + %6u + %6u + %6u = %6u\n", ++count, i, j, k, l, m, n[i], n[j], n[k], n[l], n[m], sum);
                    }
                }
            }
        }
    }
    return 0;
}
