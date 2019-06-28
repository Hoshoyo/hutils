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
hmac(
    void(*hash_function)(char*, int, char*),
    char* key, int key_length, char* message, int message_length, char* result, int result_length) 
{
    char temp_key[HMAC_BLOCK_SIZE] = {0};
    char o_key_pad[HMAC_BLOCK_SIZE] = {0};
    char i_key_pad[HMAC_BLOCK_SIZE] = {0};

    if(key_length > HMAC_BLOCK_SIZE) {
        hash_function(key, key_length, temp_key);
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

    char h[512] = {0}; // enough space for all types of hashes
    hash_function(m, message_length + HMAC_BLOCK_SIZE, h);

    memcpy(m, o_key_pad, HMAC_BLOCK_SIZE);
    memcpy(m + HMAC_BLOCK_SIZE, h, result_length);

    hash_function(m, HMAC_BLOCK_SIZE + result_length, result);
    free(m);
}

// prf12 implements the TLS 1.2 pseudo-random function, as defined in RFC 5246, Section 5.
void prf12(void(*hash_function)(char*, int, char*), 
    int hash_result_length,
    const char* secret, int secret_length,
    const char* label, int label_length,
    const char* seed, int seed_length,
    char* result, int result_length) 
{
    int label_and_seed_length = label_length + seed_length;
    char* label_and_seed = calloc(1, label_and_seed_length);

    memcpy(label_and_seed, label, label_length);
    memcpy(label_and_seed + label_length, seed, seed_length);

    phash(hash_function, hash_result_length, secret, secret_length, seed, seed_length, result, result_length);
}

void prf10(
    const char* secret, int secret_length, 
    const char* label, int label_length, 
    const char* seed, int seed_length,
    char* result, int result_length) 
{
    const int MD5_RESULT_LENGTH = 16;
    const int SHA1_RESULT_LENGTH = 20;

    int label_and_seed_length = label_length + seed_length;
    char* label_and_seed = calloc(1, label_and_seed_length);
    memcpy(label_and_seed, label, label_length);
    memcpy(label_and_seed + label_length, seed, seed_length);

    int s1_len = (secret_length + 1) / 2;
    int s2_len = secret_length - (secret_length / 2);
    char* s1 = calloc(1, s1_len);
	char* s2 = calloc(1, s2_len);

    memcpy(s1, secret, s1_len);
    memcpy(s2, secret + s1_len, s2_len);

    phash(md5, MD5_RESULT_LENGTH, s1,  s1_len, label_and_seed, label_and_seed_length, result, result_length);

    char* res_temp = calloc(1, result_length);
	phash(sha1, SHA1_RESULT_LENGTH, s2, s2_len, label_and_seed, label_and_seed_length, res_temp, result_length);

	for (int i = 0; i < result_length; ++i) {
        result[i] ^= res_temp[i];
	}

    free(s1);
    free(s2);
    free(label_and_seed);
    free(res_temp);
}

void phash(
    void(*hash_function)(char*, int, char*), int hash_result_length_bytes,
    char* secret, int secret_length, char* seed, int seed_length, char* result, int result_length_bytes) 
{
    int length = result_length_bytes;
    char A[512] = {0};
    char T[512] = {0};

    // Calculate A(1) = hmac(secret, A(0))
    hmac(hash_function, secret, secret_length, seed, seed_length, A, hash_result_length_bytes);

    if(length == 0) return;
    char* temp = calloc(1, hash_result_length_bytes + seed_length);
    memcpy(temp + hash_result_length_bytes, seed, secret_length);

    int offset = 0;
    while(length > 0) {
        // Next A
        memcpy(temp, A, hash_result_length_bytes);

        hmac(hash_function, secret, secret_length, temp, hash_result_length_bytes + seed_length, T, hash_result_length_bytes);
        int a = MIN(length, hash_result_length_bytes);
        memcpy(result + offset, T, a);
        length -= a;
        offset += a;

        hmac(hash_function, secret, secret_length, A, hash_result_length_bytes, A, hash_result_length_bytes);
    }

    free(temp);
}

void test_phash() {
    #define RES_LENGTH 200
    char res[RES_LENGTH] = {0};
    phash(sha1, 20, "hello", 5, "world", 5, res, RES_LENGTH);
    
    for(int i = 0; i < RES_LENGTH; ++i) {
        if(i != 0) printf(", ");
        printf("%d", (unsigned char)res[i]);
    }
}

void test_sha1() {
    char res[20] = {0};
    char in[256] = {0};
    for(int j = 0; j < 256; ++j) {
        in[j] = j;
    }
    
    for(int i = 0; i < 256; ++i) {
        sha1(in, i, res);
        sha1_print(res);
        printf("\n");
    }
}

void test_hmac() {
    char res[20] = {0};
    hmac(sha1, "", 0, "", 0, res, 20);
    sha1_print(res);
    printf("\n");
    hmac(
        sha1,
        "gigantic key which shall not be passed as an argument to any function", sizeof("gigantic key which shall not be passed as an argument to any function") - 1, 
        "The quick brown fox jumps over the lazy dog", sizeof("The quick brown fox jumps over the lazy dog") - 1, res, 20);
    sha1_print(res);
    printf("\n");
    
    hmac(md5, "", 0, "", 0, res, 16);
    md5_print(res);
    printf("\n");

    hmac(md5,
    "gigantic key which shall not be passed as an argument to any function", sizeof("gigantic key which shall not be passed as an argument to any function") - 1, 
        "The quick brown fox jumps over the lazy dog", sizeof("The quick brown fox jumps over the lazy dog") - 1, res, 16);
    md5_print(res);
    printf("\n");
}

int main() {
    //test_phash();
    test_hmac();

    //char res[16] = {0};
    //md5("gigantic key which shall not be passed as an argument to any function", sizeof("gigantic key which shall not be passed as an argument to any function") - 1, res);
    //md5_print(res);

    return 0;
}