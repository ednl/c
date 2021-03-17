#include <stdio.h>

#define F (3U)      // Fizz factor
#define B (5U)      // Buzz factor
#define N (1000U)   // count from 1 to N
#define L (10000U)  // repeat L times

int main(void)
{
    unsigned int i, n, f, b;

    for (i = 0; i < L; ++i) {
        f = F; b = B;
        for (n = 1; n <= N; ++n) {
            switch (((n == b) << 1) | (n == f)) {  // 2-bit condition
                case 0: printf("%u ", n);                    break;
                case 1: printf("Fizz ");     f += F;         break;
                case 2: printf("Buzz ");     b += B;         break;
                case 3: printf("FizzBuzz "); f += F; b += B; break;
            }
        }
        printf("\n");
    }
    return 0;
}
