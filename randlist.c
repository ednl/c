#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // Must supply 1 command line argument
    if (argc != 2) {
        fprintf(stderr, "Error: provide one argument as list length.\n");
        exit(1);
    }

    // Convert to int
    int n = atoi(argv[1]);
    if (n < 2) {
        fprintf(stderr, "Error: that's not a list length.\n");
        exit(2);
    }

    // Allocate memory
    int *list = NULL;
    if ((list = (int*)malloc((size_t)n * sizeof *list)) == NULL) {
        fprintf(stderr, "Error: out of memory.\n");
        exit(3);
    }

    // Seed RNG
    srand((unsigned int)time(NULL));

    // Init
    for (int i = 0; i < n; ++i) {
        list[i] = i + 1;
    }

    // Shuffle
    for (int i = 0; i < n; ++i) {
        int j = (int)((unsigned int)rand() / ((RAND_MAX + 1U) / (unsigned int)n));
        int t = list[i];
        list[i] = list[j];
        list[j] = t;
    }

    // Print
    for (int i = 0; i < n - 1; ++i) {
        printf("%i,", list[i]);
    }
    printf("%i\n", list[n - 1]);

    // Done
    free(list);
    return 0;
}
