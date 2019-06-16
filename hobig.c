typedef int bool;
typedef unsigned char u8;
typedef unsigned long long int u64;
typedef long long int s64;
#define true 1
#define false 0

#include "light_array.h"
#include "table.h"

#define printf(...) printf(__VA_ARGS__); fflush(stdout)

typedef struct {
    int  negative;
    u64* value;    // dynamic light array
} HoBigInt;

HoBigInt 
hobig_int_new(s64 v) {
    HoBigInt result = { 0, array_new(u64) };
    if(v < 0) {
        result.negative = 1;
        array_push(result.value, -v);
    } else {
        array_push(result.value, v);
    }
    return result;
}

void
hobig_free(HoBigInt v) {
    if(v.value) array_free(v.value);
}

static void 
print_number(unsigned char* num, int length) {
    bool seen_number = false;
    for(int i = length-1; i >= 0; --i) {
        if(num[i] == 0 && !seen_number)
            continue;
        else
            seen_number = true;
        printf("%d", num[i]);
    }
    if(!seen_number) printf("0");
}

// a will be the destination
static void 
big_dec_sum(u8* dst, u8* src, int length) {
    u8 carry = 0;
    for(int i = 0; i < length; ++i) {
        u8 a = dst[i];
        u8 b = src[i];
        dst[i] = mult_table[a][b][carry][0]; // this is the result
        carry = mult_table[a][b][carry][1];
    }
}

void 
hobig_int_print(HoBigInt n) {
    if(!n.value || array_length(n.value) == 0){
        printf("0");
        return;
    }

    if(n.negative) {
        printf("-");
    }
    size_t length = (array_length(n.value) + 1) * 64;

    u8* result = calloc(length, 1); // The result is at least the size of the binary number + 1
    u8* buffer = calloc(length, 1); // Buffer that will hold powers of 2 in decimal
    buffer[0] = 1;

    for(int k = 0; k < array_length(n.value); k++) {
        u64 v = n.value[k];
        for(int i = 0; i < sizeof(*n.value) * 8; ++i) {
            int bit = (v >> i) & 1;
            if(bit) {
                big_dec_sum(result, buffer, length);
            }
            big_dec_sum(buffer, buffer, length);
        }
    }

    print_number(result, length);
}

static void 
big_gen_pow2(int p, u8* buffer, int length) {
    if(length == 0) return;
    buffer[0] = 1;
    for(int i = 0; i < p; ++i) {
        u8 carry = 0;
        for(int i = 0; i < length-1; ++i) {
            u8 a = buffer[i];
            u8 b = buffer[i];
            buffer[i] = mult_table[a][b][carry][0]; // this is the result
            carry = mult_table[a][b][carry][1];
        }
    }
}

HoBigInt
hobig_int_copy(HoBigInt v) {
    HoBigInt result = {0};
    result.negative = v.negative;
    result.value = array_copy(v.value);
    return result;
}

static void
multiply_by_pow2(HoBigInt* n, int power) {
    int word_size_bytes = sizeof(*n->value);
    int word_size_bits = word_size_bytes * 8;
    int word_shift_count = power / word_size_bits;
    int shift_amount = (power % word_size_bits);

    u64 s = 0;
    for(int i = 0; i < array_length(n->value); ++i) {
        u64 d = n->value[i] >> (word_size_bits - shift_amount);
        n->value[i] <<= shift_amount;
        n->value[i] |= s;
        s = d;
    }
    if(s) {
        // grow array
        array_push(n->value, s);
    }

    if(word_shift_count) {
        // insert zeros at the beggining
        array_allocate(n->value, word_shift_count);
        memcpy(n->value + array_length(n->value), n->value, array_length(n->value) * word_size_bytes);
        memset(n->value, 0, word_shift_count * word_size_bytes);
        array_length(n->value) += word_shift_count;
    }
}

void 
hobig_int_sum(HoBigInt* dst, HoBigInt* src) {
    HoBigInt s = {0};
    int free_source = 0;
    if(dst == src) {
        s = hobig_int_copy(*src);
        src = &s;
        free_source = 1;
    }

    // destination is at least the size of src or bigger
    if(array_length(dst->value) < array_length(src->value)) {
        array_allocate(dst->value, array_length(src->value) - array_length(dst->value));
        array_length(dst->value) = array_length(src->value);
    }
    u64 carry = 0;
    for(int i = 0; i < array_length(src->value); ++i) {
        u64 sum = dst->value[i] + src->value[i] + carry;
        if(sum < src->value[i]) {
            carry = 1;
        } else {
            carry = 0;
        }
        dst->value[i] = sum;
    }
    if(carry) {
        if(array_length(src->value) == array_length(dst->value)) {
            // grow destination
            array_push(dst->value, 1);
        } else {
            // destination is bigger, so sum 1 to it
            if(dst->value[array_length(src->value)] != (u64)-1) {
                dst->value[array_length(src->value)] += 1;
            } else {
                HoBigInt big_one = hobig_int_new(1);
                hobig_int_sum(dst, &big_one);
            }
        }
    }

    if(free_source) {
        hobig_free(*src);
    }
}

