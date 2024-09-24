// https://en.wikipedia.org/wiki/Narcissistic_number
// https://oeis.org/A005188
//   1: 1, 2, 3, 4, 5, 6, 7, 8, 9
//   2: 153, 370, 371, 407
//   3: 1634, 8208, 9474
//   4: 54748, 92727, 93084
//   5: 548834
//   6: 1741725, 4210818, 9800817, 9926315
//   7: 24678050, 24678051, 88593477
//   8: 146511208, 472335975, 534494836, 912985153
//   9: 4679307774
//  10: 32164049650, 32164049651
#include <stdio.h>
int main(void)
{
    const int max = 1000 * 1000 * 1000;  // greatest power of 10 representable as int
    int powers[10] = {1,1,1,1,1,1,1,1,1,1};
    for (int n = 1, magnitude = 10; n < max; magnitude *= 10) {
        for (int i = 0; i < 10; ++i)
            powers[i] *= i;
        for (; n < magnitude; ++n) {
            int sum = 0;
            for (int x = n; x; x /= 10)
                sum += powers[x % 10];
            if (sum == n)
                printf("%d\n", n);  // Narcissistic or Armstrong number
        }
    }
    return 0;
}
