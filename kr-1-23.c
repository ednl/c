/**
 * Kernighan & Ritchie - The C Programming Language, second edition
 * Exercise 1-23, page 34:
 *     Write a program to remove all comments from a C program. Don't forget
 *     to handle quoted strings and character constants properly. C comments
 *     do not nest.
 */

#include <stdio.h>   // fopen, fclose, fgets, fputc

typedef enum state {
    SOURCE, OLDCOM, NEWCOM, STRLIT, CHRLIT
} State;

int main(int argc, char *argv[])
{
    FILE *f;
    if (argc > 1) {  // command line argument(s)
        if (!(f = fopen(argv[1], "r"))) {
            fprintf(stderr, "File not found: \"%s\"\n", argv[1]);
            return 1;
        }
    } else  // input is pipe or redirect to stdin, or manual input
        f = stdin;

    State state = SOURCE;
    char line[BUFSIZ];
    while (fgets(line, sizeof line, f))
        for (int i = 0; line[i]; i++)
            switch (state) {
            case SOURCE:
                if (line[i] == '/' && line[i + 1] == '*') {
                    state = OLDCOM;
                    i++;
                } else if (line[i] == '/' && line[i + 1] == '/') {
                    state = NEWCOM;
                    i++;
                } else {
                    fputc(line[i], stdout);
                    if (line[i] == '"')
                        state = STRLIT;
                    else if (line[i] == '\'')
                        state = CHRLIT;
                }
                break;
            case OLDCOM:
                if (line[i] == '*' && line[i + 1] == '/')
                    state = SOURCE;
                    i++;
                break;
            case NEWCOM:
                if (line[i] == '\n') {
                    fputc(line[i], stdout);
                    state = SOURCE;
                }
                break;
            case STRLIT:
                fputc(line[i], stdout);
                if (line[i] == '"')
                    state = SOURCE;
                else if (line[i] == '\\')
                    fputc(line[++i], stdout);
                break;
            case CHRLIT:
                fputc(line[i], stdout);
                if (line[i] == '\'')
                    state = SOURCE;
                else if (line[i] == '\\')
                    fputc(line[++i], stdout);
                break;
            }

    if (f != stdin)
        fclose(f);
    return 0;
}
