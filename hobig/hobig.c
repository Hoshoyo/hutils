#include "hobig.h"

typedef int bool;
typedef unsigned char u8;
typedef unsigned long long int u64;
typedef long long int s64;
#define true 1
#define false 0

#include <stdarg.h>
#include <assert.h>
#include "../light_array.h"
#include "table.h"

#if defined(__linux__)
#include <time.h>
double os_time_us() {
    struct timespec t_spec;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_spec);
    u64 res = t_spec.tv_nsec + 1000000000 * t_spec.tv_sec;
    return (double)res / 1000.0;
}
#else
u64 random_integer(u64 min, u64 max) {
    return 1;
}
u64 random_64bit_integer() {
    return 1;
}

double os_time_us() {
    return 0.0;
}
#endif

typedef enum {
    TIME_SLOT_MULTIPLY,
    TIME_SLOT_DIVIDE,
    TIME_SLOT_ADD,
    TIME_SLOT_SUBTRACT,
    TIME_SLOT_COMPARE,
    TIME_SLOT_MOD_DIV,
} TimeSlot;
static double elapsed_times[64];
static int    execution_count[64];

#define COLLECT_TIMES 1

#if COLLECT_TIMES
#define TIME_COUNT() double time_count_start = os_time_us()
#define TIME_END(X) elapsed_times[X] += os_time_us() - time_count_start; execution_count[X]++
#else
#define TIME_COUNT()
#define TIME_END(X)
#endif

void
print_time_slots() {
    printf("Multiply:  %.2f ms, executed %d times\n", 0.001 * elapsed_times[TIME_SLOT_MULTIPLY], execution_count[TIME_SLOT_MULTIPLY]);
    printf("Divide:    %.2f ms, executed %d times\n", 0.001 * elapsed_times[TIME_SLOT_DIVIDE], execution_count[TIME_SLOT_DIVIDE]);
    printf("Add:       %.2f ms, executed %d times\n", 0.001 * elapsed_times[TIME_SLOT_ADD], execution_count[TIME_SLOT_ADD]);
    printf("Subtract:  %.2f ms, executed %d times\n", 0.001 * elapsed_times[TIME_SLOT_SUBTRACT], execution_count[TIME_SLOT_SUBTRACT]);
    printf("Compare:   %.2f ms, executed %d times\n", 0.001 * elapsed_times[TIME_SLOT_COMPARE], execution_count[TIME_SLOT_COMPARE]);
    printf("ModDivide: %.2f ms, executed %d times\n", 0.001 * elapsed_times[TIME_SLOT_MOD_DIV], execution_count[TIME_SLOT_MOD_DIV]);
}

HoBigInt 
hobig_int_new(u64 v) {
    HoBigInt result = { 0, array_new(u64) };
    array_push(result.value, v);
    return result;
}

