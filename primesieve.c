#include <stdio.h>   // printf
#include <time.h>    // clock, CLOCKS_PER_SEC
#include <math.h>    // sqrtf, floorf
#include <stdlib.h>  // calloc
#include <string.h>  // memcpy

#define ISPRIME  (0U)
#define NOTPRIME (1U)
#define LIMIT    (1000000U)
#define HALF     (LIMIT >> 1)
#define FIVESEC  (CLOCKS_PER_SEC * 5)

static unsigned int rooth;

typedef struct {
    unsigned int limit, count;
} VALID;
static const VALID valid[] = {
    {          10U, 4U         },
    {         100U, 25U        },
    {        1000U, 168U       },
    {       10000U, 1229U      },
    {      100000U, 9592U      },
    {     1000000U, 78498U     },
    {    10000000U, 664579U    },
    {   100000000U, 5761455U   },
    {  1000000000U, 50847534U  },
    // { 10000000000U, 455052511U },
};

static unsigned char isvalid(unsigned int limit, unsigned int count)
{
    for (size_t i = 0; i < sizeof valid / sizeof *valid; ++i) {
        if (valid[i].limit == limit) {
            return valid[i].count == count;
        }
    }
    return 0;
}

static void sieve(unsigned char *a)
{
    unsigned int i, j; //, rooth = ((unsigned int)sqrtf(LIMIT)) >> 1;

    memset(a, ISPRIME, HALF);
    for (i = 1; i <= rooth; ++i) {
        if (a[i] == ISPRIME) {
            for (j = (i * i + i) << 1; j < HALF; j += (i << 1) + 1) {
                a[j] = NOTPRIME;
            }
        }
    }
}

static unsigned int count(unsigned char *a)
{
    unsigned int i, c = LIMIT > 2;

    for (i = 1; i < HALF; ++i) {
        if (a[i] == ISPRIME) {
            ++c;
        }
    }
    return c;
}

int main(void)
{
    unsigned char *a = (unsigned char *)malloc(HALF);
    unsigned int passes = 0;
    rooth = ((unsigned int)sqrtf(LIMIT)) >> 1;

    clock_t stop = clock() + FIVESEC;
    do {
        sieve(a);
        ++passes;
    } while (clock() < stop);

    unsigned int c = count(a);
    printf("%u %u %u %s\n", passes, LIMIT, c, isvalid(LIMIT, c) ? "yes" : "no");
    free(a);
    return 0;
}
