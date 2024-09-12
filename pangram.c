/**
 * Determines if input contains all letters of the (7-bit ASCII)
 * alphabet, or: is this a https://en.wikipedia.org/wiki/Pangram
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno

#define PANGRAM ((1 << 26) - 1)  // complete checklist
#define TOLOWER (1 << 5)         // 'A' | 32 == 'a'

static int read_args(const int argc, char **argv)
{
    int map = 0;
    for (int i = 1; i < argc; ++i) {
        const char *s = argv[i];
        while (*s) {
            const char c = *s++ | TOLOWER;
            if (c >= 'a' && c <= 'z' && (map |= 1 << (c - 'a')) == PANGRAM)
                return PANGRAM;
        }
    }
    return map;
}

static int read_until(const int stop)
{
    int c, map = 0;
    while ((c = fgetc(stdin)) != stop) {
        c |= TOLOWER;
        if (c >= 'a' && c <= 'z' && (map |= 1 << (c - 'a')) == PANGRAM)
            return PANGRAM;
    }
    return map;
}

int main(int argc, char *argv[])
{
    int found = 0;

    if (!isatty(fileno(stdin))) {
        // Input is pipe or redirect to stdin of this program
        found = read_until(EOF);
    } else if (argc > 1) {
        // Command line arguments
        found = read_args(argc, argv);
    } else {
        // Manual input
        printf("? ");
        found = read_until('\n');
    }

    // Pangram found = success
    if (found == PANGRAM)
        return 0;

    // Report missing letters
    for (int c = 'a'; c <= 'z'; ++c, found >>= 1)
        if (!(found & 1))
            fputc(c, stderr);
    fputc('\n', stderr);

    // Not a pangram = error
    return 1;
}
