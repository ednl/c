#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define N 100
uint64_t p[N] = {2, 3};

int main(void)
{
    int k = 0;
    for (int i = 2; i < N; ++i) {
        bool notfound = true;
        for (uint64_t q = p[i - 1] + 2; notfound; q += 2) {
            while (k < i && p[k] * p[k] <= q)
                ++k;
            bool isprime = true;
            for (int j = 1; isprime && j < k; ++j)
                isprime = isprime && q % p[j];
            p[i] = q;
        }
    }
    return 0;
}
