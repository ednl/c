#include <stdio.h>
#include <stdint.h>  // uint32_t, uint64_t, UINT64_C

// Modular exponentiation: calculate remainder r = b^e mod m
// Ref. https://en.wikipedia.org/wiki/Modular_exponentiation
static uint32_t modpow(uint64_t base, uint64_t exponent, const uint64_t modulus)
{
    if (modulus == 1)
        return 0;  // shortcut
    if (modulus == 0 || modulus > (UINT64_C(1) << 32))
        return -1;  // error (but also maximum unsigned value)
    uint64_t rem = 1;
    base %= modulus;  // now: base <= modulus-1
    for (; exponent; exponent >>= 1) {
        if (exponent & 1)  // current LSB set?
            rem = (rem * base) % modulus;  // parentheses for clarity
        base = (base * base) % modulus;  // overflow if base >= 1<<32
    }
    return rem;  // 0 <= rem < 1<<32 because modulus <= 1<<32
}

int main(void)
{
    printf("%u\n", modpow(4, 13, 497));  // 445
}
