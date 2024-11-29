// Homework assignment posted by /u/ig_grr in /r/cprogramming/
// https://www.reddit.com/r/cprogramming/comments/1h1xljb/files_in_c/
// Paraphrased: make a `wc' (the unix tool) clone but also count sentences.

#include <stdio.h>   // printf, fopen, fclose, FILE, EOF, NULL, size_t
#include <ctype.h>   // isspace
#include <unistd.h>  // isatty, fileno

static void wordflush(char *const buf, size_t *const len)
{
    buf[*len] = '\0';     // end of word.
    // printf("%s\n", buf);  // print finished word.
    *len = 0;             // no longer inside a word.
}

int main(int argc, char *argv[])
{
    char buf[BUFSIZ];
    FILE *f = NULL;
    if (!isatty(fileno(stdin))) {
        // Input is pipe or redirect to stdin of this program.
        f = stdin;
    } else if (argc > 1) {
        // Command line arguments.
        f = fopen(argv[1], "r");
    } else {
        // Manual input.
        printf("File name? ");
        fgets(buf, sizeof buf, stdin);
        for (char *c = buf; *c; ++c)
            if (*c == '\n') {
                *c = '\0';
                break;
            }
        f = fopen(buf, "r");
    }
    if (!f)
        return 1;

    size_t wordlen = 0;
    int wordcount = 0, linecount = 0, sentcount = 0;

    int c, prev = EOF;
    while ((c = fgetc(f)) != EOF) {
        if (isspace(c)) {
            if (wordlen)                   // previously inside a word?
                wordflush(buf, &wordlen);
            if (c == '\n')
                ++linecount;               // count new line at separator.
            if (prev == '.' || prev == '!' || prev == '?')
                ++sentcount;               // count new sentence at end.
        } else {
            if (!wordlen)                  // not already inside a word?
                ++wordcount;               // count new word at begin.
            if (wordlen < sizeof buf - 1)  // leave room for null terminator.
                buf[wordlen++] = c;        // remember ongoing word.
        }
        prev = c;
    }
    if (f != stdin)
        fclose(f);

    // Non-empty file without closing newline?
    if (prev != EOF && prev != '\n')
        ++linecount;

    // Unfinished word/sentence?
    if (wordlen) {
        wordflush(buf, &wordlen);
        ++sentcount;
    }

    printf("%d %d %d\n", wordcount, linecount, sentcount);
    return 0;
}
