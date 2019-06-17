#include <stdio.h>
#include "../hobig.c"


void test_hobig_mod_div() {
    // Result = 1
    // HoBigInt n   = hobig_int_new(5);
    // HoBigInt pow = hobig_int_new(117);
    // HoBigInt mod = hobig_int_new(19);

    HoBigInt n   = hobig_new_dec("8197459871094878951798740923874981", 0);
    HoBigInt pow = hobig_new_dec("4198783950798472098372176371283", 0);
    HoBigInt mod = hobig_new_dec("23098419208371261834769749217491", 0);

    HoBigInt r = hobig_int_mod_div(&n, &pow, &mod);

    hobig_int_print(r);
    printf("\n");

    hobig_free(n);
    hobig_free(pow);
    hobig_free(mod);
    hobig_free(r);

}

int main() {
    test_hobig_mod_div();

    return 0;
}