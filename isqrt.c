#include <stdio.h>    // printf
#include <stdlib.h>   // NULL
#include <limits.h>   // CHAR_BIT
#include <stdint.h>   // uint64_t
#include <stdbool.h>  // bool

// Width of binary representation without leading zeros.
// Range: 1..64 (0 => 1).
// clz = Count Leading Zeros (undefined for n=0)
static inline int bit_width(const uint64_t n)
{
    return (int)sizeof(n) * CHAR_BIT - __builtin_clzll(n | 1);
}

// Index of most significant bit that is set.
// Range: 0..63 (0 => 0).
static inline int msb_index(uint64_t n)
{
    return bit_width(n) - 1;
}

// Binary representation of number n, no leading zeros.
// Use buf if provided (must be at least 65 char long).
static char* bin(const uint64_t n, char* const buf)
{
    static char sbuf[sizeof(n) * CHAR_BIT + 1];
    char* a = buf ? buf : sbuf;
    uint64_t testbit = (uint64_t)1 << msb_index(n);
    do {
        *a++ = n & testbit ? '1' : '0';
    } while (testbit >>= 1);
    *a = '\0';
    return buf ? buf : sbuf;
}

// Integer square root
// https://en.wikipedia.org/wiki/Integer_square_root#Example_implementation_in_C
static uint64_t isqrt(const uint64_t n)
{
    if (n <= 1)
        return n;

    uint64_t x0, x1 = (uint64_t)1 << (msb_index(n) / 2 + 1);
    do {
        x0 = x1;
        x1 = (x0 + n / x0) / 2;
    } while (x0 > x1);
    return x0;
}

// https://en.wikipedia.org/wiki/Pronic_number
static bool is_pronic(const uint64_t n)
{
    if (n & 1)
        return false;
    const uint64_t r = isqrt(n);
    return r * (r + 1) == n;
}

int main(void)
{
    for (uint64_t i = 0; i <= 100; ++i)
        if (is_pronic(i))
            printf("%2llu = %7s\n", i, bin(i, NULL));
    return 0;
}
