#include <stdio.h>  // printf
#include <time.h>   // clock_gettime

#define LIM (1000000UL)   // count number of primes which are smaller than this
#define SIZE (LIM >> 1)   // uneven number count
#define FIVESEC (CLOCKS_PER_SEC * 5)

static unsigned char prime[SIZE] = {0};

int main(void)
{
    unsigned long i, fac, nxt, dif, count, passes = 0;
    clock_t stop = clock() + FIVESEC;

    do {
        // Init
        for (i = 1; i < SIZE; ++i) {
            prime[i] = 1;
        }

        // Sieve
        fac = 1;
        nxt = 4;
        dif = 3;
        while (nxt < SIZE) {
            if (prime[fac]) {
                for (i = nxt; i < SIZE; i += dif) {
                    prime[i] = 0;
                }
            }
            ++fac;
            nxt += 3;
            dif += 2;
        }

        // Loop
        ++passes;
    } while (clock() < stop);

    // Count
    count = (LIM >= 2);
    for (i = 1; i < SIZE; ++i) {
        count += prime[i];
    }

    // Result
    printf("%lu %lu\n", count, passes);
    return 0;
}
