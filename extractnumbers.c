#include <stdio.h>
#include <unistd.h>    // isatty, fileno
#include <ctype.h>     // isdigit
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64

int main(int argc, char *argv[])
{
    char buf[BUFSIZ];
    FILE *f = NULL;

    if (!isatty(fileno(stdin))) {
        // Input is pipe or redirect to stdin of this program
        f = stdin;
    } else if (argc > 1) {
        // Command line argument(s)
        f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, "File not found: %s\n", argv[1]);
            return 1;
        }
    } else {
        // Manual input
        printf("File name? ");
        if (fgets(buf, sizeof buf, stdin)) {
            f = fopen(buf, "r");
            if (!f) {
                fprintf(stderr, "File not found: %s", buf);
                return 2;
            }
        }
    }
    while (fgets(buf, sizeof buf, f)) {
        for (const char *c = buf; *c; ) {
            while (*c && !isdigit(*c))
                ++c;
            if (*c) {
                uint64_t x = 0;
                while (*c && isdigit(*c))
                    x = x * 10 + (*c++ - '0');
                printf("%"PRIu64"\n", x);
            }
        }
    }
    fclose(f);
    return 0;
}
