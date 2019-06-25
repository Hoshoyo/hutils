#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define BIG_ENDIAN_32(X) (((X) << 24) | (((X) << 8) & 0xff0000) | (((X) >> 8) & 0xff00) | ((X) >> 24))
#define ROL(V, B) (((V) << (B)) | ((V) >> (32 - (B))))
#define BLOCK_INTS 16

void buffer_to_block(char* buffer, uint32_t block[16]) {
    for (uint64_t i = 0; i < BLOCK_INTS; i += 1) {
        block[i] = ((uint32_t)(buffer[4*i+3] & 0xff) | ((uint32_t)(buffer[4*i+2] & 0xff)<<8)
                   | ((uint32_t)(buffer[4*i+1] & 0xff)<<16)
                   | ((uint32_t)(buffer[4*i+0] & 0xff)<<24));
    }
}

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

static uint32_t r[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};
static uint32_t k[] = {
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
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

void transform(uint32_t digest[4], uint32_t block[16]) {
    uint32_t A = digest[0];
    uint32_t B = digest[1];
    uint32_t C = digest[2];
    uint32_t D = digest[3];

    for (int i = 0; i < 64; i += 1) {
        uint32_t F = 0;
        uint32_t g = 0;

        if (i >= 0 && i <= 15) {
            F = (B & C) | ((~B) & D);
            g = i;
        } else if (i < 32) {
            F = (D & B) | ((~D) & C);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            F = B ^ C ^ D;
            g = (3 * i + 5) % 16;
        } else {
            F = C ^ (B | (~D));
            g = (7 * i) % 16;
        }
        F = F + A + k[i] + block[g];
        uint32_t temp = D;
        D = C;
        C = B;
        B = B + ROL(F, r[i]);
        A = temp;
    }
    //Add this chunk's hash to result so far:
    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
}

void md5(char* buffer, int length, char out[16]) {
    uint32_t digest[] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };
    uint32_t block[16] = {0};

    uint64_t total_bits = length * 8;

    // for each 64 bit chunk do
    for(int i = 0; i < length / 64; ++i) {
        buffer_to_block(buffer, block);
        transform(digest, block);
        buffer += 64;
    }

    char last_buffer[64] = {0};

    // n is the amount of bytes still left
    int n = length % 64;
    // copy it to the buffer with padding
    memcpy(last_buffer, buffer, n);

    last_buffer[n++] = 0x80; // this is safe, since n <= 63

    if(n > 56) {
        // there is no more space to put the length
        memcpy(block, last_buffer, 64);
        transform(digest, block);
        memset(last_buffer, 0, 64);
        memcpy(block, last_buffer, 64);
        block[BLOCK_INTS - 1] = (uint32_t)(total_bits >> 32);
        block[BLOCK_INTS - 2] = (uint32_t)(total_bits);
        transform(digest, block);
    } else {
        // there is still space
        //buffer_to_block(last_buffer, block);
        memcpy(block, last_buffer, 64);

        block[BLOCK_INTS - 1] = (uint32_t)(total_bits >> 32);
        block[BLOCK_INTS - 2] = (uint32_t)(total_bits);
        transform(digest, block);
    }

    ((uint32_t*)out)[0] = digest[0];
    ((uint32_t*)out)[1] = digest[1];
    ((uint32_t*)out)[2] = digest[2];
    ((uint32_t*)out)[3] = digest[3];
}

void md5_print(char in[16]) {
    char result_buffer[8];
    #define DIGEST_SIZE 4
    for (uint64_t i = 0; i < DIGEST_SIZE; i += 1) {
        uint32_t v = BIG_ENDIAN_32(((uint32_t*)in)[i]);
        u32_to_str_base16(v, 1, result_buffer);
        printf("%.*s", 8, (char*)result_buffer);
    }
}

void test_md5() {
    char res[16] = {0};
    char in[256] = {0};
    for(int j = 0; j < 256; ++j) {
        in[j] = '6';
    }
    
    for(int i = 0; i < 256; ++i) {
        if(i == 64) {
            int xx = 0;
        }
        md5(in, i, res);
        printf("%d ", i);
        md5_print(res);
        printf("\n");
    }
}

int main() {
    //char res[16] = {0};
    //md5("abc", 3, res);
    //md5_print(res);

    test_md5();

    return 0;
}