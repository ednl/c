#include <stdio.h>
#include <stdlib.h>   // rand
#include <limits.h>   // CHAR_BIT
#include <math.h>     // sqrt, floor
#include "./startstoptimer.h"

// Width of binary representation without leading zeros.
// Range: 1..32 (0 => 1).
// clz = Count Leading Zeros (undefined for n=0)
// Index of most significant bit that is set.
// Range: 0..31 (0 => 0).
static int msb_index(const int n)
{
    return (int)sizeof n * CHAR_BIT - 1 - __builtin_clz(n | 1);
}

// Integer square root
// https://en.wikipedia.org/wiki/Integer_square_root#Example_implementation_in_C
static int isqrt(const int n)
{
    if (n <= 1)
        return n;

    int x0, x1 = 1 << (msb_index(n) / 2 + 1);
    do {
        x0 = x1;
        x1 = (x0 + n / x0) / 2;
    } while (x0 > x1);
    return x0;
}

int main(void)
{
    int sum;

    sum = 0;
    srand(0);
    starttimer();
    for (int i = 0; i < 10000; ++i)
        sum += floor(sqrt(rand()));
    printf("Time: %.0f us\n", stoptimer_us());
    printf("%d\n", sum);

    sum = 0;
    srand(0);
    starttimer();
    for (int i = 0; i < 10000; ++i)
        sum += isqrt(rand());
    printf("Time: %.0f us\n", stoptimer_us());
    printf("%d\n", sum);
}
