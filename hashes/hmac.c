#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hhashes.h"

#define BLOCK_INTS (16)
#define HMAC_BLOCK_SIZE 64
#define SHA1_BLOCK_SIZE 64
#define BLOCK_BYTES (16 * 4)
#define DIGEST_SIZE 5
#define MAX(A, B) ((A > B) ? (A) : (B))
#define MIN(A, B) ((A < B) ? (A) : (B))

void
hmac_sha1(char* key, int key_length, char* message, int message_length, char result[20]) {
    char temp_key[HMAC_BLOCK_SIZE] = {0};
    char o_key_pad[HMAC_BLOCK_SIZE] = {0};
    char i_key_pad[HMAC_BLOCK_SIZE] = {0};

    if(key_length > HMAC_BLOCK_SIZE) {
        sha1(key, key_length, temp_key);
    }
    if(key_length < HMAC_BLOCK_SIZE) {
        memcpy(temp_key, key, key_length);
    }
    
    for(int i = 0; i < HMAC_BLOCK_SIZE; ++i) {
        o_key_pad[i] = 0x5c ^ temp_key[i];
        i_key_pad[i] = 0x36 ^ temp_key[i];
    }

    char* m = calloc(1, MAX(message_length, HMAC_BLOCK_SIZE) + HMAC_BLOCK_SIZE + 1);
    memcpy(m, i_key_pad, HMAC_BLOCK_SIZE);
    memcpy(m + HMAC_BLOCK_SIZE, message, message_length);

    char h[20] = {0};
    sha1(m, message_length + HMAC_BLOCK_SIZE, h);

    memcpy(m, o_key_pad, HMAC_BLOCK_SIZE);
    memcpy(m + HMAC_BLOCK_SIZE, h, 20);

    sha1(m, HMAC_BLOCK_SIZE + 20, result);
    free(m);
}

void phash_sha1(char* secret, int secret_length, char* seed, int seed_length, char* result, int result_length_bytes) {
    int length = result_length_bytes;
    const int hash_length = 20;
    char A[20] = {0};
    char T[20] = {0};

    // Calculate A(1) = hmac(secret, A(0))
    hmac_sha1(secret, secret_length, seed, seed_length, A);

    if(length == 0) return;
    char* temp = calloc(1, hash_length + seed_length);

    int offset = 0;
    while(length > 0) {
        // Next A
        memcpy(temp, A, hash_length);
        memcpy(temp + hash_length, seed, seed_length);

        hmac_sha1(secret, secret_length, temp, hash_length + seed_length, T);
        int a = MIN(length, hash_length);
        memcpy(result + offset, T, a);
        length -= a;
        offset += a;

        hmac_sha1(secret, secret_length, A, hash_length, A);
    }

    free(temp);
}

void test_sha1() {
    char res[20] = {0};
    char in[256] = {0};
    for(int j = 0; j < 256; ++j) {
        in[j] = '6';
    }
    
    for(int i = 0; i < 256; ++i) {
        sha1(in, i, res);
        sha1_print(res);
        printf("\n");
    }
}

void test_hmac_sha1() {
    char res[20] = {0};
    hmac_sha1(
        "gigantic key which shall not be passed as an argument to any function", sizeof("gigantic key which shall not be passed as an argument to any function") - 1, 
        "The quick brown fox jumps over the lazy dog", sizeof("The quick brown fox jumps over the lazy dog") - 1, res);
    sha1_print(res);
}

void test_phash_sha1() {
    #define RES_LENGTH 200
    char res[RES_LENGTH] = {0};
    phash_sha1("hello", 5, "world", 5, res, RES_LENGTH);
    
    for(int i = 0; i < RES_LENGTH; ++i) {
        if(i != 0) printf(", ");
        printf("%d", (unsigned char)res[i]);
    }
}

int main() {
    test_phash_sha1();

    return 0;
}