HoBigInt
hobig_int_make(u64 n) {
    HoBigInt result = {0, array_new_len(u64, n) };
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

u64 bigendian_word(u64 v) {
    u64 r = 
        ((v & 0xff00000000000000) >> 56) |
        ((v & 0xff000000000000) >> 40) |
        ((v & 0xff0000000000) >> 24) |
        ((v & 0xff00000000) >> 8) |
        ((v & 0xff000000) << 8) |
        ((v & 0xff0000) << 24) |
        ((v & 0xff00) << 40) |
        ((v & 0xff) << 56)
    ;
    return r;
}

HoBigInt
hobig_int_new_from_memory(const char* m, int length) {
    HoBigInt result = {0};
    if(length == 0) return result;

    int arr_length = (length + sizeof(u64) - 1) / sizeof(u64);
    result.value = array_new_len(u64, arr_length);

    int block_count = length / sizeof(u64);
    int rest = length % sizeof(u64);

    int i = length;
	for (int k = 0; i >= sizeof(u64); k++) {
        result.value[k] = bigendian_word(*(u64*)&m[i-sizeof(u64)]);
		i -= sizeof(u64);
    }
    if (i > 0) {
		u64 d = 0;
		for (int s = 0; i > 0; s += 8) {
			d |= (u64)(m[i-1]) << s;
			i--;
		}
	    result.value[arr_length-1] = d;
	}

    array_length(result.value) = arr_length;

    return result;
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

    free(result);
    free(buffer);
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

// Compares two numbers considering sign
// Return value:
//  1 -> left is bigger
//  0 -> they are equal
// -1 -> right is bigger
int
hobig_int_compare_signed(HoBigInt* left, HoBigInt* right) {
    TIME_COUNT();
    // Check the sign first
    if(left->negative != right->negative) {
        if(left->negative) { TIME_END(TIME_SLOT_COMPARE); return -1; }
        TIME_END(TIME_SLOT_COMPARE);
        return 1;
    }

    // If both are negative, the biggest absolute will be
    // the lower value.
    int negative = (left->negative && right->negative) ? -1 : 1;

    size_t llen = array_length(left->value);
    size_t rlen = array_length(right->value);

    if(llen > rlen) { TIME_END(TIME_SLOT_COMPARE); return 1 * negative; }
    if(llen < rlen) { TIME_END(TIME_SLOT_COMPARE); return -1 * negative; }

    for(int i = llen - 1;; --i) {
        if(left->value[i] > right->value[i]) {
            TIME_END(TIME_SLOT_COMPARE);
            return 1 * negative;
        } else if (left->value[i] < right->value[i]) {
            TIME_END(TIME_SLOT_COMPARE);
            return -1 * negative;
        }
        if(i == 0) break;
    }
    
    TIME_END(TIME_SLOT_COMPARE);
    return 0;
}

// Compares the absolute value of two numbers, ignoring the sign
// Return value:
//  1 -> left is bigger
//  0 -> they are equal
// -1 -> right is bigger
int
hobig_int_compare_absolute(HoBigInt* left, HoBigInt* right) {
    TIME_COUNT();
    if(left->value == right->value) {
        TIME_END(TIME_SLOT_COMPARE);
        return 0;
    }

    size_t llen = array_length(left->value);
    size_t rlen = array_length(right->value);

    if(llen > rlen) { TIME_END(TIME_SLOT_COMPARE); return 1; }
    if(llen < rlen) { TIME_END(TIME_SLOT_COMPARE); return -1; }


    for(int i = llen - 1;; --i) {
        if(left->value[i] > right->value[i]) {
            TIME_END(TIME_SLOT_COMPARE);
            return 1;
        } else if (left->value[i] < right->value[i]) {
            TIME_END(TIME_SLOT_COMPARE);
            return -1;
        }
        if(i == 0) break;
    }

    TIME_END(TIME_SLOT_COMPARE);
    return 0;
}

void
hobig_int_sub(HoBigInt* dst, HoBigInt* src);

void 
hobig_int_add(HoBigInt* dst, HoBigInt* src) {
    TIME_COUNT();
    // Check to see if a subtraction is preferred
    if(dst->negative != src->negative) {
        // Subtract instead
        if(dst->negative) {
            // -x + y => -x -(-y)
            src->negative = 1;
            hobig_int_sub(dst, src);
            src->negative = 0;
        } else {
            // x + (-y) => x - y
            src->negative = 0;
            hobig_int_sub(dst, src);
            src->negative = 1;
        }
        TIME_END(TIME_SLOT_ADD);
        return;
    }

    HoBigInt s = {0};
    int free_source = 0;
    if(dst == src) {
        s = hobig_int_copy(*src);
        src = &s;
        free_source = 1;
    }

    // destination is at least the size of src or bigger
    if(array_length(dst->value) < array_length(src->value)) {
        size_t count = array_length(src->value) - array_length(dst->value);
        array_allocate(dst->value, count);
        memset(dst->value + array_length(dst->value), 0, count * sizeof(*dst->value));
        array_length(dst->value) = array_length(src->value);
    }
    u64 carry = 0;
    for(int i = 0; i < array_length(src->value); ++i) {
        u64 sum = dst->value[i] + src->value[i] + carry;
        if(sum < src->value[i] || 
            (sum == src->value[i] && dst->value[i] > 0 && src->value[i] > 0)) {
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
                hobig_int_add(dst, &big_one);
            }
        }
    }

    if(free_source) {
        hobig_free(*src);
    }

    TIME_END(TIME_SLOT_ADD);
}

void
hobig_int_sub(HoBigInt* dst, HoBigInt* src) {
    TIME_COUNT();
    int comparison = hobig_int_compare_absolute(dst, src);

    int dst_sign = dst->negative;
    int src_sign = src->negative;

    if(dst->negative != src->negative) {
        assert(comparison != 0);
        // if different sign and dst > src perform an absolute sum
        dst->negative = 0;
        src->negative = 0;
        
        hobig_int_add(dst, src);
        // final sign is going to be the destination sign, since its absolute
        // value is bigger.
        dst->negative = dst_sign;

        // restore src
        src->negative = src_sign;
        TIME_END(TIME_SLOT_SUBTRACT);
        return;
    }

    switch(comparison) {
        case 0: {
            // Result is 0
            dst->negative = 0;
            *dst->value = 0;
            array_length(dst->value) = 1;
        } break;
        case 1: {
            // dst > src
            u64 borrow = 0;
            for(int i = 0; i < array_length(src->value); ++i) {
                u64 start = dst->value[i];
                dst->value[i] -= borrow;
                dst->value[i] -= (src->value[i]);
                if(dst->value[i] > start 
                    || (dst->value[i] == start && dst->value[i] > 0 && src->value[i] > 0)) {
                    borrow = 1;
                } else {
                    borrow = 0;
                }
            }
            if(borrow) {
                dst->value[array_length(src->value)] -= 1;
            }
        } break;
        case -1: {
            // dst < src
            dst->negative = (dst->negative) ? 0 : 1;
            u64 borrow = 0;
            for(int i = 0; i < array_length(src->value); ++i) {
                u64 start = src->value[i];
                dst->value[i] = src->value[i] - borrow - dst->value[i];
                if(dst->value[i] > start) {
                    borrow = 1;
                } else {
                    borrow = 0;
                }
            }
            assert(borrow == 0);
        } break;
        default: assert(0); break;
    }
    // Reduce the array size of destination if it is the case
    size_t dst_length = array_length(dst->value);
    size_t reduction = 0;
    for(size_t i = dst_length - 1;;--i) {
        if(dst->value[i] == 0)
            reduction++;
        else
            break;
        if(i == 0) break;
    }
    array_length(dst->value) -= reduction;

    TIME_END(TIME_SLOT_SUBTRACT);
}

static void 
hobig_int_mul_pow10(HoBigInt* start, int p) {
    if(p == 0) {
        return;
    }
    // 8x + 2x
    for(int i = 0; i < p; ++i) {
        HoBigInt copy2 = hobig_int_copy(*start);

        multiply_by_pow2(start, 3);     // multiply by 8
        multiply_by_pow2(&copy2, 1);    // multiply by 2

        hobig_int_add(start, &copy2);   // sum the result

        array_free(copy2.value);        // free temporary
    }
}

// Multiplication works by multiplying the powers of 2 i.e.: for 777
// 777 * x => 512 * x + 256 * x + 8 * x + 1 * x
void 
hobig_int_mul(HoBigInt* dst, HoBigInt* src) {
    TIME_COUNT();
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
                hobig_int_add(dst, &dst_copy);
            }
            multiply_by_pow2(&dst_copy, 1);
        }
    }
    hobig_free(dst_copy);

    if(free_source) {
        hobig_free(*src);
    }

    TIME_END(TIME_SLOT_MULTIPLY);
}

