/**
 * Compiler output:
 * enum-types.c:8:15: warning: implicit conversion from enumeration type 'enum fruit'
 * to different enumeration type 'Veg' (aka 'enum veg') [-Wenum-conversion]
 *     8 |     Veg veg = BANANA;
 *       |         ~~~   ^~~~~~
 * 1 warning generated.
 *
 * Program output:
 * veg=1
 */
#include <stdio.h>
typedef enum fruit {APPLE, BANANA} Fruit;
typedef enum veg {CABBAGE} Veg;
int main(void)
{
    Veg veg = BANANA;
    printf("veg=%d\n", veg);
    return 0;
}
