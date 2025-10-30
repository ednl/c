#include <stdio.h>   // printf
#include <stdlib.h>  // random, RAND_MAX
#include <string.h>  // memset
#include <time.h>
#include <stdbool.h>

#define DIM   9
#define TOTAL (DIM * DIM)
#define FREE  '.'
#define BEGIN 'A'
#define END  ('Z' + 1)

typedef enum dir {
    UP, DOWN, LEFT, RIGHT, DIRSIZE
} Dir;

typedef struct vec {
    int x, y;
} Vec;

static const Vec step[] = {
    [UP   ] = { 0, -1},
    [DOWN ] = { 0,  1},
    [LEFT ] = {-1,  0},
    [RIGHT] = { 1,  0},
};
static const Vec startpos = {DIM / 2, DIM / 2};
static char grid[DIM][DIM];

// https://en.cppreference.com/w/c/numeric/random/rand
static int random_unbiased(const int n)
{
    if (n < 2)
        return 0;
    const unsigned long m = (RAND_MAX + 1UL) / n;
    unsigned long x;
    do {
        x = random() / m;
    } while (x == (unsigned long)n);
    return (int)x;
}

static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

static bool isfree(const Vec pos)
{
    if (pos.x < 0 || pos.x >= DIM || pos.y < 0 || pos.y >= DIM)
        return false;
    return grid[pos.y][pos.x] == FREE;
}

static void set(const Vec pos, const char val)
{
    grid[pos.y][pos.x] = val;
}

int main(void)
{
    srandom(time(NULL));
    Dir dir[DIRSIZE];
    for (unsigned walk = 0, max = 0; max < TOTAL; ++walk) {
        memset(grid, FREE, sizeof grid);
        Vec pos = startpos;
        unsigned len = 0;
        char head = BEGIN;
        while (true) {
            set(pos, head++);
            if (head == END)
                head = BEGIN;
            len++;
            int count = 0;
            for (int i = 0; i < DIRSIZE; ++i)
                if (isfree(add(pos, step[i]))) dir[count++] = i;
            if (!count)
                break;
            add_r(&pos, step[dir[random_unbiased(count)]]);
        }
        if (len > max) {
            for (int i = 0; i < DIM; ++i) {
                fwrite(grid[i], DIM, 1, stdout);
                fputc('\n', stdout);
            }
            printf("\n%u (%u)\n\n", max = len, walk);
        }
    }
    return 0;
}
