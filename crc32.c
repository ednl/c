// Code from https://www.w3.org/TR/PNG/#D-CRCAppendix
// Adapted for 32/64-bit architecture

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define NBITS        8
#define NVALUES      (1 << NBITS)
#define MODINDEX     (NVALUES - 1)
#define ONESCOMPL32  0xffffffffU
#define CRC_INIT     (ONESCOMPL32)
#define G_POLYNOMIAL 0xedb88320U

// CRC-32 of byte data
static uint32_t crc32(unsigned char *buf, int len)
{
    static uint32_t crc_table[NVALUES];  // table of CRCs of all n-bit messages
    static bool crc_table_todo = true;
    uint32_t c;
    int n, k;

    if (crc_table_todo) {
        // Pre-compute first 256 CRC values
        for (n = 0; n < NVALUES; ++n) {
            c = (uint32_t) n;
            for (k = 0; k < NBITS; k++) {
                if (c & 1) {
                    c = G_POLYNOMIAL ^ (c >> 1);
                } else {
                    c >>= 1;
                }
            }
            crc_table[n] = c;
        }
        crc_table_todo = false;
    }

    c = CRC_INIT;
    for (n = 0; n < len; ++n) {
        c = crc_table[(c ^ buf[n]) & MODINDEX] ^ (c >> NBITS);
    }

    return c ^ ONESCOMPL32;
}

int main(void)
{
    unsigned char i, j, msg[10];

    // Fill msg with values 0, 1, 2, 4, 8, 16, 32, 64, 128, 255
    msg[0] = 0;
    j = 1;
    for (i = 1; i < 9; ++i) {
        msg[i] = j;
        j <<= 1;
    }
    msg[9] = 255;

    // Test CRC-32
    // 00000000 77073096 ee0e612c 076dc419 0edb8832 1db71064 3b6e20c8 76dc4190 edb88320 2d02ef8d (init 0)
    // d202ef8d a505df1b 3c0c8ea1 d56f2b94 dcd967bf cfb5ffe9 e96ccf45 a4deae1d 3fba6cad ff000000 (init -1)
    for (i = 0; i < 10; ++i) {
        printf("%08"PRIx32" ", crc32(&msg[i], 1));
    }
    printf("\n");
    return 0;
}
