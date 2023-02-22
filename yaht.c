// Simulate throwing with dice repeatedly to determine
// chance of 'yahtzee' (5 the same) after 3 tries
//
// By E. Dronkert https://github.com/ednl

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DICE  5UL
#define MAJORITY ((DICE + 1UL) >> 1)
#define FACES 6UL
#define TRIES 200UL
#define GAMES 10000000UL
#define PERCT (GAMES / 100UL)

// Histograms
// use index [1..max] inclusive for both
static uint64_t vals[FACES + 1UL] = {0};
static uint64_t yaht[TRIES + 1UL] = {0};

// One unbiased die roll [1..FACES]
// Ref. https://en.cppreference.com/w/c/numeric/random/rand
static uint64_t roll(void)
{
    return 1UL + random() / ((RAND_MAX + 1UL) / FACES);
}

int main(void)
{
    srandom(time(NULL));
    uint64_t games = GAMES, mintry = TRIES, maxtry = 0;

    while (games--) {
        // What value occurs most, how many times
        uint64_t try = 0, best = 0, count = 0;

        // Get 'yahtzee' in any number of tries
        while (count < DICE) {
            // Reset values histogram
            for (uint64_t i = 1; i <= FACES; ++i)
                if (i != best)
                    vals[i] = 0;

            // Roll remaining dice
            for (uint64_t i = count; i < DICE; ++i)
                ++vals[roll()];

            // Find most common value
            if (count < MAJORITY) {
                for (uint64_t i = 1; i <= FACES; ++i)
                    if (vals[i] > count) {
                        count = vals[i];
                        best = i;
                    }
            } else
                count = vals[best];

            ++try;
        }

        if (try < mintry)
            mintry = try;
        if (try > maxtry)
            maxtry = try;
        if (try <= TRIES)
            ++yaht[try];
    }

    uint64_t maxval = 0, mode = 0;
    double cumul = 0, median = 0, mean = 0;
    for (uint64_t i = mintry; i <= maxtry && i <= TRIES; ++i) {
        if (yaht[i] > maxval) {
            maxval = yaht[i];
            mode = i;
        }
        if (cumul < 0.5) {
            double prev = cumul;
            cumul += (double)yaht[i] / GAMES;
            if (cumul >= 0.5)
                median = i - 1 + (0.5 - prev) / (cumul - prev);
        }
        mean += (double)(i * yaht[i]) / GAMES;
    }

    printf("N_min    = %3llu\n", mintry);
    printf("N_max    = %3llu\n", maxtry);
    printf("N_mode   = %3llu\n", mode);
    printf("N_median = %6.2f (1st ord appr)\n", median);
    printf("N_mean   = %6.2f\n\n", mean);
    printf("Chance of Yahtzee in 1: %.3f%%\n", (double)yaht[1] / PERCT);
    printf("Chance of Yahtzee in 3: %.2f%%\n", (double)(yaht[1] + yaht[2] + yaht[3]) / PERCT);

    return 0;
}
