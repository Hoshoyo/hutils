#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static uint64_t 
u32_to_str_base16(uint32_t value, int leading_zeros, char* buffer)
{
    int i = 0;
    for (; i < 8; i += 1) {
        uint32_t f = (value & 0xf0000000) >> 28;
        if(f > 9) buffer[i] = (char)f + 0x57;
        else buffer[i] = (char)f + 0x30;
        value <<= 4;
    }
    return i;
}

uint32_t rol(uint32_t value, uint32_t bits) {
    return (value << bits) | (value >> (32 - bits));
}

void buffer_to_block(char* buffer, uint32_t block[16]) {
    const uint64_t BLOCK_INTS =  16;
    /* Convert the string (byte buffer) to a uint32_t array (MSB) */
    for (uint64_t i = 0; i < BLOCK_INTS; i += 1) {
        block[i] = ((uint32_t)(buffer[4*i+3] & 0xff) | ((uint32_t)(buffer[4*i+2] & 0xff)<<8)
                   | ((uint32_t)(buffer[4*i+1] & 0xff)<<16)
                   | ((uint32_t)(buffer[4*i+0] & 0xff)<<24));
    }
}

void md5(char* s, int length) {
    const uint64_t BLOCK_BYTES = 16 * 4;

    int r[64] = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };
    int k[64];
    
    while ((uint64_t)length < BLOCK_BYTES) {
        s[length] = 0;
        length += 1;
    }

    for(int i = 0; i < 64; i += 1) {
        k[i] = (int)floorf((float)((int)fabs(sinf((float)(i + 1))) * 0x10000000));
    }

    int a0 = 0x67452301;
    int b0 = 0xEFCDAB89;
    int c0 = 0x98BADCFE;
    int d0 = 0x10325476;

    s[length] = 0x80;

    for (int i = 0; i < length; i += 64) {
        uint32_t block[16];
        buffer_to_block(s, block);

        int A = a0;
        int B = b0;
        int C = c0;
        int D = d0;

        for (int i = 0; i < 64; i += 1) {
            int F;
            int g;

            if (i >= 0 && i <= 15) {
                F = (B & C) | ((~B) & D);
                g = i;
            } else if (i >= 16 && i <= 32) {
                F = (D & B) | ((~D) & C);
                g = (5 * i + 1) % 16;
            } else if (i >= 32 && i <= 47) {
                F = B ^ C ^ D;
                g = (3 * i + 5) % 16;
            } else if(i >= 48 && i <= 63) {
                F = C ^ (B | (~D));
                g = (7 * i) % 16;
            }
            //Be wary of the below definitions of a,b,c,d
            F = F + A + k[i] + (int)block[g];
            A = D;
            D = C;
            C = B;
            B = B + (int)rol((uint32_t)F, (uint32_t)s[i]);
        }
        //Add this chunk's hash to result so far:
        a0 = a0 + A;
        b0 = b0 + B;
        c0 = c0 + C;
        d0 = d0 + D;
    }
    
    char digest[16];
    //a0 append b0 append c0 append d0 //(Output is in little-endian)
    * (uint32_t*)digest = (uint32_t)a0;
    *((uint32_t*)digest + 1) = (uint32_t)b0;
    *((uint32_t*)digest + 2) = (uint32_t)c0;
    *((uint32_t*)digest + 3) = (uint32_t)d0;

    /* Hex std::string */
    char result_buffer[8];
    #define DIGEST_SIZE 5
    for (uint64_t i = 0; i < DIGEST_SIZE; i += 1) {
        u32_to_str_base16((uint32_t)digest[i], 1, result_buffer);
        printf("%.*s", 8, (char*)result_buffer);
    }
}

int main() {
    char sa[256];
    sa[0] = 'a';
    sa[1] = 'b';
    sa[2] = 'c';

    md5(sa, 3);

    return 0;
}