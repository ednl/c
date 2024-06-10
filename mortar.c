#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static uint64_t lcm(const uint64_t a, const uint64_t b)
{
    if (!a || !b)
        return 0;
    uint64_t p, q, r;
    if (a >= b) {
        p = a;
        q = b;
    } else {
        p = b;
        q = a;
    }
    while (q) {
        r = p % q;
        p = q;
        q = r;
    }
    return a / p * b;
}

static bool mortar(uint64_t N, uint64_t n)
{
    return n > 1 ? !(N % n) && mortar(N, n - 1) : true;
}

int main(void)
{
    // n = floor(sqrt(N))
    for (uint64_t N = 1, n = 1, next = 1;; ++n) {
        next += (n << 1) + 1;
        if (next < N)
            break;
        for (; N < next; ++N)
            if (mortar(N, n))
                printf("%llu\n", N);
    }
    return 0;
}
