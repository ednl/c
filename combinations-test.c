#include <stdio.h>
#include "combinations.h"

int main(void)
{
    const int n = 6, k = 3;
    int count = 0, *a = NULL;
    while ((a = combinations(n, k))) {
        printf("%3d:", ++count);
        for (int i = k; i > 0;)
            printf(" %d", a[--i]);
        printf("\n");
    }
    return 0;
}
