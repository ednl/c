/**
 * \cc -std=c17 -Wall -Wextra -pedantic -O1 -g -fsanitize=address,undefined signedintoverflow.c
 * signedintoverflow.c:7:11: runtime error: signed integer overflow: 1000000000 * 10 cannot be represented in type 'int'
 */

#include <stdio.h>
int main(void)
{
    int x = 1;
    for (int i = 1; i < 33; ++i) {
        x *= 10;
        printf("%2d: %11d 0x%08x\n", i, x, x);
    }
}
