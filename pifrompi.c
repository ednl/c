#include <stdio.h>     // printf, fopen, getline
#include <stdlib.h>    // atoll
#include <string.h>    // strlen_s
#include <stdint.h>    // uint64_t, uint_fast32_t, UINT32_C
#include <inttypes.h>  // PRIuFAST32
#include <math.h>      // sqrt
#include <stdbool.h>   // bool, true, false
#include <errno.h>     // errno after strtoull

#define NO_ERROR (0)
#define MAXLEN (19)  // floor(log10(2^64 - 1)) = max digits base 10 fully representable in 64-bit unsigned
#define BUFLEN (MAXLEN + 1)  // room for closing '\0'

static const char *digitfile = "pi-10million.txt";
typedef unsigned long long u64;

// Get len-digit number in base 10 from file f, return true on success
static bool readnum(FILE *f, u64 *num, unsigned char digits)
{
    static char buf[BUFLEN];
    int len = (int)digits + 1;

    if (len > BUFLEN) {
        return false;
    }
    if (fgets(buf, len, f) != NULL) {
        if (strnlen(buf, (size_t)digits) == (size_t)digits) {
            *num = strtoull(buf, NULL, 10);
            return errno == NO_ERROR;
        }
    }
    return false;
}

// Greatest common divisor
static u64 gcd(u64 a, u64 b)
{
    u64 r;
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

// Least common multiple
// static u64 lcm(u64 a, u64 b)
// {
//     if (a == 0 || b == 0) {
//         return 0;
//     }
//     if (a > b) {
//         return (a / gcd(a, b)) * b;  // try & prevent overflow
//     }
//     return (b / gcd(b, a)) * a;
// }

int main(void)
{
    FILE *fp = NULL;
    u64 a, b, total, coprime;
    unsigned char digits;
    long double pi, err;
    int retval = NO_ERROR;

    if ((fp = fopen(digitfile, "r")) == NULL) {
        fprintf(stderr, "Text file with random digits not found.\n");
        return 1;
    }

    for (digits = 1; digits <= MAXLEN; ++digits) {

        coprime = total = 0;
        while (readnum(fp, &a, digits)) {
            if (readnum(fp, &b, digits)) {
                if (gcd(a, b) == 1) {
                    ++coprime;
                }
                ++total;
            }
        }

        if (total == 0) {
            fprintf(stderr, "\nNo numbers found; estimation of pi not possible.\n\n");
            retval = 2;
            break;
        }

        if (coprime == 0) {
            fprintf(stderr, "\nNo coprimes found; estimation of pi not possible.\n\n");
            retval = 3;
            break;
        }

        pi = sqrtl((long double)total / coprime * 6);
        err = pi - (long double)M_PI;
        printf("%2u %.5Lf %+.5Lf %7llu\n", digits, pi, err, total);
        rewind(fp);

    }
    fclose(fp);
    return retval;
}
