#include <stdio.h>
#define SZ_F  (sizeof(float))
#define SZ_D  (sizeof(double))
#define SZ_LD (sizeof(long double))
//#define SZ_F128 (sizeof(__float128))  // often unsupported
int main(void) {
    printf("float: %zu\ndouble: %zu\nlong double: %zu\n", SZ_F, SZ_D, SZ_LD);
    //printf("float128: %zu\n", SZ_F128);
    return 0;
}
