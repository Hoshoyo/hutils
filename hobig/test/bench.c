#include <stdio.h>
#include <string.h>
#include "../hobig.c"

void test_divide_benchmark() {
    HoBigInt v = hobig_new_dec("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 0);
    HoBigInt d = hobig_new_dec("97129831982739812798798718975981279857289", 0);

    double start = os_time_us();
    
    hobig_int_div(&v, &d);

    hobig_int_print(v);
    printf("\n");
    printf("%.4f\n", 0.001 * (os_time_us() - start));
}

void test_multiply_benchmark() {
    HoBigInt v = hobig_new_dec("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 0);
    HoBigInt d = hobig_new_dec("3230291580290704972938712983791287012085289572097481203112093812908390218093890218903120", 0);

    double start = os_time_us();
    
    for(int k = 0; k < 1000; ++k) {
        v = hobig_new_dec("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 0);
        d = hobig_new_dec("3230291580290704972938712983791287012085289572097481203112093812908390218093890218903120", 0);
        for(int i = 0; i < 16; ++i) {
            hobig_int_mul(&v, &d);
        }
    }

    printf("%.4f\n", 0.001 * (os_time_us() - start));
}

void test_addition_benchmark() {
    HoBigInt v = hobig_new_dec("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 0);
    HoBigInt d = hobig_new_dec("3230291580290704972938712983791287012085289572097481203112093812908390218093890218903120", 0);

    double start = os_time_us();
    for(int j = 0; j < 100; ++j) {
        for(int i = 0; i < 1000000; ++i) {
            hobig_int_add(&v, &d);
        }
    }
    printf("%.4f\n", 0.001 * (os_time_us() - start));
}

void test_subtraction_benchmark() {
    HoBigInt v = hobig_new_dec("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 0);
    HoBigInt d = hobig_new_dec("3230291580290704972938712983791287012085289572097481203112093812908390218093890218903120", 0);

    double start = os_time_us();
    for(int j = 0; j < 100; ++j) {
        for(int i = 0; i < 1000000; ++i) {
            hobig_int_sub(&v, &d);
        }
    }
    printf("%.4f\n", 0.001 * (os_time_us() - start));
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("usage: %s <benchmark name>\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "sum") == 0) {
        test_addition_benchmark();
    } else if(strcmp(argv[1], "sub") == 0) {
        test_subtraction_benchmark();
    } else if(strcmp(argv[1], "mul") == 0) {
        test_multiply_benchmark();
    } else if(strcmp(argv[1], "div") == 0) {
        test_divide_benchmark();
    } else {
        printf("Invalid benchmark name");
    }

    return 0;
}