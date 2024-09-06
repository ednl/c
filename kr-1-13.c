/**
 * K&R exercise 1-13, page 24
 * Make histogram of word length in input.
 */
#include <stdio.h>
#include <ctype.h>  // isspace

#define MAXLEN 12

// Histogram of word length up to & including MAXLEN
static int hist[MAXLEN + 1];

static void horz_bar(int width)
{
    while (width--)
        putchar('#');
    putchar('\n');
}

int main(void)
{
    // Process data
    int c, len = 0, maxcount = 0;
    while ((c = getchar()) != EOF) {
        if (isspace(c)) {
            if (len) {
                int i = len <= MAXLEN ? len : 0;
                if (++hist[i] > maxcount)
                    maxcount = hist[i];
                len = 0;
            }
        } else
            len++;
    }

    // Draw axis and labels
    printf("  0");
    for (int i = 5; i < maxcount + 5; i += 5)
        printf("%5d", i);
    printf("\n--+");
    for (int i = 0; i < maxcount; i += 5)
        printf("----+");
    putchar('\n');

    // Show histogram
    for (int i = 1; i <= MAXLEN; ++i) {
        printf("%2d ", i);
        horz_bar(hist[i]);
    }

    // Words longer than MAXLEN
    if (hist[0]) {
        printf(">> ");
        horz_bar(hist[0]);
    }

    return 0;
}
