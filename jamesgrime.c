/****************************************************************
 * Roll set of special dice repeatedly, add up face values,
 *   next roll is with 'sum' of the same dice,
 *   stop if sum is zero, start with five dice.
 * Every die has face values: 0, 0, 0, 0, 2, 3.
 *
 * Ref. https://twitter.com/jamesgrime/status/1651896236457246721
 * By E. Dronkert https://github.com/ednl
 ****************************************************************/

#include <stdio.h>   // printf
#include <stdlib.h>  // arc4random
// #include <stdint.h>  // uint64_t

#define FACES       6  // each die has X faces
#define START       5  // start with X dice
#define GAMES 1000000  // simulate X games
#define ROLLS     100  // keep histogram of games with up to & incl X rolls

// Face values
static const int face[FACES] = {0, 0, 0, 0, 2, 3};

// Histogram of number of rolls per game
static int hist[ROLLS + 1] = {0};

// One unbiased die roll [0..FACES-1], translate to face value
// Ref. man arc4random
static int roll(void)
{
    return face[arc4random_uniform(FACES)];
}

int main(void)
{
    int maxcount = 0;
    for (int i = 0; i < GAMES; ++i) {
        int dice = START, count = 0;
        while (dice) {
            int sum = 0;
            for (int j = 0; j < dice; ++j)
                sum += roll();
            ++count;
            dice = sum;
        }
        if (count > maxcount)
            maxcount = count;
        if (count > ROLLS)
            count = ROLLS;
        hist[count]++;
    }
    printf("max = %d\n", maxcount);

    double expectation = 0;
    for (int i = 1; i <= ROLLS; ++i) {
        const double r = (double)hist[i] / GAMES;
        expectation += r * i;
        // printf("%3d %.6f\n", i, r);
    }
    printf("exp = %.3f\n", expectation);

    return 0;
}
