#include <stdio.h>
#include <stdlib.h>  // abs
#include <string.h>  // memset

#define CIRCLE 360
#define TURN1 45
#define DIRS (CIRCLE / TURN1)
#define W 64
#define H 8

typedef struct {
    int x, y;
} Pos;

static const Pos step[DIRS] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};

static char snow[H][W];

static int manh(const Pos a)
{
    return abs(a.x) + abs(a.y);
}

static Pos mul(const Pos a, const int m)
{
    return (Pos){a.x * m, a.y * m};
}

static void add(Pos * const a, const Pos b)
{
    a->x += b.x;
    a->y += b.y;
}

int main(void)
{
    FILE *f = fopen("infi-aoc22-input.txt", "r");
    if (!f)
        return 1;

    Pos pos = {0};  // start at origin
    int head = 0;   // head north
    char cmd[8];
    int val;

    memset(snow, ' ', sizeof snow);
    snow[0][0] = '*';

    while (fscanf(f, "%7s %d", cmd, &val) == 2) {
        switch (cmd[0]) {
            case 'd':
                head = (head + val / TURN1) % DIRS;
                if (head < 0) head += DIRS;
                break;
            case 'l':
                while (val--) {
                    add(&pos, step[head]);
                    snow[pos.y][pos.x] = '*';
                }
                break;
            case 's':
                add(&pos, mul(step[head], val));
                snow[pos.y][pos.x] = '*';
                break;
        }
    }
    fclose(f);
    printf("Part 1: %d\n", manh(pos));
    for (int y = H - 1; y >= 0; --y) {
        snow[y][W - 1] = '\0';
        puts(snow[y]);
    }
    return 0;
}
