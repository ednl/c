#include <stdio.h>

int main(void)
{
    unsigned long long i = 1;

    printf("1 2 fizz 4 buzz fizz 7 8 fizz buzz\n");
    while (1) {
        printf("%1$llu1 fizz %1$llu3 %1$llu4 fizzbuzz %1$llu6 %1$llu7 fizz %1$llu9 buzz fizz %2$llu2 %2$llu3 fizz buzz %2$llu6 fizz %2$llu8 %2$llu9 fizzbuzz %3$llu1 %3$llu2 fizz %3$llu4 buzz fizz %3$llu7 %3$llu8 fizz buzz\n", i, i + 1, i + 2);
        i += 3;
    }
    return 0;
}
