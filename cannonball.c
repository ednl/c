#include <stdio.h>

// static long long known[] = {
//     3, 4, 5, 6, 8, 10, 11, 14, 17, 20, 23, 26, 29, 30, 32, 35,
//     38, 41, 43, 50, 88, 145, 276, 322, 823, 2378, 2386, 31265
// };

int main(void)
{
    // size_t i, n = sizeof known / sizeof *known;
    // for (i = 0; i < n; ++i) {
    //     polygon = known[i];
    unsigned long long ap, ac, aa, vp, vc, va, vv, polygon, pa, pv;

    polygon = 3;
    while (1) {
        printf("%llu", polygon);
        ap = vp = 1;
        ac = vc = aa = va = polygon;
        vv = polygon + 1;
        do {
            pa = aa - 1;
            while (vv > aa && aa > pa) {
                ap += 2;
                ac += polygon;
                pa = aa;
                aa += ac - ap;
            }
            pv = vv - 1;
            while (aa > vv && vv > pv) {
                vp += 2;
                vc += polygon;
                va += vc - vp;
                pv = vv;
                vv += va;
            }
        } while (aa != vv && aa > pa && vv > pv);
        if (aa == vv)
            printf(" %llu\n", vv);

        ++polygon;
    }
    return 0;
}
