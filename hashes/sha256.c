#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define BIG_ENDIAN_32(X) (((X) << 24) | (((X) << 8) & 0xff0000) | (((X) >> 8) & 0xff00) | ((X) >> 24))

#define SHA256_DIGEST_SIZE 8

#define ROL(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROR(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROR(x,2) ^ ROR(x,13) ^ ROR(x,22))
#define EP1(x) (ROR(x,6) ^ ROR(x,11) ^ ROR(x,25))
#define S0(x) (ROR(x,7) ^ ROR(x,18) ^ ((x) >> 3))
#define S1(x) (ROR(x,17) ^ ROR(x,19) ^ ((x) >> 10))

static uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void 
buffer_to_block(char* buffer, int length, uint32_t block[16]) {
    for (uint64_t i = 0; i < 16; i += 1) {
        block[i] = ((uint32_t)(buffer[4*i+3] & 0xff) | ((uint32_t)(buffer[4*i+2] & 0xff)<<8)
            | ((uint32_t)(buffer[4*i+1] & 0xff)<<16)
            | ((uint32_t)(buffer[4*i+0] & 0xff)<<24));
    }
}

static void transform(char* buffer, uint32_t digest[8], uint32_t ms[64]) {
    for(int i = 0; i < 16; ++i) {
        ms[i] = BIG_ENDIAN_32(((uint32_t*)buffer)[i]);
    }
    for(int i = 16; i < 64; ++i) {
        ms[i] = S1(ms[i - 2]) + ms[i - 7] + S0(ms[i - 15]) + ms[i - 16];
    }

    uint32_t a = digest[0];
	uint32_t b = digest[1];
	uint32_t c = digest[2];
	uint32_t d = digest[3];
	uint32_t e = digest[4];
	uint32_t f = digest[5];
	uint32_t g = digest[6];
	uint32_t h = digest[7];

    for (int i = 0; i < 64; ++i) {
		uint32_t t1 = h + EP1(e) + CH(e,f,g) + k[i] + ms[i];
		uint32_t t2 = EP0(a) + MAJ(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	digest[0] += a;
	digest[1] += b;
	digest[2] += c;
	digest[3] += d;
	digest[4] += e;
	digest[5] += f;
	digest[6] += g;
	digest[7] += h;
}

void 
sha256(char* buffer, int length, char out[24]) {
    uint64_t total_bits = length * 8;

    uint32_t digest[8] = { 
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    uint32_t message_schedule[64] = {0};

    // for each 64 bit chunk do
    for(int i = 0; i < length / 64; ++i) {
        transform(buffer, digest, message_schedule);
        buffer += 64;
    }

    char last_buffer[64] = {0};
    // n is the amount of bytes still left
    int n = length % 64;
    // copy it to the buffer with padding
    memcpy(last_buffer, buffer, n);

    last_buffer[n++] = 0x80;

    if(n > 56) {
        // there is no more space to put the length
        transform(last_buffer, digest, message_schedule);
        memset(last_buffer, 0, 64);
        ((uint32_t*)last_buffer)[16 - 1] = BIG_ENDIAN_32((uint32_t)total_bits);
        ((uint32_t*)last_buffer)[16 - 2] = BIG_ENDIAN_32((uint32_t)(total_bits >> 32));
        transform(last_buffer, digest, message_schedule);
    } else {
        // there is still space
        ((uint32_t*)last_buffer)[16 - 1] = BIG_ENDIAN_32((uint32_t)total_bits);
        ((uint32_t*)last_buffer)[16 - 2] = BIG_ENDIAN_32((uint32_t)(total_bits >> 32));
        transform(last_buffer, digest, message_schedule);
    }

    ((uint32_t*)out)[0] = BIG_ENDIAN_32(digest[0]);
    ((uint32_t*)out)[1] = BIG_ENDIAN_32(digest[1]);
    ((uint32_t*)out)[2] = BIG_ENDIAN_32(digest[2]);
    ((uint32_t*)out)[3] = BIG_ENDIAN_32(digest[3]);
    ((uint32_t*)out)[4] = BIG_ENDIAN_32(digest[4]);
    ((uint32_t*)out)[5] = BIG_ENDIAN_32(digest[5]);
    ((uint32_t*)out)[6] = BIG_ENDIAN_32(digest[6]);
    ((uint32_t*)out)[7] = BIG_ENDIAN_32(digest[7]);
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

void 
sha256_to_string(char in[32], char out[64]) {
    for (uint64_t i = 0; i < SHA256_DIGEST_SIZE; i += 1) {
        uint32_t v = ((uint32_t*)in)[i];
        u32_to_str_base16(BIG_ENDIAN_32(v), 1, (char*)out + (i * 8));
    }
}

void sha256_print(char in[32]) {
    char r[64] = {0};
    sha256_to_string(in, r);
    printf("%.*s", 64, r);
}

void test_sha256() {
    char res[32] = {0};
    char buffer[256] = {0};
    for(int i = 0; i < 256; ++i) {
        buffer[i] = '6';
    }

    for(int i = 0; i < 256; ++i) {
        sha256(buffer, i, res);
        printf("%d ", i);
        sha256_print(res);
        printf("\n");
    }
}

int main() {
    test_sha256();

    return 0;
}