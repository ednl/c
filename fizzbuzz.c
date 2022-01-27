#include <stdio.h>   // (f)printf
#include <stdlib.h>  // atoi

#define MINFIZZ  (1)
#define MAXFIZZ  (100)
#define MAXCOUNT (1000)

// Least Common Multiple
static int lcm(int a, int b)
{
    if (a == 0 || b == 0) {
        return 0;
    }
    int p = abs(a), q = abs(b), r;
    if (q > p) {
        r = p;
        p = q;
        q = r;
    }
    while (q) {
        r = p % q;
        p = q;
        q = r;
    }
    return a / p * b;
}

int main(int argc, char *argv[])
{
    if (!argc || !argv) {
        fprintf(stderr, "\nArgument list error (null pointer exception)\n\n");
        return 1;
    }

    int fizz = 3, buzz = 5, count = 15;

    if (argc > 4) {
        fprintf(stderr, "\nArguments: [fizz buzz] [count]\n\n");
        return 2;
    } else if (argc == 2) {
        // One argument = count
        count = atoi(argv[1]);
    } else if (argc == 3) {
        // Two arguments = fizz, buzz
        fizz  = atoi(argv[1]);
        buzz  = atoi(argv[2]);
        count = lcm(fizz, buzz);  // stop at the first FizzBuzz
        if (count < MINFIZZ) {
            count = MINFIZZ;
        } else if (count > MAXCOUNT) {
            count = MAXCOUNT;
        }
    } else if (argc == 4) {
        // Three arguments
        fizz  = atoi(argv[1]);
        buzz  = atoi(argv[2]);
        count = atoi(argv[3]);
    }
    // Sanity check
    if (fizz  < MINFIZZ || fizz  > MAXFIZZ ||
        buzz  < MINFIZZ || buzz  > MAXFIZZ ||
        count < MINFIZZ || count > MAXCOUNT) {
        fprintf(stderr,
            "\nArgument range: fizz %u..%u, buzz %u..%u, count %u..%u\n\n",
            MINFIZZ, MAXFIZZ, MINFIZZ, MAXFIZZ, MINFIZZ, MAXCOUNT);
        return 3;
    }

    // Summary
    printf("Fizz  : %d\nBuzz  : %d\nCount : %d\n", fizz, buzz, count);

    // Do The Thing
    int n, f = fizz, b = buzz;
    for (n = MINFIZZ; n <= count; ++n) {
        switch (((n == b) << 1) | (n == f)) {          // 2-bit condition
            case 0: printf("%u", n); break;            // no common factor
            case 1: printf("Fizz"); f += fizz; break;  // common factor fizz
            case 2: printf("Buzz"); b += buzz; break;  // common factor buzz
            case 3: printf("FizzBuzz"); f += fizz; b += buzz; break;
        }
        printf(n < count ? " " : "\n");
    }
    return 0;
}
