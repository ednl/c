#include <stdio.h>   // printf
#include <stdlib.h>  // srand, rand
#include <time.h>    // time

#define N 10  // list length
#define M 10  // element range

static int a[N];

int main(void)
{
    srand(time(0));
    printf("Input:");
    for (int i = 0; i < N; ++i) {
        a[i] = rand() % M;  // biased, but fine for this
        printf(" %d", a[i]);
    }
    printf("\n");

    int dup = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < i; ++j)  // look at previous numbers
            if (a[i] == a[j])
                goto next_i;  // already processed
        for (int j = i + 1; j < N; ++j)  // look at next numbers
            dup += a[i] == a[j];
    next_i:;
    }
    printf("Sum of duplicate counts: %d\n", dup);
    return 0;
}
