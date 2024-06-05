#include <stdio.h>
#include <stdint.h>

#define BASE 10
#define LEN   4

// https://en.wikipedia.org/wiki/Kaprekar%27s_routine
static uint32_t kaprekar(uint32_t x)
{
    // Convert to array of digits
    uint8_t a[LEN];
    for (int i = 0; i < LEN; ++i) {
        a[i] = x % BASE;
        x /= BASE;
    }
    // Insertion sort
    for (int i = 1, j; i < LEN; ++i) {
        const uint8_t ins = a[i];
        for (j = i; j && a[j - 1] > ins; --j)
            a[j] = a[j - 1];
        if (j != i)
            a[j] = ins;
    }
    // Convert to numbers with ascending and descending digits
    uint32_t asc = 0, dsc = 0;
    for (int i = 0, j = LEN - 1; i < LEN; ++i, --j) {
        asc = asc * BASE + a[i];
        dsc = dsc * BASE + a[j];
    }
    // Return positive difference
    return dsc - asc;
}

int main(void)
{
    for (uint32_t n = 0; n <= 9999; ++n) {
        printf("%u", n);
        uint32_t prev, next = n, count = 0;
        while (1) {
            prev = next;
            next = kaprekar(prev);
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
