/**
 * K&R exercise 1-12, page 21
 * Write a program that prints its input one word per line.
 */
#include <stdio.h>

#define INSIDE  1
#define OUTSIDE 0

int main(void)
{
    int c, state = OUTSIDE;
    while ((c = getchar()) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (state == INSIDE)
                putc('\n', stdout);
            state = OUTSIDE;
        } else {
            putc(c, stdout);
            state = INSIDE;
        }
    }
    return 0;
}
