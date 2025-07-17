// https://cs50.harvard.edu/x/psets/2/substitution/
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>   // isupper, islower, toupper, tolower
#include <stddef.h>  // ptrdiff_t
#include <string.h>  // strchr

#define BUFLEN 256

static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const unsigned keylen = sizeof alphabet - 1;

static bool isvalidkey(const char *key)
{
    bool seen[keylen] = {0};
    int len = 0;
    for (char *letter; *key; ++key, ++len) {
        if (!(letter = strchr(alphabet, toupper(*key))))
            return false;  // every letter in key must be in alphabet
        ptrdiff_t pos = letter - alphabet;
        if (seen[pos])
            return false;
        seen[pos] = true;
    }
    return len == keylen;
}

static void encrypt(char *txt, const char *key)
{
    for (; *txt; ++txt) {
        if (*txt == '\n') {
            *txt = '\0';
            return;
        }
        const char *letter = strchr(alphabet, toupper(*txt));
        if (!letter)
            continue;
        const char subst = key[letter - alphabet];
        if (isupper(*txt))
            *txt = toupper(subst);
        else if (islower(*txt))
            *txt = tolower(subst);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <key>\n", argv[0]);
        return 1;
    }
    const char *key = argv[1];
    if (!isvalidkey(key)) {
        fprintf(stderr, "Invalid key.\n");
        return 1;
    }
    char buf[BUFLEN] = {0};
    printf("plaintext:  ");
    fgets(buf, sizeof buf, stdin);
    encrypt(buf, key);
    printf("ciphertext: %s\n", buf);
    return 0;
}
