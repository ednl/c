// https://cs50.harvard.edu/x/psets/2/substitution/
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define KEYLEN ('Z' - 'A' + 1)
#define BUFLEN 256

static bool isvalidkey(const char *key)
{
    bool set[KEYLEN] = {0};
    int len = 0;
    for (const char *c = key; *c; ++c, ++len) {
        int i = -1;  // invalid value
        if (isupper(*c))
            i = *c - 'A';
        else if (islower(*c))
            i = *c - 'a';
        if (i < 0 || i >= KEYLEN || set[i])
            return false;
        set[i] = true;
    }
    return len == KEYLEN;
}

static char encrypt(const char *key, const char c)
{
    if (isupper(c))
        return toupper(key[c - 'A']);
    if (islower(c))
        return tolower(key[c - 'a']);
    return c;
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
    printf("ciphertext: ");
    for (const char *c = buf; isprint(*c); ++c)
        fputc(encrypt(key, *c), stdout);
    fputc('\n', stdout);
    return 0;
}