static void 
print_array(u64* a) {
    printf("length:   %lu\n", array_length(a));
    printf("capacity: %lu\n", array_capacity(a));
    printf("data:     %p", a);
    printf("{ ");
    for(size_t i = 0; i < array_length(a); ++i) {
        if(i != 0) printf(", ");
        printf("%lld", a[i]);
    }
    printf(" }\n");
}

void 
hobig_int_mul_pow10(HoBigInt* start, int p) {
    if(p == 0) {
        return;
    }
    // 8x + 2x
    for(int i = 0; i < p; ++i) {
        HoBigInt copy2 = hobig_int_copy(*start);

        multiply_by_pow2(start, 3);     // multiply by 8
        multiply_by_pow2(&copy2, 1);    // multiply by 2

        hobig_int_sum(start, &copy2);   // sum the result

        array_free(copy2.value);        // free temporary
    }
}

// Multiplication works by multiplying the powers of 2 i.e.: for 777
// 777 * x => 512 * x + 256 * x + 8 * x + 1 * x
void 
hobig_int_mul(HoBigInt* dst, HoBigInt* src) {
    int free_source = 0;
    HoBigInt s = {0};
    if(dst == src) {
        s = hobig_int_copy(*src);
        src = &s;
        free_source = 1;
    }

    dst->negative = (dst->negative ^ src->negative);

    int word_size = sizeof(u64) * 8;
    HoBigInt dst_copy = hobig_int_copy(*dst);
    memset(dst->value, 0, array_length(dst->value) * sizeof(*dst->value));
    
    for(int k = 0; k < array_length(src->value); k++) {
        u64 v = src->value[k];
        for(int i = 0; i < word_size; ++i) {
            int bit = (v >> i) & 1;
            if(bit) {
                hobig_int_sum(dst, &dst_copy);
            }
            multiply_by_pow2(&dst_copy, 1);
        }
    }

    if(free_source) {
        hobig_free(*src);
    }
}

HoBigInt 
hobig_new_dec(const char* number, unsigned int* error) {
    HoBigInt result = {0};
    if(error) *error = 0;

    int len = strlen(number);
    if(len == 0) {
        if(error) *error = 1;
        return result;
    }

    int index = 0;
    if(number[index] == '-') {
        result.negative = 1;
        index++;
    }

    int first = number[len - 1] - 0x30;
    if(first < 0 || first > 9) {
        if(error) *error = 1;
        return result;
    }
    result.value = array_new(u64);
    array_push(result.value, first);

    // All digits to be used in multiplication
    HoBigInt digits[10] = { 0 };
    for(int i = 1; i < 10; ++i) {
        digits[i] = hobig_int_new(i);
    }

    // Powers of ten that will be used for every step
    HoBigInt powers_of_ten = hobig_int_new(1);

    for(int i = len - 2; i >= index; --i) {
        int n = number[i] - 0x30;
        if(n < 0 || n > 9) {
            // Not a decimal number
            if(error) *error = 1;
            break;
        }

        // Calculate n * 10^power
        // Start at 10^1

        // Generate the power of 10
        hobig_int_mul_pow10(&powers_of_ten, 1);

        // When the digit is 0, we still advance the powers of 10
        // but we do not attempt to sum up anything to the number
        if(n == 0) continue;

        // Grab a copy to be used to multiply by the digit value
        HoBigInt pow10val = hobig_int_copy(powers_of_ten);
        // Multiply by the digit value n
        hobig_int_mul(&pow10val, &digits[n]);

        // Sum it back to the final result
        hobig_int_sum(&result, &pow10val);

        // Free temporary
        hobig_free(pow10val);
    }

    // Free digits
    for(int i = 1; i < 10; ++i) {
        hobig_free(digits[i]);
    }
    // Free suppor power of 10
    hobig_free(powers_of_ten);

    if(error && *error) {
        hobig_free(result);
    }

    return result;
}