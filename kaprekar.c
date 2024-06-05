// https://en.wikipedia.org/wiki/Kaprekar%27s_routine

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BASE 10
#define LEN   4

static int cmpbyte(const void *p, const void *q)
{
    const uint8_t a = *(const uint8_t *)p;
    const uint8_t b = *(const uint8_t *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static uint32_t kap(uint32_t x)
{
    uint8_t d[LEN];
    for (int i = 0; i < LEN; ++i) {
        d[i] = x % BASE;
        x /= BASE;
    }
    qsort(d, LEN, sizeof *d, cmpbyte);
    uint32_t a = 0, b = 0;
    for (int i = 0; i < LEN; ++i) {
        a = a * BASE + d[LEN - 1 - i];
        b = b * BASE + d[i];
    }
    return a - b;
}

int main(void)
{
    uint32_t prev, next, count;
    for (uint32_t n = 0; n <= 9999; ++n) {
        printf("%u", n);
        next = n;
        count = 0;
        while (1) {
            prev = next;
            next = kap(prev);
            if (next != prev) {
                ++count;
                // printf(" %u", next);
            } else
                break;
        }
        printf(" %u\n", count);
    }
    return 0;
}
