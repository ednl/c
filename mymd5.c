#include <stdio.h>    // fputs, size_t
#include <stdint.h>   // uint8_t, uint32_t, uint64_t
#include <stdbool.h>  // bool, false, true
#include "mymd5.h"

// Single hex character from value [0..15], hopefully fast
static char hexc(const uint8_t val)
{
    return "0123456789abcdef"[val];
}

// Ref.: https://en.wikipedia.org/wiki/MD5#Algorithm
//   message must be null terminated string of any length
//   digest buffer size must be >= 33 (32 hex characters + NUL)
void mymd5(const char* const message, char* const digest)
{
    static const uint32_t rot[64] = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};
    static const uint32_t K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

    // a0, b0, c0, d0
    uint32_t sum[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};

    uint64_t msglen = 0;  // message length in bytes (later in bits mod 2^64)
    const char* c = message;
    bool padded = false, terminated = false;

    // for each 512-bit (64-byte) chunk of padded message do
    while (!terminated) {

        size_t chunklen = 0;
        uint8_t chunk[64] = {0};

        while (*c && chunklen < 64)
            chunk[chunklen++] = (uint8_t)*c++;
        msglen += chunklen;

        // Padding: append 0x80 and pad with 0x00 bytes so that the message length in bytes ≡ 56 (mod 64).
        if (chunklen < 64) {
            if (!padded) {
                chunk[chunklen++] = 0x80;  // rest of chunk array is already zero from initialisation
                padded = true;
            }
            if (chunklen <= 56) {
                // Store length of original message *in bits* in last 64 bits of chunk (little-endian!)
                chunklen = 56;
                msglen <<= 3;  // byte count is now bit count (mod 2^64)
                while (msglen) {
                    chunk[chunklen++] = (uint8_t)msglen;  // use only the LSByte
                    msglen >>= 8;  // next byte
                }
                terminated = true;
            }
        }

        // Break 512-bit message into sixteen 32-bit words M[j]
        uint32_t M[16];
        for (int j = 0; j < 16; ++j) {
            int k = j << 2;  // j * 4
            M[j] = (uint32_t)chunk[k + 3] << 24  // high to low prec: cast, shift, or
                 | (uint32_t)chunk[k + 2] << 16
                 | (uint32_t)chunk[k + 1] <<  8
                 | (uint32_t)chunk[k];
        }

        uint32_t A = sum[0], B = sum[1], C = sum[2], D = sum[3];
        for (uint32_t i = 0; i < 64; ++i) {
            uint32_t F, g;
            switch (i >> 4) {
                case 0:
                    F = D ^ (B & (C ^ D));
                    g = i;
                    break;
                case 1:
                    F = C ^ (D & (B ^ C));
                    g = (i * 5 + 1) & 0xf;
                    break;
                case 2:
                    F = B ^ C ^ D;
                    g = (i * 3 + 5) & 0xf;
                    break;
                default:
                    F = C ^ (B | ~D);
                    g = (i * 7) & 0xf;
                    break;
            }
            F += A + K[i] + M[g];
            A = D;
            D = C;
            C = B;
            B += F << rot[i] | F >> (32 - rot[i]);
        }
        sum[0] += A;
        sum[1] += B;
        sum[2] += C;
        sum[3] += D;
    }

    // Save hex digest[0..31]
    char* d = digest;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            *d++ = hexc(sum[i] >>  4 & 0xf);  // high to low prec: shift, and
            *d++ = hexc(sum[i]       & 0xf);
            sum[i] >>= 8;
        }
    *d = '\0';  // digest[32] = NUL
}

// Return string pointer to md5 hex digest of message
//   message must be null terminated string of any length
// NB: not thread-safe because it uses a single static buffer
char* mymd5_tostr(const char* const message)
{
    static char digest[64];
    mymd5(message, digest);
    return digest;
}

// Print md5 hex digest of message to stdout
//   message must be null terminated string of any length
// NB: not thread-safe because mymd5_str() uses a single static buffer
void mymd5_print(const char* const message)
{
    puts(mymd5_tostr(message));
}

// Transform message to md5 hex digest in-place
//   message must be null terminated string of any length
//   message buffer size must be >= 33 (32 hex characters + NUL)
void mymd5_inplace(char* const message)
{
    mymd5(message, message);
}

// Transform message to md5 hex digest in-place, stretched N times
//   message must be null terminated string of any length
//   message buffer size must be >= 33 (32 hex characters + NUL)
//   stretch should be > 0 and not very large
void mymd5_stretch(char* const message, const int n)
{
    for (int i = 0; i < n; ++i)
        mymd5(message, message);
}
