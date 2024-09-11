/**
 * Determines if input is a 'pangram'
 */

#include <stdio.h>
#include <unistd.h>   // isatty, fileno

int main(int argc, char *argv[])
{
    int found = 0;
    if (isatty(fileno(stdin))) {
        // No pipe or redirect, try command line arguments
        for (int i = 1; i < argc; ++i) {
            const char *s = argv[i];
            while (*s) {
                const char c = *s++ | (1 << 5);
                if (c >= 'a' && c <= 'z' && (found |= 1 << (c - 'a')) == (1 << 26) - 1)
                    return 0;
            }
        }
    } else {
        // Input is pipe or redirect to stdin of this program
        int c;
        while ((c = fgetc(stdin)) != EOF) {  // get char from standard input
            c |= 1 << 5;                     // set bit 5 = tolower for A-Z (assumes input is ASCII)
            if (c >= 'a' && c <= 'z') {      // is the current char a letter?
                found |= 1 << (c - 'a');     // set bit corresponding to this letter
                if (found == (1 << 26) - 1)  // are all 26 bits set?
                    return 0;                // 0 = success
            }
        }
    }
    for (int c = 'a'; c <= 'z'; ++c, found >>= 1)
        if (!(found & 1))
            fputc(c, stderr);
    fputc('\n', stderr);
    return 1;                            // 1 = error
}
