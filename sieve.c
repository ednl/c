#include <stdio.h>    // printf
#include <stdlib.h>   // malloc
#include <stdbool.h>  // bool, true

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;
    int n = atoi(argv[1]);
    if (n < 1)
        return 2;
    unsigned *p = malloc(n * sizeof *p);
    if (!p)
        return 3;

    p[0] = 2;
    p[1] = 3;
    if (n < 3)
        goto done;

    int i = 2, k = 0;
    for (unsigned q = 5, r = 4;; q += 2) {
        for (; r <= q; ++k, r = p[k] * p[k]);
        bool isprime = true;
        for (int j = 1; isprime && j < k; ++j)
            isprime = isprime && q % p[j];
        if (isprime) {
            p[i++] = q;
            if (i == n)
                break;
        }
    }

done:;
    printf("P%d = %u\n", n, p[n - 1]);
    free(p);
    return 0;
}
