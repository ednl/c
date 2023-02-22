// Simulate throwing with dice repeatedly to determine
// chance of 'Yahtzee' (5 the same) after 3 (or N) tries.
//
// By E. Dronkert https://github.com/ednl

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define DICE  5
#define MAJORITY ((DICE + 1) >> 1)
#define SIDES 6
#define TRIES 200
#define GAMES (100 * 1000 * 1000)
#define PERCT (GAMES / 100)

// Histograms
// use index [1..max] inclusive for both
static int cast[SIDES] = {0};
static int hist[TRIES] = {0};

// One unbiased die roll [0..SIDES - 1]
// Ref. https://en.cppreference.com/w/c/numeric/random/rand
static int roll(void)
{
    return random() / ((RAND_MAX + 1UL) / SIDES);
}

int main(void)
{
    srandom(time(NULL));
    int games = GAMES;
    while (games--) {
        // What value occurs most, how many times
        int try = 0, best = 0, count = 0;

        // Get 'yahtzee' in any number of tries
        while (count < DICE) {
            ++try;

            // Reset die count
            if (count < MAJORITY) {
                for (int i = 0; i < SIDES; ++i)
                    cast[i] = 0;
                if (count)
                    cast[best] = count;
            }

            // Roll remaining dice
            for (int i = count; i < DICE; ++i)
                ++cast[roll()];

            // Find most common value
            if (count < MAJORITY) {
                for (int i = 0; i < SIDES; ++i)
                    if (cast[i] > count)
                        count = cast[(best = i)];
            } else
                count = cast[best];
        }
        if (try < TRIES)
            ++hist[try];
    }

    int mintry = 0, maxtry = TRIES - 1;
    while (!hist[mintry])
        ++mintry;
    while (!hist[maxtry])
        --maxtry;

    printf("Tries,Count,Partial,Cumulative\n");
    int maxval = 0, mode = 0;
    double cumul = 0, median = 0, mean = 0;
    for (int i = mintry; i <= maxtry; ++i) {
        if (hist[i] > maxval)
            maxval = hist[(mode = i)];
        double prev = cumul, part = (double)hist[i] / GAMES;
        cumul += part;
        printf("%d,%d,%.8f,%.8f\n", i, hist[i], part, cumul);
        if (prev < 0.5 && cumul >= 0.5)
            median = i - 1 + (0.5 - prev) / (cumul - prev);
        mean += part * i;
    }
    printf("\n");

    printf("N_min    = %3d\n", mintry);
    printf("N_max    = %3d\n", maxtry);
    printf("N_mode   = %3d\n", mode);
    printf("N_median = %6.2f (1st ord appr)\n", median);
    printf("N_mean   = %6.2f\n\n", mean);
    printf("Chance of Yahtzee in 1: %.4f%%\n", (double)hist[1] / PERCT);
    printf("Chance of Yahtzee in 3: %.3f%%\n", (double)(hist[1] + hist[2] + hist[3]) / PERCT);

    return 0;
}
