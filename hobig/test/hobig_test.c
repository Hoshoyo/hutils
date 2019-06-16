#include <stdio.h>
#include "../hobig.c"


int main() {
    unsigned int error = 0;
    HoBigInt n1 = hobig_new_dec("1787687534840581043851512481241209348120984", 0);
    HoBigInt n2 = hobig_new_dec("1787687534840581043851512481241209348120982", &error);

    hobig_int_mul(&n1, &n2);

    hobig_int_print(n1);

    hobig_free(n1);
    hobig_free(n2);

    return 0;
}