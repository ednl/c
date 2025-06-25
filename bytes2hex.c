#include <stdio.h>
#include <stdint.h>  // uint8_t

static int bytes2hex(const uint8_t *const digest, const int bitcount, char *buf, const int bufsize)
{
    // Sanity check, bitcount must be multiple of 8, buf must have room for NUL
    if (!digest || !buf || bitcount < 8 || bufsize < 3 || (bitcount & 7) || bufsize <= (bitcount >> 2))
        return 0;
    const uint8_t *const end = digest + (bitcount >> 3);
    for (const uint8_t *byte = digest; byte < end; ++byte) {
        *buf++ = "0123456789abcdef"[*byte >> 4 & 0xf];  // use 4 MSB
        *buf++ = "0123456789abcdef"[*byte      & 0xf];  // use 4 LSB
    }
    *buf = '\0';
    return bitcount >> 2;  // return number of hex characters written to buf
}

int main(void)
{
    uint32_t md5init[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    char buf[33];
    if (bytes2hex((const uint8_t *)md5init, 8 * sizeof md5init, buf, sizeof buf))
        printf("[%s]\n", buf);  // "[0123456789abcdeffedcba9876543210]"
    return 0;
}
