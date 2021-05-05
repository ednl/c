#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define LEN   (10)
#define BASE   (1)
#define REPEAT (1)

int main(int argc, char *argv[])
{
    // List length
    int len = LEN;
    if (argc > 1) {
        len = atoi(argv[1]);
        if (len < 1) {
            fprintf(stderr, "List length must be greater than zero.\n");
            exit(1);
        }
    }

    // Base value
    int base = BASE;
    if (argc > 2) {
        base = atoi(argv[2]);
        if (base > INT_MAX - len) {
            fprintf(stderr, "For length %i, maximum base value is %i.\n", len, INT_MAX - len);
            exit(2);
        }
    }

    // Repeat count
    int repeat = REPEAT;
    if (argc > 3) {
        repeat = atoi(argv[3]);
        if (repeat < 1) {
            fprintf(stderr, "Repeat count must be greater than zero.\n");
            exit(3);
        }
    }

    // No more than 3 command line arguments
    if (argc > 4) {
        fprintf(stderr, "Useage: %s [length=%i] [base=%i] [repeat=%i]\n", argv[0], LEN, BASE, REPEAT);
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
            int t = list[i];
            list[i] = list[j];
            list[j] = t;
        }

        // Print
        for (int i = 0; i < len - 1; ++i) {
            printf("%i,", list[i]);
        }
        printf("%i\n", list[len - 1]);

    }

    // Done
    free(list);
    return 0;
}
