#pragma once

typedef struct {
    int                negative;
    unsigned long long int* value;    // dynamic light_array
} HoBigInt;

// Result structs
typedef struct {
    HoBigInt quotient;
    HoBigInt remainder;
} HoBigInt_DivResult;

// Create and destroy
HoBigInt hobig_int_new(unsigned long long int v);
void     hobig_free(HoBigInt v);
HoBigInt hobig_int_copy(HoBigInt v);
HoBigInt hobig_int_new_from_memory(const char* m, int length);
HoBigInt hobig_int_new_decimal(const char* number, unsigned int* error);

// Comparison
int hobig_int_compare_signed(HoBigInt* left, HoBigInt* right);
int hobig_int_compare_absolute(HoBigInt* left, HoBigInt* right);

// Arithmetic
void               hobig_int_add(HoBigInt* dst, HoBigInt* src);
void               hobig_int_sub(HoBigInt* dst, HoBigInt* src);
void               hobig_int_mul(HoBigInt* dst, HoBigInt* src);
HoBigInt_DivResult hobig_int_div(HoBigInt* dividend, HoBigInt* divisor);
HoBigInt           hobig_int_mod_div(HoBigInt* n, HoBigInt* exp, HoBigInt* m);
HoBigInt           hobig_int_gcd(HoBigInt* a, HoBigInt* b);

// Misc
int hobig_int_bitcount(HoBigInt* v);

// Print
void hobig_int_print(HoBigInt n);