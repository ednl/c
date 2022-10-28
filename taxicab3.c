#include <stdio.h>   // printf
#include <stdlib.h>  // qsort

#define N 100U
#define M ((N * (N + 1U)) >> 1U)

typedef struct {
    unsigned int x, a, b;
} Triple;

static Triple sum[M] = {0};

static int cmp(const void *a, const void *b)
{
    const unsigned int p = *(const unsigned int*)a;
    const unsigned int q = *(const unsigned int*)b;
    return (q < p) - (p < q);
}

int main(void)
{
    unsigned int i, j, k = 0, n = 0;

    for (j = 1; j < N; ++j) {
        sum[j] = (Triple){j * j * j, 0, j};
    }
    k = N;
    for (i = 1; i < N; ++i)
        for (j = i; j < N; ++j)
            sum[k++] = (Triple){sum[i].x + sum[j].x, i, j};

    qsort(sum, M, sizeof(Triple), cmp);

    for (i = 0; i < M - 1; ++i)
        if (sum[i].x == sum[i + 1].x)
            printf("%3u: %8u = %3u^3 + %3u^3 = %3u^3 + %3u^3\n", ++n, sum[i].x, sum[i].a, sum[i].b, sum[i + 1].a, sum[i + 1].b);

    return 0;
}
