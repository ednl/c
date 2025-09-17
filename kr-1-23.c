/**
 * Kernighan & Ritchie - The C Programming Language, second edition
 * Exercise 1-23, page 34:
 *     Write a program to remove all comments from a C program. Don't forget
 *     to handle quoted strings and character constants properly. C comments
 *     do not nest.
 *
 * Compile:
 *   clang -std=c99 -Wall -Wextra -pedantic -O1 kr-1-23.c
 *   gcc   -std=c99 -Wall -Wextra -pedantic -O1 kr-1-23.c
 * Incompatible with C90 or earlier due to:
 *   // comments
 *   variable declaration in for loop
 *   declaration after statement
 * Run:
 *   file name argument: ./a.out comments.c
 *   input redirection : ./a.out < comments.c
 *   piped input       : cat comments.c | ./a.out
 *   manual input      : ./a.out (^D to end)
 * Debug compile:
 *   clang -DDEBUG kr-1-23.c
 * Debug run:
 *   ./a.out comments.c 1>/dev/null
 */

// fopen, fclose, fgets, fputc
#include <stdio.h>

// System state while reading a C source file
//   SOURCE: normal source code
//   OLDCOM: old style C comments /* */
//   NEWCOM: new style C comments //
//   STRLIT: string literal with escape sequences
//   CHRLIT: char literal with escape sequences
typedef enum state {
    SOURCE, OLDCOM, NEWCOM, STRLIT, CHRLIT
} State;

int main(int argc, char *argv[])
{
    FILE *f;
    if (argc > 1) {  // file name as command line argument
        if (!(f = fopen(argv[1], "r"))) {
            fprintf(stderr, "File not found: \"%s\"\n", argv[1]);
            return 1;
        }
    } else  // input is pipe or redirect to stdin, or manual input
        f = stdin;

    State state = SOURCE;
    char line[BUFSIZ];
#ifdef DEBUG
    State prev = state;
    int j = 1;  // line number
#endif
    while (fgets(line, sizeof line, f)) { // get one line of input at a time
    #ifdef DEBUG
        fprintf(stderr, "%d: %d", j++, state);  // line number and starting state
    #endif
        for (int i = 0; line[i]; i++) { // iterate over every character
            switch (state) {
            case SOURCE:
                if (line[i] == '/' && line[i + 1] == '*') {
                    // old comment start
                    state = OLDCOM;
                    i++;
                } else if (line[i] == '/' && line[i + 1] == '/') {
                    // new comment start
                    state = NEWCOM;
                    i++;
                } else {
                    fputc(line[i], stdout);
                    if (line[i] == '"')
                        // string literal start
                        state = STRLIT;
                    else if (line[i] == '\'')
                        // character literal start
                        state = CHRLIT;
                }
                break;
            case OLDCOM:
                if (line[i] == '*' && line[i + 1] == '/') {
                    // old comment end
                    state = SOURCE;
                    i++;  // skip the slash too
                }
                break;
            case NEWCOM:
                if (line[i] == '\r' || line[i] == '\n') {
                    // new comment end, line end on unix, win, mac
                    state = SOURCE;
                    fputc(line[i], stdout);  // include the line ending
                }
                break;
            case STRLIT:
                fputc(line[i], stdout);  // print every source code character as is
                if (line[i] == '"')
                    // string literal end
                    state = SOURCE;
                else if (line[i] == '\\')
                    // backslash escape sequence, next char must not be interpreted
                    fputc(line[++i], stdout);
                break;
            case CHRLIT:
                fputc(line[i], stdout);  // print every source code character as is
                if (line[i] == '\'')
                    // character literal end
                    state = SOURCE;
                else if (line[i] == '\\')
                    // backslash escape sequence, next char must not be interpreted
                    fputc(line[++i], stdout);
                break;
            }
        #ifdef DEBUG
            if (prev != state)
                fprintf(stderr, "%d", prev = state);  // changed state
        #endif
        }
    #ifdef DEBUG
        fprintf(stderr, "\n");
    #endif
    }

    if (f != stdin)
        fclose(f);
    return 0;
}
