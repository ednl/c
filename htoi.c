#include <stdio.h>
#include <ctype.h>   // isspace
#include <stdlib.h>  // strtoul
#include <limits.h>  // INT_MAX
#include <stdbool.h>
#include "startstoptimer.h"

// Two hex digits per byte
#define HEXDIGITS (sizeof(unsigned) << 1)
#define TOPNIBBLE (0xFU << ((HEXDIGITS - 1) << 2))

// c >= 32 && c < 128
static inline bool readable_ascii(const char c)
{
    return c & 0x60;
}

// Overflow if left-shifted by 4
static inline bool overflow_danger(const unsigned x)
{
    return x & TOPNIBBLE;
}

// c in [0..9,A..F,a..f]
static inline bool ishexdigit(const char c)
{
    return readable_ascii(c) && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}

// Hexadecimal ASCII byte string to (unsigned) int
static unsigned htoi(const char *s)
{
    if (!s)
        return 0;
    while (isspace(*s))  // skip leading spaces
        ++s;
    if (*s == '0' && (*(s + 1) | 0x20) == 'x')  // set bit 5 = to lower case
        s += 2;  // skip "0x" hex prefix
    unsigned x = 0;
    while (readable_ascii(*s) && !overflow_danger(x)) {
        const char c = *s++ | 0x20;  // doesn't affect '0'..'9' and 0x10..0x19 already excluded
        if (c >= '0' && c <= '9')
            x = x << 4 | (c & 0xF);  // times 16, add 0..9
        else if (c >= 'a' && c <= 'f')
            x = x << 4 | ((c & 0x7) + 9);  // times 16, add 10..15
        else
            return x;  // stop at first non-hexdigit
    }
    return ishexdigit(*s) ? UINT_MAX : x;  // too many hex digits = error
}

static unsigned htoi_alt(const char *s)
{
    unsigned long x = strtoul(s, NULL, 16);
    return x > UINT_MAX ? UINT_MAX : x;
}

int main(void)
{
    volatile unsigned x;

    starttimer();
    for (int i = 0; i < 1000000000; ++i)
        x = htoi(" 0xCafeBabe");
    printf("%.2f s\n", stoptimer_s());

    starttimer();
    for (int i = 0; i < 1000000000; ++i)
        x = htoi_alt(" 0xCafeBabe");
    printf("%.2f s\n", stoptimer_s());

    return 0;
}
