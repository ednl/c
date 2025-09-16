/**
 * Kernighan & Ritchie - The C Programming Language, second edition
 * Exercise 1-23, page 34:
 *     Write a program to remove all comments from a C program. Don't forget
 *     to handle quoted strings and character constants properly. C comments
 *     do not nest.
 */

#include <stdio.h>

typedef enum state {
    CODE, STRCONST, CHARCONST, COMMENT, SKIP2CODE
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
    int cur, prev = EOF, next = fgetc(f);
    while ((cur = next) != EOF) {
        next = fgetc(f);
        switch (cur) {
            case '/':
                if (state == CODE && next == '*') {
                    state = COMMENT;
                    cur = fgetc(f);  // cur might be EOF now
                    next = cur == EOF ? EOF : fgetc(f);
                } else if (state == COMMENT && prev == '*')
                    state = SKIP2CODE;
                break;
            case '"':
                if (state == CODE)
                    state = STRCONST;
                else if (state == STRCONST && prev != '\\')
                    state = CODE;
                break;
            case '\'':
                if (state == CODE)
                    state = CHARCONST;
                else if (state == CHARCONST && prev != '\\')
                    state = CODE;
                break;
        }
        if (state == SKIP2CODE)
            state = CODE;
        else if (state != COMMENT && cur != EOF)
            fputc(cur, stdout);
        prev = cur;
    }
    fclose(f);
    return 0;
}
