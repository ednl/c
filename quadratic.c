#include <stdlib.h>  // lldiv
#include <stddef.h>  // size_t
#include <stdint.h>  // int64_t
#include <float.h>   // DBL_EPSILON
#include <math.h>    // sqrt, fabs
#include "quadratic.h"

struct root_int {
    int64_t x0, x1;
    size_t count;
};

struct root_real {
    double x0, x1;
    size_t count;
};

Root_int quadratic_int(int64_t a, int64_t b, int64_t c)
{
    Root_int r = {0};
    switch (((a == 0) << 2) | ((b == 0) << 1) | (c == 0)) {
        case 7: r.count = 3; break; // 0=0    (a=0,b=0,c=0)
        case 3: /* fallthrough */   // ax^2=0 (a!=0,b=0,c=0)
        case 5: r.count = 1; break; // bx=0   (a=0,b!=0,c=0)
        case 4: { // bx+c=0 (a=0,b!=0,c!=0)
                const lldiv_t d = lldiv(-c, b);
                if (d.rem == 0) {
                    r.x0 = d.quot;
                    r.count = 1;
                }
            } break;
        case 1: { // ax^2+bx=0 (a!=0,b!=0,c=0)
                const lldiv_t d = lldiv(-b, a);
                if (!d.rem) {
                    r.x1 = d.quot;  // x0=0
                    r.count = 2;
                } else
                    r.count = 1;
            } break;
        case 2: { // ax^2+c=0 (a!=0,b=0,c!=0)
                const lldiv_t d = lldiv(-c, a);
                if (!d.rem && d.quot > 0) {
                    const int64_t h = llround(sqrt(d.quot));
                    if (h * h == d.quot) {
                        r.x0 = -h;
                        r.x1 =  h;
                        r.count = 2;
                    }
                }
            } break;
        case 0: { // ax^2+bx+c=0 (a!=0,b!=0,c!=0)
                const lldiv_t dba = lldiv(-b, a);
                if (dba.rem == 0 && (dba.quot & 1) == 0) {
                    const lldiv_t dca = lldiv(c, a);
                    if (dca.rem == 0) {
                        const int64_t sq = dba.quot * dba.quot - 4 * dca.quot;
                        if (sq > 0) {
                            const int64_t h = llround(sqrt(sq));
                            if (h * h == sq && (h & 1) == 0) {
                                r.x0 = (dba.quot - h) / 2;
                                r.x1 = (dba.quot + h) / 2;
                                r.count = 2;
                            }
                        } else if (sq == 0) {
                            r.x0 = dba.quot / 2;
                            r.count = 1;
                        }
                    }
                }
            } break;
    }
    return r;
}

Root_real quadratic_real(int64_t a, int64_t b, int64_t c)
{
    Root_real r = {0};
    switch (((a == 0) << 2) | ((b == 0) << 1) | (c == 0)) {
        case 7: r.count = 3; break; // 0=0    (a=0,b=0,c=0)
        case 3: /* fallthrough */   // ax^2=0 (a!=0,b=0,c=0)
        case 5: r.count = 1; break; // bx=0   (a=0,b!=0,c=0)
        case 4: // bx+c=0 (a=0,b!=0,c!=0)
            r.x0 = (double)-c / b;
            r.count = 1;
            break;
        case 1: // ax^2+bx=0 (a!=0,b!=0,c=0)
            r.x1 = (double)-b / a;
            r.count = 2;
            break;
        case 2: { // ax^2+c=0 (a!=0,b=0,c!=0)
                const double sq = (double)-c / a;
                if (sq >= 0) {
                    r.x1 = sqrt(sq);
                    r.x0 = -r.x1;
                    r.count = 2;
                } else
                    r.x1 = sqrt(-sq);
            } break;
        case 0: { // ax^2+bx+c=0 (a!=0,b!=0,c!=0)
                const double bb = (double)b / a;
                const double cc = (double)c / a;
                const double mid = (double)-bb / 2;
                const double sq = mid * mid - cc;
                if (fabs(sq) < DBL_EPSILON) {
                    r.x0 = mid;
                    r.count = 1;
                } else if (sq >= 0) {
                    const double h = sqrt(sq);
                    r.x0 = mid - h;
                    r.x1 = mid + h;
                    r.count = 2;
                } else {
                    r.x0 = mid;
                    r.x1 = sqrt(-sq);
                }
            } break;
    }
    return r;
}

