#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DIGITS (1000000)
#define NUMLEN (5)
#define N      (DIGITS / NUMLEN)

static int num[N] = {0};

static int gcd(int a, int b)
{
    int r;
    if (b > a) {
        r = a;
        a = b;
        b = r;
    }
    while (b > 0) {
        r = a % b;
        a = b;
        b = r;
    }
    return a;
}

int main(void)
{
    FILE *fp;
    char *s = NULL, *ch;
    size_t t = 0;
    int i = 0, n = 0, coprime = 0;
    double pi;

    if ((fp = fopen("digits.txt", "r")) != NULL) {
        while (getline(&s, &t, fp) > 0) {
            ch = s;
            while (*ch != '\n' && *ch != '\r' && *ch != '\0' && i < N) {
                while (*ch >= '0' && *ch <= '9') {
                    ++ch;  // skip number
                }
                while (*ch == ' ') {
                    ++ch;  // skip spaces
                }
                if (*ch >= '0' && *ch <= '9') {
                    num[n++] = atoi(ch);
                }
            }
        }
        free(s);
        fclose(fp);
    }

    while (i < n) {
        if (gcd(num[i], num[i + 1]) == 1) {
            ++coprime;
        }
        i += 2;
    }

    if (coprime) {
        pi = sqrt((double)n / coprime * 3);
        printf("\npi ~= %f\n\n", pi);
    } else {
        printf("No coprimes found; estimation of pi not possible.\n");
    }
    return 0;
}
