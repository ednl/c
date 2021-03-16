#include <stdio.h>
#include <stdlib.h>    // free
#include <string.h>    // memcpy
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include <math.h>      // sqrtl, M_PI
#include <stdbool.h>   // bool, true, false
#include <errno.h>     // errno after strtoumax

#define NO_ERROR      (0)
#define DIGITSPERLINE (50)  // there are 10 groups of 5 digits on every line
#define MAXDIGITS     (19)  // floor(log10(2^64 - 1)) = max digits in base 10 fully representable in 64-bit unsigned
#define BUFSIZE       (DIGITSPERLINE * 12)  // one buffer contains 12 lines of digits (very divisible)

// Downloaded from https://www.rand.org/content/dam/rand/pubs/monograph_reports/MR1418/MR1418.digits.txt.zip
static const char *digitfile = "digits.txt";

static FILE *fp = NULL;
static char buf[BUFSIZE] = {0};
static size_t bufindex = 0, buflen = 0;

// Read chunks from file into buffer
static size_t fillbuf(void)
{
    char *s = NULL, *c;
    size_t t;

    // Clean up buffer
    if (bufindex > 0) {
        if (bufindex < buflen) {
            // Save remaining content
            t = buflen - bufindex;
            memmove(buf, &buf[bufindex], t);
            bufindex = 0;
            buflen = t;
        } else {
            // All processed, discard
            bufindex = buflen = 0;
        }
    }
    // Add to buffer
    t = 0;
    while (buflen <= BUFSIZE - DIGITSPERLINE && getline(&s, &t, fp) > 0) {
        c = s;
        while (*c >= '0' && *c <= '9') {
            ++c;  // skip line number
        }
        while (*c != '\n' && *c != '\0') {
            while (*c == ' ') {
                ++c;  // skip spaces
            }
            while (*c >= '0' && *c <= '9' && buflen < BUFSIZE) {
                buf[buflen++] = *c++;
            }
        }
    }
    free(s);
    return buflen - bufindex;
}

// Get number from buffer, return true on success
static bool getnum(uint64_t *num, unsigned char digits)
{
    if (buflen - bufindex < digits) {
        if (fillbuf() < digits) {
            return false;
        }
    }
    *num = 0;
    while (digits--) {
        *num = *num * 10 + (uint64_t)(buf[bufindex++] - '0');
    }
    return true;
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

    for (digits = 1; digits <= MAXDIGITS; ++digits) {

        rewind(fp);
        coprime = pairs = 0;
        bufindex = buflen = 0;

        while (getnum(&a, digits)) {
            if (getnum(&b, digits)) {
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
