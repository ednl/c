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
    int i = 0;
    while (t.u) {
        printf("%3d: %08x %.8e\n", i++, t.u, t.r);
        t.r /= 2;
    }
    return 0;
}
