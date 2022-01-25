#include <stdio.h>
#include <stdlib.h>  // atoi

#define N    (20)
#define SPC (' ')
#define DOT ('.')
#define DEL (127)

static int buf[N] = {0};

static void printbuf(int i)
{
    if (i > 0 && i <= N) {
        for (int j = 0; j < i; ++j)
            printf("%02x ", buf[j]);
        for (int j = i; j < N; ++j)
            printf("   ");
        for (int j = 0; j < i; ++j)
            printf("%c", buf[j] >= SPC && buf[j] < DEL ? buf[j] : DOT);
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(stderr, "Usage: dumpchar <file name> [number of chars]\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "File not found: %s\n", argv[1]);
        return 2;
    }
    int n = N;
    if (argc > 2) {
        n = atoi(argv[2]);
        if (n < 1)
            n = 1;
    }
    int i = 0;
    while (!feof(f) && n--) {
        int c = fgetc(f);
        buf[i++] = c;
        if (i == N) {
            printbuf(i);
            i = 0;
        }
    }
    fclose(f);
    printbuf(i);
    return 0;
}
