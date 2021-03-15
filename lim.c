#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <float.h>
// #include <math.h>

int main(void)
{
    printf("\n----- Char is Signed or Unsigned ------\n");
    printf("char == %ssigned char\n", CHAR_MAX == SCHAR_MAX ? "" : "un");

    printf("\n----- Integer Sizes -------------------\n");
    printf("char         = %zu\n", sizeof(char     ) * CHAR_BIT);
    printf("short        = %zu\n", sizeof(short    ) * CHAR_BIT);
    printf("int          = %zu\n", sizeof(int      ) * CHAR_BIT);
    printf("long         = %zu\n", sizeof(long     ) * CHAR_BIT);
    printf("long long    = %zu\n", sizeof(long long) * CHAR_BIT);
    printf("size_t       = %zu\n", sizeof(size_t   ) * CHAR_BIT);
    printf("intptr_t     = %zu\n", sizeof(intptr_t ) * CHAR_BIT);
    printf("intmax_t     = %zu\n", sizeof(intmax_t ) * CHAR_BIT);

    printf("\n----- Fast Integer Sizes --------------\n");
    printf("int_fast8_t  = %zu\n", sizeof(int_fast8_t ) * CHAR_BIT);
    printf("int_fast16_t = %zu\n", sizeof(int_fast16_t) * CHAR_BIT);
    printf("int_fast32_t = %zu\n", sizeof(int_fast32_t) * CHAR_BIT);
    printf("int_fast64_t = %zu\n", sizeof(int_fast64_t) * CHAR_BIT);

    printf("\n----- Floating Point Types ------------\n");
    printf("float            = %zu\n", sizeof(float      ) * CHAR_BIT);
    printf("double           = %zu\n", sizeof(double     ) * CHAR_BIT);
    printf("long double      = %zu\n", sizeof(long double) * CHAR_BIT);
    printf("FLT_RADIX        = %d\n", FLT_RADIX);
    printf("FLT_ROUNDS       = %d (-1,0,1,2,3 = ?,to 0,nearest,+inf,-inf)\n", FLT_ROUNDS);
    printf("FLT_EVAL_METHOD  = %d (0,1,2 = none,double,long double)\n", FLT_EVAL_METHOD);
    printf("FLT_HAS_SUBNORM  = %d (-1,0,1 = ?,no,yes)\n", FLT_HAS_SUBNORM);

    printf("\n----- Float Limits --------------------\n");
    printf("FLT_DECIMAL_DIG  = %d\n", FLT_DECIMAL_DIG);
    printf("FLT_MIN          = %.8e\n", FLT_MIN);
    printf("FLT_TRUE_MIN     = %.8e\n", FLT_TRUE_MIN);
    printf("FLT_MAX          = %.8e\n", FLT_MAX);
    printf("FLT_EPSILON      = %.8e\n", FLT_EPSILON);
    printf("FLT_DIG          = %d\n", FLT_DIG);
    printf("FLT_MANT_DIG     = %d\n", FLT_MANT_DIG);
    printf("FLT_MIN_EXP      = %d\n", FLT_MIN_EXP);
    printf("FLT_MIN_10_EXP   = %d\n", FLT_MIN_10_EXP);
    printf("FLT_MAX_EXP      = %d\n", FLT_MAX_EXP);
    printf("FLT_MAX_10_EXP   = %d\n", FLT_MAX_10_EXP);

    printf("\n----- Double Limits -------------------\n");
    printf("DBL_DECIMAL_DIG  = %d\n", DBL_DECIMAL_DIG);
    printf("DBL_MIN          = %.8e\n", DBL_MIN);
    printf("DBL_TRUE_MIN     = %.8e\n", DBL_TRUE_MIN);
    printf("DBL_MAX          = %.8e\n", DBL_MAX);
    printf("DBL_EPSILON      = %.8e\n", DBL_EPSILON);
    printf("DBL_DIG          = %d\n", DBL_DIG);
    printf("DBL_MANT_DIG     = %d\n", DBL_MANT_DIG);
    printf("DBL_MIN_EXP      = %d\n", DBL_MIN_EXP);
    printf("DBL_MIN_10_EXP   = %d\n", DBL_MIN_10_EXP);
    printf("DBL_MAX_EXP      = %d\n", DBL_MAX_EXP);
    printf("DBL_MAX_10_EXP   = %d\n", DBL_MAX_10_EXP);

    printf("\n----- Long Double Limits --------------\n");
    printf("LDBL_DECIMAL_DIG = %d\n", LDBL_DECIMAL_DIG);
    printf("LDBL_MIN         = %.8Le\n", LDBL_MIN);
    printf("LDBL_TRUE_MIN    = %.8Le\n", LDBL_TRUE_MIN);
    printf("LDBL_MAX         = %.8Le\n", LDBL_MAX);
    printf("LDBL_EPSILON     = %.8Le\n", LDBL_EPSILON);
    printf("LDBL_DIG         = %d\n", LDBL_DIG);
    printf("LDBL_MANT_DIG    = %d\n", LDBL_MANT_DIG);
    printf("LDBL_MIN_EXP     = %d\n", LDBL_MIN_EXP);
    printf("LDBL_MIN_10_EXP  = %d\n", LDBL_MIN_10_EXP);
    printf("LDBL_MAX_EXP     = %d\n", LDBL_MAX_EXP);
    printf("LDBL_MAX_10_EXP  = %d\n", LDBL_MAX_10_EXP);

    printf("\n");
	return 0;
}
