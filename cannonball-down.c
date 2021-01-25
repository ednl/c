// CANNONBALL NUMBERS
// https://en.wikipedia.org/wiki/Cannonball_problem
// For a stacking of cannonballs in a square pyramid ("polygon = 4"),
// is there a single square with holds the same number of cannonballs?
// In other words: which square pyramidal number is square?

// Numberphile and Matt Parker: same, but now polygon >= 3
// https://youtu.be/q6L06pyt9CA
// https://www.numberphile.com/cannon-ball-numbers

#include <stdio.h>
#include <limits.h>

static unsigned long long a0, ac, aa, v0, vc, va, vv, polygon = 4257300000;
// Checked:
// 2^32..4257300000
// 2^24..0

static int next_a(void)
{
    unsigned long long prev = aa;
    a0 += 2;
    ac += polygon;
    aa += ac - a0;
    return aa > prev;
}

static int next_v(void)
{
    unsigned long long prev = vv;
    v0 += 2;
    vc += polygon;
    va += vc - v0;
    vv += va;
    return vv > prev;
}

int main(void)
{
    unsigned long long prev = polygon + 1;
    int ok_a, ok_v, found;

    printf("CANNONBALL NUMBERS\n");
    printf("List all N-polygon pyramidal numbers which are N-polygonal\n");
    printf("where N >= 3 and any number <= %llu\n",  ULLONG_MAX);
    printf("\n");

    while (polygon < prev) {
        if (!(polygon % 100000ULL))
            printf("[%llu]\n", polygon);
        a0 = v0 = 1;
        ac = vc = aa = va = polygon;
        vv = polygon + 1;
        ok_a = ok_v = 1;
        found = 0;
        while (ok_v && ok_a) {
            while (aa != vv && ok_v && ok_a) {
                while (aa < vv && ok_a && ok_v)
                    ok_a = next_a();
                while (vv < aa && ok_v && ok_a)
                    ok_v = next_v();
            }
            if (aa == vv) {
                if (!found) {
                    printf("%llu:", polygon);
                }
                printf(" %llu", vv);
                ok_a = next_a();
                ++found;
            }
        }
        if (found) {
            printf(" (%i)\n", found);
        }
        prev = polygon--;
    }
    return 0;
}
