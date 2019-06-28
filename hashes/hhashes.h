#pragma once

void sha1_to_string(char in[20], char out[40]);
void sha1(const char* buffer, int length, char* out);
void sha1_print(char in[20]);

void md5(const char* buffer, int length, char* out);
void md5_print(char in[16]);
void test_md5();