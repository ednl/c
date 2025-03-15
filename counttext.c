#include <stdio.h>   // stdin, stderr, fgetc, fputc, EOF
#include <unistd.h>  // isatty, fileno
#include <string.h>  // strtok

static const char *space = " \t\f\v";  // not '\r' or '\n'

int main(int argc, char *argv[])
{
    char buf[BUFSIZ];
    FILE *f = NULL;
    int lines = 0, words = 0, sentences = 0;

    if (!isatty(fileno(stdin))) {
        // Input is pipe or redirect to stdin of this program.
        f = stdin;
    } else if (argc > 1) {
        // Command line arguments.
        f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, "File not found: %s\n", argv[1]);
            return 1;
        }
    } else {
        // Manual input.
        printf("File name? ");
        if (fgets(buf, sizeof buf, stdin)) {
            f = fopen(buf, "r");
            if (!f) {
                fprintf(stderr, "File not found: %s\n", buf);
                return 2;
            }
        }
    }
    while (fgets(buf, sizeof buf, stdin)) {
        char *s = strtok(buf, space);
        while (s) {
            if (*s == '\n' || *s == '\r') {
                ++lines;
            } else {
                ++words;
                printf("%s\n", s);
            }
            s = strtok(NULL, space);
        }
    }
    fclose(f);
    return 0;
}
