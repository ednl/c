/**
 * Compile:
 *     cc -std=c99 -Wall -Wextra -Wpedantic -O3 -march=native -mtune=native -o yahtc yaht.c
 */

#include <stdio.h>   // printf
#include <stdlib.h>  // srandom, random
#include <time.h>    // time
#include <string.h>  // memset
#include <stdint.h>  // uint64_t
#include <limits.h>  // INT_MAX
#include <math.h>    // sqrt

#define DICE  5
#define SIDES 6
#define BATCH 10000
#define ERROR 0.005

// Half or better: no more switch
#define MAJORITY ((DICE + 1) >> 1)

// Unbiased die-roll [0..N-1] using random()
// Ref.: https://en.cppreference.com/w/c/numeric/random/rand
static unsigned roll(void)
{
    unsigned long x;
    do {
        x = random() / ((RAND_MAX + 1ul) / SIDES);
    } while (x == SIDES);
    return x;
}

// Play one game until Yahtzee (=all dice have same value)
// Return number of throws needed
static int play(void)
{
    int bins[SIDES] = {0};  // histogram of dice values
    int throws = 0;  // number of throws of the dice
    int high = 0;  // highest count of one die value
    int pips = 0;  // which die value has the highest count
    while (high < DICE) {
        throws++;
        // Roll remaining dice
        for (int i = high; i < DICE; i++)
            bins[roll()]++;
        // Find or update highest count of any die value (pips)
        if (high < MAJORITY) {
            // Switch is still possible
            for (int i = 0; i < SIDES; ++i)
                if (bins[i] > high)
                    high = bins[(pips = i)];
            if (high < MAJORITY) {
                memset(bins, 0, sizeof bins);  // reset
                if (high > 1)
                    bins[pips] = high;  // restore highest count
                else           // all different? (high=1)
                    high = 0;  // then re-roll with all dice
            }
        } else
            // Majority reached so only look at this die value count
            high = bins[pips];
    }
    return throws;
}

int main(void)
{
    // Seed random number generator
    srandom(time(NULL));

    uint64_t games = 0;  // simulation counter
    uint64_t sum = 0;    // total number of throws in all games
    uint64_t in3 = 0;    // games that ended in at most 3 throws

    // Number of tries: running mean, unscaled variance, standard deviation
    double mean = 0, M2 = 0, stddev;

    // Run simulation until standard deviation is small enough
    do {
        // Single game wouldn't change stddev much
        for (int i = 0; i < BATCH; ++i) {
            games++;
            const int throws = play();
            sum += throws;
            in3 += throws < 4;
            const double estm = (double)sum / games;  // current estimate of the requested value
            // Update running mean and unscaled variance
            // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm
            const double delta = estm - mean;
            mean += delta / games;
            M2 += delta * (estm - mean);
        }
        // Scale as population variance (= exact variance of given data)
        // and take square root for new standard deviation
        stddev = sqrt(M2 / games);
    } while (stddev >= ERROR);

    printf("Yahtzee takes %.2f throws on average.\n", mean);
    printf("At most three in %.2f percent of games.\n", (double)in3 / games * 100);
    return 0;
}
