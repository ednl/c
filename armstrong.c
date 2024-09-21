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
    unsigned power[10] = {1,1,1,1,1,1,1,1,1,1};
    for (unsigned digits = 1, beg = 1, end = 10;; ++digits) {
        for (unsigned i = 0; i < 10; ++i)
            power[i] *= i;  // should check for overflow
        printf("%u:", digits);
        for (unsigned n = beg; n < end; ++n) {
            unsigned sum = 0;
            for (unsigned a = n; a; a /= 10)
                sum += power[a % 10];
            if (sum == n)
                printf(" %u", n);
        }
        printf("\n");
        beg = end;
        unsigned shift = end << 3;  // times 8 is less than times 10
        end *= 10;
        if (shift <= beg || end <= shift)
            break;  // overflow
    }
    return 0;
}
