#include <stdio.h>     // printf, fopen, fgets, rewind
#include <string.h>    // strnlen
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // strtoumax
#include <math.h>      // sqrtl
#include <stdbool.h>   // bool, true, false
#include <errno.h>     // errno after strtoumax

#define NO_ERROR (0)
#define MAXLEN (19)  // floor(log10(2^64 - 1)) = max digits base 10 fully representable in 64-bit unsigned
#define BUFLEN (MAXLEN + 1)  // make room for closing '\0'

// Downloaded from http://introcs.cs.princeton.edu/data/pi-10million.txt
static const char *digitfile = "pi-10million.txt";

// Get number from file, return true on success
static bool readnum(FILE *f, uint64_t *num, unsigned char digits)
{
    static char buf[BUFLEN];
    int len = (int)digits + 1;

    if (len > BUFLEN) {
        return false;
    }
    if (fgets(buf, len, f) != NULL) {
        if (strnlen(buf, (size_t)digits) == (size_t)digits) {
            *num = strtoumax(buf, NULL, 10);
            return errno == NO_ERROR;
        }
    }
    return false;
}

// Greatest common divisor
static uint64_t gcd(uint64_t a, uint64_t b)
{
    uint64_t r;
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
    FILE *fp = NULL;
    uint64_t a, b, pairs, coprime;
    unsigned char digits;
    long double perc, pi, err;
    int retval = NO_ERROR;

    if ((fp = fopen(digitfile, "r")) == NULL) {
        fprintf(stderr, "Text file with random digits not found: %s\n", digitfile);
        return 1;
    }

    printf("\n");
    printf("dig  pairs  coprime percent   pi     error\n");
    printf("---+-------+-------+-------+-------+--------\n");

    for (digits = 1; digits <= MAXLEN; ++digits) {

        rewind(fp);
        coprime = pairs = 0;

        while (readnum(fp, &a, digits)) {
            if (readnum(fp, &b, digits)) {
                if (gcd(a, b) == 1) {
                    ++coprime;
                }
                ++pairs;
            }
        }

        if (pairs == 0) {
            fprintf(stderr, "\nNo numbers found in file: %s\n\n", digitfile);
            retval = 2;
            break;
        }

        if (coprime == 0) {
            fprintf(stderr, "\nNo coprimes found; estimation of pi not possible.\n\n");
            retval = 3;
            break;
        }

        perc = (long double)coprime / pairs;
        pi = sqrtl((long double)6 / perc);
        err = pi - (long double)M_PI;
        printf("%3u %7"PRIu64" %7"PRIu64" %.3Lf%% %.5Lf %+.5Lf\n", digits, pairs, coprime, perc * 100, pi, err);

    }
    fclose(fp);
    printf("\n");
    return retval;
}
