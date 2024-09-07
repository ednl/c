/**
 * K&R exercise 1-13, page 24
 * Make histogram of word length in input.
 */
#include <stdio.h>

#define MAXWORDLEN 15
#define BINS (MAXWORDLEN + 1)  // one extra for overflow

static int histogram[BINS];

int main(void)
{
    int c, wordlen = 0, minbin = BINS, maxbin = -1;
    while ((c = getchar()) != EOF) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
            ++wordlen;
        else if (wordlen) {
            const int bin = wordlen <= MAXWORDLEN ? wordlen - 1 : BINS - 1;
            if (bin < minbin) minbin = bin;
            if (bin > maxbin) maxbin = bin;
            ++histogram[bin];
            wordlen = 0;
        }
    }

    for (int bin = minbin; bin <= maxbin; ++bin) {
        const int label = bin + 1;
        if (label <= MAXWORDLEN)
            printf("%3d ", label);
        else
            printf(">%2d ", MAXWORDLEN);
        while (histogram[bin]--)
            putchar('#');
        putchar('\n');
    }

    return 0;
}
