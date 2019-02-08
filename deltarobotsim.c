#include <stdio.h>

/**
 * Delta robot physical characteristics
 * L_BASE = base equilateral triangle size (mm)
 * L_TOP  = top equilateral triangle size (mm)
 * L_ARM1 = lower arm length (mm)
 * L_ARM2 = upper arm length (mm)
 */
#define L_BASE  175.0f
#define L_TOP    62.5f
#define L_ARM1   50.0f
#define L_ARM2  202.0f

/**
 * Mathematical constants
 */
#define SQR3    1.73205080757f

/**
 * Global constants for faster calculation
 */
static const float k0 = (L_BASE - L_TOP) / 2;         /*  1/2 . (L1 - L2)          */
static const float k1 = SQR3 * (L_BASE - L_TOP) / 3;  /*  1/3 . sqr(3) . (L1 - L2) */
static const float k2 = SQR3 * (L_BASE - L_TOP) / 6;  /*  1/6 . sqr(3) . (L1 - L2) */
static const float m0 = L_ARM1 / 2;                   /*  1/2 . L_ARM1             */
static const float m1 = SQR3 * L_ARM1 / 2;            /*  1/2 . sqr(3) . L_ARM1    */
static const float arm2target = L_ARM2 * L_ARM2;      /*  what arm2() should be    */
static const float sintable[91] = {
	0.0f         ,0.0174524064f,0.0348994967f,0.0523359562f,0.0697564737f,
	0.0871557427f,0.1045284633f,0.1218693434f,0.1391731010f,0.1564344650f,
	0.1736481777f,0.1908089954f,0.2079116908f,0.2249510543f,0.2419218956f,
	0.2588190451f,0.2756373558f,0.2923717047f,0.3090169944f,0.3255681545f,
	0.3420201433f,0.3583679495f,0.3746065934f,0.3907311285f,0.4067366431f,
	0.4226182617f,0.4383711468f,0.4539904997f,0.4694715628f,0.4848096202f,
	0.5f         ,0.5150380749f,0.5299192642f,0.5446390350f,0.5591929035f,
	0.5735764364f,0.5877852523f,0.6018150232f,0.6156614753f,0.6293203910f,
	0.6427876097f,0.6560590290f,0.6691306064f,0.6819983601f,0.6946583705f,
	0.7071067812f,0.7193398003f,0.7313537016f,0.7431448255f,0.7547095802f,
	0.7660444431f,0.7771459615f,0.7880107536f,0.7986355100f,0.8090169944f,
	0.8191520443f,0.8290375726f,0.8386705679f,0.8480480962f,0.8571673007f,
	0.8660254038f,0.8746197071f,0.8829475929f,0.8910065242f,0.8987940463f,
	0.9063077870f,0.9135454576f,0.9205048535f,0.9271838546f,0.9335804265f,
	0.9396926208f,0.9455185756f,0.9510565163f,0.9563047560f,0.9612616959f,
	0.9659258263f,0.9702957263f,0.9743700648f,0.9781476007f,0.9816271834f,
	0.9848077530f,0.9876883406f,0.9902680687f,0.9925461516f,0.9945218954f,
	0.9961946981f,0.9975640503f,0.9986295348f,0.9993908270f,0.9998476952f,
	1.0f
};

float sinLookup(char degrees);
float cosLookup(char degrees);
float arm2(char n, char a, float x, float y, float z);

int main(void)
{
    float x = 0.0f, y = 0.0f, z = 137.5f;
    char a = 0;
    float e0, e1 = 0.0f;

    printf("Find servo angles in degrees for given x,y,z coordinates in mm (centered: x=0, y=0)\n");
    while (z <= 241.0f) {

        e0 = arm2(0, a, x, y, z);
        if (e0 > arm2target) {

            while (e0 > arm2target) {
                e1 = e0;
                e0 = arm2(0, ++a, x, y, z);
            }
            if (e1 - arm2target < arm2target - e0)
                --a;

        } else if (e0 < arm2target) {

            while (e0 < arm2target) {
                e1 = e0;
                e0 = arm2(0, --a, x, y, z);
            }
            if (arm2target - e1 < e0 - arm2target)
                ++a;

        }
        printf("(0,0,%.1f) = %i\n", z, a);
        z += 0.5f;
    }
    return 0;
}

/**
 * Sine and cosine functions
 * -90 <= degrees <= 90
 */
float sinLookup(char degrees)
{
	if (degrees >= 0)
		return sintable[degrees];
	return -sintable[-degrees];
}
float cosLookup(char degrees)
{
	if (degrees >= 0)
		return sintable[90 - degrees];
	return sintable[90 + degrees];
}

/**
 * Calculate upper arm length (squared) connected to servo n
 *   n = servo number 0/1/2
 *   a = servo angle in degrees -90 .. +90
 *   p = delta robot target point (x,y,z)
 */
float arm2(char n, char a, float x, float y, float z)
{
	/* (dx,dy,dz) = p_pols(x,y,z) - p_elleboog(x,y,z) */
	float dx = x, dy = y, dz = z - L_ARM1 * sinLookup(a);
	float cosa = cosLookup(a);

	switch (n)
	{
		case 0:
			dx -= k1 + L_ARM1 * cosa;
			break;
		case 1:
			dx += k2 + m0 * cosa;
			dy -= k0 + m1 * cosa;
			break;
		case 2:
			dx += k2 + m0 * cosa;
			dy += k0 + m1 * cosa;
			break;
	}
	return dx*dx + dy*dy + dz*dz;
}
