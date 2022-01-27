#include <stdio.h>   // printf
#include <stdlib.h>  // atol, srand, rand, RAND_MAX
#include <limits.h>  // INT_MAX
#include <time.h>    // time (for srand)

#define LEN   (10)
#define BASE   (1)
#define REPEAT (1)

int main(int argc, char *argv[])
{
    int len    = LEN;
    int base   = BASE;
    int repeat = REPEAT;

    // List length
    if (argc > 1 && argv && argv[1]) {
        long try = atol(argv[1]);
        if (try < 1 || try > INT_MAX) {
            fprintf(stderr, "List length must be between 1 and %u.\n", INT_MAX);
            exit(1);
        }
        len = (int)try;
    }

    // Base value
    if (argc > 2 && argv && argv[2]) {
        long try = atol(argv[2]);
        long max = INT_MAX - (len - 1);
        if (try > max) {
            fprintf(stderr, "For length %i, maximum base value is %li.\n", len, max);
            exit(2);
        }
        base = (int)try;
    }

    // Repeat count
    if (argc > 3 && argv && argv[3]) {
        long try = atol(argv[3]);
        if (try < 1 || try > INT_MAX) {
            fprintf(stderr, "Repeat count must be between 1 and %u.\n", INT_MAX);
            exit(3);
        }
        repeat = (int)try;
    }

    // No more than 3 command line arguments
    if (argc > 4) {
        fprintf(stderr, "Usage: %s [length=%i] [base=%i] [repeat=%i]\n", argv && argv[0] ? argv[0] : "randlist", LEN, BASE, REPEAT);
        exit(4);
    }

    // Allocate memory
    int *list = NULL;
    if ((list = (int*)malloc((size_t)len * sizeof *list)) == NULL) {
        fprintf(stderr, "Error: out of memory.\n");
        exit(3);
    }

    // Seed RNG
    srand((unsigned int)time(NULL));

    // Init
    for (int i = 0; i < len; ++i) {
        list[i] = base + i;
    }

    while (repeat--) {

        // Shuffle
        for (int i = 0; i < len; ++i) {
            int j = (int)((unsigned int)rand() / ((RAND_MAX + 1U) / (unsigned int)len));
            int k = list[i];
            list[i] = list[j];
            list[j] = k;
        }

        // Print
        for (int i = 0; i < len - 1; ++i) {
            printf("%i,", list[i]);
        }
        printf("%i\n", list[len - 1]);

    }

    free(list);
    return 0;
}
