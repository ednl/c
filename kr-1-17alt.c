/**
 * Kernighan & Ritchie - The C Programming Language, second edition
 * Exercise 1-17, page 31:
 *     Write a program to print all input lines that are longer
 *     than 80 characters.
 *
 * Example lines:
 * ..........................................................................79
 * ...........................................................................80
 * ............................................................................81
 * .............................................................................82
 * ............................................................................81
 * ...........................................................................80
 * ..........................................................................79
 */
#include <stdio.h>

#define LIMIT (80)
static char buf[LIMIT];

int main(void)
{
    int c, pos = 0;
    while ((c = fgetc(stdin)) != EOF)
        if (c == '\n') {
            if (pos > LIMIT)
                fputc(c, stdout);
            pos = 0;
        } else if (pos < LIMIT) {
            buf[pos++] = (char)c;
        } else {
            if (pos == LIMIT)
                for (pos = 0; pos < LIMIT; ++pos)
                    fputc(buf[pos], stdout);
            fputc(c, stdout);
            ++pos;
        }
    return 0;
}
