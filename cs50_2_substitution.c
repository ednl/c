// https://cs50.harvard.edu/x/psets/2/substitution/
#include <stdio.h>
#include <stdbool.h>

#define KEYLEN 26
#define BUFLEN 256

static bool isupper_ascii(const char c)
{
    return c >= 'A' && c <= 'Z';
}

static bool islower_ascii(const char c)
{
    return c >= 'a' && c <= 'z';
}

static char toupper_ascii(const char c)
{
    return islower_ascii(c) ? c - 'a' + 'A' : c;
}

static char tolower_ascii(const char c)
{
    return isupper_ascii(c) ? c - 'A' + 'a' : c;
}

static bool isvalidkey(const char *key)
{
    bool inkey[KEYLEN] = {0};
    int len = 0;
    for (const char *c = key; *c && len <= KEYLEN; ++c, ++len) {
        int i = -1;  // invalid
        if (isupper_ascii(*c))
            i = *c - 'A';
        else if (islower_ascii(*c))
            i = *c - 'a';
        if (i < 0 || i >= KEYLEN || inkey[i])
            return false;
        inkey[i] = true;
    }
    return len == KEYLEN;
}

static char encrypt(const char *key, const char c)
{
    if (isupper_ascii(c))
        return toupper_ascii(key[c - 'A']);
    if (islower_ascii(c))
        return tolower_ascii(key[c - 'a']);
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
    for (const char *c = buf; *c && *c != '\r' && *c != '\n'; ++c)
        fputc(encrypt(key, *c), stdout);
    fputc('\n', stdout);
    return 0;
}
