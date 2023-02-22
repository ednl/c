// Simulate games of Yahtzee to determine
// chance of 'yahtzee' (5 the same) after 3 tries
//
// By E. Dronkert https://github.com/ednl

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DICE  5UL
#define FACES 6UL
#define TRIES 3UL
#define GAMES 10000000UL

// Histogram
static uint64_t hist[FACES + 1UL];  // use index [1..FACES]

// One unbiased die roll [1..FACES]
// Ref. https://en.cppreference.com/w/c/numeric/random/rand
static uint64_t roll(void)
{
    return 1UL + random() / ((RAND_MAX + 1UL) / FACES);
}

int main(void)
{
    srandom(time(NULL));
    uint64_t yahtzee = 0, games = GAMES;

    while (games--) {
        // What value occurs most, how many times
        uint64_t best = 0, count = 0;

        // Try to get 'yahtzee' in a number of tries
        for (uint64_t j = 0; j < TRIES && count < DICE; ++j) {
            // Reset histogram
            for (uint64_t i = 1; i <= FACES; ++i)
                if (i != best)
                    hist[i] = 0;

            // Roll remaining dice
            for (uint64_t i = count; i < DICE; ++i)
                hist[roll()]++;

            // Find most common value
            for (uint64_t i = 1; i <= FACES; ++i)
                if (hist[i] > count) {
                    count = hist[i];
                    best = i;
                }
        }
        // Test for yahtzee
        if (count == DICE)
            yahtzee++;
    }
    printf("Chance of yahtzee: %.3f%%\n", (double)yahtzee / GAMES * 100UL);
    return 0;
}
