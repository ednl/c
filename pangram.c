/**
 * Determines if input contains all letters of the (ASCII) alphabet
 * as described by https://en.wikipedia.org/wiki/Pangram
 */

#include <stdio.h>
#include <unistd.h>   // isatty, fileno

#define PANGRAM ((1 << 26) - 1)
#define TOLOWER (1 << 5)

static int letters(const char *s)
{
    int found = 0;
    while (*s) {
        const char c = *s++ | TOLOWER;
        if (c >= 'a' && c <= 'z' && (found |= 1 << (c - 'a')) == PANGRAM)
            return PANGRAM;
    }
    return found;
}

int main(int argc, char *argv[])
{
    char buf[BUFSIZ];
    int found = 0;
    if (!isatty(fileno(stdin))) {
        // Input is pipe or redirect to stdin of this program
        while (fgets(buf, sizeof buf, stdin))
            if ((found |= letters(buf)) == PANGRAM)
                return 0;
    } else if (argc > 1) {
        // Command line arguments
        for (int i = 1; i < argc; ++i)
            if ((found |= letters(argv[i])) == PANGRAM)
                return 0;
    } else {
        // Manual input
        printf("? ");
        if (fgets(buf, sizeof buf, stdin) && (found = letters(buf)) == PANGRAM)
            return 0;
    }

    // Report missing letters
    for (int c = 'a'; c <= 'z'; ++c, found >>= 1)
        if (!(found & 1))
            fputc(c, stderr);
    fputc('\n', stderr);
    return 1;  // not a pangram
}
