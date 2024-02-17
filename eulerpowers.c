// https://en.wikipedia.org/wiki/Euler%27s_sum_of_powers_conjecture
// Counterexample: find N Kth powers whose sum is equal to a Kth power where N<K.
// For N=4, K=5 should find: 144^5 = 27^5 + 84^5 + 110^5 + 133^5
// Code slightly optimised from https://www.reddit.com/r/math/comments/1apnbf5/has_there_even_been_a_famous_and_longstanding/kq92guo/
// Lots more optimisations at https://rosettacode.org/wiki/Euler%27s_sum_of_powers_conjecture

#include <stdio.h>   // printf
#include <stdlib.h>  // abs
#include <string.h>  // memcpy
#include <stdint.h>  // uint64_t
#include <math.h>    // round

#define N 7132U  // maximum N so that (N-1)^5 < 2^64

static uint64_t p[N];  // p[i] = i^5
#ifdef RESIDUE
static int8_t   r[N] = {0,1,-1,1,1,1,-1,-1,-1,1,-1};  // r[i] = i^5 % 11 (=-1/0/1)
#endif

static uint64_t ifourth(uint64_t x)
{
    x *= x;
    return x * x;
}

static uint64_t ififth(const uint64_t x)
{
    return ifourth(x) * x;
}

static double ffourth(double x)
{
    x *= x;
    return x * x;
}

static double ffifth(const double x)
{
    return ffourth(x) * x;
}

// Slower: round(exp(0.2 * log(x))
static double fast_root5(const double x) {
    const uint64_t magic = 3685583637919522816ULL;
    union {
        double f;
        uint64_t i;
    } u;
    u.f = x;
    u.i /= 5;
    u.i += magic;
    const double x5 = ffifth(u.f);
    return u.f * ((x - x5) / (3 * x5 + 2 * x)) + u.f;
}

int main(void)
{
    // List of fifth powers: p[i] = i^5
    for (uint64_t i = 0; i < N; ++i)
        p[i] = ififth(i);

    #ifdef RESIDUE
    // List of residues: r[i] = i^5 mod 11 = [-1,0,+1]
    const int8_t * const src = r;
    const int8_t * const end = r + sizeof r;
    size_t len = sizeof *r * 11;
    int8_t *dst = r + len, *nxt = dst + len;
    while (nxt <= end) {
        memcpy(dst, src, len);
        dst = nxt;
        len <<= 1;
        nxt = dst + len;
    }
    if (dst < end)
        memcpy(dst, src, end - dst);
    #endif

    // for (unsigned n = 5; n < N; ++n)  // N=7132 to avoid 64-bit overflow, limit N further for reasonable runtime
    for (unsigned n = 5; n < 721; ++n)
        for (unsigned a = 1; a < n - 3; ++a) {

            #ifdef RESIDUE
            const int8_t res1 = r[n] - r[a];  // first residue, no limitations here, but must end at 0 in inner loop
            #endif
            if (p[a] * 4 >= p[n]) break;
            const uint64_t partial_a = p[n] - p[a];

            for (unsigned b = a + 1; b < n - 2; ++b) {

                #ifdef RESIDUE
                const int8_t res2 = res1 - r[b];  // second residue must be |res2| <= 2
                if (abs(res2) > 2) continue;
                #endif
                if (p[b] * 3 >= partial_a) break;
                const uint64_t partial_b = partial_a - p[b];

                for (unsigned c = b + 1; c < n - 1; ++c) {

                    #ifdef RESIDUE
                    const int8_t res3 = res2 - r[c];  // third residue must be |res3| <= 1
                    if (abs(res3) > 1) continue;
                    #endif
                    if (p[c] * 2 >= partial_b) break;
                    const uint64_t partial_c = partial_b - p[c];
                    const unsigned d = (unsigned)(round(fast_root5_2(partial_c)));
                    if (p[d] == partial_c)  // last residue check is redundant if also using equality check
                        printf("%u^5 = %u^5 + %u^5 + %u^5 + %u^5\n", n, a, b, c, d);
                }
            }
        }

    return 0;
}
