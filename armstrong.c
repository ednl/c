// https://en.wikipedia.org/wiki/Narcissistic_number
// https://oeis.org/A005188
//   1: 1, 2, 3, 4, 5, 6, 7, 8, 9
//   2:
//   3: 153, 370, 371, 407
//   4: 1634, 8208, 9474
//   5: 54748, 92727, 93084
//   6: 548834
//   7: 1741725, 4210818, 9800817, 9926315
//   8: 24678050, 24678051, 88593477
//   9: 146511208, 472335975, 534494836, 912985153
//  10: 4679307774
//  11: 32164049650, 32164049651, 40028394225, 42678290603, 44708635679, 49388550606, 82693916578, 94204591914

// c * (b - 1)^c = b^(c - 1)
// log(c) + c * log(b - 1) = (c - 1) * log(b)
// log(c) = c * log(b) - log(b) - c * log(b - 1)
// log(c) = c * log(b / (b - 1)) - log(b)

// b=10 : 1 <= c <= 60

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define BASE 10
#define MAXMAG (UINT64_MAX / BASE)
#define MAXPOW (UINT64_MAX / (BASE - 1))

uint64_t powers[BASE];

int main(void)
{
    for (int i = 1; i < BASE; ++i)  // p[0] = 0
        powers[i] = 1;  // x^0 = 1 for 1 <= x < BASE

    uint64_t n = 1;
    for (uint64_t magnitude = BASE; magnitude <= MAXMAG; magnitude *= BASE) {
        if (powers[BASE - 1] > MAXPOW) {
            fprintf(stderr, "Immininent overflow at n=%"PRIu64"\n", n);
            return 1;
        }
        for (int i = 2; i < BASE; ++i)
            powers[i] *= i;
        for (; n < magnitude; ++n) {
            uint64_t sum = 0;
            for (uint64_t x = n; x; x /= BASE) {
                uint64_t old = sum;
                sum += powers[x % BASE];
                if (sum < old)
                    break;
            }
            if (sum == n)
                printf("%"PRIu64"\n", n);  // Narcissistic or Armstrong number
        }
    }
    return 0;
}
