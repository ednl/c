#include <stdio.h>
#include <stdlib.h>  // malloc, free
#include <unistd.h>  // isatty, fileno

#define DATASIZE 32768  // reserved data size
#define CODESIZE  1024  // initial code size

static char data[DATASIZE];
static char *code;
static size_t codesize, codelen;
static size_t ip, dp;  // instruction pointer, data pointer

// Read from stdin until stop character and check for pangram.
static size_t read_until(const int stop)
{
    int c;
    size_t i = 0;
    while ((c = fgetc(stdin)) != stop) {
        //
    }
    return i;
}

int main(int argc, char *argv[])
{
    code = malloc(CODESIZE * sizeof *code);
    if (!code)
        return 1;
    if (!isatty(fileno(stdin))) {
        // Input is pipe or redirect to stdin of this program.
        codelen = read_until(EOF);
    } else if (argc > 1) {
        // Command line arguments.
    } else {
        // Manual input.
        printf("? ");
        codelen = read_until('\n');
    }
    while (ip < codelen) {
        int c;
        switch (code[ip]) {
            case '+': data[dp]++;                   break;
            case ',':
                if ((c = fgetc(stdin)) != EOF && c != '\n')
                    data[dp] = c & 0xff;
                break;
            case '-': data[dp]--;                   break;
            case '.': fputc(data[dp], stdout);      break;
            case '<': if (dp) dp--;                 break;
            case '>': if (dp != DATASIZE - 1) dp++; break;
            case '[': // TODO: brackets can be nested
                if (!data[dp])
                    while (ip < codelen && code[ip] != ']')
                        ++ip;
                break;
            case ']':
                if (data[dp])
                    while (ip && code[ip] != '[')
                        --ip;
                break;
        }
    }
    return 0;
}
