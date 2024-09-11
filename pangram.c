/**
 * Determines if input is a 'pangram'
 */

#include <stdio.h>
#include <unistd.h>   // isatty, fileno

int main(int argc, char *argv[])
{
    int c, found = 0;                    // ints must be at least 32-bit for this to work
    while ((c = fgetc(stdin)) != EOF) {  // get char from standard input
        c |= 1 << 5;                     // set bit 5 = tolower for A-Z (assumes input is ASCII)
        if (c >= 'a' && c <= 'z') {      // is the current char a letter?
            found |= 1 << (c - 'a');     // set bit corresponding to this letter
            if (found == (1 << 26) - 1)  // are all 26 bits set?
                return 0;                // 0 = success
        }
    }
    for (c = 'a'; c <= 'z'; ++c, found >>= 1)
        if (!(found & 1))
            fputc(c, stderr);
    fputc('\n', stderr);
    return 1;                            // 1 = error
}
