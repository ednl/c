// Programmeeropdracht voor eerstejaars natuurkunde aan de UvA 2026
// via Ivo van Vulpen https://mu.social/profile/ivovanvulpen.bsky.social/post/3myfwtrxw2c2l
// https://nl.wikipedia.org/wiki/Bevriende_getallen

// Find the first pair of amicable numbers (a,b) where a,b<300
// https://en.wikipedia.org/wiki/Amicable_numbers

#include <stdio.h>

#define N 300
static unsigned aliquot[N];

int main(void)
{
    // Add proper divisors to make aliquot sums
    for (unsigned divisor = 1; divisor < N / 2; divisor++)
        for (unsigned multiple = 2 * divisor; multiple < N; multiple += divisor)
            aliquot[multiple] += divisor;

    // Find first amicable pair
    for (unsigned a = 1; a < N; a++) {
        const unsigned b = aliquot[a];
        if (a != b && b < N && aliquot[b] == a) {
            printf("%u,%u\n", a, b);
            return 0;  // success
        }
    }
    return 1;  // failure
}
