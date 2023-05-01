/******************************************************************
 * Roll set of special dice repeatedly, add up face values,
 *   next roll is with 'sum' of the same dice, stop if sum is zero.
 * Every die has face values: 0, 0, 0, 0, 2, 3.
 *
 * Ref. https://twitter.com/jamesgrime/status/1651896236457246721
 * By E. Dronkert https://github.com/ednl
 ******************************************************************/

#include <stdio.h>     // printf
#include <stdlib.h>    // rand, random, arc4random, realloc, free
#include <string.h>    // memset
#include <stdint.h>    // UINT64_MAX
#include <inttypes.h>  // PRIu64
#include <stdbool.h>   // bool
#ifdef __linux__
#include <time.h>      // time for srand
#include <bsd/stdlib.h>      // arc4random on Linux, link option: -lbsd
#endif
#include "startstoptimer.h"  // if used, compile with extra source file: startstoptimer.c

// Simulation parameters, can be changed via command line arguments
#define RNG          1  // use which RNG: 0=rand, 1=random, 2=arc4random, 3=arc4random_uniform
#define FACES        6  // each die has X faces
#define START1       1  // first start with X dice
#define START2      20  // last start with X dice
#define GAMES  1000000  // simulate X games per start
#define SHOWPROGRESS 1  // progress bar, of sorts (1=yes, 0=no)

// Dividing factors for unbiased dice
// Ref.: https://en.cppreference.com/w/c/numeric/random/rand
#define RDIVU   ((RAND_MAX + 1U ) / FACES)
#define RDIVUL  ((RAND_MAX + 1UL) / FACES)
#define RDIVULL ((1ULL << 32) / FACES)

// Part of the problem posed by James Grime
static const uint64_t facevalue[FACES + 1] = {0, 0, 0, 0, 2, 3, 0};

// Parse command line arguments
static bool str2ui64(const char * str, uint64_t * val)
{
    long long a = atoll(str);
    if (a >= 0 && a <= UINT32_MAX) {
        *val = (uint64_t)a;
        return true;
    }
    return false;
}

// Unbiased die roll with rand(), supposedly bad
static uint64_t roll_bad(void)
{
    unsigned int x = FACES;
    while (x == FACES)
        x = (unsigned int)rand() / RDIVU;
    return x;
}

// Unbiased die roll with random(), supposedly better
static uint64_t roll_ok(void)
{
    unsigned long x = FACES;
    while (x == FACES)
        x = (unsigned long)random() / RDIVUL;
    return x;
}

// Unbiased die roll with arc4random(), supposedly best
// (this version with own uniform distribution)
static uint64_t roll_good(void)
{
    uint64_t x = FACES;
    while (x == FACES)
        x = (uint64_t)arc4random() / RDIVULL;
    return x;
}

// Unbiased die roll with arc4random(), supposedly best
// (this version with provided uniform distribution)
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

    // Parse command line arguments
    // arg1: RNG [0..3], arg1/2: start1 [1..], arg1/2/3: start2 [start1..], arg if 100+: number of games
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

    // Show game parameters
    printf("rng engine      : ");
    switch (rng) {
        case 0:
            printf("rand");
            roll = roll_bad;
#ifdef __APPLE__
            sranddev();
#else
            srand((unsigned int)time(NULL));
#endif
            break;
        case 1:
            printf("random");
            roll = roll_ok;
#ifdef __APPLE__
            srandomdev();
#else
            srandom((unsigned int)time(NULL));
#endif
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
        printf("start with dice : %"PRIu64"\n", start1);
    } else {
        printf("start dice from : %"PRIu64"\n", start1);
        printf("start dice to   : %"PRIu64"\n", start2);
    }
    printf("games per start : %"PRIu64"\n\n", games);

    // Simulation
    printf("dice,maxdice,maxrolls,exprolls\n");
    for (uint64_t start = start1; start <= start2; ++start) {  // start with X dice
        printf("%"PRIu64",", start);
        memset(hist, 0, histsize * sizeof *hist);

        uint64_t maxdice = 0, maxrolls = 0, progress = games / 50;
        for (uint64_t i = 0; i < games; ++i) {  // play X games until completion
#if defined(SHOWPROGRESS) && SHOWPROGRESS
            if (!(i % progress))
                fprintf(stderr, ".");
#endif
            uint64_t dice = start, rolls = 0;
            while (dice) {  // stop when no more dice
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
            hist[rolls]++;  // histogram of number of rolls per game
        }
        printf("%"PRIu64",%"PRIu64",", maxdice, maxrolls);

        // Expectation value of the number of rolls for a game with X dice to start
        double expectation = 0;
        for (uint64_t i = 1; i <= maxrolls; ++i)
            expectation += (double)(hist[i] * i) / games;
        printf("%.3f\n", expectation);
    }

    free(hist);
    printf("\nTime: %.1f s\n", stoptimer_s());
    return 0;
}
