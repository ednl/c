#include <stdio.h>
#include <ctype.h>  // isspace

#define MAXLEN 80
static int hist[MAXLEN + 1];

int main(void)
{
    int c, len = 0, maxlen = 0, maxcount = 0;
    while ((c = getchar()) != EOF) {
        if (isspace(c)) {
            if (len > 0 && len <= MAXLEN) {
                hist[len]++;
                if (len > maxlen)
                    maxlen = len;
                if (hist[len] > maxcount)
                    maxcount = hist[len];
            } else if (len > MAXLEN)
                hist[0]++;
            len = 0;
        } else
            len++;
    }

    printf("   0");
    for (int i = 5; i < maxcount + 5; i += 5)
        printf("%5d", i);
    printf("\n---+");
    for (int i = 0; i < maxcount; i += 5)
        printf("----+");
    putchar('\n');

    for (int i = 0; i <= maxlen; ++i) {
        printf("%2d | ", i);
        for (int j = 0; j < hist[i]; ++j)
            putchar('#');
        putchar('\n');
    }

    printf("---+");
    for (int i = 0; i < maxcount; i += 5)
        printf("----+");
    printf("\n   0");
    for (int i = 5; i < maxcount + 5; i += 5)
        printf("%5d", i);
    putchar('\n');

    return 0;
}
