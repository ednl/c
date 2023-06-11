#include <stdio.h>   // printf
#include <stdlib.h>  // random, srandom, RAND_MAX
#include <stdint.h>  // uint8_t
#include <time.h>    // time for srandom

typedef struct {
    uint8_t number;  // 0 = zero, [1..36]
    uint8_t parity;  // 0 = zero, 1 = odd, 2 = even
    uint8_t range;   // 0 = zero, 1 = low, 2 = high
    uint8_t color;   // 0 = zero, 1 = red, 2 = black
    uint8_t dozen;   // 0 = zero, [1..3]
    uint8_t column;  // 0 = zero, [1..3]
    uint8_t row;     // 0 = zero, [1..12]
    uint8_t snake;   // 0 = zero, 1 = snake, 2 = not snake
} result_t;

static const uint8_t pocket[] = {0,32,15,19,4,21,2,25,17,34,6,27,13,36,11,30,8,23,10,5,24,16,33,1,20,14,31,9,22,18,29,7,28,12,35,3,26};
static const size_t N = sizeof pocket / sizeof *pocket;

static const char * ord[] = {"", "first", "second", "third"};

// Unbiased draw [0..N-1]
// Ref.: https://en.cppreference.com/w/c/numeric/random/rand
static result_t draw(void)
{
    size_t i = N;
    // If RAND_MAX+1 is not divisible by N then certain i for some
    // high values of random() will be equal to N; discard those.
    // For N=37, values 2**32-22 through 2**31-1 are inadmissible.
    while (i == N)
        // random() has type long but largest value is still RAND_MAX=(2**31)−1
        // so casting to unsigned int does not lose precision
        i = (unsigned int)random() / ((RAND_MAX + 1U) / N);

    uint8_t n = pocket[i];
    if (n == 0)
        return (result_t){0};
    uint8_t p = n & 1 ? 1 : 2;
    uint8_t r = n <= 18 ? 1 : 2;
    uint8_t c = (n <= 10) || (n >= 19 && n <= 28) ? p : 3 - p;
    uint8_t d = 1 + (n - 1) / 12;
    uint8_t col = 1 + (n - 1) % 3;
    uint8_t row = 1 + (n - 1) / 3;
    uint8_t q = (n - 1) / 9;  // quartile 0..3
    uint8_t qm = (n - 1) % 9;  // intra quartile 0..8
    uint8_t s = q & 1 ? !(qm % 2) && (qm & 6) : !(qm % 4);
    return (result_t){n, p, r, c, d, col, row, 2 - s};
}

int main(void)
{
    int i = 0;
    result_t spin;

    // use random() and srandom because time doesn't work well
    // with srand(): very low variability from 8 or 16 LSBs
#ifdef __APPLE__
    srandomdev();
#else
    srandom((unsigned int)time(NULL));
#endif

    do {
        spin = draw();
        printf("%3d: %2d =", ++i, spin.number);
        if (spin.number) {
            printf(" %s,", spin.parity == 1 ? "odd" : "even");
            printf(" %s,", spin.range == 1 ? "low" : "high");
            printf(" %s,", spin.color == 1 ? "red" : "black");
            printf(" %s dozen,", ord[spin.dozen]);
            printf(" %s column,", ord[spin.column]);
            printf(" row %u,", spin.row);
            printf(" %ssnake\n", spin.snake == 1 ? "" : "no ");
        } else
            puts(" house wins");
    } while (spin.number);
    return 0;
}
