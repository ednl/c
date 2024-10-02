/**
 * Print the longest input line.
 * Rewritten from an example in K&R 2nd ed., par. 1.9, page 29 or 30.
 */

#include <stdio.h>  // getchar, printf, EOF

// Maximum input line length.
#define MAXLINE 1000

// Read a line into buf, return length.
static int readline(char buf[static const 1], const int lim)
{
    int c, i;
    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        buf[i] = c;
    if (c == '\n')
        buf[i++] = c;
    buf[i] = '\0';
    return i;
}

// Copy 'from' into 'to'; assume 'to' is big enough.
static void copystring(char to[static restrict const 1], const char from[static restrict const 1])
{
    for (int i = 0; (to[i] = from[i]) != '\0'; ++i);
}

int main(void)
{
    char line[MAXLINE];     // current input line
    char longest[MAXLINE];  // longest line saved here
    int max = 0;            // maximum length seen so far
    int len;                // current line length
    while ((len = readline(line, MAXLINE)) > 0)
        if (len > max) {
            max = len;
            copystring(longest, line);
        }
    if (max > 0)  // there was a line
        printf("%s", longest);
    return 0;
}
