#include <stdio.h>   // printf
#include <stdlib.h>  // rand

#define N 10  // list length
#define M  5  // element range

static int a[N];

int main(void)
{
    printf("Input:");
    for (int i = 0; i < N; ++i)
        printf(" %d", a[i] = 1 + rand() % M);
    printf("\n");

    int dup = 0;
    for (int i = 0; i < N - 1; ++i)
        for (int j = 0, seen = 0; j < N; ++j) {
            if (i > j && a[i] == a[j])
                seen = 1;
            dup += !seen && i < j && a[i] == a[j];
        }
    printf("%d\n", dup);
    return 0;
}
