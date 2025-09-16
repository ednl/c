/**
 * Kernighan & Ritchie - The C Programming Language, second edition
 * Exercise 1-23, page 34:
 *     Write a program to remove all comments from a C program. Don't forget
 *     to handle quoted strings and character constants properly. C comments
 *     do not nest.
 */

#include <stdio.h>

typedef enum state {
    COMMENT, CODE, STRCONST, CHARCONST
} State;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file with /* comments */>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "File not found: \"%s\"\n", argv[1]);
        return 2;
    }

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
                    state = STRCONST;
                else if (cur == '\'')
                    state = CHARCONST;
            }
            break;
        case STRCONST:
            fputc(cur, stdout);
            if (cur == '"')
                state = CODE;
            else if (cur == '\\' && next != EOF) {  // backslash-escape
                fputc(next, stdout);
                next = fgetc(f);
            }
            break;
        case CHARCONST:
            fputc(cur, stdout);
            if (cur == '\'')
                state = CODE;
            else if (cur == '\\' && next != EOF) {  // backslash-escape
                fputc(next, stdout);
                next = fgetc(f);
            }
            break;
        }
    }
    fclose(f);
    return 0;
}
