#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CACHEPOW 32
#define CACHEMAX (UINT64_C(1) << CACHEPOW)

int main(void)
{
    uint16_t *cache = calloc(CACHEMAX, sizeof *cache);
    if (!cache) {
        fprintf(stderr, "Unable to claim 2^%d x %zu bytes of memory.\n", CACHEPOW, sizeof *cache);
        return EXIT_FAILURE;
    }

    puts("1 1 0");
    uint16_t i = 1, maxlen = 0;
    for (uint64_t n = 2; n; ++n) {
        uint16_t len = 0;
        for (uint64_t k = n; k > 1; ) {
            while (k & 1) {  // odd
                k += (k >> 1) + 1;  // k = (3k+1)/2
                len += 2;
            }
            const int z = __builtin_ctzll(k);
            k >>= z;
            len += z;
            if (k < CACHEMAX && cache[k]) {
                len += cache[k];
                break;
            }
        }
        if (n < CACHEMAX && !cache[n])
            cache[n] = len;
        if (len > maxlen)
            printf("%u %llu %u\n", ++i, n, maxlen = len);
    }
    free(cache);
    return 0;
}
