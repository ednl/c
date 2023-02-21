// Simulate games of Yahtzee to determine
// chance of 'yahtzee' (5 the same) after 3 tries
//
// By E. Dronkert https://github.com/ednl

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DICE  5
#define FACES 6
#define TRIES 3
#define GAMES 10000000

// Histogram
static int hist[FACES + 1];  // use index [1..FACES]

// One unbiased die roll [1..FACES]
// Ref. https://en.cppreference.com/w/c/numeric/random/rand
static int roll(void)
{
    return 1 + rand() / ((RAND_MAX + 1U) / FACES);
}

int main(void)
{
    srand(time(NULL));
    int yahtzee = 0, games = GAMES;

    while (games--) {
        // Reset histogram
        for (int i = 1; i <= FACES; ++i)
            hist[i] = 0;

        // What value occurs most, how many times
        int best = 0, count = 0;

        // Try to get 'yahtzee' in a number of tries
        for (int j = 0; j < TRIES && count < DICE; ++j) {
            // Roll remaining dice
            for (int i = count; i < DICE; ++i)
                hist[roll()]++;

            if (!best) {
                // Find most common value after first try
                for (int i = 1; i <= FACES; ++i)
                    if (hist[i] > count) {
                        count = hist[i];
                        best = i;
                    }
            } else
                count = hist[best];
        }

        // Test for yahtzee
        if (count == DICE)
            yahtzee++;
    }

    printf("Chance of yahtzee: %.2f%%\n", (double)yahtzee / GAMES * 100);
    return 0;
}