HoBigInt 
hobig_int_new_decimal(const char* number, unsigned int* error) {
    HoBigInt result = {0};
    if(error) *error = 0;

    int len = strlen(number);
    if(len == 0) {
        if(error) *error = 1;
        return result;
    }

    int sign = 0;
    int index = 0;
    if(number[index] == '-') {
        sign = 1;
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
        hobig_int_add(&result, &pow10val);

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

    result.negative = sign; // do it only now, to leave the sums positive

    return result;
}

HoBigInt_DivResult
hobig_int_div(HoBigInt* dividend, HoBigInt* divisor) {
    TIME_COUNT();
    HoBigInt_DivResult result = { 0 };

    if(*divisor->value == 0) {
        // Division by 0
        assert(0);
    }

    int comparison = hobig_int_compare_absolute(dividend, divisor);

    if(comparison == -1) {
        // Dividend is smaller than divisor, quotient = 0 and remainder = dividend
        result.quotient = hobig_int_new(0);
        result.remainder = hobig_int_copy(*dividend);
    } else if(comparison == 0) {
        // Both numbers are equal, quotient is 1 and remainder is 0
        result.quotient = hobig_int_new(1);
        result.remainder = hobig_int_new(0);
    } else {
        // Perform long division since dividend > divisor
        // 100101010 | 111101
        HoBigInt remainder = hobig_int_new(0);
        HoBigInt quotient = hobig_int_new(0);
        HoBigInt one = hobig_int_new(1);

        for(int k = array_length(dividend->value) - 1 ;; --k) {
            u64 v = dividend->value[k];
            for(int i = sizeof(*dividend->value) * 8 - 1; i >= 0; --i) {
                multiply_by_pow2(&quotient, 1);
                multiply_by_pow2(&remainder, 1);
                int bit = (v >> i) & 1;
                if(bit) {
                    hobig_int_add(&remainder, &one);
                }
                int comparison = hobig_int_compare_absolute(&remainder, divisor);
                if(comparison == 1) {
                    // Ready to divide, quotient receives one
                    // and divisor is subtracted from remainder 
                    hobig_int_add(&quotient, &one);
                    hobig_int_sub(&remainder, divisor);
                } else if(comparison == 0) {
                    // Division is 1 and remainder 0
                    *remainder.value = 0;
                    array_length(remainder.value) = 1;
                    hobig_int_add(&quotient, &one);
                } else {
                    // Still not ready to divide
                    // Put a zero in the quotient
                }
            }
            if(k == 0) break;
        }
        result.quotient = quotient;
        result.remainder = remainder;
        hobig_free(one);
    }

    TIME_END(TIME_SLOT_DIVIDE);
    return result;
}

// Returns the number of leading zeros
static int 
hobig_int_leading_zeros_count(HoBigInt n) {
    u64 v = n.value[array_length(n.value) - 1];

    int c = 64;
    for(int i = 0; i < 64; ++i, --c) {
        if(v == 0) break;
        v >>= 1;
    }

    return c;
}

// Same as dividing by 2^shift_amt
void 
hobig_int_shr(HoBigInt* v, int shift_amt) {
    int opposite = 64 - shift_amt;
    u64 mask = (0xffffffffffffffff << opposite);
    v->value[0] >>= shift_amt;

    for(u64 i = 1; i < array_length(v->value); ++i) {
        u64 current = v->value[i];
        v->value[i] >>= shift_amt;
        v->value[i - 1] |= (mask & (current << opposite));
    }
}

// Same as multiplying by 2^shift_amt
void 
hobig_int_shl(HoBigInt* v, int shift_amt) {
    int opposite = 64 - shift_amt;
    u64 mask = (0xffffffffffffffff >> opposite);
    u64 prev = 0;
    for(u64 i = 0; i < array_length(v->value); ++i) {
        u64 current = v->value[i];
        v->value[i] <<= shift_amt;
        v->value[i] |= prev;
        prev = (current >> opposite) & mask;
    }
}

HoBigInt
hobig_int_mod_div(HoBigInt* n, HoBigInt* exp, HoBigInt* m) {
    TIME_COUNT();
    // Holds the final sum of mods of the powers of 2
    // i.e. 5^117 mod 19
    // final = (5^1 mod 19) * (5^4 mod 19) * (5^16 mod 19) * (5^32 mod 19) * (5^64 mod 19) = 61200
    HoBigInt final = hobig_int_new(1);

    // Calculate the first modulo n^1 mod m
    HoBigInt_DivResult powers = hobig_int_div(n, m);

    // For each power of 2 execute
    for(int k = 0; k < array_length(exp->value); ++k) {
        u64 v = exp->value[k];
        for(int i = 0; i < sizeof(*exp->value) * 8; ++i) {
            int bit = (v >> i) & 1;

            if(bit) {
                // Sum the power of 2 result to the final 
                // result when the bit is set
                hobig_int_mul(&final, &powers.remainder);

				HoBigInt_DivResult accumulated = hobig_int_div(&final, m);
				hobig_free(final);
				hobig_free(accumulated.quotient);
				final = accumulated.remainder;

            }            
            hobig_int_mul(&powers.remainder, &powers.remainder);

            HoBigInt_DivResult ps = hobig_int_div(&powers.remainder, m);

            hobig_free(powers.quotient);
            hobig_free(powers.remainder);
            powers = ps;
        }
    }
    hobig_free(powers.quotient);
    hobig_free(powers.remainder);

    // Calculate the final modulo final mod m (in the example 61200 mod 19)
    HoBigInt_DivResult result = hobig_int_div(&final, m);
    hobig_free(result.quotient);
    hobig_free(final);

    TIME_END(TIME_SLOT_MOD_DIV);
    return result.remainder;
}

// Use the euclidean algorithm to calculate GCD(a, b) (Greatest common divisor).
HoBigInt
hobig_int_gcd(HoBigInt* a, HoBigInt* b) {
    if(array_length(a->value) == 1 && *a->value == 0) {
        return hobig_int_copy(*b);
    }
    if(array_length(b->value) == 1 && *b->value == 0) {
        return hobig_int_copy(*a);
    }
    HoBigInt_DivResult d = hobig_int_div(a, b);
    return hobig_int_gcd(b, &d.remainder);
}

int
hobig_int_bitcount(HoBigInt* v) {
    return (int)(array_length(v->value) * sizeof(*v->value) * 8);
}

// Compares if a is one less than b
static int
compare_if_one_less(HoBigInt* a, HoBigInt* b) {
    if(a->value[0] != b->value[0] - 1) {
        return 0;
    }

    if(array_length(a->value) != array_length(b->value))
        return 0;

    // Rest must be equal
    for(int i = 1; i < array_length(a->value); ++i) {
        if(a->value[i] != b->value[i])
            return 0;
    }
    return 1;
}

extern u64 random_integer(u64 min, u64 max);
extern u64 random_64bit_integer();

HoBigInt
hobig_random(HoBigInt* max) {
    HoBigInt r = hobig_int_copy(*max);
    u64 m = r.value[array_length(r.value) - 1];
    u64 r0 = random_integer(0, m);
    r.value[array_length(r.value) - 1] = r0;

    for(int i = 0; i < array_length(r.value) - 1; ++i) {
        r.value[0] = random_64bit_integer();
    }

    assert(hobig_int_compare_absolute(max, &r) == 1);
    return r;
}

// Receives n-1 (even number)
static HoBigInt
miller_rabin_pre(HoBigInt* n, int* out_power) {
    HoBigInt one = hobig_int_new(1);
    HoBigInt two = hobig_int_new(2);
    HoBigInt copy = hobig_int_copy(*n);
    int power = 0;

    while(!(copy.value[0] & 1)) {
        // TODO(psv): optimize division by 2
        HoBigInt_DivResult res = hobig_int_div(&copy, &two);
        hobig_free(copy);
        hobig_free(res.remainder);
        copy = res.quotient;
        power++;
    }

    hobig_free(one);
    hobig_free(two);

    *out_power = power;
    return copy;
}

// Must be odd number
int
miller_rabin_probably_prime(HoBigInt* in, int k) {
    HoBigInt one = hobig_int_new(1);
    HoBigInt two = hobig_int_new(2);
    HoBigInt original = hobig_int_copy(*in);

    // calculate n-1
    HoBigInt n_minus_one = hobig_int_copy(*in);
    hobig_int_sub(&n_minus_one, &one);

    int power = 0;
    HoBigInt d = miller_rabin_pre(&n_minus_one, &power);

    for(int i = 0; i < k; ++i) {
        // 1 < a < n - 1  (1 < a < 52 where n = 53)
        HoBigInt a = hobig_random(&n_minus_one);
        
        // Calculate b0 => a^m mod n
        HoBigInt x = hobig_int_mod_div(&a, &d, &original);

        // First test equal to 1
        if( hobig_int_compare_absolute(&x, &one) == 0 ||
            compare_if_one_less(&x, &original)) {
            continue;
        }

        int cont = 0;
        for(int r = 0; r < power; ++r) {
            x = hobig_int_mod_div(&x, &two, &original);
            if(compare_if_one_less(&x, &original)) {
                cont = true;
                break;
            }
        }
        if(!cont) return 0;
    }

    return 1;
}

int 
hobig_is_prime(HoBigInt* n, int k) {
    // if it is an even number
    if(!(n->value[0] & 1)) {
        return 0;
    }

    // Check trivial primes
    if(array_length(n->value) == 1) {
        if(*n->value < 2) return 0;
        if(*n->value == 2 || *n->value == 3 || *n->value == 5) return 1;
    }

    return miller_rabin_probably_prime(n, k);
}

HoBigInt
hobig_random_possible_prime(int bits) {
    HoBigInt product_small_primes = hobig_int_new(16294579238595022365ull);
    u64 small_primes[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53 };

    int chunk_count = bits / (sizeof(u64) * 8);

    HoBigInt result = {0};
    result.value = array_new(u64);
    array_allocate(result.value, chunk_count);
    array_length(result.value) = chunk_count;

    int its = 0;
    while(1) {
        for(int i = 0; i < chunk_count; ++i) {
            result.value[i] = random_64bit_integer();
        }
        
        *result.value |= 1; // make sure is odd

        HoBigInt_DivResult d = hobig_int_div(&result, &product_small_primes);
        u64 mod = *d.remainder.value;
        hobig_free(d.quotient);
        hobig_free(d.remainder);

        HoBigInt big_delta = hobig_int_new(0);
        for (u64 delta = 0; delta < 1<<20; delta += 2) {
            u64 m = mod + delta;
            int cont = 0;
            for (int k = 0; k < sizeof(small_primes) / sizeof(*small_primes); ++k) {
                if (m % small_primes[k] == 0 && (bits > 6 || m != small_primes[k])) {
                    cont = 1;
                    break;
                }
            }
            if(cont) continue;

            if (delta > 0) {
                *big_delta.value = delta;
                hobig_int_add(&result, &big_delta);
            }
            break;
        }

        if(miller_rabin_probably_prime(&result, 20)) {
            break;
        }
    }

    return result;
}


HoBigInt
hobig_int_mod_div_fast(HoBigInt* n, HoBigInt* exp, HoBigInt* m) {
    HoBigInt answer = hobig_int_new(1);
    HoBigInt two = hobig_int_new(2);

    HoBigInt_DivResult r = hobig_int_div(n, m);
    HoBigInt base = r.remainder;
    hobig_free(r.quotient);

    HoBigInt e = hobig_int_copy(*exp);

    int i = 0;
	while (e.value[0] > 0) {
		if ((e.value[0] & 1) == 1) {
            hobig_int_mul(&answer, &base);
            HoBigInt_DivResult r = hobig_int_div(&answer, m);
            hobig_free(answer);
            hobig_free(r.quotient);
			answer = r.remainder;
		}

        // TODO(psv): better div by 2
        HoBigInt_DivResult dr = hobig_int_div(&e, &two);
        hobig_free(e);
        hobig_free(dr.remainder);
        e = dr.quotient;

        hobig_int_mul(&base, &base);

        HoBigInt_DivResult bb = hobig_int_div(&base, m);
        hobig_free(bb.quotient);
        hobig_free(base);
        base = bb.remainder;

        //printf("%d\n", i++);
	}

    return answer;
}