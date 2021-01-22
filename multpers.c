// Multiplicative Persistence
// https://en.wikipedia.org/wiki/Persistence_of_a_number
// https://oeis.org/A003001
// https://www.youtube.com/watch?v=E4mrC39sEOQ

#include <stdio.h>   // printf
#include <string.h>  // memcpy

#define MAXLEN (1000)
static unsigned char org[MAXLEN], n1[MAXLEN], n2[MAXLEN];

static void show(unsigned char *, int);
static int pers(unsigned char *, unsigned char *, int);
static int inc(unsigned char *, int);

static void show(unsigned char *a, int len)
{
    for (int i = len - 1; i >= 0; --i)
        printf("%u", a[i]);
    printf("\n");
}

static int pers(unsigned char *a, unsigned char *b, int alen)
{
    unsigned char *t;
    int i, j, blen, p = 0;

    while (alen > 1) {
        ++p;
        b[0] = 1;
        blen = 1;
        for (i = 0; i < alen; ++i) {
            if (a[i] == 0) {
                return p;
            } else {
                for (j = 0; j < blen; ++j)
                    b[j] *= a[i];
                for (j = 0; j < blen; ++j) {
                    if (b[j] >= 10) {
                        if (j == blen - 1) {
                            b[blen++] = b[j] / 10;
                        } else {
                            b[j + 1] += b[j] / 10;
                        }
                        b[j] %= 10;
                    }
                }
            }
        }
        t = a;
        a = b;
        b = t;
        alen = blen;
    }
    return p;
}

static int inc(unsigned char *a, int len)
{
    int i = 0;
    a[0]++;
    while (a[i] >= 10 && i < len) {
        if (i == len - 1) {
            a[len++] = a[i] / 10;
        } else {
            a[i + 1] += a[i] / 10;
        }
        a[i++] %= 10;
    }

    unsigned char prev = 0, has2 = 0, has5 = 0;
    for (i = len - 1; i >= 0; --i) {
        if (prev == 0 && a[i] < 2) {
            a[i] = 2;
            has2 = 1;
        } else if (prev == 0 && a[i] == 5) {
            a[i] = 6;
            has2 = 1;
        } else if (prev == 2 && a[i] < 6) {
            a[i] = 6;
            has2 = 1;
        } else if (prev == 3 && a[i] < 5) {
            a[i] = 5;
            has5 = 1;
        } else if (prev == 3 && a[i] == 6) {
            a[i] = 7;
        } else if (prev == 4 && a[i] < 7) {
            a[i] = 7;
        } else if (prev == 6 && a[i] < 7) {
            a[i] = 7;
        } else if (a[i] < prev) {
            a[i] = prev;
        }

        if (a[i] == 2 || a[i] == 4 || a[i] == 6 || a[i] == 8) {
            if (has5)
                a[i]++;
            else
                has2 = 1;
        } else if (a[i] == 5) {
            if (has2)
                a[i] = 6;
            else
                has5 = 1;
        }
        prev = a[i];
    }
    return len;
}

// int main(int argc, char *argv[])
int main(void)
{
    // int len = 3;
    // for (int i = 0; i < 100; ++i) {
    //     len = inc(org, len);
    //     show(org, len);
    // }
    int i, p, pmax = -1, len = 280;
    printf("len = %i\n", len);

    while (1) {
        memcpy(n1, org, len);
        p = pers(n1, n2, len);
        if (p > pmax) {
            printf("%2i ", (pmax = p));
            show(org, len);
            if (pmax == 12)
                return 0;
        }
        i = len;
        len = inc(org, len);
        if (len != i) {
            printf("len %i\n", len);
            pmax = -1;
        }
    }
}
