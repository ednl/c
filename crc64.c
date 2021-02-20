#include <stdio.h>      // printf
#include <stdint.h>     // uint64_t, UINT64_C
#include <inttypes.h>   // PRIu64, PRIX64
#include <stdbool.h>    // bool, true, false

#define CRC64_CHARBITS   (UINT64_C(8))
#define CRC64_NBITS      (UINT64_C(64))
#define CRC64_MBITS      (CRC64_NBITS - CRC64_CHARBITS)
#define CRC64_MSB        (UINT64_C(1) << (CRC64_NBITS - UINT64_C(1)))
#define CRC64_TABLESIZE  (UINT64_C(1) << CRC64_CHARBITS)
#define CRC64_MAXINDEX   (CRC64_TABLESIZE - UINT64_C(1))
#define CRC64_COMPLEMENT (UINT64_C(-1))
#define CRC64_GPOLYNOM   (UINT64_C(0x42F0E1EBA9EA3693))

typedef uint64_t hash_t;

static hash_t crc64(unsigned char *data, unsigned int len)
{
    static hash_t crc64_table[CRC64_TABLESIZE];
    static bool firstrun = true;
    hash_t crc;
    unsigned int i, j;

    if (firstrun) {
        // Make big-endian (MSB) table of first 256 CRC-64 values
        crc = CRC64_MSB;
        crc64_table[0] = 0;
        for (i = 1; i < CRC64_TABLESIZE; i <<= 1) {
            if (crc & CRC64_MSB) {
                crc = (crc << 1) ^ CRC64_GPOLYNOM;
            } else {
                crc <<= 1;
            }
            for (j = 0; j < i; ++j) {
                crc64_table[i + j] = crc ^ crc64_table[j];
            }
        }
        firstrun = false;
    }

    crc = CRC64_COMPLEMENT;
    for (i = 0; i < len; ++i) {
        crc = (crc << CRC64_CHARBITS) ^ crc64_table[(data[i] ^ (crc >> CRC64_MBITS)) & CRC64_MAXINDEX];
    }
    return crc ^ CRC64_COMPLEMENT;
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

    // init  0: 0000000000000000 42F0E1EBA9EA3693 85E1C3D753D46D26 493366450E42ECDF 9266CC8A1C85D9BE 663D78FF90E185EF CC7AF1FF21C30BDE DA050215EA6C212F F6FAE5C07D3274CD 9AFCE626CE85B507
    // init -1: 9AFCE626CE85B5F8 D80C07CD676F836B 1F1D25F19D51D8DE D3CF8063C0C75927 089A2AACD2006C46 FCC19ED95E643017 568617D9EF46BE26 40F9E43324E994D7 6C0603E6B3B7C135 00000000000000FF
    for (i = 0; i < 10; ++i) {
        printf("%016"PRIX64" ", crc64(&msg[i], 1));
    }
    printf("\n");
    return 0;
}
