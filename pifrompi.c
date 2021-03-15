#include <stdio.h>     // printf, fopen, getline
#include <stdlib.h>    // atoll
#include <string.h>    // strlen_s
#include <stdint.h>    // uint64_t, uint_fast32_t, UINT32_C
#include <inttypes.h>  // PRIuFAST32
#include <math.h>      // sqrt

#define NUMLEN (16)
#define BUFLEN (NUMLEN + 1)

static const char *digits = "pi-10million.txt";
static char buf[BUFLEN];

static int64_t getnum(FILE *f)
{
    if (fgets(buf, sizeof buf, f) != NULL) {
        if (strnlen(buf, NUMLEN) == NUMLEN) {
            return atoll(buf);
        }
    }
    return -1;
}

static int64_t gcd(int64_t a, int64_t b)
{
    int64_t r;
    if (b > a) {
        r = a;
        a = b;
        b = r;
    }
    while (b) {
        r = a % b;
        a = b;
        b = r;
    }
    return a;
}

int main(void)
{
    FILE *fp;
    int64_t a, b, n = 0, coprime = 0;
    double p;

    if ((fp = fopen(digits, "r")) == NULL) {
        fprintf(stderr, "Text file with random digits not found.\n");
        return 1;
    }

    while ((a = getnum(fp)) >= 0) {
        if ((b = getnum(fp)) >= 0) {
            if (gcd(a, b) == 1) {
                ++coprime;
            }
            ++n;
        }
    }
    fclose(fp);

    printf("\nfile        : %s\n", digits);
    printf("digits      : %"PRId64"\n", n * 2 * NUMLEN);
    printf("per number  : %d\n", NUMLEN);
    printf("numbers     : %"PRId64"\n", n * 2);
    printf("pairs       : %"PRId64"\n", n);
    printf("coprimes    : %"PRId64"\n", coprime);

    if (n == 0) {
        fprintf(stderr, "\nNo numbers found; estimation of pi not possible.\n\n");
        return 2;
    }

    if (coprime == 0) {
        fprintf(stderr, "\nNo coprimes found; estimation of pi not possible.\n\n");
        return 3;
    }

    p = (double)coprime / n;
    printf("probability : %.2f%%\n", p * 100);
    printf("pi          : %.5f\n\n", sqrt(6 / p));
    return 0;
}
