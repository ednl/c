#include <stdio.h>
#include "combinations.h"

int main(void)
{
    const int n = 3, k = 2;
    int *a = NULL;
    while ((a = combinations(n, k))) {
        for (int i = 0; i < k; ++i)
            printf(" %d", a[i]);
        printf("\n");
    }
    return 0;
}
