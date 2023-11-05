#include <stdio.h>
#include <stdint.h>
#include <assert.h>

int main(void)
{
    assert(sizeof(float) == sizeof(uint32_t));
    union T {
        uint32_t u;
        float r;
    } t = {0x40000001};
    for (int i = 0; t.u; ++i, t.r /= 2)
        printf("%3d: %08x %.8e\n", i, t.u, t.r);
    return 0;
}
