/**
 * Kernighan & Ritchie - The C Programming Language, second edition
 * Exercise 1-23, page 34:
 *     Write a program to remove all comments from a C program. Don't forget
 *     to handle quoted strings and character constants properly. C comments
 *     do not nest.
 */

#include <stdio.h>   // fopen, fclose, fgetc, fputc
#include <unistd.h>  // isatty, fileno

typedef enum state {
    COMMENT, CODE, STRLIT, CHARLIT
} State;

// Backslash-escape in string and character literals
static void escape(const int cur, int *const next, FILE *f)
{
    if (cur == '\\' && *next != EOF) {
        fputc(*next, stdout);
        *next = fgetc(f);
    }
}

int main(int argc, char *argv[])
{
    FILE *f = NULL;
    if (!isatty(fileno(stdin)))
        // Input is pipe or redirect to stdin of this program
        f = stdin;
    else if (argc == 2)
        // Command line argument
        f = fopen(argv[1], "r");
    if (!f)
        return 1;

    State state = CODE;
    int cur, next = fgetc(f);
    while ((cur = next) != EOF) {
        next = fgetc(f);
        switch (state) {
        case COMMENT:
            if (cur == '*' && next == '/') {
                state = CODE;
                next = fgetc(f);  // skip ahead or slash will be printed
            }
            break;
        case CODE:
            if (cur == '/' && next == '*') {
                state = COMMENT;
                next = fgetc(f);  // skip ahead or '/*/' will fail
            } else {
                fputc(cur, stdout);
                if (cur == '"')
                    state = STRLIT;
                else if (cur == '\'')
                    state = CHARLIT;
            }
            break;
        case STRLIT:
            fputc(cur, stdout);
            if (cur == '"')
                state = CODE;
            else
                escape(cur, &next, f);
            break;
        case CHARLIT:
            fputc(cur, stdout);
            if (cur == '\'')
                state = CODE;
            else
                escape(cur, &next, f);
            break;
        }
    }
    if (f != stdin)
        fclose(f);
    return 0;
}
