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

// fgetc, fputc, stdin, stdout
#include <stdio.h>

// Print lines longer than 80 characters
#define LIMIT (80)

// Buffer at most 80 characters
static char buf[LIMIT];

int main(void)
{
    int c, pos = 0, lines = 0;

    // Read input character by character
    while ((c = fgetc(stdin)) != EOF) {

        // Absorb all end-of-line characters
        while (c == '\r' || c == '\n') {

            // Carriage Return?
            if (c == '\r') {
                if (pos > LIMIT)
                    fputc(c, stdout);
                if ((c = fgetc(stdin)) != '\n')
                    pos = 0;
            }

            // Line Feed?
            if (c == '\n') {
                if (pos > LIMIT)
                    fputc(c, stdout);
                c = fgetc(stdin);
                pos = 0;
            }
        }

        // Input may be exhausted after end-of-line
        if (c == EOF)
            break;

        // Regular character
        if (pos < LIMIT) {

            // Store in buffer
            buf[pos++] = (char)c;

        } else {

            // First character over the limit?
            if (pos == LIMIT) {
                for (pos = 0; pos < LIMIT; ++pos)
                    fputc(buf[pos], stdout);
                ++lines;
            }

            // Output character and keep count
            fputc(c, stdout);
            ++pos;

        }
    }

    printf("Lines longer than %d characters: %d\n", LIMIT, lines);
    return 0;
}
