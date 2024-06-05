#include <stdio.h>
#include <stdint.h>
// #include "startstoptimer.h"

#define BASE 10
#define LEN   4

// https://en.wikipedia.org/wiki/Kaprekar%27s_routine
static uint32_t kaprekar(uint32_t x)
{
    // Convert to array of digits
    uint8_t digit[LEN];
    for (int i = 0; i < LEN; ++i) {
        digit[i] = x % BASE;
        x /= BASE;
    }
    // Insertion sort
    for (int i = 1, j; i < LEN; ++i) {
        const uint8_t ins = digit[i];
        for (j = i; j && digit[j - 1] > ins; --j)
            digit[j] = digit[j - 1];
        if (j != i)
            digit[j] = ins;
    }
    // Return positive difference
    uint32_t diff = 0;
    for (int i = 0, j = LEN - 1; i < LEN; ++i, --j)
        diff = diff * BASE + digit[j] - digit[i];
    return diff;
}

int main(void)
{
    // starttimer();
    for (uint32_t n = 0; n <= 9999; ++n) {
        printf("%0*u", LEN, n);
        uint32_t next = n, count = 0;
        while (1) {
            uint32_t prev = next;
            next = kaprekar(prev);
            if (next != prev) {
                ++count;
                printf(" %0*u", LEN, next);  // show intermediate numbers
            } else
                break;
        }
        printf(" %u\n", count);  // how many steps to loop of length 1
    }
    // printf("Time: %.0f µs\n", stoptimer_us());
    return 0;
}
