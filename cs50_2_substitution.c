// https://cs50.harvard.edu/x/psets/2/substitution/
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>   // isupper, islower, toupper, tolower
#include <stddef.h>  // ptrdiff_t
#include <string.h>  // strchr

#define BUFLEN 256

static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static bool isvalidkey(const char *key)
{
    bool seen[sizeof alphabet - 1] = {0};
    int len = 0;
    for (char *letter; *key; ++key, ++len) {
        if (!(letter = strchr(alphabet, toupper(*key))))
            return false;  // every letter in key must be in alphabet
        const ptrdiff_t pos = letter - alphabet;
        if (seen[pos])
            return false;
        seen[pos] = true;
    }
    return len == sizeof alphabet - 1;  // don't count string terminator
}

static char *encrypt(char *txt, const char *key)
{
    for (char *c = txt; *c; ++c) {
        const char *letter = strchr(alphabet, toupper(*c));
        if (!letter)
            continue;
        const char subst = key[letter - alphabet];
        if (isupper(*c))
            *c = toupper(subst);
        else if (islower(*c))
            *c = tolower(subst);
    }
    return txt;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {  // must provide exactly 1 command line argument
        fprintf(stderr, "Usage: %s <key>\n", argv[0]);
        return 1;
    }
    const char *key = argv[1];
    if (!isvalidkey(key)) {  // must contain all letters once
        fprintf(stderr, "Invalid key.\n");
        return 2;
    }
    char buf[BUFLEN] = {0};  // init to zero for completeness check
    printf("plaintext:  ");
    if (!fgets(buf, sizeof buf, stdin)) {
        fprintf(stderr, "Input error.\n");
        return 3;
    }
    if (buf[BUFLEN - 2] && buf[BUFLEN - 2] != '\n') {  // must fit in buf
        fprintf(stderr, "Incomplete input.\n");
        return 4;
    }
    printf("ciphertext: %s", encrypt(buf, key));  // result still has '\n'
    return 0;
}
