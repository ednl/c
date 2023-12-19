/**
 * Problem: distribute points evenly on a sphere.
 * Try: n-body problem with repulsive force,
 *      and motion constricted to sphere.
 * Status: works for N=2, 4, 6; not for 3, 5.
 * For N=3: angles should be 120,120,120 but are 90,90,180 in a T-shape.
 * Same for N=5: all points q[1..] in a plane perpendicular to q[0].
 */

#include <stdio.h>
#include <stdlib.h>  // rand
#include <math.h>    // sqrt, acos
#include <float.h>   // DBL_EPSILON
#include <time.h>    // time

#define N 5
#define DEG (180 / M_PI)

typedef struct vec {
    double x, y, z;
} Vec;

typedef struct body {
    Vec q, v, a;  // position, velocity, acceleration
} Body;

static Body body[N];
static double phi[N][N];  // phi_ij = angle between qi and qj

// static Vec add(const Vec v, const Vec w)
// {
//     return (Vec){v.x + w.x, v.y + w.y, v.z + w.z};
// }

static void add_r(Vec* const v, const Vec w)
{
    v->x += w.x;
    v->y += w.y;
    v->z += w.z;
}

static Vec sub(const Vec v, const Vec w)
{
    return (Vec){v.x - w.x, v.y - w.y, v.z - w.z};
}

static void sub_r(Vec* const v, const Vec w)
{
    v->x -= w.x;
    v->y -= w.y;
    v->z -= w.z;
}

static Vec mul(const Vec v, const double a)
{
    return (Vec){v.x * a, v.y * a, v.z * a};
}

static void mul_r(Vec* const v, const double a)
{
    v->x *= a;
    v->y *= a;
    v->z *= a;
}

static double dot(const Vec v, const Vec w)
{
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

static Vec cross(const Vec v, const Vec w)
{
    return (Vec){v.y * w.z - v.z * w.y, v.z * w.x - v.x * w.z, v.x * w.y - v.y * w.x};
}

static double len(const Vec v)
{
    return sqrt(dot(v, v));
}

static void norm_r(Vec* const v)
{
    const double n = len(*v);
    if (fpclassify(n) != FP_ZERO && isgreater(fabs(n), DBL_EPSILON))
        mul_r(v, 1 / n);
}

static double angle(const Vec v, const Vec w)
{
    return acos(dot(v, w));  // divide by len(v).len(w) if not normalised
}

static void integrate(const double dt)
{
    // Sum forces
    Body* bi = &body[0];
    for (int i = 0; i < N - 1; ++i, ++bi) {
        bi->a = (Vec){0};
        Body* bj = bi + 1;
        for (int j = i + 1; j < N; ++j, ++bj) {
            bj->a = (Vec){0};
            const Vec dq = sub(bi->q, bj->q);  // repulsive force (normally: qj-qi = attractive)
            const double dotdq = dot(dq, dq), dq3 = dotdq * sqrt(dotdq);
            const Vec da = mul(dq, 10 / dq3);  // factor 10 for more effect
            add_r(&bi->a, da);
            sub_r(&bj->a, da);
        }
    }
    // Integration step
    Body* b = &body[0];
    for (int i = 0; i < N; ++i, ++b) {
        sub_r(&b->a, mul(b->q, dot(b->a, b->q) / dot(b->q, b->q)));  // orth. projection of a onto q
        sub_r(&b->a, mul(b->v, -0.1));  // friction term
        add_r(&b->v, mul(b->a, dt));  // velocity integration step
        add_r(&b->q, mul(b->v, dt));  // position integration step
        norm_r(&b->q);
    }
}

// phi_ij = angle between qi and qj
static void getangles(void)
{
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            phi[i][j] = angle(body[i].q, body[j].q);
}

static void show(void)
{
    for (int i = 0; i < N; ++i)
        printf("%2d: len(%6.3f,%6.3f,%6.3f)=%.2f\n", i, body[i].q.x, body[i].q.y, body[i].q.z, len(body[i].q));
    printf("\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 1; j < N; ++j)
            if (i < j) printf(" %5.1f", phi[i][j] * DEG);
            else       printf("      ");
        printf("\n");
    }
}

static Vec matmul(const double M[3][3], const Vec x)
{
    return (Vec){
        M[0][0] * x.x + M[0][1] * x.y + M[0][2] * x.z,
        M[1][0] * x.x + M[1][1] * x.y + M[1][2] * x.z,
        M[2][0] * x.x + M[2][1] * x.y + M[2][2] * x.z};
}

int main(void)
{
    // Start at random positions.
    srand48(time(NULL));
    for (int i = 0; i < N; ++i) {
        body[i].q.x = drand48() * 2 - 1;
        body[i].q.y = drand48() * 2 - 1;
        body[i].q.z = drand48() * 2 - 1;
        norm_r(&body[i].q);
    }
    getangles();
    show();

    // Evolve
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 2000; ++j)  // increase for closer to final positions
            integrate(0.001);  // decrease for more accurate integration
        getangles();
        printf("%4d:\n", i);
        show();
    }

    // Rotate so that q0 becomes (1,0,0), for clearer picture.
    Vec u = cross(body[0].q, (Vec){1,0,0});
    norm_r(&u);
    const double theta = angle(body[0].q, (Vec){1,0,0});
    const double c = cos(theta), c1 = 1 - c;
    const double s = sin(theta);
    double R[3][3];
    R[0][0] = c + u.x * u.x * c1;
    R[1][1] = c + u.y * u.y * c1;
    R[2][2] = c + u.z * u.z * c1;
    R[0][1] = u.x * u.y * c1 - u.z * s;
    R[1][0] = u.x * u.y * c1 + u.z * s;
    R[0][2] = u.x * u.z * c1 + u.y * s;
    R[2][0] = u.x * u.z * c1 - u.y * s;
    R[1][2] = u.y * u.z * c1 - u.x * s;
    R[2][1] = u.y * u.z * c1 + u.x * s;

    for (int i = 0; i < N; ++i)
        body[i].q = matmul(R, body[i].q);
    getangles();
    show();

    return 0;
}
