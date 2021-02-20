#include <stdio.h>      // printf
#include <stdint.h>     // uint64_t, UINT64_C
#include <inttypes.h>   // PRIu64, PRIX64
#include <stdbool.h>    // bool, true, false

#define CRC64_CHARBITS   (UINT64_C(8))
#define CRC64_NBITS      (UINT64_C(64))
#define CRC64_MBITS      (CRC64_NBITS - UINT64_C(8))
#define CRC64_MSB        (UINT64_C(1) << (CRC64_NBITS - UINT64_C(1)))
#define CRC64_TABLESIZE  (UINT64_C(256))
#define CRC64_MAXINDEX   (CRC64_TABLESIZE - UINT64_C(1))
#define CRC64_COMPLEMENT (UINT64_C(-1))
#define CRC64_GPOLYNOM   (UINT64_C(0x42F0E1EBA9EA3693))

typedef uint64_t hash_t;
static hash_t crc64_table[CRC64_TABLESIZE];

// Make big-endian (MSB) table of first 256 CRC-64 values
static void crc64_maketable(void)
{
    unsigned int i = 1, j;
    hash_t crc = CRC64_MSB;

    crc64_table[0] = 0;
    do {
        if (crc & CRC64_MSB) {
            crc = (crc << 1) ^ CRC64_GPOLYNOM;
        } else {
            crc <<= 1;
        }
        for (j = 0; j < i; ++j) {
            crc64_table[i + j] = crc ^ crc64_table[j];
        }
        i <<= 1;
    } while (i < CRC64_TABLESIZE);
}

static hash_t crc64(unsigned char *data, unsigned int len)
{
    unsigned int i;
    hash_t rem = CRC64_COMPLEMENT;

    for (i = 0; i < len; ++i) {
        rem = (rem << CRC64_CHARBITS) ^ crc64_table[(data[i] ^ (rem >> CRC64_MBITS)) & CRC64_MAXINDEX];
    }
    return rem ^ CRC64_COMPLEMENT;
}

int main(void)
{
    // printf("5432109876543210\n%016"PRIX64"\n", CRC64_COMPLEMENT);
    // return 0;

    unsigned char i, j, msg[10];

    crc64_maketable();
    for (i = 0; i < 32; ++i) {
        for (j = 0; j < 8; ++j) {
            printf("%016"PRIX64" ", crc64_table[i * 8 + j]);
        }
        printf("\n");
    }
    printf("\n");

    // Fill msg with values 0, 1, 2, 4, 8, 16, 32, 64, 128, 255
    msg[0] = 0;
    j = 1;
    for (i = 1; i < 9; ++i) {
        msg[i] = j;
        j <<= 1;
    }
    msg[9] = 255;

    for (i = 0; i < 10; ++i) {
        printf("%016"PRIX64" ", crc64(&msg[i], 1));
    }
    printf("\n");

    return 0;
}
