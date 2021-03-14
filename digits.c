#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DIGITS (1000000)
#define NUMLEN (5)
#define N      (DIGITS / NUMLEN)

static const char *digits = "digits.txt";
static int num[N] = {0};

static int gcd(int a, int b)
{
    int r;
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
    char *s = NULL, *c;
    size_t t = 0;
    int i = 0, n = 0, coprime = 0;
    double pi;

    if ((fp = fopen(digits, "r")) == NULL) {
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
