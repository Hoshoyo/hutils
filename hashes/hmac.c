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

void
hmac_sha1(char* key, int key_length, char* message, int message_length, char result[20]) {
    char temp_key[HMAC_BLOCK_SIZE] = {0};
    char o_key_pad[HMAC_BLOCK_SIZE] = {0};
    char i_key_pad[HMAC_BLOCK_SIZE] = {0};

    if(key_length > HMAC_BLOCK_SIZE) {
        // TODO
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

int main() {
    char res[20] = {0};
    test_sha1();
    //hmac_sha1("", 0, "", 0, res);
    sha1_print(res);
    return 0;
}