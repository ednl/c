/******************************************************************
 * Roll set of special dice repeatedly, add up face values,
 *   next roll is with 'sum' of the same dice, stop if sum is zero.
 * Every die has face values: 0, 0, 0, 0, 2, 3.
 *
 * Ref. https://twitter.com/jamesgrime/status/1651896236457246721
 * By E. Dronkert https://github.com/ednl
 ******************************************************************/

#include <stdio.h>   // printf
#include <stdlib.h>  // arc4random, malloc, free
#include <string.h>  // memset

#define FACES      6  // each die has X faces
#define START1     1  // first start with X dice
#define START2    20  // last start with X dice
#define GAMES 100000  // simulate X games per start

// One unbiased die roll [0..FACES-1], translate to face value
// Ref. man arc4random
static int roll(void)
{
    static const int face[FACES] = {0, 0, 0, 0, 2, 3};
    return face[arc4random_uniform(FACES)];
}

int main(void)
{
    size_t histsize = 100;
    size_t *hist = malloc(histsize * sizeof *hist);

    printf("dice,max,exp\n");
    for (int start = START1; start <= START2; ++start) {
        printf("%d,", start);
        memset(hist, 0, histsize * sizeof *hist);

        size_t maxcount = 0;
        for (int i = 0; i < GAMES; ++i) {
            int dice = start;
            size_t count = 0;
            while (dice) {
                int sum = 0;
                for (int j = 0; j < dice; ++j)
                    sum += roll();
                ++count;
                dice = sum;
            }
            if (count > maxcount)
                maxcount = count;
            if (count >= histsize) {
                size_t newsize = count + 100;
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
            hist[count]++;
        }
        printf("%zu,", maxcount);

        double expectation = 0;
        for (size_t i = 1; i <= maxcount; ++i)
            expectation += (double)(hist[i] * i) / GAMES;
        printf("%.3f\n", expectation);
    }

    free(hist);
    return 0;
}
