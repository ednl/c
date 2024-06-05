#include <stdio.h>
#include <stdlib.h>  // atoi
#include <stdint.h>  // uint64_t
// #include "startstoptimer.h"

#define MINBASE  2
#define MAXBASE 36
#define MAXLEN  19  // 10^19 < 2^64
#define MAXLOOP 64

#define BASE 10
#define LEN   4

static const char *digitchar = "0123456789abcdefghijklmnopqrstuvwxyz";

static void putnum(uint64_t x, const int base, const int len)
{
    // if (base < MINBASE || base > MAXBASE || len < 1 || len > MAXLEN)
    //     return;
    char s[MAXLEN];
    int i = 0;
    while (i < len && x) {
        s[i++] = digitchar[x % base];
        x /= base;
    }
    while (i < len)
        s[i++] = '0';
    while (i--)
        putchar(s[i]);
}

// https://en.wikipedia.org/wiki/Kaprekar%27s_routine
static uint64_t kaprekar(uint64_t x, const int base, const int len)
{
    // if (base < MINBASE || base > MAXBASE || len < 1 || len > MAXLEN)
    //     return 0;
    // Convert to array of digit values
    uint8_t digit[MAXLEN] = {0};
    for (int i = 0; i < len && x; ++i) {
        digit[i] = x % base;
        x /= base;
    }
    // Insertion sort
    for (int i = 1, j; i < len; ++i) {
        const uint8_t ins = digit[i];
        for (j = i; j && digit[j - 1] > ins; --j)
            digit[j] = digit[j - 1];
        if (j != i)
            digit[j] = ins;
    }
    // Return positive difference
    uint64_t diff = 0;
    for (int i = 0, j = len - 1; i < len; ++i, --j)
        diff = diff * base + digit[j] - digit[i];
    return diff;
}

int main(int argc, char *argv[])
{
    // starttimer();
    int base = BASE;
    int len = LEN;
    if (argc > 1) {
        int t = atoi(argv[1]);
        if (t >= 1 && t <= MAXLEN)
            len = t;
    }
    if (argc > 2) {
        int t = atoi(argv[2]);
        if (t >= MINBASE && t <= MAXBASE)
            base = t;
    }

    uint64_t end = 1;
    for (int i = 0; i < len; ++i)
        end *= base;

    for (uint64_t n = 0; n < end; ++n) {
        putnum(n, base, len);
        uint64_t next = n;
        int count = 0;
        while (1) {
            uint64_t prev = next;
            next = kaprekar(prev, base, len);
            if (next != prev) {
                ++count;
                // Show intermediate numbers
                putchar(' ');
                putnum(next, base, len);
            } else
                break;
        }
        printf(" (%d,1)\n", count);  // how many steps to loop of length 1
    }
    // printf("Time: %.0f µs\n", stoptimer_us());
    return 0;
}
