#include <stdio.h>
#include <stdint.h>    // uint64_t
#include <math.h>

static const char *digitfile = "digits.txt";

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

// Least common multiple
static uint64_t lcm(uint64_t a, uint64_t b)
{
    if (a == 0 || b == 0) {
        return 0;
    }
    if (a > b) {
        return (a / gcd(a, b)) * b;  // try & prevent overflow
    }
    return (b / gcd(b, a)) * a;
}

int main(void)
{
    FILE *fp;
    char *s = NULL, *c;
    size_t t = 0;
    int i = 0, n = 0, coprime = 0;
    double pi;

    if ((fp = fopen(digitfile, "r")) == NULL) {
        fprintf(stderr, "Text file with random digits not found.\n");
        return 1;
    }

    // Format of every line: "00000   10097 32533  76520 13586  34673 54876  80959 09117  39292 74945"
    while (getline(&s, &t, fp) > 0) {
        c = s;
        while (*c != '\n' && *c != '\r' && *c != '\0' && i < N) {
            while (*c >= '0' && *c <= '9') {
                ++c;  // skip line number and previously processed numbers
            }
            while (*c == ' ') {
                ++c;  // skip spaces
            }
            if (*c >= '0' && *c <= '9') {
                num[n++] = atoi(c);  // interpret number from c onwards
            }
        }
    }
    free(s);
    fclose(fp);

    if (n & 1) {
        --n;  // make n even
    }
    while (i < n) {
        if (gcd(num[i], num[i + 1]) == 1) {
            ++coprime;
        }
        i += 2;
    }

    if (coprime == 0) {
        fprintf(stderr, "No coprimes found; estimation of pi not possible.\n");
        return 2;
    }

    pi = sqrt((double)n / coprime * 3);
    printf("\npi ~= %.6f\n\n", pi);
    return 0;
}
