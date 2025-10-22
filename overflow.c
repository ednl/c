#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
int main(void)
{
    int64_t a =         3  * 1000 * 1000 * 1000;
    int64_t b = INT64_C(3) * 1000 * 1000 * 1000;
    printf("%"PRId64"\n", a);  // -1294967296
    printf("%"PRId64"\n", b);  // 3000000000
}
