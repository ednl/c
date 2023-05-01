/******************************************************************
 * Roll set of special dice repeatedly, add up face values,
 *   next roll is with 'sum' of the same dice, stop if sum is zero.
 * Every die has face values: 0, 0, 0, 0, 2, 3.
 *
 * Ref. https://twitter.com/jamesgrime/status/1651896236457246721
 * By E. Dronkert https://github.com/ednl
 ******************************************************************/

#include <stdio.h>    // printf
#include <stdlib.h>   // arc4random, malloc, free
#include <string.h>   // memset
#include <time.h>     // time for srand
#include <stdint.h>   // UINT32_MAX
#include <stdbool.h>  // bool
#include "startstoptimer.h"

#define RNG         1  // use which RNG: 0=rand, 1=random, 2=arc4random, 3=arc4random_uniform
#define FACES       6  // each die has X faces
#define START1      1  // first start with X dice
#define START2     20  // last start with X dice
#define GAMES 1000000  // simulate X games per start

#define RDIVU   ((RAND_MAX + 1U ) / FACES)
#define RDIVUL  ((RAND_MAX + 1UL) / FACES)
#define RDIVULL ((1ULL << 32) / FACES)

static const uint64_t facevalue[FACES + 1] = {0, 0, 0, 0, 2, 3, 0};

static bool str2ui64(const char * str, uint64_t * val)
{
    long long a = atoll(str);
    if (a >= 0 && a <= UINT32_MAX) {
        *val = (uint64_t)a;
        return true;
    }
    return false;
}

static uint64_t roll_bad(void)
{
    unsigned int x = FACES;
    while (x == FACES)
        x = (unsigned int)rand() / RDIVU;
    return x;
}

static uint64_t roll_ok(void)
{
    unsigned long x = FACES;
    while (x == FACES)
        x = (unsigned long)random() / RDIVUL;
    return x;
}

static uint64_t roll_good(void)
{
    uint64_t x = FACES;
    while (x == FACES)
        x = (uint64_t)arc4random() / RDIVULL;
    return x;
}

static uint64_t roll_excellent(void)
{
    return arc4random_uniform(FACES);
}

int main(int argc, char * argv[])
{
    uint64_t (*roll)(void) = NULL;
    uint64_t rng = RNG, start1 = START1, start2 = START2, games = GAMES;
    uint64_t histsize = 0;
    uint64_t *hist = NULL;

    starttimer();

    int argi = 1;
    while (argi < argc) {
        uint64_t a = 0;
        if (str2ui64(argv[argi], &a)) {
            if (argi == 1 && a < 4) {
                rng = a;
            } else if ((argi == 1 || argi == 2) && a > 0 && a <= 100) {
                start1 = start2 = a;
            } else if ((argi == 1 || argi == 2 || argi == 3) && a > start1 && a <= 100)
                start2 = a;
            else if (a > 100)
                games = a;
        }
        ++argi;
    }

    printf("rng engine      : ");
    switch (rng) {
        case 0:
            printf("rand");
            roll = roll_bad;
            // srand((unsigned int)time(NULL));
            sranddev();
            break;
        case 1:
            printf("random");
            roll = roll_ok;
            // srandom((unsigned int)time(NULL));
            srandomdev();
            break;
        case 2:
            printf("arc4random");
            roll = roll_good;
            break;
        case 3:
            printf("arc4random_uniform");
            roll = roll_excellent;
            break;
        default:
            fprintf(stderr, "Internal error: invalid RNG\n");
            return 1;
    }
    printf("()\n");

    if (start1 == start2) {
        printf("start with dice : %llu\n", start1);
    } else {
        printf("start dice from : %llu\n", start1);
        printf("start dice to   : %llu\n", start2);
    }
    printf("games per start : %llu\n\n", games);

    printf("dice,maxdice,maxrolls,exprolls\n");
    for (uint64_t start = start1; start <= start2; ++start) {
        printf("%llu,", start);
        memset(hist, 0, histsize * sizeof *hist);

        uint64_t maxdice = 0, maxrolls = 0, progress = games / 50;
        for (uint64_t i = 0; i < games; ++i) {
            if (!(i % progress))
                fprintf(stderr, ".");
            uint64_t dice = start, rolls = 0;
            while (dice) {
                if (dice > maxdice)
                    maxdice = dice;
                uint64_t sum = 0;
                while (dice--)
                    sum += facevalue[(*roll)()];
                ++rolls;
                dice = sum;
            }
            if (rolls > maxrolls)
                maxrolls = rolls;
            if (rolls >= histsize) {
                uint64_t newsize = rolls + 100;
                void *p = realloc(hist, newsize * sizeof *hist);
                if (p) {
                    hist = p;
                    memset(&hist[histsize], 0, (newsize - histsize) * sizeof *hist);
                    histsize = newsize;
                } else {
                    free(hist);
                    fprintf(stderr, "Out of memory\n");
                    exit(1);
                }
            }
            hist[rolls]++;
        }
        printf("%llu,%llu,", maxdice, maxrolls);

        double expectation = 0;
        for (uint64_t i = 1; i <= maxrolls; ++i)
            expectation += (double)(hist[i] * i) / games;
        printf("%.3f\n", expectation);
    }

    free(hist);
    printf("\nTime: %.1f s\n", stoptimer_s());
    return 0;
}
