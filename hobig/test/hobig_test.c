#include <stdio.h>
#include "../hobig.c"


int main() {
    #if 0
    unsigned int error = 0;
    HoBigInt n1 = hobig_new_dec("1787687534840581043851512481241209348120984", 0);
    HoBigInt n2 = hobig_new_dec("504139859013457891357189758912742131231131", &error);

    hobig_int_mul(&n1, &n2);

    hobig_int_print(n1);

    hobig_free(n1);
    hobig_free(n2);
    #endif

    HoBigInt a = hobig_new_dec("789219851475734513498857134758719438", 0);
    HoBigInt b = hobig_new_dec("17801401298091820983", 0);
    //HoBigInt a = hobig_new_dec("21905251264990169654", 0);
    //HoBigInt b = hobig_new_dec("17801401298091820983", 0);

    //hobig_int_sub(&a, &b);

    //hobig_int_print(a);

    HoBigInt_DivResult r = hobig_int_div(&a, &b);
    hobig_int_print(r.quotient);
    printf(", ");
    hobig_int_print(r.remainder);
    printf("\n");

    return 0;
}