#include <stdio.h>
#include <string.h>  // strlen
#include <ctype.h>   // isspace
#include <stdbool.h>

// Reverse string from s to end (exclusive)
static void revstr(char *s, char *end)
{
    while (s < --end) {
        const char c = *s;
        *s++ = *end;
        *end = c;
    }
}

// Reverse words in string, keep char order in words
static void reverse_words(char *s)
{
    // NULL or empty?
    if (!s || !*s)
        return;
    char *end = s + strlen(s);  // *end == '\0'
    // Skip spaces at begin ("ltrim")
    while (s < end && isspace(*s))
        ++s;
    // Skip spaces at end ("rtrim") incl. newline
    while (s < end && isspace(*(end - 1)))
        --end;
    // Reverse trimmed part of line char by char
    revstr(s, end);
    // Unreverse words
    while (s < end) {
        char *sep = s + 1;
        while (sep < end && !isspace(*sep))
            ++sep;
        revstr(s, sep);
        s = sep + 1;
        while (s < end && isspace(*s))
            ++s;
    }
}

int main(void)
{
    char buf[BUFSIZ];
    while (fgets(buf, sizeof buf, stdin) && *buf && *buf != '\n') {
        reverse_words(buf);
        printf("%s\n", buf);
    }
    return 0;
}
