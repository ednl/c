#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

// Modular exponentiation
// (modulus-1)^2 must not overflow in 'base' which is guaranteed
// if modulus is 32-bit and base is 64-bit.
uint64_t modpow(uint64_t base, uint64_t exponent, const uint64_t modulus)
{
    if (modulus == 1)
        return 0;
    if (modulus > (UINT64_C(1) << 32))
        return -1;
    uint64_t r = 1;
    base %= modulus;  // base <= modulus-1
    for (; exponent; exponent >>= 1) {
        if (exponent & 1)
            r = r * base % modulus;
        base = base * base % modulus;  // overflow if base >= 1<<32
    }
    return r;  // 0 <= r < 1<<32
}

int main(void)
{
    printf("%"PRIu64"\n", modpow(4, 13, 497));
}
